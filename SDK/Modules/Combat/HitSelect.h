#pragma once

#include "../../includes.h"

enum HitSelectMode {
    HITSELECT_MODE_BURST = 0,
    HITSELECT_MODE_CRITICALS = 1
};

extern bool hitSelectEnabled;
extern int hitSelectMode;
extern int hitSelectPauseDuration;
extern int hitSelectWaitForFirstHit;
extern int hitSelectHitLaterInTrades;
extern bool hitSelectDisableDuringKB;
extern bool hitSelectOnlyWhileDamaged;
extern bool hitSelectUseServerAttackTime;
extern bool hitSelectFakeSwing;
extern int hitSelectCancelRateInCombat;
extern int hitSelectCancelRateMissed;
extern bool hitSelectSwordOnly;

void InitializeHitSelect();
void UpdateHitSelect();
void RenderHitSelectUI();
void CleanupHitSelect();

void hitSelectThread();
