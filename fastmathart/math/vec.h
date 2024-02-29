#pragma once
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>

namespace math
{
    inline float random_float()
    {
        static std::uniform_real_distribution<float> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
    }

    inline float random_float(float min, float max)
    {
        return min + (max - min) * random_float();
    }

    template <typename Number>
    struct vec3
    {
        Number x, y, z;
        static_assert(std::is_arithmetic<Number>::value,
                      "vec3 can only be instantiated with arithmetic types");

        vec3(Number x, Number y, Number z)
            : x(x)
            , y(y)
            , z(z)
        {}

        vec3(Number scalar)
            : x(scalar)
            , y(scalar)
            , z(scalar)
        {}

        vec3()
            : x(0)
            , y(0)
            , z(0)
        {}

        vec3 operator+(vec3 other)
        {
            return vec3(this->x + other.x, this->y + other.y,
                        this->z + other.z);
        }

        vec3 operator-(vec3 other)
        {
            return vec3(this->x - other.x, this->y - other.y,
                        this->z - other.z);
        }

        vec3 operator*(Number scalar)
        {
            return vec3(this->x * scalar, this->y * scalar, this->z * scalar);
        }

        vec3 operator*(vec3 other)
        {
            return vec3(this->x * other.x, this->y * other.y,
                        this->z * other.z);
        }

        vec3 operator-()
        {
            return vec3(-this->x, -this->y, -this->z);
        }

        vec3 operator+(Number scalar)
        {
            return vec3(this->x + scalar, this->y + scalar, this->z + scalar);
        }

        void operator*=(Number scalar)
        {
            this->x *= scalar;
            this->y *= scalar;
            this->z *= scalar;
        }

        void operator/=(Number scalar)
        {
            *this *= 1.0f / scalar;
        }

        vec3 operator+=(vec3 other)
        {
            this->x += other.x;
            this->y += other.y;
            this->z += other.z;
            return *this;
        }

        Number length()
        {
            return std::sqrt(this->x * this->x + this->y * this->y
                             + this->z * this->z);
        }

        vec3 normalize()
        {
            Number length = this->length();
            return vec3(this->x / length, this->y / length, this->z / length);
        }

        friend std::ostream &operator<<(std::ostream &os, const vec3<Number> v)
        {
            os << "vec3(" << v.x << ' ' << v.y << ' ' << v.z << ')';
            return os;
        }

        static vec3 random()
        {
            return vec3(random_float(-1, 1), random_float(-1, 1),
                        random_float(-1, 1))
                .normalize();
        }

        friend vec3<Number> operator+(const vec3<Number> &u,
                                      const vec3<Number> &v)
        {
            return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
        }

        friend vec3<Number> operator-(const vec3<Number> &u,
                                      const vec3<Number> &v)
        {
            return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
        }

        friend vec3<Number> operator*(const vec3<Number> &u,
                                      const vec3<Number> &v)
        {
            return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
        }

        friend vec3<Number> operator*(Number t, const vec3<Number> &v)
        {
            return vec3(t * v.x, t * v.y, t * v.z);
        }

        friend vec3<Number> operator/(vec3<Number> v, Number t)
        {
            return (1 / t) * v;
        }

        friend vec3<Number> operator+(vec3<Number> v, Number t)
        {
            return vec3(v.x + t, v.y + t, v.z + t);
        }

        friend vec3<Number> operator-(const vec3<Number> &v)
        {
            return vec3(-v.x, -v.y, -v.z);
        }

        friend Number dot(const vec3<Number> &u, const vec3<Number> &v)
        {
            return u.x * v.x + u.y * v.y + u.z * v.z;
        }

        friend vec3<Number> cross(const vec3<Number> &u, const vec3<Number> &v)
        {
            return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z,
                        u.x * v.y - u.y * v.x);
        }

        friend bool almost_eq(const vec3<Number> &u, const vec3<Number> &v, float tolerance = 1e-8)
        {
            return std::abs(u.x - v.x) < tolerance && std::abs(u.y - v.y) < tolerance && std::abs(u.z - v.z) < tolerance;
        }
    };

    using fvec3 = vec3<float>;

} // namespace math