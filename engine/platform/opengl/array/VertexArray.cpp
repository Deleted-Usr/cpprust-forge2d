#include <opengl/array/VertexArray.hpp>

#include <opengl/buffer/Buffer.hpp>

namespace forge::internal::opengl
{
    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &id_);
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

    void VertexArray::addVertexBuffer(const Buffer& vbo, const std::vector<VertexAttribute>& attributes)
    {
        glBindVertexArray(id_);
        vbo.bind();

        for (const auto& attr : attributes)
        {
            glVertexAttribPointer(
                attr.index,
                attr.size,
                attr.type,
                attr.normalised,
                attr.stride,
                reinterpret_cast<void*>(attr.offset)
            );
            glEnableVertexAttribArray(attr.index);
        }

        glBindVertexArray(0);
        vbo.unbind();
    }

    void VertexArray::setIndexBuffer(const Buffer& ebo)
    {
        glBindVertexArray(id_);
        ebo.bind(); // When bound inside a VAO bind session, the EBO is saved to the VAO state

        unbind();
    }
}
