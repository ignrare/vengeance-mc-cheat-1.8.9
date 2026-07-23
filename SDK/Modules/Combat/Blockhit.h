#pragma once

#include "../../includes.h"

extern bool blockhitEnabled;
extern bool blockhitOnlyWhileClicking;
extern int blockhitDelayMin;
extern int blockhitDelayMax;
extern int blockhitBlockChance;
extern int blockhitHoldLengthMin;
extern int blockhitHoldLengthMax;
extern bool blockhitSwordOnly;

void InitializeBlockhit();
void UpdateBlockhit();
void RenderBlockhitUI();
void CleanupBlockhit();

void blockhitThread();
