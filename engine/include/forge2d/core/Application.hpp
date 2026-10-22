// Application.hpp
#pragma once

#include <memory>

#include <forge2d/core/Game.hpp>
#include <forge2d/core/Window.hpp>
#include <forge2d/graphics/Renderer2D.hpp>

namespace forge
{
    struct ApplicationConfig
    {
        WindowConfig window;
    };

    class Application
    {
        public:
            Application(const ApplicationConfig& config, std::unique_ptr<Game> game);

            int run();

        private:
            Window window_;

            Renderer2D renderer_;

            std::unique_ptr<Game> game_;
    };
}
