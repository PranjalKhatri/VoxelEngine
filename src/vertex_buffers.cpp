#include "vertex_buffers.hpp"

using namespace pop::gfx;

GLBuffer::GLBuffer(BufferType target) : buffer_id_{0}, target_{target} {
    glGenBuffers(1, &buffer_id_);
}

GLBuffer::~GLBuffer() {
    if (buffer_id_ != 0) {
        glDeleteBuffers(1, &buffer_id_);
    }
}

void GLBuffer::Bind() {
    glBindBuffer(static_cast<GLenum>(target_), buffer_id_);
}

void GLBuffer::BufferData(GLsizeiptr size, const void* data, GLenum usage) {
    // Bind buffer first
    glBindBuffer(static_cast<GLenum>(target_), buffer_id_);
    glBufferData(static_cast<GLenum>(target_), size, data, usage);
}

VertexArray::VertexArray() { glGenVertexArrays(1, &array_id_); }

VertexArray::~VertexArray() {
    if (array_id_ != 0) {
        glDeleteVertexArrays(1, &array_id_);
    }
}

void VertexArray::Bind() { glBindVertexArray(array_id_); }

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
