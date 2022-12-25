#version 330 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform int type;
  
void main()
{
	if(type == 0)
	{
    		FragColor = ourColor;
	}else{
		FragColor = texture(ourTexture, TexCoord);
	}
}
