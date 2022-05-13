#pragma once

#include <libdragon.h>

typedef enum SurfaceType {
	ST_DISPLAY,
	ST_SPRITE,
} SurfaceType;

typedef union SurfaceData {
	display_context_t disp;
	sprite_t *sprite;
} SurfaceData;

typedef struct private_hwdata {
	SurfaceType type;
	SurfaceData data;
} private_hwdata;
