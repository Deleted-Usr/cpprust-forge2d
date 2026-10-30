#include "PlatformerGame.hpp"

#include <forge2d/core/Application.hpp>

#include <exception>
#include <iostream>
#include <memory>

int main()
{
    try
    {
        const forge::ApplicationConfig config {
            .window {
                .title = "Forge2D Platformer",

                .width = 800,
                .height = 600,

                .vsync = true
            }
        };

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
