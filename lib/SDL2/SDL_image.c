#include "SDL_image.h"

#include <libdragon.h>

typedef struct SDL_BlitMap {
	sprite_t *sprite;
} SDL_BlitMap;

int IMG_Init(int flags) {
	return 0;
}

void IMG_Quit(void) {
}

sprite_t *spritesheet_load(const char *sprite_path) {
	int fp = dfs_open(sprite_path);

	sprite_t *sprite = NULL;
	sprite = (sprite_t *)malloc(dfs_size(fp));

	dfs_read(sprite, 1, dfs_size(fp), fp);
	dfs_close(fp);

	return sprite;
}

SDL_Surface *IMG_Load(const char *file) {
	SDL_Surface *surface = malloc(sizeof(SDL_Surface));
	surface->map = malloc(sizeof(SDL_BlitMap));

	surface->map->sprite = spritesheet_load(file);

	if (!surface->map->sprite) {
		debugf("Error loading image %s\n", file);
	}

	return surface;
}
