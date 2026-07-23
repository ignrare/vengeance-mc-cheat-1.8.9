

#include "Keybinds.h"
#include "Core.h"

std::vector<Keybind> keybinds;
bool keybindCaptureMode = false;
std::string keybindCaptureAction = "";

void InitializeKeybinds() {}
void UpdateKeybinds() {}
void CleanupKeybinds() { keybinds.clear(); }
void AddKeybind(const std::string& action, int key) {}
void RemoveKeybind(const std::string& action) {}
int GetKeybind(const std::string& action) { return -1; }
bool IsKeyPressed(int key) { return (GetAsyncKeyState(key) & 0x8000) != 0; }
void StartKeybindCapture(const std::string& action) {}
void StopKeybindCapture() {}
int CaptureKeyPressed() { return -1; }
