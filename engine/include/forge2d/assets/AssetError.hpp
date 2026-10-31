#pragma once

#include <stdexcept>

namespace forge
{
    /// Thrown by loaders when as asset can't be read or decoded.
    /// AssetManager catches it and substitutes a fallback asset.
    class AssetError : public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };
}
