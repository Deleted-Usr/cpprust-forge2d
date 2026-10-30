#include <iostream>
#include <ranges>

#include <glad/gl.h>

#include <forge2d/graphics/Renderer2D.hpp>

#include <opengl/Shader.hpp>
#include <opengl/array/VertexArray.hpp>
#include <opengl/buffer/Buffer.hpp>

#include <stb/stb_image.cpp>



namespace forge
{
    struct Renderer2D::Impl
    {
        std::unique_ptr<Shader> shapeShader;

        internal::opengl::VertexArray triangleVAO;
        internal::opengl::Buffer triangleVBO;

        internal::opengl::VertexArray quadVAO;
        internal::opengl::Buffer quadVBO;
        internal::opengl::Buffer quadEBO;

        unsigned int texture1;
        unsigned int texture2;
    };

    Renderer2D::Renderer2D(const std::filesystem::path& shaderDirectory) : impl_(std::make_unique<Impl>())
    {
        impl_->shapeShader = std::make_unique<Shader>(
            shaderDirectory / "shape.vert.glsl",
            shaderDirectory / "shape.frag.glsl"
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
            // positions          // texture coords
            0.5f,  0.5f, 0.0f,    1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f,    1.0f, 0.0f, // bottom right
           -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, // bottom left
           -0.5f,  0.5f, 0.0f,    0.0f, 1.0f  // top left
        };

        constexpr unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        // TODO - should have something similar to std::vector<VertexAttribute> but for texture options



        loadTexture(impl_->texture1, "assets/container.jpg", GL_RGB);
        loadTexture(impl_->texture2, "assets/img.png", GL_RGBA);

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

    void Renderer2D::loadTexture(unsigned int& tex, const std::string& path, int format)
    {
        stbi_set_flip_vertically_on_load(true);

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        stbi_image_free(data);
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

        impl_->triangleVAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer2D::drawRect(math::Transform2D transform, const Colour colour) const
    {
        impl_->shapeShader->bind();

        impl_->shapeShader->setVec4("uColour", colour.toVec4());
        impl_->shapeShader->setMat4("uModel", transform.matrix());
        impl_->shapeShader->setInt("texture1", 0);
        impl_->shapeShader->setInt("texture2", 1);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, impl_->texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, impl_->texture2);

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
