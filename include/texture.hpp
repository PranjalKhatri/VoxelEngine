#pragma once

#include "gl_types.hpp"
#include "glad/glad.h"
#include <string_view>

namespace pop::gfx {

TextureFormat GetFormatFromChannels(int num_channels);

class Texture {
   public:
    explicit Texture(TextureType type);
    ~Texture();

    Texture(const Texture&)            = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // loads the texture from the given file using stb_image lib
    bool LoadFromFile(std::string_view path, bool flip_vertically);

    void Bind(GLenum unit = 0) const;
    void Unbind();

    TextureHandle GetHandle() const { return tex_id_; }

   private:
    GLuint         tex_id_{};
    int            width_{}, height_{}, nchannels_{};
    unsigned char* data_{};
    TextureType    tex_type_;
};

}  // namespace pop::gfx
