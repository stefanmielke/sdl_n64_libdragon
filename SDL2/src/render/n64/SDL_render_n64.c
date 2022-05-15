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

#if SDL_VIDEO_RENDER_N64

#include "SDL_hints.h"
#include "../SDL_sysrender.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <libdragon.h>

/* N64 renderer implementation, based on the PGE  */

#define N64_SCREEN_WIDTH    320
#define N64_SCREEN_HEIGHT   240

#define N64_FRAME_BUFFER_WIDTH  N64_SCREEN_WIDTH * 16
#define N64_FRAME_BUFFER_SIZE   (N64_FRAME_BUFFER_WIDTH * N64_SCREEN_HEIGHT)

/**
 * Holds n64 specific texture data
 *
 * Part of a hot-list of textures that are used as render targets
 * When short of vram we spill Least-Recently-Used render targets to system memory
 */
typedef struct N64_TextureData
{
    void                *data;                              /**< Image data. */
    unsigned int        size;                               /**< Size of data in bytes. */
    unsigned int        width;                              /**< Image width. */
    unsigned int        height;                             /**< Image height. */
    unsigned int        textureWidth;                       /**< Texture width (power of two). */
    unsigned int        textureHeight;                      /**< Texture height (power of two). */
    unsigned int        bits;                               /**< Image bits per pixel. */
    unsigned int        format;                             /**< Image format - one of ::pgePixelFormat. */
    unsigned int        pitch;
    sprite_t            *sprite;
} N64_TextureData;

typedef struct N64_BlendState
{
    SDL_BlendMode mode;
    unsigned int color;
    int shadeModel;
    SDL_Texture* texture;
} N64_BlendState;

typedef struct N64_RenderData
{
    /** currently bound rendertarget */
    SDL_Texture *boundTarget;
    /* libdragon's render context */
    display_context_t displayContext;
} N64_RenderData;


typedef struct VertV
{
    float   x, y, z;
} VertV;


typedef struct VertTV
{
    float   u, v;
    float   x, y, z;
} VertTV;

typedef struct VertCV
{
    SDL_Color col;
    float     x, y, z;
} VertCV;


typedef struct VertTCV
{
    float     u, v;
    SDL_Color col;
    float     x, y, z;
} VertTCV;

#define PI 3.14159265358979f

#define radToDeg(x) ((x)*180.f/PI)
#define degToRad(x) ((x)*PI/180.f)

 void
Swap(float *a, float *b)
{
    float n=*a;
    *a = *b;
    *b = n;
}

/* Return next power of 2 */
static int
TextureNextPow2(unsigned int w)
{
    unsigned int n = 2;
    if(w == 0)
        return 0;

    while(w > n)
        n <<= 1;

    return n;
}

static int
PixelFormatToN64FMT(Uint32 format)
{
    switch (format) {
    case SDL_PIXELFORMAT_ABGR8888:
        return DEPTH_32_BPP;
    default:
        return DEPTH_16_BPP;
    }
}

 int
TextureSwizzle(N64_TextureData *n64_texture, void* dst)
{
    // int bytewidth, height;
    // int rowblocks, rowblocksadd;
    // int i, j;
    // unsigned int blockaddress = 0;
    // unsigned int *src = NULL;
    // unsigned char *data = NULL;

    // if(n64_texture->swizzled)
    //     return 1;

    // bytewidth = n64_texture->textureWidth*(n64_texture->bits>>3);
    // height = n64_texture->size / bytewidth;

    // rowblocks = (bytewidth>>4);
    // rowblocksadd = (rowblocks-1)<<7;

    // src = (unsigned int*) n64_texture->data;

    // data = dst;
    // if(!data) {
    //     data = SDL_malloc(n64_texture->size);
    // }

    // if(!data) {
    //     return SDL_OutOfMemory();
    // }

    // for(j = 0; j < height; j++, blockaddress += 16)
    // {
    //     unsigned int *block;

    //     block = (unsigned int*)&data[blockaddress];

    //     for(i = 0; i < rowblocks; i++)
    //     {
    //         *block++ = *src++;
    //         *block++ = *src++;
    //         *block++ = *src++;
    //         *block++ = *src++;
    //         block += 28;
    //     }

    //     if((j & 0x7) == 0x7)
    //         blockaddress += rowblocksadd;
    // }

    // // TextureStorageFree(n64_texture->data);
    // n64_texture->data = data;
    // n64_texture->swizzled = SDL_TRUE;

    // sceKernelDcacheWritebackRange(n64_texture->data, n64_texture->size);
    return 1;
}

 int
