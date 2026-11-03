(4 - 11 - 2026)

## Engine Resource Manager

A resource manager is something Forge2D could benefit from greatly. Forge already has an `AssetManager` for **game** 
content (sprites, tiles, eventually fonts and sounds). The engine itself also depends on content (shaders, the
"missing" texture, vertex positions, eventually a default font), but at the moment that content has no owner. Each
system loads its own, from wherever it can find.

- Shaders are loaded in `Renderer2D` via a shader wrapper class and directly supplied file paths.
- The Asset Manager's missing texture placeholder is generated in `AssetManager.cpp` via a `makeMissingTexture` method
- Shape Vertex data is held and initialised directly within `Renderer2D`.


