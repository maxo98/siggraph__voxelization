#include "Quadrilatere.h"

Quadrilatere::Quadrilatere(const GLfloat _vertex[][3], const GLfloat _colors[4]){

	glGenBuffers(2, vbo);//On génère 2 Buffers et un objet VertexArrays
	glGenVertexArrays(1, &vao);

	setVertex(_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//Configuration de l'amplacement des données
	glEnableVertexAttribArray(0);

	setColors(_colors);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}

Quadrilatere::Quadrilatere(const GLfloat _vertex[][3], const Texture &obj_texture){

	glGenBuffers(1, vbo);//On génère 2 Buffers et un objet VertexArrays
	glGenVertexArrays(1, &vao);

	setVertex(_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//Configuration de l'amplacement des données
	glEnableVertexAttribArray(0);

	setTexture(obj_texture);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

}

Quadrilatere::~Quadrilatere(){//Destuction des objets alloués
	
	if(texture != 0)
		glDeleteBuffers(1, vbo);
	else
		glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, &vao);
}

void Quadrilatere::setVertex(const GLfloat _vertex[][3]){
	unsigned char i,cpt;	

	for(i = 0; i < 4; i++)
	{
		for(cpt = 0; cpt < 3; cpt++)
		{
			vertex[i][cpt] = _vertex[i][cpt];
		}
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_DYNAMIC_DRAW);//Copie des données dans le buffer

}

void Quadrilatere::setColors(const GLfloat _colors[4]){

	if(texture != 0)
		return;	

	unsigned char i,cpt;	
	float stack[4][4];


	for(i = 0; i < 4; i++)
	{
		for(cpt = 0; cpt < 4; cpt++)	
		{		
			stack[i][cpt] = _colors[cpt];
			if(i == 1)
				colors[cpt] = _colors[cpt];
		}
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(stack), stack, GL_DYNAMIC_DRAW);//Copie des données dans le buffer	
}

void Quadrilatere::setTexture(const Texture &obj_texture){
	

	obj_texture.getTexture(texture, vbo_texture);

	if(vbo_texture == 0)
	{
		return;
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);


}

void Quadrilatere::renderFill()
{	
	if(texture != 0)
		glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);	
}
