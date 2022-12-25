#include "Shader.h"

Shader::Shader(const std::string &vert, const std::string &frag)
{
	shaderProgram = glCreateProgram();
		
	//glBindAttribLocation(shaderProgram, 0, "in_Position");
	//glBindAttribLocation(shaderProgram, 1, "in_Color");

	if (!LoadVertexShader(vert))
	{
		std::cerr<< "Impossible de charger "<< vert <<".\n";
		initialise = false;
		return;
	}

	if (!LoadFragmentShader(frag))
	{
		std::cerr<< "Impossible de charger "<< frag <<".\n";
		initialise = false;
		return;
	}

	initialise = LinkShaders();
}

Shader::~Shader()
{
	/* Cleanup all the things we bound and allocated */
	glUseProgram(0);
	glDetachShader(shaderProgram,vertexShader);
	glDetachShader(shaderProgram,fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}

bool Shader::LoadVertexShader(const std::string &filename)
{
	std::cout << "Loading Vertex shader" << std::endl;
 
	std::string str = ReadFile(filename.c_str());//Lecture du fichier
 
	char* src = const_cast<char*>( str.c_str());
	int32_t size = str.length();

	unsigned int old_shader = vertexShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);//Creation d'un nouveau shader

	glShaderSource(vertexShader, 1, &src, &size);

	glCompileShader(vertexShader);

	int wasCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &wasCompiled );

	if (wasCompiled == 0)
	{
		std::cout << "=======================================\n";
		std::cout << "Shader compilation failed : " << std::endl;
		PrintErrorInfo(vertexShader);
		return false;
	}

	glDetachShader(shaderProgram,old_shader);//Deliement de l'ancien shader et liement du nouveau
	glAttachShader(shaderProgram, vertexShader);
	glDeleteShader(old_shader);

	return true;
}

bool Shader::LoadFragmentShader(const std::string &filename)
{
	std::cout << "Loading Fragment Shader" << std::endl;

	std::string str = ReadFile(filename.c_str());//Lecture du fichier
 
	char* src = const_cast<char*>( str.c_str());
	int32_t size = str.length();
		
	unsigned int old_shader = fragmentShader;

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//Creation d'un nouveau shader
 
	glShaderSource(fragmentShader, 1, &src, &size);
 
	glCompileShader(fragmentShader);
 
	int wasCompiled = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &wasCompiled );
 
	if (wasCompiled == false)
	{
		std::cout << "=======================================\n";
		std::cout << "Shader compilation failed : " << std::endl;
		PrintErrorInfo(fragmentShader);
		return false;
	}

	glDetachShader(shaderProgram,old_shader);//Deliement de l'ancien shader et liement du nouveau
	glAttachShader(shaderProgram, fragmentShader);
	glDeleteShader(old_shader);

	return true;
}

std::string Shader::ReadFile(const char* file)//Lecture du fichier
{
	std::ifstream t(file);
 
	std::stringstream buffer;
	buffer << t.rdbuf();
 
	std::string fileContent = buffer.str();
 
	return fileContent;
}

bool Shader::LinkShaders()//Lien et compilation du programme finale
{
	glLinkProgram(shaderProgram);
 
	int isLinked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);

	if (isLinked == false)
	{
		std::cout << "=======================================\n";
		std::cout << "Shader linking failed : " << std::endl;
		PrintErrorInfo(shaderProgram);
	}

	return isLinked != 0;
}

void Shader::PrintErrorInfo(int32_t shaderId)//Affichage de l'erreur
{
	GLint maxLength = 0;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

	char* shaderInfoLog = new char[maxLength];
	glGetShaderInfoLog(shaderId, maxLength, &maxLength, shaderInfoLog );
 
	std::cout << "\tError info : " << shaderInfoLog << std::endl;
 
	std::cout << "=======================================\n\n";
	delete[] shaderInfoLog;
}
