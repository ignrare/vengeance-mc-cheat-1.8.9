#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "../SDK/Esp/Position.h"
#include "../SDK/Utils/Math.h"
#include "../imgui/imgui.h"
#include <Windows.h>
#include <gl/gl.h>
#include <jni.h>
#include <map>
#include <string>
#include <vector>

struct ArmorSlotData {
  bool equipped;
  int durability;
  int maxDurability;
  char displayName[32];
};

struct NTEntityData {
  float health;
  float maxHealth;
  int armor;
  char name[32];
  char weapon[24];
  ArmorSlotData armorSlots[4];
};

struct ESPOverlayData {
  float boxLeft, boxRight, boxTop, boxBottom;
  float hpFrac;
  float hp, maxHp;
  int armor;
  char name[32];
  char weapon[24];
  ArmorSlotData armorSlots[4];
  bool valid;
};

struct EntityRefData {
  jobject entityRef;
  bool valid;
};

struct ModuleInfo {
  const char *name;
  const char *desc;
};

struct CategoryInfo {
  const char *name;
  std::vector<ModuleInfo> modules;
};

struct KeyBind {
  int virtualKey;
  const char *name;
  bool *targetVariable;
  bool isWaitingForKey;
  std::string displayName;
};

struct NetworkNode {
  float x, y, vx, vy, sz;
};

typedef void(__stdcall *T_glEnable)(GLenum);
typedef void(__stdcall *T_glDisable)(GLenum);
typedef void(__stdcall *T_glOrtho)(GLdouble, GLdouble, GLdouble, GLdouble,
                                   GLdouble, GLdouble);
typedef void(__stdcall *T_glScalef)(GLfloat, GLfloat, GLfloat);
typedef void(__stdcall *T_glTranslatef)(GLfloat, GLfloat, GLfloat);
typedef BOOL(__stdcall *T_SwapBuffers)(HDC);

extern GL::Font glFont;
extern HHOOK keyHookHandle;

extern T_glEnable pglEnable;
extern T_glDisable pglDisable;
extern T_glOrtho pglOrtho;
extern T_glScalef pglScalef;
extern T_glTranslatef pglTranslatef;
extern T_SwapBuffers pSwapBuffers;

extern Vector2D screenCenter;
extern std::vector<Vector2D> entityScreenPositions;

extern bool showMenu;
extern bool espEnabled;
extern bool xrayEnabled;
extern bool chestEspEnabled;
extern bool tracerEnabled;
extern bool chamsEnabled;
extern bool nametagsEnabled;
extern bool trajectoriesEnabled;
extern bool arraylistEnabled;
extern bool wallhacks;

extern bool sInventory;
extern bool sChest;
extern bool sPlayer;
extern bool sNickname;
extern bool sChamsActive;

extern Position entity;
extern Position players;
extern Position chest;
extern Position largeChest;

extern ImVec4 espOutlineColor;
extern ImVec4 espFillColor;
extern ImVec4 chestOutlineColor;
extern ImVec4 chestFillColor;
extern ImVec4 tracerColor;
extern float espOutlineWidth;
extern bool espShowFill;
extern bool espShowOutline;
extern int espBoxStyle;
extern float espCornerLength;
extern bool espShowDistance;
extern ImVec4 espDistanceColor;
extern bool espShowName;
extern bool espShowHealthBar;
extern bool espShowArmor;
extern float espInfoScale;
extern float tracerWidth;
extern int tracerOrigin;
extern bool tracerShowDistance;
extern ImVec4 tracerDistanceColor;
extern float chestOutlineWidth;
extern bool chestShowFill;
extern bool chestShowOutline;
extern bool chestShowTracers;
extern ImVec4 chestTracerColor;
extern float chestTracerWidth;
extern float xrayOpacity;
extern bool xrayShowOres;
extern bool xrayShowChests;
extern bool xrayShowSpawners;

extern ImVec4 g_currentOutlineColor;
extern ImVec4 g_currentFillColor;
extern float g_currentOutlineWidth;
extern bool g_currentShowFill;
extern bool g_currentShowOutline;
extern int g_currentBoxStyle;
extern float g_currentCornerLength;

