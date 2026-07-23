

#pragma once

#include "../../includes.h"

enum RefillMode {
    REFILL_MODE_ON_OPEN_INVENTORY = 0,
    REFILL_MODE_AUTOMATICALLY_OPEN = 1
};

enum RefillMouseButton {
    REFILL_MOUSE_LEFT = 0,
    REFILL_MOUSE_RIGHT = 1,
    REFILL_MOUSE_LEFT_RIGHT = 2
};

extern bool refillEnabled;
extern int refillMode;
extern int refillMouseButton;
extern int refillTriggerKey;
extern bool refillSoup;
extern bool refillPotion;
extern bool refillUseNonHealthPotions;
extern int refillDelayAfterOpen;
extern int refillDelayBeforeClose;
extern int refillSpeed;
extern bool refillSmartSpeed;
extern bool refillCloseOnComplete;
extern bool refillDisableOnComplete;
extern bool refillRandomSlots;

void InitializeRefill();
void UpdateRefill();
void RenderRefillUI();
void CleanupRefill();
