#include <forge2d/time/Clock.hpp>

#include <GLFW/glfw3.h>

namespace forge::time
{
    Clock::Clock()
    {
        refNanos  = glfwGetTime();
        stopNanos = 0L;
    }

    bool Clock::isRunning() const
    {
        return stopNanos == 0L;
    }

    Time Clock::getElapsedTime() const
    {
        long end           = isRunning() ? glfwGetTime() : stopNanos;
        long elapsedMicros = (end - refNanos) / 1000L;

        return Time::microseconds(elapsedMicros);
    }

    void Clock::start()
    {
        if (!isRunning())
        {
            refNanos += glfwGetTime() - stopNanos;
            stopNanos = 0L;
        }
    }

    void Clock::stop()
    {
        if (isRunning())
        {
            stopNanos = glfwGetTime();
        }
    }

    Time Clock::restart()
    {
        Time elapsed = getElapsedTime();

        refNanos  = glfwGetTime();
        stopNanos = 0L;

        return elapsed;
    }

    Time Clock::reset()
    {
        Time elapsed = getElapsedTime();
        long now     = glfwGetTime();

        refNanos  = now;
        stopNanos = now;

        return elapsed;
    }
}
