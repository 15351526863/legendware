#include "../gui/core/core.h"
#include "input_manager.h"
#include <unordered_map>
#include <functional>
#include <vector>

class InputManager {
private:
    struct KeyBind {
        int key;
        std::function<void()> callback;
        bool requires_release;
        bool was_pressed;
    };
    
    static std::vector<KeyBind> key_binds;
    static std::unordered_map<int, bool> key_states;
    
public:
    static void Initialize() {
        // Clear existing binds
        key_binds.clear();
        key_states.clear();
        
        // Register default binds
        RegisterKeyBind(VK_INSERT, []() { ToggleMenu(); }, true);
        RegisterKeyBind(VK_DELETE, []() { ToggleAimbot(); }, true);
        // Add more default binds as needed
    }
    
    static void RegisterKeyBind(int key, std::function<void()> callback, bool requires_release = true) {
        KeyBind bind;
        bind.key = key;
        bind.callback = callback;
        bind.requires_release = requires_release;
        bind.was_pressed = false;
        key_binds.push_back(bind);
    }
    
    static void ProcessKeyInput(int key, bool is_down) {
        key_states[key] = is_down;
        
        for (auto& bind : key_binds) {
            if (bind.key == key) {
                if (is_down && !bind.was_pressed) {
                    // Key just pressed
                    if (!bind.requires_release) {
                        bind.callback();
                    }
                    bind.was_pressed = true;
                } else if (!is_down && bind.was_pressed) {
                    // Key just released
                    if (bind.requires_release) {
                        bind.callback();
                    }
                    bind.was_pressed = false;
                }
            }
        }
    }
    
    static bool IsKeyDown(int key) {
        auto it = key_states.find(key);
        return it != key_states.end() ? it->second : false;
    }
    
    static bool IsKeyBound(int key) {
        for (const auto& bind : key_binds) {
            if (bind.key == key) return true;
        }
        return false;
    }
};

// Static member definitions
std::vector<InputManager::KeyBind> InputManager::key_binds;
std::unordered_map<int, bool> InputManager::key_states;

bool menu_open = false;
bool aimbot_enabled = false;

// Global functions
void ProcessKeyBindings(int key, bool is_down) {
    InputManager::ProcessKeyInput(key, is_down);
}

bool IsKeyBound(int key) {
    return InputManager::IsKeyBound(key);
}

void InitializeInputSystem() {
    InputManager::Initialize();
}

void ToggleMenu() {
    menu_open = !menu_open;
    // if there is global variable for menu state, update it as well
    core::s::is_opened = menu_open;
}

void ToggleAimbot() {
    aimbot_enabled = !aimbot_enabled;
}
