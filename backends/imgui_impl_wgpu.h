

#pragma once
#include "imgui.h"
#include <webgpu/webgpu.h>

IMGUI_IMPL_API bool ImGui_ImplWGPU_Init(WGPUDevice device, int num_frames_in_flight, WGPUTextureFormat rt_format);
IMGUI_IMPL_API void ImGui_ImplWGPU_Shutdown();
IMGUI_IMPL_API void ImGui_ImplWGPU_NewFrame();
IMGUI_IMPL_API void ImGui_ImplWGPU_RenderDrawData(ImDrawData* draw_data, WGPURenderPassEncoder pass_encoder);

IMGUI_IMPL_API void ImGui_ImplWGPU_InvalidateDeviceObjects();
IMGUI_IMPL_API bool ImGui_ImplWGPU_CreateDeviceObjects();
