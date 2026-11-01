#pragma once

#include <atomic>
#include <format>
#include <source_location>
#include <string>
#include <string_view>

// ------------------------------------------------------------
// FORGE_DEBUG_BREAK(): stop in the debugger if one is attached
// ------------------------------------------------------------
#if defined(_MSC_VER)
    #define FORGE_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__)
    #define FORGE_DEBUG_BREAK() __builtin_debugtrap()
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    #define FORGE_DEBUG_BREAK() __asm__ __volatile("int3")
#else
    #include <csignal>
    #define FORGE_DEBUG_BREAK() std::raise(SIGTRAP)
#endif

namespace forge::detail
{
    /// Logs the failure, breaks into the debugger, then aborts.
    [[noreturn]] void assertFailed(std::string_view expression, std::source_location location, std::string_view message = {});

    /// Logs the failure as an error and returns; the caller decides how to recover.
    void ensureFailed(std::string_view expression, std::source_location location, std::string_view message = {});
}

// Optional message, std::format style:
//   FORGE_ASSERT(index < size);
//   FORGE_ASSERT(index < size, "index {} out of range ({})", index, size);
#define FORGE_DETAIL_MESSAGE(...) __VA_OPT__(, std::format(__VA_ARGS__))

// ------------------------------------------------------------
// FORGE_ASSERT: programmer errors. Debug builds only; in release the condition
// is NOT evaluated, so it must have no side effects.
// ------------------------------------------------------------
#ifdef FORGE2D_DEBUG
    #define FORGE_ASSERT(condition, ...)                                                                                      \
        do                                                                                                                    \
        {                                                                                                                     \
            if (!(condition)) [[unlikely]]                                                                                    \
            {                                                                                                                 \
                ::forge::detail::assertFailed(#condition, std::source_location::current() FORGE_DETAIL_MESSAGE(__VA_ARGS__)); \
            }                                                                                                                 \
        } while (false)
#else
    // sizeof keeps variables "used" (no unused warnings) without evaluating anything.
    #define FORGE_ASSERT(condition, ...) ((void)sizeof(!(condition)))
#endif

// ------------------------------------------------------------
// FORGE_CHECK: invariants that must hold in EVERY build, where carrying on
// would corrupt state (e.g. no GL context). Always evaluated, always fatal.
// ------------------------------------------------------------
#define FORGE_CHECK(condition, ...)                                                                                       \
    do                                                                                                                    \
    {                                                                                                                     \
        if (!(condition)) [[unlikely]]                                                                                    \
        {                                                                                                                 \
            ::forge::detail::assertFailed(#condition, std::source_location::current() FORGE_DETAIL_MESSAGE(__VA_ARGS__)); \
        }                                                                                                                 \
    } while (false)

// ------------------------------------------------------------
// FORGE_ENSURE: recoverable problems. Every build, NOT fatal. Logs only the first
// time each call site fails, so it can't flood the console from a draw loop.
// Returns the condition, so it can guard an early-out:
//
//   if (!FORGE_ENSURE(texture.valid(), "drawSprite with empty texture")) return;
// ------------------------------------------------------------
#define FORGE_ENSURE(condition, ...)                                                                                      \
    (static_cast<bool>(condition) || [&]() -> bool {                                                                      \
        static std::atomic_flag reported;                                                                                 \
                                                                                                                          \
        if (!reported.test_and_set())                                                                                     \
        {                                                                                                                 \
            ::forge::detail::ensureFailed(#condition, std::source_location::current() FORGE_DETAIL_MESSAGE(__VA_ARGS__)); \
        }                                                                                                                 \
                                                                                                                          \
        return false;                                                                                                     \
    }())

// For switch cases / branches that should be impossible.
#define FORGE_UNREACHABLE(...) ::forge::detail::assertFailed("unreachable", std::source_location::current() FORGE_DETAIL_MESSAGE(__VA_ARGS__))
