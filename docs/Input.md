# InputManager

`Application` owns the input snapshot and passes it by const reference to each
`Game::update()`. Query input during updates:

```c++
void update(float deltaTime, const InputManager& input) override
{
    if (input.isKeyDown(Key::D))
    {
        player.moveRight(deltaTime);
    }
}
```

## Button state

- `isKeyDown(key)`: held at the end of event processing.
- `isKeyPressed(key)`: became held during this input frame.
- `isKeyReleased(key)`: became released during this input frame.
- Mouse buttons use `isMouseButtonDown`, `isMouseButtonPressed`, and
  `isMouseButtonReleased` with `MouseButton::Left`, `Right`, `Middle`, or
  `Button4` through `Button8`.

Queries never consume events. A press and release between updates produces both
`pressed` and `released`, while `down` is false. Held state persists across
frames; press/release flags reset at the next frame boundary. OS key repeats do
not generate additional presses. Unknown and out-of-range keys/buttons return
false.

## Mouse, scrolling, and text

- `mousePosition()` is window-local logical position, with positive Y downward.
  Convert through the viewport/camera separately for world coordinates.
- `mouseDelta()` accumulates movement for this frame using the same axes.
  The first sample and the first sample after a focus change establish a baseline.
- `scrollDelta()` accumulates fractional horizontal and vertical scroll offsets;
  positive Y means scroll up.
- `textInput()` returns this frame's typed text as a UTF-8
  `std::string_view`, valid until the next `beginFrame()` call or manager destruction.
- `isFocused()` reports window focus. Focus loss releases held keys and mouse
  buttons. Press, scroll, and text events are ignored while unfocused.

Movement, scroll, and text reset each input frame. Mouse position persists.

### UTF-8 text input

Use text input for a name field, chat or console. Physical key queries handle
gameplay controls and editing commands; text events supply characters after the
platform interprets the keyboard layout and modifiers.

The returned view borrows the manager's `std::string` buffer without copying it.
Append it to an owned string to keep text across frames:

```c++
std::string playerName; // Persistent UI state.

// During update, when this field has UI focus:
playerName.append(input.textInput());

// Or take an owned copy of just this frame's text:
const std::string typedThisFrame{input.textInput()};
```

Multiple events are concatenated in arrival order. ASCII uses one byte per code
point; other code points can use two, three or four UTF-8 bytes. For example,
`A` + `é` + `中` + `😀` produces ten bytes. `size()` counts bytes, and indexing
accesses bytes, not complete Unicode characters. Removing the last byte with
`pop_back()` can split an encoded character; text editing needs Unicode-aware
cursor and deletion logic.

Repeated queries do not consume text. An empty input frame clears the manager's
buffer; copy any text you need before then. The view is length-delimited and may
contain an embedded NUL if one is supplied as a text event. Do not treat its
`data()` as a complete C string or retain it across snapshot changes.