extern bool aimTestEnabled;
extern bool aimbotEnabled;
extern bool aimbotKeyMode;
extern bool aimbotKeyPressed;
extern int aimbotKey;
extern bool aimNearest;
extern float aimDistance;
extern int aimSpeedInt;
extern float aimHeightFactor;
extern float aimVerticalSpeed;
extern float aimAngleDeg;
extern int aimAssistMode;
extern bool aimLockOnActive;
extern bool aimLockOnTarget;
extern float aimLockedWorldX, aimLockedWorldY, aimLockedWorldZ;
extern int aimLockLostFrames;
extern int aimActivation;
extern bool aimDisableInLiquid;
extern bool aimVerticalAim;
extern int aimSwitchCriteria;
extern float aimSwitchTime;
extern float aimLastSwitchTime;
extern float aimLastMouseX;
extern float aimLastMouseY;
extern LARGE_INTEGER aimLastFrameTime;
extern bool aimSmoothingActive;
extern double aimPrevTargetX, aimPrevTargetY, aimPrevTargetZ;
extern float g_f5SelfViewZ;
extern float g_f5SelfViewX;
extern jobject g_aimLockedEntity;
extern DWORD g_aimLockLostTick;
extern bool aimbotSwordOnly;

extern bool clickerEnabled;
extern double currentCps;
extern int totalClicks;
extern int clickerActivationMode;
extern int clickerActivationKey;
extern bool clickerActivationKeyHeld;
extern bool clickerLeftClick;
extern bool clickerAllowInInventory;
extern int clickerCpsInt;
extern bool clickerEnableRandomization;
extern double clickerRandomizationAmount;
extern bool clickerExhaustMode;
extern int clickerExhaustChance;
extern double clickerExhaustDropCps;
extern bool clickerSpikeMode;
extern int clickerSpikeChance;
extern double clickerSpikeIncreaseCps;
extern int clickerHotkeyCode;
extern bool g_capturingClickerKey;
extern LARGE_INTEGER clickerFrequency;
extern LARGE_INTEGER clickerLastTime;
extern double clickerAccumulatedError;
extern bool clickerSwordOnly;
extern const char *clickerThreadStatus;

extern bool rightClickerEnabled;
extern double rightClickerCurrentCps;
extern int rightClickerTotalClicks;
extern int rightClickerActivationMode;
extern int rightClickerActivationKey;
extern bool rightClickerActivationKeyHeld;
extern bool rightClickerAllowInInventory;
extern int rightClickerCpsInt;
extern bool rightClickerEnableRandomization;
extern double rightClickerRandomizationAmount;
extern bool rightClickerBlatant;
extern bool g_capturingRightClickerKey;
extern std::string rightClickerThreadStatus;

extern bool fastPlaceEnabled;
extern int fastPlaceTickDelay;
extern jfieldID g_fRightClickDelayTimer;
extern bool g_fastPlaceJniReady;

extern bool blockhitEnabled;
extern bool blockhitOnlyWhileClicking;
extern int blockhitDelayMin;
extern int blockhitDelayMax;
extern int blockhitBlockChance;
extern int blockhitHoldLengthMin;
extern int blockhitHoldLengthMax;
extern bool blockhitSwordOnly;
extern const char *blockhitThreadStatus;

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
extern const char *hitSelectStatus;
extern bool hitSelectSwordOnly;

extern volatile int g_hsTargetHurtResist;
extern volatile int g_hsTargetHurtTime;
extern volatile bool g_hsAimingAtEntity;
extern volatile bool g_hsPlayerOnGround;
extern volatile double g_hsMotionY;
extern volatile bool g_hsTakingKB;
extern volatile bool g_hsNeedFakeSwing;
extern volatile bool g_hsFirstHitReceived;
extern volatile DWORD g_hsLastDamageTime;
extern volatile DWORD g_hsCombatStartTime;
extern volatile DWORD g_hsLastAttackTime;
extern volatile DWORD g_hsLastAimTime;
extern volatile bool g_hsTargetJustDamaged;
extern volatile bool g_sprintPending;
extern volatile bool g_holdingSword;

