#include <cstdint>
#include "../math/vec.h"

enum pixel_format
{
    RGB_8 = 0,
    RGB_f32,
    LinearRGB,
    Oklab
};

template <pixel_format T>
struct color_t : math::vec3<float>
{};


template <>
struct color_t<Oklab> : math::vec3<float>
{
    union { float l; float x; };
    union { float a; float y; };
    union { float b; float z; };

    color_t(float l, float a, float b);
};

template <>
struct color_t<RGB_8> : math::vec3<uint8_t>
{
    union { uint8_t r; uint8_t x; };
    union { uint8_t g; uint8_t y; };
    union { uint8_t b; uint8_t z; };

    color_t<Oklab> toOklab();
    color_t<RGB_f32> toRGB_f32();
};

template <>
struct color_t<RGB_f32> : math::vec3<float>
{
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };

    color_t<Oklab> toOklab();
    color_t<RGB_8> toRGB_8();
};