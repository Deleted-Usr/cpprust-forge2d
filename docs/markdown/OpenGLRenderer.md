(3 - 11 - 2026)

## OpenGL Renderer2D

Right now, the current state of the renderer is extremely unconventional and messy. 

`Renderer2D` currently does two jobs. It decides what to draw (shapes, sprites, transforms), and it also 
speaks OpenGL (VAOs,`glDrawElements`, `glPolygonMode`, uniforms). That's why GL calls feel all over the 
place.

Every draw call is its own direct GPU draw. Each `drawRect`/`drawSprite` rebinds a shader, does 3
`glGetUniformLocation` lookups and issues one draw. A tilemap of 2,000 tiles means 2,000 draw calls
and 6,000 uniform lookups.

The projection is hard-coded to 800x600. It ignores the window size, resizes and any camera object.

And lastly, the draw methods are all `const` even though they change GPU state, and once batching is
introduced, they will change object state too. This is easily fixed by removing the `const` from each
method.

Structurally, this is ok for getting something drawn on screen quickly and learning raw OpenGL, but when 
looking at it from a production standpoint it needs to change.

For `Renderer2D`, some good sources to take inspiration from are: Hazel, raylib, and libGDX. Each is useful
for a different part of the design.

- [Hazel - Renderer2D.cpp]
  The closest match to what is being built: a C++ renderer with quad batching, texture slots, circles, lines,
  and rendering statistics. Study StartBatch, NextBatch, and Flush to understand how individual drawing calls
  become larger GPU submissions. Look at Borrowing its batching ideas while choosing ownership and API conventions
  that suit Forge2D.

- [raylib — rlgl.h]
  Useful for understanding how simple drawing operations feed a buffered OpenGL implementation. Pay attention 
  to render batches, draw records, texture changes, and when buffered work must be submitted. Main takeaway would 
  be keeping the public drawing API simple while handling GPU details underneath.

- [libGDX — SpriteBatch.java]
  Particularly readable given my Java experience. Study begin, end, draw, flush, and switchTexture, plus what 
  happens when shaders, blending, or projection matrices change. It’s a strong reference for making the renderer’s 
  behavior predictable.

- [Dear ImGui - OpenGL Backend]
  Useful for backend engineering: uploading vertex/index data, applying clipping rectangles, handling framebuffer 
  scaling, and managing OpenGL state. Study this when integrating the renderer with editor UI or other rendering 
  systems.

- [LearnOpenGL - Rendering Sprites]
  A clear explanation of the underlying sprite pipeline: quad geometry, orthographic projection, transforms, shaders, 
  and textures. Best for understanding the fundamentals before adding batching. (**has been heavily referenced all the
  way through development**)

### Shape Batching

The first big architectural improvement the Renderer needs is shape or sprite batching. Shape or sprite batching in
OpenGL is the process of grouping multiple shapes or sprites into a single large vertex buffer and flushing that buffer
using a single draw call. This removes the need to call `glDrawArrays` or `glDrawElements` separately for every single
object. 

This greatly reduces CPU-to-GPU overhead, as each draw call indroduces driver hardware overhead, grouping objectscuts 
thousands of draw calls down to just one. It improves frame rates by minimising state changes and draw calls bottlenecks.
And it optimises memory bandwidth by sending one large chunk of data via a single buffer update, which is obviously
much faster than many small scattered transfers.

#### How it works

Shape batching can be done in various ways, but the most prominent way sprite batching is done uses 4 general steps:
1. **Allocate a Shared Buffer**: Create a fixed-size Vertex Buffer Object (VBO) on the GPU using `GL_DYNAMIC_DRAW`
   since geometry updates every frame.

2. **Pack Vertex Data**: For each shape, calculate its vertex positions, colour, texture coords, and texture slot
   index. Write them into a CPU-side array.

3. **Upload Once Per Frame**: Use `glBufferSubData` to copy the packed CPU array into the GPU buffer in one go.

