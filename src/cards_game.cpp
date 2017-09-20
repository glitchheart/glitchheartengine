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

static entity* AddCube(game_state* GameState, renderer& Renderer, Entity_Type Type, math::v3 P = math::v3(), math::quat O = math::quat(), math::v3 S = math::v3(1.0f))
{
    if(!GameState->CubeInitialized)
    {
        LoadGLIMModel(Renderer, "../assets/models/cube.glim", &GameState->Cube);
        GameState->CubeInitialized = true;
    }
    
    auto* Entity = &GameState->Entities[GameState->EntityCount++];
    Entity->Position = P;
    Entity->Orientation = O;
    Entity->Scale = S;
    Entity->Type = Type;
    Entity->Model = GameState->Cube;
    
    return Entity;
}

static entity* AddEntity(game_state* GameState, renderer& Renderer, Entity_Type Type, math::v3 P = math::v3(), math::quat O = math::quat(),  math::v3 S = math::v3(1.0f))
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
        //auto* Player = AddCube(GameState, Renderer, Entity_Player);
        auto* Player = AddEntity(GameState, Renderer, Entity_Player);
        Renderer.CurrentCameraHandle = 0;
        Renderer.ClearColor = math::rgba(0.1f, 0.1f, 0.1f, 1.0f);
        Renderer.Cameras[Renderer.CurrentCameraHandle].Zoom = 20.0f;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportHeight = Renderer.WindowHeight;
        Renderer.Cameras[Renderer.CurrentCameraHandle];
        
        //GameState->EntityCount = 1;
        
        sounds Sounds = {};
        //@Incomplete: Get actual sizes, this is retarded
        memcpy(&GameState->Sounds.SoundEffects, SoundEffects, sizeof(sound_effect) * (64 + 32));
        
        //LoadTextures(Renderer, &GameState->TotalArena);
        
        
        GameState->IsInitialized = true;
        GameMemory->IsInitialized = true;
    }
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    
    
    auto Target = math::v3();
    auto DeltaX = InputController->MouseX - GameState->PrevMouseX;
    auto DeltaY = InputController->MouseY - GameState->PrevMouseY;
    math::quat Orientation = Camera.Orientation;
    if(MOUSE(Mouse_Right))
    {
        if(DeltaY > 0.0)
        {
            Orientation = math::Rotate(Camera.Orientation, 1.0f, math::v3(1.0f, 0.0f, 0.0f));
        }
        else if(DeltaY < 0.0)
        {
            Orientation = math::Rotate(Camera.Orientation, -1.0f, math::v3(1.0f, 0.0f, 0.0f));
        }
        
        if(DeltaX > 0.0)
        {
            Orientation = math::Rotate(Camera.Orientation, 1.0f, math::v3(0.0f, 1.0f, 0.0f));
        }
        else if(DeltaX < 0.0)
        {
            Orientation = math::Rotate(Camera.Orientation, -1.0f, math::v3(0.0f, 1.0f, 0.0f));
        }
        
    }
    
    math::v3 Position = Camera.Position;
    if(MOUSE(Mouse_Middle))
    {
        
        if(DeltaY > 0.0)
        {
            Position.y += 1.0f;
        }
        else if(DeltaY < 0.0)
        {
            Position.y += -1.0f;
        }
        
        if(DeltaX > 0.0)
        {
            Position.x += -1.0f;
        }
        else if(DeltaX < 0.0)
        {
            Position.x += 1.0f;
        }
    }
    
    auto Near = -100.0f;
    auto Far = 1000.0f;
    
    
    CameraTransform(Renderer, Renderer.Cameras[Renderer.CurrentCameraHandle], Position, Orientation, math::v3(), Renderer.Cameras[Renderer.CurrentCameraHandle].Zoom, Near, Far, CFlag_Orthographic & ~CFlag_NoLookAt);
    
    
    for(i32 Index = 0; Index < GameState->EntityCount; Index++)
    {
        auto* Entity = &GameState->Entities[Index];
        
        switch(Entity->Type)
        {
            case Entity_Player:
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
                
                Entity->Position += V * (r32)DeltaTime;
                
                // Entity code
                /*Entity->Model.Position = Entity->Position;
                Entity->Model.Orientation = Entity->Orientation;
                Entity->Model.Scale = Entity->Scale;
                Entity->Model.Position = math::v3(0.0f, 5.0f, 0.0f);*/
                PushFilledQuad(Renderer, Entity->Position, Entity->Scale, math::v3(), math::rgba(1.0f, 0.0f, 0.0f, 1.0f), 0, false);
                
                //PushModel(Renderer, Entity->Model);
            }
            break;
        }
    }
    
    if(KEY_DOWN(Key_Escape))
    {
        GameMemory->ExitGame = true;
    }
    
    
    Renderer.ShowMouseCursor = true;
    
    GameState->PrevMouseX = (r32)InputController->MouseX;
    GameState->PrevMouseY = (r32)InputController->MouseY;
}


