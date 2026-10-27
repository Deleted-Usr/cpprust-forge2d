#include <forge2d/time/Time.hpp>

namespace forge::time
{
    Time::Time(long microseconds)
    {
        this->ms = microseconds;
    }

    Time Time::Zero()
    {
        return Time(0L);
    }

    // Factory Methods
    Time Time::seconds(float amount)
    {
        return Time(static_cast<long>(amount * 1'000'000.0f));
    }

    Time Time::milliseconds(long amount)
    {
        return Time(amount * 1000.0f);
    }

    Time Time::microseconds(long amount)
    {
        return Time(amount);
    }

    // Conversions
    float Time::asSeconds() const
    {
        return ms / 1'000'000.0f;
    }

    long Time::asMilliseconds() const
    {
        return ms / 1000L;
    }

    long Time::asMicroseconds() const
    {
        return ms;
    }

    std::string Time::toString() const
    {
        return std::format("Time({})", asSeconds());
    }
}
