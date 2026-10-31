#pragma once

#include <string_view>

namespace forge::assets
{
    // Typed asset identifiers. Distinct types mean
    // loadTexture(SomeFont) is a compile error.
    struct TextureAsset
    {
        std::string_view path;
        bool preload = true;
    };

    struct FontAsset
    {
        std::string_view path;
        bool preload = true;
    };
}
