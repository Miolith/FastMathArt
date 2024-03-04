#pragma once
#include <algorithm>
#include <ranges>
#include <vector>

#include "vec.h"

namespace math
{

    constexpr inline math::fvec3 lerp(const math::fvec3 point1,
                                      const math::fvec3 point2, float t)
    {
        return point1 * (1.0f - t) + point2 * t;
    }

    struct CubicBezier
    {
        math::fvec3 p1;
        math::fvec3 p2;
        math::fvec3 p3;
        math::fvec3 p4;

        constexpr CubicBezier(math::fvec3 p1, math::fvec3 p2, math::fvec3 p3,
                              math::fvec3 p4)
            : p1(p1)
            , p2(p2)
            , p3(p3)
            , p4(p4)
        { }

        CubicBezier() = default;

        constexpr math::fvec3 valueAt(const float t) const
        {
            auto p2_p3 = lerp(p2, p3, t);
            return lerp(lerp(lerp(p1, p2, t), p2_p3, t),
                        lerp(p2_p3, lerp(p3, p4, t), t), t);
        }

        constexpr std::pair<CubicBezier, CubicBezier> split(const float t)
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

        constexpr float length(int precision = 15) const
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

        static constexpr CubicBezier straightLine(const math::fvec3 &p1,
                                                  const math::fvec3 &p2)
        {
            return CubicBezier{ p1, p1, p2, p2 };
        }
    };

    class BezierPath
    {
    private:
        std::vector<CubicBezier> curves;

    public:
        BezierPath(std::vector<CubicBezier> curves)
            : curves(curves)
        { }

        BezierPath() = default;

        constexpr math::fvec3 valueAt(const float t) const
        {
            auto curve_index = static_cast<int>(t * curves.size());
            auto curve_t = (t * curves.size()) - curve_index;
            return curves[curve_index].valueAt(curve_t);
        }

        constexpr float length(int precision = 15) const
        {
            float length = 0.0f;
            for (auto &curve : curves)
            {
                length += curve.length(precision);
            }
            return length;
        }

        constexpr void addCurve(const CubicBezier &curve)
        {
            curves.push_back(curve);
        }

        constexpr std::size_t size() const
        {
            return curves.size();
        }

        constexpr CubicBezier &operator[](std::size_t index)
        {
            return curves[index];
        }

        constexpr const CubicBezier &operator[](std::size_t index) const
        {
            return curves[index];
        }

        constexpr auto longest_curve() const
        {
            return std::distance(curves.begin(),
                                 std::max_element(curves.begin(), curves.end(),
                                                  [](const CubicBezier &a,
                                                     const CubicBezier &b) {
                                                      return a.length()
                                                          < b.length();
                                                  }));
        }

        void splitAt(int index, float t)
        {
            auto [left_half, right_half] = curves[index].split(t);
            curves[index] = left_half;
            curves.insert(curves.begin() + index + 1, right_half);
        }

        auto begin()
        {
            return curves.begin();
        }

        auto end()
        {
            return curves.end();
        }
    };

    inline void alignPaths(BezierPath &path1, BezierPath &path2)
    {
        auto path1_length = path1.size();
        auto path2_length = path2.size();

        if (path1_length < path2_length)
        {
            int diff = path2_length - path1_length;

            // split the bezier curves at the longest segments
            // until the path lengths are equal
            for (int i = 0; i < diff; i++)
            {
                auto longest_bezier = path1.longest_curve();
                path1.splitAt(longest_bezier, 0.5f);
            }
        }
        else if (path1_length > path2_length)
        {
            alignPaths(path2, path1);
        }
    }

    inline CubicBezier interpolate(const CubicBezier path1,
                                   const CubicBezier path2, float t)
    {
        return CubicBezier{ lerp(path1.p1, path2.p1, t),
                            lerp(path1.p2, path2.p2, t),
                            lerp(path1.p3, path2.p3, t),
                            lerp(path1.p4, path2.p4, t) };
    }

    inline BezierPath interpolatePaths(const BezierPath &path1,
                                       const BezierPath &path2, float t)
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

    inline constexpr auto unit_circle()
    {
        constexpr float a = 1.00005519;
        constexpr float b = 0.55342686;
        constexpr float c = 0.99873585;

        return std::array<fvec3, 12>{ fvec3(0, a, 0),   fvec3(b, c, 0),
                                      fvec3(c, b, 0),   fvec3(a, 0, 0),
                                      fvec3(c, -b, 0),  fvec3(b, -c, 0),
                                      fvec3(0, -a, 0),  fvec3(-b, -c, 0),
                                      fvec3(-c, -b, 0), fvec3(-a, 0, 0),
                                      fvec3(-c, b, 0),  fvec3(-b, c, 0) };
    }

    inline std::array<fvec3, 12> circle_bezier(float radius)
    {
        std::array<fvec3, 12> p = unit_circle();

        for (auto &point : p)
            point = point * radius;

        return p;
    }

} // namespace math