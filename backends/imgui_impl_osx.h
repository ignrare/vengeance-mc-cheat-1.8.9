

#include "imgui.h"

@class NSEvent;
@class NSView;

IMGUI_IMPL_API bool     ImGui_ImplOSX_Init(NSView* _Nonnull view);
IMGUI_IMPL_API void     ImGui_ImplOSX_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplOSX_NewFrame(NSView* _Nullable view);
IMGUI_IMPL_API bool     ImGui_ImplOSX_HandleEvent(NSEvent* _Nonnull event, NSView* _Nullable view);
