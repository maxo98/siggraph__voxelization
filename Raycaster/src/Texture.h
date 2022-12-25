#pragma once

#include "stb_image.h"
#include <string>
#include <iostream>

#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL.h>

class Texture{
public:
	Texture(const std::string &file);

	inline ~Texture(){stbi_image_free(data); glDeleteBuffers(1, &vbo); glDeleteTextures(1,  &texture);}

	inline void getTexture(GLuint &_texture, GLuint &_buffer)const{ _texture = texture; _buffer = vbo;}
private:
	int width, height, nrChannels;
	unsigned char *data;
	GLuint texture, vbo = 0;
};
