#pragma once

#include <filesystem>
#include <memory>

#include <opengl/array/VertexArray.hpp>
#include <opengl/shaders/Shader.hpp>

namespace forge::internal
{
    class  GPUTexture;
    struct RenderView2D;
    struct RenderBatch2D;
}

namespace forge::internal::opengl
{
    /// Owns all GL state and resources needed to submit ordered 2D batches.
    class OpenGLRenderer2D
    {
        public:
            static constexpr std::size_t MaxVertices = 16'384;
            static constexpr std::size_t MaxIndices  = 24'576;
            static constexpr std::size_t MaxTextures = 8;

            explicit OpenGLRenderer2D(const std::filesystem::path& shaderDirectory);

            void beginFrame (int framebufferWidth,       int framebufferHeight, Colour clearColour);
            void submit     (const RenderBatch2D& batch, const RenderView2D& view);
            void endFrame   ();

            [[nodiscard]] std::shared_ptr<const GPUTexture> whiteTexture() const { return whiteTexture_; }

        private:
            Shader shader_;

            Buffer vertexBuffer_;
            Buffer indexBuffer_;

            VertexArray vertexArray_;

            static constexpr GLuint viewBinding  = 0;
            Buffer viewBuffer_;

            std::shared_ptr<const GPUTexture> whiteTexture_;
    };
}
