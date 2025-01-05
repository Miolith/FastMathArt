#include "pixelUtils.h"

#include <cstdint>
#include <cstring>

/*
======================================
Oklab 32bits float per channel, member functions
======================================
*/

_color_implem<Oklab>::_color_implem(float _l, float _a, float _b)
    : l(_l)
    , a(_a)
    , b(_b)
{ }

/*
======================================
RGB 8 bits unsigned int per channel, member functions
======================================
*/

_color_implem<RGB_8>::_color_implem(uint8_t _r, uint8_t _g, uint8_t _b)
    : r(_r)
    , g(_g)
    , b(_b)
{ }

color_t<Oklab> _color_implem<RGB_8>::toOklab()
{
    float r = this->r / 255.0f;
    float g = this->g / 255.0f;
    float b = this->b / 255.0f;

    float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
    float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
    float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    float l_ = std::cbrt(l);
    float m_ = std::cbrt(m);
    float s_ = std::cbrt(s);

    float l_16 = 116.0f * l_ - 16.0f;
    float a_ = 500.0f * (l_ - m_);
    float b_ = 200.0f * (m_ - s_);

    return color_t<Oklab>(l_16, a_, b_);
}

color_t<RGB_f32> _color_implem<RGB_8>::toRGB_f32()
{
    return color_t<RGB_f32>{ static_cast<float>(this->r) / 255.0f,
                             static_cast<float>(this->g) / 255.0f,
                             static_cast<float>(this->b) / 255.0f };
}

color_t<LinearRGB_8> _color_implem<RGB_8>::toLinearRGB_8()
{
    static constexpr float gamma = 2.2f;
    return color_t<LinearRGB_8>{
        static_cast<uint8_t>(std::pow(this->r / 255.0f, gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->g / 255.0f, gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->b / 255.0f, gamma) * 255.0f)
    };
}

/*
======================================
RGB 32bits float per channel, member functions
======================================
*/

_color_implem<RGB_f32>::_color_implem(float _r, float _g, float _b)
    : r(_r)
    , g(_g)
    , b(_b)
{ }

color_t<RGB_8> _color_implem<RGB_f32>::toRGB_8()
{
    return color_t<RGB_8>(static_cast<uint8_t>(this->r * 255.0f),
                          static_cast<uint8_t>(this->g * 255.0f),
                          static_cast<uint8_t>(this->b * 255.0f));
}

color_t<Oklab> _color_implem<RGB_f32>::toOklab()
{
    float l = 0.4122214708f * this->r + 0.5363325363f * this->g
        + 0.0514459929f * this->b;
    float m = 0.2119034982f * this->r + 0.6806995451f * this->g
        + 0.1073969566f * this->b;
    float s = 0.0883024619f * this->r + 0.2817188376f * this->g
        + 0.6299787005f * this->b;

    float l_ = std::cbrt(l);
    float m_ = std::cbrt(m);
    float s_ = std::cbrt(s);

    float l_16 = 116.0f * l_ - 16.0f;
    float a_ = 500.0f * (l_ - m_);
    float b_ = 200.0f * (m_ - s_);

    return color_t<Oklab>(l_16, a_, b_);
}

color_t<LinearRGB_8> _color_implem<RGB_f32>::toLinearRGB_8()
{
    static constexpr float gamma = 2.2f;
    return color_t<LinearRGB_8>{
        static_cast<uint8_t>(std::pow(this->r, gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->g, gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->b, gamma) * 255.0f)
    };
}

color_t<LinearRGB_f32> _color_implem<RGB_f32>::toLinearRGB_f32()
{
    static constexpr float gamma = 2.2f;
    return color_t<LinearRGB_f32>{ std::pow(this->r, gamma),
                                   std::pow(this->g, gamma),
                                   std::pow(this->b, gamma) };
}

/*
======================================
LinearRGB 8 bits unsigned int per channel, member functions
======================================
*/

_color_implem<LinearRGB_8>::_color_implem(uint8_t _r, uint8_t _g, uint8_t _b)
    : r(_r)
    , g(_g)
    , b(_b)
{ }

color_t<RGB_8> _color_implem<LinearRGB_8>::toRGB_8()
{
    static constexpr float gamma = 2.2f;
    return color_t<RGB_8>{
        static_cast<uint8_t>(std::pow(this->r / 255.0f, 1.0f / gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->g / 255.0f, 1.0f / gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->b / 255.0f, 1.0f / gamma) * 255.0f)
    };
}

color_t<Oklab> _color_implem<LinearRGB_8>::toOklab()
{
    color_t<RGB_f32> rgb_f32 = this->toRGB_f32();
    return rgb_f32.toOklab();
}

color_t<RGB_f32> _color_implem<LinearRGB_8>::toRGB_f32()
{
    static constexpr float gamma = 2.2f;
    return color_t<RGB_f32>{ std::pow(this->r / 255.0f, 1.0f / gamma),
                             std::pow(this->g / 255.0f, 1.0f / gamma),
                             std::pow(this->b / 255.0f, 1.0f / gamma) };
}

