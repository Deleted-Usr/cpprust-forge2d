#include <forge2d/math/Transform2D.hpp>

namespace forge::math
{
    Mat4 Transform2D::matrix() const
    {
        const Mat4 translationMatrix = Mat4::translate(position);
        const Mat4 rotationMatrix    = Mat4::rotate(rotation);
        const Mat4 scaleMatrix       = Mat4::scale(scale);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }
}