#include "Shader.h"
#include "Window.h"
#include "Quadrilatere.h"
#include "Line.h"

#include <math.h>

#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL.h>
 
#define PI 3.14159265

#define PROGRAM_NAME "Raycaster V1.0"

#define mapWidth 24
#define mapHeight 24
#define mapDepth 3

#define windowWidth 1920
#define windowHeight 1080

//const int worldMap[mapWidth][mapHeight][mapDepth]=
//{
//  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
//  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
//  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
//};

int worldMap[mapWidth][mapHeight][mapDepth] = {0};

const GLfloat bot[5][3] = {
	{ -1.0,  -1.0, 0.0}, 
	{  1.0,  -1.0, 0.0}, 
	{  1.0, 0.0, 0.0},
	{  -1.0, 0.0, 0.0}
};

const GLfloat top[5][3] = {
	{ -1.0,  0.0, 0.0}, 
	{  1.0,  0.0, 0.0}, 
	{  1.0, 1.0, 0.0},
	{  -1.0, 1.0, 0.0}
};

const GLfloat rouge[4] = { 1.0f, 0.0, 0.0, 1.0 };
const GLfloat ombreRouge[4] = { 0.7, 0.0, 0.0, 1.0 };

const GLfloat vert[4] = { 0.0, 1.0, 0.0, 1.0};
const GLfloat ombreVert[4] = { 0.0, 0.7, 0.0, 1.0 };

const GLfloat bleu[4] = { 0.0, 0.0, 1.0, 1.0 };
const GLfloat ombreBleu[4] = { 0.0, 0.0, 0.7, 1.0 };

const GLfloat ciel[4] = { 0.3, 0.5, 0.6, 1.0 };
const GLfloat noir[4] = { 0.0, 0.0, 0.0, 1.0 };

void pollEvents(Fenetre &_fenetre, SDL_Event &_keyboard, int &_mouse) {//Input
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		_fenetre.pollEvents(event);
		_keyboard = event;
		
		if(event.type == SDL_MOUSEMOTION)
		{
			_mouse = event.motion.x;
		}
	}
}



