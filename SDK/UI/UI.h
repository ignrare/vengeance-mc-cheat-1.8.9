#pragma once

#include "../Modules/Combat/Aimbot.h"
#include "../Modules/Combat/AutoClicker.h"
#include "../Modules/Combat/Blockhit.h"
#include "../Modules/Visuals/ChestESP.h"
#include "../Modules/Visuals/ESP.h"
#include "../Modules/Visuals/Tracer.h"
#include "../Modules/Visuals/XRay.h"

namespace UITheme {

const ImVec4 BACKGROUND_DARK = ImVec4(0.07f, 0.07f, 0.07f, 0.98f);
const ImVec4 BACKGROUND_DARKER = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
const ImVec4 PANEL_BG = ImVec4(0.10f, 0.10f, 0.10f, 0.95f);

const ImVec4 TEXT_PRIMARY = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
const ImVec4 TEXT_SECONDARY = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
const ImVec4 TEXT_DISABLED = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

const ImVec4 ACCENT_BLUE = ImVec4(0.0f, 0.48f, 1.0f, 1.0f);
const ImVec4 ACCENT_BLUE_HOVER = ImVec4(0.0f, 0.58f, 1.0f, 1.0f);
const ImVec4 ACCENT_BLUE_ACTIVE = ImVec4(0.0f, 0.38f, 0.8f, 1.0f);
const ImVec4 ACCENT_BLUE_DIM = ImVec4(0.0f, 0.48f, 1.0f, 0.3f);

const ImVec4 BUTTON_NORMAL = ImVec4(0.15f, 0.15f, 0.15f, 0.8f);
const ImVec4 BUTTON_HOVER = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
const ImVec4 BUTTON_ACTIVE = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);

const ImVec4 BUTTON_SELECTED = ImVec4(0.10f, 0.15f, 0.25f, 1.0f);
const ImVec4 BUTTON_SELECTED_HOVER = ImVec4(0.15f, 0.20f, 0.35f, 1.0f);

const ImVec4 BORDER_COLOR = ImVec4(0.15f, 0.15f, 0.15f, 0.5f);
const ImVec4 SEPARATOR_COLOR = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);

const ImVec4 TOGGLE_OFF = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
const ImVec4 TOGGLE_ON = ACCENT_BLUE;

const ImVec4 SLIDER_GRAB = ACCENT_BLUE;
const ImVec4 SLIDER_GRAB_ACTIVE = ACCENT_BLUE_HOVER;
}

namespace UILayout {
const float SIDEBAR_WIDTH = 140.0f;
const float TITLE_BAR_HEIGHT = 35.0f;
const float FOOTER_HEIGHT = 28.0f;
const float WINDOW_PADDING = 15.0f;
const float PADDING = 12.0f;
const float ITEM_SPACING = 8.0f;
const float SECTION_SPACING = 12.0f;

const float WINDOW_ROUNDING = 8.0f;
const float BUTTON_ROUNDING = 4.0f;
const float FRAME_ROUNDING = 4.0f;
const float GRAB_ROUNDING = 3.0f;

const float CATEGORY_BUTTON_HEIGHT = 32.0f;
const float MODULE_BUTTON_HEIGHT = 28.0f;
const float TOGGLE_SIZE = 16.0f;
const float CHECKBOX_SIZE = 14.0f;
}

struct CategoryInfo {
  const char *name;
  std::vector<const char *> modules;
};

extern std::vector<CategoryInfo> g_categories;
extern int g_selectedCategory;
extern int g_selectedModule;

void InitializeUI();
void UpdateUI();
void RenderModernMenu();
void CleanupUI();

void LoadCustomFonts();
extern ImFont *g_customFont;
extern ImFont *g_smallFont;

void ApplyModernTheme();

bool CustomCheckbox(const char *label, bool *value);
bool CustomSlider(const char *label, float *value, float min, float max,
                  const char *format = "%.1f");
bool CustomSliderInt(const char *label, int *value, int min, int max,
                     const char *format = "%d");
bool ModernToggle(const char *label, bool *value,
                  const char *description = nullptr);
bool CategoryButton(const char *label, bool selected);
bool ModuleTabButton(const char *label, bool selected);
void SectionHeader(const char *text);
void RenderModulePanel(const char *title, std::function<void()> renderFunc);

void RenderCombatUI();
void RenderVisualsUI();
void RenderMovementUI();
void RenderNetworkUI();
void RenderUtilitiesUI();
void RenderSettingsUI();
