#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "graphics/vertex_buffers.hpp"

namespace pop::voxel::vertex_data {

struct VertexData {
    // Data to be fitted in GLuint
    struct PackedDataLayout {
        // NOTE: the bits are 1 greater than the log2(kSize_x) because the block
        // at ksize-1 will have a vertex at ksize.
        static constexpr uint8_t X_BITS    = 5;
        static constexpr uint8_t Y_BITS    = 7;
        static constexpr uint8_t Z_BITS    = 5;
        static constexpr uint8_t NORM_BITS = 3;
        static constexpr uint8_t ID_BITS   = 12;

        static constexpr uint8_t X_SHIFT    = 0;
        static constexpr uint8_t Y_SHIFT    = X_BITS;
        static constexpr uint8_t Z_SHIFT    = Y_SHIFT + Y_BITS;
        static constexpr uint8_t NORM_SHIFT = Z_SHIFT + Z_BITS;
        static constexpr uint8_t ID_SHIFT   = NORM_SHIFT + NORM_BITS;

        static constexpr uint32_t X_MASK    = (1u << X_BITS) - 1;
        static constexpr uint32_t Y_MASK    = (1u << Y_BITS) - 1;
        static constexpr uint32_t Z_MASK    = (1u << Z_BITS) - 1;
        static constexpr uint32_t NORM_MASK = (1u << NORM_BITS) - 1;
        static constexpr uint32_t ID_MASK   = (1u << ID_BITS) - 1;
        // Total size verification
        static_assert(X_BITS + Y_BITS + Z_BITS + NORM_BITS + ID_BITS <= 32,
                      "Vertex data exceeds 32 bits! :|");
    };
    using PackedDataType = uint32_t;
    PackedDataType packedVertexData;
    // Color to be multiplied in vertex shader
    uint8_t        r, g, b, a;
};
inline std::vector<gfx::Attribute> GetVertexAttributes() {
    constexpr int stride = sizeof(VertexData);
    return {{0, 1, gfx::GLType::kUInt, false, stride, 0, true},
            {1, 4, gfx::GLType::kUByte, true, stride, offsetof(VertexData, r),
             false}};
}

constexpr VertexData PackVertexData(int x, int y, int z, int normal, int texId,
                                    uint8_t r, uint8_t g, uint8_t b,
                                    uint8_t a) {
    VertexData::PackedDataType packed = 0;
    using pckLayout                   = VertexData::PackedDataLayout;

    packed |= (static_cast<VertexData::PackedDataType>(x) & pckLayout::X_MASK)
              << pckLayout::X_SHIFT;
    packed |= (static_cast<VertexData::PackedDataType>(y) & pckLayout::Y_MASK)
              << pckLayout::Y_SHIFT;
    packed |= (static_cast<VertexData::PackedDataType>(z) & pckLayout::Z_MASK)
              << pckLayout::Z_SHIFT;
    packed |=
        (static_cast<VertexData::PackedDataType>(normal) & pckLayout::NORM_MASK)
        << pckLayout::NORM_SHIFT;
    packed |=
        (static_cast<VertexData::PackedDataType>(texId) & pckLayout::ID_MASK)
        << pckLayout::ID_SHIFT;

    return {packed, r, g, b, a};
}
}  // namespace pop::voxel::vertex_data
