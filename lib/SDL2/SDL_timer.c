#include "SDL_timer.h"

#include <libdragon.h>

void SDL_Delay(Uint32 ms) {
}

Uint32 SDL_GetTicks(void) {
	return get_ticks_ms();
}
