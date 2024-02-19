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

template <pixel_format T>
struct color_t : math::vec3<float>
{};

template <>
struct color_t<Oklab> : math::vec3<float>
{
    // clang-format off
    union { float l; float x; };
    union { float a; float y; };
    union { float b; float z; };
    // clang-format on

    color_t(float l, float a, float b);
};

template <>
struct color_t<RGB_8> : math::vec3<uint8_t>
{
    // clang-format off
    union { uint8_t r; uint8_t x; };
    union { uint8_t g; uint8_t y; };
    union { uint8_t b; uint8_t z; };
    // clang-format on

    color_t(uint8_t r, uint8_t g, uint8_t b);
    color_t<Oklab> toOklab();
    color_t<RGB_f32> toRGB_f32();
    color_t<LinearRGB_8> toLinearRGB_8();
    color_t<LinearRGB_f32> toLinearRGB_f32();
};

template <>
struct color_t<RGB_f32> : math::vec3<float>
{
    // clang-format off
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };
    // clang-format on

    color_t(float r, float g, float b);
    color_t<Oklab> toOklab();
    color_t<RGB_8> toRGB_8();
    color_t<LinearRGB_8> toLinearRGB_8();
    color_t<LinearRGB_f32> toLinearRGB_f32();
};

template <>
struct color_t<LinearRGB_8> : math::vec3<uint8_t>
{
    // clang-format off
    union { uint8_t r; uint8_t x; };
    union { uint8_t g; uint8_t y; };
    union { uint8_t b; uint8_t z; };
    // clang-format on

    color_t<RGB_8> toRGB_8();
    color_t<RGB_f32> toRGB_f32();
    color_t<Oklab> toOklab();
    color_t<LinearRGB_f32> toLinearRGB_f32();
};

template <>
struct color_t<LinearRGB_f32> : math::vec3<float>
{
    // clang-format off
    union { float r; float x; };
    union { float g; float y; };
    union { float b; float z; };
    // clang-format on

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