// Game.hpp
#pragma once

namespace forge
{
    class Renderer2D;

    class Game
    {
        public:
            virtual ~Game() = default;

            virtual void onStart() {}
            virtual void onStop()  {}

            virtual void update(float deltaTime)      = 0;
            virtual void render(Renderer2D& renderer) = 0;
    };
}
