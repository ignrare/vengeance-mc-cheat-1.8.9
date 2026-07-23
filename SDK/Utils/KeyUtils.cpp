#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "KeyUtils.h"
#include <Windows.h>


std::string VirtualKeyToString(int vk) {
  if (vk == 0)
    return "Not Set";

  switch (vk) {
  case VK_NUMPAD0:
    return "Numpad 0";
  case VK_NUMPAD1:
    return "Numpad 1";
  case VK_NUMPAD2:
    return "Numpad 2";
  case VK_NUMPAD3:
    return "Numpad 3";
  case VK_NUMPAD4:
    return "Numpad 4";
  case VK_NUMPAD5:
    return "Numpad 5";
  case VK_NUMPAD6:
    return "Numpad 6";
  case VK_NUMPAD7:
    return "Numpad 7";
  case VK_NUMPAD8:
    return "Numpad 8";
  case VK_NUMPAD9:
    return "Numpad 9";
  case VK_F1:
    return "F1";
  case VK_F2:
    return "F2";
  case VK_F3:
    return "F3";
  case VK_F4:
    return "F4";
  case VK_F5:
    return "F5";
  case VK_F6:
    return "F6";
  case VK_F7:
    return "F7";
  case VK_F8:
    return "F8";
  case VK_F9:
    return "F9";
  case VK_F10:
    return "F10";
  case VK_F11:
    return "F11";
  case VK_F12:
    return "F12";
  case VK_SPACE:
    return "Space";
  case VK_RETURN:
    return "Enter";
  case VK_ESCAPE:
    return "Escape";
  case VK_TAB:
    return "Tab";
  case VK_LSHIFT:
    return "Left Shift";
  case VK_RSHIFT:
    return "Right Shift";
  case VK_LCONTROL:
    return "Left Ctrl";
  case VK_RCONTROL:
    return "Right Ctrl";
  case VK_LMENU:
    return "Left Alt";
  case VK_RMENU:
    return "Right Alt";
  default:
    if (vk >= 0x41 && vk <= 0x5A) {
      return std::string(1, (char)vk);
    }
    return "Key " + std::to_string(vk);
  }
}
