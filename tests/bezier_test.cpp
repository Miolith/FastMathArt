#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "../fastmathart/math/bezier.h"
#include "../fastmathart/math/vec.h"

TEST_CASE("Bezier curve")
{
    using namespace math;

    CubicBezier curve{ { 0.0f, 0.0f, 0.0f },
                       { 1.0f, 1.0f, 0.0f },
                       { 2.0f, 1.0f, 0.0f },
                       { 3.0f, 0.0f, 0.0f } };

    CHECK(almost_eq(curve.valueAt(0.0f), fvec3(0.0f, 0.0f, 0.0f)));
    CHECK(almost_eq(curve.valueAt(1.0f), fvec3(3.0f, 0.0f, 0.0f)));
    CHECK(almost_eq(curve.valueAt(0.5f), fvec3(1.5f, 0.75f, 0.0f)));
}