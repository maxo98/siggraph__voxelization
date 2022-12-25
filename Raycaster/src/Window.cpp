#include "Window.h"

SDL_Window *Fenetre::_window = nullptr;

Fenetre::Fenetre(const std::string &title, int width, int height) :
	_title(title), _width(width), _height(height)
{
	_closed = !init();
}

Fenetre::~Fenetre()
{
	SDL_GL_DeleteContext(contexteOpenGL);
	SDL_DestroyWindow(_window);
	//if (_sound_effect.size() != 0) {
	//	for (unsigned char i = 0; i < _sound_effect.size(); i++)
	//	{
	//		Mix_FreeChunk(_sound_effect[i]);
	//	}
	//}
	//Mix_CloseAudio();
	SDL_Quit();
}

bool Fenetre::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Failed to initialise SDL.\n";
		return 0;
	}

	// Set our OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//Double Buffer

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	//{
	//	return false;
	//}

	_window = SDL_CreateWindow(
		_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_width, _height,
		SDL_WINDOW_OPENGL
	);

	if (_window == nullptr)
	{
		std::cerr << "Failed to create window.\n";
		return 0;
	}

	// Création du contexte OpenGL

    	contexteOpenGL = SDL_GL_CreateContext(_window);

	if(contexteOpenGL == 0)

	{

		std::cout << SDL_GetError() << std::endl;

		return 0;
	}


	//Initialisation de glew
	glewExperimental = GL_TRUE; 
	glewInit();

	SDL_GL_SetSwapInterval(1);//Configure la mise à jour de l'image pour qu'elle soit synchronisé avec le retour vertical

	return true;
}

void Fenetre::pollEvents(SDL_Event &event)
{

	switch (event.type) {
	case SDL_QUIT:
		_closed = true;
		break;

	case SDL_KEYDOWN:
		//std::cout << "a button was clicked on the keyboard.\n";
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
			_closed = true;
			break;
		}
		break;

	default:
		break;
	}
}

void Fenetre::clear() const {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}
