#pragma once

#include <string>
#include <SDL.h>
//#include <SDL_image.h>
#include <iostream>
#include "Window.h"
#include "Shader.h"
#include "Texture.h"


#define GL3_PROTOTYPES 1
#include <GL/glew.h>

class Quadrilatere{
public:
	Quadrilatere(const GLfloat _vertex[4][3], const GLfloat _colors[4]);
	Quadrilatere(const GLfloat _vertex[4][3], const Texture &obj_texture);

	~Quadrilatere();

	void setVertex(const GLfloat _vertex[4][3]);
	void setColors(const GLfloat _colors[4]);
	void setTexture(const Texture &obj_texture);

	inline void* getVertex(){return &*(vertex);}
	//inline float* getColors(){return colors;}

	void renderFill();
	

private:

	GLfloat vertex[4][3];
	GLfloat colors[4];
	GLuint vbo[2], vao, vbo_texture;
	unsigned int texture = 0;
};
