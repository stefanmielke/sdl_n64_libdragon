/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

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

/* This is a generic "format not supported" image framework */

#include "SDL_image.h"

#ifndef LOAD_SPRITE
#define LOAD_SPRITE
#endif

#ifdef LOAD_SPRITE

#include <libdragon.h>

/* See if an image is contained in a data source */
int IMG_isSPRITE(SDL_RWops *src)
{
    int start;
    int is_SPRITE;

    if ( !src )
        return 0;
    start = SDL_RWtell(src);
    is_SPRITE = 0;

    // width, height, bit_depth (2 or 4)
    unsigned short magic[3];
    if ( SDL_RWread(src, magic, 1, sizeof(magic)) == sizeof(magic) ) {
        if ( magic[0] > 0 && magic[1] > 0 && (magic[2] == 2 || magic[2] == 4)) {
            is_SPRITE = 1;
        }
    }

    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_SPRITE);
}

/* Load a SPRITE type image from an SDL datasource */
SDL_Surface *IMG_LoadSPRITE_RW(SDL_RWops *src)
{
    int start;
    const char *error = NULL;
    SDL_Surface *surface = NULL;

    if ( !src ) {
        /* The error message has been set in SDL_RWFromFile */
        return NULL;
    }
    start = SDL_RWtell(src);

	const int size = SDL_RWsize(src);
	sprite_t *sprite = malloc(size);

    if (!SDL_RWread(src, sprite, size, 1)) {
        error = "Error reading SPRITE data";
        goto error;
    }

    Uint32 format;
    if( sprite->bitdepth == 2 ) {
        format = SDL_PIXELFORMAT_RGBA5551;
    }
    else {
        format = SDL_PIXELFORMAT_RGBA8888;
    }

    surface = SDL_CreateRGBSurfaceWithFormat(0, sprite->width, sprite->height, sprite->bitdepth * 8, format);
    if ( !surface ) {
        error = "Could not create the Surface";
        goto error;
    }

    // copying only the pixels to another struct to free the sprite_t created
    // the pixels will be deleted on SDL_FreeSurface
    size_t data_size = size - sizeof(sprite_t);

    void *pixels = malloc(size - sizeof(sprite_t));
    memcpy(pixels, sprite->data, data_size);
    free(sprite);

    surface->pixels = pixels;

error:
    if ( error ) {
        SDL_RWseek(src, start, RW_SEEK_SET);
        if ( surface ) {
            SDL_FreeSurface(surface);
            surface = NULL;
        }
        IMG_SetError("%s", error);
    }
    return surface;
}

#else

/* See if an image is contained in a data source */
int IMG_isSPRITE(SDL_RWops *src)
{
    return(0);
}

/* Load a SPRITE type image from an SDL datasource */
SDL_Surface *IMG_LoadSPRITE_RW(SDL_RWops *src)
{
    return(NULL);
}

#endif /* LOAD_SPRITE */
