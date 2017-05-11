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
#include "opengl_rendering.h"
#include "opengl_rendering.cpp"
#include "level.cpp"

#define ANIMATION_LOADING
#include "animation.h"
#include "animation.cpp"
#include "collision.h"
#include "entity.h"
#define KEY_INIT
#include "keycontroller.cpp"
#include "keys_glfw.h"
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

static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

std::map<std::string, std::string> LoadConfig(std::string Filename)
{
    std::ifstream Input(Filename);          
    std::map<std::string, std::string> Ans;
    while (Input)                           
    {
        std::string Key;                                                           
        std::string Value;                                                         
        std::getline(Input, Key, ':');                                             
        std::getline(Input, Value, '\n');                                          
        std::string::size_type Pos1 = Value.find_first_of("\"");                   
        std::string::size_type Pos2 = Value.find_last_of("\"");                    
        if (Pos1 != std::string::npos && Pos2 != std::string::npos && Pos2 > Pos1) 
        {
            Value = Value.substr(Pos1 + 1, Pos2 - Pos1 - 1);
            Ans[Key] = Value;                                
        }
    }
    Input.close(); //Close the file stream
    return Ans;    //And return the result
}

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

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glfwSetWindowAspectRatio(Window, 16, 9);
    glViewport(0, 0, Width, Height);
}

void SpawnMillionBarrels(game_state* GameState)
{
    uint32 OneMillion = 1;
    for(uint32 i = 0; i < OneMillion; i++) {
        for(uint32 j = 0; j < OneMillion; j++) {
            entity Barrel = {};
            Barrel.Name = "barrel";
            Barrel.Type = Entity_Barrel;
            Barrel.Layer = Layer_Environment;
            Barrel.IgnoreLayers = Layer_Environment;
            render_entity BarrelRenderEntity = { };
            BarrelRenderEntity.ShaderIndex = Shader_Texture;
            BarrelRenderEntity.TextureHandle = LoadTexture("../assets/textures/barrel.png");
            Barrel.RenderEntity = BarrelRenderEntity;
            Barrel.Rotation = glm::vec3(0, 0, 0);
            Barrel.Position = glm::vec2(2 + i,2 + j);
            Barrel.Scale = glm::vec3(2, 2, 0);
            Barrel.Velocity = glm::vec2(0,0);
            Barrel.Center = glm::vec2(0.5, 0.5);
            Barrel.IsStatic = true;
            
            collision_AABB CollisionAABB;
            
            CollisionAABB.Extents = glm::vec2(0.5f,0.5f);
            CollisionAABB.Center = glm::vec2(0.5f,0.5f);
            Barrel.CollisionAABB = CollisionAABB;
            
            Barrel.EntityIndex = GameState->EntityCount;
            GameState->Entities[GameState->EntityCount++] = Barrel;
        }
    }
}

