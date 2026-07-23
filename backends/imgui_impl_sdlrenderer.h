

#pragma once
#include "imgui.h"

struct SDL_Renderer;

IMGUI_IMPL_API bool     ImGui_ImplSDLRenderer_Init(SDL_Renderer* renderer);
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_RenderDrawData(ImDrawData* draw_data);

IMGUI_IMPL_API bool     ImGui_ImplSDLRenderer_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplSDLRenderer_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplSDLRenderer_DestroyDeviceObjects();
