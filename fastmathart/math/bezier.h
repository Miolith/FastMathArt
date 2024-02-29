#pragma once
#include <algorithm>
#include <ranges>
#include <vector>

#include "vec.h"

namespace math
{

    inline math::fvec3 lerp(const math::fvec3 point1, const math::fvec3 point2,
                            float t)
    {
        return point1 * (1.0f - t) + point2 * t;
    }

    struct CubicBezier
    {
        math::fvec3 p1;
        math::fvec3 p2;
        math::fvec3 p3;
        math::fvec3 p4;

        CubicBezier(math::fvec3 p1, math::fvec3 p2, math::fvec3 p3,
                    math::fvec3 p4)
            : p1(p1)
            , p2(p2)
            , p3(p3)
            , p4(p4)
        {}

        CubicBezier() = default;

        math::fvec3 valueAt(float t) const
        {
            auto p2_p3 = lerp(p2, p3, t);
            return lerp(lerp(lerp(p1, p2, t), p2_p3, t),
                        lerp(p2_p3, lerp(p3, p4, t), t), t);
        }

        std::pair<CubicBezier, CubicBezier> split(float t)
        {
            auto p12 = lerp(p1, p2, t);
            auto p23 = lerp(p2, p3, t);
            auto p34 = lerp(p3, p4, t);
            auto p123 = lerp(p12, p23, t);
            auto p234 = lerp(p23, p34, t);
            auto p1234 = lerp(p123, p234, t);
            return std::make_pair(CubicBezier{ p1, p12, p123, p1234 },
                                  CubicBezier{ p1234, p234, p34, p4 });
        }

        float length(int precision = 15) const
        {
            float length = 0.0f;
            auto prev = valueAt(0.0f);
            for (int i = 1; i <= precision; i++)
            {
                auto next = valueAt(i / 100.0f);
                length += (next - prev).length();
                prev = next;
            }
            return length;
        }

        static CubicBezier straightLine(const math::fvec3 &p1,
                                        const math::fvec3 &p2)
        {
            return CubicBezier{ p1, p1, p2, p2 };
        }
    };

    inline void alignPaths(std::vector<CubicBezier> &path1,
                           std::vector<CubicBezier> &path2)
    {
        auto path1_length = path1.size();
        auto path2_length = path2.size();
        if (path1_length > path2_length)
        {
            int diff = path1_length - path2_length;

            // split the bezier curves at the longest segments
            // until the path lengths are equal
            for (int i = 0; i < diff; i++)
            {
                auto longest_bezier = std::max_element(
                    path1.begin(), path1.end(),
                    [](const CubicBezier &a, const CubicBezier &b) {
                        return a.length() < b.length();
                    });
                auto index = std::distance(path1.begin(), longest_bezier);

                auto [left_half, right_half] = longest_bezier->split(0.5f);

                path1[index] = left_half;
                path1.insert(path1.begin() + index + 1, right_half);
            }
        }
        else if (path2_length > path1_length)
        {
            alignPaths(path2, path1);
        }
    }

    inline CubicBezier interpolate(const CubicBezier &path1,
                                   const CubicBezier &path2, float t)
    {
        return CubicBezier{ lerp(path1.p1, path2.p1, t),
                            lerp(path1.p2, path2.p2, t),
                            lerp(path1.p3, path2.p3, t),
                            lerp(path1.p4, path2.p4, t) };
    }

    inline std::vector<CubicBezier>
    interpolatePaths(const std::vector<CubicBezier> &path1,
                     const std::vector<CubicBezier> &path2, float t)
    {
        if (path1.size() != path2.size())
            throw std::runtime_error(
                "Paths must have the same number of curves");

        std::vector<CubicBezier> result;
        result.reserve(path1.size());
        for (std::size_t i = 0; i < path1.size(); i++)
        {
            result.push_back(interpolate(path1[i], path2[i], t));
        }
        return result;
    }

} // namespace math