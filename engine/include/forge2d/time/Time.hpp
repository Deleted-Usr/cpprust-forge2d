#pragma once

#include <string>
#include <format>

namespace forge::time
{
    class Time
    {
        public:
            static Time Zero();

            // Factory methods
            static Time seconds(float amount);
            static Time milliseconds(long amount);
            static Time microseconds(long amount);

            // Conversions
            [[nodiscard]]
            float asSeconds() const;

            [[nodiscard]]
            long asMilliseconds() const;

            [[nodiscard]]
            long asMicroseconds() const;

            [[nodiscard]]
            std::string toString() const;

        private:
            long ms = 0L; // Internal microseconds

            explicit Time(long microseconds); // Private constructor to enforce the use of factory methods.
    };
}
