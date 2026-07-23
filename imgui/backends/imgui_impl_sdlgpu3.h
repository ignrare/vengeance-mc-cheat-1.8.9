

#pragma once
#include "imgui.h"
#ifndef IMGUI_DISABLE
#include <SDL3/SDL_gpu.h>

struct ImGui_ImplSDLGPU3_InitInfo
{
    SDL_GPUDevice*       Device             = nullptr;
    SDL_GPUTextureFormat ColorTargetFormat  = SDL_GPU_TEXTUREFORMAT_INVALID;
    SDL_GPUSampleCount   MSAASamples        = SDL_GPU_SAMPLECOUNT_1;
};

IMGUI_IMPL_API bool     ImGui_ImplSDLGPU3_Init(ImGui_ImplSDLGPU3_InitInfo* info);
IMGUI_IMPL_API void     ImGui_ImplSDLGPU3_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplSDLGPU3_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplSDLGPU3_PrepareDrawData(ImDrawData* draw_data, SDL_GPUCommandBuffer* command_buffer);
IMGUI_IMPL_API void     ImGui_ImplSDLGPU3_RenderDrawData(ImDrawData* draw_data, SDL_GPUCommandBuffer* command_buffer, SDL_GPURenderPass* render_pass, SDL_GPUGraphicsPipeline* pipeline = nullptr);

IMGUI_IMPL_API void     ImGui_ImplSDLGPU3_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplSDLGPU3_DestroyDeviceObjects();

IMGUI_IMPL_API void     ImGui_ImplSDLGPU3_UpdateTexture(ImTextureData* tex);

#endif