int main(int argc, char *argv[])
{
	Fenetre fenetre("Raycaster V1.0", windowWidth, windowHeight);

	int timer = 0;
	int i, cpt;

	int uniformLocation;

	SDL_Event keyboard;///////////Variables pour la lecture d'entré
	int lastMouse = 0;
	int mouse = 0;
	double motion_x = 0;

	float mouvSpeed = 0.3;

	float wall_vertex[2][3] = {//////////////////Varaibles pour le rendu
				{0,0,0},
				{0,0,0},
				};
	
	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapWidth; y++)
		{
			for (int z = 0; z < mapDepth; z++)
			{
				worldMap[x][y][z] = 0;
			}
		}
	}


	for (int x = 8; x < 10; x++)
	{
		for (int y = 8; y < 10; y++)
		{
			for (int z = 0; z < mapDepth; z++)
			{
				worldMap[x][y][z] = 1;
			}
		}
	}

	double posX = 22, posY = 12;  //x and y start position
	double dirX = -1, dirY = 0; //initial direction vector
	double planeX = 0.0, planeY = 0.66; //the 2d raycaster version of camera plane

	double oldDirX;
	double oldPlaneX;

	double cameraX;//Position sur le plan de la camera
	double rayDirX;//Direction du rayon
	double rayDirY;

	int mapX;//Case dans laquelle le rayon vient d'entré
	int mapY;

	//Dans quel direction se diriger pour chaque axe (soit +1, soit -1)
	int stepX;
	int stepY;

	//Longueur du rayon jusqu'à la prochaine bordure X et Y
	double sideDistX;
	double sideDistY;

	//Distance d'un coté X ou Y vers un coté respectivement X ou Y
	double perpWallDist;
	double deltaDistX;
      	double deltaDistY;

	bool hit; //was there a wall hit?
	int side; //coté du mur touché Nord/Sud ou Est/Ouest

	double lineHeight;//longueur de la ligne à afficher

	double drawStart;//Position de la ligne
	double drawEnd;

	Shader shader("E:\\Cedric\\ESGI\\Raycaster\\x64\\Debug\\VertexShader.vert","E:\\Cedric\\ESGI\\Raycaster\\x64\\Debug\\FragShader1.frag");////////////Shader

	// Set up shader ( will be covered in the next part )
	// ===================
	if (!shader.getInit())
		return false;

	glUseProgram(shader.getShader());

	uniformLocation = glGetUniformLocation(shader.getShader(), "type");
	glUniform1i(uniformLocation, 0);

	Quadrilatere sol(bot, noir);//////////Objets
	Line wall(top, noir);


	SDL_ShowCursor(SDL_DISABLE);////////Options fenêtre SDL
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_SetWindowGrab(Fenetre::_window,SDL_TRUE);
	SDL_WarpMouseInWindow(Fenetre::_window,windowWidth/2,windowHeight/2);

	glClearColor(0.3, 0.5, 0.6, 1.0);//////Premier affichage (fond couleur)
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(Fenetre::_window);


	while (!fenetre.isClosed())///////////////////////////////////////////////////////////////////////////////Main loop
	{
		pollEvents(fenetre, keyboard, mouse);///////////////////////Entré Clavier
		if(SDLK_b && keyboard.type == SDL_KEYDOWN)
		{
			switch(keyboard.key.keysym.sym)
			{
				case SDLK_z:
					if(worldMap[int(posX + dirX * mouvSpeed)][int(posY)][0] == false) posX += dirX * mouvSpeed;
	      			if(worldMap[int(posX)][int(posY + dirY * mouvSpeed)][0] == false) posY += dirY * mouvSpeed;
					break;
				case SDLK_q:
					if(worldMap[int(posX - planeX * mouvSpeed)][int(posY)][0] == false) posX -= planeX * mouvSpeed;
	      			if(worldMap[int(posX)][int(posY - planeY * mouvSpeed)][0] == false) posY -= planeY * mouvSpeed;
					break;
				case SDLK_s:
					if(worldMap[int(posX - dirX * mouvSpeed)][int(posY)][0] == false) posX -= dirX * mouvSpeed;
      				if(worldMap[int(posX)][int(posY - dirY * mouvSpeed)][0] == false) posY -= dirY * mouvSpeed;
					break;
				case SDLK_d:
					if(worldMap[int(posX + planeX * mouvSpeed)][int(posY)][0] == false) posX += planeX * mouvSpeed;
	      			if(worldMap[int(posX)][int(posY + planeY * mouvSpeed)][0] == false) posY += planeY * mouvSpeed;
					break;
			}
		}

		if(timer + 42 < SDL_GetTicks())////////////////////Affichage
		{

			glClear(GL_COLOR_BUFFER_BIT);
			motion_x = 0;

			if(lastMouse != mouse)//////Rotation Camera
			{
				motion_x = (windowWidth/2 -mouse)/(windowWidth/(2*PI));
				lastMouse = mouse;

				oldDirX = dirX;
			    dirX = dirX * cos(motion_x) - dirY * sin(motion_x);
			    dirY = oldDirX * sin(motion_x) + dirY * cos(motion_x);
			    oldPlaneX = planeX;
			    planeX = planeX * cos(motion_x) - planeY * sin(motion_x);
			    planeY = oldPlaneX * sin(motion_x) + planeY * cos(motion_x);


				SDL_WarpMouseInWindow(Fenetre::_window,windowWidth/2,windowHeight/2);
				
			}

			sol.renderFill();
			for(int x = 0; x < windowWidth; x++)/////////////////////////Raycasting
			{
				//Calcul la position et direction du rayon
			    cameraX = (2*x) / double(windowWidth) -1; //Position x sur le plan de la camera
			    rayDirX = dirX + planeX * cameraX;
			    rayDirY = dirY + planeY * cameraX;

				//Dans quel case de la carte on se trouve
			    mapX = int(posX);
			    mapY = int(posY);

			    //Distance d'un coté X ou Y vers un coté respectivement X ou Y
			    deltaDistX = fabs(1 / rayDirX);
			    deltaDistY = fabs(1 / rayDirY);

			    hit = false; //was there a wall hit?

				//calculate step and initial sideDist
				if (rayDirX < 0)
				{
					stepX = -1;
					sideDistX = (posX - mapX) * deltaDistX;
				}
				else
				{
					stepX = 1;
					sideDistX = (mapX + 1.0 - posX) * deltaDistX;
				}
				if (rayDirY < 0)
				{
					stepY = -1;
					sideDistY = (posY - mapY) * deltaDistY;
				}
				else
				{
					stepY = 1;
					sideDistY = (mapY + 1.0 - posY) * deltaDistY;
				}

			    //Test case par case
			    while (hit == false && mapX < mapWidth && mapX >= 0 && mapY < mapHeight && mapY >= 0)
			    {
					//jump to next map square, OR in x-direction, OR in y-direction
					if (sideDistX < sideDistY)
					{
				 		sideDistX += deltaDistX;
						mapX += stepX;
						side = 0;
					}
					else
					{
				  		sideDistY += deltaDistY;
				 		mapY += stepY;
				  		side = 1;
					}
					//Check if ray has hit a wall
					if (worldMap[mapX][mapY][0] > 0) hit = true;
			    }

				if (mapX >= mapWidth || mapX < 0 || mapY >= mapHeight || mapY < 0) continue;

				//Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
      			if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
      			else           perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

				//Calcul la longueur de la ligne à affciher
			    lineHeight = int(windowHeight / perpWallDist);

			    //Calcul des positions Y de la ligne
			    drawStart = -lineHeight/2 + windowHeight / 2;
			    if(drawStart < 0)drawStart = 0;

			    drawEnd = lineHeight/2 + windowHeight / 2;
			    if(drawEnd >= windowHeight)drawEnd = windowHeight - 1;

				drawStart = drawStart/(windowHeight/2) -1;
				drawEnd = drawEnd/(windowHeight/2) -1;


				if (side == 1) {//////////Couleur

					
				      	switch(worldMap[mapX][mapY][0])
				      	{
						case 1:  wall.setColors(rouge);  break; 
						case 2:  wall.setColors(bleu);  break; 
						case 3:  wall.setColors(bleu);   break; 
						case 4:  wall.setColors(vert);  break; 
						default: wall.setColors(vert); break; 
				      	}
				}else{

					switch(worldMap[mapX][mapY][0])
				      	{
						case 1:  wall.setColors(ombreRouge);  break; 
						case 2:  wall.setColors(ombreBleu);  break; 
						case 3:  wall.setColors(ombreBleu);   break; 
						case 4:  wall.setColors(ombreVert);  break; 
						default: wall.setColors(ombreVert); break; 
				      	}

				}

				wall_vertex[0][0] = cameraX;
				wall_vertex[1][0] = cameraX;

				wall_vertex[0][1] = drawEnd;
				wall_vertex[1][1] = drawStart;
				

				wall.setVertex(wall_vertex);
				
				wall.renderFill();//Affichage de la ligne
				
			}

			timer = SDL_GetTicks();
			
			SDL_GL_SwapWindow(Fenetre::_window);//Affichage du buffer
		}
	}

	return 0;
}



