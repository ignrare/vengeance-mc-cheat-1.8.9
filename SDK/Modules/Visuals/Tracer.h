#pragma once

#include "../../includes.h"

extern bool tracerEnabled;
extern ImVec4 tracerColor;

extern float tracerWidth;
extern int tracerOrigin;
extern bool tracerShowDistance;
extern ImVec4 tracerDistanceColor;

void InitializeTracer();
void UpdateTracer();
void RenderTracerUI();
void CleanupTracer();
void RenderTracers();
