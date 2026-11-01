#include <forge2d/debug/Assert.hpp>
#include <forge2d/debug/Log.hpp>

#include <cstdlib>

namespace forge::detail
{
    namespace
    {
        /// Logs through the normal path, but with the failing call site's location
        /// instead of this file's.
        void report(
            const log::Level level,
            const std::string_view kind,
            const std::string_view expression,
            const std::source_location location,
            const std::string_view message
        )
        {
            if (message.empty())
            {
                log::detail::write(level, location, std::format("{} failed: {}", kind, expression));
            }
            else
            {
                log::detail::write(level, location, std::format("{} failed: {} ({})", kind, expression, message));
            }
        }
    }

    void assertFailed(const std::string_view expression, const std::source_location location, const std::string_view message)
    {
        report(log::Level::FATAL, "Assertion", expression, location, message);

        #ifndef FORGE2D_DEBUG
            FORGE_DEBUG_BREAK();
        #endif

        std::abort();
    }

    void ensureFailed(std::string_view expression, std::source_location location, std::string_view message)
    {
        report(log::Level::ERROR, "Ensure", expression, location, message);

        #ifndef FORGE2D_DEBUG
            // Uncomment to stop in the debugger on the first failure of each ENSURE:
            // FORGE_DEBUG_BREAK();
        #endif
    }
}
