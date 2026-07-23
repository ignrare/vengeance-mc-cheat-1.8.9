#pragma once

#include "../../includes.h"

enum AimbotMode {
    AIMBOT_MODE_LOCKON = 0,
    AIMBOT_MODE_SINGLE = 1
};

enum SwitchCriteria {
    SWITCH_DISTANCE = 0,
    SWITCH_FOV_TIME = 1
};

enum ActivationCondition {
    ACTIVATION_ALWAYS = 0,
    ACTIVATION_CLICK_ONLY = 1,
    ACTIVATION_MOUSE_MOVEMENT = 2,
    ACTIVATION_SPRINTING = 3
};

extern bool aimbotEnabled;
extern bool aimbotKeyMode;
extern bool aimbotKeyPressed;
extern int aimbotKey;
extern bool aimNearest;
extern float aimDistance;
extern int aimSpeedInt;
extern float aimHeightFactor;
extern float aimAngleDeg;
extern bool aimTestEnabled;

extern int aimAssistMode;
extern bool aimLockOnActive;
extern float aimLockedWorldX, aimLockedWorldY, aimLockedWorldZ;
extern int aimLockLostFrames;
extern int aimActivation;
extern bool aimDisableInLiquid;
extern bool aimVerticalAim;
extern int aimSwitchCriteria;
extern float aimSwitchTime;
extern float aimLastSwitchTime;
extern bool aimbotSwordOnly;

void InitializeAimbot();
void UpdateAimbot();
void RenderAimbotUI();
void CleanupAimbot();

bool IsPlayerInLiquid();
bool IsPlayerSprinting();
bool HasMouseMovement();
void LockOnTarget(int targetId);
void ReleaseLock();
int FindBestTarget();
bool ShouldSwitchTarget(int currentTarget, int potentialTarget);
