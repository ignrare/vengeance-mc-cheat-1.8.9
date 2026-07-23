#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fstream>
#include <tchar.h>
#include <random>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>
using byte = unsigned char;
#include "../imgui/imgui.h"
#include "../SDK/Esp/Color.h"
#include "../SDK/Esp/Radius.h"
#include "../SDK/Esp/Position.h"
#include "Detours.h"
#include <gl/gl.h> 
#include "../SDK/GLDraw/glDraw.h"
#include "../SDK/GLText/gltext.h"
#include <stdio.h>
#include <iostream>
#include <mutex>
#include "GlText/gltext.h"
#pragma comment(lib,"opengl32.lib")
#define KEY_X 0x58
#define KEY_V 0x56
#define KEY_B 0x42

#define KEY_B_DOWN 3145729
#define KEY_B_UP 3224371201
#define KEY_V_DOWN 3080193
#define KEY_V_UP 3224305665
#define KEY_X_DOWN 2949121
#define KEY_X_UP 3224174593
#pragma once
typedef void(__stdcall* T_glEnable)(GLenum);
typedef void(__stdcall* T_glDisable)(GLenum);
typedef BOOL(__stdcall* T_SwapBuffers)(HDC);
typedef void(__stdcall* T_glScalef)(float, float, float);
typedef void(__stdcall* T_glTranslatef)(float, float, float);
typedef void(__stdcall* T_glOrtho)(double, double, double, double, double, double);

extern GL::Font glFont;
extern HHOOK keyHookHandle;
using namespace std;


void __stdcall myglEnable(unsigned int cap);
void __stdcall myglDisable(unsigned int cap);
void __stdcall myglOrtho(double left, double right, double bottom, double top, double zNear, double zFar);
void __stdcall myglTranslatef(float x, float y, float z);
void __stdcall myglScalef(float x, float y, float z);