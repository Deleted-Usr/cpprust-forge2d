#include <forge2d/time/Time.hpp>

namespace forge::time
{
    Time::Time(const std::int64_t microseconds) : micros_(microseconds) { }

    Time Time::Zero()
    {
        return Time(0);
    }

    // Factory Methods
    Time Time::seconds(float amount)
    {
        return Time(static_cast<std::int64_t>(static_cast<double>(amount) * 1'000'000.0));
    }

    Time Time::milliseconds(const std::int64_t amount)
    {
        return Time(amount * 1000);
    }

    Time Time::microseconds(const std::int64_t amount)
    {
        return Time(amount);
    }

    // Conversions
    float Time::asSeconds() const
    {
        return static_cast<float>(static_cast<double>(micros_) / 1'000'000.0);
    }

    std::int64_t Time::asMilliseconds() const
    {
        return micros_ / 1000;
    }

    std::int64_t Time::asMicroseconds() const
    {
        return micros_;
    }

    std::string Time::toString() const
    {
        return std::format("Time({})", asSeconds());
    }
}
