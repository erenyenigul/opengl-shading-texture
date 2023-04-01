// fragment shading of sphere model
#include <GLFW/glfw3.h>
#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <iostream>
#include <math.h>

#include "Angel.h"
#include "scene.h"
#include "shader.h"

using namespace std;

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

//--------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 033: // Escape Key
    case 'q':
    case 'Q':
        exit(EXIT_SUCCESS);
        break;
    }
}
//----------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

    GLfloat left = -10.0, right = 10.0;
    GLfloat top = 10.0, bottom = -10.0;
    GLfloat zNear = -20.0, zFar = 20.0;
    GLfloat aspect = GLfloat(width) / height;

    if (aspect > 1.0)
    {
        left *= aspect;
        right *= aspect;
    }
    else
    {
        top /= aspect;
        bottom /= aspect;
    }
}

void adjustFOV(GLFWwindow *window, Shader &shader, int width, int height)
{
    
    GLfloat aspect_ratio = ((float) width) / height;

    mat4 projection = Ortho(-15.0f, aspect_ratio * 15.0f, -15.0f, 15.0f, -15.0f, 15.0f);

    shader.setUniformMatrix4fv("Projection", projection);

}

GLFWwindow *createWindow(int width, int height)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Homework 1", NULL, NULL);

    if (window == NULL)
    {
        cout << "Failed to create window!" << endl;
        glfwTerminate();
        exit(1);
    }

    return window;
}

void destroyWindow(GLFWwindow *window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

//----------------------------------------------------------------------
int main(int argc, char **argv)
{
    
    GLFWwindow *window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwMakeContextCurrent(window);

    Shader shader("src/shader/vshader.glsl", "src/shader/fshader.glsl");
    shader.use();

    adjustFOV(window, shader, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    Cube object(vec4(-12.0, 20, 0.0, 0.0), shader);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
        object.update();
        object.display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyWindow(window);
    return 0;
}