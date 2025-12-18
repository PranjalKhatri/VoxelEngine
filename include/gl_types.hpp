#pragma once

#include "glad/glad.h"

namespace pop::gfx {

using ShaderHandle = uint32_t;

enum class ShaderType : GLuint {
    Vertex   = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER
};

enum class BufferType : GLuint {
    kNone                    = 0,
    kArrayBuffer             = GL_ARRAY_BUFFER,
    kElementArrayBuffer      = GL_ELEMENT_ARRAY_BUFFER,
    kCopyReadBuffer          = GL_COPY_READ_BUFFER,
    kCopyWriteBuffer         = GL_COPY_WRITE_BUFFER,
    kPixelPackBuffer         = GL_PIXEL_PACK_BUFFER,
    kPixelUnpackBuffer       = GL_PIXEL_UNPACK_BUFFER,
    kUniformBuffer           = GL_UNIFORM_BUFFER,
    kShaderStorageBuffer     = GL_SHADER_STORAGE_BUFFER,
    kAtomicCounterBuffer     = GL_ATOMIC_COUNTER_BUFFER,
    kTransformFeedbackBuffer = GL_TRANSFORM_FEEDBACK_BUFFER,
    kTextureBuffer           = GL_TEXTURE_BUFFER,
    kDrawIndirectBuffer      = GL_DRAW_INDIRECT_BUFFER,
    kDispatchIndirectBuffer  = GL_DISPATCH_INDIRECT_BUFFER,
    kQueryBuffer             = GL_QUERY_BUFFER,
    kParameterBuffer         = GL_PARAMETER_BUFFER
};
enum class GLType : GLenum {
    kByte      = GL_BYTE,
    kUByte     = GL_UNSIGNED_BYTE,
    kShort     = GL_SHORT,
    kUShort    = GL_UNSIGNED_SHORT,
    kInt       = GL_INT,
    kUInt      = GL_UNSIGNED_INT,
    kFloat     = GL_FLOAT,
    kDouble    = GL_DOUBLE,
    kHalfFloat = GL_HALF_FLOAT,
    kFixed     = GL_FIXED
};
enum class TextureFormat {
    kRGB          = GL_RGB,
    kRG           = GL_RG,
    kRGBA         = GL_RGBA,
    kRed          = GL_RED,
    kDepth        = GL_DEPTH,
    kDepthStencil = GL_DEPTH_STENCIL
};

enum class TextureWrap {
    kRepeat         = GL_REPEAT,
    kMirroredRepeat = GL_MIRRORED_REPEAT,
    kClampToEdge    = GL_CLAMP_TO_EDGE,
    kClampToBorder  = GL_CLAMP_TO_BORDER
};
enum class TextureType {
    kTexture1D = GL_TEXTURE_1D,
    kTexture2D = GL_TEXTURE_2D,
    kTexture3D = GL_TEXTURE_3D,
};
enum class TextureFilter { kNearest = GL_NEAREST, kLinear = GL_LINEAR };
}  // namespace pop::gfx
