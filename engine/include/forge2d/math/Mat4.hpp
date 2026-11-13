#pragma once

/// @file Mat4.hpp
/// @brief 4×4 matrices for 2D transforms and projections.

#include <array>
#include <cstddef>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Vec4.hpp>

namespace forge::math
{
    /// @brief A 4×4 float matrix, stored column-major so it can be passed straight to OpenGL.
    ///
    /// Matrices transform column vectors (`matrix * vector`), so in a product the **rightmost**
    /// matrix is applied first: `translate(p) * rotate(r) * scale(s)` scales, then rotates, then
    /// translates.
    ///
    /// Games rarely need it directly: `Transform2D`, `Camera2D` and `Renderer2D` build the
    /// matrices they need. A default-constructed matrix is the identity.
    ///
    /// @code
    /// using forge::math::Mat4;
    ///
    /// const Mat4 model = Mat4::translate({ 100, 50 }) * Mat4::rotate(angle);
    /// const forge::math::Vec4 corner = model * forge::math::Vec4{ 16, 16, 0, 1 };
    /// @endcode
    class Mat4
    {
        public:
            /// Number of floats in the matrix.
            static constexpr std::size_t SIZE = 16;

            /// @brief Creates the identity matrix.
            Mat4();

            /// @brief The identity matrix, which leaves vectors unchanged.
            [[nodiscard]]
            static Mat4 identity();

            /// @brief A matrix with every element set to `0`.
            [[nodiscard]]
            static Mat4 zero();

            /// @brief A matrix that moves points by @p translation in X and Y.
            [[nodiscard]]
            static Mat4 translate(const Vec2& translation);

            /// @brief A matrix that rotates points around the origin, in the XY plane.
            ///
            /// @param radians Rotation angle. Because +Y points down on screen, a positive angle
            ///                turns things **clockwise** on screen.
            [[nodiscard]]
            static Mat4 rotate(float radians);

            /// @brief A matrix that scales points by @p scale in X and Y, relative to the origin.
            ///
            /// Z is left unchanged.
            [[nodiscard]]
            static Mat4 scale(const Vec2& scale);

            /// @brief An orthographic projection that maps the given box to OpenGL clip space
            ///        (-1 to 1 on each axis).
            ///
            /// For a Y-down screen space, pass the screen's height as @p bottom and `0` as
            /// @p top: `orthographic(0, width, height, 0)`.
            ///
            /// @param left      X that maps to -1.
            /// @param right     X that maps to +1.
            /// @param bottom    Y that maps to -1 (the bottom of the screen).
            /// @param top       Y that maps to +1 (the top of the screen).
            /// @param nearPlane Near clipping distance. Defaults to `-1`.
            /// @param farPlane  Far clipping distance. Defaults to `1`.
            ///
            /// @throws std::invalid_argument If `left == right`, `bottom == top` or
            ///         `nearPlane == farPlane`.
            [[nodiscard]]
            static Mat4 orthographic(
                float left,
                float right,
                float bottom,
                float top,
                float nearPlane = -1.0f,
                float farPlane = 1.0f
            );

            /// @brief Matrix product. The result applies @p other first, then this matrix.
            [[nodiscard]]
            Mat4 operator*(const Mat4& other) const;

            /// @brief Transforms @p vector by this matrix.
            ///
            /// Use `w = 1` for points (affected by translation) and `w = 0` for directions.
            [[nodiscard]]
            Vec4 operator*(const Vec4& vector) const;

            /// @brief Sets this matrix to `*this * other`, so @p other is applied first.
            Mat4& operator*=(const Mat4& other);

            /// @brief The element at @p row and @p column.
            ///
            /// @pre @p row and @p column are less than `4`. Out-of-range indices aren't checked.
            [[nodiscard]]
            float& at(std::size_t row,std::size_t column);

            /// @brief The element at @p row and @p column.
            ///
            /// @pre @p row and @p column are less than `4`. Out-of-range indices aren't checked.
            [[nodiscard]]
            const float& at(std::size_t row,std::size_t column) const;

            /// @brief Pointer to the 16 elements in column-major order, for uploading to OpenGL
            ///        without transposing.
            [[nodiscard]]
            float* data();

            /// @brief Pointer to the 16 elements in column-major order, for uploading to OpenGL
            ///        without transposing.
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