TextureUnswizzle(N64_TextureData *n64_texture, void* dst)
{
    // int bytewidth, height;
    // int widthblocks, heightblocks;
    // int dstpitch, dstrow;
    // int blockx, blocky;
    // int j;
    // unsigned int *src = NULL;
    // unsigned char *data = NULL;
    // unsigned char *ydst = NULL;

    // if(!n64_texture->swizzled)
    //     return 1;

    // bytewidth = n64_texture->textureWidth*(n64_texture->bits>>3);
    // height = n64_texture->size / bytewidth;

    // widthblocks = bytewidth/16;
    // heightblocks = height/8;

    // dstpitch = (bytewidth - 16)/4;
    // dstrow = bytewidth * 8;

    // src = (unsigned int*) n64_texture->data;

    // data = dst;

    // if(!data) {
    //     data = SDL_malloc(n64_texture->size);
    // }

    // if(!data)
    //     return SDL_OutOfMemory();

    // ydst = (unsigned char *)data;

    // for(blocky = 0; blocky < heightblocks; ++blocky)
    // {
    //     unsigned char *xdst = ydst;

    //     for(blockx = 0; blockx < widthblocks; ++blockx)
    //     {
    //         unsigned int *block;

    //         block = (unsigned int*)xdst;

    //         for(j = 0; j < 8; ++j)
    //         {
    //             *(block++) = *(src++);
    //             *(block++) = *(src++);
    //             *(block++) = *(src++);
    //             *(block++) = *(src++);
    //             block += dstpitch;
    //         }

    //         xdst += 16;
    //     }

    //     ydst += dstrow;
    // }

    // TextureStorageFree(n64_texture->data);

    // n64_texture->data = data;

    // n64_texture->swizzled = SDL_FALSE;

    // sceKernelDcacheWritebackRange(n64_texture->data, n64_texture->size);
    return 1;
}

 int
TextureBindAsTarget(N64_RenderData* data, N64_TextureData* n64_texture)
{
    return 0;
}

static void
N64_WindowEvent(SDL_Renderer *renderer, const SDL_WindowEvent *event)
{
    fprintf(stderr, "N64_WindowEvent: %u\n", event->event);
}


static int
N64_CreateTexture(SDL_Renderer *renderer, SDL_Texture *texture)
{
    fprintf(stderr, "N64_CreateTexture\n");
    N64_TextureData *n64_texture = (N64_TextureData *)SDL_calloc(1, sizeof(*n64_texture));
    if(!n64_texture)
        return SDL_OutOfMemory();

    n64_texture->width = texture->w;
    n64_texture->height = texture->h;
    n64_texture->textureHeight = TextureNextPow2(texture->h);
    n64_texture->textureWidth = TextureNextPow2(texture->w);
    n64_texture->format = PixelFormatToN64FMT(texture->format);

    switch(n64_texture->format)
    {
        case DEPTH_16_BPP:
            n64_texture->bits = 16;
            break;
        case DEPTH_32_BPP:
            n64_texture->bits = 32;
            break;
        default:
            return -1;
    }

    n64_texture->pitch = n64_texture->width * SDL_BYTESPERPIXEL(texture->format);
    n64_texture->size = n64_texture->height * n64_texture->pitch;

    sprite_t *sprite = SDL_calloc(1, sizeof(sprite_t) + n64_texture->size);
    sprite->width = n64_texture->width;
    sprite->height = n64_texture->height;
    sprite->bitdepth = SDL_BYTESPERPIXEL(texture->format);
    sprite->vslices = 1;
    sprite->hslices = 1;
    
    n64_texture->sprite = sprite;

    n64_texture->data = n64_texture->sprite->data;
    if(!n64_texture->data)
    {
        SDL_free(n64_texture);
        return SDL_OutOfMemory();
    }
    texture->driverdata = n64_texture;

    return 0;
}

 int
