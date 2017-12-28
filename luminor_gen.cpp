#include "Halide.h"

namespace {

	class Luminor : public Halide::Generator<Luminor> {
	public:
		Input<Buffer<uint8_t>>  input{ "input", 3 };
		Input<float>   b_sigma{ "brightness", 0.0 };
		Input<float>   c_sigma{ "contrast", 1.0 };
		Input<float>   g_sigma{ "gamma", 0.5 };


		Output<Buffer<uint8_t>> luminor{ "output", 3 };
//	Output<Buffer<uint8_t>>	luminorTable{ 1 };

		void generate() {
			Var x("x"), y("y"), z("z"), c("c");
		    Var v("v");

//			Expr px = input(x, y, c);
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
//			Func luminor("luminor");
			luminor.store_root().compute_at(luminorTable, v);
			luminor(x, y, c) = luminorTable(input(x, y, c));

			// The CPU schedule.
			luminor.compute_root().parallel(y).vectorize(x, 16);
		}
	};
	class LuminorRGBA : public Halide::Generator<Luminor> {
	public:
		Input<Buffer<uint8_t>>  input{ "input", 3 };
		Input<float>   b_sigma{ "brightness", 0.0 };
		Input<float>   c_sigma{ "contrast", 1.0 };
		Input<float>   g_sigma{ "gamma", 0.5 };


		Output<Buffer<uint8_t>> luminor_rgba{ "output", 3 };
//Output<Buffer<uint8_t>>	luminorTable{  1 };

		void generate() {
			Var x("x"), y("y"), z("z"), c("c");
		    Var v("v");

//			Expr px = input(x, y, c);
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
			luminor_rgba(x, y, c) = luminorTable(input(x, y, c));
			luminor_rgba(x, y, 3) = input(x, y, 3);


			// The CPU schedule.
			luminor_rgba.compute_root().parallel(y).vectorize(x, 16);
		}
	};

}  // namespace

HALIDE_REGISTER_GENERATOR(Luminor, luminor)
HALIDE_REGISTER_GENERATOR(LuminorRGBA, luminor_rgba)
