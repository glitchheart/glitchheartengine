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
#include "collision.cpp"
#include "entity.h"
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
    glfwSwapInterval(1);
    
    //load render_state
    RenderSetup(&GameState.RenderState);
    
    //TODO(Daniel) Do something useful with this. And remember the texture_manager
    entity Player = {};
    Player.Name = "Player";
    Player.Type = Entity_Player;
    Player.Player.WalkingSpeed = 10.0f;
    
    collision_rect CollisionRect;
    CollisionRect.X = 0.6f;
    CollisionRect.Y = 0.6f;
    CollisionRect.Width = 0.8f;
    CollisionRect.Height = 0.8f;
    
    collision_AABB CollisionAABB;
    CollisionAABB.Center = glm::vec2(0,0);
    CollisionAABB.Extents = glm::vec2(0.5f,0.5f);
    
    Player.CollisionRect = CollisionRect;
    Player.CollisionAABB = CollisionAABB;
    animation IdleAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_idle.pownim", &IdleAnimation, &GameState.RenderState);
    Player.Animations.insert(std::pair<char*, animation>(IdleAnimation.Name, IdleAnimation));
    
    animation WalkingAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_walk.pownim", &WalkingAnimation, &GameState.RenderState);
    Player.Animations.insert(std::pair<char*, animation>(WalkingAnimation.Name, WalkingAnimation));
    
    animation AttackingAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_attack.pownim", &AttackingAnimation, &GameState.RenderState);
    Player.Animations.insert(std::pair<char*, animation>(AttackingAnimation.Name, AttackingAnimation));
    
    PlayAnimation(&Player, "player_idle");
    
    render_entity PlayerRenderEntity = { };
    PlayerRenderEntity.ShaderIndex = Shader_SpriteSheetShader;
    PlayerRenderEntity.TextureHandle = LoadTexture("../assets/textures/player.png");
    
    Player.RenderEntity = PlayerRenderEntity;
    Player.Rotation = glm::vec3(0, 0, 0.3f);
    Player.Scale = glm::vec3(2, 2, 0);
    
    Player.EntityIndex = GameState.EntityCount;
    GameState.Entities[GameState.EntityCount++] = Player;
    
    entity Crosshair = {};
    Crosshair.Name = "Crosshair";
    Crosshair.Type = Entity_Crosshair;
    
    collision_rect CollisionRect2;
    CollisionRect2.X = 0.6f;
    CollisionRect2.Y = 0.6f;
    CollisionRect2.Width = 0.8f;
    CollisionRect2.Height = 0.8f;
    Crosshair.CollisionRect = CollisionRect2;
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
    
    collision_rect CollisionRect3;
    CollisionRect3.X = 0.6f;
    CollisionRect3.Y = 0.6f;
    CollisionRect3.Width = 0.8f;
    CollisionRect3.Height = 0.8f;
    Enemy.CollisionRect = CollisionRect3;
    
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = glm::vec2(0,0);
    CollisionAABB3.Extents = glm::vec2(0.5f,0.5f);
    Enemy.CollisionAABB = CollisionAABB3;
    
    render_entity EnemyRenderEntity = { };
    EnemyRenderEntity.ShaderIndex = Shader_Texture;
    EnemyRenderEntity.TextureHandle = LoadTexture("../assets/textures/enemy.png");
    Enemy.RenderEntity = EnemyRenderEntity;
    Enemy.Rotation = glm::vec3(0, 0, 0);
    Enemy.Position = glm::vec2(5,0);
    Enemy.Scale = glm::vec3(2, 2, 0);
    
    Enemy.EntityIndex = GameState.EntityCount;
    GameState.Entities[GameState.EntityCount++] = Enemy;
    
    GameState.Camera.Zoom = 2.5f;
    
    GenerateRoom(ROOM_WIDTH, ROOM_HEIGHT, &GameState.Room);
    
    GameState.Room.RenderEntity = {};
    GameState.Room.RenderEntity.ShaderIndex = Shader_Tile;
    GameState.Room.RenderEntity.TextureHandle = LoadTexture("../assets/textures/tiles.png");
    
    printf("%s\n", glGetString(GL_VERSION));
    
    glfwGetFramebufferSize(GameState.RenderState.Window, &GameState.RenderState.WindowWidth, &GameState.RenderState.WindowHeight);
    glViewport(0, 0, GameState.RenderState.WindowWidth, GameState.RenderState.WindowHeight);
    
    GameState.Camera.ViewportWidth = Width / 20;
    GameState.Camera.ViewportHeight = Height / 20;
    
    game_code Game = LoadGameCode();
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager);
    std::thread t(&ListenToFileChanges, &AssetManager);
    
    GameState.Console = {};
    GameState.EditorUI = {};
    
    sound_manager SoundManager = {};
    InitAudio(&SoundManager);
    LoadSounds(&SoundManager);
    
    if (SoundManager.IsInitialized)
    {
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
        
        if (GetKey(Key_Escape, &GameState))
            glfwSetWindowShouldClose(GameState.RenderState.Window, GLFW_TRUE);
        
        ReloadAssets(&AssetManager, &GameState);
        ReloadDlls(&Game);
        
        Game.Update(DeltaTime, &GameState);
        CheckConsoleInput(&GameState, (real32)DeltaTime);
        CheckEditorUIInput(&GameState, (real32)DeltaTime);
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
    
    CleanupSound(&GameState);
    
    glfwDestroyWindow(GameState.RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
