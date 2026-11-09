#pragma once

#include <forge2d/time/Time.hpp>

namespace forge::time
{
    class Clock
    {
        public:
            Clock();

            [[nodiscard]]
            bool isRunning() const;

            [[nodiscard]]
            Time getElapsedTime() const;

            void start();
            void stop();

            /// Resets the elapsed time to zero and leaves it running.
            /// @return the time before the restart.
            Time restart();

            /// Resets the elapsed time to zero and stops the clock.
            /// @return the elapsed time before the reset.
            Time reset();

        private:
            std::int64_t refNanos_;
            std::int64_t stopNanos_;
    };
}
