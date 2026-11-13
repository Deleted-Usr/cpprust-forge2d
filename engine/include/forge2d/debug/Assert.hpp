#pragma once

/// @file Assert.hpp
/// @brief Assertion macros for programmer errors, broken invariants and recoverable problems.
///
/// Pick the macro by what should happen when the condition is false:
///
/// | Macro               | Checked in        | On failure                                       |
/// | ------------------- | ----------------- | ------------------------------------------------ |
/// | `FORGE_ASSERT`      | Debug builds only | Logs `FATAL`, breaks in Debug builds, aborts     |
/// | `FORGE_CHECK`       | Every build       | Logs `FATAL`, breaks in Debug builds, aborts     |
/// | `FORGE_ENSURE`      | Every build       | Logs `ERROR` once per call site, returns `false` |
/// | `FORGE_UNREACHABLE` | Every build       | Logs `FATAL`, breaks in Debug builds, aborts     |
///
/// Every macro takes an optional `std::format`-style message after the condition:
///
/// @code
/// FORGE_ASSERT(index < size);
/// FORGE_ASSERT(index < size, "index {} out of range ({})", index, size);
/// @endcode
///
/// Failures are logged through `forge::log` with the file and line of the failing macro. In
/// Debug builds, fatal failures then stop in the debugger with `FORGE_DEBUG_BREAK()` before
/// aborting.

#include <atomic>
#include <format>
#include <source_location>
#include <string>
#include <string_view>

// ------------------------------------------------------------
// FORGE_DEBUG_BREAK(): stop in the debugger if one is attached
// ------------------------------------------------------------

/// @def FORGE_DEBUG_BREAK()
/// @brief Stops in the debugger at this line.
///
/// @warning If no debugger is attached, this usually terminates the program.
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
    /// @brief Logs a failed assertion as `FATAL`, breaks into the debugger in Debug builds, then
    ///        aborts.
    ///
    /// Used by the macros; prefer them in normal code.
    [[noreturn]] void assertFailed(std::string_view expression, std::source_location location, std::string_view message = {});

    /// @brief Logs a failed `FORGE_ENSURE` as `ERROR` and returns, so the caller can recover.
    ///
    /// Used by the macros; prefer them in normal code.
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

/// @def FORGE_ASSERT(condition, ...)
/// @brief Stops the program if @p condition is false. For programmer errors, checked in Debug
///        builds only.
///
/// Use it for mistakes in calling code, such as passing an out-of-range index.
///
/// @param condition Expression that must be true.
/// @param ...       Optional `std::format` string and arguments describing the failure.
///
/// @warning Outside Debug builds, @p condition is **not evaluated**, so it must have no side
///          effects. `FORGE_ASSERT(file.open())` would never open the file in a release build.
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

/// @def FORGE_CHECK(condition, ...)
/// @brief Stops the program if @p condition is false, in every build.
///
/// Use it for invariants where carrying on would corrupt state or crash later, such as failing
/// to load the OpenGL functions. @p condition is always evaluated.
///
/// @param condition Expression that must be true.
/// @param ...       Optional `std::format` string and arguments describing the failure.
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

/// @def FORGE_ENSURE(condition, ...)
/// @brief Checks @p condition in every build and logs an error if it's false, without
///        stopping the program.
///
/// Use it for recoverable problems, such as bad arguments that can be ignored. Each call site
/// only logs the **first** time it fails, so it can't flood the console from inside a loop.
/// The message is only formatted when it's logged.
///
/// @param condition Expression to check. Always evaluated.
/// @param ...       Optional `std::format` string and arguments describing the failure.
/// @return @p condition converted to `bool`, so it can guard an early return.
///
/// @code
/// if (!FORGE_ENSURE(texture.valid(), "drawSprite with empty texture")) return;
/// @endcode
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

/// @def FORGE_UNREACHABLE(...)
/// @brief Marks code that should be impossible to reach, such as a `switch` case for an
///        invalid enum value. Stops the program if it's reached, in every build.
///
/// @param ... Optional `std::format` string and arguments describing the failure.
#define FORGE_UNREACHABLE(...) ::forge::detail::assertFailed("unreachable", std::source_location::current() FORGE_DETAIL_MESSAGE(__VA_ARGS__))
