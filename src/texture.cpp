#include "texture.hpp"
#include "glad/glad.h"
#include "stb_image.h"
#include <iostream>

namespace pop::gfx {
TextureFormat GetFormatFromChannels(int ch) {
    switch (ch) {
        case 1:
            return TextureFormat::kRed;
        case 2:
            return TextureFormat::kRG;
        case 3:
            return TextureFormat::kRGB;
        case 4:
            return TextureFormat::kRGBA;
        default:
            return TextureFormat::kRGB;
    }
}
Texture::Texture(TextureType type)
    : width_{0}, height_{0}, nchannels_{0}, data_{nullptr}, tex_type_{type} {
    glGenTextures(1, &tex_id_);
    glBindTexture(static_cast<GLenum>(tex_type_), tex_id_);

    // Default wrapping
    glTexParameteri(static_cast<GLenum>(tex_type_), GL_TEXTURE_WRAP_S,
                    GL_REPEAT);
    glTexParameteri(static_cast<GLenum>(tex_type_), GL_TEXTURE_WRAP_T,
                    GL_REPEAT);

    // Default filtering

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
Texture::~Texture() {
    if (data_) {
        stbi_image_free(data_);
        data_ = nullptr;
    }
    if (tex_id_) glDeleteTextures(1, &tex_id_);
}

Texture::Texture(Texture&& other) noexcept
    : tex_id_{other.tex_id_},
      width_{other.width_},
      height_{other.height_},
      nchannels_{other.nchannels_},
      data_{other.data_},
      tex_type_{other.tex_type_} {
    other.tex_id_ = 0;
    other.data_   = nullptr;
    other.width_ = other.height_ = other.nchannels_ = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (tex_id_) glDeleteTextures(1, &tex_id_);
        if (data_) stbi_image_free(data_);

        tex_id_    = other.tex_id_;
        width_     = other.width_;
        height_    = other.height_;
        nchannels_ = other.nchannels_;
        data_      = other.data_;
        tex_type_  = other.tex_type_;

        other.tex_id_ = 0;
        other.data_   = nullptr;
        other.width_ = other.height_ = other.nchannels_ = 0;
    }
    return *this;
}
void Texture::Bind(GLuint slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(static_cast<GLenum>(tex_type_), tex_id_);
}

void Texture::Unbind() { glBindTexture(static_cast<GLenum>(tex_type_), 0); }

bool Texture::LoadFromFile(std::string_view path, bool flip_vertically) {
    stbi_set_flip_vertically_on_load(flip_vertically);

    // Only 2D files can be loaded from disk (1D/3D must be procedural)
    data_ = stbi_load(path.data(), &width_, &height_, &nchannels_, 0);

    if (!data_) {
        std::cerr << "Unable to load data from file: " << path << "\n";
        return false;
    }

    GLenum format    = static_cast<GLenum>(GetFormatFromChannels(nchannels_));
    GLenum gl_target = static_cast<GLenum>(tex_type_);
    Bind();

    switch (tex_type_) {
        case TextureType::kTexture1D: {
            // Only width is relevant; ignore height_
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, width_, 0, format,
                         GL_UNSIGNED_BYTE, data_);
            break;
        }

        case TextureType::kTexture2D: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, format,
                         GL_UNSIGNED_BYTE, data_);
            break;
        }

        case TextureType::kTexture3D: {
            // You cannot load 3D texture from a single image
            std::cerr << "Loading 3D textures from file is not supported.\n";
            stbi_image_free(data_);
            data_ = nullptr;
            return false;
        }

        default:
            std::cerr << "Unknown texture type.\n";
            stbi_image_free(data_);
            data_ = nullptr;
            return false;
    }

    // Only generate mipmaps for 1D/2D textures
    if (tex_type_ == TextureType::kTexture1D ||
        tex_type_ == TextureType::kTexture2D) {
        glGenerateMipmap(gl_target);
    }

    return true;
}
}  // namespace pop::gfx
