#include "SDL_render.h"

#include <libdragon.h>

#include "SDL_n64.h"

extern bool gen_event_flag;

static uint32_t render_color;
static SDL_Renderer internal_renderer;

SDL_Renderer *SDL_CreateRenderer(SDL_Window *window, int index, Uint32 flags) {
	internal_renderer.disp = 0;
	internal_renderer.window = window;
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

int SDL_QueryTexture(SDL_Texture *texture, Uint32 *format, int *access, int *w, int *h) {
	if (!texture)
		return -1;

	if (format)
		*format = texture->sprite->format;
	if (w)
		*w = texture->sprite->width;
	if (h)
		*h = texture->sprite->height;

	return 0;
}

SDL_Texture *SDL_CreateTexture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h) {
	SDL_Texture *texture = malloc(sizeof(SDL_Texture));
	texture->sprite = malloc(sizeof(sprite_t));
	texture->sprite->bitdepth = renderer->window->bpp;
	texture->sprite->format = 0;  // FORMAT_UNCOMPRESSED
	texture->sprite->width = w;
	texture->sprite->height = h;
	texture->sprite->hslices = 1;
	texture->sprite->vslices = 1;
	void **sprite_location = (void **)(&texture->sprite->data[0]);
	*sprite_location = malloc(w * h * (sizeof(uint8_t) * 4));

	return texture;
}

void SDL_DestroyTexture(SDL_Texture *texture) {
	if (texture) {
		free((void *)texture->sprite->data[0]);
		free(texture->sprite);
		free(texture);
	}
}

int SDL_LockTexture(SDL_Texture *texture, const SDL_Rect *rect, void **pixels, int *pitch) {
	if (!texture)
		return -1;

	return 0;
}

void SDL_UnlockTexture(SDL_Texture *texture) {
}

int SDL_RenderCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect,
				   const SDL_Rect *dstrect) {
	// libdragon only supports full textures for now
	if (srcrect == NULL) {
		graphics_draw_sprite(renderer->disp, dstrect->x, dstrect->y, texture->sprite);
		return 0;
	}
	return -1;
}

void SDL_RenderPresent(SDL_Renderer *renderer) {
	if (!renderer)
		return;

	display_show(renderer->disp);

	gen_event_flag = true;
}

void SDL_DestroyRenderer(SDL_Renderer *renderer) {
}