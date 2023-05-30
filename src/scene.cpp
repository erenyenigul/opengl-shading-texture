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
#include "shader.h"

#define FLOOR_Y_POS -13.5

SceneObject::SceneObject(vec4 position, Shader &shader, int numVertices)
    : position{position},
      initialPosition{position},
      initialSpeed{vec4(2.0, .0, 0.0, 0.0)},
      shader{shader},
      points{new point4[numVertices]},
      normals{new vec3[numVertices]},
      numVertices{numVertices},
      transformation{mat4(1.0)},
      material{Material{
            color4(1.0, 1.0, 1.0, 1.0),
            color4(1.0, 1.0, 1.0, 1.0),
            color4(1.0, 1.0, 1.0, 1.0),
            1.0
      }}
{
    transformation = Translate(initialPosition[0], initialPosition[1], initialPosition[2]);
}

void SceneObject::setMaterial(Material m)
{
    this->material = m;
}

Material& SceneObject::getMaterial()
{
    return this->material;
}

void SceneObject::transform(mat4 transformation)
{
    this->transformation = transformation * this->transformation;
}

vec4 SceneObject::getPosition()
{
    return this->transformation * vec4(0,0,0,1);
}

void SceneObject::configGl()
{
    // Create a vertex array object
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * numVertices + sizeof(vec3) * numVertices,
                 NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4) * numVertices, this->points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4) * numVertices,
                    sizeof(vec3) * numVertices, this->normals);

    // set up vertex arrays
    GLuint vPosition = shader.getAttribLocation("vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vNormal = shader.getAttribLocation("vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4) * this->numVertices));
}

void SceneObject::revert()
{
    this->transformation = Translate(initialPosition[0], initialPosition[1], initialPosition[2]);
    this->speed = initialSpeed;
}

void SceneObject::update()
{
    double time = glfwGetTime();
    double delta = time - this->previousTime;
    double min_refresh_time = 0.01;

    if (delta > min_refresh_time)
    {
        vec4 position = this->getPosition();

        if (position.y < FLOOR_Y_POS)
        {
            this->speed.y = abs(this->speed.y) * 0.8f;
        }

        this->speed += this->acceleration * delta;
        vec4 scaled = this->speed * delta;
        this->transform(Translate(scaled[0], scaled[1], scaled[2]));

        this->previousTime = time;
    }
}

void SceneObject::display()
{
    shader.setUniformMatrix4fv("Transformation", this->transformation);
    shader.setUniform4fv("AmbientMaterial", material.ambient);
    shader.setUniform4fv("DiffuseMaterial", material.diffuse);
    shader.setUniform4fv("SpecularMaterial", material.specular);
    shader.setUniform1f("Shininess", material.shininess);

    glBindVertexArray(this->vao);

    glDrawArrays(GL_TRIANGLES, 0, this->numVertices);
}

void SceneObject::form()
{
}

void Ball::triangle(const point4 &a, const point4 &b, const point4 &c)
{
    vec3 normal = normalize(cross(b - a, c - b));
    this->normals[Index] = normal;
    this->points[Index] = a;
    this->Index++;
    this->normals[Index] = normal;
    this->points[Index] = b;
    this->Index++;
    this->normals[Index] = normal;
    this->points[Index] = c;
    this->Index++;
}

point4 Ball::unit(const point4 &p)
{
    float len = p.x * p.x + p.y * p.y + p.z * p.z;
    point4 t;
    if (len > DivideByZeroTolerance)
    {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}

void Ball::divide_triangle(const point4 &a, const point4 &b,
                           const point4 &c, int count)
{
    if (count > 0)
    {
        point4 v1 = unit(a + b);
        point4 v2 = unit(a + c);
        point4 v3 = unit(b + c);
        divide_triangle(a, v1, v2, count - 1);
        divide_triangle(c, v2, v3, count - 1);
        divide_triangle(b, v3, v1, count - 1);
        divide_triangle(v1, v3, v2, count - 1);
    }
    else
    {
        triangle(a, b, c);
    }
}

void Ball::tetrahedron(int count)
{
    point4 v[4] = {
        vec4(0.0, 0.0, 1.0, 1.0),
        vec4(0.0, 0.942809, -0.333333, 1.0),
        vec4(-0.816497, -0.471405, -0.333333, 1.0),
        vec4(0.816497, -0.471405, -0.333333, 1.0)};

    divide_triangle(v[0], v[1], v[2], count);
    divide_triangle(v[3], v[2], v[1], count);
    divide_triangle(v[0], v[3], v[1], count);
    divide_triangle(v[0], v[2], v[3], count);
}

void Ball::form()
{
    tetrahedron(6);
}

Ball::Ball(vec4 position, Shader &shader)
    : SceneObject(position, shader, 3*pow(4,(8 + 1)))
{
    form();
    configGl();
}

Cube::Cube(vec4 position, Shader &shader) : SceneObject(position, shader, 12)
{
    form();
    configGl();
}

void Cube::form()
{
    // this->points containing 12 different vertices that form 6 faces
    // this->normals containing 12 different normals that form 6 faces
    // this->numVertices = 12

    // 6 faces
    // 2 triangles per face

    // 2 triangles per face
    // 3 vertices per triangle

    this->points[0] = point4(-2.0, -2.0, 2.0, 1.0);
    this->points[1] = point4(-2.0, 2.0, 2.0, 1.0);
    this->points[2] = point4(2.0, 2.0, 2.0, 1.0);
    this->points[3] = point4(2.0, 2.0, 2.0, 1.0);
    this->points[4] = point4(2.0, -2.0, 2.0, 1.0);
    this->points[5] = point4(-2.0, -2.0, 2.0, 1.0);
    this->points[6] = point4(-2.0, -2.0, -2.0, 1.0);
    this->points[7] = point4(-2.0, 2.0, -2.0, 1.0);
    this->points[8] = point4(2.0, 2.0, -2.0, 1.0);
    this->points[9] = point4(2.0, 2.0, -2.0, 1.0);
    this->points[10] = point4(2.0, -2.0, -2.0, 1.0);
    this->points[11] = point4(-2.0, -2.0, -2.0, 1.0);

    this->normals[0] = vec3(0.0, 0.0, 1.0);
    this->normals[1] = vec3(0.0, 0.0, 1.0);
    this->normals[2] = vec3(0.0, 0.0, 1.0);
    this->normals[3] = vec3(0.0, 0.0, 1.0);
    this->normals[4] = vec3(0.0, 0.0, 1.0);
    this->normals[5] = vec3(0.0, 0.0, 1.0);
    this->normals[6] = vec3(0.0, 0.0, -1.0);
    this->normals[7] = vec3(0.0, 0.0, -1.0);
    this->normals[8] = vec3(0.0, 0.0, -1.0);
    this->normals[9] = vec3(0.0, 0.0, -1.0);
    this->normals[10] = vec3(0.0, 0.0, -1.0);
    this->normals[11] = vec3(0.0, 0.0, -1.0);
}