#pragma once

#include <chrono>
#include <cstdint>
#include <format>
#include <functional>
#include <source_location>
#include <string_view>

namespace forge::log
{
    enum class Level : std::uint8_t
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        OFF
    };

    /// One log message, handed to every sink.
    struct Record
    {
        Level level;

        std::string_view message;
        std::source_location location;
        std::chrono::steady_clock::duration uptime; // time since the logger started
    };

    /// A sink receives every record that passes the level filter.
    /// The console sink is installed by default; an editor console
    /// (maybe egui!) or a log file would be extra sinks.
    using Sink = std::function<void(const Record&)>;

    void setLevel(Level level)  noexcept;
    [[nodiscard]] Level level() noexcept;

    [[nodiscard]] inline bool enabled(const Level l) noexcept { return l >= level(); }

    void addSink(Sink sink);
    void setConsoleEnabled(bool enabled) noexcept;

    [[nodiscard]] std::string_view toString(Level level) noexcept;

    namespace detail
    {
        void write(const Level level, const std::source_location location, const std::string_view message);
    }

// The level check happens BEFORE std::format runs, so a disabled log line costs
// one comparison, not a string allocation.
#define FORGE_LOG(lvl, ...)                                                                              \
    do                                                                                                   \
    {                                                                                                    \
        if (::forge::log::enabled(lvl))                                                                  \
        {                                                                                                \
            ::forge::log::detail::write(lvl, std::source_location::current(), std::format(__VA_ARGS__)); \
        }                                                                                                \
    } while (false)

// Trace/Debug are compiled out entirely in release builds.
#ifdef FORGE2D_DEBUG
    #define FORGE_LOG_TRACE(...) FORGE_LOG(::forge::log::Level::TRACE, __VA_ARGS__)
    #define FORGE_LOG_DEBUG(...) FORGE_LOG(::forge::log::Level::DEBUG, __VA_ARGS__)
#else
    #define FORGE_LOG_TRACE(...) ((void)0)
    #define FORGE_LOG_DEBUG(...) ((void)0)
#endif

#define FORGE_LOG_INFO(...)  FORGE_LOG(::forge::log::Level::INFO,  __VA_ARGS__)
#define FORGE_LOG_WARN(...)  FORGE_LOG(::forge::log::Level::WARN,  __VA_ARGS__)
#define FORGE_LOG_ERROR(...) FORGE_LOG(::forge::log::Level::ERROR, __VA_ARGS__)
#define FORGE_LOG_FATAL(...) FORGE_LOG(::forge::log::Level::FATAL, __VA_ARGS__)
}
