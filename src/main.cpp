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

#define INIT_POS vec4(-10.0, 15.0, 0.0, 0.0)

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// Global variables
SceneObject *object;
Shader *shader;

bool isWireframe = false;
bool isBall = false;

//--------------------------------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case 'h':
    case 'H':
        if (action == GLFW_PRESS){
            std::cout << "Jumping Stuff Usage\n"
            "i -- initialize the pose (top left corner of the window) \n"
            "c -- switch between two colors (of your choice), which is used to draw lines or triangles. \n"
            "h -- help; print explanation of your input control (simply to the command line) \n"
            "q -- quit (exit) the program.\n"
            "mouse left button -- switch between wireframe and solid mode \n"
            "mouse right button -- switch between cube and sphere \n";
        }
        break;
    case 'c':
    case 'C':
    if (action == GLFW_PRESS)
        object->switchColor();
        break;
    case 'i':
    case 'I':
        object->revert();
        break;
    case 033: // Escape Key
    case 'q':
    case 'Q':
        exit(EXIT_SUCCESS);
        break;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if ( action == GLFW_PRESS ) {
        switch( button ) {
            case GLFW_MOUSE_BUTTON_LEFT:
                if (action == GLFW_PRESS)
                {
                    isWireframe = !isWireframe;
                    glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
                }
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                if (action == GLFW_PRESS)
                {
                    isBall = !isBall;
                    if(isBall)
                        object = new Ball(INIT_POS, *shader);
                    else
                        object = new Cube(INIT_POS, *shader);
                }
                break;
        }
    }
}

//----------------------------------------------------------------------


void adjustFOV(GLFWwindow *window, Shader &shader, int width, int height)
{

    GLfloat aspect_ratio = ((float)width) / height;

    mat4 projection = Ortho(-15.0f, aspect_ratio * 15.0f, -15.0f, 15.0f, -15.0f, 15.0f);

    shader.setUniformMatrix4fv("Projection", projection);
}

void reshape(GLFWwindow *window, int width, int height)
{
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

    adjustFOV(window, *shader, width, height);
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
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, reshape);
    glfwMakeContextCurrent(window);

    shader = new Shader("src/shader/vshader.glsl", "src/shader/fshader.glsl");
    shader->use();
    
    adjustFOV(window, *shader, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    object = new Cube(INIT_POS, *shader);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        object->update();
        object->display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyWindow(window);
    return 0;
}