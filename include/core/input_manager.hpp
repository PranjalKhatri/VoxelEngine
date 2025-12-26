#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

namespace pop::core {

class InputManager {
   public:
    using InputAction = std::function<void()>;
    void RegisterKeyboardAction(int key, InputAction action);
    void RegisterMouseAction(int key, InputAction action);
    void HandleMouseEvent(int button, int action, int mods);
    void HandleKeyboardEvent(int key, int scanCode, int action, int mods);

   private:
    std::unordered_map<int, std::vector<InputAction>> keyboard_registry_;
    std::unordered_map<int, std::vector<InputAction>> mouse_registry_;
};

}  // namespace pop::core
