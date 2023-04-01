#include "scene.h"
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

Ball::Ball(vec4 position)
{
    position = position;
}

void Ball::update()
{
    if (this->position.y < FLOOR_Y_POS)
    {
        this->speed.y = -this->speed.y * 0.8f;
        this->position.y = FLOOR_Y_POS + 0.1;
    }

    this->speed += this->acceleration;
    this->position += this->speed;
}

void Ball::display()
{
    glUniform4fv(this->positionLoc, 1, this->position);
}

void Ball::setUniform(GLuint loc)
{
    this->positionLoc = loc;
}