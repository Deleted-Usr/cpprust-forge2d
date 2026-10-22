#pragma once

#include <forge2d/core/Game.hpp>
#include <forge2d/math/vec2.hpp>

class PlatformerGame final : public forge::Game
{
    public:
        void onStart() override;
        void onStop()  override;

        void update(float deltaTime) override;
        void render(forge::Renderer2D& renderer) override;

    private:
        Vec2 position_ = {.x = 100.f, .y = 100.f};
};
