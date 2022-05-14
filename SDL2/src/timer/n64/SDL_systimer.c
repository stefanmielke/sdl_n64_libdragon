/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#ifdef SDL_TIMER_N64

#include <libdragon.h>

static SDL_bool ticks_started = SDL_FALSE;

void
SDL_TicksInit(void)
{
    if (ticks_started) {
        return;
    }
    ticks_started = SDL_TRUE;

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
	rdp_init();

    dfs_init(0xB0201000);

    debug_init_isviewer();
    timer_init();
}

void
SDL_TicksQuit(void)
{
    fprintf(stderr, "SDL_TicksQuit\n");
    ticks_started = SDL_FALSE;

    timer_close();
}

Uint64
SDL_GetTicks64(void)
{
    if (!ticks_started) {
        SDL_TicksInit();
    }

    return get_ticks();
}

Uint64
SDL_GetPerformanceCounter(void)
{
    return SDL_GetTicks64();
}

Uint64
SDL_GetPerformanceFrequency(void)
{
    return 1000;
}

void SDL_Delay(Uint32 ms)
{
    // do not delay?
    // the N64 operates at 60hz or 50hz varying on the region, no need to lock the thread
}

#endif /* SDL_TIMER_N64 */

/* vim: ts=4 sw=4
 */
