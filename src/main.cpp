#include "main.h"
#include "game.h"
#include "rendering.cpp"
#include "world.cpp"
#include "entity.cpp"
#include "keycontroller.cpp"
#include <algorithm>

static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

static void HandleError(char const *File, int32 LineNum, char const *msg)
{
    fprintf(stderr, "Error on in file %s on line %d\n", File, LineNum);
    fprintf(stderr, "%s\n", msg);
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

struct game_code
{
    HMODULE GameCodeDLL;
    FILETIME LastDllWriteTime;
    update *Update;

    bool32 IsValid;
    const char* DllPath = "build/game.dll";
    const char* TempDllPath = "build/game_temp.dll";
};

static game_code LoadGameCode()
{
    game_code Result = {};

    CopyFile(Result.DllPath, Result.TempDllPath,false);
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

static void ReloadGameCode(game_code* GameCode)
{
    UnloadGameCode(GameCode);
    *GameCode = LoadGameCode();
}

static void ReloadDlls(game_code* Game)
{
    FILETIME LastWriteTime = GetLastWriteTime(Game->DllPath);
    
    if(CompareFileTime(&Game->LastDllWriteTime, &LastWriteTime) != 0)
    {
        std::cout << "RELOAD" << std::endl;
        ReloadGameCode(Game);
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

    auto Map = LoadConfig("./assets/.config");

    Title = Map["title"];
    Version = Map["version"];
    ScreenWidth = std::stoi(Map["screen_width"]);
    ScreenHeight = std::stoi(Map["screen_height"]);
    Fullscreen = strcmp("true", Map["fullscreen"].c_str()) == 0;

    // GameState.RenderState = RendersState;

    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    game_state GameState = {};
    render_state RenderState = {};
    GameState.RenderState = RenderState;
    GameState.RenderState.Window = glfwCreateWindow(ScreenWidth, ScreenHeight, (Title + std::string(" ") + Version).c_str(), Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

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

    glfwSetWindowUserPointer(GameState.RenderState.Window,&GameState);
    glfwSetKeyCallback(GameState.RenderState.Window, KeyCallback);
    glfwSetCursorPosCallback(GameState.RenderState.Window, CursorPositionCallback);

    glfwMakeContextCurrent(GameState.RenderState.Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //load render_state
    LoadAllShaders(&GameState.RenderState);

    entity_manager EntityManager = {}; //TODO(Daniel) Do something useful with this. And remember the texture_manager

    GameState.Player = {};
    GameState.Player.Type = Entity_Player;
    GameState.Player.player.WalkingSpeed = 10.0f;
    GameState.Player.ShaderIndex = 0; //TODO(Daniel) TextureShader - Should have an enumeration for this
    GameState.Player.TextureHandle = LoadTexture("./assets/textures/player.png");
    GameState.Player.Rotation = glm::vec3(0, 0, 0.3f);
    GameState.Player.Scale = glm::vec3(2, 2, 0);

    // //@TESTCODE

    GenerateTilemap(&GameState.TilemapData);
    GameState.TilemapData.TileAtlasTexture = LoadTexture("./assets/textures/tiles.png");

    double LastFrame = glfwGetTime();
    double CurrentFrame = 0.0;
    double DeltaTime;

    printf("%s", glGetString(GL_VERSION));

    glfwGetFramebufferSize(GameState.RenderState.Window, &GameState.RenderState.WindowWidth, &GameState.RenderState.WindowHeight);
    glViewport(0, 0, GameState.RenderState.WindowWidth, GameState.RenderState.WindowHeight);

    GameState.Camera.ViewportWidth = Width / 20;
    GameState.Camera.ViewportHeight = Height / 20;

    game_code Game = LoadGameCode();
    uint32 LoadCounter = 0;

    //setup asset reloading
    asset_manager AssetManager = {};
    std::thread t(&ListenToFileChanges, &AssetManager);
    
    while (!glfwWindowShouldClose(GameState.RenderState.Window))
    {
        //calculate deltatime
        CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;   
        LastFrame = CurrentFrame;

        if (IsKeyDown(GLFW_KEY_ESCAPE,&GameState))
            glfwSetWindowShouldClose(GameState.RenderState.Window, GLFW_TRUE);

        ReloadAssets(&AssetManager, &GameState.RenderState);
        ReloadDlls(&Game);

        GLint Viewport[4];
        glGetIntegerv(GL_VIEWPORT,Viewport);
        
        memcpy(GameState.RenderState.Viewport, Viewport,sizeof(GLint) * 4);

        Game.Update(DeltaTime, &GameState);
        Render(&GameState);
        glfwPollEvents();
        
    }

    glfwDestroyWindow(GameState.RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}