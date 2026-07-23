#pragma once

#include "../../includes.h"

extern bool espEnabled;
extern ImVec4 espOutlineColor;
extern ImVec4 espFillColor;

extern float espOutlineWidth;
extern bool espShowFill;
extern bool espShowOutline;
extern int espBoxStyle;
extern float espCornerLength;
extern bool espShowDistance;
extern ImVec4 espDistanceColor;

void InitializeESP();
void UpdateESP();
void RenderESPUI();
void CleanupESP();
void RenderESP();
void UpdateESPColors();
