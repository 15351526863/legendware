#pragma once
#include <functional>
#include <windows.h>

void InitializeInputSystem();
void ProcessKeyBindings(int key, bool is_down);
bool IsKeyBound(int key);

// Function declarations for key binding callbacks
void ToggleMenu();
void ToggleAimbot();