int main(void)
{
    //load config file
    std::string Title;
    std::string Version;
    int ScreenWidth;
    int ScreenHeight;
    bool Fullscreen;
    
    auto Map = LoadConfig("../assets/.config");
    
    Title = Map["title"];
    Version = Map["version"];
    ScreenWidth = std::stoi(Map["screen_width"]);
    ScreenHeight = std::stoi(Map["screen_height"]);
    Fullscreen = strcmp("true", Map["fullscreen"].c_str()) == 0;
    
    glfwSetErrorCallback(ErrorCallback);
    
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    game_state GameState = {};
    render_state RenderState = {};
    GameState.RenderState = RenderState;
    GameState.RenderState.Window = glfwCreateWindow(ScreenWidth, ScreenHeight, (Title + std::string(" ") + Version).c_str(), Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    
    glfwSetFramebufferSizeCallback(GameState.RenderState.Window, FramebufferSizeCallback);
    
    //center window on screen
    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int Width, Height;
    
    glfwGetFramebufferSize(GameState.RenderState.Window, &Width, &Height);
    glfwSetWindowPos(GameState.RenderState.Window, Mode->width / 2 - Width / 2, Mode->height / 2 - Height / 2);
    
    if (!GameState.RenderState.Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    input_controller Input = {};
    
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if(present)
    {
        Input.ControllerPresent = true;
        printf("Controller present\n");
    }
    
    GameState.InputController = Input;
    
    glfwSetWindowUserPointer(GameState.RenderState.Window, &GameState);
    glfwSetKeyCallback(GameState.RenderState.Window, KeyCallback);
    glfwSetCharCallback(GameState.RenderState.Window, CharacterCallback);
    glfwSetCursorPosCallback(GameState.RenderState.Window, CursorPositionCallback);
    glfwSetMouseButtonCallback(GameState.RenderState.Window,MouseButtonCallback);
    glfwSetInputMode(GameState.RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwMakeContextCurrent(GameState.RenderState.Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);
    
    //load render_state
    RenderSetup(&GameState.RenderState);
    
    entity Player = {};
    Player.Name = "Player";
    Player.Type = Entity_Player;
    Player.Player.WalkingSpeed = 10.0f;
    
    animation IdleAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_idle_new.pownim", &IdleAnimation, &GameState.RenderState);
    Player.Animations.insert(std::pair<char*, animation>(IdleAnimation.Name, IdleAnimation));
    
    animation WalkingAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_walk_new.pownim", &WalkingAnimation, &GameState.RenderState);
    Player.Animations.insert(std::pair<char*, animation>(WalkingAnimation.Name, WalkingAnimation));
    
    animation AttackingAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_attack_new.pownim", &AttackingAnimation, &GameState.RenderState);
    Player.Animations.insert(std::pair<char*, animation>(AttackingAnimation.Name, AttackingAnimation));
    
    render_entity PlayerRenderEntity = { };
    PlayerRenderEntity.ShaderIndex = Shader_SpriteSheetShader;
    
    PlayerRenderEntity.TextureHandle = LoadTexture("../assets/textures/new_player.png");
    
    Player.RenderEntity = PlayerRenderEntity;
    Player.Rotation = glm::vec3(0, 0, 0);
    Player.Scale = glm::vec3(2, 2, 0);
    Player.Velocity = glm::vec2(0,0);
    
    collision_AABB CollisionAABB;
    CollisionAABB.Center = Player.Position;
    CollisionAABB.Extents = glm::vec2(0.5f,1.0f);
    Player.CollisionAABB = CollisionAABB;
    Player.Center = glm::vec2(0.5f, 0.5f);
    Player.EntityIndex = GameState.EntityCount;
    GameState.Entities[GameState.EntityCount++] = Player;
    
    
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
    
    entity Enemy = {};
    Enemy.Name = "enemy";
    Enemy.Type = Entity_Enemy;
    
    render_entity EnemyRenderEntity = { };
    EnemyRenderEntity.ShaderIndex = Shader_Texture;
    EnemyRenderEntity.TextureHandle = LoadTexture("../assets/textures/enemy.png");
    Enemy.RenderEntity = EnemyRenderEntity;
    Enemy.Rotation = glm::vec3(0, 0, 0);
    Enemy.Position = glm::vec2(5,0);
    Enemy.Scale = glm::vec3(2, 2, 0);
    Enemy.Velocity = glm::vec2(-2,0);
    
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = Enemy.Position;
    CollisionAABB3.Extents = glm::vec2(0.5f,0.5f);
    Enemy.CollisionAABB = CollisionAABB3;
    
    Enemy.EntityIndex = GameState.EntityCount;
    GameState.Entities[GameState.EntityCount++] = Enemy;
    
    SpawnMillionBarrels(&GameState);
    
    GameState.Camera.Zoom = 2.5f;
    
    GenerateRoom(0, 0, 10, 10, &GameState.Room);
    
    
    
    GameState.Room.RenderEntity = {};
    GameState.Room.RenderEntity.ShaderIndex = Shader_Tile;
    GameState.Room.RenderEntity.TextureHandle = LoadTexture("../assets/textures/tiles.png");
    
    printf("%s\n", glGetString(GL_VERSION));
    
    glfwGetFramebufferSize(GameState.RenderState.Window, &GameState.RenderState.WindowWidth, &GameState.RenderState.WindowHeight);
    glViewport(0, 0, GameState.RenderState.WindowWidth, GameState.RenderState.WindowHeight);
    
    GameState.Camera.ViewportWidth = Width / 20;
    GameState.Camera.ViewportHeight = Height / 20;
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
