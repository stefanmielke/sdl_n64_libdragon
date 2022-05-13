# Nintendo 64 port of SDL 1 and 2

It includes all header files used by SDL and SDL2, but not necessarily implement all functions.

If you need a function and it's not implemented it should give linker errors. You are free to implement those functions anywhere in your project, as long as you compile that file.

**PRs are always welcome.**

## Usage

Copy all lib/SDL or lib/SDL2 into your project.

Set to compile all `.c` files inside that folder.

Include the folder to use `<SDL/SDL.h>` or `<SDL2/SDL.h>` in your project.

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
SDL2 := $(wildcard lib/SDL2/*.c)
# choose one below
OBJS = $(BUILD_DIR)/main.o $(SDL:%.c=%.o) OR $(SDL2:%.c=%.o)

hello.z64: N64_ROM_TITLE="Hello World"

$(BUILD_DIR)/hello.elf: $(OBJS)

clean:
	rm -f $(BUILD_DIR) *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
```
