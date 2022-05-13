#include "SDL_video.h"

#include <libdragon.h>

#include "SDL_n64.h"

static SDL_Window internal_window;

SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
	int ld_resolution = RESOLUTION_320x240;
	const int ld_bpp = DEPTH_16_BPP;
	const int ld_buffers = 2;

	if (w > 512) {
		w = 640;
		if (h > 240) {
			h = 480;
			ld_resolution = RESOLUTION_640x480;
		} else {
			h = 240;
			ld_resolution = RESOLUTION_640x240;
		}
	} else if (w > 320) {
		w = 512;
		if (h > 240) {
			ld_resolution = RESOLUTION_512x480;
		} else {
			ld_resolution = RESOLUTION_512x240;
		}
	} else {
		h = 240;
		if (w > 256) {
			w = 320;
			ld_resolution = RESOLUTION_320x240;
		} else {
			w = 256;
			ld_resolution = RESOLUTION_256x240;
		}
	}

	display_init(ld_resolution, ld_bpp, ld_buffers, GAMMA_NONE, ANTIALIAS_RESAMPLE);

	internal_window.w = w;
	internal_window.h = h;
	internal_window.bpp = ld_bpp;
	internal_window.resolution = ld_resolution;
	return &internal_window;
}

// we do not destroy windows
void SDL_DestroyWindow(SDL_Window *window) {
}

// windows is always shown
void SDL_ShowWindow(SDL_Window *window) {
}

int SDL_GetWindowDisplayIndex(SDL_Window *window) {
	return 0;
}

int SDL_GetDisplayBounds(int displayIndex, SDL_Rect *rect) {
	rect->x = 0;
	rect->y = 0;
	rect->w = internal_window.w;
	rect->h = internal_window.h;

	return 0;
}

int SDL_SetWindowFullscreen(SDL_Window *window, Uint32 flags) {
	return 0;
}

int SDL_GetNumVideoDisplays(void) {
	return 1;
}

void SDL_SetWindowPosition(SDL_Window *window, int x, int y) {
}

void SDL_SetWindowSize(SDL_Window *window, int w, int h) {
}

void SDL_GetWindowSize(SDL_Window *window, int *w, int *h) {
	*w = window->w;
	*h = window->h;
}
