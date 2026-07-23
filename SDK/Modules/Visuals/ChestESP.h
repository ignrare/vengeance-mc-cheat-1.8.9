#pragma once

#include "../../includes.h"

extern bool chestEspEnabled;
extern ImVec4 chestOutlineColor;
extern ImVec4 chestFillColor;

extern float chestOutlineWidth;
extern bool chestShowFill;
extern bool chestShowOutline;
extern bool chestShowTracers;
extern ImVec4 chestTracerColor;
extern float chestTracerWidth;

void InitializeChestESP();
void UpdateChestESP();
void RenderChestESPUI();
void CleanupChestESP();
void RenderChestESP();
