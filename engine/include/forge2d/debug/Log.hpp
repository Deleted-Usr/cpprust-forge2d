#pragma once

/// @file Log.hpp
/// @brief Levelled, thread-safe logging to the console and custom sinks.

#include <chrono>
#include <cstdint>
#include <format>
#include <functional>
#include <source_location>
#include <string_view>

/// @brief Logging functions and the `FORGE_LOG_*` macros.
///
/// Log with the macros, which take a `std::format` string and arguments and record the file
/// and line they were called from:
///
/// @code
/// FORGE_LOG_INFO("loaded {} textures", assets.textureCount());
/// FORGE_LOG_WARN("player fell out of the world at {}, {}", pos.x, pos.y);
/// @endcode
///
/// By default every level is shown, and messages go to the console with colours: `INFO`,
/// `WARN` and below to stdout, `ERROR` and `FATAL` to stderr (flushed immediately, so they
/// appear before a crash). Each line looks like:
///
/// @code{.txt}
/// [    1.234s] [WARN ] AssetManager.cpp:42  could not load texture: ...
/// @endcode
///
/// Logging is thread-safe, and lines from different threads never interleave.
namespace forge::log
{
    /// @brief How important a message is. Levels are ordered from least to most severe.
    enum class Level : std::uint8_t
    {
        TRACE, ///< Very detailed tracing. Compiled out of non-Debug builds.
        DEBUG, ///< Information for debugging. Compiled out of non-Debug builds.
        INFO,  ///< Normal progress messages.
        WARN,  ///< Something unexpected that the engine recovered from.
        ERROR, ///< Something failed. Written to stderr.
        FATAL, ///< A failure the program can't continue from. Logging it doesn't stop the program.
        OFF    ///< Pass to `setLevel()` to turn logging off. Not a message level.
    };

    /// @brief One log message, as given to every sink.
    struct Record
    {
        /// How important the message is.
        Level level;

        /// The formatted message, without a trailing newline.
        ///
        /// @warning Only valid during the sink call. Copy it to keep it.
        std::string_view message;

        /// Where the message was logged from.
        std::source_location location;

        /// Time since logging was first used.
        std::chrono::steady_clock::duration uptime; // time since the logger started
    };

    /// @brief A function that receives every message that passes the level filter.
    ///
    /// Add one with `addSink()` to send logs somewhere else, such as a file or an in-game
    /// console. Sinks are called in the order they were added, after the console.
    ///
    /// @warning Sinks are called while the logger's lock is held, from whichever thread logged
    ///          the message. A sink must not log itself (that would deadlock) and should be
    ///          quick.
    using Sink = std::function<void(const Record&)>;

    /// @brief Sets the least severe level that's logged. Messages below it are discarded.
    ///
    /// @param level The new minimum level. `Level::OFF` turns logging off. Defaults to
    ///              `Level::TRACE` (log everything).
    void setLevel(Level level)  noexcept;

    /// @brief The current minimum level. See `setLevel()`.
    [[nodiscard]] Level level() noexcept;

    /// @brief Whether a message at level @p l would currently be logged.
    ///
    /// Use it to skip expensive work that only feeds a log message.
    [[nodiscard]] inline bool enabled(const Level l) noexcept { return l >= level(); }

    /// @brief Adds a sink that receives every logged message from now on.
    ///
    /// Sinks can't be removed, so anything @p sink captures must live until the program ends.
    ///
    /// @code
    /// forge::log::addSink([](const forge::log::Record& r) {
    ///     logFile << forge::log::toString(r.level) << ' ' << r.message << '\n';
    /// });
    /// @endcode
    void addSink(Sink sink);

    /// @brief Turns the built-in console output on or off. Sinks are unaffected.
    ///
    /// @param enabled `true` (the default) to write to stdout/stderr.
    void setConsoleEnabled(bool enabled) noexcept;

    /// @brief The name of @p level in capitals, such as `"WARN"`.
    [[nodiscard]] std::string_view toString(Level level) noexcept;

    namespace detail
    {
        /// @brief Sends an already formatted message to the console and every sink. Used by the
        ///        macros; prefer them in normal code.
        void write(const Level level, const std::source_location location, const std::string_view message);
    }

    /// @brief Logs a `std::format`-style message at level @p lvl.
    ///
    /// The level is checked **before** the message is formatted, so a filtered-out message
    /// costs one comparison and no allocation. Prefer the level-specific macros below.
    ///
    /// @param lvl A `forge::log::Level`.
    /// @param ... A format string followed by its arguments.
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
        /// @brief Logs at `TRACE` level. In non-Debug builds it does nothing and its arguments
        ///        aren't evaluated.
        #define FORGE_LOG_TRACE(...) FORGE_LOG(::forge::log::Level::TRACE, __VA_ARGS__)

        /// @brief Logs at `DEBUG` level. In non-Debug builds it does nothing and its arguments
        ///        aren't evaluated.
        #define FORGE_LOG_DEBUG(...) FORGE_LOG(::forge::log::Level::DEBUG, __VA_ARGS__)
    #else
        #define FORGE_LOG_TRACE(...) ((void)0)
        #define FORGE_LOG_DEBUG(...) ((void)0)
    #endif

    /// @brief Logs at `INFO` level.
    #define FORGE_LOG_INFO(...)  FORGE_LOG(::forge::log::Level::INFO,  __VA_ARGS__)

    /// @brief Logs at `WARN` level.
    #define FORGE_LOG_WARN(...)  FORGE_LOG(::forge::log::Level::WARN,  __VA_ARGS__)

    /// @brief Logs at `ERROR` level.
    #define FORGE_LOG_ERROR(...) FORGE_LOG(::forge::log::Level::ERROR, __VA_ARGS__)

    /// @brief Logs at `FATAL` level. This only logs; it doesn't stop the program (use
    ///        `FORGE_CHECK` for that).
    #define FORGE_LOG_FATAL(...) FORGE_LOG(::forge::log::Level::FATAL, __VA_ARGS__)
}
