#pragma once

#include <string>
#include <SDL.h>
#include <iostream>
//#include <SDL_mixer.h>
#include <vector>

#define GL3_PROTOTYPES 1
#include <GL/glew.h>

class Fenetre {
public:
	Fenetre(const std::string &title, int width, int height);

	~Fenetre();

	void clear() const;
	void pollEvents(SDL_Event &event);

	inline bool isClosed() const { return _closed; }

	//inline void set_sound(Mix_Chunk *sound_effect) {_sound_effect.push_back(sound_effect);}

private:
	bool init();

private:
	std::string _title;
	int _width = 800;
	int _height = 600;
	//std::vector<Mix_Chunk*> _sound_effect;

	bool _closed = false;

	
	SDL_GLContext contexteOpenGL;

public:
	static SDL_Window *_window;
};
