#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "types.h"
#include "keycontroller.h"
#include "entity.h"
#include "rendering.cpp"
#include "world.cpp"

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

    //load render_state
    render_state RenderState = {};
    LoadAllShaders(&RenderState);

    entity_manager EntityManager = {}; //TODO(danieL) Do something useful with this. And remember the texture_manager

    entity PlayerEntity = {};
    PlayerEntity.Type = Entity_Player;
    PlayerEntity.player.WalkingSpeed = 200.0f;
    PlayerEntity.ShaderIndex = 0; //TODO(danieL) TextureShader - Should have an enumeration for this
    PlayerEntity.TextureHandle = LoadTexture("./assets/textures/player.png");

    entity PalmTreeEntity = {};

    double LastFrame = glfwGetTime();
    double CurrentFrame = 0.0;
    double DeltaTime;

    while (!glfwWindowShouldClose(Window))
    {
        //calculate deltatime
        CurrentFrame = glfwGetTime();
        DeltaTime = CurrentFrame - LastFrame;
        LastFrame = CurrentFrame;

        if (IsKeyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(Window, GLFW_TRUE);

        if(IsKeyDown(GLFW_KEY_LEFT))
            PlayerEntity.Position.x += -PlayerEntity.player.WalkingSpeed * DeltaTime;
        else if(IsKeyDown(GLFW_KEY_RIGHT))
            PlayerEntity.Position.x += PlayerEntity.player.WalkingSpeed * DeltaTime;

        if(IsKeyDown(GLFW_KEY_UP))
            PlayerEntity.Position.y += -PlayerEntity.player.WalkingSpeed * DeltaTime;
        else if(IsKeyDown(GLFW_KEY_DOWN))
            PlayerEntity.Position.y += PlayerEntity.player.WalkingSpeed * DeltaTime;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 1.0f, 1.0f);

        glfwGetFramebufferSize(Window, &Width, &Height);

        glViewport(-10, 0, Width, Height);
        glm::mat4 ProjectionMatrix = glm::ortho(0.0f, static_cast<GLfloat>(Width), static_cast<GLfloat>(Height), 0.0f, -1.0f, 1.0f);
        
        Render(&RenderState, PlayerEntity, ProjectionMatrix);
        
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glfwDestroyWindow(Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static Update(double DeltaTime, entity Entity)
{

}