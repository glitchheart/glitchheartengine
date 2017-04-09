#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include "sprite.h"
#include "shader.h"
#include "keycontroller.h"
#include "entity.h"

entity player;
entity idiot;

double deltaTime;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

std::map<std::string,std::string> load_config(std::string filename)
{
    std::ifstream input(filename); //The input stream
    std::map<std::string,std::string> ans; //A map of key-value pairs in the file
    while(input) //Keep on going as long as the file stream is good
    {
        std::string key; //The key
        std::string value; //The value
        std::getline(input, key, ':'); //Read up to the : delimiter into key
        std::getline(input, value, '\n'); //Read up to the newline into value
        std::string::size_type pos1 = value.find_first_of("\""); //Find the first quote in the value
        std::string::size_type pos2 = value.find_last_of("\""); //Find the last quote in the value
        if(pos1 != std::string::npos && pos2 != std::string::npos && pos2 > pos1) //Check if the found positions are all valid
        {
            value = value.substr(pos1+1,pos2-pos1-1); //Take a substring of the part between the quotes
            ans[key] = value; //Store the result in the map
        }
    }
    input.close(); //Close the file stream
    return ans; //And return the result
}

int main(void)
{   
    //load config file
    std::string title;
    std::string version;
    int screenWidth;
    int screenHeight;
    bool fullscreen;

    auto map = load_config("./assets/.config");

    title = map["title"];
    version = map["version"];
    screenWidth = std::stoi(map["screen_width"]);
    screenHeight = std::stoi(map["screen_height"]);
    fullscreen = strcmp("true", map["fullscreen"].c_str()) == 0;

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(screenWidth, screenHeight, (title + std::string(" ") + version).c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    //center window on screen
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    load_sprite("./assets/textures/palm_tree.png", "./assets/shaders/spriteshader", &player.Player.spr);
    load_sprite("./assets/textures/it_works.png", "./assets/shaders/spriteshader", &idiot.Player.spr);

    idiot.Player.spr.position[0] -= 0.5f;

    double lastFrame = glfwGetTime();
    double currentFrame = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        //calculate deltatime
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (is_key_down(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    
        if(is_key_down(GLFW_KEY_LEFT))
            player.Player.spr.position[0] += -2.0f * deltaTime;
        else if(is_key_down(GLFW_KEY_RIGHT))
            player.Player.spr.position[0] += 2.0f * deltaTime;

        if(is_key_down(GLFW_KEY_UP))
            player.Player.spr.position[1] += 2.0f * deltaTime;
        else if(is_key_down(GLFW_KEY_DOWN))
            player.Player.spr.position[1] += -2.0f * deltaTime;

        float ratio;
        int width, height;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 1.0f, 1.0f);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

        mat4x4 projectionMatrix;
        mat4x4_ortho(projectionMatrix, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        
        render_sprite(player.Player.spr, projectionMatrix);
        render_sprite(idiot.Player.spr, projectionMatrix);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}