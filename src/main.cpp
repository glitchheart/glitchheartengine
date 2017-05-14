#include "main.h"

#include <glad/glad.h>
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include "al.h"
#include "alc.h"
#include "windows.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>
#ifdef MINGW
#include <mingwthreads/mingw.thread.h>
#else
#include <thread>
#endif
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <map>

#include "game.h"
#include "platform_sound.h"
#include "platform_sound.cpp"
#include "filehandling.h"
#define KEY_INIT
#include "keycontroller.cpp"
#include "keys_glfw.h"
#include "opengl_rendering.h"
#include "opengl_rendering.cpp"

#define ANIMATION_LOADING
#include "animation.h"
#include "animation.cpp"
#include "collision.h"
#include "entity.h"
#include "level.cpp"
#include "console.h"
#include "console.cpp"
#include "editor_ui.h"
#include "editor_ui.cpp"

struct game_code
{
    HMODULE GameCodeDLL;
    FILETIME LastDllWriteTime;
    update *Update;
    
    bool32 IsValid;
    const char *DllPath = "game.dll";
    const char *TempDllPath = "game_temp.dll";
};

static game_code LoadGameCode()
{
    game_code Result = {};
    
    CopyFile(Result.DllPath, Result.TempDllPath, false);
    Result.Update = UpdateStub;
    Result.GameCodeDLL = LoadLibraryA(Result.TempDllPath);
    
    Result.LastDllWriteTime = GetLastWriteTime(Result.DllPath);
    
    if (Result.GameCodeDLL)
    {
        Result.Update = (update *)GetProcAddress(Result.GameCodeDLL, "Update");
        Result.IsValid = Result.Update != 0;
    }
    
    if (!Result.IsValid)
    {
        printf("Invalid\n");
        Result.Update = UpdateStub;
    }
    
    return Result;
}