TextureShouldSwizzle(N64_TextureData* n64_texture, SDL_Texture *texture)
{
    return -1;
    // return !((texture->access == SDL_TEXTUREACCESS_TARGET) && InVram(n64_texture->data))
    //          && (texture->w >= 16 || texture->h >= 16);
}

 void
TextureActivate(SDL_Texture * texture)
{
    // N64_TextureData *n64_texture = (N64_TextureData *) texture->driverdata;
    // int scaleMode = (texture->scaleMode == SDL_ScaleModeNearest) ? GU_NEAREST : GU_LINEAR;

    // /* Swizzling is useless with small textures. */
    // if (TextureShouldSwizzle(n64_texture, texture))
    // {
    //     TextureSwizzle(n64_texture, NULL);
    // }

    // sceGuTexWrap(GU_REPEAT, GU_REPEAT);
    // sceGuTexMode(n64_texture->format, 0, 0, n64_texture->swizzled);
    // sceGuTexFilter(scaleMode, scaleMode); /* GU_NEAREST good for tile-map */
    //                                       /* GU_LINEAR good for scaling */
    // sceGuTexImage(0, n64_texture->textureWidth, n64_texture->textureHeight, n64_texture->textureWidth, n64_texture->data);
}

static int
N64_LockTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                 const SDL_Rect *rect, void **pixels, int *pitch);

static int
N64_UpdateTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                   const SDL_Rect *rect, const void *pixels, int pitch)
{
    // fprintf(stderr, "N64_UpdateTexture\n");
    const Uint8 *src;
    Uint8 *dst;
    int row, length,dpitch;
    src = pixels;

    N64_LockTexture(renderer, texture, rect, (void **)&dst, &dpitch);
    length = rect->w * SDL_BYTESPERPIXEL(texture->format);
    if (length == pitch && length == dpitch) {
        SDL_memcpy(dst, src, length * rect->h);
    } else {
        fprintf(stderr, "N64_UpdateTexture slow path.\n");
        for (row = 0; row < rect->h; ++row) {
            SDL_memcpy(dst, src, length);
            src += pitch;
            dst += dpitch;
        }
    }

    return 0;
}

static int
N64_LockTexture(SDL_Renderer *renderer, SDL_Texture *texture,
                 const SDL_Rect *rect, void **pixels, int *pitch)
{
    // fprintf(stderr, "N64_LockTexture\n");
    N64_TextureData *n64_texture = (N64_TextureData *)texture->driverdata;

    *pixels = (void *)((Uint8 *)n64_texture->data + rect->y * n64_texture->pitch +
                rect->x * SDL_BYTESPERPIXEL(texture->format));
    *pitch = n64_texture->pitch;

    return 0;
}

static void
N64_UnlockTexture(SDL_Renderer *renderer, SDL_Texture *texture)
{
    // fprintf(stderr, "N64_UnlockTexture\n");
    // we don't need to do anything here, since the LockTexture gave access to the raw pixels in the sprite
}

static void
N64_SetTextureScaleMode(SDL_Renderer *renderer, SDL_Texture *texture, SDL_ScaleMode scaleMode)
{
    fprintf(stderr, "N64_SetTextureScaleMode\n");
    /* Nothing to do because TextureActivate takes care of it */
}

static int
N64_SetRenderTarget(SDL_Renderer *renderer, SDL_Texture *texture)
{
    fprintf(stderr, "N64_SetRenderTarget\n");

    return 0;
}

static int
N64_QueueSetViewport(SDL_Renderer *renderer, SDL_RenderCommand *cmd)
{
    // fprintf(stderr, "N64_QueueSetViewport\n");
    return 0;  /* nothing to do in this backend. */
}

static int
N64_QueueDrawPoints(SDL_Renderer *renderer, SDL_RenderCommand *cmd, const SDL_FPoint *points, int count)
{
    fprintf(stderr, "N64_QueueDrawPoints\n");
    VertV *verts = (VertV *) SDL_AllocateRenderVertices(renderer, count * sizeof (VertV), 4, &cmd->data.draw.first);
    int i;

    if (!verts) {
        return -1;
    }

    cmd->data.draw.count = count;

    for (i = 0; i < count; i++, verts++, points++) {
        verts->x = points->x;
        verts->y = points->y;
        verts->z = 0.0f;
    }

    return 0;
}