4. **Issue a Single Draw Call**: Bind the Vertex Array Object (VAO) and call `glDrawArrays` or `glDrawElements`
   once for the entire batch.
   
A very basic and bare-bones implementation might look like this:
```c++
// 1. Setup a dynamic VBO once during initialization
GLuint vao, vbo;
glGenVertexArrays(1, &vao);
glGenBuffers(1, &vbo);

glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
// Allocate max size buffer (e.g., 1000 sprites * 6 vertices * sizeof(Vertex))
glBufferData(GL_ARRAY_BUFFER, maxBufferSize, nullptr, GL_DYNAMIC_DRAW);

// Configure vertex attributes (position, texCoords, texIndex, etc.)
// ... setup glVertexAttribPointer calls ...

// 2. Inside your render loop / per frame:
Vertex* bufferPtr = cpuVertexArray; // Fill this array with your shapes' vertices
int vertexCount = populateSpriteBatch(cpuVertexArray); 

glBindBuffer(GL_ARRAY_BUFFER, vbo);
// Overwrite or update sub-region of the buffer
glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), cpuVertexArray);

// 3. Single draw call for all packed shapes!
glBindVertexArray(vao);
glDrawArrays(GL_TRIANGLES, 0, vertexCount);
```

Obviously this would be abstracted further, but that is what the raw OpenGL calls should look like. 

Each of the 5 sources above also define something along the lines of a vertex struct that hold position, colour, and
texture coordinate positions, and an integer for a texture index. When vertices are submitted to the batcher, they're
submitted as an instantiation of the vertex struct: `vertices_.push_back({ position, texCoords, textureIndex });`. For
Forge2D however, colour coordinates will be discarded for now in favour of setting colours directly in the shape's
fragment shader.

They also pre-generate indices for the index buffer using a process along the lines of a large reserved `std::vector`,
an integer for offset counting, and iterating through the vector to set indices.
```c++
std::vector<GLuint> indices(MAX_INDEX_COUNT);
uint32_t offset = 0;
for (size_t i = 0; i < MAX_INDEX_COUNT; i += 6)
{
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    ...
    
    offset += 4;
}
```

At the beginning of the frame, a `begin()` method gets called that clears the vertices array and resets the texture 
slot index to 0. Then at the end of the frame, or when the vertex batch gets full, a `flush()` usually gets called in 
where `GL_ARRAY_BUFFER` gets bound and `glDrawElements` gets called, drawing shapes and sprites to the display.

That is the general gist of an OpenGL sprite batcher, though since Forge is likely to introduce Vulkan as a potential
rendering backend, the batcher should be kept library agnostic, as to avoid directly needing OpenGL or Vulkan function
calls and types. Then again, two implementations of sprite batching would most likely be easier to add and maintain
across the two backends. Below is a proposed class (`.hpp` only) for the Shape Batching system. 

This may be extremely different from what actually gets implemented in Forge.

Proposed Class `.hpp` only (example):
```c++
namespace forge::internal
{
    struct Vertex
    {
        math::Vec2 position;
        math::Vec2 texCoords;
        float texIndex;
    };
    
    class VertexBatcher
    {
        public:
            VertexBatcher();
            ~VertexBatcher() = default;
        
            void submitQuad();
            void submitTriangle();
            void submitCircle();
            void submitLine();
        
        private:
            void begin();
            void flush();
        
            static const std::size_t MAX_VERTEX_COUNT;
            static const std::size_t MAX_INDEX_COUNT;
        
            opengl::VertexArray VAO;
            opengl::Buffer VBO;
            opengl::Buffer EBO;
    };
}
```

### Uniform Buffer Objects and Uniform Caching 

Now that the per-shape VBO, EBO, and draw call problem has now been resolved, the obvious next thing to look 
at is the three or four individual shader lookups occurring in each draw method. This can be resolved in 2 ways:
**Uniform Buffer Objects** and **Uniform Caching**. They solve two major bottlenecks in OpenGL:

