#include "SDL_image.h"

#include "SDL_surface.h"

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
	surface->hwdata = malloc(sizeof(private_hwdata));

	surface->hwdata->type = ST_SPRITE;
	surface->hwdata->data.sprite = spritesheet_load(file);

	if (!surface->hwdata->data.sprite) {
		debugf("Error loading image %s\n", file);
	}

	return surface;
}
