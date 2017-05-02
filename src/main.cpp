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

//CONSOLE STUFF TODO(Daniel) MOOOOOOOOOOOOOOOOOOVE
void ExecuteCommand(game_state *GameState)
{
    if(strcmp(" ",  GameState->Console.Buffer) != 0
       && strcmp("",  GameState->Console.Buffer) != 0) //NOTE(Daniel) if the command isn't an empty string
    {
        char* Result = &GameState->Console.Buffer[0];
        
        if (strcmp(GameState->Console.Buffer, "exit") == 0)
        {
            //TODO(niels): Need to find a way to call this from here
            //             This should probably be in platform code anyway?
            //             Doesn't really make sense to have it in game code
            //CleanupSound(GameState);
            
            alcMakeContextCurrent(0);
            alcDestroyContext(GameState->SoundManager.Context);
            alcCloseDevice(GameState->SoundManager.Device);
            
            glfwDestroyWindow(GameState->RenderState.Window);
            glfwTerminate();
            exit(EXIT_SUCCESS);
        }
        else if(strcmp(GameState->Console.Buffer, "build") == 0)
        {
            Result = "Building...";
            system("..\\build.bat"); //TODO(Daniel) separate thread
        }
        else if(strstr(GameState->Console.Buffer, "zoom") != NULL)
        {
            //NOTE(Daniel) copy the string before splitting it. The call to strtok manipulates it.
            char ResultCopy[40];
            strcpy(&ResultCopy[0], Result);
            
            char* Pointer;
            char* StrZoomAmount = 0;
            
            Pointer = strtok(&ResultCopy[0], " "); //skip only spaces
            
            int Count = 0;
            
            while(Pointer != NULL && Count < 1)
            {
                if(Count == 0)
                    StrZoomAmount  = strtok(NULL, " ");
                else
                    strtok(NULL, " ");
                Count++;
            }
            real32 ZoomAmount = (real32) strtod(StrZoomAmount, NULL);
            GameState->Camera.Zoom = ZoomAmount;
            
            Result = CombineStrings("Zoom set to ", StrZoomAmount);
        }
        else if(strstr(GameState->Console.Buffer, "jump"))
        {
            //NOTE(Daniel) copy the string before splitting it. The call to strtok manipulates it.
            char ResultCopy[40];
            strcpy(&ResultCopy[0], Result);
            
            char* Pointer;
            char* StrX = 0;
            char* StrY = 0;
            
            Pointer = strtok(&ResultCopy[0], " "); //skip only spaces
            
            int Count = 0;
            
            while(Pointer != NULL && Count < 2)
            {
                if(Count == 0)
                    StrX  = strtok(NULL, " ");
                else if(Count == 1)
                    StrY = strtok(NULL, " ");
                else
                    strtok(NULL, " ");
                Count++;
            }
            
            real32 X = (real32) strtod(StrX, NULL);
            
            real32 Y = (real32) strtod(StrY, NULL);
            
            GameState->Player.Position = glm::vec2(X, Y);
            
            sprintf(Result, "Jumped to position %.2f %.2f", X, Y);
        }
        else
        {
            Result = CombineStrings(Result, ": Command not found");
        }
        
        //NOTE(Daniel) Copy the command into the history buffer
        for(int i = HISTORY_BUFFER_LINES - 1; i > 0; i--)
        {
            sprintf(GameState->Console.HistoryBuffer[i], GameState->Console.HistoryBuffer[i - 1]);
        }
        
        sprintf(GameState->Console.HistoryBuffer[0], Result);
        
        for(int i = 0; i < CONSOLE_BUFFER_SIZE; i++)
            GameState->Console.Buffer[i] = '\0';
        
        GameState->Console.BufferIndex = 0;
    }    
}

//console stuff
static void CheckConsoleInput(game_state* GameState, real32 DeltaTime)
{
    if(GameState->Console.Open && GameState->Console.CurrentTime < GameState->Console.TimeToAnimate)
    {
        GameState->Console.CurrentTime += DeltaTime;
    }
    else if(!GameState->Console.Open && GameState->Console.CurrentTime > 0)
    {
        GameState->Console.CurrentTime -= DeltaTime;
    }
    
    if (GetKeyDown(Key_Tab, GameState))
    {
        GameState->Console.Open = !GameState->Console.Open;
        
        if(GameState->Console.Open)
            GameState->Console.CurrentTime = 0.0f;
        else
            GameState->Console.CurrentTime = GameState->Console.TimeToAnimate;
    }
    
    if (GetKeyDown(Key_Backspace, GameState) && GameState->Console.Open)
    {
        if (GameState->Console.BufferIndex > 0)
            GameState->Console.Buffer[--GameState->Console.BufferIndex] = '\0';
    }
    
    if (GetKeyDown(Key_Enter, GameState) && GameState->Console.Open)
    {
        ExecuteCommand(GameState);
    }
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
    
    entity_manager EntityManager = {}; //TODO(Daniel) Do something useful with this. And remember the texture_manager
    
    GameState.Player = {};
    GameState.Crosshair = {};
    GameState.Crosshair.Type = Entity_Crosshair;
    
    render_entity CrosshairRenderEntity = { };
    CrosshairRenderEntity.ShaderIndex = Shader_Texture;
    CrosshairRenderEntity.TextureHandle = LoadTexture("../assets/textures/crosshair.png");
    GameState.Crosshair.RenderEntity = CrosshairRenderEntity;
    GameState.Crosshair.Rotation = glm::vec3(0, 0, 0);
    GameState.Crosshair.Scale = glm::vec3(1, 1, 0);
    
    GameState.Camera.Zoom = 2.5f;
    GameState.Player.Type = Entity_Player;
    GameState.Player.player.WalkingSpeed = 10.0f;
    
    collision_rect CollisionRect;
    CollisionRect.X = 0;
    CollisionRect.Y = 0;
    CollisionRect.Width = 0.5;
    CollisionRect.Height = 0.5;
    
    GameState.Player.CollisionRect = CollisionRect;
    animation IdleAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_idle.pownim", &IdleAnimation, &GameState.RenderState);
    GameState.Player.Animations.insert(std::pair<char*, animation>(IdleAnimation.Name, IdleAnimation));
    
    animation WalkingAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_walk.pownim", &WalkingAnimation, &GameState.RenderState);
    GameState.Player.Animations.insert(std::pair<char*, animation>(WalkingAnimation.Name, WalkingAnimation));
    
    animation AttackingAnimation = {};
    LoadAnimationFromFile("../assets/animations/player_anim_attack.pownim", &AttackingAnimation, &GameState.RenderState);
    GameState.Player.Animations.insert(std::pair<char*, animation>(AttackingAnimation.Name, AttackingAnimation));
    
    PlayAnimation(&GameState.Player, "player_walk");
    
    render_entity PlayerRenderEntity = { };
    PlayerRenderEntity.ShaderIndex = Shader_SpriteSheetShader;
    PlayerRenderEntity.TextureHandle = LoadTexture("../assets/textures/player.png");
    
    GameState.Player.RenderEntity = PlayerRenderEntity;
    GameState.Player.Rotation = glm::vec3(0, 0, 0.3f);
    GameState.Player.Scale = glm::vec3(2, 2, 0);
    
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
