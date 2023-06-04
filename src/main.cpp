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
#include <string>

#include "Angel.h"
#include "scene.h"
#include "shader.h"
#include "readppm.h"

using namespace std;

#define SCREEN_WIDTH 1024.0
#define SCREEN_HEIGHT 512.0

#define INIT_POS vec4(-15.0, 11.0, 0.0, 0.0)
#define INIT_LIGHT_POS vec4(1.0, 0.0, 0.0, 0.0)
#define ORIGIN vec4(0.0, 0.0, 0.0, 0.0)

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

SceneObject *object;
Camera *camera;
Shader *shader;
Light *light;
GLuint textures[2];
int textureIndex = 0;

Material metalRed{
    color4(0.1, 0.1, 0.1, 1),
    color4(0.61424, 0.14136, 0.04136, 1),
    color4(0.727811, 0.626959, 0.626959, 1),
    64};

Material metalBlue{
    color4(0.1, 0.1, 0.1, 1),
    color4(0.04136, 0.14136, 0.61424, 1),
    color4(0.727811, 0.626959, 0.626959, 1),
    64};

Material plasticRed{
    color4(0.1, 0.1, 0.1, 1),
    color4(0.5, 0.1, 0.1, 1),
    color4(0.7, 0.6, 0.6, 1),
    10000000};

Material plasticBlue{
    color4(0.1, 0.1, 0.1, 1),
    color4(0.1, 0.1, 0.5, 1),
    color4(0.6, 0.6, 0.7, 1),
    10000000};

Material metalWhite{
    color4(0.1, 0.1, 0.1, 1),
    color4(1, 1, 1, 1),
    color4(1, 1, 1, 1),
    1000};

bool isWireframe = false;
bool isBall = false;
bool lightFollow = false;

bool isMetal = false;
bool isRed = false;

int displayMode = 0;
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
    case 'a':
    case 'A':
        camera->setEye(RotateY(5.0) * camera->getEye());
        break;
    case 'e':
    case 'E':
        if (action == GLFW_PRESS)
        {
            glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
            textureIndex = (textureIndex + 1) % 2;
            break;
        }
    case 'h':
    case 'H':
        if (action == GLFW_PRESS)
        {
            std::cout << "Shading & Texture Ball Usage\n"
                         "i -- initialize the pose (top left corner of the window) \n"
                         "c -- switch between two colors, which is used to draw lines or triangles. \n"
                         "h -- help; print explanation of your input control (simply to the command line) \n"
                         "q -- quit (exit) the program.\n"
                         "a -- rotate the camera\n"
                         "e -- switch between two textures\n"
                         "l -- switch between light follow mode and light fixed mode\n"
                         "m -- switch between metal and plastic material\n"
                         "t -- switch between shading, wireframe, and texture display modes\n"
                         "s -- switch between phong and gouraud shading\n"
                         "o -- turn off the specular, diffuse and ambient components one by one\n"
                         "z -- zoom in\n"
                         "w -- zoom out\n"
                         "mouse left button -- switch between wireframe and solid mode \n";
        }
        break;
    case 'c':
    case 'C':
        if (action != GLFW_PRESS)
            return;

        if (isRed)
            object->setMaterial(isMetal ? metalBlue : plasticBlue);
        else
            object->setMaterial(isMetal ? metalRed : plasticRed);

        isRed = !isRed;
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
    {
        if (action == GLFW_PRESS)
            lightFollow = !lightFollow;

        vec4 pos = light->getPosition();
        pos[3] = lightFollow ? 1 : 0;

        light->setPosition(pos);
        break;
    }

    case 'm':
    case 'M':
        if (action != GLFW_PRESS)
            return;

        if (isMetal)
            object->setMaterial(isRed ? plasticRed : plasticBlue);
        else
            object->setMaterial(isRed ? metalRed : metalBlue);

        isMetal = !isMetal;
        break;
    case 't':
    case 'T':
        if (action == GLFW_PRESS)
        {
            string folder;
            switch (displayMode)
            {
            case 0:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                folder = isPhong ? "src/shader/phong/" : "src/shader/gouraud/";
                shader->switchProgram((folder + "vshader.glsl").c_str(), (folder + "fshader.glsl").c_str());
                displayMode = 1;
                break;
            case 1:
                shader->switchProgram("src/shader/texture/vshader.glsl", "src/shader/texture/fshader.glsl");
                displayMode = 2;
                break;
            case 2:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                displayMode = 0;
                break;
            }
            shader->use();
            object->configGl();
            light->configGl();
            camera->configGl();
        }
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
        camera->setEye(camera->getEye() + (camera->getAt() - camera->getEye()) * 0.1);
        break;
    case 'z':
    case 'Z':
        camera->setEye(camera->getEye() - (camera->getAt() - camera->getEye()) * 0.1);
        break;
    case 'o':
    case 'O':
        if (action != GLFW_PRESS)
            return;
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

    shader = new Shader("src/shader/gouraud/vshader.glsl", "src/shader/gouraud/fshader.glsl");
    shader->use();

    bool textureFlag = true; // enable texture mapping

    GLuint TextureFlagLoc; // texture flag uniform location

    Texture earth = readppm("src/asset/earth.ppm");
    Texture basketball = readppm("src/asset/basketball.ppm");

    glGenTextures(2, textures);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, earth.rows, earth.cols, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, earth.content);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, basketball.rows, basketball.cols, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, basketball.content);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textures[0]); // set current texture

    light = new Light(shader,
                      point4(0.0, 1.0, 0.0, 1.0),
                      color4(0.4, 0.4, 0.4, 1),
                      color4(1, 1, 1, 1),
                      color4(1, 1, 1, 1));

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    camera = new Camera(shader);
    camera->setProjection(Perspective(45.0, SCREEN_WIDTH / SCREEN_HEIGHT, 0.1, 100.0));
    camera->lookAt(vec4(0.0, 0.0, 30.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0));

    object = new Ball(INIT_POS, *shader, 2.0);
    object->setMaterial(plasticBlue);
  
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        object->update();
        object->display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}