static int
N64_QueueGeometry(SDL_Renderer *renderer, SDL_RenderCommand *cmd, SDL_Texture *texture,
        const float *xy, int xy_stride, const SDL_Color *color, int color_stride, const float *uv, int uv_stride,
        int num_vertices, const void *indices, int num_indices, int size_indices,
        float scale_x, float scale_y)
{
    fprintf(stderr, "N64_QueueGeometry\n");
    int i;
    int count = indices ? num_indices : num_vertices;

    cmd->data.draw.count = count;
    size_indices = indices ? size_indices : 0;

    if (texture == NULL) {
        VertCV *verts;
        verts = (VertCV *)SDL_AllocateRenderVertices(renderer, count * sizeof(VertCV), 4, &cmd->data.draw.first);
        if (!verts) {
            return -1;
        }

        for (i = 0; i < count; i++) {
            int j;
            float *xy_;
            SDL_Color col_;
            if (size_indices == 4) {
                j = ((const Uint32 *)indices)[i];
            } else if (size_indices == 2) {
                j = ((const Uint16 *)indices)[i];
            } else if (size_indices == 1) {
                j = ((const Uint8 *)indices)[i];
            } else {
                j = i;
            }

            xy_ = (float *)((char*)xy + j * xy_stride);
            col_ = *(SDL_Color *)((char*)color + j * color_stride);

            verts->x = xy_[0] * scale_x;
            verts->y = xy_[1] * scale_y;
            verts->z = 0;

            verts->col = col_;

            verts++;
        }
    } else {
        N64_TextureData *n64_texture = (N64_TextureData *)texture->driverdata;
        VertTCV *verts;
        verts = (VertTCV *) SDL_AllocateRenderVertices(renderer, count * sizeof (VertTCV), 4, &cmd->data.draw.first);
        if (!verts) {
            return -1;
        }

        for (i = 0; i < count; i++) {
            int j;
            float *xy_;
            SDL_Color col_;
            float *uv_;

            if (size_indices == 4) {
                j = ((const Uint32 *)indices)[i];
            } else if (size_indices == 2) {
                j = ((const Uint16 *)indices)[i];
            } else if (size_indices == 1) {
                j = ((const Uint8 *)indices)[i];
            } else {
                j = i;
            }

            xy_ = (float *)((char*)xy + j * xy_stride);
            col_ = *(SDL_Color *)((char*)color + j * color_stride);
            uv_ = (float *)((char*)uv + j * uv_stride);

            verts->x = xy_[0] * scale_x;
            verts->y = xy_[1] * scale_y;
            verts->z = 0;

            verts->col = col_;

            verts->u = uv_[0] * n64_texture->textureWidth;
            verts->v = uv_[1] * n64_texture->textureHeight;

            verts++;
        }
    }

    return 0;
}

static int
N64_QueueFillRects(SDL_Renderer *renderer, SDL_RenderCommand *cmd, const SDL_FRect *rects, int count)
{
    fprintf(stderr, "N64_QueueFillRects\n");
    VertV *verts = (VertV *)SDL_AllocateRenderVertices(renderer, count * 2 * sizeof(VertV), 4, &cmd->data.draw.first);
    int i;

    if (!verts) {
        return -1;
    }

    cmd->data.draw.count = count;
    for (i = 0; i < count; i++, rects++) {
        const SDL_FRect *rect = &rects[i];
        verts->x = rect->x;
        verts->y = rect->y;
        verts->z = 0.0f;
        verts++;

        verts->x = rect->x + rect->w;
        verts->y = rect->y + rect->h;
        verts->z = 0.0f;
        verts++;
    }

    return 0;
}

