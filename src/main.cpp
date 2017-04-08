#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "sprite.h"
#include "shader.h"
#include "keycontroller.h"

Sprite sprite;
double deltaTime;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1080, 720, "Powder engine v0.0.1", NULL, NULL);

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

    load_sprite("./assets/textures/palm_tree.png", "./assets/shaders/spriteshader", &sprite);

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
            sprite.position[0] += -2.0f * deltaTime;
        else if(is_key_down(GLFW_KEY_RIGHT))
            sprite.position[0] += 2.0f * deltaTime;

        if(is_key_down(GLFW_KEY_UP))
            sprite.position[1] += 2.0f * deltaTime;
        else if(is_key_down(GLFW_KEY_DOWN))
            sprite.position[1] += -2.0f * deltaTime;

        float ratio;
        int width, height;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 1.0f, 1.0f);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

        mat4x4 projectionMatrix;
        mat4x4_ortho(projectionMatrix, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        
        render_sprite(sprite, projectionMatrix);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}