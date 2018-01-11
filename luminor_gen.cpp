#include "Halide.h"

#ifndef BUILD_RGBA

namespace {

    class Luminor : public Halide::Generator<Luminor> {
    public:
        Input<Buffer<uint8_t>>  input{ "input", 3 };
        Input<float>   b_sigma{ "brightness", 0.0 };
        Input<float>   c_sigma{ "contrast", 1.0 };
        Input<float>   g_sigma{ "gamma", 0.5 };


        Output<Buffer<uint8_t>> luminor{ "output", 3 };

        void generate() {
            Var x("x"), y("y"), z("z"), c("c");
            Var v("v");

            Expr px = cast<float_t>(v);
            
            // change brightness
            px = px + b_sigma;

            px = px * (1.0f / 255.0f);

            // change contrast
            px = (px - 0.5f)*c_sigma + 0.5f;

            // change gamma value
            px = Halide::pow(px, 1.0f / g_sigma);

            px = Halide::max(px, 0.0f);
            px = Halide::min(px, 1.0f);

            px = cast<uint8_t>(px * 255);

            Func luminorTable("luminor_table");
            luminorTable(v) = px;
            luminorTable.compute_root().vectorize(v, 16);

            // Construct the luminor
            luminor.store_root().compute_at(luminorTable, v);
            luminor(x, y, c) = luminorTable(input(x, y, c));

            // The CPU schedule.
            luminor.compute_root().parallel(y).vectorize(x, 16);
        }
    };
}  // namespace

HALIDE_REGISTER_GENERATOR(Luminor, luminor)

#else

namespace {

    class LuminorRGBA : public Halide::Generator<LuminorRGBA> {
    public:
        Input<Buffer<uint8_t>>  input{ "input", 3 };
        Input<float>   b_sigma{ "brightness", 0.0 };
        Input<float>   c_sigma{ "contrast", 1.0 };
        Input<float>   g_sigma{ "gamma", 0.5 };


        Output<Buffer<uint8_t>> luminor_rgba{ "output", 3 };
        const int PIXEL_STRIDE = 4;// 3 as RGB, 4 as RGBA

        void generate() {
            Var x("x"), y("y"), z("z"), c("c");
            Var v("v");

            Expr px = cast<float_t>(v);
            
            // change brightness
            px = px + b_sigma;

            px = px * (1.0f / 255.0f);

            // change contrast
            px = (px - 0.5f)*c_sigma + 0.5f;

            // change gamma value
            px = Halide::pow(px, 1.0f / g_sigma);

            px = Halide::max(px, 0.0f);
            px = Halide::min(px, 1.0f);

            px = cast<uint8_t>(px * 255);

            Func luminorTable("luminor_table");
            luminorTable(v) = px;
            luminorTable.compute_root().vectorize(v, 16);

            // Construct the luminor
            luminor_rgba.store_root().compute_at(luminorTable, v);
            luminor_rgba(x, y, c) = select(c==3, input(x, y, 3), 
                                                 luminorTable(input(x, y, c)));

            // The CPU schedule.
            luminor_rgba.compute_root().parallel(y).vectorize(x, 16);
            
            // via http://halide-lang.org/tutorials/tutorial_lesson_16_rgb_generate.html
            // Another common format is 'interleaved', in which the
            // red, green, and blue values for each pixel occur next
            // to each other in memory:
            //
            // RGBRGBRGBRGBRGBRGBRGBRGB
            // RGBRGBRGBRGBRGBRGBRGBRGB
            // RGBRGBRGBRGBRGBRGBRGBRGB
            // RGBRGBRGBRGBRGBRGBRGBRGB
            //
            // In this case the stride in x is three, the stride in y
            // is three times the width of the image, and the stride
            // in c is one. We can tell Halide to assume (and assert)
            // that this is the case for the input and output like so:
            input
                .dim(0).set_stride(PIXEL_STRIDE) // stride in dimension 0 (x) is three
                .dim(2).set_stride(1); // stride in dimension 2 (c) is one
            luminor_rgba
                .dim(0).set_stride(PIXEL_STRIDE)
                .dim(2).set_stride(1);

            // For interleaved layout, you may want to use a different
            // schedule. We'll tell Halide to additionally assume and
            // assert that there are three color channels, then
            // exploit this fact to make the loop over 'c' innermost
            // and unrolled.

            input.dim(2).set_bounds(0, PIXEL_STRIDE); // Dimension 2 (c) starts at 0 and has extent 3.
            luminor_rgba.dim(2).set_bounds(0, PIXEL_STRIDE);

            // Move the loop over color channels innermost and unroll
            // it.
            luminor_rgba.reorder(c, x, y).unroll(c);

            // Note that if we were dealing with an image with an
            // alpha channel (RGBA), then the stride in x and the
            // bounds of the channels dimension would both be four
            // instead of three.
        }
    };

}  // namespace

HALIDE_REGISTER_GENERATOR(LuminorRGBA, luminor_rgba)
#endif

