#include "SDL_surface.h"

#include <libdragon.h>

SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask,
								  Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	SDL_Surface *surface = malloc(sizeof(SDL_Surface));
	surface->pixels = malloc(width * height * depth);
	surface->format = malloc(sizeof(SDL_PixelFormat));
	surface->format->BitsPerPixel = depth;
	return surface;
}

void SDL_FreeSurface(SDL_Surface *surface) {
	free(surface->pixels);
	free(surface->format);
	free(surface);
}

int SDL_SaveBMP_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst) {
	return 0;
}

int SDL_FillRect(SDL_Surface *dst, const SDL_Rect *rect, Uint32 color) {
	SDL_Rect r;
	if (rect) {
		r = *rect;
	} else {
		r.x = 0;
		r.y = 0;
		r.w = dst->w;
		r.h = dst->h;
	}

	for (size_t x = 0; x < r.x; ++x) {
		for (size_t y = 0; y < r.y; ++y) {
			// dst->pixels[x + (y * r.w)] = color;
		}
	}

	return 0;
}

int SDL_UpperBlit(SDL_Surface *src, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
	
}
