#pragma once

#include "../../includes.h"

extern bool xrayEnabled;

extern float xrayOpacity;
extern bool xrayShowOres;
extern bool xrayShowChests;
extern bool xrayShowSpawners;

void InitializeXRay();
void UpdateXRay();
void RenderXRayUI();
void CleanupXRay();
void ApplyXRay();
void RemoveXRay();
