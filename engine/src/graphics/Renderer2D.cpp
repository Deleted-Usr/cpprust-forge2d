#include <ranges>

#include <glad/gl.h>

#include <opengl/shaders/Shader.hpp>
#include <opengl/array/VertexArray.hpp>
#include <opengl/buffer/Buffer.hpp>

#include <forge2d/debug/Assert.hpp>
#include <forge2d/graphics/Renderer2D.hpp>
#include <forge2d/graphics/Texture2D.hpp>

namespace forge
{
    struct Renderer2D::Impl
    {
        std::unique_ptr<internal::opengl::Shader> shapeShader;
        std::unique_ptr<internal::opengl::Shader> spriteShader;

        internal::opengl::VertexArray triangleVAO;
        internal::opengl::Buffer triangleVBO;

        internal::opengl::VertexArray quadVAO;
        internal::opengl::Buffer quadVBO;
        internal::opengl::Buffer quadEBO;

        internal::opengl::VertexArray lineVAO;
        internal::opengl::Buffer lineVBO;

        math::Mat4 projection = math::Mat4::orthographic(0.0f, 800, 600, 0.0f);
    };

    Renderer2D::Renderer2D(const std::filesystem::path& shaderDirectory) : impl_(std::make_unique<Impl>())
    {
        impl_->shapeShader = std::make_unique<internal::opengl::Shader>(
            shaderDirectory / "shape.vert.glsl",
            shaderDirectory / "shape.frag.glsl"
        );

        impl_->spriteShader = std::make_unique<internal::opengl::Shader>(
            shaderDirectory / "sprite.vert.glsl",
            shaderDirectory / "sprite.frag.glsl"
        );

        initTriangle();
        initQuad();
        initLine();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    Renderer2D::~Renderer2D() = default;

    void Renderer2D::initTriangle()
    {
        // Vertex Data
        constexpr float vertices[] = {
             0.0f,  1.0f, 0.0f, // bottom-left
             1.0f,  1.0f, 0.0f, // bottom-right
             0.5f,  0.0f, 0.0f  // top-middle
        };

        impl_->triangleVBO = internal::opengl::Buffer::createVertexBuffer(vertices, sizeof(vertices));

        constexpr int stride = 3 * sizeof(float);
        std::vector<internal::opengl::VertexAttribute> layout = {
            { 0, 3, GL_FLOAT, GL_FALSE, 0 }
        };

        impl_->triangleVAO.addVertexBuffer(impl_->triangleVBO, stride, layout);
    }

    void Renderer2D::initQuad()
    {
        constexpr float vertices[] = {
            // positions         // texture coords
            0.0f, 0.0f, 0.0f,    0.0f, 0.0f, // top-left
            1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // top-right
            1.0f, 1.0f, 0.0f,    1.0f, 1.0f, // bottom-right
            0.0f, 1.0f, 0.0f,    0.0f, 1.0f  // bottom-left
        };

        constexpr unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        impl_->quadVBO = internal::opengl::Buffer::createVertexBuffer(vertices, sizeof(vertices));
        impl_->quadEBO = internal::opengl::Buffer::createIndexBuffer(indices, sizeof(indices));

        constexpr int stride = 5 * sizeof(float);
        std::vector<internal::opengl::VertexAttribute> layout = {
            { 0, 3, GL_FLOAT, GL_FALSE, 0 },
            { 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float) },
        };

        impl_->quadVAO.addVertexBuffer(impl_->quadVBO, stride, layout);
        impl_->quadVAO.setIndexBuffer(impl_->quadEBO);
    }

    void Renderer2D::initLine()
    {
        constexpr float vertices[] = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        };

        impl_->lineVBO = internal::opengl::Buffer::createVertexBuffer(vertices, sizeof(vertices));

        constexpr int stride = 3 * sizeof(float);
        std::vector<internal::opengl::VertexAttribute> layout = {
            {0, 3, GL_FLOAT, GL_FALSE, 0 }
        };

        impl_->lineVAO.addVertexBuffer(impl_->lineVBO, stride, layout);
    }

    void Renderer2D::beginFrame(const Colour& clearColor)
    {
        glClearColor(
            clearColor.r,
            clearColor.g,
            clearColor.b,
            clearColor.a
        );

        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer2D::endFrame()
    {
        // Nothing yet.
    }

    void Renderer2D::drawTriangle(math::Transform2D transform, const Colour colour, bool fill) const
    {
        impl_->shapeShader->bind();

        impl_->shapeShader->setVec4("uColour", colour.toVec4());
        impl_->shapeShader->setMat4("uModel", transform.matrix());
        impl_->shapeShader->setMat4("uProjection", impl_->projection);

        impl_->triangleVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer2D::drawRect(math::Transform2D transform, const Colour colour, bool fill) const
    {
        impl_->shapeShader->bind();

        impl_->shapeShader->setVec4("uColour", colour.toVec4());
        impl_->shapeShader->setMat4("uModel", transform.matrix());
        impl_->shapeShader->setMat4("uProjection", impl_->projection);

        impl_->quadVAO.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void Renderer2D::drawSprite(const Texture2D& texture, math::Transform2D transform, Colour tint) const
    {
        if (!FORGE_ENSURE(texture.valid(), "drawSprite called with an empty Texture2D")) return;

        impl_->spriteShader->bind();

        // TODO - Texture will auto-generate a texture ID for use within sprite batching

        impl_->spriteShader->setMat4("uModel", transform.matrix());
        impl_->spriteShader->setMat4("uProjection", impl_->projection);
        impl_->spriteShader->setVec4("uTint", tint.toVec4());
        impl_->spriteShader->setInt("uTexture", 0);

        // TODO - Loop through a set number of textures.
        //  Take in an std::vector<Texture2D> and loop through

        texture.bind(0);

        impl_->quadVAO.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void Renderer2D::drawLine(math::Vec2 from, math::Vec2 to, const Colour colour) const
    {
        const auto delta = to - from;
        if (delta.length() == 0.0f) return;

        math::Transform2D transform;
        transform.position = from;
        transform.rotation = std::atan2(delta.y, delta.x);
        transform.scale = { delta.length(), 1.0f };

        impl_->shapeShader->bind();

        impl_->shapeShader->setVec4("uColour", colour.toVec4());
        impl_->shapeShader->setMat4("uModel", transform.matrix());
        impl_->shapeShader->setMat4("uProjection", impl_->projection);

        impl_->lineVAO.bind();
        glDrawArrays(GL_LINES, 0, 2);
    }

    void Renderer2D::drawCircle(math::Vec2 center, float radius, const Colour colour, bool fill) const
    {

    }
}
