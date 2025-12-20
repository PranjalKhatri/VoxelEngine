#include "vertex_buffers.hpp"
#include <iostream>
#include "glad/glad.h"

using namespace pop::gfx;

GLBuffer::GLBuffer(BufferType target, bool lazy)
    : buffer_id_{0}, target_{target} {
    if (!lazy) glGenBuffers(1, &buffer_id_);
}

GLBuffer::~GLBuffer() {
    if (buffer_id_ != 0) {
        glDeleteBuffers(1, &buffer_id_);
    }
}
GLBuffer::GLBuffer(GLBuffer&& other) noexcept
    : buffer_id_(other.buffer_id_), target_(other.target_) {
    other.buffer_id_ = 0;
}
GLBuffer& GLBuffer::operator=(GLBuffer&& other) noexcept {
    if (this != &other) {
        if (buffer_id_ != 0) {
            glDeleteBuffers(1, &buffer_id_);
        }

        buffer_id_ = other.buffer_id_;
        target_    = other.target_;

        other.buffer_id_ = 0;
    }
    return *this;
}

void GLBuffer::Generate() {
    if (!buffer_id_) glGenBuffers(1, &buffer_id_);
}

void GLBuffer::Bind() {
    if (!buffer_id_) Generate();
    glBindBuffer(static_cast<GLenum>(target_), buffer_id_);
}
void GLBuffer::UnBind() { glBindBuffer(static_cast<GLenum>(target_), 0); }

void GLBuffer::BufferData(GLsizeiptr size, const void* data, GLenum usage) {
    // Bind buffer first
    glBindBuffer(static_cast<GLenum>(target_), buffer_id_);
    glBufferData(static_cast<GLenum>(target_), size, data, usage);
}

VertexArray::VertexArray(bool lazy) {
    if (!lazy) {
        glGenVertexArrays(1, &array_id_);
        std::cout << "Vertex array created with id " << array_id_ << "\n";
    }
}

void VertexArray::Generate() {
    if (!array_id_) glGenVertexArrays(1, &array_id_);
    std::cout << "Vertex Array created with id " << array_id_ << "\n";
}

VertexArray::~VertexArray() {
    if (array_id_ != 0) {
        glDeleteVertexArrays(1, &array_id_);
    }
}
VertexArray::VertexArray(VertexArray&& other) noexcept
    : array_id_(other.array_id_) {
    other.array_id_ = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this != &other) {
        if (array_id_ != 0) {
            glDeleteVertexArrays(1, &array_id_);
        }
        array_id_       = other.array_id_;
        other.array_id_ = 0;
    }
    return *this;
}

void VertexArray::Bind() {
    if (!array_id_) Generate();
    glBindVertexArray(array_id_);
}

void VertexArray::UnBind() { glBindVertexArray(0); }

void VertexArray::AddAttribute(Attribute attribute) {
    Bind();
    bool is_integer_type =
        (attribute.type == GLType::kInt || attribute.type == GLType::kUInt ||
         attribute.type == GLType::kShort ||
         attribute.type == GLType::kUShort || attribute.type == GLType::kByte ||
         attribute.type == GLType::kUByte);

    if (is_integer_type) {
        glVertexAttribIPointer(attribute.index, attribute.components,
                               static_cast<GLenum>(attribute.type),
                               attribute.stride,
                               reinterpret_cast<const void*>(attribute.offset));
    } else {
        glVertexAttribPointer(attribute.index, attribute.components,
                              static_cast<GLenum>(attribute.type),
                              attribute.normalized ? GL_TRUE : GL_FALSE,
                              attribute.stride,
                              reinterpret_cast<const void*>(attribute.offset));
    }
    glEnableVertexAttribArray(attribute.index);
}
