#pragma once

#include "SDL.h"
#include <libdragon.h>

struct SDL_Window {
	int w;
	int h;
	int bpp;
	int resolution;
};

struct SDL_Renderer {
	display_context_t disp;
	SDL_Window *window;
};

struct SDL_Texture {
	sprite_t *sprite;
};

#define printf debugf