extern bool reachEnabled;
extern float reachMin;
extern float reachMax;
extern bool reachSprintOnly;
extern bool reachAttackOnly;
extern bool reachDisableInLiquid;
extern bool reachOnlyMoving;
extern bool reachWhileJumping;
extern const char *reachStatus;
extern bool reachSwordOnly;

extern bool velocityEnabled;
extern float velocityHorizontal;
extern float velocityVertical;
extern float velocityChance;
extern bool velocityOnlyWhileMoving;
extern bool velocityOnlyOnGround;
extern bool g_velocityJniReady;
extern const char *velocityStatus;

extern bool sprintEnabled;
extern bool g_sprintJniReady;

extern bool sprintResetEnabled;
extern int sprintResetMode;
extern int sprintResetDelay;
extern int sprintResetStopDuration;
extern bool sprintResetRandomize;
extern bool sprintResetWaitForDamage;
extern bool sprintResetWeaponOnly;
extern volatile bool g_sprintResetPending;
extern volatile DWORD g_lastAttackTime;

extern bool speedEnabled;
extern float speedMultiplier;
extern bool flyEnabled;
extern float flySpeed;
extern bool noFallEnabled;
extern bool invWalkEnabled;
extern int invWalkMode;
extern bool g_invWalkJniReady;
extern bool g_noFallJniReady;
extern bool jumpResetEnabled;
extern int jumpResetChance;
extern bool jumpResetOnlyWhenHurt;
extern bool noJumpDelayEnabled;
extern bool scaffoldEnabled;
extern bool scaffoldTower;
extern bool scaffoldRotate;
extern bool scaffoldLegitMode;
extern bool scaffoldAutoBlock;
extern int scaffoldDelayMs;
extern float scaffoldMinDist;
extern bool stepEnabled;
extern int stepMode;
extern float stepHeight;
extern bool stepReverseStep;

extern float nametagsSize;
extern bool nametagsShowHealth;
extern bool nametagsShowArmor;
extern bool nametagsShowWeapon;

extern float trajectoriesLineWidth;
extern int trajectoriesMaxTicks;
extern bool trajectoriesLandingCross;

extern bool throwpotEnabled;
extern int throwpotTriggerKey;
extern int throwpotPotCount;
extern int throwpotSwitchDelay;
extern int throwpotThrowDelay;
extern int throwpotCooldownMs;
extern bool throwpotSwordOnly;
extern bool g_throwpotKeyHeld;
extern DWORD g_throwpotLastThrow;
extern const char *throwpotStatus;

extern bool autosoupEnabled;
extern int autosoupTriggerKey;
extern int autosoupSwitchDelay;
extern int autosoupEatDelay;
extern int autosoupCooldownMs;
extern bool autosoupDropBowls;
extern bool autosoupSwordOnly;
extern bool g_autosoupKeyHeld;
extern DWORD g_autosoupLastEat;
extern const char *autosoupStatus;

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
extern bool g_refillKeyHeld;
extern const char *refillStatus;

extern bool autoToolEnabled;
extern int autoToolActivationDelay;
extern bool autoToolSwitchBack;
extern bool autoToolAllowSword;
extern bool autoToolAllowTool;
extern bool autoToolAllowFists;
extern bool autoToolAllowOther;
extern bool autoToolSneakOnly;

extern bool noPacketEnabled;
extern bool pingSpoofEnabled;
extern int pingSpoofAmount;

extern float arraylistFontScale;
extern float arraylistBgAlpha;
extern float arraylistBarWidth;
extern bool arraylistShowInfo;
extern bool arraylistRainbow;
extern float arraylistRainbowSpeed;
extern ImVec4 arraylistAccentColor;
extern ImVec4 arraylistTextColor;
extern ImVec4 arraylistInfoColor;
extern int arraylistPosition;
extern bool arraylistBackground;
extern bool arraylistAccentBar;
extern bool arraylistRoundedCorners;
extern bool arraylistWatermark;
extern bool arraylistWmShowSettings;
extern bool arraylistWmShowName;
extern bool arraylistWmShowVersion;
extern bool arraylistWmShowFps;
extern bool arraylistWmShowKeybinds;
extern float arraylistWmFontScale;
extern int arraylistWmPosition;
extern ImVec4 arraylistWmTextColor;
extern ImVec4 arraylistWmBgColor;
extern ImVec4 arraylistWmBorderColor;
extern const char *g_clientName;
extern const char *g_clientVersion;

