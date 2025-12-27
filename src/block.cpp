#include "block/block.hpp"
#include "util/directions.hpp"
namespace pop::block {
using util::direction;
uint8_t Block::GetTextureCode(direction dir) const {
    switch (dir) {
        case direction::kTop:
            return settings_.uv.top;
        case direction::kBottom:
            return settings_.uv.bottom;
        case direction::kNorth:
            return settings_.uv.north;
        case direction::kSouth:
            return settings_.uv.south;
        case direction::kWest:
            return settings_.uv.west;
        case direction::kEast:
            return settings_.uv.east;
        default:
            return 0;
    }
}
};  // namespace pop::block