#### Uniform Buffer Objects
A Uniform Buffer Object, or UBO, is an advanced OpenGL feature that allows a collection of global uniform data to be 
stored in a single GPU memory buffer. Unlike standard uniforms, which must be updated independently per shader program 
via `glUniform*` and `glGetUniformLocation` functions, UBOs enable you to share the exact same uniform data across 
multiple shader programs and update them in a single operation, significantly slashing CPU overhead and reducing API 
driver overhead.

To bridge the gap between shader-specific code and independent memory buffers, OpenGL uses a decoubled **Binding 
Point System**:

1. **Uniform Block (Shader)**: Inside a GLSL shader, related uniforms get grouped into a block. This block is
   assigned a Block Index by the shader program.
   
2. **Uniform Buffer Object (C++ App)**: This is the physical memory allocated on the GPU containing the byte 
   data.

3. **Binding Points (OpenGL Context)**: The context maintains an array of slot bridges (`GL_MAX_UNIFORM_BUFFER_BINDINGS`).
   The shader's block index gets linked to a specific slot bridge, and then the UBO gets bound to that same slot bridge.

A basic implementation of this is fairly straight-forward:

```glsl
// Vertex Shader

#version 460 core
layout (location = 0) in vec3 aPos;

// the layout(std140) qualifier guarantees standardised memory alignment
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

```c++
// Get the uniform block index inside the specific shader program
GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, "Matrices");

// Link this specific shader's block index to Context Binding Point 0
unsigned int bindingPoint = 0;
glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);

// Create and allocate the UBO
GLuint ubo;
glGenBuffers(1, &ubo);
glGeBindBuffer(GL_UNIFORM_BUFFER, ubo);

// Allocate space for the two matrices (64 bytes each = 128 bytes total)
// GL_DYNAMIC_DRAW tells the GPU that this data will be updated frequently
glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
glBindBuffer(GL_UNIFORM_BUFFER, 0);

// Link the physical buffer to the exact same global context binding point
glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
```

```c++
// Updating Data Dynamically (In the Render Loop)

mat4 projectionMatrix = mat4::perspective(...);
mat4 viewMatrix       = mat4::lookAt(...);

glBindBuffer(GL_UNIFORM_BUFFER, ubo);

// Overwrite the first 64 bytes with the projection matrix
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), &projectionMatrix[0][0]);

// Overwrite the next 64 bytes with the view matrix
glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), &viewMatrix[0][0]);

glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

The example above assumes a three-dimensional world space, but the general implementation shouldn't be very different 
for a two-dimensional space. As well, it is not using OpenGL's Direct State Access, of which Forge uses.

### Hard-Coded Projection Matrix

Right now, the projection matrix for `Renderer2D` is hardcoded as 800x600 pixels. This is very obviously not how an
engine should work. An engine should take the window configurations and pass them into the renderer so OpenGL knows
how to transform the objects it draws.

The projection is currently stored inside `Renderer2D::Impl`:

```c++
math::Mat4 projection = math::Mat4::orthographic(0.0f, 800, 600, 0.0f);
```

An orthographic projection maps a rectangular area of the scene into OpenGL's clip space without making
objects smaller as they move further away. With these bounds, `(0, 0)` is the top-left of the visible area
and `(800, 600)` is the bottom-right. Positive X moves right and positive Y moves down, which suits the
current shape and sprite coordinates. The default near and far planes are `-1.0f` and `1.0f`.

The issue is that those bounds never change. `OpenGLContext::initialise()` already sets the viewport from
the framebuffer size and installs a framebuffer resize callback, but that callback only calls `glViewport`.
The viewport decides which framebuffer pixels receive the rendered image; the projection decides which
scene coordinates are visible. Updating one does not update the other.

