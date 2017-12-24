HALIDE_BIN_PATH ?= ../../Halide
HALIDE_SRC_PATH ?= ../../Halide
LDFLAGS ?=
BIN ?= ./bin
IMAGES ?= ../images
HL_TARGET ?= host
UNAME ?= $(shell uname)

ifeq ($(OS), Windows_NT)
# Flags for mingw environment
LDFLAGS += -lole32 -lkernel32 -lpsapi -luuid -Wl,--stack,8388608
else
LDFLAGS += -ldl -lpthread -lz
endif

CXX ?= g++
GXX ?= g++
AS ?= as
PYTHON ?= python3

CFLAGS += -I $(HALIDE_BIN_PATH)/include/ -I $(HALIDE_SRC_PATH)/tools/ -I $(HALIDE_SRC_PATH)/apps/support/
CXXFLAGS += -std=c++11 -I $(HALIDE_BIN_PATH)/include/ -I $(HALIDE_SRC_PATH)/tools/ -I $(HALIDE_SRC_PATH)/apps/support/ -fopenmp

ifeq ($(UNAME), Darwin)
CXXFLAGS += -fvisibility=hidden
endif

LIB_HALIDE = $(HALIDE_BIN_PATH)/lib/libHalide.a

HALIDE_RUNTIME = halide_mingw64.a

LIBPNG_LIBS_DEFAULT = $(shell libpng-config --ldflags)
LIBPNG_CXX_FLAGS ?= $(shell libpng-config --cflags)
# Workaround for libpng-config pointing to 64-bit versions on linux even when we're building for 32-bit
ifneq (,$(findstring -m32,$(CXX)))
ifneq (,$(findstring x86_64,$(LIBPNG_LIBS_DEFAULT)))
LIBPNG_LIBS ?= -lpng
endif
endif
LIBPNG_LIBS ?= $(LIBPNG_LIBS_DEFAULT)

# Workaround brew Cellar path for libpng-config output.
LIBJPEG_LINKER_PATH ?= $(shell echo $(LIBPNG_LIBS_DEFAULT) | sed -e'/-L.*[/][Cc]ellar[/]libpng/!d;s=\(.*\)/[Cc]ellar/libpng/.*=\1/lib=')
LIBJPEG_LIBS ?= $(LIBJPEG_LINKER_PATH) -ljpeg

# There's no libjpeg-config, unfortunately. We should look for
# jpeglib.h one directory level up from png.h . Also handle
# Mac OS brew installs where libpng-config returns paths
# into the PNG cellar.
LIBPNG_INCLUDE_DIRS = $(filter -I%,$(LIBPNG_CXX_FLAGS))
LIBJPEG_CXX_FLAGS ?= $(shell echo $(LIBPNG_INCLUDE_DIRS) | sed -e'/[Cc]ellar[/]libpng/!s=\(.*\)=\1/..=;s=\(.*\)/[Cc]ellar/libpng/.*=\1/include=')

IMAGE_IO_LIBS = $(LIBPNG_LIBS) $(LIBJPEG_LIBS)
IMAGE_IO_CXX_FLAGS = $(LIBPNG_CXX_FLAGS) $(LIBJPEG_CXX_FLAGS)

IMAGE_IO_FLAGS = $(IMAGE_IO_LIBS) $(IMAGE_IO_CXX_FLAGS)

.PHONY: clean test

all: samplemain

luminor.o: luminor_gen.py
	$(PYTHON) $?

samplemain: luminor.o ../samplemain.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -O3 -ffast-math -Wall -Werror -I. $? -o $@ $(IMAGE_IO_FLAGS) $(LDFLAGS)

clean:
	rm -f samplemain samplemain.exe test* luminor.*

test: all
	./samplemain luminor_input.png test_0_10_10.png 0 1.0 1.0
	./samplemain luminor_input.png test_100_10_10.png 100 1.0 1.0
	./samplemain luminor_input.png test_0_20_10.png 0 2.0 1.0
	./samplemain luminor_input.png test_0_10_20.png 0 1.0 2.0
