V=1

SDL2_ROOT=SDL2
SDL2_SRC=$(SDL2_ROOT)/src

SDL2_IMAGE_ROOT=SDL2_image
SDL2_IMAGE_SRC=$(SDL2_IMAGE_ROOT)/src

SDL2_ROOT_DIR=$(SDL2_ROOT)
SDL2_IMAGE_ROOT_DIR=$(SDL2_IMAGE_ROOT)

include $(N64_INST)/include/n64.mk
include $(SDL2_ROOT)/Makefile_sdl2.mk
include $(SDL2_IMAGE_ROOT)/Makefile_sdl2_image.mk

BUILD_DIR=build

all: sdl2 sdl2_image

# SDL2

sdl2: CC=$(N64_CC)
sdl2: AS=$(N64_AS)
sdl2: LD=$(N64_LD)
sdl2: CFLAGS+=$(N64_CFLAGS) -I$(CURDIR)/src -I$(CURDIR)/include 
sdl2: ASFLAGS+=$(N64_ASFLAGS) -I$(CURDIR)/src -I$(CURDIR)/include
sdl2: LDFLAGS+=$(N64_LDFLAGS)
sdl2: SDL2_n64.a

SDL2_OBJ  := $(C_SDL_FILES:%.c=%.o)
SDL2_DEPS := $(C_SDL_FILES:%.c=%.d)

SDL2_n64.a: $(SDL2_OBJ)
	@echo "    [AR] $@"
	$(AR) -rcs -o $@ $^

# SDL2_image
sdl2_image: CC=$(N64_CC)
sdl2_image: AS=$(N64_AS)
sdl2_image: LD=$(N64_LD)
sdl2_image: CFLAGS+=$(N64_CFLAGS) -I$(CURDIR)/src -I$(CURDIR)/include 
sdl2_image: ASFLAGS+=$(N64_ASFLAGS) -I$(CURDIR)/src -I$(CURDIR)/include
sdl2_image: LDFLAGS+=$(N64_LDFLAGS)
sdl2_image: SDL2_image_n64.a

SDL2_IMAGE_OBJ  := $(C_SDL_IMAGE_FILES:%.c=%.o)
SDL2_IMAGE_DEPS := $(C_SDL_IMAGE_FILES:%.c=%.d)

SDL2_image_n64.a: $(SDL2_IMAGE_OBJ)
	@echo "    [AR] $@"
	$(AR) -rcs -o $@ $^

clean:
	find . -name '*.o' -delete
	find . -name '*.d' -delete
	find . -name '*.a' -delete

-include $(SDL2_DEPS) $(SDL2_IMAGE_DEPS)

.PHONY: all