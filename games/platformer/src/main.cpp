#include "PlatformerGame.hpp"

#include <forge2d/core/Application.hpp>

#include <exception>
#include <iostream>
#include <memory>

int main()
{
    try
    {
        forge::ApplicationConfig config;

        config.window.title = "Forge2D Platformer";

        config.window.width  = 800;
        config.window.height = 600;
        config.window.vsync  = true;

        auto game = std::make_unique<PlatformerGame>();

        forge::Application app(config, std::move(game));

        return app.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
}
