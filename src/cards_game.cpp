#include "cards_game.h"
#include "gmap.cpp"
#include "rendering.cpp"
#include "skeletal_animation.cpp"
#include "keycontroller.cpp"
#include "ui.cpp"
#include "sound.cpp"
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
    
    if(!GameState->IsInitialized)
    {
        auto* Player = AddCube(GameState, Renderer, Entity_Player);
        Renderer.CurrentCameraHandle = 0;
        Renderer.ClearColor = math::rgba(0.1f, 0.1f, 0.1f, 1.0f);
        GameState->EntityCount = 1;
        
        GameState->IsInitialized = true;
    }
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    
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
                Entity->Model.Position = Entity->Position;
                Entity->Model.Orientation = Entity->Orientation;
                Entity->Model.Scale = Entity->Scale;
                PushModel(Renderer, Entity->Model);
            }
            break;
        }
    }
    
    PushDirectionalLight(Renderer, Normalize(Camera.Position - GameState->Entities[0].Position), math::v3(1.0f, 1.0f, 1.0f), math::v3(1.0f, 1.0f, 1.0f), math::v3(1.0f, 1.0f, 1.0f));
    
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
    
    CameraTransform(Renderer, Camera, Position, Orientation, Target, 1.0f, Near, Far, CFlag_Orthographic);
    
    //CameraTransform(Renderer, Camera, math::v3(1.0f, 1.0f, 1.0f), math::quat(), math::v3(), 1.0f, -100.0f, 1000.0f, CFlag_Orthographic |~ CFlag_NoLookAt);
    
    
    if(KEY_DOWN(Key_Escape))
    {
        GameMemory->ExitGame = true;
    }
    
    GameState->PrevMouseX = (r32)InputController->MouseX;
    GameState->PrevMouseY = (r32)InputController->MouseY;
}


