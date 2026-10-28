#pragma once

#include <vector>
#include <glad/gl.h>

#include <opengl/buffer/Buffer.hpp>

namespace forge::internal::opengl
{
    struct VertexAttribute
    {
        GLuint index;          // The shader layout location (e.g., layout(location = 0))
        GLint size;            // Number of components (e.g., 3 for RGB/XYZ)
        GLenum type;           // Data type (e.g., GL_FLOAT)
        GLboolean normalised;
        GLsizei stride;        // Total bytes for one complete vertex
        std::size_t offset;    // Byte offset inside the buffer
    };

    class VertexArray
    {
        public:
            VertexArray();
            ~VertexArray();

            VertexArray(const VertexArray&) = delete;
            VertexArray& operator=(const VertexArray&) = delete;
            VertexArray(VertexArray&& other) noexcept;
            VertexArray& operator=(VertexArray&& other) noexcept;

            void bind() const;
            static void unbind();

            /// Links a VBO to this VAO using the specified layout attributes.
            void addVertexBuffer(const Buffer& vbo, const std::vector<VertexAttribute>& attributes);

            /// Links an EBO/IBO to this VAO
            void setIndexBuffer(const Buffer& ebo);

            [[nodiscard]]
            unsigned int id() const { return id_; }

        private:
            unsigned int id_ = 0;
    };
}
