#include "SDL_render.h"

#include <libdragon.h>

extern bool gen_event_flag;

static uint32_t render_color;

struct SDL_Renderer {
	display_context_t disp;
} internal_renderer;

SDL_Renderer *SDL_CreateRenderer(SDL_Window *window, int index, Uint32 flags) {
	internal_renderer.disp = 0;
	return &internal_renderer;
}

int SDL_SetRenderDrawColor(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	render_color = graphics_make_color(r, g, b, a);

	return 0;
}

int SDL_RenderClear(SDL_Renderer *renderer) {
	if (!renderer)
		return -1;

	while (!(renderer->disp = display_lock()))
		;

	graphics_fill_screen(renderer->disp, render_color);

	return 0;
}

int SDL_RenderFillRect(SDL_Renderer *renderer, const SDL_Rect *rect) {
	if (!renderer)
		return -1;

	graphics_draw_box(renderer->disp, rect->x, rect->y, rect->w, rect->h, render_color);

	return 0;
}

void SDL_RenderPresent(SDL_Renderer *renderer) {
	if (!renderer)
		return;

	display_show(renderer->disp);

	gen_event_flag = true;
}

void SDL_DestroyRenderer(SDL_Renderer *renderer) {
}