static int
N64_QueueCopy(SDL_Renderer *renderer, SDL_RenderCommand *cmd, SDL_Texture *texture,
             const SDL_Rect *srcrect, const SDL_FRect *dstrect)
{
    // TODO: properly implement this. We'll need this PR merged (or similar interface): https://github.com/DragonMinded/libdragon/pull/238

    // fprintf(stderr, "N64_QueueCopy\n");
    // VertTV *verts;
    // verts = (VertTV *)SDL_AllocateRenderVertices(renderer, 2 * sizeof(VertTV), 4, &cmd->data.draw.first);
    // if (!verts) {
    //     fprintf(stderr, "N64_QueueCopy error\n");
    //     return -1;
    // }

    // const float x = dstrect->x;
    // const float y = dstrect->y;
    // const float width = dstrect->w;
    // const float height = dstrect->h;

    // const float u0 = srcrect->x;
    // const float v0 = srcrect->y;
    // const float u1 = srcrect->x + srcrect->w;
    // const float v1 = srcrect->y + srcrect->h;

    // fprintf(stderr, "   u:%.2f,v:%.2f,x:%.2f,y:%.2f\n", u0, v0, x, y);
    // fprintf(stderr, "   u:%.2f,v:%.2f,x:%.2f,y:%.2f\n", u1, v1, x + width, y + height);

    // cmd->data.draw.count = 1;

    // verts->u = u0;
    // verts->v = v0;
    // verts->x = x;
    // verts->y = y;
    // verts->z = 0;
    // verts++;

    // verts->u = u1;
    // verts->v = v1;
    // verts->x = x + width;
    // verts->y = y + height;
    // verts->z = 0;
    // verts++;

    return 0;
}

static int
N64_QueueCopyEx(SDL_Renderer * renderer, SDL_RenderCommand *cmd, SDL_Texture * texture,
               const SDL_Rect * srcrect, const SDL_FRect * dstrect,
               const double angle, const SDL_FPoint *center, const SDL_RendererFlip flip)
{
    // TODO: properly implement this. We'll need this PR merged (or similar interface): https://github.com/DragonMinded/libdragon/pull/238
    fprintf(stderr, "N64_QueueCopyEx\n");
    // VertTV *verts = (VertTV *)SDL_AllocateRenderVertices(renderer, 4 * sizeof (VertTV), 4, &cmd->data.draw.first);
    // if (!verts) {
    //     fprintf(stderr, "N64_QueueCopyEx error\n");
    //     return -1;
    // }

    // const float centerx = center->x;
    // const float centery = center->y;
    // const float x = dstrect->x + centerx;
    // const float y = dstrect->y + centery;
    // const float width = dstrect->w - centerx;
    // const float height = dstrect->h - centery;
    // double s, c;
    // float cw, sw, ch, sh;

    // float u0 = srcrect->x;
    // float v0 = srcrect->y;
    // float u1 = srcrect->x + srcrect->w;
    // float v1 = srcrect->y + srcrect->h;

    // cmd->data.draw.count = 1;

    // sincos(degToRad(angle), &s, &c);

    // cw = c * width;
    // sw = s * width;
    // ch = c * height;
    // sh = s * height;

    // if (flip & SDL_FLIP_VERTICAL) {
    //     Swap(&v0, &v1);
    // }

    // if (flip & SDL_FLIP_HORIZONTAL) {
    //     Swap(&u0, &u1);
    // }

    // verts->u = u0;
    // verts->v = v0;
    // verts->x = x - cw + sh;
    // verts->y = y - sw - ch;
    // verts->z = 0;
    // verts++;

    // verts->u = u0;
    // verts->v = v1;
    // verts->x = x - cw - sh;
    // verts->y = y - sw + ch;
    // verts->z = 0;
    // verts++;

    // verts->u = u1;
    // verts->v = v1;
    // verts->x = x + cw - sh;
    // verts->y = y + sw + ch;
    // verts->z = 0;
    // verts++;

    // verts->u = u1;
    // verts->v = v0;
    // verts->x = x + cw + sh;
    // verts->y = y + sw - ch;
    // verts->z = 0;
    // verts++;

    return 0;
}

 void
ResetBlendState(N64_BlendState* state)
{
}

static void
StartDrawing(SDL_Renderer *renderer)
{
    // fprintf(stderr, "StartDrawing\n");
    N64_RenderData *data = (N64_RenderData *)renderer->driverdata;
    
    data->displayContext = 0;
    while (!(data->displayContext = display_lock()))
        ;
}

 void
N64_SetBlendState(N64_RenderData *data, N64_BlendState *state)
{
}

