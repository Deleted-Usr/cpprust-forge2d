#pragma once

#include <array>
#include <cstddef>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Vec4.hpp>

namespace forge::math
{
    class Mat4
    {
        public:
            static constexpr std::size_t SIZE = 16;

            /// Creates an identity matrix.
            Mat4();

            [[nodiscard]]
            static Mat4 identity();

            [[nodiscard]]
            static Mat4 zero();

            [[nodiscard]]
            static Mat4 translate(const Vec2& translation);

            [[nodiscard]]
            static Mat4 rotate(float radians);

            [[nodiscard]]
            static Mat4 scale(const Vec2& scale);

            [[nodiscard]]
            static Mat4 orthographic(
                float left,
                float right,
                float bottom,
                float top,
                float nearPlane = -1.0f,
                float farPlane = 1.0f
            );

            [[nodiscard]]
            Mat4 operator*(const Mat4& other) const;

            [[nodiscard]]
            Vec4 operator*(const Vec4& vector) const;

            Mat4& operator*=(const Mat4& other);

            [[nodiscard]]
            float& at(std::size_t row,std::size_t column);

            [[nodiscard]]
            const float& at(std::size_t row,std::size_t column) const;

            [[nodiscard]]
            float* data();

            [[nodiscard]]
            const float* data() const;

        private:
            std::array<float, SIZE> values_{};

            [[nodiscard]]
            static constexpr std::size_t index(std::size_t row,std::size_t column)
            {
                // Column-major:
                //
                //  0   4   8  12
                //  1   5   9  13
                //  2   6  10  14
                //  3   7  11  15

                return column * 4 + row;
            }
    };
}
