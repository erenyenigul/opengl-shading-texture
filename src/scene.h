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

struct Material
{
    color4 ambient;
    color4 diffuse;
    color4 specular;
    float shininess;
};

class Camera
{
public:
    Camera(Shader *shader) : shader{shader},
                             projection{mat4(1.0)},
                             modelView{mat4(1.0)}
    {
        configGl();
    }

    void lookAt(vec4 eye, vec4 at, vec4 up)
    {
        this->eye = eye;
        this->at = at;
        this->up = up;

        configGl();
    }

    void setProjection(mat4 p)
    {
        this->projection = p;
        configGl();
    }

    void configGl()
    {
        modelView = LookAt(eye, at, up);

        shader->setUniformMatrix4fv("ModelView", modelView);
        shader->setUniformMatrix4fv("Projection", projection);
    }

    void setEye(vec4 e)
    {
        this->eye = e;
        configGl();
    }

    void setAt(vec4 a)
    {
        this->at = a;
        configGl();
    }

    void setUp(vec4 u)
    {
        this->up = u;
        configGl();
    }

    vec4 getEye()
    {
        return this->eye;
    }

    vec4 getAt()
    {
        return this->at;
    }

private:
    Shader *shader;
    mat4 modelView;
    mat4 projection;

    vec4 eye;
    vec4 at;
    vec4 up;
};

class Light
{

public:
    Light(Shader *s)
    {
        shader = s;
    }

    Light(Shader *s, vec4 p, color4 a, color4 d, color4 sp)
    {
        shader = s;
        position = p;
        ambient = a;
        diffuse = d;
        specular = sp;

        configGl();
    }

    void configGl()
    {
        shader->setUniform4fv("LightPosition", position);
        shader->setUniform4fv("AmbientLight", ambient * ambientEnabled);
        shader->setUniform4fv("DiffuseLight", diffuse * diffuseEnabled);
        shader->setUniform4fv("SpecularLight", specular * specularEnabled);
    }

    void setPosition(vec4 p)
    {
        position = p;
        configGl();
    }

    void setAmbient(color4 a)
    {
        ambient = a;
        configGl();
    }

    void setDiffuse(color4 d)
    {
        diffuse = d;
        configGl();
    }

    void setSpecular(color4 s)
    {
        specular = s;
        configGl();
    }

    vec4 getPosition()
    {
        return position;
    }

    void disableAmbient()
    {
        ambientEnabled = false;
        configGl();
    }

    void enableAmbient()
    {
        ambientEnabled = true;
        configGl();
    }

    void disableDiffuse()
    {
        diffuseEnabled = false;
        configGl();
    }

    void enableDiffuse()
    {
        diffuseEnabled = true;
        configGl();
    }

    void disableSpecular()
    {
        specularEnabled = false;
        configGl();
    }

    void enableSpecular()
    {
        specularEnabled = true;
        configGl();
    }

private:
    color4 ambient;
    color4 diffuse;
    color4 specular;
    vec4 position;
    Shader *shader;

    bool ambientEnabled = true;
    bool diffuseEnabled = true;
    bool specularEnabled = true;
};

class SceneObject
{

public:
    SceneObject(vec4 position, Shader &shader, int numVertices, float scale);

    void transform(mat4 transformation);
    void update();
    void display();
    void revert();
    void switchColor();
    void configGl();

    void setMaterial(Material m);
    Material &getMaterial();
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
    vec2 *texCoords;

    bool isRed = true;
    int numVertices = 0;

    virtual void form();

    mat4 transformation;
    mat4 initialTransformation;
    float scale;

    Material material;

private:
    vec4 initialPosition;
    vec4 initialSpeed;
};

class Ball : public SceneObject
{

public:
    Ball(vec4 position, Shader &shader, float scale);

private:
    void form();

    int Index = 0;

    void tetrahedron(int count);
    void divide_triangle(const point4 &a, const point4 &b,
                         const point4 &c, int count);
    void triangle(const point4 &a, const point4 &b, const point4 &c);
    point4 unit(const point4 &p);
};

class Floor : public SceneObject
{
public:
    Floor(vec4 position, Shader &shader, float scale);

private:
    void form();
};


#endif