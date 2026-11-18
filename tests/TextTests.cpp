#include "TestSupport.hpp"

#include <fstream>
#include <iterator>

#include <graphics/internal/FontData.hpp>
#include <graphics/internal/AtlasPacker.hpp>
#include <forge2d/assets/AssetError.hpp>

using namespace forge;

int main()
{
    try
    {
        std::ifstream input(std::string(FORGE2D_FONT_FIXTURE_DIR) + "/Ubuntu-Light.ttf", std::ios::binary);
        std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(input), {}};

        auto font = internal::FontAccess::create(bytes);
        auto copy = font;

        check(copy == font && font.valid(), "font is a shared handle");

        auto face = internal::FontAccess::data(font);

        const auto one       = font.measureText("AV g",       32);
        const auto multiline = font.measureText("AV g\nnext", 32);

        check(one.x > 0 && one.y > 0, "font measurement without a GL context");

        check(near(multiline.y, one.y * 2), "multiline line spacing");
        check(near(font.measureText("a\r\nb", 32).y, font.measureText("a\nb", 32).y), "CRLF is one newline");

        check(font.measureText(" ",  32).x >  0, "space advances");
        check(font.measureText("\t", 32).x >= font.measureText("    ", 32).x, "tab stops");
        check(font.measureText("",   32).y == 0, "empty text has zero size");

        check(Font{}.measureText("A", 32).x == 0, "empty font measurement");

        check(face->layout("\xc3\xa9", 32).glyphs.size() == 1, "UTF-8 scalar decoding");
        check(face->layout("\xff",     32).glyphs.size() == 1, "malformed UTF-8 replacement");

        auto layout = face->layout("Ag", 32);

        const auto& a = layout.glyphs[0];
        const auto& g = layout.glyphs[1];

        check(g.y + g.height > a.y + a.height, "descender extends below capital baseline");

        const auto bitmap = face->rasterise(a.index, 32);

        check(bitmap.pixels.size() == bitmap.width * bitmap.height, "bitmap storage matches glyph metrics");
        check(bitmap.width == a.width && bitmap.height == a.height, "layout and raster metrics agree");

        bool rejected = false;
        try
        {
            (void)font.measureText("A", -1);
        }
        catch (const std::invalid_argument&)
        {
            rejected = true;
        }

        check(rejected, "invalid size rejected across bridge");

        rejected = false;
        try
        {
            (void)internal::FontAccess::create({});
        }
        catch (const AssetError&)
        {
            rejected = true;
        }

        check(rejected, "invalid font converted to asset error");

        internal::AtlasPacker packer(16);

        const auto first  = packer.insert(6, 6);
        const auto second = packer.insert(6, 6);
        const auto third  = packer.insert(6, 6);

        check(first && second && third, "shelf packing fills rows");

        check(first->x  == 1 && first->y == 1, "transparent atlas border");
        check(second->x >= first->x + 8,       "padding separates adjacent glyphs");
        check(third->y  >= first->y + 8,       "shelf moves to next row");

        check(!packer.insert(20, 2), "oversize glyph rejected");
        check( packer.insert(6,  6).has_value(), "failed allocation preserves packer state");
        check(!packer.insert(1,  1), "full atlas requests another page");

        std::cout << "Text CPU checks passed: " << checks << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