For example, displaying the same 800x600 area across a 1600x900 framebuffer scales X by 2 and Y by 1.5.
A square becomes a rectangle. Keeping a fixed logical resolution can be intentional, but it needs an
explicit scaling policy, such as letterboxing, to preserve the aspect ratio.

#### Make the Visible Area Explicit

A simple first step would be to let the application supply the projection dimensions rather than choosing
them inside the renderer. For a pixel-based coordinate system, rebuilding the projection from the framebuffer
dimensions makes one scene unit correspond to one framebuffer pixel. Resizing then reveals more or less of
the scene instead of stretching it.

A proposed method could look like this; it is not part of the current `Renderer2D` API:

```c++
void Renderer2D::setProjectionSize(int width, int height)
{
    // A minimised window can have a zero-sized framebuffer.
    if (width <= 0 || height <= 0) return;

    impl_->projection = math::Mat4::orthographic(
        0.0f,
        static_cast<float>(width),
        static_cast<float>(height),
        0.0f
    );
}
```

This should be called with the actual framebuffer dimensions during initialisation and whenever they change.
The platform layer should pass resize information through the window/application layer so that the OpenGL
context does not need to own or know about `Renderer2D`. Extend the existing callback or forward its event;
installing another GLFW framebuffer callback would replace the existing one.

Window size and framebuffer size also need to be treated separately. On a high-DPI display, a window's
logical dimensions may differ from its framebuffer dimensions. `glViewport` uses framebuffer pixels. If the
desired drawing coordinates use logical window units instead, build the projection from those logical
dimensions while keeping the viewport in framebuffer pixels. The important part is choosing the coordinate
system deliberately and updating both sizes when needed. Skip rendering while the framebuffer has zero
width or height, rather than constructing an invalid projection or dividing by zero for an aspect ratio.

#### Move Towards a Camera

Once the game needs scrolling, zooming, or multiple views, the visible bounds belong in a camera. The
camera should provide a projection matrix and a view matrix, while the renderer consumes those matrices
at the start of a scene or render pass. This keeps camera behaviour independent of OpenGL.

The vertex transformation would then become:

```glsl
gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
```

The model matrix places an object in the world. The view matrix applies the inverse of the camera's
transform, so moving the camera right makes the scene move left. The projection maps that camera-space
area into clip space. For screen-space UI, an identity view matrix and a separate projection allow the UI
to stay fixed while the world camera moves.

For a world camera, a useful resize policy is to keep the visible height constant and calculate the visible
width as `visibleHeight * framebufferAspectRatio`. Another valid policy is to keep an 800x600 virtual canvas
and fit a centred 4:3 viewport inside the framebuffer, leaving bars around it where necessary. These policies
serve different purposes, so they should be selected by the application or camera rather than hidden in a
renderer constant.

This also fits the earlier batching and UBO design. Supply the camera matrices once per scene or pass and
share them between the shape and sprite shaders. If the camera or projection changes while geometry is
queued, flush that geometry using the old matrices before uploading the new ones. Otherwise, earlier shapes
could be drawn with the next camera's view. With batching, model transforms can be applied while packing
vertices, leaving the shared camera matrices to be applied by the vertex shader.

To check the eventual implementation, draw a square near each corner and resize between wide and tall
windows. Confirm that the chosen policy preserves square proportions, that the visible bounds match the
policy, and that minimising and restoring the window works. Camera movement should affect world objects
while leaving a separate UI pass fixed, and high-DPI rendering should use the intended coordinate scale.

[Hazel - Renderer2D.cpp]: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Renderer/Renderer2D.cpp
[raylib — rlgl.h]: https://github.com/raysan5/raylib/blob/master/src/rlgl.h
[libGDX — SpriteBatch.java]: https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/g2d/SpriteBatch.java
[Dear ImGui - OpenGL Backend]: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl3.cpp
[LearnOpenGL - Rendering Sprites]: https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites
