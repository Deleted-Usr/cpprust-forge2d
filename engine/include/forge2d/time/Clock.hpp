#pragma once

/// @file Clock.hpp
/// @brief A stopwatch for measuring elapsed time.

#include <forge2d/time/Time.hpp>

namespace forge::time
{
    /// @brief A stopwatch that measures real elapsed time and can be paused.
    ///
    /// A new clock starts running immediately. It uses a monotonic system clock, so it's
    /// unaffected by changes to the computer's date and time.
    ///
    /// @code
    /// forge::time::Clock clock;               // running from zero
    /// // ...
    /// const forge::time::Time lap = clock.restart();
    /// FORGE_LOG_INFO("level took {}s", lap.asSeconds());
    /// @endcode
    ///
    /// @note Not thread-safe. Don't call a non-const function while another thread uses the
    ///       same clock.
    class Clock
    {
        public:
            /// @brief Creates a clock that starts running from zero.
            Clock();

            /// @brief Whether the clock is running (not stopped).
            [[nodiscard]]
            bool isRunning() const;

            /// @brief Time counted so far. While stopped, it stays at the time when it was stopped.
            [[nodiscard]]
            Time getElapsedTime() const;

            /// @brief Resumes a stopped clock, continuing from its current elapsed time. Does
            ///        nothing if it's already running.
            void start();

            /// @brief Pauses the clock, keeping its elapsed time. Does nothing if it's already
            ///        stopped.
            void stop();

            /// @brief Resets the elapsed time to zero and keeps (or starts) the clock running.
            ///
            /// Useful for measuring laps or frame times.
            ///
            /// @return The elapsed time before the restart.
            Time restart();

            /// @brief Resets the elapsed time to zero and stops the clock. Call `start()` to start
            ///        it again.
            ///
            /// @return The elapsed time before the reset.
            Time reset();

        private:
            std::int64_t refNanos_;
            std::int64_t stopNanos_;
    };
}
