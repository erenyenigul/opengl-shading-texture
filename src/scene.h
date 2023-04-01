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

#define FLOOR_Y_POS -13.0

class Ball
{

public:
    Ball(vec4 position);

    void update();
    void display();
    void setUniform(GLuint loc);

private:
    vec4 position = vec4(-10.0, 10.0, 0.0, 0.0);
    vec4 speed = vec4(.02, .0, 0.0, 0.0);
    vec4 acceleration = vec4(0.0, -0.001, 0.0, 0.0);
    GLuint positionLoc = 0;
};

#endif