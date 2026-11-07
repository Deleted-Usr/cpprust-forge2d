#pragma once

#include <span>

#include <glad/gl.h>

namespace forge::internal::opengl
{
    enum class BufferType
    {
        ARRAY_BUFFER,
        ELEMENT_BUFFER,
        UNIFORM_BUFFER,
    };

    enum class BufferUsage
    {
        STATIC,
        DYNAMIC,
        STREAM,
    };

    class Buffer
    {
        public:
            Buffer() noexcept = default;

            Buffer(const Buffer&) = delete;
            Buffer& operator=(const Buffer&) = delete;

            // Allow moving resources
            Buffer(Buffer&& other) noexcept;
            Buffer& operator=(Buffer&& other) noexcept;

            // Factory methods
            static Buffer createVertexBuffer(const void* data, GLsizeiptr size, BufferUsage usage = BufferUsage::STATIC);
            static Buffer createIndexBuffer(const void* data,  GLsizeiptr size, BufferUsage usage = BufferUsage::STATIC);

            /// Allocates `size` bytes with no initial data, for per-frame updates
            static Buffer createDynamicBuffer(BufferType type, GLsizeiptr size);

            ~Buffer();

            void bind()   const;
            void unbind() const;

            void bindBase(GLuint bindingPoint) const;
            void updateDynamicBuffer(std::span<const std::byte> bytes, int offset = 0);

            [[nodiscard]]
            unsigned int id() const { return id_; }

        private:
            Buffer(BufferType type, const void* data, GLsizeiptr size, BufferUsage usage);

            unsigned int id_ = 0;
            BufferType type_ = BufferType::ARRAY_BUFFER;

            GLsizeiptr capacity_ = 0;
    };
}
