#include <opengl/buffer/Buffer.hpp>

#include <opengl/GLConversions.hpp>

#include "forge2d/debug/Assert.hpp"

namespace forge::internal::opengl
{
    Buffer::Buffer(BufferType type, const void* data, GLsizeiptr size, BufferUsage usage) : type_(type)
    {
        glCreateBuffers(1, &id_);
        glNamedBufferData(id_, size, data, openGLBufferUsage(usage));
    }

    Buffer::Buffer(Buffer&& other) noexcept : id_(other.id_), type_(other.type_), capacity_(other.capacity_)
    {
        other.id_ = 0;
        other.capacity_ = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        if (this != &other)
        {
            glDeleteBuffers(1, &id_);

            type_ = other.type_;
            id_   = other.id_;
            capacity_ = other.capacity_;

            other.id_ = 0;
            other.capacity_ = 0;
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

    Buffer Buffer::createDynamicBuffer(const BufferType type, const GLsizeiptr size)
    {
        Buffer buffer { type, nullptr, size, BufferUsage::DYNAMIC };
        buffer.capacity_ = size;
        return buffer;
    }

    void Buffer::bind() const
    {
        glBindBuffer(openGLBufferType(type_), id_);
    }

    void Buffer::unbind() const
    {
        glBindBuffer(openGLBufferType(type_), 0);
    }

    void Buffer::bindBase(const GLuint bindingPoint) const
    {
        glBindBufferBase(openGLBufferType(type_), bindingPoint, id_);
    }

    void Buffer::updateDynamicBuffer(std::span<const std::byte> bytes, int offset)
    {
        FORGE_CHECK(offset >= 0 && static_cast<GLsizeiptr>(offset) <= capacity_ &&
                    static_cast<GLsizeiptr>(bytes.size()) <= capacity_ - offset, "Buffer::update overflow");

        glInvalidateBufferData(id_);
        glNamedBufferSubData(id_, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(bytes.size()), bytes.data());
    }
}