extern std::vector<NTEntityData> g_ntEntityData;
extern std::vector<NTEntityData> g_ntEntityDataReady;
extern std::vector<ESPOverlayData> g_espOverlays;
extern std::vector<ESPOverlayData> g_espOverlaysReady;
extern std::vector<EntityRefData> g_playerEntityRefs;
extern std::vector<EntityRefData> g_playerEntityRefsReady;

extern volatile int g_trajHeldItemType;
extern volatile float g_trajYaw, g_trajPitch;
extern volatile float g_trajEyeX, g_trajEyeY, g_trajEyeZ;
extern volatile float g_trajBowPower;
extern float g_camProjection[16];
extern float g_camModelview[16];
extern bool g_camMatricesValid;

extern float g_menuFadeAnim;
extern float g_menuScaleAnim;
extern int g_lastCategory;
extern float g_cardAppearTimer;
extern bool g_themeApplied;
extern std::vector<NetworkNode> g_netNodes;
extern bool g_nodesInit;
extern float g_globalTime;
extern float g_breathPhase;
extern std::map<int, float> g_cardBounce;
extern std::map<int, float> g_cardHoverGlow;
extern std::map<int, float> g_cardPressAnim;
extern std::map<int, float> g_cardRippleAnim;
extern std::map<int, ImVec2> g_cardRipplePos;
extern std::map<std::string, float> g_elementAppear;
extern std::map<int, float> g_iconAppearAnim;
extern std::map<int, float> g_dotEnabledAnim;
extern std::map<std::string, float> g_subSettingsAnim;

extern bool showCombatWindow;
extern bool showVisualsWindow;
extern bool showMovementWindow;
extern bool showNetworkWindow;
extern bool showUtilitiesWindow;
extern bool showSettingsWindow;

extern std::vector<std::string> enabledModules;

extern std::vector<CategoryInfo> g_categories;
extern int g_selectedCategory;
extern int g_openModuleCat;
extern int g_openModuleIdx;
extern float g_settingsPanelAnim;
extern float g_tabAnims[6];

extern std::map<ImGuiID, float> g_animState;

extern std::vector<KeyBind> g_keybinds;
extern bool g_isCapturingKey;
extern int g_capturingIndex;

extern bool g_unloadRequested;
extern bool g_isUnloading;
extern HMODULE g_hModule;
extern HANDLE g_keyDispatcherThread;
extern DWORD g_keyDispatcherThreadId;
extern HANDLE g_moduleThreads[16];
extern int g_moduleThreadCount;
extern CRITICAL_SECTION g_unloadCS;
extern bool g_unloadCSInitialized;
extern bool g_threadsSpawned;

extern bool g_imguiInitialized;
extern HWND g_imguiHwnd;
extern bool g_wndProcHooked;
extern WNDPROC g_originalWndProc;
extern HWND g_targetWindow;
extern bool g_fullscreenDetected;
extern RECT g_lastWindowRect;
extern bool g_wasFullscreen;
extern bool g_cursorCaptured;
extern volatile DWORD g_renderFrameIndex;
extern POINT g_lastCursorPos;
extern bool g_blockMouseMovement;
extern bool g_cursorVisible;
extern DWORD g_lastEKeyTime;
extern DWORD g_lastEscKeyTime;
extern bool g_isInventoryOpen;

