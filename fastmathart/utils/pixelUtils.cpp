#include "pixelUtils.h"

#include <cstdint>

/*
======================================
Oklab 32bits float per channel, member functions
======================================
*/

color_t<Oklab>::color_t(float l, float a, float b)
    : vec3<float>(l, a, b)
{}


/*
======================================
RGB 8 bits unsigned int per channel, member functions
======================================
*/

color_t<RGB_8>::color_t(uint8_t r, uint8_t g, uint8_t b)
    : vec3<uint8_t>(r, g, b)
{}

color_t<Oklab> color_t<RGB_8>::toOklab()
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

color_t<RGB_f32> color_t<RGB_8>::toRGB_f32()
{
    return color_t<RGB_f32>{ static_cast<float>(this->r) / 255.0f,
                             static_cast<float>(this->g) / 255.0f,
                             static_cast<float>(this->b) / 255.0f };
}

color_t<LinearRGB_8> color_t<RGB_8>::toLinearRGB_8()
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


color_t<RGB_8> color_t<RGB_f32>::toRGB_8()
{
    return color_t<RGB_8>(static_cast<uint8_t>(this->r * 255.0f),
                          static_cast<uint8_t>(this->g * 255.0f),
                          static_cast<uint8_t>(this->b * 255.0f));
}

color_t<Oklab> color_t<RGB_f32>::toOklab()
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

color_t<LinearRGB_8> color_t<RGB_f32>::toLinearRGB_8()
{
    static constexpr float gamma = 2.2f;
    return color_t<LinearRGB_8>{
        static_cast<uint8_t>(std::pow(this->r, gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->g, gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->b, gamma) * 255.0f)
    };
}

color_t<LinearRGB_f32> color_t<RGB_f32>::toLinearRGB_f32()
{
    static constexpr float gamma = 2.2f;
    return color_t<LinearRGB_f32>{
        std::pow(this->r, gamma),
        std::pow(this->g, gamma),
        std::pow(this->b, gamma)
    };
}

/*
======================================
LinearRGB 8 bits unsigned int per channel, member functions
======================================
*/

color_t<RGB_8> color_t<LinearRGB_8>::toRGB_8()
{
    static constexpr float gamma = 2.2f;
    return color_t<RGB_8>{
        static_cast<uint8_t>(std::pow(this->r / 255.0f, 1.0f / gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->g / 255.0f, 1.0f / gamma) * 255.0f),
        static_cast<uint8_t>(std::pow(this->b / 255.0f, 1.0f / gamma) * 255.0f)
    };
}

color_t<Oklab> color_t<LinearRGB_8>::toOklab()
{
    color_t<RGB_f32> rgb_f32 = this->toRGB_f32();
    return rgb_f32.toOklab();
}

color_t<RGB_f32> color_t<LinearRGB_8>::toRGB_f32()
{
    static constexpr float gamma = 2.2f;
    return color_t<RGB_f32>{
        std::pow(this->r / 255.0f, 1.0f / gamma),
        std::pow(this->g / 255.0f, 1.0f / gamma),
        std::pow(this->b / 255.0f, 1.0f / gamma)
    };
}

color_t<LinearRGB_f32> color_t<LinearRGB_8>::toLinearRGB_f32()
{
    return color_t<LinearRGB_f32>{
        this->r / 255.0f,
        this->g / 255.0f,
        this->b / 255.0f
    };
}