#pragma once

#include <string>
#include <SDL.h>
//#include <SDL_image.h>
#include <iostream>
//#include <SDL_ttf.h>
//#include <SDL_mixer.h>
#include <vector>
#include <glm.hpp>

class Window {
public:
	Window(const std::string& title, int width, int height);

	~Window();

	void clear() const;
	void pollEvents(SDL_Event& event);

	inline bool isClosed() const { return _closed; }

	void setPixelColor(glm::vec2 pos, glm::vec3 color);
	void setPixelColor(glm::vec2 pos, glm::vec4 color);

	inline SDL_Window* getWindow() { return _window; };

	//inline void set_police(TTF_Font *police) { _police = police; }
	//inline void set_sound(Mix_Chunk *sound_effect) {_sound_effect.push_back(sound_effect);}

private:
	bool init();

private:
	std::string _title;
	int _width = 800;
	int _height = 600;
	//TTF_Font *_police = nullptr;
	//std::vector<Mix_Chunk*> _sound_effect;

	bool _closed = false;

	SDL_Window* _window = nullptr;

public:
	static SDL_Renderer* renderer;
};