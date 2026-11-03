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

Obviously, this would be abstracted further, but that is what the raw OpenGL calls should look like. The Renderer would
also be tightly integrated with the Engine Resource Manager, as the manager would hold the raw float arrays containing
the vertex data (`constexpr float[] vertices { ... }`)

TODO - Write mermaid diagram for Renderer refactor :) **THIS DOCUMENT IS NOT FINISHED**

[Hazel - Renderer2D.cpp]: https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Renderer/Renderer2D.cpp
[raylib — rlgl.h]: https://github.com/raysan5/raylib/blob/master/src/rlgl.h
[libGDX — SpriteBatch.java]: https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/graphics/g2d/SpriteBatch.java
[Dear ImGui - OpenGL Backend]: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl3.cpp
[LearnOpenGL - Rendering Sprites]: https://learnopengl.com/In-Practice/2D-Game/Rendering-Sprites
