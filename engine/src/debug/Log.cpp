#include <forge2d/debug/Log.hpp>

#include <atomic>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace forge::log
{
    /// Function-local statics: initialised on first use, so logging works even
    /// from other static initialisers
    namespace
    {
        struct State
        {
            std::atomic<Level> level { Level::TRACE };
            std::atomic<bool> console { true };

            std::mutex mutex; // guards sinks and keeps lines from interleaving
            std::vector<Sink> sinks;

            const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        };

        State& state()
        {
            static State s;
            return s;
        }

        std::string_view fileName(std::string_view path) noexcept
        {
            const auto slash = path.find_last_of("/\\");
            return slash == std::string_view::npos ? path : path.substr(slash + 1);
        }

        std::string_view colour(const Level level) noexcept
        {
            switch (level)
            {
                case Level::TRACE: return "\x1b[90m";    // grey
                case Level::DEBUG: return "\x1b[36m";    // cyan
                case Level::INFO:  return "\x1b[32m";    // green
                case Level::WARN:  return "\x1b[33m";    // yellow
                case Level::ERROR: return "\x1b[31m";    // red
                case Level::FATAL: return "\x1b[1;41m";  // bold, red background
                case Level::OFF:   break;
            }

            return "";
        }

        // [   1.234s] [WARN ] AssetManager.cpp:42  could not load texture: ...
        void writeConsole(const Record& record)
        {
            const double seconds = std::chrono::duration<double>(record.uptime).count();

            const std::string line = std::format(
                "{}[{:9.3f}s] [{:<5}]\x1b[0m {}:{}  {}\n",

                colour(record.level),

                seconds,

                toString(record.level),
                fileName(record.location.file_name()),

                record.location.line(),
                record.message
            );

            std::FILE* stream = record.level >= Level::ERROR ? stderr : stdout;
            std::fputs(line.c_str(), stream);

            if (record.level >= Level::ERROR)
            {
                std::fflush(stream); // make sure it's visible before a possible crash
            }
        }
    }

    void setLevel(const Level level) noexcept
    {
        state().level.store(level, std::memory_order_acquire);
    }

    Level level() noexcept
    {
        return state().level.load(std::memory_order_relaxed);
    }

    void setConsoleEnabled(const bool enabled) noexcept
    {
        state().console.store(enabled);
    }

    void addSink(Sink sink)
    {
        auto& s = state();
        const std::scoped_lock lock(s.mutex);
        s.sinks.push_back(sink);
    }

    std::string_view toString(const Level level) noexcept
    {
        switch (level)
        {
            case Level::TRACE: return "TRACE";
            case Level::DEBUG: return "DEBUG";
            case Level::INFO:  return "INFO";
            case Level::WARN:  return "WARN";
            case Level::ERROR: return "ERROR";
            case Level::FATAL: return "FATAL";
            case Level::OFF:   return "OFF";
        }

        return "?";
    }

    void detail::write(const Level level, const std::source_location location, const std::string_view message)
    {
        auto& s = state();

        const Record record {
            .level    = level,
            .message  = message,
            .location = location,
            .uptime   = std::chrono::steady_clock::now() - s.start,
        };

        const std::scoped_lock lock(s.mutex);

        if (s.console.load())
        {
            writeConsole(record);
        }

        for (const Sink& sink : s.sinks)
        {
            sink(record);
        }
    }
}
