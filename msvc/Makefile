
#ARCH=x86
ARCH=x64
HALIDE_BIN_PATH = E:\local\gnu\halide-20171030\$(ARCH)
HALIDE_SRC_PATH = ../../Halide
BIN = ./bin
HL_TARGET = host


CXX = cl
GXX = cl
LD = link
PYTHON = python3

WFLAGS  = -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE
CFLAGS =  -nologo -c $(CFLAGS) $(WFLAGS) -I $(HALIDE_BIN_PATH)/include/ -I $(HALIDE_SRC_PATH)/tools/ -I $(HALIDE_SRC_PATH)/apps/support/
CXXFLAGS = $(CXXFLAGS) /O2 /Oi /Ot /GL /EHsc  /favor:blend /MD /Qpar /openmp /arch:AVX2  -I $(HALIDE_BIN_PATH)/include/ -I $(HALIDE_SRC_PATH)/tools/ -I $(HALIDE_SRC_PATH)/apps/support/
LDFLAGS = -nologo $(LDFLAGS)   -incremental:no /LTCG

# Flags for mingw environment
LDFLAGS = $(LDFLAGS) ole32.lib kernel32.lib psapi.lib uuid.lib

LIB_HALIDE = $(HALIDE_BIN_PATH)/Release/Halide.lib
FUNCTION = luminor

LIBPNG_LIBS_DEFAULT = /link /LIBPATH:"E:\Home\src\opensource\clang\libpng\projects\vstudio\$(ARCH)\Release" /link /LIBPATH:"E:\Home\src\opensource\clang\zlib-1.2.8"
LIBPNG_CXX_FLAGS = -I E:\Home\src\opensource\clang\libpng -I E:\Home\src\opensource\clang\zlib-1.2.8
# Workaround for libpng-config pointing to 64-bit versions on linux even when we're building for 32-bit
LIBPNG_LIBS = $(LIBPNG_LIBS_DEFAULT) libpng16.lib zlib.lib

LIBJPEG_CXX_FLAGS = -I E:\Home\src\opensource\clang\jpeg-9b
LIBJPEG_LIBS = /link /LIBPATH:"E:\Home\src\opensource\clang\jpeg-9b\$(ARCH)\Release" jpeg.lib

IMAGE_IO_CXX_FLAGS = $(LIBPNG_CXX_FLAGS) $(LIBJPEG_CXX_FLAGS)
IMAGE_IO_LIBS = $(LIBPNG_LIBS) $(LIBJPEG_LIBS)

all: samplemain.exe

clean:
	rm -f *.obj samplemain samplemain.exe samplemain_rgba samplemain_rgba.exe test* luminor.* luminor_rgba.* luminor_gen luminor_gen.exe Halide.dll

$(FUNCTION)_gen.exe: ../$(FUNCTION)_gen.cpp $(HALIDE_SRC_PATH)/tools/GenGen.cpp
	$(CXX) $(CXXFLAGS)  -I. $? $(LIB_HALIDE)

Halide.dll: $(HALIDE_BIN_PATH)/Release/Halide.dll
	cp $? .

$(FUNCTION).lib: $(FUNCTION)_gen.exe Halide.dll
	$(FUNCTION)_gen -g $(FUNCTION) -o . target=host

samplemain.exe: ../samplemain.cpp $(FUNCTION).lib
	$(CXX) $(CXXFLAGS) -I. $? $(IMAGE_IO_CXX_FLAGS) $(IMAGE_IO_LIBS)

test: all
	samplemain ../images/rgb.png test_0_10_10.png 0 1.0 1.0
	samplemain ../images/rgb.png test_100_10_10.png 100 1.0 1.0
	samplemain ../images/rgb.png test_0_20_10.png 0 2.0 1.0
	samplemain ../images/rgb.png test_0_10_20.png 0 1.0 2.0

$(FUNCTION)_rgba.lib: $(FUNCTION)_gen.exe Halide.dll
	$(FUNCTION)_gen -g $(FUNCTION)_rgba -o . target=host

samplemain_rgba.exe: ../samplemain.cpp $(FUNCTION)_rgba.lib
	$(CXX) /DRUN_RGBA  $(CXXFLAGS) -I. $? $(IMAGE_IO_CXX_FLAGS) $(IMAGE_IO_LIBS) /link /OUT:$@

testA: samplemain_rgba.exe
	samplemain_rgba ../images/rgba.png testa_0_10_10.png 0 1.0 1.0
	samplemain_rgba ../images/rgba.png testa_100_10_10.png 100 1.0 1.0
	samplemain_rgba ../images/rgba.png testa_0_20_10.png 0 2.0 1.0
	samplemain_rgba ../images/rgba.png testa_0_10_20.png 0 1.0 2.0
