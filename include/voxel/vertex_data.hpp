#pragma once

#include <cstdint>
namespace pop::voxel::vertex_data {

struct Layout {
    // NOTE: the bits are 1 greater than the log2(kSize_x) because the block at
    // ksize-1 will have a vertex at ksize.
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

using VertexType                       = uint32_t;
static constexpr size_t BytesPerVertex = sizeof(VertexType);

constexpr VertexType PackData(int x, int y, int z, int normal, int texId) {
    VertexType packed = 0;

    packed |= (static_cast<VertexType>(x) & Layout::X_MASK) << Layout::X_SHIFT;
    packed |= (static_cast<VertexType>(y) & Layout::Y_MASK) << Layout::Y_SHIFT;
    packed |= (static_cast<VertexType>(z) & Layout::Z_MASK) << Layout::Z_SHIFT;
    packed |= (static_cast<VertexType>(normal) & Layout::NORM_MASK)
              << Layout::NORM_SHIFT;
    packed |= (static_cast<VertexType>(texId) & Layout::ID_MASK)
              << Layout::ID_SHIFT;

    return packed;
}

}  // namespace pop::voxel::vertex_data
