#include "SDL_video.h"

#include "SDL_surface.h"

#include <libdragon.h>

extern bool gen_event_flag;

SDL_Surface internal_surface;
static uint32_t render_color;

static bool first_draw = true;

SDL_Surface *SDL_SetVideoMode(int w, int h, int bpp, Uint32 flags) {
	internal_surface.hwdata = malloc(sizeof(private_hwdata));
	internal_surface.hwdata->type = ST_DISPLAY;
	internal_surface.hwdata->data.disp = 0;

	int ld_resolution = RESOLUTION_320x240;
	const int ld_bpp = bpp > 16 ? DEPTH_32_BPP : DEPTH_16_BPP;
	const int ld_buffers = 2;

	if (w > 512) {
		ld_resolution = h > 240 ? RESOLUTION_640x480 : RESOLUTION_640x240;
	} else if (w > 320) {
		ld_resolution = h > 240 ? RESOLUTION_512x480 : RESOLUTION_512x240;
	} else {
		ld_resolution = w > 256 ? RESOLUTION_320x240 : RESOLUTION_256x240;
	}

	display_init(ld_resolution, ld_bpp, ld_buffers, GAMMA_NONE, ANTIALIAS_RESAMPLE);

	render_color = graphics_make_color(110, 0, 0, 255);

	return &internal_surface;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
}

int SDL_Flip(SDL_Surface *screen) {
	if (!screen || screen->hwdata->type != ST_DISPLAY)
		return -1;

	if (first_draw) {
		first_draw = false;
	} else {
		display_show(screen->hwdata->data.disp);

		gen_event_flag = false;
	}

	while (!(screen->hwdata->data.disp = display_lock()))
		;

	graphics_fill_screen(screen->hwdata->data.disp, render_color);

	return 0;
}

int SDL_UpperBlit(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
	return SDL_LowerBlit(src, srcrect, dst, dstrect);
}

int SDL_LowerBlit(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
	if (first_draw) {
		return 0;
	}

	if (dst->hwdata->type != ST_DISPLAY || src->hwdata->type != ST_SPRITE)
		return -1;

	graphics_draw_sprite_trans(dst->hwdata->data.disp, dstrect->x, dstrect->y,
							   src->hwdata->data.sprite);

	return 0;
}