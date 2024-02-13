#pragma once
#include "vec.h"

namespace math
{

    inline math::fvec3 lerp(const math::fvec3 point1, const math::fvec3 point2,
                            float t)
    {
        return point1 * t + point2 * (1.0f - t);
    }

    struct CubicBezier
    {
        math::fvec3 p1;
        math::fvec3 p2;
        math::fvec3 p3;
        math::fvec3 p4;

        math::fvec3 valueAt(float t)
        {
            auto p2_p3 = lerp(p2, p3, t);
            return lerp(lerp(lerp(p1, p2, t), p2_p3, t),
                        lerp(p2_p3, lerp(p3, p4, t), t), t);
        }
    };
} // namespace math