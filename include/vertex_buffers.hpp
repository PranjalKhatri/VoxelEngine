#pragma once

#include "gl_types.hpp"
#include "glad/glad.h"
#include <cstddef>

namespace pop::gfx {

class GLBuffer {
   public:
    explicit GLBuffer(BufferType target);
    ~GLBuffer();
    // Returns the buffer id
    GLuint id() const { return buffer_id_; }
    // Binds this buffer object to the specified target
    void Bind();
    // calls glBindBuffer with target and then bufferData
    void BufferData(GLsizeiptr size, const void* data, GLenum usage);

   private:
    GLuint     buffer_id_;
    BufferType target_;
};
struct Attribute {
    GLuint      index;       // attribute location
    GLint       components;  // 1–4
    GLType      type;        // GL_FLOAT, GL_INT, ...
    bool        normalized;  // normalize integer types?
    GLsizei     stride;      // bytes per vertex
    std::size_t offset;      // byte offset in struct
};
class VertexArray {
   public:
    explicit VertexArray();
    ~VertexArray();
    // Returns the shader id
    GLuint id() const { return array_id_; }
    // Binds this vertexArray
    void Bind();
    void UnBind();
    // Adds and enables the attribute
    void AddAttribute(Attribute attribute);

   private:
    GLuint array_id_;
};
}  // namespace pop::gfx
