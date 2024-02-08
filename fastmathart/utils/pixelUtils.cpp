#include <cstdint>
#include "pixelUtils.h"
    

color_t<Oklab>::color_t(float l, float a, float b)
    : vec3<float>(l, a, b)
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
    return color_t<RGB_f32>(this->r / 255.0f, this->g / 255.0f, this->b / 255.0f);
}

color_t<RGB_8> color_t<RGB_f32>::toRGB_8()
{
    return color_t<RGB_8>(static_cast<uint8_t>(this->r * 255.0f),
                          static_cast<uint8_t>(this->g * 255.0f),
                          static_cast<uint8_t>(this->b * 255.0f));
}

color_t<Oklab> color_t<RGB_f32>::toOklab()
{
    float l = 0.4122214708f * this->r + 0.5363325363f * this->g + 0.0514459929f * this->b;
    float m = 0.2119034982f * this->r + 0.6806995451f * this->g + 0.1073969566f * this->b;
    float s = 0.0883024619f * this->r + 0.2817188376f * this->g + 0.6299787005f * this->b;

    float l_ = std::cbrt(l);
    float m_ = std::cbrt(m);
    float s_ = std::cbrt(s);

    float l_16 = 116.0f * l_ - 16.0f;
    float a_ = 500.0f * (l_ - m_);
    float b_ = 200.0f * (m_ - s_);

    return color_t<Oklab>(l_16, a_, b_);
}