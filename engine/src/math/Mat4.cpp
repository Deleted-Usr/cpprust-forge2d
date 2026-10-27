#include <forge2d/math/Mat4.hpp>

#include <cmath>
#include <stdexcept>

namespace forge::math
{
    Mat4::Mat4()
    {
        values_.fill(0.0f);

        at(0, 0) = 1.0f;
        at(1, 1) = 1.0f;
        at(2, 2) = 1.0f;
        at(3, 3) = 1.0f;
    }

    Mat4 Mat4::identity()
    {
        return {};
    }

    Mat4 Mat4::zero()
    {
        Mat4 result;
        result.values_.fill(0.0f);

        return result;
    }

    Mat4 Mat4::translate(const Vec2& translation)
    {
        Mat4 result = identity();

        result.at(0, 3) = translation.x;
        result.at(1, 3) = translation.y;

        return result;
    }

    Mat4 Mat4::rotate(float radians)
    {
        Mat4 result = identity();

        const float cosine = std::cos(radians);
        const float sine   = std::sin(radians);

        // Rotation around the Z axis.
        //
        // [ cos  -sin   0   0 ]
        // [ sin   cos   0   0 ]
        // [  0     0    1   0 ]
        // [  0     0    0   1 ]

        result.at(0, 0) = cosine;
        result.at(0, 1) = -sine;

        result.at(1, 0) = sine;
        result.at(1, 1) = cosine;

        return result;
    }

    Mat4 Mat4::scale(const Vec2& scale)
    {
        Mat4 result = identity();

        result.at(0, 0) = scale.x;
        result.at(1, 1) = scale.y;

        return result;
    }

    Mat4 Mat4::orthographic(
        const float left,
        const float right,
        const float bottom,
        const float top,
        const float nearPlane,
        const float farPlane
    )
    {
        if (right == left)
        {
            throw std::invalid_argument("Orthographic projection requires right != left");
        }

        if (top == bottom)
        {
            throw std::invalid_argument("Orthographic projection requires top != bottom");
        }

        if (farPlane == nearPlane)
        {
            throw std::invalid_argument("Orthographic projection requires far != near");
        }

        Mat4 result = zero();

        result.at(0, 0) = 2.0f / (right - left);
        result.at(1, 1) = 2.0f / (top - bottom);

        result.at(2, 2) = -2.0f / (farPlane - nearPlane);

        result.at(0, 3) = -(right + left) / (right - left);
        result.at(1, 3) = -(top + bottom) / (top - bottom);

        result.at(2, 3) = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.at(3, 3) = 1.0f;

        return result;
    }

    Mat4 Mat4::operator*(const Mat4& other) const
    {
        Mat4 result = zero();

        for (std::size_t row = 0; row < 4; ++row)
        {
            for (std::size_t column = 0; column < 4; ++column)
            {
                float value = 0.0f;

                for (std::size_t k = 0; k < 4; ++k)
                {
                    value += at(row, k) * other.at(k, column);
                }

                result.at(row, column) = value;
            }
        }

        return result;
    }

    Vec4 Mat4::operator*(const Vec4& vector) const
    {
        return {
            at(0, 0) * vector.x
                + at(0, 1) * vector.y
                + at(0, 2) * vector.z
                + at(0, 3) * vector.w,

            at(1, 0) * vector.x
                + at(1, 1) * vector.y
                + at(1, 2) * vector.z
                + at(1, 3) * vector.w,

            at(2, 0) * vector.x
                + at(2, 1) * vector.y
                + at(2, 2) * vector.z
                + at(2, 3) * vector.w,

            at(3, 0) * vector.x
                + at(3, 1) * vector.y
                + at(3, 2) * vector.z
                + at(3, 3) * vector.w
        };
    }

    Mat4& Mat4::operator*=(const Mat4& other)
    {
        *this = *this * other;
        return *this;
    }

    float& Mat4::at(const std::size_t row, const std::size_t column)
    {
        return values_[index(row, column)];
    }

    const float& Mat4::at(const std::size_t row, const std::size_t column) const
    {
        return values_[index(row, column)];
    }

    float* Mat4::data()
    {
        return values_.data();
    }

    const float* Mat4::data() const
    {
        return values_.data();
    }
}
