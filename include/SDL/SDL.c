#include "SDL.h"

#include <libdragon.h>

bool gen_event_flag = false;

int SDL_Init(Uint32 flags) {
	if (flags & SDL_INIT_VIDEO) {
		display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
		rdp_init();
	}
	if (flags & SDL_INIT_AUDIO) {
		audio_init(44100, 4);
		mixer_init(32);
	}
	if (flags & SDL_INIT_TIMER) {
		timer_init();
	}
	if (flags & SDL_INIT_JOYSTICK) {
		controller_init();
	}

	// always init DFS
	dfs_init(DFS_DEFAULT_LOCATION);
	debug_init_isviewer();

	return 0;
}

uint32_t SDL_WasInit(uint32_t flags) {
	return 0;
}

void SDL_Quit(void) {
}
