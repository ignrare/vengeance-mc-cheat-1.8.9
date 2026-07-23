

#include "imgui.h"
#include "imgui_impl_sdlrenderer.h"
#if defined(_MSC_VER) && _MSC_VER <= 1500
#include <stddef.h>
#else
#include <stdint.h>
#endif

#include <SDL.h>
#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

struct ImGui_ImplSDLRenderer_Data
{
    SDL_Renderer*   SDLRenderer;
    SDL_Texture*    FontTexture;
    ImGui_ImplSDLRenderer_Data() { memset(this, 0, sizeof(*this)); }
};

static ImGui_ImplSDLRenderer_Data* ImGui_ImplSDLRenderer_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplSDLRenderer_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
}

bool ImGui_ImplSDLRenderer_Init(SDL_Renderer* renderer)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendRendererUserData == NULL && "Already initialized a renderer backend!");
    IM_ASSERT(renderer != NULL && "SDL_Renderer not initialized!");

    ImGui_ImplSDLRenderer_Data* bd = IM_NEW(ImGui_ImplSDLRenderer_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_sdlrenderer";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    bd->SDLRenderer = renderer;

    return true;
}

void ImGui_ImplSDLRenderer_Shutdown()
{
    ImGui_ImplSDLRenderer_Data* bd = ImGui_ImplSDLRenderer_GetBackendData();
    IM_ASSERT(bd != NULL && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDLRenderer_DestroyDeviceObjects();

    io.BackendRendererName = NULL;
    io.BackendRendererUserData = NULL;
    IM_DELETE(bd);
}

static void ImGui_ImplSDLRenderer_SetupRenderState()
{
	ImGui_ImplSDLRenderer_Data* bd = ImGui_ImplSDLRenderer_GetBackendData();

	SDL_RenderSetViewport(bd->SDLRenderer, NULL);
	SDL_RenderSetClipRect(bd->SDLRenderer, NULL);
}

void ImGui_ImplSDLRenderer_NewFrame()
{
    ImGui_ImplSDLRenderer_Data* bd = ImGui_ImplSDLRenderer_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplSDLRenderer_Init()?");

    if (!bd->FontTexture)
        ImGui_ImplSDLRenderer_CreateDeviceObjects();
}

void ImGui_ImplSDLRenderer_RenderDrawData(ImDrawData* draw_data)
{
	ImGui_ImplSDLRenderer_Data* bd = ImGui_ImplSDLRenderer_GetBackendData();

    float rsx = 1.0f;
	float rsy = 1.0f;
	SDL_RenderGetScale(bd->SDLRenderer, &rsx, &rsy);
    ImVec2 render_scale;
	render_scale.x = (rsx == 1.0f) ? draw_data->FramebufferScale.x : 1.0f;
	render_scale.y = (rsy == 1.0f) ? draw_data->FramebufferScale.y : 1.0f;

	int fb_width = (int)(draw_data->DisplaySize.x * render_scale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * render_scale.y);
	if (fb_width == 0 || fb_height == 0)
		return;

    struct BackupSDLRendererState
    {
        SDL_Rect    Viewport;
        bool        ClipEnabled;
        SDL_Rect    ClipRect;
    };
    BackupSDLRendererState old = {};
    old.ClipEnabled = SDL_RenderIsClipEnabled(bd->SDLRenderer) == SDL_TRUE;
    SDL_RenderGetViewport(bd->SDLRenderer, &old.Viewport);
    SDL_RenderGetClipRect(bd->SDLRenderer, &old.ClipRect);

	ImVec2 clip_off = draw_data->DisplayPos;
	ImVec2 clip_scale = render_scale;

    ImGui_ImplSDLRenderer_SetupRenderState();
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {

                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplSDLRenderer_SetupRenderState();
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {

                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_min.x < 0.0f) { clip_min.x = 0.0f; }
                if (clip_min.y < 0.0f) { clip_min.y = 0.0f; }
                if (clip_max.x > fb_width) { clip_max.x = (float)fb_width; }
                if (clip_max.y > fb_height) { clip_max.y = (float)fb_height; }
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                SDL_Rect r = { (int)(clip_min.x), (int)(clip_min.y), (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y) };
                SDL_RenderSetClipRect(bd->SDLRenderer, &r);

                const float* xy = (const float*)((const char*)(vtx_buffer + pcmd->VtxOffset) + IM_OFFSETOF(ImDrawVert, pos));
                const float* uv = (const float*)((const char*)(vtx_buffer + pcmd->VtxOffset) + IM_OFFSETOF(ImDrawVert, uv));
#if SDL_VERSION_ATLEAST(2,0,19)
                const SDL_Color* color = (const SDL_Color*)((const char*)(vtx_buffer + pcmd->VtxOffset) + IM_OFFSETOF(ImDrawVert, col));
#else
                const int* color = (const int*)((const char*)(vtx_buffer + pcmd->VtxOffset) + IM_OFFSETOF(ImDrawVert, col));
#endif

				SDL_Texture* tex = (SDL_Texture*)pcmd->GetTexID();
                SDL_RenderGeometryRaw(bd->SDLRenderer, tex,
                    xy, (int)sizeof(ImDrawVert),
                    color, (int)sizeof(ImDrawVert),
                    uv, (int)sizeof(ImDrawVert),
                    cmd_list->VtxBuffer.Size - pcmd->VtxOffset,
                    idx_buffer + pcmd->IdxOffset, pcmd->ElemCount, sizeof(ImDrawIdx));
            }
        }
    }

    SDL_RenderSetViewport(bd->SDLRenderer, &old.Viewport);
    SDL_RenderSetClipRect(bd->SDLRenderer, old.ClipEnabled ? &old.ClipRect : NULL);
}

bool ImGui_ImplSDLRenderer_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDLRenderer_Data* bd = ImGui_ImplSDLRenderer_GetBackendData();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    bd->FontTexture = SDL_CreateTexture(bd->SDLRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    if (bd->FontTexture == NULL)
    {
        SDL_Log("error creating texture");
        return false;
    }
    SDL_UpdateTexture(bd->FontTexture, NULL, pixels, 4 * width);
    SDL_SetTextureBlendMode(bd->FontTexture, SDL_BLENDMODE_BLEND);

    io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTexture);

    return true;
}

void ImGui_ImplSDLRenderer_DestroyFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDLRenderer_Data* bd = ImGui_ImplSDLRenderer_GetBackendData();
    if (bd->FontTexture)
    {
        io.Fonts->SetTexID(0);
        SDL_DestroyTexture(bd->FontTexture);
        bd->FontTexture = NULL;
    }
}

bool ImGui_ImplSDLRenderer_CreateDeviceObjects()
{
    return ImGui_ImplSDLRenderer_CreateFontsTexture();
}

void ImGui_ImplSDLRenderer_DestroyDeviceObjects()
{
    ImGui_ImplSDLRenderer_DestroyFontsTexture();
}
