#include <forge2d/time/Clock.hpp>

#include <chrono>

namespace forge::time
{
    namespace
    {
        std::int64_t nowNanos()
        {
            using namespace std::chrono;
            return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
        }
    }

    Clock::Clock() : refNanos_(nowNanos()), stopNanos_(0) { }

    bool Clock::isRunning() const
    {
        return stopNanos_ == 0;
    }

    Time Clock::getElapsedTime() const
    {
        const std::int64_t end = isRunning() ? nowNanos() : stopNanos_;
        return Time::microseconds(static_cast<long>((end - refNanos_) / 1000));
    }

    void Clock::start()
    {
        if (!isRunning())
        {
            refNanos_ += nowNanos() - stopNanos_;
            stopNanos_ = 0;
        }
    }

    void Clock::stop()
    {
        if (isRunning())
        {
            stopNanos_ = nowNanos();
        }
    }

    Time Clock::restart()
    {
        const Time elapsed = getElapsedTime();

        refNanos_  = nowNanos();
        stopNanos_ = 0;

        return elapsed;
    }

    Time Clock::reset()
    {
        const Time         elapsed = getElapsedTime();
        const std::int64_t now     = nowNanos();

        refNanos_  = now;
        stopNanos_ = now;

        return elapsed;
    }
}