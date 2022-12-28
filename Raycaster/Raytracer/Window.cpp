//#include "stdafx.h"
#include "Window.h"

SDL_Renderer* Window::renderer = nullptr;

Window::Window(const std::string& title, int width, int height) :
	_title(title), _width(width), _height(height)
{
	_closed = !init();
}

Window::~Window()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

bool Window::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Failed to initialise SDL.\n";
		return 0;
	}

	_window = SDL_CreateWindow(
		_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_width, _height,
		0
	);

	if (_window == nullptr)
	{
		std::cerr << "Failed to create window.\n";
		return 0;
	}

	renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == nullptr)
	{
		std::cerr << "Failed to create renderer.\n";
		return 0;
	}

	return true;
}

void Window::pollEvents(SDL_Event& event)
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

void Window::clear() const {
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 77, 128, 153, 255);
	SDL_RenderClear(renderer);
}

void Window::setPixelColor(glm::vec2 pos, glm::vec3 color)
{
	setPixelColor(pos, glm::vec4(color, 255.0));
}

void Window::setPixelColor(glm::vec2 pos, glm::vec4 color)
{
	SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
	SDL_RenderDrawPoint(renderer, pos[0], pos[1]);
}

void Window::setPixelColor(glm::vec2 pos, Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
	SDL_RenderDrawPoint(renderer, pos[0], pos[1]);
}