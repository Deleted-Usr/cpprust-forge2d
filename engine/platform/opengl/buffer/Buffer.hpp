#pragma once

#include <glad/gl.h>

#include "BufferType.hpp"
#include "BufferUsage.hpp"

#include <opengl/GLConversions.hpp>

namespace forge::internal::opengl
{
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

            ~Buffer();

            void bind()   const;
            void unbind() const;

            [[nodiscard]]
            unsigned int id() const { return id_; }

        private:
            Buffer(BufferType type, const void* data, GLsizeiptr size, BufferUsage usage);

            unsigned int id_ = 0;
            BufferType type_ = BufferType::ARRAY_BUFFER;
    };
}
