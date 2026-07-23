#pragma once

#include "../../includes.h"

enum ClickerActivationMode {
    CLICKER_MODE_TOGGLE = 0,
    CLICKER_MODE_HOLD_KEY = 1,
    CLICKER_MODE_HOLD_LMB = 2
};

extern bool clickerEnabled;
extern int clickerActivationMode;
extern int clickerActivationKey;
extern bool clickerActivationKeyHeld;
extern bool clickerLeftClick;
extern int clickerCpsInt;
extern bool clickerEnableRandomization;
extern double clickerRandomizationAmount;
extern bool clickerExhaustMode;
extern int clickerExhaustChance;
extern double clickerExhaustDropCps;
extern bool clickerSpikeMode;
extern int clickerSpikeChance;
extern double clickerSpikeIncreaseCps;
extern double currentCps;
extern int totalClicks;
extern bool clickerSwordOnly;

void InitializeAutoClicker();
void UpdateAutoClicker();
void RenderAutoClickerUI();
void CleanupAutoClicker();

void clickerThread();
