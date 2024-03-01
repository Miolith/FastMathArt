#include <cstdint>

#include "../math/vec.h"
#include "../render.h"

enum pixel_format
{
    RGB_8 = 0,
    RGB_f32,
    LinearRGB_8,
    LinearRGB_f32,
    Oklab
};

template <pixel_format F>
struct _color_implem
{ };

template <pixel_format F>
struct color_t : _color_implem<F>
{
    using type_x = decltype(_color_implem<F>::x);
    using type_y = decltype(_color_implem<F>::y);
    using type_z = decltype(_color_implem<F>::z);

    color_t(type_x x, type_y y, type_z z)
        : _color_implem<F>(x, y, z)
    { }

    friend color_t<F> operator+(const color_t<F> &lhs, const color_t<F> &rhs)
    {
        return color_t<F>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    friend color_t<F> operator-(const color_t<F> &lhs, const color_t<F> &rhs)
    {
        return color_t<F>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    friend color_t<F> operator*(const color_t<F> &lhs, const color_t<F> &rhs)
    {
        return color_t<F>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    friend color_t<F> blend(const color_t<F> &a, const color_t<F> &b, float t)
    {
        return color_t<F>(a.x * (1.0f - t) + b.x * t,
                          a.y * (1.0f - t) + b.y * t,
                          a.z * (1.0f - t) + b.z * t);
    }

    friend color_t<F> operator*(const color_t<F> &lhs, float rhs)
    {
        return color_t<F>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
    }
};

template <>
struct _color_implem<Oklab>
{
    // clang-format off
    union { float l; float x; };
    union { float a; float y; };
    union { float b; float z; };
    // clang-format on

    _color_implem(float l, float a, float b);
};

template <>
struct _color_implem<RGB_8>
{
    // clang-format off
    union { uint8_t r; uint8_t x; };
    union { uint8_t g; uint8_t y; };
    union { uint8_t b; uint8_t z; };
    // clang-format on

    _color_implem(uint8_t r, uint8_t g, uint8_t b);
    color_t<Oklab> toOklab();
    color_t<RGB_f32> toRGB_f32();
    color_t<LinearRGB_8> toLinearRGB_8();
    color_t<LinearRGB_f32> toLinearRGB_f32();
};

template <>
struct _color_implem<RGB_f32>
{
    // clang-format off
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };
    // clang-format on

    _color_implem(float r, float g, float b);
    color_t<Oklab> toOklab();
    color_t<RGB_8> toRGB_8();
    color_t<LinearRGB_8> toLinearRGB_8();
    color_t<LinearRGB_f32> toLinearRGB_f32();
};

template <>
struct _color_implem<LinearRGB_8>
{
    // clang-format off
    union { uint8_t r; uint8_t x; };
    union { uint8_t g; uint8_t y; };
    union { uint8_t b; uint8_t z; };
    // clang-format on

    _color_implem(uint8_t r, uint8_t g, uint8_t b);
    color_t<RGB_8> toRGB_8();
    color_t<RGB_f32> toRGB_f32();
    color_t<Oklab> toOklab();
    color_t<LinearRGB_f32> toLinearRGB_f32();
};

template <>
struct _color_implem<LinearRGB_f32>
{
    // clang-format off
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };
    // clang-format on

    _color_implem(float r, float g, float b);
    color_t<RGB_8> toRGB_8();
    color_t<RGB_f32> toRGB_f32();
    color_t<Oklab> toOklab();
    color_t<LinearRGB_8> toLinearRGB_8();
};

template <pixel_format P>
color_t<P> cast_to_color_t(PyAPI::Color &color);

template <>
color_t<RGB_f32> cast_to_color_t<RGB_f32>(PyAPI::Color &color);

struct pixel_buffer_t
{
    std::unique_ptr<uint8_t[]> buffer;
    int width;
    int height;

    pixel_buffer_t(int width, int height);
    pixel_buffer_t(pixel_buffer_t &&other);
    void set_pixel(int x, int y, const color_t<RGB_8> &color);
    color_t<RGB_8> get_pixel(int x, int y);
    void clear(const color_t<RGB_8> color = color_t<RGB_8>(0, 0, 0));
};

struct video_buffer_t
{
    std::unique_ptr<uint8_t[]> buffer;
    int width;
    int height;
    int frames;

    video_buffer_t(int width, int height, int frames);
    video_buffer_t(video_buffer_t &&other);
    void set_frame(const pixel_buffer_t &framebuffer, int frame_index);
    void set_all_frames(const pixel_buffer_t &framebuffer);
};