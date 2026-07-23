#pragma once

#include "../includes.h"

struct Keybind {
    int key;
    bool isPressed;
    bool wasPressed;
    std::string action;
};

extern std::vector<Keybind> keybinds;
extern bool keybindCaptureMode;
extern std::string keybindCaptureAction;

void InitializeKeybinds();
void UpdateKeybinds();
void CleanupKeybinds();
void AddKeybind(const std::string& action, int key);
void RemoveKeybind(const std::string& action);
int GetKeybind(const std::string& action);
bool IsKeyPressed(int key);
std::string VirtualKeyToString(int key);
void StartKeybindCapture(const std::string& action);
void StopKeybindCapture();
int CaptureKeyPressed();

enum KeybindAction {
    ACTION_AIMBOT_TOGGLE = 0,
    ACTION_AIMBOT_LOCKON = 1,
    ACTION_AUTOCLICKER_TOGGLE = 2,
    ACTION_ESP_TOGGLE = 3,
    ACTION_TRACER_TOGGLE = 4,
    ACTION_XRAY_TOGGLE = 5,
    ACTION_CHESTESP_TOGGLE = 6,
    ACTION_BLOCKHIT_TOGGLE = 7,
    ACTION_MENU_TOGGLE = 8
};
