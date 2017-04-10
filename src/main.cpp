#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include "types.h"
#include "sprite.h"
#include "shader.h"
#include "keycontroller.h"
#include "entity.h"

static void ErrorCallback(int Error, const char* Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

std::map<std::string,std::string> LoadConfig(std::string Filename)
{
    std::ifstream Input(Filename); //The input stream
    std::map<std::string,std::string> Ans; //A map of key-value pairs in the file
    while(Input) //Keep on going as long as the file stream is good
    {
        std::string Key; //The key
        std::string Value; //The value
        std::getline(Input, Key, ':'); //Read up to the : delimiter into key
        std::getline(Input, Value, '\n'); //Read up to the newline into value
        std::string::size_type Pos1 = Value.find_first_of("\""); //Find the first quote in the value
        std::string::size_type Pos2 = Value.find_last_of("\""); //Find the last quote in the value
        if(Pos1 != std::string::npos && Pos2 != std::string::npos && Pos2 > Pos1) //Check if the found positions are all valid
        {
            Value = Value.substr(Pos1+1,Pos2-Pos1-1); //Take a substring of the part between the quotes
            Ans[Key] = Value; //Store the result in the map
        }
    }
    Input.close(); //Close the file stream
    return Ans; //And return the result
}

int main(void)
{   
    sprite_manager SpriteManager = {};

    entity Player = {};
    entity Enemy = {};

    double DeltaTime;
        
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

    GLFWwindow* Window;

    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    Window = glfwCreateWindow(ScreenWidth, ScreenHeight, (Title + std::string(" ") + Version).c_str(), Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    //center window on screen
    const GLFWvidmode* Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int Width, Height;

    glfwGetFramebufferSize(Window, &Width, &Height);
    glfwSetWindowPos(Window, Mode->width / 2 - Width / 2, Mode->height / 2 - Height / 2);

    if (!Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetCursorPosCallback(Window, CursorPositionCallback);

    glfwMakeContextCurrent(Window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    LoadSprite("./assets/textures/player.png", "./assets/shaders/spriteshader", &Player.Player.spriteHandle, &SpriteManager);
    LoadSprite("./assets/textures/it_works.png", "./assets/shaders/spriteshader", &Enemy.Player.spriteHandle, &SpriteManager);
    
    // SpriteManager.Sprites[1].position[0] -= 0.5f;

    double LastFrame = glfwGetTime();
    double CurrentFrame = 0.0;

    while (!glfwWindowShouldClose(Window))
    {
        //calculate deltatime
        CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        if (IsKeyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(Window, GLFW_TRUE);
    
        auto PlayerSprite = &SpriteManager.Sprites[Player.Player.spriteHandle];
        auto EnemySprite = &SpriteManager.Sprites[Enemy.Player.spriteHandle];

        if(IsKeyDown(GLFW_KEY_LEFT))
            PlayerSprite->Position[0] += -2.0f * DeltaTime;
        else if(IsKeyDown(GLFW_KEY_RIGHT))
            PlayerSprite->Position[0] += 2.0f * DeltaTime;

        if(IsKeyDown(GLFW_KEY_UP))
            PlayerSprite->Position[1] += 2.0f * DeltaTime;
        else if(IsKeyDown(GLFW_KEY_DOWN))
            PlayerSprite->Position[1] += -2.0f * DeltaTime;

        float Ratio;    

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 1.0f, 1.0f);

        glfwGetFramebufferSize(Window, &Width, &Height);
        Ratio = Width / (float) Height;
        glViewport(0, 0, Width, Height);

        mat4x4 ProjectionMatrix;
        mat4x4_ortho(ProjectionMatrix, -Ratio, Ratio, -1.f, 1.f, 1.f, -1.f);
        
        RenderSprite(*PlayerSprite, ProjectionMatrix);
        RenderSprite(*EnemySprite, ProjectionMatrix);
        
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glfwDestroyWindow(Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}