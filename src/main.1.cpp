#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <SOIL/SOIL.h>

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 330 core\n"
"layout (location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 model;\n"
"uniform mat4 projection\n"
"void main()\n"
"{\n"
"    TexCoords = vertex.zw;\n"
"    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec3 color\n"
"uniform sampler2D image;\n"
"uniform vec3 spriteColor;\n"
"void main()\n"
"{\n"
"    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
"}\n";

struct Shader 
{
    GLuint program;
};

struct Sprite
{
    vec2 position;
    Shader shader;
};

static void initRenderData()
{

}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    
    //GLuints for handles of buffers, shaders and program
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    
    //GLints for locations of different data for shaders
    GLint mvp_location, vpos_location, vcol_location;
    
    //set error callback to handle errors
    glfwSetErrorCallback(error_callback);
    
    //initialize glfw. if failure then exit
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    //window hints for version
    //version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    //disable window resizing
    glfwWindowHint(GLFW_RESIZABLE, 0);

    //create window
    window = glfwCreateWindow(1080, 720, "Ocean King", NULL, NULL);
    
    //if window creation failed then terminate the application
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //set callback for key input
    glfwSetKeyCallback(window, key_callback);

    //set context of glfw to the window handle of the window just created
    glfwMakeContextCurrent(window);

    //load GLAD
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    //vsync
    glfwSwapInterval(1);

    GLuint quadVAO;
    GLuint VBO;
    GLfloat vertices[] = 
    { 
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);

    //create the vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    
    //set the actual shader
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    
    //compile it
    glCompileShader(vertex_shader);
    
    //do the exact same for the fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    //create the program
    program = glCreateProgram();
    
    //attach both shaders to the program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    
    //and link the program to the context
    glLinkProgram(program);

    //get handles of different uniforms and attributes of shader
    GLint model_location = glGetUniformLocation(program, "model");
    GLint projection_location = glGetUniformLocation(program, "projection");
    GLint color_location = glGetAttribLocation(program, "spriteColor");

    vec2 size = {200, 300};

    mat4x4 model;
    mat4x4_translate(model, 100, 100, 0.0f);  

    mat4x4_translate(model, 0.5f * size[0], 0.5f * size[1], 0.0f); 
    // mat4x4_translate(model, -0.5f * size[0], -0.5f * size[1], 0.0f);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform3f(color_location, 1.0f, 0.0f, 0.0f);
    // Load and generate the texture
    int width, height;
    unsigned char* image = SOIL_load_image("it_works.png", &width, &height, 0, SOIL_LOAD_RGB); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); 

    //while the window is not closed run
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        
        int width, height;

        mat4x4 projection;

        glfwGetFramebufferSize(window, &width, &height);
        
        ratio = width / (float) height;
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        
        mat4x4_identity(projection);
        mat4x4_ortho(projection, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        glUseProgram(program);

        glUniformMatrix4fv(projection_location, 1, GL_FALSE, (const GLfloat*) projection);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat*) model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}