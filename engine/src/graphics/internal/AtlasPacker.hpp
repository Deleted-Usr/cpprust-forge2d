#pragma once

#include <algorithm>
#include <optional>
#include <stdexcept>

namespace forge::internal
{
    struct AtlasRect { int x, y; };

    /// Append-only shelf packer. One transparent texel surrounds every glyph.
    class AtlasPacker
    {
        public:
            explicit AtlasPacker(int size) : size_(size)
            {
                if (size < 4) throw std::invalid_argument("Atlas page must be at least 4 texels wide");
            }

            std::optional<AtlasRect> insert(int width, int height)
            {
                if (width <= 0 || height <= 0 || width > size_ - 2 || height > size_ - 2)
                {
                    return {};
                }

                int x = x_, y = y_, row = rowHeight_;

                if (x + width + 2 > size_)
                {
                    x   = 0;
                    y  += row;
                    row = 0;
                }

                if (y + height + 2 > size_)
                {
                    return {};
                }

                x_ = x + width + 2;
                y_ = y;

                rowHeight_ = std::max(row, height + 2);

                return AtlasRect{x + 1, y + 1};
            }

        private:
            int x_ = 0;
            int y_ = 0;

            int size_;
            int rowHeight_ = 0;
    };
}
