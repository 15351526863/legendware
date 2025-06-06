// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "hooks.h"
#include "..\menu\menu.h"
#include "../utils/input_manager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static std::unordered_map<int, bool> key_states;
static std::unordered_map<int, bool> key_pressed;
WNDPROC original_wndproc = nullptr;

LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Handle key messages first, before GUI
    switch (uMsg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam < 256) {
                bool was_down = key_states[wParam];
                key_states[wParam] = true;
                key_pressed[wParam] = !was_down; // True only on first press
                
                // Process key bindings BEFORE GUI
                ProcessKeyBindings(wParam, true);
            }
            break;
            
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (wParam < 256) {
                key_states[wParam] = false;
                key_pressed[wParam] = false;
                ProcessKeyBindings(wParam, false);
            }
            break;
    }
    
    // Only handle GUI input if menu is open AND key isn't bound to a function
    if (menu_open && !IsKeyBound(wParam)) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
            return true;
        }
    }
    
    return CallWindowProc(original_wndproc, hWnd, uMsg, wParam, lParam);
}
