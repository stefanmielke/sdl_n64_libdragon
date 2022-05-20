# Nintendo 64 port of SDL 1 and 2, and SDL2_Image

It includes all header files used by SDL and SDL2, but not necessarily implement all functions.

If you need a function and it's not implemented it should give linker errors. You are free to implement those functions anywhere in your project, as long as you compile that file.

SDL2_image currently supports the following formats:

- BMP
- GIF
- LBM
- PCX
- PNM
- SPRITE (from libragon's `mksprite` tool)
- TGA
- XCF
- XPM
- XV

**PRs are always welcome.**

## Usage

### SDL 1

Copy all lib/SDL into your project.

Set to compile all `.c` files inside that folder.

Include the folder to use `<SDL/SDL.h>` or `<SDL.h>` in your project.

Sample Makefile:
```Makefile
V=1
SOURCE_DIR=src
BUILD_DIR=build
include $(N64_INST)/include/n64.mk

# adding the lib folder here
N64_CFLAGS += -Ilib/

all: hello.z64
.PHONY: all

# adding files to compile
SDL := $(wildcard lib/SDL/*.c)
# choose one below
OBJS = $(BUILD_DIR)/main.o $(SDL:%.c=%.o)

hello.z64: N64_ROM_TITLE="Hello World"

$(BUILD_DIR)/hello.elf: $(OBJS)

clean:
	rm -f $(BUILD_DIR) *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
```

### SDL 2

Include the source as submodule or copy this repository.

Include `/SDL2/Makefile_sdl2.mk` into your makefile.

Add `$(C_SDL_FILES)` to your C source files for compilation, and set `SDL2_ROOT_DIR` to the root of the SDL2 folder (`/SDL2` on this repo).

You may also need to relocate the DFS file inside your ROM due to the amount of code that is inside SDL2. So use `N64_DFS_OFFSET` to set it accordingly.

You can find Makefiles using SDL2 [here](https://github.com/stefanmielke/opentyrian/blob/master/Makefile) and [here](https://github.com/stefanmielke/nKaruga_n64/blob/master/Makefile.n64).

### SDL2_Image

Include the source as submodule or copy this repository.

Include `/SDL2_image/Makefile_sdl2_image.mk` into your makefile.

Add `$(C_SDL_IMAGE_FILES)` to your C source files for compilation, and set `SDL2_IMAGE_ROOT_DIR` to the root of the SDL2 folder (`/SDL2_image` on this repo).