static int
N64_RunCommandQueue(SDL_Renderer * renderer, SDL_RenderCommand *cmd, void *vertices, size_t vertsize)
{
    // fprintf(stderr, "N64_RunCommandQueue\n");
    N64_RenderData *data = (N64_RenderData *) renderer->driverdata;
    StartDrawing(renderer);

    // Uint8 *gpumem = (Uint8 *)sceGuGetMemory(vertsize);
    // if (!gpumem) {
    //     return SDL_SetError("Couldn't obtain a %d-byte vertex buffer!", (int) vertsize);
    // }
    // SDL_memcpy(gpumem, vertices, vertsize);

    while (cmd) {
        switch (cmd->command) {
            case SDL_RENDERCMD_SETDRAWCOLOR: {
                // fprintf(stderr, "\tSDL_RENDERCMD_SETDRAWCOLOR\n");
                break;
            }

            case SDL_RENDERCMD_SETVIEWPORT: {
                // fprintf(stderr, "\tSDL_RENDERCMD_SETVIEWPORT\n");
                // SDL_Rect *viewport = &cmd->data.viewport.rect;
                // sceGuOffset(2048 - (viewport->w >> 1), 2048 - (viewport->h >> 1));
                // sceGuViewport(2048, 2048, viewport->w, viewport->h);
                // sceGuScissor(viewport->x, viewport->y, viewport->w, viewport->h);
                break;
            }

            case SDL_RENDERCMD_SETCLIPRECT: {
                // fprintf(stderr, "\tSDL_RENDERCMD_SETCLIPRECT\n");
                // const SDL_Rect *rect = &cmd->data.cliprect.rect;
                // if(cmd->data.cliprect.enabled){
                //     sceGuEnable(GU_SCISSOR_TEST);
                //     sceGuScissor(rect->x, rect->y, rect->w, rect->h);
                // } else {
                //     sceGuDisable(GU_SCISSOR_TEST);
                // }
                break;
            }

            case SDL_RENDERCMD_CLEAR: {
                // fprintf(stderr, "\tSDL_RENDERCMD_CLEAR\n");
                const Uint8 r = cmd->data.color.r;
                const Uint8 g = cmd->data.color.g;
                const Uint8 b = cmd->data.color.b;
                const Uint8 a = cmd->data.color.a;

                // fprintf(stderr, "       %u, %u, %u, %u\n", r, g, b, a);

                uint32_t color = graphics_make_color(r, g, b, a);
                graphics_fill_screen(data->displayContext, color);
                break;
            }

            case SDL_RENDERCMD_DRAW_POINTS: {
                fprintf(stderr, "\tSDL_RENDERCMD_DRAW_POINTS\n");
                const Uint8 r = cmd->data.draw.r;
                const Uint8 g = cmd->data.draw.g;
                const Uint8 b = cmd->data.draw.b;
                const Uint8 a = cmd->data.draw.a;

                uint32_t color = graphics_make_color(r, g, b, a);
                graphics_fill_screen(data->displayContext, color);

                const size_t count = cmd->data.draw.count;
                const VertV *verts = (VertV *)(cmd->data.draw.first);
                for (size_t i = 0; i < count; ++i) {
                    graphics_draw_pixel(data->displayContext, verts[i].x, verts[i].y, color);
                }
                break;
            }

            case SDL_RENDERCMD_DRAW_LINES: {
                fprintf(stderr, "\tSDL_RENDERCMD_DRAW_LINES\n");
                const Uint8 r = cmd->data.draw.r;
                const Uint8 g = cmd->data.draw.g;
                const Uint8 b = cmd->data.draw.b;
                const Uint8 a = cmd->data.draw.a;

                uint32_t color = graphics_make_color(r, g, b, a);
                graphics_fill_screen(data->displayContext, color);

                const size_t count = cmd->data.draw.count;
                const VertV *verts = (VertV *)(cmd->data.draw.first);
                for (size_t i = 0; i < count; i += 2) {
                    graphics_draw_line(data->displayContext, verts[i].x,
                        verts[i].y, verts[i + 1].x, verts[i + 1].y, color);
                }

                break;
            }

            case SDL_RENDERCMD_FILL_RECTS: {
                fprintf(stderr, "\tSDL_RENDERCMD_FILL_RECTS\n");
                // const size_t count = cmd->data.draw.count;
                // const VertV *verts = (VertV *) (gpumem + cmd->data.draw.first);
                // const Uint8 r = cmd->data.draw.r;
                // const Uint8 g = cmd->data.draw.g;
                // const Uint8 b = cmd->data.draw.b;
                // const Uint8 a = cmd->data.draw.a;
                break;
            }

            case SDL_RENDERCMD_COPY: {
                // fprintf(stderr, "\tSDL_RENDERCMD_COPY\n");
                // const size_t count = cmd->data.draw.count;
                // const VertTV *verts = (VertTV *)(cmd->data.draw.first);
                // const Uint8 a = cmd->data.draw.a;
                // const Uint8 r = cmd->data.draw.r;
                // const Uint8 g = cmd->data.draw.g;
                // const Uint8 b = cmd->data.draw.b;
                // fprintf(stderr, "       count: %u\n", count);
                // fprintf(stderr, "       u:%.2f,v:%.2f,x:%.2f,y:%.2f\n", verts->u, verts->v, verts->x, verts->y);
                // verts++;
                // fprintf(stderr, "       u:%.2f,v:%.2f,x:%.2f,y:%.2f\n", verts->u, verts->v, verts->x, verts->y);

                N64_TextureData *n64_texture = cmd->data.draw.texture->driverdata;
                graphics_draw_sprite(data->displayContext, 0, 0, n64_texture->sprite);
                break;
            }

            case SDL_RENDERCMD_COPY_EX: {
                fprintf(stderr, "\tSDL_RENDERCMD_COPY_EX\n");
                // const VertTV *verts = (VertTV *) (gpumem + cmd->data.draw.first);
                // const Uint8 a = cmd->data.draw.a;
                // const Uint8 r = cmd->data.draw.r;
                // const Uint8 g = cmd->data.draw.g;
                // const Uint8 b = cmd->data.draw.b;
                // N64_BlendState state = {
                //     .color = GU_RGBA(r,g,b,a),
                //     .texture = cmd->data.draw.texture,
                //     .mode = cmd->data.draw.blend,
                //     .shadeModel = GU_SMOOTH
                // };
                // N64_SetBlendState(data, &state);
                // sceGuDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 4, 0, verts);
                break;
            }

            case SDL_RENDERCMD_GEOMETRY: {
                fprintf(stderr, "\tSDL_RENDERCMD_GEOMETRY\n");
                // const size_t count = cmd->data.draw.count;
                // if (cmd->data.draw.texture == NULL) {
                //     const VertCV *verts = (VertCV *) (gpumem + cmd->data.draw.first);
                //     sceGuDisable(GU_TEXTURE_2D);
                //     /* In GU_SMOOTH mode */
                //     sceGuDrawArray(GU_TRIANGLES, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, count, 0, verts);
                //     sceGuEnable(GU_TEXTURE_2D);
                // } else {
                //     const VertTCV *verts = (VertTCV *) (gpumem + cmd->data.draw.first);
                //     const Uint8 a = cmd->data.draw.a;
                //     const Uint8 r = cmd->data.draw.r;
                //     const Uint8 g = cmd->data.draw.g;
                //     const Uint8 b = cmd->data.draw.b;
                //     N64_BlendState state = {
                //         .color = GU_RGBA(r,g,b,a),
                //         .texture = NULL,
                //         .mode = cmd->data.draw.blend,
                //         .shadeModel = GU_FLAT
                //     };
                //     TextureActivate(cmd->data.draw.texture);
                //     N64_SetBlendState(data, &state);
                //     sceGuDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, count, 0, verts);
                // }
                break;
            }

            case SDL_RENDERCMD_NO_OP:
                fprintf(stderr, "\tSDL_RENDERCMD_NO_OP\n");
                break;
        }

        cmd = cmd->next;
    }

    return 0;
}

