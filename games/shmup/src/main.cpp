#include "ShmupGame.hpp"

#include <forge2d/core/Application.hpp>
#include <forge2d/debug/Log.hpp>

int main()
{
    try
    {
        const forge::ApplicationConfig config {
            .window {
                .title = "Forge2D Shoot Em' Up",

                .width = 400,
                .height = 650,

                .vsync = true
            }
        };

        auto game = std::make_unique<ShmupGame>();
        forge::Application app(config, std::move(game));

        return app.run();
    }
    catch (std::exception& e)
    {
        FORGE_LOG_ERROR("Fatal Error: {}",e.what());
        return 1;
    }
}