static void UnloadGameCode(game_code *GameCode)
{
    if (GameCode->GameCodeDLL)
    {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->Update = UpdateStub;
}

static void ReloadGameCode(game_code *GameCode)
{
    UnloadGameCode(GameCode);
    *GameCode = LoadGameCode();
}

static void ReloadDlls(game_code *Game)
{
    FILETIME LastWriteTime = GetLastWriteTime(Game->DllPath);
    
    if (CompareFileTime(&Game->LastDllWriteTime, &LastWriteTime) != 0)
    {
        std::cout << "RELOAD" << std::endl;
        ReloadGameCode(Game);
    }
}

void SpawnMillionBarrels(game_state* GameState)
{
    uint32 OneMillion = 2;
    for(uint32 i = 0; i < OneMillion; i++)
    {
        for(uint32 j = 0; j < OneMillion; j++)
        {
            entity Barrel = {};
            Barrel.Name = "barrel";
            Barrel.Type = Entity_Barrel;
            Barrel.Layer = Layer_Environment;
            Barrel.IgnoreLayers = Layer_Environment;
            render_entity BarrelRenderEntity = { };
            BarrelRenderEntity.ShaderIndex = Shader_Texture;
            BarrelRenderEntity.TextureHandle = GameState->RenderState.BarrelTexture;
            Barrel.RenderEntity = BarrelRenderEntity;
            Barrel.Rotation = glm::vec3(0, 0, 0);
            Barrel.Position = glm::vec2(2 + i,2 + j);
            Barrel.Scale = glm::vec3(2, 2, 0);
            Barrel.Velocity = glm::vec2(0,0);
            Barrel.Center = glm::vec2(0.5, 0.5);
            Barrel.IsStatic = true;
            Barrel.IsKinematic = false;
            Barrel.Pickup = true;
            
            collision_AABB CollisionAABB;
            
            CollisionAABB.Extents = glm::vec2(0.5f,0.5f);
            CollisionAABB.Center = glm::vec2(Barrel.Position.x + Barrel.Center.x * Barrel.Scale.x,
                                             Barrel.Position.y + Barrel.Center.y * Barrel.Scale.y);
            //CollisionAABB.Center = glm::vec2(0.5f,0.5f);
            Barrel.CollisionAABB = CollisionAABB;
            
            Barrel.EntityIndex = GameState->EntityCount;
            GameState->Entities[GameState->EntityCount++] = Barrel;
        }
    }
}

int main(void)
{
    game_state GameState = {};
    GameState.InputController = {};
    render_state RenderState;
    
    InitializeOpenGL(&GameState, &RenderState);
    
    GameState.RenderState = RenderState;
    
    entity Crosshair = {};
    Crosshair.Name = "Crosshair";
    Crosshair.Type = Entity_Crosshair;
    
    Crosshair.IsKinematic = true;
    collision_AABB CollisionAABB2;
    CollisionAABB2.Center = glm::vec2(0,0);
    CollisionAABB2.Extents = glm::vec2(0.5f,0.5f);
    Crosshair.CollisionAABB = CollisionAABB2;
    
    render_entity CrosshairRenderEntity = { };
    CrosshairRenderEntity.ShaderIndex = Shader_Texture;
    CrosshairRenderEntity.TextureHandle = LoadTexture("../assets/textures/crosshair.png");
    Crosshair.RenderEntity = CrosshairRenderEntity;
    Crosshair.Rotation = glm::vec3(0, 0, 0);
    Crosshair.Scale = glm::vec3(1, 1, 0);
    
    Crosshair.EntityIndex = GameState.EntityCount;
    GameState.Entities[GameState.EntityCount++] = Crosshair;
    
    SpawnMillionBarrels(&GameState);
    
    GameState.Camera.Zoom = 2.5f;
    
    level Level;
    LoadLevelFromFile("../assets/levels/level_02.plv", &Level, &GameState);
    GameState.CurrentLevel = Level;
    
    GameState.Entities[GameState.PlayerIndex].Position = Level.PlayerStartPosition;
    
    GameState.Camera.ViewportWidth = RenderState.WindowWidth / 20;
    GameState.Camera.ViewportHeight = RenderState.WindowHeight / 20;
    GameState.GameMode = Mode_InGame;
    game_code Game = LoadGameCode();
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager);
    std::thread t(&ListenToFileChanges, &AssetManager);
    
    GameState.Console = {};
    GameState.EditorUI = {};
    
    sound_device SoundDevice = {};
    InitAudio(&SoundDevice);
    
    if (SoundDevice.IsInitialized)
    {
        sound_manager SoundManager = {};
        LoadSounds(&SoundManager);
        ResetSoundQueue(&SoundManager);
        GameState.SoundManager = SoundManager;
    }
    
    real64 LastFrame = glfwGetTime();
    real64 CurrentFrame = 0.0;
    real64 DeltaTime;
    GLint Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport);
    
    memcpy(GameState.RenderState.Viewport, Viewport, sizeof(GLint) * 4);
    
    InitCommands();
    
    while (!glfwWindowShouldClose(GameState.RenderState.Window))
    {
        //calculate deltatime
        CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;
        real64 FPS = 1.0/DeltaTime;
        GameState.RenderState.FPS = FPS;
        
        if(GameState.GameMode == Mode_Exit || GetKeyDown(Key_Q, &GameState) && GetKey(Key_LeftCtrl, &GameState))
            glfwSetWindowShouldClose(GameState.RenderState.Window, GLFW_TRUE);
        
        ReloadAssets(&AssetManager, &GameState);
        ReloadDlls(&Game);
        
        Game.Update(DeltaTime, &GameState);
        CheckConsoleInput(&GameState, DeltaTime);
        CheckEditorUIInput(&GameState, DeltaTime);
        Render(&GameState);
        PlaySounds(&GameState);
        
        SetControllerInvalidKeys(&GameState.InputController);
        SetInvalidKeys(&GameState.InputController);
        SetMouseInvalidKeys(&GameState.InputController);
        
        //TODO(Daniel) Move this out of the main loop and into the key_controller.cpp somehow
        
        glfwPollEvents();
        
        if(GameState.InputController.ControllerPresent)
        {
            ControllerKeys(&GameState,GLFW_JOYSTICK_1);
        }
    }
    
    CleanupSound(&SoundDevice,&GameState.SoundManager);
    
    glfwDestroyWindow(GameState.RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
