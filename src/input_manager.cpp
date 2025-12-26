#include "GLFW/glfw3.h"
#include "core/input_manager.hpp"
#include <functional>
#include <unordered_map>

namespace pop::core {

void InputManager::RegisterKeyboardAction(int key, InputAction action) {
    keyboard_registry_[key].emplace_back(std::move(action));
}

void InputManager::RegisterMouseAction(int button, InputAction action) {
    mouse_registry_[button].emplace_back(std::move(action));
}
void InputManager::HandleMouseEvent(int button, int action, int /*mods*/) {
    if (action != GLFW_PRESS) return;
    for (auto &iactions : mouse_registry_[button]) {
        iactions();
    }
}
void InputManager::HandleKeyboardEvent(int key, int /*scanCode*/, int action,
                                       int /*mods*/) {
    if (action != GLFW_PRESS) return;
    for (auto &iactions : keyboard_registry_[key]) {
        iactions();
    }
}

}  // namespace pop::core
