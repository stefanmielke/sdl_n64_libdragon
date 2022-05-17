SDL2_SOURCE_DIR ?= libs/sdl_n64/SDL2/src

N64_CFLAGS += -D_POSIX_C_SOURCE=0 -D__MIPS__

# warnings from original code
N64_CFLAGS += -Wno-error=format-truncation -Wno-format-truncation -Wno-error=format -Wno-error=array-bounds
N64_CFLAGS += -Wno-error=format-overflow= -Wno-error=restrict -Wno-error=maybe-uninitialized -Wno-error=use-after-free -Wno-error=pointer-to-int-cast

C_SDL_FILES  := $(wildcard ${SDL2_SOURCE_DIR}/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/atomic/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/audio/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/cpuinfo/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/dynapi/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/events/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/file/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/haptic/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/hidapi/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/libm/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/locale/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/misc/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/power/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/render/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/render/*/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/sensor/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/stdlib/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/thread/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/timer/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/video/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/video/yuv2rgb/*.c)

C_SDL_FILES  += $(wildcard ${SDL2_SOURCE_DIR}/audio/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/sensor/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/haptic/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/locale/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/misc/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/thread/generic/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/joystick/*.c)

C_SDL_FILES  += $(wildcard ${SDL2_SOURCE_DIR}/timer/n64/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/joystick/n64/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/video/n64/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/render/n64/*.c)