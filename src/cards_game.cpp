#include "cards_game.h"
#include "gmap.cpp"
#include "rendering.cpp"
#include "skeletal_animation.cpp"
#include "keycontroller.cpp"
#include "ui.cpp"
#include "sound.cpp"
#include "collision.cpp"
#if GLITCH_DEBUG
#include "debug.cpp"
#endif

platform_api Platform;

static entity* AddEntity(game_state* GameState, renderer& Renderer, EType Type, math::v3 P = math::v3(), math::quat O = math::quat(),  math::v3 S = math::v3(1.0f))
{
    auto* Entity = &GameState->Entities[GameState->EntityCount++];
    Entity->Position = P;
    Entity->Orientation = O;
    Entity->Scale = S;
    Entity->Type = Type;
    
    return Entity;
}

extern "C" UPDATE(Update)
{
    Platform = GameMemory->PlatformAPI;
    
#if GLITCH_DEBUG
    debug_state* DebugState = GameMemory->DebugState;
#endif
    
    game_state* GameState = GameMemory->GameState;
    
    if(!GameState)
    {
        GameState = GameMemory->GameState = BootstrapPushStruct(game_state, TotalArena);
        GameState->IsInitialized = false;
    }
    
    Assert(GameState);
    
    if(!GameState->IsInitialized || !GameMemory->IsInitialized)
    {
        auto* Player = AddEntity(GameState, Renderer, EType_Player);
        Player->Player.Speed = 10.0f;
        Renderer.CurrentCameraHandle = 0;
        Renderer.ClearColor = math::rgba(0.3f, 0.3f, 0.3f, 1.0f);
        Renderer.Cameras[Renderer.CurrentCameraHandle].Zoom = 20.0f;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportHeight = Renderer.WindowHeight;
        Renderer.Cameras[Renderer.CurrentCameraHandle].Position = math::v3(32.5f, 15, 0);
        
        GameState->Grid = math::v2(16, 8);
        GameState->GridScale = 4.0f;
        
        sounds Sounds = {};
        //@Incomplete: Get actual sizes, this is retarded
        memcpy(&GameState->Sounds.SoundEffects, SoundEffects, sizeof(sound_effect) * (64 + 32));
        
        LoadTextures(Renderer, &Renderer.TextureArena, Concat(CARDS_ASSETS, "textures/"));
        
        GameState->IsInitialized = true;
        GameMemory->IsInitialized = true;
    }
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    
    auto Near = -100.0f;
    auto Far = 1000.0f;
    
    CameraTransform(Renderer, Camera, Camera.Position, Camera.Orientation, Camera.Target, Camera.Zoom, Near, Far);
    
    DisableDepthTest(Renderer);
    
    for(i32 I = 0; I < GameState->Grid.x; I++)
    {
        for(i32 J = 0; J < GameState->Grid.y; J++)
        {
            PushFilledQuad(Renderer, math::v3(I * GameState->GridScale, J * GameState->GridScale, 0), math::v3(GameState->GridScale, GameState->GridScale, 1.0f), math::v3(), math::rgba(1.0f, 0.0f, 0.0f, 0.4f), "card_ace_1", false);
            PushOutlinedQuad(Renderer, math::v3(I * GameState->GridScale, J * GameState->GridScale, 0), math::v3(GameState->GridScale, GameState->GridScale, 1.0f), math::v3(), math::rgba(1.0f, 1.0f, 1.0f, 1.0f), false);
        }
    } 
    
    FOR_ENT(Index)
    {
        auto* Entity = &GameState->Entities[Index];
        if(auto* Player = GET_ENT(Entity, Player))
        {
            auto V = math::v3();
            if(KEY(Key_W))
            {
                V.y = 1.0f;
            }
            else if(KEY(Key_S))
            {
                V.y = -1.0f;
            }
            
            if(KEY(Key_A))
            {
                V.x = -1.0f;
            }
            else if(KEY(Key_D))
            {
                V.x = 1.0f;
            }
            
            Entity->Velocity = V;
            
            Entity->Position += V * (r32)DeltaTime * Player->Speed;
            
            PushFilledQuad(Renderer, Entity->Position, Entity->Scale, math::v3(), math::rgba(1.0f, 0.0f, 0.0f, 1.0f), 0, false);
        }
    }
    
    EnableDepthTest(Renderer);
    
    if(KEY_DOWN(Key_Escape))
    {
        GameMemory->ExitGame = true;
    }
    
    
    Renderer.ShowMouseCursor = true;
    
    GameState->PrevMouseX = (r32)InputController->MouseX;
    GameState->PrevMouseY = (r32)InputController->MouseY;
}


