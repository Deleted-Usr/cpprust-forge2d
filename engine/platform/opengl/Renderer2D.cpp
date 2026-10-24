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
        shapeShader_ = std::make_unique<Shader>(
            shaderDirectory / "shape.vert.glsl",
            shaderDirectory / "shape.frag.glsl"
        );

        initTriangle();
        initQuad();

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void Renderer2D::initTriangle()
    {
        // Vertex Data
        constexpr float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        // Vertex Array Objects
        glGenVertexArrays(1, &triangleVAO_);
        glBindVertexArray(triangleVAO_);

        // Vertex Buffer Objects
        glGenBuffers(1, &triangleVBO_);
        glBindBuffer(GL_ARRAY_BUFFER, triangleVBO_);

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

    void Renderer2D::initQuad()
    {
        constexpr float vertices[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
           -0.5f, -0.5f, 0.0f,  // bottom left
           -0.5f,  0.5f, 0.0f   // top left
        };

        constexpr unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &quadVAO_);
        glBindVertexArray(quadVAO_);

        glGenBuffers(1, &quadVBO_);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);

        glGenBuffers(1, &quadEBO_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO_);

        glBufferData( // For VBO
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        glBufferData( // For EBO
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(indices),
            indices,
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(float),
            nullptr
        );
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    Renderer2D::~Renderer2D()
    {
        // Triangles
        if (triangleVBO_ != 0)
        {
            glDeleteBuffers(1, &triangleVBO_);
        }

        if (triangleVAO_ != 0)
        {
            glDeleteVertexArrays(1, &triangleVAO_);
        }

        // Quads
        if (quadVBO_ != 0)
        {
            glDeleteBuffers(1, &quadVBO_);
        }

        if (quadVAO_ != 0)
        {
            glDeleteVertexArrays(1, &quadVAO_);
        }

        if (quadEBO_ != 0)
        {
            glDeleteBuffers(1, &quadEBO_);
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
        shapeShader_->bind();

        glBindVertexArray(triangleVAO_);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Renderer2D::drawQuad()
    {
        shapeShader_->bind();

        glBindVertexArray(quadVAO_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    void Renderer2D::endFrame()
    {
        // Nothing yet.
    }
}
