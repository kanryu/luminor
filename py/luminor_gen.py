#!/usr/bin/python3

# it need to import the Halide-language python_binding,
# you must build it yourself for run this script
from halide import *

import numpy as np
from scipy.misc import imread, imsave
import os.path

int_t = Int(32)
uint8_t = UInt(8)
float_t = Float(32)

def get_luminor(input, b_sigma, c_sigma, g_sigma):
    return get_luminorImpl(input, b_sigma, c_sigma, g_sigma, False)

def get_luminorRGBA(input, b_sigma, c_sigma, g_sigma):
    return get_luminorImpl(input, b_sigma, c_sigma, g_sigma, True)

def get_luminorImpl(input, b_sigma, c_sigma, g_sigma, with_alpha):
    """
    input: a Buffer as an bitmap image on memory
    b_sigma: float value for brightness(default: 0.0, range: -255 to 255)
    c_sigma: float value for contrast(default: 1.0, range: 0.1 to 10.0)
    g_sigma: float value for gamma value(default: 1.0, range: 0.1 to 10.0)
    """

    assert type(input) == ImageParam
    assert input.dimensions() == 3

    x, y, c = Var("x"), Var("y"), Var("c")
    v = Var("v")

    px = cast(float_t, v)
    px = px * (1.0/255.0)

    # change brightness
    px = px + b_sigma * (1.0/255.0)

    # change contrast
    px = (px-0.5)*c_sigma+0.5

    # change gamma value
    px = pow(px, 1.0/g_sigma)

    px = max(px, 0.0)
    px = min(px, 1.0)

    px = cast(uint8_t, px*255)

    # look-up-table for the luminor
    luminor_table = Func('luminor_table')
    luminor_table[v] = px
    luminor_table.compute_root().vectorize(v, 16)

    luminor = Func('luminor')
    luminor.store_root().compute_at(luminor_table, v)
    luminor[x, y, c] = luminor_table[input[x, y, c]]

    # for RGBA pixels, Alpha is not modified
    if with_alpha:
        luminor[x, y, 3] = input[x, y, 3]

    luminor.compute_root().parallel(y).vectorize(x, 16)

    return luminor


GRAY=False
WITH_ALPHA=False

def get_input_data():

    if GRAY:
        image_path = os.path.join(os.path.dirname(__file__), "../images/gray.png")
    elif WITH_ALPHA:
        image_path = os.path.join(os.path.dirname(__file__), "../images/rgba.png")
    else:
        image_path = os.path.join(os.path.dirname(__file__), "../images/rgb.png")

    assert os.path.exists(image_path), \
        "Could not find %s" % image_path
    rgb_data = imread(image_path)
    print("rgb_data", type(rgb_data), rgb_data.shape, rgb_data.dtype)

    # grey_data = np.mean(rgb_data, axis=2, dtype=np.float32).astype(rgb_data.dtype)
    # input_data = np.copy(grey_data, order="F")
    if GRAY:
        rgb_data = np.reshape(rgb_data, rgb_data.shape+(1,))

    input_data = np.copy(rgb_data, order="F")

    return input_data

def main():

    # define and compile the function
    b_sigma = Param(float_t, 'brightness', 0.0)
    c_sigma = Param(float_t, 'contrast', 1.0)
    g_sigma = Param(float_t, 'gamma',    0.5)

    input = ImageParam(uint8_t, 3, "input_buffer")
    if WITH_ALPHA:
        luminor = get_luminorRGBA(input, b_sigma, c_sigma, g_sigma)
    else:
        luminor = get_luminor(input, b_sigma, c_sigma, g_sigma)
    luminor.compile_jit()

    # preparing input and output memory buffers (numpy ndarrays)
    input_data = get_input_data()
    input_image = Buffer(input_data)
    input.set(input_image)

    output_data = np.empty(input_data.shape, dtype=input_data.dtype, order="F")
    output_image = Buffer(output_data)

    print("input_image", input_image)
    print("output_image", output_image)

    # do the actual computation
    # print(dir(luminor))
    # exit()
    luminor.realize(output_image)
#    luminor.compile_to_lowered_stmt("luminor.html", [input, b_sigma, c_sigma, g_sigma], StmtOutputFormat.HTML);
    #luminor.compile_to_static_library("luminor.a", [input, b_sigma, c_sigma, g_sigma]);
    #luminor.compile_to_header("luminor.h", [input, b_sigma, c_sigma, g_sigma]);
    # luminor.compile_to_c("luminor.cpp", [input, b_sigma, c_sigma, g_sigma]);
    luminor.compile_to_file("luminor", [input, b_sigma, c_sigma, g_sigma]);
    # luminor.compile_to_assembly("luminor", [input, b_sigma, c_sigma, g_sigma]);

    # save results
    input_path = "luminor_input.png"
    output_path = "luminor_result.png"
    if GRAY:
        input_data = np.reshape(input_data, input_data.shape[:2])
        output_data = np.reshape(output_data, output_data.shape[:2])
    imsave(input_path, input_data)
    imsave(output_path, output_data)
    print("\nluminor realized on output image.",
          "Result saved at", output_path,
          "( input data copy at", input_path, ")")

    print("\nEnd of game. Have a nice day!")
    return


if __name__ == "__main__":
    main()
