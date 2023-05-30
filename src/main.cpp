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
#include "readppm.h"

using namespace std;

#define SCREEN_WIDTH 1024.0
#define SCREEN_HEIGHT 512.0

#define INIT_POS vec4(-10.0, 15.0, 0.0, 0.0)
#define INIT_LIGHT_POS vec4(1.0, 0.0, 0.0, 0.0)
#define ORIGIN vec4(0.0, 0.0, 0.0, 0.0)

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

SceneObject *object;
Camera *camera;
Shader *shader;
Light *light;

Material metal{
    color4(0.61424, 0.14136, 0.04136, 1),
    color4(0.61424, 0.14136, 0.04136, 1),
    color4(0.727811, 0.626959, 0.626959, 1),
    64};

Material plastic{
    color4(0.5, 0.1, 0.1, 1),
    color4(0.5, 0.1, 0.1, 1),
    color4(0.7, 0.6, 0.6, 1),
    10000000};

bool isWireframe = false;
bool isBall = false;
bool lightFollow = false;
bool isMetal = false;
bool isPhong = false;

int colorOffIndex = 0;

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
    case 's':
    case 'S':
        if (action == GLFW_PRESS)
        {
            if (isPhong)
            {
                shader->switchProgram("src/shader/gouraud/vshader.glsl", "src/shader/gouraud/fshader.glsl");
            }
            else
            {
                shader->switchProgram("src/shader/phong/vshader.glsl", "src/shader/phong/fshader.glsl");
            }
            isPhong = !isPhong;

            shader->use();
            object->configGl();
            light->configGl();
            camera->configGl();
        }
        break;

    case 'w':
    case 'W':
        camera->setEye(camera->getEye() + vec4(0.0, 0.0, 0.5, 0.0));
        break;
    case 'z':
    case 'Z':
        camera->setEye(camera->getEye() + vec4(0.0, 0.0, -0.5, 0.0));
        break;
    case 'o':
    case 'O':
        if(action != GLFW_PRESS) return;
        colorOffIndex = (colorOffIndex + 1) % 4;

        light->enableAmbient();
        light->enableDiffuse();
        light->enableSpecular();
        switch (colorOffIndex)
        {
        case 1:
            light->disableAmbient();
            break;
        case 2:
            light->disableDiffuse();
            break;
        case 3:
            light->disableSpecular();
            break;
        default:
            break;
        }
    }
}

void reshape(GLFWwindow *window, int width, int height)
{
    camera->setProjection(Perspective(45.0, ((float)width) / height, 0.1, 100.0));
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

    shader = new Shader("src/shader/texture/vshader.glsl", "src/shader/texture/fshader.glsl");
    shader->use();

    GLuint textures[2];
    bool textureFlag = true; //enable texture mapping

    GLuint  TextureFlagLoc; // texture flag uniform location

    Texture earth = readppm("src/asset/earth.ppm");
    
    glGenTextures( 2, textures );
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, earth.rows, earth.cols, 0,
		  GL_RGB, GL_UNSIGNED_BYTE, earth.content );
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR); //try here different alternatives
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //try here different alternatives

    glBindTexture( GL_TEXTURE_2D, textures[0] ); //set current texture

    light = new Light(shader,
                      point4(1.001, -1.001, 1.001, 0.0),
                      color4(0.3, 0.3, 0.3, 1),
                      color4(1, 1, 1, 1),
                      color4(1, 1, 1, 1));

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    camera = new Camera(shader);
    camera->setProjection(Perspective(45.0, SCREEN_WIDTH / SCREEN_HEIGHT, 0.1, 100.0));
    camera->lookAt(vec4(0.0, 0.0, 10.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0));

    object = new Ball(ORIGIN, *shader);
    object->setMaterial(metal);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //object->update();
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