#include "SDL_video.h"

#include <libdragon.h>

struct SDL_Window {
} internal_window;

SDL_Window *SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
	int ld_resolution = RESOLUTION_320x240;
	const int ld_bpp = DEPTH_16_BPP;
	const int ld_buffers = 2;

	if (w > 512) {
		ld_resolution = h > 240 ? RESOLUTION_640x480 : RESOLUTION_640x240;
	} else if (w > 320) {
		ld_resolution = h > 240 ? RESOLUTION_512x480 : RESOLUTION_512x240;
	} else {
		ld_resolution = w > 256 ? RESOLUTION_320x240 : RESOLUTION_256x240;
	}

	display_init(ld_resolution, ld_bpp, ld_buffers, GAMMA_NONE, ANTIALIAS_RESAMPLE);

	return &internal_window;
}

void SDL_DestroyWindow(SDL_Window *window) {
}
