#pragma once
 
#include <GL/glew.h>
#include <SDL.h>
 
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
 
class Shader
{
public:

	Shader(const std::string &vert, const std::string &frag);

	~Shader();

	bool LoadVertexShader(const std::string &filename);
	bool LoadFragmentShader(const std::string &filename);

	bool LinkShaders();

	inline bool getInit(){return initialise;}

	inline GLuint getShader(){return shaderProgram;}

private:
	
	void PrintErrorInfo(int32_t shaderId);

	std::string ReadFile(const char* file);
 
	// The handles to the induvidual shader
	GLuint vertexShader, fragmentShader;

	bool initialise;

	// The handle to our shader program
	GLuint shaderProgram;
 
};
