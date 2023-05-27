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
#define INIT_LIGHT_POS vec4(10.0, 0.0, 0.0, 1.0)

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// Global variables
SceneObject *object;
Shader *shader;
Light *light;

Material metal{
    color4(0.1745, 0.01175, 0.01175, 0),
    color4(0.61424, 0.04136, 0.04136, 0),
    color4(0.727811, 0.626959, 0.626959, 0),
    0.001f};

Material plastic
{
    color4(0.0, 0.0, 0.0, 0),
    color4(0.5, 0, 0, 0),
    color4(0.7, 0.6, 0.6, 0),
    .25
};

bool isWireframe = false;
bool isBall = false;
bool lightFollow = false;
bool isMetal = false;

//--------------------------------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case 'h':
    case 'H':
        if (action == GLFW_PRESS)
        {
            std::cout << "Jumping Stuff Usage\n"
                         "i -- initialize the pose (top left corner of the window) \n"
                         "c -- switch between two colors (of your choice), which is used to draw lines or triangles. \n"
                         "h -- help; print explanation of your input control (simply to the command line) \n"
                         "q -- quit (exit) the program.\n"
                         "mouse left button -- switch between wireframe and solid mode \n";
        }
        break;
    case 'c':
    case 'C':
        if (action == GLFW_PRESS)
            ;
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
    case 'L':
    case 'l':
        if (action == GLFW_PRESS)
            lightFollow = !lightFollow;

        if (!lightFollow)
            shader->setUniform4fv("LightPosition", INIT_LIGHT_POS);
        break;
    case 'm':
    case 'M':
        if (action != GLFW_PRESS)
            return;

        if (isMetal)
        {

            object->setMaterial(metal);
        }
        else
        {

            object->setMaterial(plastic);
        }
        isMetal = !isMetal;
        break;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS)
            {
                isWireframe = !isWireframe;
                glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
            }
            break;
        }
    }
}

//----------------------------------------------------------------------

void initView(GLFWwindow *window, Shader &shader, int width, int height)
{
    GLfloat aspect_ratio = ((float)width) / height;

    mat4 projection = Perspective(45.0, aspect_ratio, 0.1, 100.0);
    mat4 modelView = LookAt(vec4(0.0, 0.0, 38.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0));

    shader.setUniformMatrix4fv("Projection", projection);
    shader.setUniformMatrix4fv("ModelView", modelView);
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

    initView(window, *shader, width, height);
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

int main(int argc, char **argv)
{
    GLFWwindow *window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, reshape);
    glfwMakeContextCurrent(window);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    shader = new Shader("src/shader/vshader.glsl", "src/shader/fshader.glsl");
    shader->use();

    light = new Light(shader,
                      point4(0, 0, 1, 1.0),
                      color4(1, 1, 1, 0),
                      color4(1, 1, 1, 0),
                      color4(.01f, .01f, .01f, 0));

    initView(window, *shader, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

    object = new Ball(INIT_POS, *shader);
    object->setMaterial(metal);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        object->update();
        object->display();

        if (lightFollow)
        {
            vec4 pos = object->getPosition();
            pos.z += 10;
            light->setPosition(RotateY(0.5) * light->getPosition());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}