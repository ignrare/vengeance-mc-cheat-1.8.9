#pragma once

#include "../includes.h"

void __stdcall myglEnable(unsigned int cap);
void __stdcall myglDisable(unsigned int cap);
BOOL __stdcall mySwapBuffers(HDC hDC);

BOOL InitializeHooks();
void CleanupImGui();
void UnloadDLL();

LRESULT __stdcall hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall keyHandler(int nCode, WPARAM wParam, LPARAM lParam);

struct Vector2D {
    float x, y;
    Vector2D(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Vector3D {
    float x, y, z;
    Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct EntityData {
    std::vector<float> modelview;
    std::vector<float> projection;
};

extern EntityData entity;
extern EntityData players;
extern EntityData chest;
extern EntityData largeChest;

extern int clickerHotkeyCode;

extern bool aimbotEnabled;
extern bool clickerEnabled;
extern bool espEnabled;
extern bool tracerEnabled;
extern bool xrayEnabled;
extern bool chestEspEnabled;

extern float aimDistance;
extern int aimSpeedInt;
extern float aimAngleDeg;
extern bool aimTestEnabled;

extern bool clickerSwordOnly;
extern bool aimbotSwordOnly;
extern bool blockhitSwordOnly;
extern bool reachSwordOnly;
extern bool hitSelectSwordOnly;

void savePosition(EntityData& data, float x, float y, float z);
bool projectToScreen(const std::vector<float>& mv, const std::vector<float>& pr, const int* viewport,
                   float objX, float objY, float objZ, float& winX, float& winY);
void glText(const char* text, float x, float y, float r, float g, float b);
void DrawHud();

void InitializeAimbot();
void UpdateAimbot();
void InitializeAutoClicker();
void UpdateAutoClicker();
void clickerThread();
void InitializeBlockhit();
void UpdateBlockhit();
void blockhitThread();
void InitializeHitSelect();
void UpdateHitSelect();
void hitSelectThread();

void InitializeESP();
void UpdateESP();
void InitializeTracer();
void UpdateTracer();
void InitializeXRay();
void UpdateXRay();
void InitializeChestESP();
void UpdateChestESP();

void InitializeUI();
void UpdateUI();
void RenderModernMenu();

void UpdateCursorState();