color_t<LinearRGB_f32> _color_implem<LinearRGB_8>::toLinearRGB_f32()
{
    return color_t<LinearRGB_f32>{ this->r / 255.0f, this->g / 255.0f,
                                   this->b / 255.0f };
}

/*
======================================
LinearRGB 32bits float per channel, member functions
======================================
*/

_color_implem<LinearRGB_f32>::_color_implem(float _r, float _g, float _b)
    : r(_r)
    , g(_g)
    , b(_b)
{ }

color_t<RGB_8> _color_implem<LinearRGB_f32>::toRGB_8()
{
    static constexpr float gamma = 2.2f;
    return color_t<RGB_8>{
        static_cast<uint8_t>(std::pow(this->r, 1.0f / gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->g, 1.0f / gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->b, 1.0f / gamma) * 255.0f)
    };
}

color_t<Oklab> _color_implem<LinearRGB_f32>::toOklab()
{
    color_t<RGB_f32> rgb_f32 = this->toRGB_f32();
    return rgb_f32.toOklab();
}

color_t<RGB_f32> _color_implem<LinearRGB_f32>::toRGB_f32()
{
    static constexpr float gamma = 2.2f;
    return color_t<RGB_f32>{ std::pow(this->r, 1.0f / gamma),
                             std::pow(this->g, 1.0f / gamma),
                             std::pow(this->b, 1.0f / gamma) };
}

color_t<LinearRGB_8> _color_implem<LinearRGB_f32>::toLinearRGB_8()
{
    return color_t<LinearRGB_8>{ static_cast<uint8_t>(this->r * 255.0f),
                                 static_cast<uint8_t>(this->g * 255.0f),
                                 static_cast<uint8_t>(this->b * 255.0f) };
}

/*
======================================
Casting python color messages to C++ color_t
======================================
*/

color_t<RGB_f32> cast_to_color_t_RGB_f32(const PyAPI::Color &color)
{
    return color_t<RGB_f32>(color.r, color.g, color.b);
}

color_t<RGB_8> cast_to_color_t_RGB_8(const PyAPI::Color &color)
{
    return color_t<RGB_8>(
        static_cast<uint8_t>(color.r * 255.0f),
        static_cast<uint8_t>(color.g * 255.0f),
        static_cast<uint8_t>(color.b * 255.0f));
}

/*
======================================
Buffer utils
======================================
*/

pixel_buffer_t::pixel_buffer_t(int width, int height)
    : width(width)
    , height(height)
    , owns_buffer(true)
    , buffer(new uint8_t[width * height * 3])
{ }

pixel_buffer_t::pixel_buffer_t(uint8_t *buffer, int width, int height)
    : width(width)
    , height(height)
    , owns_buffer(false)
    , buffer(buffer)
{ }

pixel_buffer_t::~pixel_buffer_t()
{
    if (owns_buffer)
        delete[] buffer;
}

void pixel_buffer_t::copy_from(pixel_buffer_t &other)
{
    if (other.width != width || other.height != height)
        return;

    std::memcpy(buffer, other.buffer, width * height * 3);
}

void pixel_buffer_t::copy_from(pixel_buffer_t &&other)
{
    if (other.width != width || other.height != height)
        return;

    std::memcpy(buffer, other.buffer, width * height * 3);
}

void pixel_buffer_t::set_pixel(int x, int y, const color_t<RGB_8> &color)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    int index = (y * width + x) * 3;
    buffer[index] = color.x;
    buffer[index + 1] = color.y;
    buffer[index + 2] = color.z;
}

color_t<RGB_8> pixel_buffer_t::get_pixel(int x, int y)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return color_t<RGB_8>(0, 0, 0);

    int index = (y * width + x) * 3;
    return color_t<RGB_8>(buffer[index], buffer[index + 1], buffer[index + 2]);
}

void pixel_buffer_t::clear(const color_t<RGB_8> color)
{
    for (int i = 0; i < width * height; i++)
    {
        buffer[i * 3] = color.x;
        buffer[i * 3 + 1] = color.y;
        buffer[i * 3 + 2] = color.z;
    }
}

video_buffer_t::video_buffer_t(int width, int height, int frames)
    : width(width)
    , height(height)
    , frames(frames)
{
    buffer = std::make_unique<uint8_t[]>(width * height * frames * 3);
}

void video_buffer_t::set_all_frames(const pixel_buffer_t &framebuffer)
{
    if (framebuffer.width != width || framebuffer.height != height)
        return;

    for (int i = 0; i < frames; i++)
    {
        std::memcpy(buffer.get() + i * width * height * 3, framebuffer.buffer,
                    width * height * 3);
    }
}

void video_buffer_t::set_frame(const pixel_buffer_t &framebuffer,
                               int frame_index)
{
    if (frame_index < 0 || frame_index >= frames)
        return;

    std::memcpy(buffer.get() + frame_index * width * height * 3,
                framebuffer.buffer, width * height * 3);
}

pixel_buffer_t video_buffer_t::get_frame(int frame_index)
{
    if (frame_index < 0 || frame_index >= frames)
        return pixel_buffer_t(0, 0);

    return pixel_buffer_t(buffer.get() + frame_index * width * height * 3,
                          width, height);
}