#include "main.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "al.h"
#include "alc.h"
#include "windows.h"
#include <SOIL/SOIL.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <mingwthreads/mingw.thread.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "filehandling.h"

#include "rendering.cpp"
#include "world.cpp"
#include "entity.cpp"
#include "keycontroller.cpp"
#include <algorithm>

struct game_code
{
    HMODULE GameCodeDLL;
    FILETIME LastDllWriteTime;
    update *Update;

    bool32 IsValid;
    const char *DllPath = "build/game.dll";
    const char *TempDllPath = "build/game_temp.dll";
};

static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

std::map<std::string, std::string> LoadConfig(std::string Filename)
{
    std::ifstream Input(Filename);          //The input stream
    std::map<std::string, std::string> Ans; //A map of key-value pairs in the file
    while (Input)                           //Keep on going as long as the file stream is good
    {
        std::string Key;                                                           //The key
        std::string Value;                                                         //The value
        std::getline(Input, Key, ':');                                             //Read up to the : delimiter into key
        std::getline(Input, Value, '\n');                                          //Read up to the newline into value
        std::string::size_type Pos1 = Value.find_first_of("\"");                   //Find the first quote in the value
        std::string::size_type Pos2 = Value.find_last_of("\"");                    //Find the last quote in the value
        if (Pos1 != std::string::npos && Pos2 != std::string::npos && Pos2 > Pos1) //Check if the found positions are all valid
        {
            Value = Value.substr(Pos1 + 1, Pos2 - Pos1 - 1); //Take a substring of the part between the quotes
            Ans[Key] = Value;                                //Store the result in the map
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
    Result.GameCodeDLL = LoadLibraryA(Result.DllPath);

    Result.LastDllWriteTime = GetLastWriteTime(Result.DllPath);

    if (Result.GameCodeDLL)
    {
        Result.Update = (update *)GetProcAddress(Result.GameCodeDLL, "Update");
        Result.IsValid = Result.Update != 0;
    }

    if (!Result.IsValid)
    {
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

    auto Map = LoadConfig("./assets/.config");

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
    // glfwSetWindowSizeCallback(GameState.RenderState.Window, WindowSizeCallback);

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
    GameState.InputController = Input;

    glfwSetWindowUserPointer(GameState.RenderState.Window, &GameState);
    glfwSetKeyCallback(GameState.RenderState.Window, KeyCallback);
    glfwSetCharCallback(GameState.RenderState.Window, CharacterCallback);
    glfwSetCursorPosCallback(GameState.RenderState.Window, CursorPositionCallback);

    glfwMakeContextCurrent(GameState.RenderState.Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //load render_state
    RenderSetup(&GameState.RenderState);

    entity_manager EntityManager = {}; //TODO(Daniel) Do something useful with this. And remember the texture_manager

    GameState.Player = {};
    GameState.Player.Type = Entity_Player;
    GameState.Player.player.WalkingSpeed = 10.0f;
    GameState.Player.ShaderIndex = Shader_Texture;
    GameState.Player.TextureHandle = LoadTexture("./assets/textures/player.png");
    GameState.Player.Rotation = glm::vec3(0, 0, 0.3f);
    GameState.Player.Scale = glm::vec3(2, 2, 0);

    GenerateTilemap(&GameState.TilemapData);
    GameState.TilemapData.TileAtlasTexture = LoadTexture("./assets/textures/tiles.png");

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

    double LastFrame = glfwGetTime();
    double CurrentFrame = 0.0;
    double DeltaTime;

    while (!glfwWindowShouldClose(GameState.RenderState.Window))
    {
        //calculate deltatime
        CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        if (GetKey(GLFW_KEY_ESCAPE, &GameState))
            glfwSetWindowShouldClose(GameState.RenderState.Window, GLFW_TRUE);

        ReloadAssets(&AssetManager, &GameState);
        ReloadDlls(&Game);

        GLint Viewport[4];
        glGetIntegerv(GL_VIEWPORT, Viewport);

        memcpy(GameState.RenderState.Viewport, Viewport, sizeof(GLint) * 4);

        Game.Update(DeltaTime, &GameState);
        Render(&GameState);

        SetInvalidKeys(&GameState.InputController); //TODO(Daniel) Move this out of the main loop and into the key_controller.cpp somehow
        glfwPollEvents();
    }

    for (uint32 LoadedSoundIndex = 0;
         LoadedSoundIndex < GameState.SoundManager.LoadedSoundCount;
         LoadedSoundIndex++)
    {
        alDeleteSources(1, &GameState.SoundManager.LoadedSounds[LoadedSoundIndex].Source);
        alDeleteBuffers(1, &GameState.SoundManager.LoadedSounds[LoadedSoundIndex].Buffer);
    }

    GameState.SoundManager.Device = alcGetContextsDevice(GameState.SoundManager.Context);
    alcMakeContextCurrent(0);
    alcDestroyContext(GameState.SoundManager.Context);
    alcCloseDevice(GameState.SoundManager.Device);

    glfwDestroyWindow(GameState.RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}