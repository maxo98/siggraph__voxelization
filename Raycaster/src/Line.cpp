#include "Line.h"

Line::Line(const GLfloat _vertex[2][3], const GLfloat _colors[4]){

	glGenBuffers(2, vbo);//On génère 2 Buffers et un objet VertexArrays
	glGenVertexArrays(1, &vao);

	setVertex(_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//Configuration de l'amplacement des données
	glEnableVertexAttribArray(0);

	setColors(_colors);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}

Line::~Line(){//Destuction des objets alloués
	
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, &vao);
}

void Line::setVertex(const GLfloat _vertex[2][3]){
	unsigned char i,cpt;	

	for(i = 0; i < 2; i++)
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

void Line::setColors(const GLfloat _colors[4]){

	unsigned char i,cpt;	
	float stack[2][4];


	for(i = 0; i < 2; i++)
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


void Line::renderFill()
{	
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 2);	
}
