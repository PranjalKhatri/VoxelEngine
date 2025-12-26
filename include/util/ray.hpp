#pragma once

#include <glm/glm.hpp>

namespace pop::util {
class Ray {
   public:
    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : origin_{origin},
          direction_{direction},
          inv_direction_{1.0 / direction.x, 1.0 / direction.y,
                         1.0 / direction.z} {}
    glm::vec3 At(float time) const { return origin_ + direction_ * time; }

   private:
    glm::vec3 origin_;
    glm::vec3 direction_, inv_direction_;
};
}  // namespace pop::util
