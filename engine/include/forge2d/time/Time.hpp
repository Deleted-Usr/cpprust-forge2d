#pragma once

/// @file Time.hpp
/// @brief A span of time with microsecond precision.

#include <cstdint>
#include <format>
#include <string>

namespace forge::time
{
    /// @brief A length of time, stored as a whole number of microseconds.
    ///
    /// Create one with `seconds()`, `milliseconds()`, `microseconds()` or `Zero()`, and read it
    /// back in whichever unit is convenient. It can be negative. `Clock::getElapsedTime()`
    /// returns one.
    ///
    /// @code
    /// const forge::time::Time cooldown = forge::time::Time::milliseconds(250);
    /// if (clock.getElapsedTime().asMicroseconds() >= cooldown.asMicroseconds())
    /// {
    ///     fire();
    ///     clock.restart();
    /// }
    /// @endcode
    ///
    /// The value is a 64-bit count, so it covers about ±292,000 years on every platform.
    class Time
    {
        public:
            /// @brief A time of zero.
            static Time Zero();

            // Factory methods

            /// @brief A time of @p amount seconds. Fractions below a microsecond are truncated
            ///        towards zero.
            static Time seconds(float amount);

            /// @brief A time of @p amount milliseconds.
            static Time milliseconds(std::int64_t amount);

            /// @brief A time of @p amount microseconds.
            static Time microseconds(std::int64_t amount);

            // Conversions

            /// @brief The time in seconds, including fractions.
            [[nodiscard]]
            float asSeconds() const;

            /// @brief The time in whole milliseconds, truncated towards zero.
            [[nodiscard]]
            std::int64_t asMilliseconds() const;

            /// @brief The time in microseconds, exactly as stored.
            [[nodiscard]]
            std::int64_t asMicroseconds() const;

            /// @brief A readable description in seconds, such as `"Time(1.5)"`, for logging.
            [[nodiscard]]
            std::string toString() const;

        private:
            std::int64_t micros_ = 0;

            explicit Time(std::int64_t microseconds); // Private constructor to enforce the use of factory methods.
    };
}
