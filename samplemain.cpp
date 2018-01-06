#include <cstdio>
#include <cstdlib>
#include <cassert>

#ifdef RUN_RGBA
#include "luminor_rgba.h"
#define LUMINOR_API luminor_rgba
#define SAMPLEMAIN "samplemain_rgba"
#define COLORS 4
#else
#include "luminor.h"
#define LUMINOR_API luminor
#define SAMPLEMAIN "samplemain"
#define COLORS 3
#endif


#include "halide_benchmark.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

using namespace Halide::Tools;
using namespace Halide::Runtime;

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("Usage: ./" SAMPLEMAIN " input.png output.png b_sigma, c_sigma, g_sigma\n"
               "e.g. ./" SAMPLEMAIN " input.png output.png 0, 1.0, 1.0\n");
        return 0;
    }

    float b_sigma = (float) atof(argv[3]);
    float c_sigma = (float) atof(argv[4]);
    float g_sigma = (float) atof(argv[5]);

    Buffer<uint8_t> input = load_and_convert_image(argv[1]);
    Buffer<uint8_t> output(input.width(), input.height(), COLORS);

    LUMINOR_API(input, b_sigma, c_sigma, g_sigma, output);

    // Timing code. Timing doesn't include copying the input data to
    // the gpu or copying the output back.

    // Manually-tuned version
    int times = 10;
    double min_t_manual = benchmark(1, times, [&]() {
        LUMINOR_API(input, b_sigma, c_sigma, g_sigma, output);
    });
    printf("Manually-tuned time: %gms in %d times\n", min_t_manual * 1e3, times);

    convert_and_save_image(output, argv[2]);

    return 0;
}