Text events retain their `char32_t` payload internally. While applying a focused
text event, the manager encodes that code point using the shortest
[UTF-8 byte sequence](https://www.rfc-editor.org/rfc/rfc3629#section-3).
Surrogate values (U+D800 through U+DFFF) and values above U+10FFFF are encoded as
U+FFFD, the replacement character (`EF BF BD` in UTF-8), so malformed internal
events do not produce invalid UTF-8. No normalization is performed. Unfocused
text events remain ignored.

## Engine ownership and timing

`Window` owns its private `GLFWInput` collector. The collector reserves the GLFW
window user pointer, installs input callbacks, and detaches them before the
native window is destroyed. Platform callbacks translate GLFW tokens into engine
events and append them to a queue; they do not change the gameplay snapshot.

Each application iteration runs:

1. `window.pollEvents()`
2. `window.updateInput(input)`
3. `game.update(deltaTime, input)`
4. Render and present

`updateInput()` drains the queue and calls `InputManager::beginFrame()`, which
clears transient state and applies events in order. Events received outside
`pollEvents()` remain pending until the next drain. Gameplay should not advance
the snapshot itself.

The collector and snapshot run on the main thread; no locking is required for
this loop. Moving simulation to another thread requires a synchronized event
handoff. If fixed updates are added later, define a tick-level input policy so
edges survive iterations with no ticks and are not repeated across catch-up ticks.

## Internal event flow

The implementation has three responsibilities:

| Component | Owns | Responsibility |
| --- | --- | --- |
| `GLFWInput` | Pending and delivered event buffers | Register native callbacks, translate tokens and record events |
| `Window` | Native window and its collector | Keep callback state alive and drain events at the update boundary |
| `InputManager` | Button states, position, deltas, text and focus | Apply events in order and expose repeatable queries |

The neutral payload types live in `forge2d/input/internal/InputEvents.hpp`.
`KeyEvent` and `MouseButtonEvent` contain engine enums and a down/up value;
cursor events contain absolute positions; scroll events contain relative offsets.
Text events contain individual Unicode code points, and focus events contain the
new window focus state. None of these payloads retains a native window pointer.
TextEvent remains a code point at this boundary; encoding to UTF-8 belongs to
InputManager when it appends to the frame's text buffer.


### Callback registration and lifetime

`GLFWInput(window)` borrows a live native window, reserves two event buffers and
installs six callbacks. Its address is stored in the window user pointer so the
static callback functions can find the correct collector. The input callback
slots and user pointer must remain reserved for that collector; existing handlers
are replaced rather than chained.

Construction queues initial focus followed by initial cursor position. Applying
these events establishes the first snapshot without inventing mouse movement.
`Window` destroys the collector before destroying the native window, allowing its
destructor to detach callbacks and clear the user pointer safely.

Key callbacks accept press/release actions and ignore OS repeat. The translation
functions map supported GLFW tokens to engine enums and reject unknown values.
Other callbacks preserve cursor samples, fractional scroll, text and focus as
events. No callback invokes gameplay or writes the current gameplay snapshot.

### Queue draining and errors

`enqueue()` appends to `pending_`. It catches allocation exceptions and remembers
the first failure so an exception cannot escape through a GLFW callback.
`takeEvents()` rethrows that failure at the C++ boundary. The failure is sticky:
the queue cannot be treated as complete and gameplay should not continue with it.

On a successful drain, `takeEvents()` swaps `pending_` with `frameEvents_` and
clears the previously delivered batch now held by `pending_`. This reuses buffer
capacity rather than copying the events. Newly arriving events go into the pending
buffer and do not change the batch being consumed.

The returned span borrows `frameEvents_` storage. Consume it before the next drain
or collector destruction. Draining again without new input returns an empty batch;
it does not replay the previous events. Do not clear pending events before polling:
callbacks can also occur during other GLFW window operations.

### Applying a snapshot

`beginFrame(events)` runs **before** the gameplay update. It first
clears button edge flags, movement, scrolling and text, then visits each event.
It retains held state, cursor position and focus from the preceding snapshot.

The private `setDown()` helper records transitions without clearing earlier edges:

| Event sequence in one batch | down | pressed | released |
| --- | --- | --- | --- |
| Up to pressed | true | true | false |
| Held, no events | true | false | false |
| Held to released | false | false | true |
| Up to pressed to released | false | true | true |

Duplicate states do not create transitions. The `stateFor()` helper returns an
immutable all-false fallback for Unknown, Count or out-of-range enum values.

Cursor application accumulates differences between samples while focused.
The first sample, and the first sample after a focus change, establish a baseline.
Unfocused samples still update absolute position but do not generate movement.
Focus loss releases all held buttons and clears movement accumulated so far.
It does not erase previously recorded edges, scroll or text; those reset at the
next snapshot boundary. Later presses, scrolling and text are ignored while
unfocused, while releases remain accepted.

### Fixed-update integration

The current application loop performs one variable-duration update per rendered
frame. If it is changed to fixed updates, poll once per outer iteration and call
`window.updateInput(input)` immediately before **each simulation tick**.

- With no ticks, leave events queued until a tick runs.
- The first tick receives pending presses, releases and relative input.
- Further catch-up ticks drain empty batches, clearing transients while retaining
  held state and absolute position.

This assigns pending events to the next available tick. Events do not carry
timestamps, and boolean edge flags record at least one transition rather than
counting every tap. Precise replay or historical tick assignment would require
additional event timing data. A separate per-render-frame UI snapshot would also
need its own delivery policy; gameplay transients should not be reused as UI events.
