#include "TestSupport.hpp"
#include <array>
#include <fstream>
#include <vector>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <forge2d/assets/Image.hpp>
#include <forge2d/assets/AssetManager.hpp>
#include <forge2d/graphics/Camera2D.hpp>
#include <forge2d/graphics/Renderer2D.hpp>
#include <forge2d/graphics/Texture2D.hpp>
#include <graphics/internal/TextureAccess.hpp>

using namespace forge;
constexpr int Size = 128;

math::Transform2D rect(float x, float y, float width, float height)
{
    return { .position = { x, y }, .scale = { width, height } };
}

void pixel(int x, int y, std::array<int, 3> expected, const std::string& label)
{
    std::array<unsigned char, 4> actual{};
    glReadPixels(x, Size - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, actual.data());

    for (int i = 0; i < 3; ++i)
    {
        check(
            std::abs(static_cast<int>(actual[i]) - expected[i]) <= 3,
            label + " channel " + std::to_string(i) + " got " + std::to_string(actual[i])
        );
    }
}

void run(const char* preview)
{
    Renderer2D renderer(FORGE2D_TEST_SHADER_DIR);
    const FrameInfo frame{ { Size, Size }, Size, Size };

    auto begin = [&] { renderer.beginFrame(frame, colours::Black); };

    // Load through the Rust decoder and the real asset cache, then verify upload
    // and sprite orientation using framebuffer pixels in a hidden GL window.
    AssetManager assets(FORGE2D_IMAGE_FIXTURE_DIR);
    const auto decoded = assets.loadTexture("rgba.png");
    check(decoded.width() == 2 && decoded.height() == 2, "decoded texture dimensions");
    check(assets.loadTexture("rgba.png") == decoded, "decoded texture cache hit");
    check(assets.loadTexture("missing.png") == assets.missingTexture(), "missing texture fallback");
    check(assets.loadTexture("gray.png") != assets.missingTexture(), "grayscale texture load");
    const std::vector<std::string> preload{"rgba.png", "red.jpg", "absent.png"};
    assets.preloadTextures(preload);
    check(assets.loadTexture("red.jpg").width() == 16, "preloaded JPEG texture");
    check(assets.loadTexture("absent.png") == assets.missingTexture(), "preload failure fallback");

    begin();
    renderer.drawSprite(decoded, rect(0, 0, 32, 32));
    renderer.endFrame();
    pixel(8, 8, {255, 0, 0}, "decoded PNG top-left red");
    pixel(24, 8, {0, 64, 0}, "decoded PNG top-right alpha");
    pixel(8, 24, {0, 0, 0}, "decoded PNG bottom-left transparent");
    pixel(24, 24, {255, 255, 255}, "decoded PNG bottom-right white");

    auto blue = Texture2D::create(Image{ 1, 1, { 0, 0, 255, 255 } });

    begin();

    renderer.drawRect(rect(10, 10, 20, 20), colours::Red);
    renderer.endFrame();

    pixel(15, 15, { 255, 0, 0 }, "default view");
    pixel(5,  5,  { 0,   0, 0 }, "background");

    begin();

    renderer.drawRect   (      rect(10, 10, 20, 20), colours::Red);
    renderer.drawSprite (blue, rect(10, 10, 20, 20),   { 1, 1, 1, 0.5f });
    renderer.drawRect   (      rect(10, 10, 20, 20), { 0, 1, 0, 0.5f });
    renderer.endFrame();

    pixel(15, 15, { 64, 128, 64 }, "interleaved alpha order");

    Camera2D camera;

    begin();

    renderer.drawRect    (rect(10, 10, 10, 10), colours::Red);

    renderer.setCamera   (camera);
    camera.setPosition   ({ 1000, 1000 }); // Selected view must be a snapshot.

    renderer.drawSprite  (blue, rect(0, 0, 16, 16));

    renderer.resetCamera ();

    renderer.drawRect    (rect(100, 10, 10, 10), colours::Green);

    renderer.endFrame();

    pixel(15, 15,  { 255, 0, 0 }, "draw before camera flush");
    pixel(70, 70,  { 0, 0, 255 }, "camera snapshot");
    pixel(105, 15, { 0, 255, 0 }, "HUD reset");

    begin();

    renderer.drawRect(rect(10, 10, 10, 10), colours::Red);
    renderer.endFrame();
    pixel(15, 15, { 255, 0, 0 }, "default restored next frame");

    begin();

    camera.setPosition({100, 100});
    camera.setZoom(2);

    renderer.setCamera(camera);
    renderer.drawRect(rect(100, 100, 5, 5), colours::Red);

    renderer.endFrame();

    pixel(72, 72, {255, 0, 0}, "camera zoom");
    pixel(76, 76, {0, 0, 0}, "camera zoom extent");

    begin();

    for (int i = 0; i < 4097; ++i)
    {
        renderer.drawRect(rect(-100, -100, 1, 1), colours::Red);
    }
    renderer.drawRect(rect(10, 10, 10, 10), colours::Green);

    renderer.endFrame();

    pixel(15, 15, {0, 255, 0}, "vertex/index capacity rollover");

    begin();

    std::vector<Texture2D> textures;
    for (int i = 0; i < 10; ++i)
    {
        textures.push_back(Texture2D::create(Image{ 1, 1, { static_cast<std::uint8_t>(i * 20), 0, 255, 255 } }));
        renderer.drawSprite(textures.back(), rect(static_cast<float>(i * 12), 10, 10, 10));
    }

    renderer.endFrame();

    for (int i = 0; i < 10; ++i)
    {
        pixel(i * 12 + 5, 15, {i * 20, 0, 255}, "texture slot rollover");
    }

    begin();

    auto temporary = Texture2D::create(Image{ 1, 1, { 255, 0, 255, 255 } });
    std::weak_ptr<const internal::GPUTexture> lifetime = internal::TextureAccess::resource(temporary);

    renderer.drawSprite(temporary, rect(10, 10, 10, 10));

    temporary = {};

    check(!lifetime.expired(), "texture retained before submission");

    renderer.endFrame();

    check(lifetime.expired(),             "texture released after submission");
    pixel(15, 15, { 255, 0, 255 }, "temporary texture drawing");

    begin();

    renderer.drawRect     (rect(10, 10, 20, 20),    colours::Red, false);
    renderer.drawTriangle (rect(40, 10, 20, 20),    colours::Green);
    renderer.drawCircle   ({ 80,  20 }, 10,             colours::Blue);
    renderer.drawCircle   ({ 110, 20 }, 10,             colours::Red, false);
    renderer.drawLine     ( { 10,  50 }, { 60, 50 }, colours::Green, 6);

    renderer.endFrame();

    pixel(20,  20, { 0,   0,   0   }, "outline centre empty");
    pixel(9,   20, { 255, 0,   0   }, "rect outline edge");
    pixel(50,  20, { 0,   255, 0   }, "triangle fill");
    pixel(80,  20, { 0,   0,   255 }, "circle fill");
    pixel(110, 20, { 0,   0,   0   }, "circle outline centre empty");
    pixel(30,  52, { 0,   255, 0   }, "line thickness inside");
    pixel(30,  54, { 0,   0,   0   }, "line thickness outside");

    renderer.beginFrame ({ { 64, 64 }, Size, Size }, colours::Black);
    renderer.drawRect   (rect(10, 10, 10, 10), colours::Red);

    renderer.endFrame();

    pixel(25, 25, { 255, 0, 0 }, "logical/framebuffer size separation");

    renderer.beginFrame({{0, 0}, 0, 0}, colours::Black);

    renderer.setCamera(camera);
    renderer.resetCamera();

    renderer.drawRect(rect(0, 0, 10, 10), colours::Red);

    renderer.endFrame();

    begin();

    renderer.drawRect(rect(10, 10, 10, 10), colours::Green);
    renderer.endFrame();

    pixel(15, 15, { 0, 255, 0 }, "resume after zero-size frame");

    bool rejected = false;

    try
    {
        (void)Texture2D::create(Image{ 2, 2, { 255 } });
    }
    catch (const std::invalid_argument&)
    {
        rejected = true;
    }

    check(rejected, "invalid image rejected before upload");

    GLuint unpackBuffer;
    glCreateBuffers(1, &unpackBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, unpackBuffer);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 16);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 1);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 1);

    auto corners = Texture2D::create(Image{ 2, 2, {
        255, 0, 0,   255, 0,   255, 0,   255,
        0,   0, 255, 255, 255, 255, 255, 255
    } });

    GLint restored = 0;

    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &restored);
    check(restored == 16, "unpack layout restored");

    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &restored);
    check(static_cast<GLuint>(restored) == unpackBuffer, "unpack buffer restored");

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glDeleteBuffers(1, &unpackBuffer);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    begin();

    renderer.drawSprite(corners, rect(0, 0, 40, 40));
    renderer.endFrame();

    pixel(5,  5,  { 255, 0, 0 },  "UV top-left");
    pixel(35, 5,  { 0, 255, 0 },  "UV top-right");
    pixel(5,  35, { 0, 0, 255 },  "UV bottom-left");

    if (preview)
    {
        renderer.beginFrame   (frame, { 0.05f, 0.06f, 0.09f, 1 });

        renderer.drawRect     (rect(8, 8,  28, 24), colours::Cyan);
        renderer.drawTriangle (rect(48, 8, 28, 24), colours::Red);

        renderer.drawCircle   ({104, 20}, 13, colours::Green);

        renderer.drawRect     (rect(8,  44, 28, 24), colours::Cyan, false);
        renderer.drawTriangle (rect(48, 44, 28, 24), colours::Red,  false);

        renderer.drawCircle   ({ 104, 56 }, 13,             colours::Green, false);
        renderer.drawLine     ( { 8,   84 }, { 74, 99 }, colours::White, 5);

        renderer.drawSprite   (corners, rect(88, 80, 30, 30));
        renderer.drawSprite   (blue,    rect(12, 94, 24, 24), { 1, 1, 1, 0.5f });

        renderer.drawRect     (rect(24, 100, 24, 20), { 1, 0, 0, 0.5f });

        renderer.endFrame();

        std::vector<unsigned char> pixels(Size * Size * 4);

        glReadPixels(0, 0, Size, Size, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        std::ofstream output(preview, std::ios::binary);
        output << "P6\n" << Size << ' ' << Size << "\n255\n";

        for (int y = Size - 1; y >= 0; --y)
        {
            for (int x = 0; x < Size; ++x)
            {
                output.write(reinterpret_cast<const char*>(pixels.data() + (y * Size + x) * 4), 3);
            }
        }
    }

    check(glGetError() == GL_NO_ERROR, "OpenGL error-free");
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

    auto* window = glfwCreateWindow(Size, Size, "Renderer tests", nullptr, nullptr);
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
        std::cout << "GPU: " << glGetString(GL_RENDERER) << '\n';
        run(argc > 1 ? argv[1] : nullptr);
        std::cout << "GPU checks passed: " << checks << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << "GPU failure: " << e.what() << '\n';
        result = 1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return result;
}
