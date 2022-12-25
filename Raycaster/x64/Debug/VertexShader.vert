#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor; 
layout (location = 2) in vec2 aTexCoord;

uniform sampler2D ourTexture;
uniform int type;

out vec4 ourColor; 
out vec2 TexCoord;


void main()
{

	float _x = 0;
	float _y = 0;

	_x = aPos.x;
	_y = aPos.y;

    	gl_Position = vec4(_x, _y, aPos.z, 1.0);
    	ourColor = aColor; 
	TexCoord = aTexCoord;
}  
