#pragma once

#include <exception>
#include <span>
#include <vector>

#include <forge2d/input/internal/InputEvents.hpp>

struct GLFWwindow;

namespace forge::internal::glfw
{
    /// @brief Collects translated input events for one native window.
    ///
    /// Window owns this collector and reserves the native window's user pointer
    /// and input callback slots for it. Callbacks only append events; InputManager
    /// builds gameplay state when Window drains the queue.
    ///
    /// @note Main-thread only. There is no synchronization between collection
    /// and takeEvents(), and callbacks may run during other GLFW window calls.
    class GLFWInput
    {
        public:
            /// @brief Attaches input callbacks and queues initial focus/cursor state.
            /// @param window A valid, initialized GLFW window that outlives this object.
            /// @pre Its user pointer and input callback slots are available for exclusive use.
            /// Existing callbacks are replaced, not chained or saved for restoration.
            explicit GLFWInput(GLFWwindow* window);

            /// @brief Removes the installed callbacks and clears the window user pointer.
            /// Must run on the main thread before the native window is destroyed.
            ~GLFWInput();

            GLFWInput(const GLFWInput&) = delete;
            GLFWInput& operator=(const GLFWInput&) = delete;

            /// @brief Drains pending events in arrival order without copying the queue.
            ///
            /// Swaps the pending and delivered buffers, reusing their allocations.
            /// New callbacks append to the new pending buffer without modifying the
            /// returned events. An empty drain is valid and does not replay old events.
            ///
            /// @return A borrowed view valid until the next takeEvents() call or destruction.
            /// @throws Rethrows the first exception captured by enqueue(). The error is
            /// retained; a failed collector must not be used to continue gameplay.
            [[nodiscard]] std::span<const InputEvent> takeEvents();

        private:
            /// @brief Queues translated key presses/releases; ignores repeats and unknown keys.
            /// Scancode and modifier arguments are currently unused. Finds this collector
            /// through the window user pointer; does nothing if that pointer is null.
            static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);

            /// @brief Queues supported mouse-button presses/releases, ignoring other actions.
            /// Translation rejects unsupported button values before they reach InputManager.
            /// Modifier bits are currently unused.
            static void mouseButtonCallback(GLFWwindow*, int button, int action, int mods);

            /// @brief Queues absolute window-local logical coordinates (+X right, +Y down).
            /// InputManager computes movement from successive samples, not this callback.
            static void cursorPositionCallback(GLFWwindow*, double x, double y);

            /// @brief Queues fractional horizontal/vertical scroll offsets; +Y means up.
            /// InputManager accumulates all offsets delivered during an input frame.
            static void scrollCallback(GLFWwindow*, double x, double y);

            /// @brief Queues one Unicode code point supplied by GLFW's character callback.
            /// Character input is separate from physical keys and is not UTF-8 encoded here.
            static void textCallback(GLFWwindow*, unsigned int codepoint);

            /// @brief Queues focus changes in order with the other input events.
            /// InputManager handles releasing held buttons and resetting the cursor baseline.
            static void focusCallback(GLFWwindow*, int focused);

            /// @brief Appends an event without allowing exceptions to escape a GLFW callback.
            /// If allocation fails, stores the first exception for takeEvents() to rethrow.
            /// This is an error handoff, not a thread-safe queue or a recovery mechanism.
            void enqueue(InputEvent event) noexcept;

            GLFWwindow* window_; ///< Borrowed handle; Window destroys this collector first.

            /// Events received since the last drain, including outside pollEvents().
            std::vector<InputEvent> pending_;

            /// Owns the storage borrowed by the most recently returned span.
            std::vector<InputEvent> frameEvents_;

            /// Sticky callback failure, checked at the next C++ queue-drain boundary.
            std::exception_ptr callbackError_;
    };
}
