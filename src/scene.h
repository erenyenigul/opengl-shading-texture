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
    SceneObject(vec4 position, Shader &shader);

    void update();
    void display();

    int NumTimesToSubdivide = 5;
    int NumTriangles = 4096;

    // (4 faces)^(NumTimesToSubdivide + 1)
    int NumVertices = 3 * NumTriangles;

private:
    void initSphere();

    vec4 position = vec4(-10.0, 10.0, 0.0, 0.0);
    vec4 speed = vec4(.0002, .0, 0.0, 0.0);
    vec4 acceleration = vec4(0.0, -0.001, 0.0, 0.0);

    GLuint vao = 0;
    
    Shader &shader;

    int Index = 0;

    point4 *points;
    vec3 *normals;

    void tetrahedron(int count);
    void divide_triangle(const point4 &a, const point4 &b,
                         const point4 &c, int count);
    void triangle(const point4 &a, const point4 &b, const point4 &c);
    point4 unit(const point4 &p);
};

#endif