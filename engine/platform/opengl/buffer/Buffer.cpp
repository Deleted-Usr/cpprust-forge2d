#include <opengl/buffer/Buffer.hpp>

namespace forge::internal::opengl
{
    Buffer::Buffer(BufferType type, const void* data, GLsizeiptr size, BufferUsage usage) : type_(type)
    {
        glCreateBuffers(1, &id_);
        glNamedBufferData(id_, size, data, toOpenGLUsage(usage));
    }

    Buffer::Buffer(Buffer&& other) noexcept : id_(other.id_), type_(other.type_)
    {
        other.id_ = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        if (this != &other)
        {
            glDeleteBuffers(1, &id_);

            type_ = other.type_;
            id_   = other.id_;

            other.id_ = 0;
        }

        return *this;
    }

    Buffer::~Buffer()
    {
        if (id_ != 0)
        {
            glDeleteBuffers(1, &id_);
        }
    }

    Buffer Buffer::createVertexBuffer(const void* data, GLsizeiptr size, BufferUsage usage)
    {
        return { BufferType::ARRAY_BUFFER, data, size, usage };
    }

    Buffer Buffer::createIndexBuffer(const void* data, GLsizeiptr size, BufferUsage usage)
    {
        return { BufferType::ELEMENT_BUFFER, data, size, usage };
    }

    void Buffer::bind() const
    {
        glBindBuffer(toOpenGLType(type_), id_);
    }

    void Buffer::unbind() const
    {
        glBindBuffer(toOpenGLType(type_), 0);
    }

    GLenum Buffer::toOpenGLType(BufferType type)
    {
        switch (type)
        {
            case BufferType::ARRAY_BUFFER:   return GL_ARRAY_BUFFER;
            case BufferType::ELEMENT_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
        }

        return GL_NONE;
    }

    GLenum Buffer::toOpenGLUsage(BufferUsage usage)
    {
        switch (usage)
        {
            case BufferUsage::STATIC:  return GL_STATIC_DRAW;
            case BufferUsage::DYNAMIC: return GL_DYNAMIC_DRAW;
            case BufferUsage::STREAM:  return GL_STREAM_DRAW;
        }

        return GL_STATIC_DRAW;
    }
}
