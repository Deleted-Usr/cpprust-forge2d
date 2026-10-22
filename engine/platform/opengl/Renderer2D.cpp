#include <forge2d/graphics/Renderer2D.hpp>

#include "Shader.hpp"

#include <glad/gl.h>

namespace
{

}

namespace forge
{
    Renderer2D::Renderer2D(const std::filesystem::path& shaderDirectory)
    {
        triangleShader_ = std::make_unique<Shader>(
            shaderDirectory / "triangle.vert.glsl",
            shaderDirectory / "triangle.frag.glsl"
        );

        // Vertex Data
        constexpr float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        // Vertex Array Objects
        glGenVertexArrays(1, &triangleVao_);
        glBindVertexArray(triangleVao_);

        // Vertex Buffer Objects
        glGenBuffers(1, &triangleVbo_);
        glBindBuffer(GL_ARRAY_BUFFER, triangleVbo_);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        // Vertex Layout
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(float),
            nullptr
        );
        glEnableVertexAttribArray(0);

        // Unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Renderer2D::~Renderer2D()
    {
        if (triangleVbo_ != 0)
        {
            glDeleteBuffers(1, &triangleVbo_);
        }

        if (triangleVao_ != 0)
        {
            glDeleteVertexArrays(1, &triangleVao_);
        }
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

    void Renderer2D::drawTriangle()
    {
        triangleShader_->bind();

        glBindVertexArray(triangleVao_);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer2D::endFrame()
    {
        // Nothing yet.
    }
}