static int
N64_RenderReadPixels(SDL_Renderer *renderer, const SDL_Rect *rect,
                    Uint32 pixel_format, void *pixels, int pitch)
{
    fprintf(stderr, "N64_RenderReadPixels\n");
    return SDL_Unsupported();
}

static void
N64_RenderPresent(SDL_Renderer * renderer)
{
    // fprintf(stderr, "N64_RenderPresent\n");
    N64_RenderData *data = (N64_RenderData *)renderer->driverdata;

    display_show(data->displayContext);
}

static void
N64_DestroyTexture(SDL_Renderer *renderer, SDL_Texture *texture)
{
    N64_RenderData *renderdata = (N64_RenderData *)renderer->driverdata;
    N64_TextureData *n64_texture = (N64_TextureData *)texture->driverdata;

    if (renderdata == 0)
        return;

    if(n64_texture == 0)
        return;

    SDL_free(n64_texture->sprite);
    SDL_free(n64_texture);
    texture->driverdata = NULL;
}

static void
N64_DestroyRenderer(SDL_Renderer * renderer)
{
    fprintf(stderr, "N64_DestroyRenderer\n");
    N64_RenderData *data = (N64_RenderData *)renderer->driverdata;
    if (data) {
        rdp_close();
        display_close();

        SDL_free(data);
        renderer->driverdata = NULL;
    }
    SDL_free(renderer);
}

