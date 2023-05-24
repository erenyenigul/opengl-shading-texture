#ifndef SCENE_H
#define SCENE_H

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
#include "shader.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512
#define FLOOR_Y_POS -13.0

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

class SceneObject
{

public:
    SceneObject(vec4 position, Shader &shader, int numVertices);

    void transform(mat4 transformation);
    void update();
    void display();
    void revert();
    void switchColor();
    vec4 getPosition();

protected:
    double previousTime = glfwGetTime();
    vec4 position = vec4(-10.0, 20.0, 0.0, 0.0);
    vec4 speed = vec4(2.0, .0, 0.0, 0.0);
    vec4 acceleration = vec4(0.0, -20.8, 0.0, 0.0);

    GLuint vao = 0;
    
    Shader &shader;

    point4 *points;
    vec3 *normals;
    
    bool isRed = true;
    int numVertices = 0;

    void configGl();
    virtual void form();

    mat4 transformation;

private:
    vec4 initialPosition;
    vec4 initialSpeed;
};


class Ball : public SceneObject
{

public:
    Ball(vec4 position, Shader &shader);

private:
    void form();
    
    int Index = 0;

    void tetrahedron(int count);
    void divide_triangle(const point4 &a, const point4 &b,
                         const point4 &c, int count);
    void triangle(const point4 &a, const point4 &b, const point4 &c);
    point4 unit(const point4 &p);
};

class Cube : public SceneObject
{

public:
    Cube(vec4 position, Shader &shader);

private:
    void form();
};

#endif