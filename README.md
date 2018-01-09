Luminor
=============

A simple and fast library that changes the brightness, contrast, and gamma value of on-memory images in real time.

Summary
-------

- A simple C/C++ language API
- Optimized processing automatically generated with Halide language
- It consists of C/C++ header file and static library

These is a part of [QuickViewer](https://github.com/kanryu/quickviewer).

Binary Releases
---------------
https://github.com/kanryu/luminor/releases


API
---

int luminor(struct halide_buffer_t *_input_buffer, float _brightness, float _contrast, float _gamma, struct halide_buffer_t *_output_buffer)

- _input_buffer
    - input bitmap of rgb24 or gray8
    - gray8 should be initialized as 3D Buffer(width, height, 1)
- _brightness
    - Add the value to each pixel
    - default: 0.0, min: -128.0, max: 127.0
- _contrast
    - Multiply each pixel by the value(the base value is 0.5(128))
    - default: 1.0, min: 0.01, max: 10.0
- _gamma
    - Change the gamma value of each pixel
    - It gets brighter when it gets bigger, it gets darker when it gets smaller
    - default: 1.0, min: 0.01, max: 10.0
- _output_buffer
    - output bitmap, the same pixel format as _input_buffer

int luminor_rgba(struct halide_buffer_t *_input_buffer, float _brightness, float _contrast, float _gamma, struct halide_buffer_t *_output_buffer)

- _input_buffer
    - input bitmap of rgba32
    - alpha byte must be at 4th byte of each pixel


How to use
------------

Only to include **luminor.h**, **HalideBuffer.h**  and link **luminor.a**, the API will work.

Details to see https://github.com/kanryu/quickviewer/blob/master/luminor/qluminor.cpp


How to build
------------

You have to get [Halide](https://github.com/halide/Halide).
If you just build the library binary version of Halide is enough.
However, if you want to check the operation of the sample program, also download Halide source code.

And,

    $ make

License
-------
MIT

Author
------

Copyright (c) 2017, KATO Kanryu
