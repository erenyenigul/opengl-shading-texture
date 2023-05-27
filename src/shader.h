#ifndef SHADER_H
#define SHADER_H

// fragment shading of sphere model
#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include <iostream>
#include <vector>
#include <math.h>

#include "scene.h"
#include "Angel.h"

struct Shader{
		const char *filename;
		GLuint program;
	
		Shader(const char *vShaderFile, const char *fShaderFile)
		{
			program = initShader(vShaderFile, fShaderFile);
		}

		void switchProgram(const char *vShaderFile, const char *fShaderFile)
		{
			program = initShader(vShaderFile, fShaderFile);
		}

		void use()
		{
			glUseProgram(program);
		}

		GLuint getUniformLocation(const char *name)
		{
			return glGetUniformLocation(program, name);
		}

        GLuint getAttribLocation(const char *name)
		{
			return glGetAttribLocation(program, name);
		}

		void setUniform4fv(const char *name, const vec4 &v)
		{
			glUniform4fv(getUniformLocation(name), 1, v);
		}

		void setUniformMatrix4fv(const char *name, const mat4 &m)
		{
			glUniformMatrix4fv(getUniformLocation(name), 1, GL_TRUE, m);
		}

		void setUniform1f(const char *name, const float &f)
		{
			glUniform1f(getUniformLocation(name), f);
		}
};

#endif SHADER_H