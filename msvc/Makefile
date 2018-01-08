!INCLUDE ../VERSION.txt

#ARCH=x86
ARCH=x64
HALIDE_BIN_PATH = E:\local\gnu\halide-20171030\$(PLATFORM)
HALIDE_SRC_PATH = ../../Halide
BIN = ./bin
HL_TARGET = host

MSVC = msvc2015

!ifdef PLATFORM
PLATFORM = x64
!else
PLATFORM = x86
!endif


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

LIBPNG_LIBS_DEFAULT = /link /LIBPATH:"E:\Home\src\opensource\clang\libpng\projects\vstudio\$(PLATFORM)\Release" /link /LIBPATH:"E:\Home\src\opensource\clang\zlib-1.2.8"
LIBPNG_CXX_FLAGS = -I E:\Home\src\opensource\clang\libpng -I E:\Home\src\opensource\clang\zlib-1.2.8
# Workaround for libpng-config pointing to 64-bit versions on linux even when we're building for 32-bit
LIBPNG_LIBS = $(LIBPNG_LIBS_DEFAULT) libpng16.lib zlib.lib

LIBJPEG_CXX_FLAGS = -I E:\Home\src\opensource\clang\jpeg-9b
LIBJPEG_LIBS = /link /LIBPATH:"E:\Home\src\opensource\clang\jpeg-9b\$(PLATFORM)\Release" jpeg.lib

IMAGE_IO_CXX_FLAGS = $(LIBPNG_CXX_FLAGS) $(LIBJPEG_CXX_FLAGS)
IMAGE_IO_LIBS = $(LIBPNG_LIBS) $(LIBJPEG_LIBS)

PACKAGE = luminor-$(VERSION)-$(MSVC)-$(PLATFORM)

all: samplemain.exe

clean:
	rm -rf *.obj samplemain samplemain.exe samplemain_rgba samplemain_rgba.exe test* luminor.* luminor_rgba.* luminor_gen luminor_gen.exe luminor_rgba_gen.exe Halide.dll halide_runtime.lib $(PACKAGE)

$(FUNCTION)_gen.exe: ../$(FUNCTION)_gen.cpp $(HALIDE_SRC_PATH)/tools/GenGen.cpp
	$(CXX) $(CXXFLAGS)  -I. $? $(LIB_HALIDE)

Halide.dll: $(HALIDE_BIN_PATH)/Release/Halide.dll
	cp $? .

$(FUNCTION).lib: $(FUNCTION)_gen.exe Halide.dll
	$(FUNCTION)_gen -g $(FUNCTION) -o . target=host-no_runtime

halide_runtime.lib: $(FUNCTION)_gen.exe Halide.dll
	$(FUNCTION)_gen -r halide_runtime -o . target=host

samplemain.exe: ../samplemain.cpp $(FUNCTION).lib halide_runtime.lib
	$(CXX) $(CXXFLAGS) -I. $? $(IMAGE_IO_CXX_FLAGS) $(IMAGE_IO_LIBS)

test: all
	samplemain ../images/rgb.png test_0_10_10.png 0 1.0 1.0
	samplemain ../images/rgb.png test_100_10_10.png 100 1.0 1.0
	samplemain ../images/rgb.png test_0_20_10.png 0 2.0 1.0
	samplemain ../images/rgb.png test_0_10_20.png 0 1.0 2.0

luminor_rgba_gen.exe: ../$(FUNCTION)_gen.cpp $(HALIDE_SRC_PATH)/tools/GenGen.cpp
	$(CXX) $(CXXFLAGS) -DBUILD_RGBA  -I. $? $(LIB_HALIDE) /link /OUT:$@

$(FUNCTION)_rgba.lib: luminor_rgba_gen.exe Halide.dll
	luminor_rgba_gen -g $(FUNCTION)_rgba -o . target=host-no_runtime
#	$(FUNCTION)_gen -g $(FUNCTION)_rgba -o . target=host-no_runtime

samplemain_rgba.exe: ../samplemain.cpp $(FUNCTION)_rgba.lib halide_runtime.lib
	$(CXX) /DRUN_RGBA  $(CXXFLAGS) -I. $? $(IMAGE_IO_CXX_FLAGS) $(IMAGE_IO_LIBS) /link /OUT:$@

testA: samplemain_rgba.exe
	samplemain_rgba azu.png testa_0_10_10.png 0 1.0 1.0
	samplemain_rgba azu.png testa_100_10_10.png 100 1.0 1.0
	samplemain_rgba azu.png testa_0_20_10.png 0 2.0 1.0
	samplemain_rgba azu.png testa_0_10_20.png 0 1.0 2.0
	samplemain_rgba azu.png testa_0_05_10.png 0 0.5 1.0
	samplemain_rgba azu.png testa_0_10_05.png 0 1.0 0.5
	samplemain_rgba azu.png testa_-50_10_10.png -50 1.0 1.0
	samplemain_rgba rrgb.png testa_-100_10_10.png -100 1.0 1.0

version:
	echo $(VERSION)
	echo $(PLATFORM)

deploy:
	rm -rf $(PACKAGE)
	mkdir $(PACKAGE)
	cp *.h *.lib ../HalideBuffer.h ../HalideRuntime.h ../LICENSE ../README.md $(PACKAGE)
	7z a ../$(PACKAGE).zip $(PACKAGE)

