SDL2_ROOT_DIR ?= libs/sdl_n64/SDL2
SDL2_IMAGE_ROOT_DIR ?= libs/sdl_n64/SDL2_image

N64_CFLAGS   += -D_POSIX_C_SOURCE=0 -D__MIPS__ -I$(SDL2_IMAGE_ROOT_DIR)/include -I$(SDL2_IMAGE_ROOT_DIR)/src -I$(SDL2_ROOT_DIR)/include
N64_CXXFLAGS += -I$(SDL2_IMAGE_ROOT_DIR)/include

# miniz usage
N64_CFLAGS += -DSPNG_USE_MINIZ -DMINIZ_NO_TIME -D__STRICT_ANSI__ -DSPNG_DISABLE_OPT

# supported files
N64_CFLAGS += -DLOAD_BMP
N64_CFLAGS += -DLOAD_SPRITE

# warnings from original code
N64_CFLAGS += -Wno-error=unused-but-set-variable -Wno-error=unused-variable

C_SDL_IMAGE_FILES := $(wildcard ${SDL2_IMAGE_ROOT_DIR}/src/*.c)