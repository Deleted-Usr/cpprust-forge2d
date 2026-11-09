#include "TestSupport.hpp"
#include <forge2d/graphics/Camera2D.hpp>
#include <graphics/internal/BatchBuilder2D.hpp>
#include <graphics/internal/RenderView2D.hpp>
#include <numbers>

using namespace forge;

// Opaque tokens: the CPU builder retains and compares resources, never dereferences them.
std::shared_ptr<const internal::GPUTexture> textureToken()
{
    auto owner = std::make_shared<int>(0);
    return { owner, reinterpret_cast<const internal::GPUTexture*>(owner.get()) };
}

void batchTests()
{
    auto white = textureToken();

    internal::BatchBuilder2D builder(white, 256, 384, 2);
    math::Transform2D transform{ .position = { 10, 20 }, .scale = { 30, 40 } };

    check(builder.tryAppendRect(transform, colours::Red, true), "append rect");

    const auto& batch = builder.batch();

    check(batch.vertices.size() == 4 && batch.indices.size() == 6,                                  "rect geometry");
    check(near(batch.vertices[2].position.x, 40) && near(batch.vertices[2].position.y, 60), "CPU transform");

    auto sprite = textureToken();
    std::weak_ptr<const internal::GPUTexture> lifetime = sprite;

    check(builder.tryAppendSprite(sprite, transform, colours::White), "append second texture");
    check(batch.vertices[4].textureSlot == 1 && batch.indices[6] == 4,    "slot and index offset");

    auto third = textureToken();

    check(!builder.tryAppendSprite(third, transform, colours::White),   "texture slot rollover");
    check(batch.vertices.size() == 8 && batch.textures.size() == 2, "failed append is atomic");

    sprite.reset();
    check(!lifetime.expired(), "queued texture remains alive");

    builder.clear();
    check(lifetime.expired(), "clear releases queued texture");

    for (int i = 0; i < 64; ++i)
    {
        check(builder.tryAppendRect(transform, colours::Red, true), "fill geometry capacity");
    }

    check(!builder.tryAppendRect(transform, colours::Blue, true), "geometry rollover");
    builder.clear();
    check(builder.tryAppendRect(transform, colours::Blue, true), "retry after clear");

    builder.clear();
    check(builder.tryAppendLine({ 0, 0 }, { 10, 0 }, colours::White, 6), "thick line");
    check(near(builder.batch().vertices[0].position.y, 3) && near(builder.batch().vertices[2].position.y, -3), "line thickness geometry");
    builder.clear();

    check(builder.tryAppendLine({ 1, 1 }, { 1, 1 }, colours::White, 6), "zero line no-op");
    check(builder.tryAppendCircle({ }, 0, colours::White, true), "zero circle no-op");

    check(builder.batch().empty(), "degenerate primitives emit no triangles");

    check(builder.tryAppendCircle({ }, 10, colours::White, false), "largest outline fits");

    for (auto index : builder.batch().indices)
    {
        check(index < builder.batch().vertices.size(), "valid outline index");
    }
    builder.clear();

    check(builder.tryAppendTriangle(transform, colours::White, false), "triangle outline");
}

void cameraTests()
{
    Camera2D camera;

    camera.setPosition({ 100, 50 });
    camera.setZoom(2);

    auto matrix = camera.viewProjection({ 800, 600 });
    auto centre = matrix * math::Vec4{ 100, 50, 0, 1 };

    check(near(centre.x, 0) && near(centre.y, 0), "camera centre");

    auto offset = matrix * math::Vec4{ 110, 50, 0, 1 };

    check(near(offset.x, 0.05f), "camera zoom");

    camera.setRotation(std::numbers::pi_v<float> * 0.5f);
    auto rotated = camera.viewProjection({ 800, 600 }) * math::Vec4{ 110, 50, 0, 1 };

    check(near(rotated.x, 0) && near(rotated.y, 20.0f / 300.0f), "camera rotation");

    struct Target { math::Vec2 point; [[nodiscard]] math::Vec2 position() const { return point; } };
    Target target{{ 100, 100 }};

    Camera2D fast, slow;

    fast.setSmoothing(0.15f);
    slow.setSmoothing(0.15f);
    fast.follow(target);
    slow.follow(target);

    for (int i = 0; i < 60; ++i) fast.update(1.0f / 60);
    for (int i = 0; i < 120; ++i) slow.update(1.0f / 120);

    check(near(fast.position().x, slow.position().x, 0.001f), "frame rate independent smoothing");

    Camera2D follow;

    follow.setLookAhead({ 10, 0 });

    target.point = { -100, 0 };

    follow.follow(target);
    follow.update(0.1f);

    check(near(follow.position().x, -90), "first follow does not invent a facing velocity");

    target.point.x -= 10;
    follow.update(0.1f);

    check(near(follow.position().x, -120), "movement updates facing");

    follow.stopFollowing();
    follow.setPosition({ 5, 6 });
    follow.update(1);

    check(near(follow.position().x, 5), "manual control after stopFollowing");

    follow.shake(20, 0.2f);
    follow.update(0.1f);

    check(near(follow.position().x, 5), "shake preserves base position");

    follow.update(1);
    const auto stable = follow.viewProjection({ 100, 100 }) * math::Vec4{ 5, 6, 0, 1 };

    check(near(stable.x, 0) && near(stable.y, 0), "shake expires");

    follow.follow(target);
    follow.setSmoothing(1);
    follow.snapToTarget();
    follow.update(0);

    check(near(follow.position().x, -120), "snap bypasses smoothing");

    auto screen = internal::RenderView2D::screen({ 800, 600 }).viewProjection;
    const auto topLeft = screen * math::Vec4{ 0, 0, 0, 1 };

    check(near(topLeft.x, -1) && near(topLeft.y, 1), "screen projection top-left origin");
}

int main()
{
    try
    {
        batchTests();
        cameraTests();

        std::cout << "CPU checks passed: " << checks << '\n';

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "CPU failure: " << e.what() << '\n';
        return 1;
    }
}

