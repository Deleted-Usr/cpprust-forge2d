#pragma once

#include <memory>

#include <forge2d/core/Game.hpp>
#include <forge2d/core/Window.hpp>

#include <forge2d/graphics/Renderer2D.hpp>
#include <forge2d/assets/AssetManager.hpp>
#include <forge2d/graphics/Camera2D.hpp>

// TODO -
//  A one shot method that can be called from anywhere in game code that executes once
//  and never again until called, or executes on a timer.
//  ----------------------------------------------------------------------------------
//  The syntax would look something like this:
//  in main, users call oneShot.setCooldown(0.5f, isRepeating = true);
//  This would set the cooldown, and sets the repeating option.
//  If true, the method would be called repeatedly based on the cooldown.

namespace forge
{
    struct ApplicationConfig
    {
        WindowConfig window;
        std::filesystem::path assetRoot = "assets";
    };

    class Application
    {
        public:
            Application(const ApplicationConfig& config, std::unique_ptr<Game> game);

            int run();

        private:
            Window       window_;
            AssetManager assets_;
            Renderer2D   renderer_;

            Camera2D     camera_;

            std::unique_ptr<Game> game_;
    };
}
