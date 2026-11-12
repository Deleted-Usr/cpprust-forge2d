#include <opengl/OpenGLRenderer2D.hpp>

#include <cstddef>
#include <span>
#include <type_traits>
#include <forge2d/assets/Image.hpp>
#include <forge2d/debug/Assert.hpp>
#include <forge2d/graphics/Texture2D.hpp>
#include <graphics/internal/RenderBatch2D.hpp>
#include <graphics/internal/RenderView2D.hpp>
#include <opengl/textures/GPUTexture.hpp>

#ifdef FORGE2D_EMBED_BATCH_SHADERS
#include <BatchShaders.hpp>
#endif

namespace forge::internal::opengl
{
    OpenGLRenderer2D::OpenGLRenderer2D(const std::filesystem::path& shaderDirectory)
    :
      #ifdef FORGE2D_EMBED_BATCH_SHADERS
            shader_ (
                Shader::Source{embedded::BatchVertex, "batch.vert.glsl (embedded)"},
                Shader::Source{embedded::BatchFragment, "batch.frag.glsl (embedded)"}
            ),
      #else
            shader_ (shaderDirectory / "batch.vert.glsl", shaderDirectory / "batch.frag.glsl"),
      #endif
      vertexBuffer_ (Buffer::createDynamicBuffer(BufferType::ARRAY_BUFFER,   MaxVertices * sizeof(Vertex2D))),
      indexBuffer_  (Buffer::createDynamicBuffer(BufferType::ELEMENT_BUFFER, MaxIndices * sizeof(std::uint32_t))),
      whiteTexture_ (std::make_shared<const GPUTexture>(Image{ 1, 1, { 255, 255, 255, 255 } }, TextureOptions{}))
    {
        #ifdef FORGE2D_EMBED_BATCH_SHADERS
                (void)shaderDirectory;
        #endif
        static_assert(std::is_standard_layout_v<Vertex2D>);

        vertexArray_.addVertexBuffer(vertexBuffer_, sizeof(Vertex2D), {
            { 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, position)    },
            { 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, uv)          },
            { 2, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, colour)      },
            { 3, 1, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, textureSlot) }
        });
        vertexArray_.setIndexBuffer(indexBuffer_);

        shader_.bind();

        for (std::size_t i = 0; i < MaxTextures; ++i)
        {
            shader_.setInt("uTextures[" + std::to_string(i) + "]", static_cast<int>(i));
        }
    }

    void OpenGLRenderer2D::beginFrame(int framebufferWidth, int framebufferHeight, Colour clearColour)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, framebufferWidth, framebufferHeight);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_RASTERIZER_DISCARD);
        glDisable(GL_FRAMEBUFFER_SRGB);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLRenderer2D::submit(const RenderBatch2D& batch, const RenderView2D& view)
    {
        if (batch.empty())
        {
            return;
        }

        FORGE_CHECK(
            batch.vertices.size() <= MaxVertices &&
            batch.indices.size()  <= MaxIndices  &&
            batch.textures.size() <= MaxTextures,
            "2D batch exceeds backend capacity"
        );

        vertexBuffer_.updateDynamicBuffer(std::as_bytes(std::span(batch.vertices)));
        indexBuffer_.updateDynamicBuffer(std::as_bytes(std::span(batch.indices)));

        shader_.bind();
        shader_.setMat4("uViewProjection", view.viewProjection);

        for (std::size_t i = 0; i < batch.textures.size(); ++i)
        {
            glBindSampler(static_cast<GLuint>(i), 0);
            batch.textures[i]->bind(static_cast<unsigned int>(i));
        }

        vertexArray_.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batch.indices.size()), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRenderer2D::endFrame()
    {
        // Application presents. No glFlush/glFinish is necessary.
    }
}
