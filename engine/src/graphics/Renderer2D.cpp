#include <iostream>
#include <ranges>

#include <glad/gl.h>

#include <forge2d/graphics/Renderer2D.hpp>
#include <forge2d/graphics/Texture2D.hpp>

#include <opengl/Shader.hpp>
#include <opengl/array/VertexArray.hpp>
#include <opengl/buffer/Buffer.hpp>


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

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    Renderer2D::~Renderer2D() = default;

    void Renderer2D::initTriangle()
    {
        // Vertex Data
        constexpr float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        impl_->triangleVBO = internal::opengl::Buffer::createVertexBuffer(vertices, sizeof(vertices));

        constexpr int stride = 3 * sizeof(float);
        std::vector<internal::opengl::VertexAttribute> layout = {
            { 0, 3, GL_FLOAT, GL_FALSE, stride, 0 }
        };

        impl_->triangleVAO.addVertexBuffer(impl_->triangleVBO, layout);
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
            { 0, 3, GL_FLOAT, GL_FALSE, stride, 0 },
            { 1, 2, GL_FLOAT, GL_FALSE, stride, 3 * sizeof(float) },
        };

        impl_->quadVAO.addVertexBuffer(impl_->quadVBO, layout);
        impl_->quadVAO.setIndexBuffer(impl_->quadEBO);
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

    void Renderer2D::drawTriangle(math::Transform2D transform, const Colour colour) const
    {
        impl_->shapeShader->bind();

        impl_->shapeShader->setVec4("uColour", colour.toVec4());
        impl_->shapeShader->setMat4("uModel", transform.matrix());
        impl_->shapeShader->setMat4("uProjection", impl_->projection);

        impl_->triangleVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer2D::drawRect(math::Transform2D transform, const Colour colour) const
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
        impl_->spriteShader->bind();

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

    }

    void Renderer2D::drawCircle(math::Vec2 center, float radius, const Colour colour) const
    {

    }
}