static int
N64_SetVSync(SDL_Renderer * renderer, const int vsync)
{
    fprintf(stderr, "N64_SetVSync\n");
    return 0;
}

SDL_Renderer *
N64_CreateRenderer(SDL_Window *window, Uint32 flags)
{
    SDL_Renderer *renderer;
    N64_RenderData *data;
    int pixelformat;

    renderer = (SDL_Renderer *) SDL_calloc(1, sizeof(*renderer));
    if (!renderer) {
        SDL_OutOfMemory();
        return NULL;
    }

    data = (N64_RenderData *) SDL_calloc(1, sizeof(*data));
    if (!data) {
        N64_DestroyRenderer(renderer);
        SDL_OutOfMemory();
        return NULL;
    }

    renderer->WindowEvent = N64_WindowEvent;
    renderer->CreateTexture = N64_CreateTexture;
    renderer->UpdateTexture = N64_UpdateTexture;
    renderer->LockTexture = N64_LockTexture;
    renderer->UnlockTexture = N64_UnlockTexture;
    renderer->SetTextureScaleMode = N64_SetTextureScaleMode;
    renderer->SetRenderTarget = N64_SetRenderTarget;
    renderer->QueueSetViewport = N64_QueueSetViewport;
    renderer->QueueSetDrawColor = N64_QueueSetViewport;  /* SetViewport and SetDrawColor are (currently) no-ops. */
    renderer->QueueDrawPoints = N64_QueueDrawPoints;
    renderer->QueueDrawLines = N64_QueueDrawPoints;  /* lines and points queue vertices the same way. */
    renderer->QueueGeometry = N64_QueueGeometry;
    renderer->QueueFillRects = N64_QueueFillRects;
    renderer->QueueCopy = N64_QueueCopy;
    renderer->QueueCopyEx = N64_QueueCopyEx;
    renderer->RunCommandQueue = N64_RunCommandQueue;
    renderer->RenderReadPixels = N64_RenderReadPixels;
    renderer->RenderPresent = N64_RenderPresent;
    renderer->DestroyTexture = N64_DestroyTexture;
    renderer->DestroyRenderer = N64_DestroyRenderer;
    renderer->SetVSync = N64_SetVSync;
    renderer->info = N64_RenderDriver.info;
    renderer->info.flags = (SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
    renderer->driverdata = data;
    renderer->window = window;

    pixelformat = PixelFormatToN64FMT(SDL_GetWindowPixelFormat(window));

    SDL_DisplayMode mode;
    SDL_GetWindowDisplayMode(window, &mode);

	int ld_resolution = RESOLUTION_320x240;
    int w = mode.w;
    int h = mode.h;
	if (w > 512) {
		ld_resolution = h > 240 ? RESOLUTION_640x480 : RESOLUTION_640x240;
	} else if (w > 320) {
		ld_resolution = h > 240 ? RESOLUTION_512x480 : RESOLUTION_512x240;
	} else {
		ld_resolution = w > 256 ? RESOLUTION_320x240 : RESOLUTION_256x240;
	}

    display_init(ld_resolution, pixelformat, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
	rdp_init();

    return renderer;
}

SDL_RenderDriver N64_RenderDriver = {
    .CreateRenderer = N64_CreateRenderer,
    .info = {
        .name = "N64",
        .flags = SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC,
        .num_texture_formats = 2,
        .texture_formats = { [0] = SDL_PIXELFORMAT_RGBA5551,
                             [1] = SDL_PIXELFORMAT_RGBA8888,
        },
        .max_texture_width = 640,
        .max_texture_height = 480,
     }
};

#endif /* SDL_VIDEO_RENDER_N64 */

/* vi: set ts=4 sw=4 expandtab: */

