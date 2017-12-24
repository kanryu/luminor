#include "Halide.h"

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

			Expr px = input(x, y, c);
			px = cast<float>(px);
			px = px * (1.0f / 255.0f);

			// change brightness
			px = px + b_sigma * (1.0f / 255.0f);

			// change contrast
			px = (px - 0.5f)*c_sigma + 0.5f;

			// change gamma value
			px = Halide::pow(px, 1.0f / g_sigma);

			px = Halide::max(px, 0.0f);
			px = Halide::min(px, 1.0f);

			px = cast<uint8_t>(px * 255);

			// Construct the luminor
//			Func luminor("luminor");
			luminor(x, y, c) = px;



			// The CPU schedule.
			luminor.compute_root().parallel(y).vectorize(x, 16);
		}
	};

}  // namespace

HALIDE_REGISTER_GENERATOR(Luminor, luminor)
