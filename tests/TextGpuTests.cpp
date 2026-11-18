#include "TestSupport.hpp"

#include <array>
#include <fstream>
#include <iterator>
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <forge2d/assets/AssetManager.hpp>
#include <forge2d/assets/AssetError.hpp>
#include <forge2d/graphics/Camera2D.hpp>
#include <forge2d/graphics/Renderer2D.hpp>

#include <graphics/internal/GlyphAtlas.hpp>
#include <graphics/internal/BatchBuilder2D.hpp>
#include <graphics/internal/RenderView2D.hpp>

#include <opengl/OpenGLRenderer2D.hpp>

using namespace forge;
constexpr int Width = 640, Height = 360;

const FrameInfo Frame{{ Width, Height }, Width, Height};

std::vector<unsigned char> readFrame()
{
    std::vector<unsigned char> pixels(Width * Height * 4);
    glReadPixels(0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    return pixels;
}

const unsigned char* pixel(const std::vector<unsigned char>& pixels, int x, int y)
{
    return pixels.data() + ((Height - y - 1) * Width + x) * 4;
}

int ink(const std::vector<unsigned char>& pixels, int x, int y, int w, int h, int channel)
{
    int count = 0;

    for (int row = y; row < y + h; ++row)
    {
        for (int col = x; col < x + w; ++col)
        {
            if (pixel(pixels, col, row)[channel] > 4)
            {
                ++count;
            }
        }
    }

    return count;
}

void run(const char* preview)
{
    AssetManager assets(FORGE2D_FONT_FIXTURE_DIR);
    auto font = assets.loadFont(assets::FontAsset{"Ubuntu-Light.ttf"});

    check(font.valid() && font == assets.loadFont("./Ubuntu-Light.ttf"), "font asset normalized cache");
    check(assets.fontCount() == 1, "one face for multiple font sizes");

    bool rejected = false;
    try
    {
        (void)assets.loadFont("absent.ttf");
    }
    catch (const AssetError&)
    {
        rejected = true;
    }

    check(rejected && assets.fontCount() == 1, "missing font throws without poisoning cache");

    Renderer2D renderer(FORGE2D_TEST_SHADER_DIR);

    auto begin = [&] { renderer.beginFrame(Frame, colours::Black); };
    begin();

    renderer.drawText("A", font, {20, 20}, 48, {1, 0, 0, 0.5f});

    renderer.endFrame();

    const auto first = readFrame();
    int partial = 0, solid = 0;

    for (int y = 0; y < 90; ++y)
    {
        for (int x = 0; x < 100; ++x)
        {
            const auto* p = pixel(first, x, y);

            check(p[0] <= 129 && p[1] == 0 && p[2] == 0, "coverage affects alpha, not text RGB");

            if (p[0] > 0 && p[0] < 120) ++partial;
            if (p[0] >= 120)            ++solid;
        }
    }

    check(partial > 10 && solid > 20, "antialiased glyph silhouette, not a solid quad");
    check(ink(first, 0, 0, 640, 20, 0) == 0, "top-left line box placement");

    begin();

    renderer.drawText("A", &font, {20, 20}, 48, {1, 0, 0, 0.5f});
    renderer.endFrame();

    check(first == readFrame(), "cached glyph reproduces identical pixels");

    begin();

    renderer.drawRect({.position={0, 0}, .scale={100, 100}}, colours::Blue);
    renderer.drawText("A", font, {20, 20}, 48, colours::Red);
    renderer.drawRect({.position={0, 0}, .scale={100, 100}}, {0, 1, 0, 0.5f});

    renderer.endFrame();

    const auto ordered = readFrame();

    for (int y = 0; y < 90; ++y)
    {
        for (int x = 0; x < 90; ++x)
        {
            const auto* a = pixel(first,   x, y);
            const auto* b = pixel(ordered, x, y);

            check(
                std::abs(static_cast<int>(b[0]) - a[0]) <= 2 && std::abs(b[1] - 128) <= 1,
                "text stays between ordered background and foreground draws"
            );
        }
    }

    begin();

    Camera2D camera;
    renderer.setCamera(camera);

    renderer.drawText("A", font, {20, 20}, 48, colours::Red);
    renderer.resetCamera();
    renderer.drawText("HUD", font, {20, 80}, 24, colours::Blue);

    renderer.endFrame();

    const auto cameraFrame = readFrame();
    check(ink(cameraFrame, 340, 200, 70, 80, 0) >  20, "world text uses the camera");
    check(ink(cameraFrame, 20,  20,  70, 70, 0) == 0,  "world text leaves screen coordinates");
    check(ink(cameraFrame, 20,  80,  90, 40, 2) >  20, "HUD text resets camera");

    begin();

    renderer.drawText(std::string(5000, 'A'), font, {-100000, 0}, 12);
    renderer.drawText("A", font, {20, 20}, 48, {1, 0, 0, 0.5f});
    renderer.drawText("", font, {}, 32);
    renderer.drawText("unused", static_cast<const Font*>(nullptr), {}, 32);

    renderer.endFrame();

    check(first == readFrame(), "glyph vertex capacity rollover preserves following draws");

    renderer.beginFrame({{0, 0}, 0, 0}, colours::Black);
    renderer.drawText("ignored", font, {}, 32);

    renderer.endFrame();

    // Exercise a deliberately tiny atlas: page rollover must preserve old UVs.
    internal::opengl::OpenGLRenderer2D backend(FORGE2D_TEST_SHADER_DIR);
    internal::GlyphAtlas atlas(backend, 32, 128);

    auto face = internal::FontAccess::data(font);
    auto layout = face->layout("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 18);
    const auto firstGlyph = atlas.get(face, layout.glyphs[0].index, 18);

    for (const auto& glyph : layout.glyphs)
    {
        (void)atlas.get(face, glyph.index, 18);
    }

    check(atlas.pageCount() > 8, "atlas rollover creates enough pages to exceed texture slots");

    const auto count   = atlas.glyphCount();
    const auto& cached = atlas.get(face, layout.glyphs[0].index, 18);

    check(
        atlas.glyphCount() == count              &&
        cached.texture     == firstGlyph.texture &&
        near(cached.uvMin.x, firstGlyph.uvMin.x),
        "cache miss packing never relocates existing glyphs"
    );

    (void)atlas.get(face, layout.glyphs[0].index, 19);

    check(atlas.glyphCount() == count + 1, "rasterisation size is part of cache key");

    internal::BatchBuilder2D batches(backend.whiteTexture(), 16384, 24576, 8);
    const auto view = internal::RenderView2D::screen({Width, Height});

    backend.beginFrame(Width, Height, colours::Black);

    int flushes = 0;

    for (std::size_t i = 0; i < layout.glyphs.size(); ++i)
    {
        const auto& p = layout.glyphs[i];
        const auto& g = atlas.get(face, p.index, 18);

        auto append = [&] { return batches.tryAppendGlyph(g.texture,
            {static_cast<float>((i % 30) * 20), static_cast<float>((i / 30) * 24)},
            {static_cast<float>(p.width),       static_cast<float>(p.height)}, g.uvMin, g.uvMax, colours::White); };

        if (!append())
        {
            backend.submit(batches.batch(), view);
            batches.clear();

            ++flushes;

            check(append(), "glyph retry fits");
        }
    }

    backend.submit(batches.batch(), view);
    batches.clear();

    check(flushes > 0 && ink(readFrame(), 0, 0, 600, 72, 0) > 200, "text atlas pages share texture-slot batching");

    std::weak_ptr<const internal::GPUTexture> retained;

    {
        std::ifstream input(std::string(FORGE2D_FONT_FIXTURE_DIR) + "/Ubuntu-Light.ttf", std::ios::binary);
        std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(input), {}};

        auto temporary = internal::FontAccess::create(bytes);
        auto data      = internal::FontAccess::data(temporary);
        const auto& g  = atlas.get(data, data->layout("A", 18).glyphs[0].index, 18);

        retained = g.texture;

        check(batches.tryAppendGlyph(g.texture, {0, 0}, {16, 16}, g.uvMin, g.uvMax, colours::White), "queue temporary face glyph");
    }

    atlas.pruneExpired();

    check(!retained.expired(), "queued batch retains expired face's atlas texture");

    backend.submit(batches.batch(), view);
    batches.clear();

    check(retained.expired(), "atlas resources released after face expiry and batch submission");

    // Atlas creation and odd-width R8 uploads restore all inherited unpack state.
    GLuint buffer;
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);

    glPixelStorei(GL_UNPACK_ALIGNMENT,   8);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,  23);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 2);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,   1);

    auto page = backend.createGlyphPage(16);
    std::vector<std::uint8_t> coverage(13 * 3, 255);
    backend.uploadGlyph(*page, 1, 1, 13, 3, coverage);

    GLint value;
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &value); check(static_cast<GLuint>(value) == buffer, "atlas upload restores PBO");
    glGetIntegerv(GL_UNPACK_ROW_LENGTH,  &value); check(value == 23, "atlas upload restores row length");
    glGetIntegerv(GL_UNPACK_ALIGNMENT,   &value); check(value == 8,  "atlas upload restores alignment");
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &value); check(value == 2,  "atlas upload restores skip pixels");
    glGetIntegerv(GL_UNPACK_SKIP_ROWS,   &value); check(value == 1,  "atlas upload restores skip rows");

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glDeleteBuffers(1, &buffer);

    glPixelStorei(GL_UNPACK_ALIGNMENT,   4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,  0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);

    if (preview)
    {
        renderer.beginFrame(Frame, {0.035f, 0.045f, 0.07f, 1});

        renderer.drawRect({.position={20, 20}, .scale={600, 320}}, {0.07f, 0.09f, 0.14f, 1});

        renderer.drawText("Forge2D Text Rendering", font, {40, 32}, 40);
        renderer.drawText("Score: 1200", font, {40, 95}, 32, colours::Cyan);
        renderer.drawText("Ag jpqy   AV To\nSecond line, same baseline spacing", &font, {40, 150}, 24);
        renderer.drawText("UTF-8: Caf\xc3\xa9  \xce\xa9", font, {40, 232}, 28, {1, 0.8f, 0.3f, 1});
        renderer.drawText("Alpha coverage + ordered batches", font, {40, 284}, 20, {0.6f, 0.8f, 1, 0.65f});

        renderer.endFrame();

        const auto image = readFrame();

        std::ofstream output(preview, std::ios::binary);
        output << "P6\n" << Width << ' ' << Height << "\n255\n";

        for (int y = 0; y < Height; ++y)
        {
            for (int x = 0; x < Width; ++x)
            {
                output.write(reinterpret_cast<const char*>(pixel(image, x, y)), 3);
            }
        }
    }

    check(glGetError() == GL_NO_ERROR, "text rendering leaves no GL errors");
}

int main(int argc, char** argv)
{
    if (!glfwInit())
    {
        return 77;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto* window = glfwCreateWindow(Width, Height, "Text tests", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 77;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();

        return 77;
    }

    int result = 0;

    try
    {
        run(argc > 1 ? argv[1] : nullptr);
        std::cout << "Text GPU checks passed: " << checks << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n'; result = 1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return result;
}