extern JavaVM *g_jvm;
extern bool g_jniReady;
extern jclass g_mcClass;
extern jmethodID g_getMinecraft;
extern jfieldID g_fThePlayer;
extern jfieldID g_fTheWorld;
extern jfieldID g_fPlayerController;
extern jclass g_entityClass;
extern jfieldID g_fPosX;
extern jfieldID g_fPosY;
extern jfieldID g_fPosZ;
extern jfieldID g_fIsDead;
extern jfieldID g_fRotYaw;
extern jfieldID g_fRotPitch;
extern jfieldID g_fPrevRotYaw;
extern jfieldID g_fPrevRotPitch;
extern jfieldID g_fRotYawHead;
extern jfieldID g_fRotPitchHead;
extern jfieldID g_fRenderYawOffset;
extern jmethodID g_mGetEyeHeight;
extern jclass g_worldClass;
extern jfieldID g_fEntityList;
extern jmethodID g_mListSize;
extern jmethodID g_mListGet;
extern jclass g_pcmpClass;
extern jmethodID g_mAttackEntity;
extern jfieldID g_fBlockReach;
extern jmethodID g_mSwingItem;

extern jfieldID g_fObjectMouseOver;
extern jfieldID g_fEntityHit;
extern jfieldID g_fHurtResistantTime;
extern jfieldID g_fHurtTime;

extern jmethodID g_mGetCurrentItem;
extern jmethodID g_mStackGetItem;
extern jmethodID g_mStackGetDisplayName;
extern jclass g_itemSwordClass;
extern bool g_swordCheckReady;

extern jfieldID g_fHealth;
extern jfieldID g_fInventory;
extern jfieldID g_fCurrentItemIdx;
extern jmethodID g_mSendUseItem;
extern jmethodID g_mGetStackInSlot;
extern jmethodID g_mDropOneItem;
extern jmethodID g_mDisplayGuiScreen;
extern jmethodID g_mCloseScreen;
extern jclass g_itemPotionClass;
extern jmethodID g_mIsSplash;
extern jclass g_inventoryClass;
extern jclass g_itemClass;

extern jfieldID g_fMotionX;
extern jfieldID g_fMotionY;
extern jfieldID g_fMotionZ;
extern jfieldID g_fOnGround;

extern jobject g_mcClassLoader;
extern jmethodID g_loadClassMethod;

extern jfieldID g_fIsSprinting;
extern jmethodID g_mSetSprinting;

extern jfieldID g_fGameSettings;
extern jfieldID g_fKeyBindForward;
extern jfieldID g_fKeyBindBack;
extern jfieldID g_fKeyBindLeft;
extern jfieldID g_fKeyBindRight;
extern jfieldID g_fKeyBindJump;
extern jfieldID g_fKeyBindSneak;
extern jfieldID g_fKeyBindPressed;

extern jfieldID g_fFallDistance;
extern jfieldID g_fSendQueue;
extern jmethodID g_mAddToSendQueue;
extern jclass g_c03PacketClass;
extern jmethodID g_c03PacketInit;

extern jclass g_itemBowClass;
extern jclass g_itemEnderPearlClass;
extern jclass g_itemSnowballClass;
extern jclass g_itemEggClass;

extern jfieldID g_fJumpTicks;
extern jfieldID g_fStepHeight;
extern jclass g_c03PosClass;
extern jmethodID g_c03PosInit;
extern jclass g_itemBlockClass;

extern jmethodID g_mAddScheduledTask;
extern jclass g_callableClass;

extern jmethodID g_mRightClickMouse;
extern jmethodID g_mOnPlayerRightClick;
extern jclass g_vec3Class;
extern jmethodID g_vec3Init;
extern jclass g_enumFacingClass;
extern jobject g_enumFacingByIndex[6];

extern jmethodID g_mGetStrVsBlock;
extern jmethodID g_mGetBlockState;
extern jclass g_blockPosClass;
extern jmethodID g_blockPosInit;
extern jmethodID g_mGetBlock;
extern jmethodID g_mIsAirBlock;
extern jfieldID g_fIsSneaking;
extern jmethodID g_mIsSneaking;
extern jmethodID g_mGetHealth;
extern jmethodID g_mGetMaxHealth;
extern jclass g_entityLivingClass;
extern jclass g_entityPlayerClass;
extern jmethodID g_mGetName;
extern jmethodID g_mGetArmorValue;
extern jmethodID g_mGetItemInUseDuration;
extern jmethodID g_mGetEquipmentInSlot;
extern jmethodID g_mGetMaxDamage;
extern jmethodID g_mGetItemDamage;
