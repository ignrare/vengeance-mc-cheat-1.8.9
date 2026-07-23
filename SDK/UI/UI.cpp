

#include "UI.h"
#include "../Core/Core.h"

ImFont *g_customFont = nullptr;
ImFont *g_smallFont = nullptr;

std::vector<CategoryInfo> g_categories = {
    {"Combat", {{"Aim Assist", "Aim assistance"}, {"AutoClicker", "CPS, Pattern"}, {"Blockhit", "Auto block"}}},
    {"Visuals", {{"ESP", "Player boxes"}, {"Tracer", "Player lines"}, {"XRay", "See through"}, {"Chest ESP", "Chest highlight"}, {"ArrayList", "Module list"}}},
    {"Settings", {{"Unload", "Eject DLL"}}}};

int g_selectedCategory = 0;
int g_selectedModule = 0;

void LoadCustomFonts() {
  ImGuiIO &io = ImGui::GetIO();

  static const ImWchar ranges[] = {
      0x0020, 0x00FF,
      0x0400, 0x044F,
      0,
  };

  g_customFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\sserife.fon", 14.0f, nullptr, ranges);
  if (!g_customFont) {
    g_customFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 14.0f, nullptr, ranges);
  }
  if (!g_customFont) {
    g_customFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 14.0f, nullptr, ranges);
  }
  if (!g_customFont) {
    g_customFont = io.Fonts->AddFontDefault();
  }

  g_smallFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\sserife.fon", 12.0f, nullptr, ranges);
  if (!g_smallFont) {
    g_smallFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 12.0f, nullptr, ranges);
  }
  if (!g_smallFont) {
    g_smallFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 12.0f, nullptr, ranges);
  }
  if (!g_smallFont) {
    g_smallFont = io.Fonts->AddFontDefault();
  }

  io.Fonts->Build();
}

void InitializeUI() {
  LoadCustomFonts();
}

void UpdateUI() {}
void CleanupUI() {}
