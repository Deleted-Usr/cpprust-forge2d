#include <opengl/array/VertexArray.hpp>

#include <opengl/buffer/Buffer.hpp>

namespace forge::internal::opengl
{
    VertexArray::VertexArray()
    {
        glCreateVertexArrays(1, &id_);
    }

    VertexArray::~VertexArray()
    {
        if (id_ != 0)
        {
            glDeleteVertexArrays(1, &id_);
        }
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept : id_(other.id_)
    {
        other.id_ = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
    {
        if (this != &other)
        {
            glDeleteVertexArrays(1, &id_);

            id_       = other.id_;
            other.id_ = 0;
        }

        return *this;
    }

    void VertexArray::bind() const
    {
        glBindVertexArray(id_);
    }

    void VertexArray::unbind()
    {
        glBindVertexArray(0);
    }

    void VertexArray::addVertexBuffer(const Buffer& vbo, GLsizei stride, const std::vector<VertexAttribute>& attributes, GLuint bindingIndex)
    {
        // Attach this VBO to a vertex-buffer binding slot.
        glVertexArrayVertexBuffer(
            id_,
            bindingIndex,
            vbo.id(),
            0,                  // Starting byte offset in the VBO
            stride
        );

        for (const auto& attr : attributes)
        {
            // Describe how to interpret this attribute.
            glVertexArrayAttribFormat(
                id_,
                attr.index,
                attr.size,
                attr.type,
                attr.normalised,
                attr.offset
            );

            // Make this attribute read from the selected VBO binding.
            glVertexArrayAttribBinding(
                id_,
                attr.index,
                bindingIndex
            );

            glEnableVertexArrayAttrib(id_, attr.index);
        }
    }

    void VertexArray::setIndexBuffer(const Buffer& ebo)
    {
        glVertexArrayElementBuffer(id_, ebo.id());
    }
}
