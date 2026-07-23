#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "Esp/Color.h"
#include "Esp/Position.h"
#include "Esp/Radius.h"
#include "GlDraw/glDraw.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include "imgui_impl_win32.h"
#include "includes.h"
#include "Modules/Utility/Refill.h"
#include <Windows.h>
#include <winhttp.h>
#include <jni.h>
#include <commdlg.h>
using byte = unsigned char;
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "Esp/img/stb_image.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <functional>
#include <map>
#include <random>
#include <string>
#include <vector>

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct KeyBind;
extern std::vector<KeyBind> g_keybinds;
extern bool g_isCapturingKey;
extern int g_capturingIndex;

bool CustomCheckbox(const char *label, bool *value);
bool CustomSlider(const char *label, float *value, float min, float max,
                  const char *format);
void SectionHeader(const char *text);
void StartKeyCapture(int index);
void StopKeyCapture();
void UpdateKeyCapture(int key);

GL::Font glFont;
HHOOK keyHookHandle = nullptr;

struct Vector2D {
  float x, y;
  Vector2D(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Vector3D {
  float x, y, z;
  Vector3D(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

const unsigned char COLOR_WHITE[3] = {255, 255, 255};
const unsigned char COLOR_GRAY[3] = {128, 128, 128};
const unsigned char COLOR_GREEN[3] = {0, 255, 0};
const unsigned char COLOR_RED[3] = {255, 0, 0};
const unsigned char COLOR_YELLOW[3] = {255, 255, 0};
const unsigned char COLOR_MERO[3] = {138, 43, 226};

ImVec4 espOutlineColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
ImVec4 espFillColor = ImVec4(1.0f, 0.2f, 0.2f, 0.20f);
ImVec4 chestOutlineColor = ImVec4(1.0f, 0.7f, 0.2f, 1.0f);
ImVec4 chestFillColor = ImVec4(1.0f, 0.7f, 0.2f, 0.20f);
ImVec4 tracerColor = ImVec4(1.0f, 1.0f, 1.0f, 0.30f);

float espOutlineWidth = 2.0f;
bool espShowFill = true;
bool espShowOutline = true;
int espBoxStyle = 0;
float espCornerLength = 0.25f;
bool espShowDistance = false;
ImVec4 espDistanceColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
bool espShowName = true;
bool espShowHealthBar = true;
bool espShowArmor = true;
float espInfoScale = 0.75f;

bool espUseImage = false;
bool espImageStretch = true;
char espImagePath[256] = "";
unsigned int espImageTexture = 0;
int espImageWidth = 0;
int espImageHeight = 0;
bool espImageLoaded = false;

float tracerWidth = 1.5f;
int tracerOrigin = 0;
bool tracerShowDistance = false;
ImVec4 tracerDistanceColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

float chestOutlineWidth = 2.0f;
bool chestShowFill = true;
bool chestShowOutline = true;
bool chestShowTracers = false;
ImVec4 chestTracerColor = ImVec4(1.0f, 0.7f, 0.2f, 0.30f);
float chestTracerWidth = 1.5f;

float xrayOpacity = 1.0f;
bool xrayShowOres = true;
bool xrayShowChests = true;
bool xrayShowSpawners = true;

static ImVec4 g_currentOutlineColor = espOutlineColor;
static ImVec4 g_currentFillColor = espFillColor;
static float g_currentOutlineWidth = 2.0f;
static bool g_currentShowFill = true;
static bool g_currentShowOutline = true;
static int g_currentBoxStyle = 0;
static float g_currentCornerLength = 0.25f;

bool showMenu = true;
bool espEnabled = false;
bool xrayEnabled = false;
bool chestEspEnabled = false;
bool tracerEnabled = false;
bool aimTestEnabled = false;
bool aimbotEnabled = false;
bool aimbotKeyMode = false;
bool aimbotKeyPressed = false;
int aimbotKey = VK_LBUTTON;
bool aimNearest = true;
float aimDistance = 6.0f;
int aimSpeedInt = 30;
float aimHeightFactor = 0.5f;
float aimVerticalSpeed = 30.0f;
float aimAngleDeg = 60.0f;

int aimAssistMode = 0;
bool aimLockOnActive = false;
bool aimLockOnTarget =
    false;
float aimLockedWorldX = 0.0f, aimLockedWorldY = 0.0f, aimLockedWorldZ = 0.0f;
int aimLockLostFrames = 0;
int aimActivation =
    0;
bool aimDisableInLiquid = false;
bool aimVerticalAim = false;
int aimSwitchCriteria = 0;
float aimSwitchTime = 2.0f;
float aimLastSwitchTime = 0.0f;
static float aimLastMouseX = 0.0f;
static float aimLastMouseY = 0.0f;

static LARGE_INTEGER aimLastFrameTime = {0};
static bool aimSmoothingActive = false;
static double aimPrevTargetX = 0.0, aimPrevTargetY = 0.0, aimPrevTargetZ = 0.0;
float g_f5SelfViewZ = -999.0f;
float g_f5SelfViewX = -999.0f;

static jobject g_aimLockedEntity =
    nullptr;
static DWORD g_aimLockLostTick = 0;

static const char *clickerThreadStatus = "Not started";
static const char *blockhitThreadStatus = "Not started";
static const char* refillStatus = "Not started";
static bool g_threadsSpawned = false;
static bool g_refillKeyHeld = false;

bool clickerEnabled = false;
double currentCps = 0.0;
int totalClicks = 0;
int clickerActivationMode = 2;
int clickerActivationKey = 0;
bool clickerActivationKeyHeld = false;
bool clickerLeftClick = true;
bool clickerAllowInInventory = false;
int clickerCpsInt = 10;
bool clickerEnableRandomization = false;
double clickerRandomizationAmount = 2.0;
bool clickerExhaustMode = false;
int clickerExhaustChance = 5;
double clickerExhaustDropCps = 2.0;
bool clickerSpikeMode = false;
int clickerSpikeChance = 3;
double clickerSpikeIncreaseCps = 5.0;
int clickerHotkeyCode = VK_F6;
bool g_capturingClickerKey = false;

LARGE_INTEGER clickerFrequency = {0};
LARGE_INTEGER clickerLastTime = {0};
double clickerAccumulatedError = 0.0;

bool rightClickerEnabled = false;
double rightClickerCurrentCps = 0.0;
int rightClickerTotalClicks = 0;
int rightClickerActivationMode = 2;
int rightClickerActivationKey = 0;
bool rightClickerActivationKeyHeld = false;
bool rightClickerAllowInInventory = false;
int rightClickerCpsInt = 12;
bool rightClickerEnableRandomization = true;
double rightClickerRandomizationAmount = 2.0;
bool rightClickerBlatant = false;
bool g_capturingRightClickerKey = false;
std::string rightClickerThreadStatus = "Not started";

bool fastPlaceEnabled = false;
int fastPlaceTickDelay = 0;
static jfieldID g_fRightClickDelayTimer = nullptr;
static bool g_fastPlaceJniReady = false;

bool blockhitEnabled = false;
bool blockhitOnlyWhileClicking = true;
int blockhitDelayMin = 100;
int blockhitDelayMax = 200;
int blockhitBlockChance = 80;
int blockhitHoldLengthMin = 50;
int blockhitHoldLengthMax = 100;

bool hitSelectEnabled = false;
int hitSelectMode = 0;
int hitSelectPauseDuration = 500;
int hitSelectWaitForFirstHit = 0;
int hitSelectHitLaterInTrades = 0;
bool hitSelectDisableDuringKB = false;
bool hitSelectOnlyWhileDamaged = false;
bool hitSelectUseServerAttackTime = false;
bool hitSelectFakeSwing = true;
int hitSelectCancelRateInCombat = 80;
int hitSelectCancelRateMissed = 50;
static const char *hitSelectStatus = "Not started";

static volatile int g_hsTargetHurtResist = 0;
static volatile int g_hsTargetHurtTime = 0;
static volatile bool g_hsAimingAtEntity = false;
static volatile bool g_hsPlayerOnGround = true;
static volatile double g_hsMotionY = 0.0;
static volatile bool g_hsTakingKB = false;
static volatile bool g_hsNeedFakeSwing = false;
static volatile bool g_hsFirstHitReceived = false;
static volatile DWORD g_hsLastDamageTime = 0;
static volatile DWORD g_hsCombatStartTime = 0;
static volatile DWORD g_hsLastAttackTime = 0;
static volatile DWORD g_hsLastAimTime = 0;
static volatile bool g_hsTargetJustDamaged = false;
volatile bool g_sprintPending = false;

static volatile bool g_holdingSword = false;

bool clickerSwordOnly = false;
bool aimbotSwordOnly = false;
bool blockhitSwordOnly = false;
bool reachSwordOnly = false;
bool hitSelectSwordOnly = false;

bool reachEnabled = false;
float reachMin = 3.0f;
float reachMax = 3.5f;
bool reachSprintOnly = false;
bool reachAttackOnly = true;
bool reachDisableInLiquid = false;
bool reachOnlyMoving = false;
bool reachWhileJumping = false;
int reachChance = 100;
bool reachMotionCompensation = true;

static double g_reachCurrent = 3.0;
static double g_reachTarget = 3.0;
static double g_reachVelocity = 0.0;
static LARGE_INTEGER g_reachLastTime = {};
static bool g_reachHasSpareGauss = false;
static double g_reachSpareGauss = 0.0;
static std::mt19937 g_reachRng{std::random_device{}()};

static double ReachNextGaussian() {
    if (g_reachHasSpareGauss) {
        g_reachHasSpareGauss = false;
        return g_reachSpareGauss;
    }
    double u, v, s;
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    do {
        u = dist(g_reachRng);
        v = dist(g_reachRng);
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);
    double mul = std::sqrt(-2.0 * std::log(s) / s);
    g_reachSpareGauss = v * mul;
    g_reachHasSpareGauss = true;
    return u * mul;
}

#include "combat_modules.h"

#define sTapConfig g_sTapModule.config
#define wTapConfig g_wTapModule.config
#define bowAimbotConfig g_bowAimbotModule.config

static bool sTapActive = false;
static bool wTapActive = false;

static const char *reachStatus = "Not initialized";
static const char *stepStatus = "Not initialized";

static jmethodID g_mAddScheduledTask =
    nullptr;
static jclass g_callableClass = nullptr;

JavaVM *g_jvm = nullptr;
bool g_jniReady = false;
jclass g_mcClass = nullptr;
jmethodID g_getMinecraft = nullptr;
jfieldID g_fThePlayer = nullptr;
jfieldID g_fTheWorld = nullptr;
jfieldID g_fPlayerController = nullptr;
jclass g_entityClass = nullptr;
jfieldID g_fPosX = nullptr;
jfieldID g_fPosY = nullptr;
jfieldID g_fPosZ = nullptr;
jfieldID g_fIsDead = nullptr;
jfieldID g_fRotYaw = nullptr;
jfieldID g_fRotPitch = nullptr;
jfieldID g_fPrevRotYaw = nullptr;
jfieldID g_fPrevRotPitch = nullptr;
jfieldID g_fRotYawHead = nullptr;
jfieldID g_fRotPitchHead = nullptr;
jfieldID g_fRenderYawOffset = nullptr;
jmethodID g_mGetEyeHeight = nullptr;
jclass g_worldClass = nullptr;
jfieldID g_fEntityList = nullptr;
jmethodID g_mListSize = nullptr;
jmethodID g_mListGet = nullptr;
jclass g_pcmpClass = nullptr;
jmethodID g_mAttackEntity = nullptr;
jfieldID g_fBlockReach = nullptr;
jmethodID g_mSwingItem = nullptr;

static jfieldID g_fObjectMouseOver = nullptr;
static jfieldID g_fEntityHit = nullptr;
static jfieldID g_fHurtResistantTime = nullptr;
static jfieldID g_fHurtTime = nullptr;

jmethodID g_mGetCurrentItem = nullptr;
jmethodID g_mStackGetItem = nullptr;
jmethodID g_mStackGetDisplayName = nullptr;
jclass g_itemSwordClass = nullptr;
static bool g_swordCheckReady = false;

bool throwpotEnabled = false;
int throwpotTriggerKey = VK_LSHIFT;
int throwpotPotCount = 2;
int throwpotSwitchDelay = 50;
int throwpotThrowDelay = 50;
int throwpotCooldownMs = 300;
bool throwpotSwordOnly = false;
static bool g_throwpotKeyHeld = false;
static DWORD g_throwpotLastThrow = 0;
static const char *throwpotStatus = "Not started";

static jfieldID g_fHealth = nullptr;
static jfieldID g_fInventory = nullptr;
static jfieldID g_fCurrentItemIdx = nullptr;
static jmethodID g_mSendUseItem = nullptr;
static jmethodID g_mGetStackInSlot =
    nullptr;
static jmethodID g_mDropOneItem = nullptr;
static jmethodID g_mDisplayGuiScreen = nullptr;
static jmethodID g_mCloseScreen = nullptr;
static jclass g_itemPotionClass = nullptr;
static jmethodID g_mIsSplash =
    nullptr;
static jclass g_inventoryClass = nullptr;
static jclass g_itemClass = nullptr;

bool autosoupEnabled = false;
int autosoupTriggerKey = VK_LCONTROL;
int autosoupSwitchDelay = 50;
int autosoupEatDelay = 50;
int autosoupCooldownMs = 200;
bool autosoupDropBowls = true;
bool autosoupSwordOnly = false;
static bool g_autosoupKeyHeld = false;
static DWORD g_autosoupLastEat = 0;
static const char *autosoupStatus = "Not started";

static jfieldID g_fMotionX = nullptr;
static jfieldID g_fMotionY = nullptr;
static jfieldID g_fMotionZ = nullptr;
static jfieldID g_fOnGround = nullptr;
static bool g_velocityJniReady = false;
static const char *velocityStatus = "Not initialized";

static jobject g_mcClassLoader = nullptr;
static jmethodID g_loadClassMethod = nullptr;

static jfieldID g_fIsSprinting = nullptr;

bool speedEnabled = false;
float speedMultiplier = 1.5f;
bool flyEnabled = false;
float flySpeed = 1.0f;
bool noFallEnabled = false;

bool sprintResetEnabled = false;
int sprintResetMode = 0;
int sprintResetDelay = 250;
int sprintResetStopDuration = 50;
bool sprintResetRandomize = true;
bool sprintResetWaitForDamage = false;
bool sprintResetWeaponOnly = false;
static volatile bool g_sprintResetPending = false;
static volatile DWORD g_lastAttackTime = 0;

bool invWalkEnabled = false;
int invWalkMode = 0;

static jfieldID g_fGameSettings = nullptr;
static jfieldID g_fKeyBindForward = nullptr;
static jfieldID g_fKeyBindBack = nullptr;
static jfieldID g_fKeyBindLeft = nullptr;
static jfieldID g_fKeyBindRight = nullptr;
static jfieldID g_fKeyBindJump = nullptr;
static jfieldID g_fKeyBindSneak = nullptr;
static jfieldID g_fKeyBindSprint = nullptr;
static jfieldID g_fKeyBindPressed = nullptr;
static bool g_invWalkJniReady = false;

static jfieldID g_fFallDistance = nullptr;
static jfieldID g_fSendQueue = nullptr;
static jmethodID g_mAddToSendQueue = nullptr;
static jclass g_c03PacketClass = nullptr;
static jmethodID g_c03PacketInit = nullptr;
static bool g_noFallJniReady = false;

static jclass g_itemBowClass = nullptr;
static jclass g_itemEnderPearlClass = nullptr;
static jclass g_itemSnowballClass = nullptr;
static jclass g_itemEggClass = nullptr;

bool jumpResetEnabled = false;
int jumpResetChance = 100;
bool jumpResetOnlyWhenHurt = true;

bool noJumpDelayEnabled = false;

bool scaffoldEnabled = false;
bool scaffoldTower = false;
bool scaffoldLegitMode = false;
bool scaffoldAutoBlock = true;
int scaffoldDelayMs = 60;
float scaffoldMinDist = 0.0f;

static float g_scaffLegitYaw = 0.0f;
static float g_scaffLegitPitch = 0.0f;
static bool g_scaffLegitActive = false;
static float g_scaffSmHeadYaw = 0.0f;
static float g_scaffSmHeadPitch = 0.0f;

bool bridgeAssistEnabled = false;
bool bridgeAssistRmbOnly = false;
bool bridgeAssistBackwardOnly = false;
bool bridgeAssistCheckPitch = true;
bool bridgeAssistBlocksOnly = true;
int bridgeAssistDelay = 0;
float bridgeAssistPitchThreshold = 60.0f;

bool stepEnabled = false;
int stepMode = 0;
float stepHeight = 1.0f;
bool stepReverseStep = false;

bool chamsEnabled = false;

float targetHudScale = 1.0f;
bool targetHudShowHealth = true;
float targetHudX = 0.0f;
float targetHudY = 0.0f;
bool targetHudEnabled = false;

static float g_displayedHpFrac = 1.0f;
static float g_targetHpFrac = 1.0f;

static float g_targetHudLastAttackTime = 0.0f;
static float g_hudDisplayTime = 5.0f;

static char g_cachedTargetName[32] = "";
static float g_cachedHp = 20.0f;
static float g_cachedMaxHp = 20.0f;

void NotifyTargetHit(const char* name = nullptr, float hp = -1.f, float maxHp = -1.f) {
    g_targetHudLastAttackTime = (float)GetTickCount64() / 1000.0f;

    printf("[DEBUG] NotifyTargetHit called with: name='%s', hp=%.1f, maxHp=%.1f\n",
           name ? name : "null", hp, maxHp);

    if (name && name[0] != '\0') {
        strncpy_s(g_cachedTargetName, name, 31);
        printf("[DEBUG] Cached name: %s\n", g_cachedTargetName);
    }
    if (hp >= 0.f) {
        g_cachedHp = hp;
        printf("[DEBUG] Cached HP: %.1f\n", g_cachedHp);
    }
    if (maxHp > 0.f) {
        g_cachedMaxHp = maxHp;
        printf("[DEBUG] Cached Max HP: %.1f\n", g_cachedMaxHp);
    }
    if (maxHp > 0.f && hp >= 0.f) {
        g_targetHpFrac = hp / maxHp;
        printf("[DEBUG] Cached HP fraction: %.2f\n", g_targetHpFrac);
    }

    printf("[DEBUG] Target HUD will show for %.1f seconds\n", g_hudDisplayTime);
}

bool sTapEnabled = false;
int sTapDelay = 1;
bool wTapEnabled = false;
int wTapDelay = 1;
bool bowAimbotEnabled = false;
float bowAimbotFov = 90.0f;
bool bowAimbotPrediction = true;

bool spiderEnabled = false;
float spiderSpeed = 0.2f;
bool safeWalkEnabled = false;
bool reverseStepEnabled = false;
float reverseStepHeight = 1.0f;
bool bouncySlimeEnabled = false;
bool fastStopEnabled = false;
bool glideEnabled = false;
float glideSpeed = 0.04f;
bool flightEnabled = false;
float flightSpeed = 1.0f;
int flightMode = 0;
bool airJumpEnabled = false;
bool highJumpEnabled = false;
float highJumpMotion = 0.55f;

bool freecamEnabled = false;
float freecamSpeed = 1.0f;
bool fullBrightEnabled = false;
int fullBrightMode = 0;
bool noHurtCamEnabled = false;
bool zoomEnabled = false;
float zoomFov = 30.0f;
int zoomKey = 'C';
bool armorEspEnabled = false;
bool itemEspEnabled = false;
ImVec4 itemEspColor = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
bool storageEspEnabled = false;
bool holeEspEnabled = false;
ImVec4 holeEspSafeColor = ImVec4(0.0f, 1.0f, 0.0f, 0.5f);
ImVec4 holeEspUnsafeColor = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);
bool uiPresetsEnabled = false;
int uiPresetsSelected = 0;
bool g_useCustomUI = false;

#include "Modules/Appearance/UIPresets/UI_VapeV4.h"
#include "Modules/Appearance/UIPresets/UI_Slinky.h"

bool blinkEnabled = false;
bool timerEnabled = false;
float timerSpeed = 2.0f;
bool fakeLagEnabled = false;
int fakeLagDelay = 200;

bool teamsEnabled = false;
int teamsMode = 0;
bool friendsEnabled = false;
bool autoSwordEnabled = false;
bool antiVoidEnabled = false;
float antiVoidDistance = 5.0f;
bool middleClickFriendEnabled = false;
bool antiBotEnabled = false;
int antiBotMode = 0;
bool autoArmorEnabled = false;
bool inventoryManagerEnabled = false;
int invManagerDelay = 100;

bool breadCrumbsEnabled = false;
ImVec4 breadCrumbsColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
bool antiAfkEnabled = false;
int antiAfkMode = 0;
bool antiBadEffectsEnabled = false;
bool panicEnabled = false;
int panicKey = VK_F12;
bool pingSpoofModEnabled = false;
int pingSpoofModAmount = 100;
bool reconnectEnabled = false;
int reconnectDelay = 5000;
bool staffNotifierEnabled = false;
bool noRotateEnabled = false;
bool antiCactusEnabled = false;

bool nametagsEnabled = false;
float nametagsSize = 0.5f;
bool nametagsShowHealth = true;
bool nametagsShowArmor = true;
bool nametagsShowWeapon = true;

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

  double posX, posY, posZ;
  bool hasPos;
};
static std::vector<NTEntityData> g_ntEntityData;
static std::vector<NTEntityData> g_ntEntityDataReady;

struct ESPOverlayData {
  float boxLeft, boxRight, boxTop, boxBottom;
  float hpFrac;
  float hp, maxHp;
  int armor;
  char name[32];
  char weapon[24];
  ArmorSlotData armorSlots[4];
  GLuint skinTexture;
  bool valid;
};
static std::vector<ESPOverlayData> g_espOverlays;
static std::vector<ESPOverlayData> g_espOverlaysReady;

static double g_espLocalPX = 0, g_espLocalPY = 0, g_espLocalPZ = 0;

bool trajectoriesEnabled = false;
float trajectoriesLineWidth = 1.5f;
int trajectoriesMaxTicks = 240;
bool trajectoriesLandingCross = true;

static jfieldID g_fJumpTicks = nullptr;
static jfieldID g_fStepHeight = nullptr;
static jclass g_c03PosClass = nullptr;
static jmethodID g_c03PosInit = nullptr;
static jclass g_itemBlockClass = nullptr;

static volatile int g_trajHeldItemType = 0;
static volatile float g_trajYaw = 0, g_trajPitch = 0;
static volatile float g_trajEyeX = 0, g_trajEyeY = 0, g_trajEyeZ = 0;
static volatile float g_trajBowPower = 1.0f;

static float g_camProjection[16] = {};
static float g_camModelview[16] = {};
static bool g_camMatricesValid = false;

static jmethodID g_mRightClickMouse = nullptr;
static jmethodID g_mOnPlayerRightClick = nullptr;
static jclass g_vec3Class = nullptr;
static jmethodID g_vec3Init = nullptr;
static jclass g_enumFacingClass = nullptr;
static jobject g_enumFacingByIndex[6] = {nullptr, nullptr, nullptr,
                                         nullptr, nullptr, nullptr};

bool autoToolEnabled = false;
int autoToolActivationDelay = 0;
bool autoToolSwitchBack = true;
bool autoToolAllowSword = false;
bool autoToolAllowTool = true;
bool autoToolAllowFists = true;
bool autoToolAllowOther = false;
bool autoToolSneakOnly = false;
static jmethodID g_mGetStrVsBlock = nullptr;
static jmethodID g_mGetBlockState = nullptr;
static jclass g_blockPosClass = nullptr;
static jmethodID g_blockPosInit = nullptr;
static jmethodID g_mGetBlock = nullptr;
static jmethodID g_mIsAirBlock = nullptr;
static jfieldID g_fIsSneaking = nullptr;
static jmethodID g_mIsSneaking = nullptr;
static jmethodID g_mGetHealth = nullptr;
static jmethodID g_mGetMaxHealth = nullptr;
static jclass g_entityLivingClass = nullptr;
static jclass g_entityPlayerClass = nullptr;
static jmethodID g_mGetName = nullptr;
static jmethodID g_mGetArmorValue = nullptr;
static jmethodID g_mGetItemInUseDuration = nullptr;
static jmethodID g_mGetEquipmentInSlot = nullptr;
static jmethodID g_mGetMaxDamage = nullptr;
static jmethodID g_mGetItemDamage = nullptr;

bool sprintEnabled = false;
static jmethodID g_mSetSprinting = nullptr;
static bool g_sprintJniReady = false;

bool velocityEnabled = false;
float velocityHorizontal = 80.0f;
float velocityVertical = 100.0f;
float velocityChance = 100.0f;
bool velocityOnlyWhileMoving = false;
bool velocityOnlyOnGround = false;

bool noPacketEnabled = false;
bool pingSpoofEnabled = false;
int pingSpoofAmount = 50;

bool arraylistEnabled = false;
float arraylistFontScale = 1.28f;
float arraylistBgAlpha = 0.90f;
float arraylistBarWidth = 3.0f;
bool arraylistShowInfo = true;
bool arraylistRainbow = false;
float arraylistRainbowSpeed = 1.0f;
ImVec4 arraylistAccentColor =
    ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
ImVec4 arraylistTextColor = ImVec4(0.93f, 0.93f, 0.96f, 1.0f);
ImVec4 arraylistInfoColor =
    ImVec4(0.50f, 0.50f, 0.56f, 1.0f);
int arraylistPosition =
    0;
bool arraylistBackground = true;
bool arraylistAccentBar = true;
bool arraylistRoundedCorners = false;
bool arraylistWatermark = false;
bool arraylistWmShowSettings = false;
bool arraylistWmShowName = true;
bool arraylistWmShowVersion = true;
bool arraylistWmShowFps = true;
bool arraylistWmShowKeybinds = false;
float arraylistWmFontScale = 1.0f;
int arraylistWmPosition = 0;
ImVec4 arraylistWmTextColor = ImVec4(0.90f, 0.90f, 0.93f, 1.0f);
ImVec4 arraylistWmBgColor = ImVec4(0.06f, 0.06f, 0.08f, 0.82f);
ImVec4 arraylistWmBorderColor = ImVec4(0.25f, 0.25f, 0.30f, 0.50f);
static const char *g_clientName = "Vengeance";
static const char *g_clientVersion = "v1.0.8";

bool refillEnabled = false;
int refillMode = 0;
int refillMouseButton = 0;
int refillTriggerKey = VK_LSHIFT;
bool refillSoup = true;
bool refillPotion = true;
bool refillUseNonHealthPotions = false;
int refillDelayAfterOpen = 50;
int refillDelayBeforeClose = 50;
int refillSpeed = 50;
bool refillSmartSpeed = true;
bool refillCloseOnComplete = true;
bool refillDisableOnComplete = false;
bool refillRandomSlots = true;

static float g_menuFadeAnim = 0.0f;
static float g_menuScaleAnim = 0.0f;
static int g_lastCategory = 0;
static float g_cardAppearTimer = 0.0f;
static bool g_themeApplied = false;

struct NetworkNode {
  float x, y, vx, vy, sz;
};
static std::vector<NetworkNode> g_netNodes;
static bool g_nodesInit = false;
static float g_globalTime = 0.0f;
static float g_breathPhase = 0.0f;
static std::map<int, float> g_cardBounce;
static std::map<int, float> g_cardHoverGlow;
static std::map<int, float> g_cardPressAnim;
static std::map<int, float> g_cardRippleAnim;
static std::map<int, ImVec2> g_cardRipplePos;
static std::map<std::string, float> g_elementAppear;
static std::map<int, float> g_iconAppearAnim;
static std::map<int, float> g_dotEnabledAnim;
static std::map<std::string, float> g_subSettingsAnim;

bool showCombatWindow = false;
bool showVisualsWindow = false;
bool showMovementWindow = false;
bool showNetworkWindow = false;
bool showUtilitiesWindow = false;
bool showSettingsWindow = false;

std::vector<std::string> enabledModules;

static bool g_unloadRequested = false;
static bool g_isUnloading = false;
static HMODULE g_hModule = nullptr;
static HANDLE g_keyDispatcherThread = nullptr;
static DWORD g_keyDispatcherThreadId = 0;

static HANDLE g_moduleThreads[16] = {
    nullptr };

static int g_moduleThreadCount = 0;
static CRITICAL_SECTION g_unloadCS;
static bool g_unloadCSInitialized = false;

static bool g_imguiInitialized = false;
static HWND g_imguiHwnd = nullptr;
static bool g_wndProcHooked = false;
static WNDPROC g_originalWndProc = nullptr;
static HWND g_targetWindow = nullptr;
static bool g_fullscreenDetected = false;
static RECT g_lastWindowRect = {0};
static bool g_wasFullscreen = false;
static bool g_cursorCaptured = false;
volatile DWORD g_renderFrameIndex = 0;
static POINT g_lastCursorPos = {0};
static bool g_blockMouseMovement = false;
static bool g_cursorVisible = false;
static DWORD g_lastEKeyTime = 0;
static DWORD g_lastEscKeyTime = 0;
static bool g_isInventoryOpen = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

bool wallhacks = false;
bool sInventory = false;
bool sChest = false;
bool sPlayer = false;
bool sNickname = false;
bool sChamsActive = false;

Position entity;
Position players;
Position chest;
Position largeChest;

struct EntityRefData {
  jobject entityRef;
  bool valid;
};
static std::vector<EntityRefData> g_playerEntityRefs;
static std::vector<EntityRefData> g_playerEntityRefsReady;

typedef void(__stdcall *T_glEnable)(GLenum);
typedef void(__stdcall *T_glDisable)(GLenum);
typedef void(__stdcall *T_glOrtho)(GLdouble, GLdouble, GLdouble, GLdouble,
                                   GLdouble, GLdouble);
typedef void(__stdcall *T_glScalef)(GLfloat, GLfloat, GLfloat);
typedef void(__stdcall *T_glTranslatef)(GLfloat, GLfloat, GLfloat);
typedef BOOL(__stdcall *T_SwapBuffers)(HDC);

T_glEnable pglEnable = nullptr;
T_glDisable pglDisable = nullptr;
T_glOrtho pglOrtho = nullptr;
T_glScalef pglScalef = nullptr;
T_glTranslatef pglTranslatef = nullptr;
T_SwapBuffers pSwapBuffers = nullptr;

Vector2D screenCenter;
std::vector<Vector2D> entityScreenPositions;

const int FONT_HEIGHT = 14;

LRESULT __stdcall keyHandler(int nCode, WPARAM wParam, LPARAM lParam);
BOOL __stdcall mySwapBuffers(HDC hDC);
void __stdcall myglOrtho(double left, double right, double bottom, double top,
                         double zNear, double zFar);
void __stdcall myglTranslatef(float x, float y, float z);
void __stdcall myglScalef(float x, float y, float z);
void __stdcall myglEnable(unsigned int cap);
void __stdcall myglDisable(unsigned int cap);

namespace UITheme {
ImVec4 BG_BASE = ImVec4(0.055f, 0.055f, 0.062f, 0.99f);
ImVec4 BG_SIDEBAR = ImVec4(0.045f, 0.045f, 0.052f, 0.99f);
ImVec4 BG_CARD = ImVec4(0.068f, 0.068f, 0.078f, 0.96f);
ImVec4 BG_CARD_BORDER = ImVec4(0.12f, 0.12f, 0.15f, 0.40f);

ImVec4 TEXT_PRIMARY = ImVec4(0.93f, 0.93f, 0.96f, 1.0f);
ImVec4 TEXT_SECONDARY = ImVec4(0.40f, 0.40f, 0.46f, 1.0f);
ImVec4 TEXT_DISABLED = ImVec4(0.24f, 0.24f, 0.28f, 1.0f);
ImVec4 TEXT_CATEGORY = ImVec4(0.33f, 0.33f, 0.40f, 1.0f);

ImVec4 ACCENT = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
ImVec4 ACCENT_HOVER = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 ACCENT_DIM = ImVec4(0.88f, 0.88f, 0.95f, 0.14f);
ImVec4 CYAN = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
ImVec4 GLOW = ImVec4(0.75f, 0.75f, 0.88f, 1.0f);

ImVec4 ITEM_HOVER = ImVec4(0.08f, 0.08f, 0.10f, 0.65f);
ImVec4 ITEM_SELECTED = ImVec4(0.88f, 0.88f, 0.95f, 0.12f);
ImVec4 ITEM_SELECTED_BORDER = ImVec4(0.88f, 0.88f, 0.95f, 0.35f);

ImVec4 TOGGLE_OFF = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
ImVec4 TOGGLE_ON = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);

ImVec4 SLIDER_BG = ImVec4(0.09f, 0.09f, 0.11f, 1.0f);
ImVec4 SEPARATOR = ImVec4(0.12f, 0.12f, 0.15f, 0.45f);

ImVec4 BUTTON_NORMAL = ImVec4(0.07f, 0.07f, 0.09f, 0.75f);
ImVec4 BUTTON_HOVER = ImVec4(0.11f, 0.11f, 0.14f, 0.85f);
ImVec4 BUTTON_ACTIVE = ImVec4(0.06f, 0.06f, 0.08f, 0.95f);

namespace Defaults {
const ImVec4 BG_BASE = ImVec4(0.055f, 0.055f, 0.062f, 0.99f);
const ImVec4 BG_SIDEBAR = ImVec4(0.045f, 0.045f, 0.052f, 0.99f);
const ImVec4 BG_CARD = ImVec4(0.068f, 0.068f, 0.078f, 0.96f);
const ImVec4 BG_CARD_BORDER = ImVec4(0.12f, 0.12f, 0.15f, 0.40f);
const ImVec4 TEXT_PRIMARY = ImVec4(0.93f, 0.93f, 0.96f, 1.0f);
const ImVec4 TEXT_SECONDARY = ImVec4(0.40f, 0.40f, 0.46f, 1.0f);
const ImVec4 TEXT_DISABLED = ImVec4(0.24f, 0.24f, 0.28f, 1.0f);
const ImVec4 TEXT_CATEGORY = ImVec4(0.33f, 0.33f, 0.40f, 1.0f);
const ImVec4 ACCENT = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
const ImVec4 ACCENT_HOVER = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
const ImVec4 ACCENT_DIM = ImVec4(0.88f, 0.88f, 0.95f, 0.14f);
const ImVec4 CYAN = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
const ImVec4 GLOW = ImVec4(0.75f, 0.75f, 0.88f, 1.0f);
const ImVec4 ITEM_HOVER = ImVec4(0.08f, 0.08f, 0.10f, 0.65f);
const ImVec4 ITEM_SELECTED = ImVec4(0.88f, 0.88f, 0.95f, 0.12f);
const ImVec4 ITEM_SELECTED_BORDER = ImVec4(0.88f, 0.88f, 0.95f, 0.35f);
const ImVec4 TOGGLE_OFF = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
const ImVec4 TOGGLE_ON = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
const ImVec4 SLIDER_BG = ImVec4(0.09f, 0.09f, 0.11f, 1.0f);
const ImVec4 SEPARATOR = ImVec4(0.12f, 0.12f, 0.15f, 0.45f);
const ImVec4 BUTTON_NORMAL = ImVec4(0.07f, 0.07f, 0.09f, 0.75f);
const ImVec4 BUTTON_HOVER = ImVec4(0.11f, 0.11f, 0.14f, 0.85f);
const ImVec4 BUTTON_ACTIVE = ImVec4(0.06f, 0.06f, 0.08f, 0.95f);
}

void ResetToDefaults() {
  BG_BASE = Defaults::BG_BASE;
  BG_SIDEBAR = Defaults::BG_SIDEBAR;
  BG_CARD = Defaults::BG_CARD;
  BG_CARD_BORDER = Defaults::BG_CARD_BORDER;
  TEXT_PRIMARY = Defaults::TEXT_PRIMARY;
  TEXT_SECONDARY = Defaults::TEXT_SECONDARY;
  TEXT_DISABLED = Defaults::TEXT_DISABLED;
  TEXT_CATEGORY = Defaults::TEXT_CATEGORY;
  ACCENT = Defaults::ACCENT;
  ACCENT_HOVER = Defaults::ACCENT_HOVER;
  ACCENT_DIM = Defaults::ACCENT_DIM;
  CYAN = Defaults::CYAN;
  GLOW = Defaults::GLOW;
  ITEM_HOVER = Defaults::ITEM_HOVER;
  ITEM_SELECTED = Defaults::ITEM_SELECTED;
  ITEM_SELECTED_BORDER = Defaults::ITEM_SELECTED_BORDER;
  TOGGLE_OFF = Defaults::TOGGLE_OFF;
  TOGGLE_ON = Defaults::TOGGLE_ON;
  SLIDER_BG = Defaults::SLIDER_BG;
  SEPARATOR = Defaults::SEPARATOR;
  BUTTON_NORMAL = Defaults::BUTTON_NORMAL;
  BUTTON_HOVER = Defaults::BUTTON_HOVER;
  BUTTON_ACTIVE = Defaults::BUTTON_ACTIVE;
}

ImVec4 Lerp(const ImVec4 &a, const ImVec4 &b, float t) {
  return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
}
ImU32 ColorWithAlpha(const ImVec4 &c, float a) {
  return ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, a));
}
}

namespace UILayout {
const float WINDOW_W = 700.0f;
const float WINDOW_H = 460.0f;
const float SIDEBAR_W = 165.0f;
const float PADDING = 12.0f;
const float ITEM_SPACING = 4.0f;
const float SECTION_SPACING = 10.0f;
const float WINDOW_ROUNDING = 10.0f;
const float CARD_ROUNDING = 8.0f;
const float FRAME_ROUNDING = 4.0f;
const float GRAB_ROUNDING = 3.0f;
const float TOGGLE_H = 14.0f;
const float TOGGLE_W = 28.0f;
const float SIDEBAR_ITEM_H = 26.0f;
const float CHECKBOX_SIZE = 16.0f;
}

static bool g_animDots = true;
static bool g_animBreathGlow = true;
static float g_animDotSpeed = 1.0f;
static float g_animDotOpacity = 1.0f;
static float g_animGlowIntensity = 1.0f;

struct ThemePreset {
  char name[64];

  ImVec4 BG_BASE, BG_SIDEBAR, BG_CARD, BG_CARD_BORDER;
  ImVec4 TEXT_PRIMARY, TEXT_SECONDARY, TEXT_DISABLED, TEXT_CATEGORY;
  ImVec4 ACCENT, ACCENT_HOVER, ACCENT_DIM, CYAN, GLOW;
  ImVec4 ITEM_HOVER, ITEM_SELECTED, ITEM_SELECTED_BORDER;
  ImVec4 TOGGLE_OFF, TOGGLE_ON;
  ImVec4 SLIDER_BG, SEPARATOR;
  ImVec4 BUTTON_NORMAL, BUTTON_HOVER, BUTTON_ACTIVE;

  bool animDots, animBreathGlow;
  float animDotSpeed, animDotOpacity, animGlowIntensity;
};

static void PresetFromCurrent(ThemePreset &p) {
  p.BG_BASE = UITheme::BG_BASE; p.BG_SIDEBAR = UITheme::BG_SIDEBAR;
  p.BG_CARD = UITheme::BG_CARD; p.BG_CARD_BORDER = UITheme::BG_CARD_BORDER;
  p.TEXT_PRIMARY = UITheme::TEXT_PRIMARY; p.TEXT_SECONDARY = UITheme::TEXT_SECONDARY;
  p.TEXT_DISABLED = UITheme::TEXT_DISABLED; p.TEXT_CATEGORY = UITheme::TEXT_CATEGORY;
  p.ACCENT = UITheme::ACCENT; p.ACCENT_HOVER = UITheme::ACCENT_HOVER;
  p.ACCENT_DIM = UITheme::ACCENT_DIM; p.CYAN = UITheme::CYAN; p.GLOW = UITheme::GLOW;
  p.ITEM_HOVER = UITheme::ITEM_HOVER; p.ITEM_SELECTED = UITheme::ITEM_SELECTED;
  p.ITEM_SELECTED_BORDER = UITheme::ITEM_SELECTED_BORDER;
  p.TOGGLE_OFF = UITheme::TOGGLE_OFF; p.TOGGLE_ON = UITheme::TOGGLE_ON;
  p.SLIDER_BG = UITheme::SLIDER_BG; p.SEPARATOR = UITheme::SEPARATOR;
  p.BUTTON_NORMAL = UITheme::BUTTON_NORMAL; p.BUTTON_HOVER = UITheme::BUTTON_HOVER;
  p.BUTTON_ACTIVE = UITheme::BUTTON_ACTIVE;
  p.animDots = g_animDots; p.animBreathGlow = g_animBreathGlow;
  p.animDotSpeed = g_animDotSpeed; p.animDotOpacity = g_animDotOpacity;
  p.animGlowIntensity = g_animGlowIntensity;
}

static void ApplyPreset(const ThemePreset &p) {
  UITheme::BG_BASE = p.BG_BASE; UITheme::BG_SIDEBAR = p.BG_SIDEBAR;
  UITheme::BG_CARD = p.BG_CARD; UITheme::BG_CARD_BORDER = p.BG_CARD_BORDER;
  UITheme::TEXT_PRIMARY = p.TEXT_PRIMARY; UITheme::TEXT_SECONDARY = p.TEXT_SECONDARY;
  UITheme::TEXT_DISABLED = p.TEXT_DISABLED; UITheme::TEXT_CATEGORY = p.TEXT_CATEGORY;
  UITheme::ACCENT = p.ACCENT; UITheme::ACCENT_HOVER = p.ACCENT_HOVER;
  UITheme::ACCENT_DIM = p.ACCENT_DIM; UITheme::CYAN = p.CYAN; UITheme::GLOW = p.GLOW;
  UITheme::ITEM_HOVER = p.ITEM_HOVER; UITheme::ITEM_SELECTED = p.ITEM_SELECTED;
  UITheme::ITEM_SELECTED_BORDER = p.ITEM_SELECTED_BORDER;
  UITheme::TOGGLE_OFF = p.TOGGLE_OFF; UITheme::TOGGLE_ON = p.TOGGLE_ON;
  UITheme::SLIDER_BG = p.SLIDER_BG; UITheme::SEPARATOR = p.SEPARATOR;
  UITheme::BUTTON_NORMAL = p.BUTTON_NORMAL; UITheme::BUTTON_HOVER = p.BUTTON_HOVER;
  UITheme::BUTTON_ACTIVE = p.BUTTON_ACTIVE;
  g_animDots = p.animDots; g_animBreathGlow = p.animBreathGlow;
  g_animDotSpeed = p.animDotSpeed; g_animDotOpacity = p.animDotOpacity;
  g_animGlowIntensity = p.animGlowIntensity;
}

static ThemePreset g_presetDark = {
  "Dark",

  {0.055f,0.055f,0.062f,0.99f}, {0.045f,0.045f,0.052f,0.99f},
  {0.068f,0.068f,0.078f,0.96f}, {0.12f,0.12f,0.15f,0.40f},

  {0.93f,0.93f,0.96f,1.0f}, {0.40f,0.40f,0.46f,1.0f},
  {0.24f,0.24f,0.28f,1.0f}, {0.33f,0.33f,0.40f,1.0f},

  {0.88f,0.88f,0.95f,1.0f}, {1.0f,1.0f,1.0f,1.0f},
  {0.88f,0.88f,0.95f,0.14f}, {0.88f,0.88f,0.95f,1.0f}, {0.75f,0.75f,0.88f,1.0f},

  {0.08f,0.08f,0.10f,0.65f}, {0.88f,0.88f,0.95f,0.12f}, {0.88f,0.88f,0.95f,0.35f},

  {0.18f,0.18f,0.22f,1.0f}, {0.88f,0.88f,0.95f,1.0f},

  {0.09f,0.09f,0.11f,1.0f}, {0.12f,0.12f,0.15f,0.45f},

  {0.07f,0.07f,0.09f,0.75f}, {0.11f,0.11f,0.14f,0.85f}, {0.06f,0.06f,0.08f,0.95f},

  true, true, 1.0f, 1.0f, 1.0f
};

static ThemePreset g_presetLight = {
  "Light",

  {0.945f,0.945f,0.938f,0.99f}, {0.955f,0.955f,0.948f,0.99f},
  {0.932f,0.932f,0.922f,0.96f}, {0.88f,0.88f,0.85f,0.40f},

  {0.07f,0.07f,0.04f,1.0f}, {0.60f,0.60f,0.54f,1.0f},
  {0.76f,0.76f,0.72f,1.0f}, {0.67f,0.67f,0.60f,1.0f},

  {0.12f,0.12f,0.05f,1.0f}, {0.0f,0.0f,0.0f,1.0f},
  {0.12f,0.12f,0.05f,0.14f}, {0.12f,0.12f,0.05f,1.0f}, {0.25f,0.25f,0.12f,1.0f},

  {0.92f,0.92f,0.90f,0.65f}, {0.12f,0.12f,0.05f,0.12f}, {0.12f,0.12f,0.05f,0.35f},

  {0.82f,0.82f,0.78f,1.0f}, {0.12f,0.12f,0.05f,1.0f},

  {0.91f,0.91f,0.89f,1.0f}, {0.88f,0.88f,0.85f,0.45f},

  {0.93f,0.93f,0.91f,0.75f}, {0.89f,0.89f,0.86f,0.85f}, {0.94f,0.94f,0.92f,0.95f},

  true, true, 1.0f, 0.6f, 0.8f
};

static ThemePreset g_presetCustom = {};
static bool g_hasCustomPreset = false;
static int g_activePresetIdx = 0;

static char g_loggedInUser[128] = "";
static HANDLE g_ipcThread = NULL;

static DWORD WINAPI IpcListenerThread(LPVOID) {

  HANDLE hMap = OpenFileMappingA(FILE_MAP_READ, FALSE, "VengeanceUserIPC");
  if (hMap) {
    char *buf = (char *)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 128);
    if (buf) {
      if (buf[0] != '\0') {
        strncpy(g_loggedInUser, buf, 127);
        g_loggedInUser[127] = '\0';
      }
      UnmapViewOfFile(buf);
    }
    CloseHandle(hMap);
  }

  while (!g_isUnloading) {
    if (g_loggedInUser[0] == '\0') {
      HANDLE hMap2 = OpenFileMappingA(FILE_MAP_READ, FALSE, "VengeanceUserIPC");
      if (hMap2) {
        char *buf2 = (char *)MapViewOfFile(hMap2, FILE_MAP_READ, 0, 0, 128);
        if (buf2 && buf2[0] != '\0') {
          strncpy(g_loggedInUser, buf2, 127);
          g_loggedInUser[127] = '\0';
          UnmapViewOfFile(buf2);
        }
        CloseHandle(hMap2);
      }
    }
    Sleep(2000);
  }
  return 0;
}

static const wchar_t* g_dllApiHost = L"localhost";
static const INTERNET_PORT g_dllApiPort = 5000;
static const char* g_dllApiSecret = "vng_s3cr3t_k3y_x9f2mQ7pL4wR8tY1";

static std::string DllHttpPost(const wchar_t* path, const std::string& jsonBody) {
  HINTERNET hSession = WinHttpOpen(L"Vengeance/1.0",
    WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
    WINHTTP_NO_PROXY_BYPASS, 0);
  if (!hSession) return "";
  HINTERNET hConnect = WinHttpConnect(hSession, g_dllApiHost, g_dllApiPort, 0);
  if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }
  HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path,
    NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
  if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

  std::wstring headers = L"Content-Type: application/json\r\nX-API-Secret: ";

  int len = MultiByteToWideChar(CP_UTF8, 0, g_dllApiSecret, -1, NULL, 0);
  std::wstring wSecret(len, 0);
  MultiByteToWideChar(CP_UTF8, 0, g_dllApiSecret, -1, &wSecret[0], len);
  wSecret.resize(len - 1);
  headers += wSecret + L"\r\n";

  BOOL ok = WinHttpSendRequest(hRequest, headers.c_str(), (DWORD)-1,
    (LPVOID)jsonBody.c_str(), (DWORD)jsonBody.size(),
    (DWORD)jsonBody.size(), 0);
  if (!ok) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }
  WinHttpReceiveResponse(hRequest, NULL);

  std::string result;
  DWORD bytesRead = 0;
  char buf[4096];
  while (WinHttpReadData(hRequest, buf, sizeof(buf) - 1, &bytesRead) && bytesRead > 0) {
    buf[bytesRead] = '\0';
    result += buf;
    bytesRead = 0;
  }
  WinHttpCloseHandle(hRequest);
  WinHttpCloseHandle(hConnect);
  WinHttpCloseHandle(hSession);
  return result;
}

static std::string ImVec4ToJson(const ImVec4& c) {
  char buf[128];
  snprintf(buf, sizeof(buf), "[%.4f,%.4f,%.4f,%.4f]", c.x, c.y, c.z, c.w);
  return buf;
}

static ImVec4 JsonToImVec4(const char* s) {
  ImVec4 c = {0,0,0,1};
  if (s && *s == '[') sscanf(s, "[%f,%f,%f,%f]", &c.x, &c.y, &c.z, &c.w);
  return c;
}

static std::string EscapeJson(const std::string& s) {
  std::string out;
  for (char c : s) {
    if (c == '"') out += "\\\"";
    else if (c == '\\') out += "\\\\";
    else out += c;
  }
  return out;
}

static void PresetSaveToApi(const ThemePreset& p, const char* username) {
  if (!username || username[0] == '\0') return;

  std::string data = "{";
  data += "\"BG_BASE\":" + ImVec4ToJson(p.BG_BASE) + ",";
  data += "\"BG_SIDEBAR\":" + ImVec4ToJson(p.BG_SIDEBAR) + ",";
  data += "\"BG_CARD\":" + ImVec4ToJson(p.BG_CARD) + ",";
  data += "\"BG_CARD_BORDER\":" + ImVec4ToJson(p.BG_CARD_BORDER) + ",";
  data += "\"TEXT_PRIMARY\":" + ImVec4ToJson(p.TEXT_PRIMARY) + ",";
  data += "\"TEXT_SECONDARY\":" + ImVec4ToJson(p.TEXT_SECONDARY) + ",";
  data += "\"TEXT_DISABLED\":" + ImVec4ToJson(p.TEXT_DISABLED) + ",";
  data += "\"TEXT_CATEGORY\":" + ImVec4ToJson(p.TEXT_CATEGORY) + ",";
  data += "\"ACCENT\":" + ImVec4ToJson(p.ACCENT) + ",";
  data += "\"ACCENT_HOVER\":" + ImVec4ToJson(p.ACCENT_HOVER) + ",";
  data += "\"ACCENT_DIM\":" + ImVec4ToJson(p.ACCENT_DIM) + ",";
  data += "\"CYAN\":" + ImVec4ToJson(p.CYAN) + ",";
  data += "\"GLOW\":" + ImVec4ToJson(p.GLOW) + ",";
  data += "\"ITEM_HOVER\":" + ImVec4ToJson(p.ITEM_HOVER) + ",";
  data += "\"ITEM_SELECTED\":" + ImVec4ToJson(p.ITEM_SELECTED) + ",";
  data += "\"ITEM_SELECTED_BORDER\":" + ImVec4ToJson(p.ITEM_SELECTED_BORDER) + ",";
  data += "\"TOGGLE_ON\":" + ImVec4ToJson(p.TOGGLE_ON) + ",";
  data += "\"TOGGLE_OFF\":" + ImVec4ToJson(p.TOGGLE_OFF) + ",";
  data += "\"SLIDER_BG\":" + ImVec4ToJson(p.SLIDER_BG) + ",";
  data += "\"SEPARATOR\":" + ImVec4ToJson(p.SEPARATOR) + ",";
  data += "\"BUTTON_NORMAL\":" + ImVec4ToJson(p.BUTTON_NORMAL) + ",";
  data += "\"BUTTON_HOVER\":" + ImVec4ToJson(p.BUTTON_HOVER) + ",";
  data += "\"BUTTON_ACTIVE\":" + ImVec4ToJson(p.BUTTON_ACTIVE) + ",";

  char abuf[64];
  snprintf(abuf, sizeof(abuf), "%s", p.animDots ? "true" : "false");
  data += std::string("\"animDots\":") + abuf + ",";
  snprintf(abuf, sizeof(abuf), "%s", p.animBreathGlow ? "true" : "false");
  data += std::string("\"animBreathGlow\":") + abuf + ",";
  snprintf(abuf, sizeof(abuf), "%.4f", p.animDotSpeed);
  data += std::string("\"animDotSpeed\":") + abuf + ",";
  snprintf(abuf, sizeof(abuf), "%.4f", p.animDotOpacity);
  data += std::string("\"animDotOpacity\":") + abuf + ",";
  snprintf(abuf, sizeof(abuf), "%.4f", p.animGlowIntensity);
  data += std::string("\"animGlowIntensity\":") + abuf + "}";

  std::string presetName = EscapeJson(p.name);
  std::string user = EscapeJson(username);

  std::string body = "{\"username\":\"" + user + "\",\"name\":\"" + presetName + "\",\"data\":" + data + "}";
  DllHttpPost(L"/api/presets/save", body);
}

static bool g_presetLoadedFromApi = false;
static bool g_presetSaving = false;
static char g_presetSaveStatus[64] = "";

static DWORD WINAPI PresetSaveThread(LPVOID param) {
  g_presetSaving = true;
  strncpy(g_presetSaveStatus, "Saving...", sizeof(g_presetSaveStatus));
  PresetSaveToApi(g_presetCustom, g_loggedInUser);
  strncpy(g_presetSaveStatus, "Saved!", sizeof(g_presetSaveStatus));
  g_presetSaving = false;
  return 0;
}

static std::string JsonGetString(const std::string& json, const std::string& key) {
  std::string search = "\"" + key + "\":";
  size_t pos = json.find(search);
  if (pos == std::string::npos) return "";
  pos += search.size();
  while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
  if (pos >= json.size()) return "";
  if (json[pos] == '"') {
    pos++;
    size_t end = json.find('"', pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
  }
  return "";
}

static std::string JsonGetArray(const std::string& json, const std::string& key) {
  std::string search = "\"" + key + "\":";
  size_t pos = json.find(search);
  if (pos == std::string::npos) return "";
  pos += search.size();
  while (pos < json.size() && json[pos] == ' ') pos++;
  if (pos >= json.size() || json[pos] != '[') return "";
  size_t end = json.find(']', pos);
  if (end == std::string::npos) return "";
  return json.substr(pos, end - pos + 1);
}

static std::string JsonGetValue(const std::string& json, const std::string& key) {
  std::string search = "\"" + key + "\":";
  size_t pos = json.find(search);
  if (pos == std::string::npos) return "";
  pos += search.size();
  while (pos < json.size() && json[pos] == ' ') pos++;
  size_t end = pos;
  while (end < json.size() && json[end] != ',' && json[end] != '}') end++;
  return json.substr(pos, end - pos);
}

static void PresetLoadFromApi(const char* username) {
  if (!username || username[0] == '\0') return;

  std::string user = EscapeJson(username);
  std::string body = "{\"username\":\"" + user + "\",\"name\":\"Custom\"}";
  std::string response = DllHttpPost(L"/api/presets/get", body);

  if (response.empty()) return;
  if (response.find("\"error\"") != std::string::npos) return;

  size_t dataPos = response.find("\"data\":");
  if (dataPos == std::string::npos) return;
  std::string data = response.substr(dataPos + 7);

  auto getColor = [&](const std::string& key) -> ImVec4 {
    std::string arr = JsonGetArray(data, key);
    if (arr.empty()) return ImVec4(0,0,0,1);
    return JsonToImVec4(arr.c_str());
  };

  g_presetCustom.BG_BASE = getColor("BG_BASE");
  g_presetCustom.BG_SIDEBAR = getColor("BG_SIDEBAR");
  g_presetCustom.BG_CARD = getColor("BG_CARD");
  g_presetCustom.BG_CARD_BORDER = getColor("BG_CARD_BORDER");
  g_presetCustom.TEXT_PRIMARY = getColor("TEXT_PRIMARY");
  g_presetCustom.TEXT_SECONDARY = getColor("TEXT_SECONDARY");
  g_presetCustom.TEXT_DISABLED = getColor("TEXT_DISABLED");
  g_presetCustom.TEXT_CATEGORY = getColor("TEXT_CATEGORY");
  g_presetCustom.ACCENT = getColor("ACCENT");
  g_presetCustom.ACCENT_HOVER = getColor("ACCENT_HOVER");
  g_presetCustom.ACCENT_DIM = getColor("ACCENT_DIM");
  g_presetCustom.CYAN = getColor("CYAN");
  g_presetCustom.GLOW = getColor("GLOW");
  g_presetCustom.ITEM_HOVER = getColor("ITEM_HOVER");
  g_presetCustom.ITEM_SELECTED = getColor("ITEM_SELECTED");
  g_presetCustom.ITEM_SELECTED_BORDER = getColor("ITEM_SELECTED_BORDER");
  g_presetCustom.TOGGLE_ON = getColor("TOGGLE_ON");
  g_presetCustom.TOGGLE_OFF = getColor("TOGGLE_OFF");
  g_presetCustom.SLIDER_BG = getColor("SLIDER_BG");
  g_presetCustom.SEPARATOR = getColor("SEPARATOR");
  g_presetCustom.BUTTON_NORMAL = getColor("BUTTON_NORMAL");
  g_presetCustom.BUTTON_HOVER = getColor("BUTTON_HOVER");
  g_presetCustom.BUTTON_ACTIVE = getColor("BUTTON_ACTIVE");

  std::string val;
  val = JsonGetValue(data, "animDots");
  if (!val.empty()) g_presetCustom.animDots = (val == "true");
  val = JsonGetValue(data, "animBreathGlow");
  if (!val.empty()) g_presetCustom.animBreathGlow = (val == "true");
  val = JsonGetValue(data, "animDotSpeed");
  if (!val.empty()) g_presetCustom.animDotSpeed = (float)atof(val.c_str());
  val = JsonGetValue(data, "animDotOpacity");
  if (!val.empty()) g_presetCustom.animDotOpacity = (float)atof(val.c_str());
  val = JsonGetValue(data, "animGlowIntensity");
  if (!val.empty()) g_presetCustom.animGlowIntensity = (float)atof(val.c_str());

  strncpy(g_presetCustom.name, "Custom", sizeof(g_presetCustom.name));
  g_hasCustomPreset = true;
  g_presetLoadedFromApi = true;
}

static DWORD WINAPI PresetLoadThread(LPVOID) {

  for (int i = 0; i < 30 && g_loggedInUser[0] == '\0' && !g_isUnloading; i++)
    Sleep(1000);
  if (g_isUnloading || g_loggedInUser[0] == '\0') return 0;
  PresetLoadFromApi(g_loggedInUser);
  return 0;
}

static std::map<ImGuiID, float> g_animState;
static float AnimLerp(ImGuiID id, float target, float speed = 0.12f) {
  float &val = g_animState[id];
  val += (target - val) * speed;
  if (fabsf(val - target) < 0.001f)
    val = target;
  return val;
}

struct ModuleInfo {
  const char *name;
  const char *desc;
};

struct CategoryInfo {
  const char *name;
  std::vector<ModuleInfo> modules;
};

static std::vector<CategoryInfo> g_categories = {
    {"All", {}},
    {"Combat",
     {{"Aim Assist", "Aim assistance"},
      {"Left Clicker", "CPS, Pattern"},
      {"Blockhit", "Auto block"},
      {"Reach", "Extended range"},
      {"Hit Select", "Filter clicks"},
      {"S-Tap", "Sprint reset tap"},
      {"W-Tap", "Forward sprint tap"},
      {"Bow Aimbot", "Bow aim assist"}}},
    {"Movement",
     {{"Velocity", "Knockback control"},
      {"Sprint", "Always sprint"},
      {"Sprint Reset", "Max knockback hits"},
      {"No Fall", "Negate fall damage"},
      {"Inv Walk", "Move in inventory"},
      {"Jump Reset", "Auto jump on KB"},
      {"No Jump Delay", "Remove jump cooldown"},
      {"Scaffold", "Auto place blocks"},
      {"Step", "Step up blocks"},
      {"Spider", "Climb walls"},
      {"SafeWalk", "No edge falling"},
      {"Reverse Step", "Step down blocks"},
      {"BouncySlime", "Bounce on slime"},
      {"FastStop", "Instant stop"},
      {"Glide", "Slow falling"},
      {"Flight", "Creative flight"},
      {"Air Jump", "Jump mid-air"},
      {"High Jump", "Higher jumps"}}},
    {"Visuals",
     {{"ESP", "Player boxes"},
      {"Tracer", "Player lines"},
      {"XRay", "See through"},
      {"StorageESP", "Storage highlight"},
      {"ArrayList", "Module list"},
      {"Chams", "Players through walls"},
      {"TargetHUD", "Target info card"},
      {"Freecam", "Detached camera"},
      {"FullBright", "Max brightness"},
      {"NoHurtCam", "No hurt shake"},
      {"Zoom", "Camera zoom"},
      {"ArmorESP", "Armor overlay"},
      {"ItemESP", "Dropped items"},
      {"HoleESP", "Safe holes"}}},
    {"Network",
     {{"Blink", "Hold packets"},
      {"Timer", "Game speed"},
      {"Fake Lag", "Simulated lag"}}},
    {"Utilities",
     {{"Right Clicker", "Right click CPS"},
      {"Fast Place", "No place delay"},
      {"Throwpot", "Auto throw potions"},
      {"AutoSoup", "Auto eat soup"},
      {"Refill", "Auto refill hotbar"},
      {"Auto Tool", "Auto select tool"},
      {"Bridge Assist", "Edge sneak assist"},
      {"Teams", "Team detection"},
      {"Friends", "Friend system"},
      {"Auto Sword", "Auto select sword"},
      {"Anti Void", "Prevent void death"},
      {"Middle Click Friend", "MC friend add"},
      {"Anti Bot", "Filter fake players"},
      {"Auto Armor", "Auto equip armor"},
      {"Inventory Manager", "Sort inventory"}}},
    {"Misc",
     {{"BreadCrumbs", "Path trail"},
      {"AntiAFK", "Prevent AFK kick"},
      {"Anti Bad Effects", "Remove bad pots"},
      {"Panic", "Disable all modules"},
      {"Ping Spoof", "Fake latency"},
      {"Reconnect", "Auto reconnect"},
      {"Staff Notifier", "Detect staff"},
      {"NoRotate", "Block rotations"},
      {"AntiCactus", "No cactus damage"}}},
    {"Settings",
     {{"Unload", "Unload Vengeance"}}},
    {"Appearance",
     {{"Colors", "Customize UI colors"},
      {"Presets", "Theme presets"},
      {"UI Presets", "Layout & style presets"}}}};

struct AllModuleMapping { int realCat; int realMod; };
static std::vector<AllModuleMapping> g_allModuleMap;

static const int g_appearanceCatIdx = 8;

static void BuildAllCategory() {
  g_categories[0].modules.clear();
  g_allModuleMap.clear();
  for (int ci = 1; ci < (int)g_categories.size(); ci++) {
    if (ci == g_appearanceCatIdx) continue;
    auto &cat = g_categories[ci];
    for (int mi = 0; mi < (int)cat.modules.size(); mi++) {
      g_categories[0].modules.push_back(cat.modules[mi]);
      g_allModuleMap.push_back({ci, mi});
    }
  }
}

static int g_selectedCategory = 0;
static int g_openModuleCat = -1;
static int g_openModuleIdx = -1;
static float g_settingsPanelAnim = 0.0f;
static float g_tabAnims[9] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

struct KeyBind {
  int virtualKey;
  const char *name;
  bool *targetVariable;
  bool isWaitingForKey;
  std::string displayName;
};

std::vector<KeyBind> g_keybinds;
bool g_isCapturingKey = false;
int g_capturingIndex = -1;

void CleanupImGui() {
  if (g_imguiInitialized) {
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    g_imguiInitialized = false;
  }
}

bool IsWindowFullscreen(HWND hWnd) {
  if (!hWnd)
    return false;

  RECT windowRect;
  GetWindowRect(hWnd, &windowRect);

  HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi = {sizeof(mi)};
  GetMonitorInfo(monitor, &mi);

  return (windowRect.left == mi.rcMonitor.left &&
          windowRect.top == mi.rcMonitor.top &&
          windowRect.right == mi.rcMonitor.right &&
          windowRect.bottom == mi.rcMonitor.bottom);
}

void RestoreWndProc() {
  if (g_wndProcHooked && g_originalWndProc && g_targetWindow) {
    SetWindowLongPtr(g_targetWindow, GWLP_WNDPROC, (LONG_PTR)g_originalWndProc);
    g_wndProcHooked = false;
    g_originalWndProc = nullptr;
    g_targetWindow = nullptr;
  }
}

void StopKeyboardThread() {
  if (g_keyDispatcherThread && g_keyDispatcherThreadId) {
    PostThreadMessage(g_keyDispatcherThreadId, WM_QUIT, 0, 0);
    DWORD waitResult = WaitForSingleObject(g_keyDispatcherThread, 2000);
    if (waitResult == WAIT_TIMEOUT) {
      TerminateThread(g_keyDispatcherThread, 0);
    }
    CloseHandle(g_keyDispatcherThread);
    g_keyDispatcherThread = nullptr;
    g_keyDispatcherThreadId = 0;
  }
}

void DetachAllHooks() {
  if (!pglScalef || !pglTranslatef || !pSwapBuffers || !pglOrtho ||
      !pglDisable || !pglEnable) {
    return;
  }

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  if (pglScalef)
    DetourDetach(reinterpret_cast<void **>(&pglScalef), myglScalef);
  if (pglTranslatef)
    DetourDetach(reinterpret_cast<void **>(&pglTranslatef), myglTranslatef);
  if (pSwapBuffers)
    DetourDetach(reinterpret_cast<void **>(&pSwapBuffers), mySwapBuffers);
  if (pglOrtho)
    DetourDetach(reinterpret_cast<void **>(&pglOrtho), myglOrtho);
  if (pglDisable)
    DetourDetach(reinterpret_cast<void **>(&pglDisable), myglDisable);
  if (pglEnable)
    DetourDetach(reinterpret_cast<void **>(&pglEnable), myglEnable);

  DetourTransactionCommit();
}

void WaitForModuleThreads() {

  for (int i = 0; i < g_moduleThreadCount; i++) {
    if (g_moduleThreads[i]) {
      DWORD waitResult = WaitForSingleObject(g_moduleThreads[i], 3000);
      if (waitResult == WAIT_TIMEOUT) {
        TerminateThread(g_moduleThreads[i], 0);
      }
      CloseHandle(g_moduleThreads[i]);
      g_moduleThreads[i] = nullptr;
    }
  }
  g_moduleThreadCount = 0;
}

static DWORD WINAPI UnloadCleanupThread(LPVOID param) {
  HMODULE hMod = (HMODULE)param;

  clickerEnabled = false;
  rightClickerEnabled = false;
  fastPlaceEnabled = false;
  blockhitEnabled = false;
  hitSelectEnabled = false;
  reachEnabled = false;
  velocityEnabled = false;
  sprintEnabled = false;
  throwpotEnabled = false;
  autosoupEnabled = false;
  refillEnabled = false;
  arraylistEnabled = false;
  arraylistWatermark = false;
  aimbotEnabled = false;
  aimTestEnabled = false;
  xrayEnabled = false;
  espEnabled = false;
  chestEspEnabled = false;
  tracerEnabled = false;
  wallhacks = false;
  g_throwpotKeyHeld = false;
  g_autosoupKeyHeld = false;
  showMenu = false;

  Sleep(500);

  WaitForModuleThreads();

  if (g_jvm) {
    JNIEnv *env = nullptr;
    bool attached = false;
    jint envResult = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
    if (envResult == JNI_EDETACHED) {
      if (g_jvm->AttachCurrentThread((void **)&env, nullptr) == JNI_OK)
        attached = true;
    } else if (envResult == JNI_OK) {

    } else {
      env = nullptr;
    }

    if (env) {
      if (g_aimLockedEntity) {
        env->DeleteGlobalRef(g_aimLockedEntity);
        g_aimLockedEntity = nullptr;
      }
      if (g_mcClass) {
        env->DeleteGlobalRef((jobject)g_mcClass);
        g_mcClass = nullptr;
      }
      if (g_entityClass) {
        env->DeleteGlobalRef((jobject)g_entityClass);
        g_entityClass = nullptr;
      }
      if (g_entityLivingClass) {
        env->DeleteGlobalRef((jobject)g_entityLivingClass);
        g_entityLivingClass = nullptr;
      }
      if (g_entityPlayerClass) {
        env->DeleteGlobalRef((jobject)g_entityPlayerClass);
        g_entityPlayerClass = nullptr;
      }
      if (g_worldClass) {
        env->DeleteGlobalRef((jobject)g_worldClass);
        g_worldClass = nullptr;
      }
      if (g_pcmpClass) {
        env->DeleteGlobalRef((jobject)g_pcmpClass);
        g_pcmpClass = nullptr;
      }
      if (g_itemSwordClass) {
        env->DeleteGlobalRef((jobject)g_itemSwordClass);
        g_itemSwordClass = nullptr;
      }
      if (g_inventoryClass) {
        env->DeleteGlobalRef((jobject)g_inventoryClass);
        g_inventoryClass = nullptr;
      }
      if (g_itemPotionClass) {
        env->DeleteGlobalRef((jobject)g_itemPotionClass);
        g_itemPotionClass = nullptr;
      }
      if (g_c03PacketClass) {
        env->DeleteGlobalRef((jobject)g_c03PacketClass);
        g_c03PacketClass = nullptr;
      }
      if (g_c03PosClass) {
        env->DeleteGlobalRef((jobject)g_c03PosClass);
        g_c03PosClass = nullptr;
      }
      if (g_itemBlockClass) {
        env->DeleteGlobalRef((jobject)g_itemBlockClass);
        g_itemBlockClass = nullptr;
      }
      if (g_itemClass) {
        env->DeleteGlobalRef((jobject)g_itemClass);
        g_itemClass = nullptr;
      }
      if (g_mcClassLoader) {
        env->DeleteGlobalRef(g_mcClassLoader);
        g_mcClassLoader = nullptr;
      }

      if (attached)
        g_jvm->DetachCurrentThread();
    }

    g_getMinecraft = nullptr;
    g_fThePlayer = nullptr;
    g_fTheWorld = nullptr;
    g_fPlayerController = nullptr;
    g_fPosX = nullptr;
    g_fPosY = nullptr;
    g_fPosZ = nullptr;
    g_fIsDead = nullptr;
    g_fRotYaw = nullptr;
    g_fRotPitch = nullptr;
    g_fPrevRotYaw = nullptr;
    g_fPrevRotPitch = nullptr;
    g_fRotYawHead = nullptr;
    g_fRotPitchHead = nullptr;
    g_fRenderYawOffset = nullptr;
    g_mOnPlayerRightClick = nullptr;
    g_vec3Class = nullptr;
    g_vec3Init = nullptr;
    g_enumFacingClass = nullptr;
    for (int i = 0; i < 6; i++)
      g_enumFacingByIndex[i] = nullptr;
    g_mGetEyeHeight = nullptr;
    g_fEntityList = nullptr;
    g_mListSize = nullptr;
    g_mListGet = nullptr;
    g_mAttackEntity = nullptr;
    g_fBlockReach = nullptr;
    g_mSwingItem = nullptr;
    g_mGetCurrentItem = nullptr;
    g_mStackGetItem = nullptr;
    g_mStackGetDisplayName = nullptr;
    g_mGetName = nullptr;
    g_mGetArmorValue = nullptr;
    g_mGetItemInUseDuration = nullptr;
    g_loadClassMethod = nullptr;
    g_jniReady = false;
  }

  StopKeyboardThread();

  RestoreWndProc();

  CleanupImGui();

  Sleep(200);

  DetachAllHooks();

  Sleep(300);
  if (hMod) {
    FreeLibraryAndExitThread(hMod, 0);
  }
  return 0;
}

void UnloadDLL() {
  if (g_isUnloading)
    return;
  g_isUnloading = true;

  HMODULE hMod = g_hModule;
  g_hModule = nullptr;
  HANDLE hThread = CreateThread(nullptr, 0, UnloadCleanupThread, hMod, 0, nullptr);
  if (hThread)
    CloseHandle(hThread);
}

DWORD __stdcall keyDispatcher(LPVOID thread) {
  keyHookHandle =
      SetWindowsHookEx(WH_KEYBOARD, keyHandler, GetModuleHandle(nullptr),
                       *reinterpret_cast<DWORD *>(thread));

  if (!keyHookHandle) {
    return GetLastError();
  }

  MSG message{};
  while (!g_isUnloading && GetMessage(&message, 0, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  if (keyHookHandle) {
    UnhookWindowsHookEx(keyHookHandle);
    keyHookHandle = nullptr;
  }

  return 0;
}

bool IsCursorVisible() {
  CURSORINFO ci{sizeof(CURSORINFO)};
  if (!GetCursorInfo(&ci))
    return false;

  if (!(ci.flags & CURSOR_SHOWING))
    return false;

  const auto handle = ci.hCursor;

  if (handle == LoadCursor(nullptr, IDC_ARROW) ||
      handle == LoadCursor(nullptr, IDC_HAND) ||
      handle == LoadCursor(nullptr, IDC_IBEAM) ||
      handle == LoadCursor(nullptr, IDC_CROSS) ||
      handle == LoadCursor(nullptr, IDC_SIZEALL) ||
      (handle > (HCURSOR)50000 && handle < (HCURSOR)100000)) {
    return true;
  }

  return false;
}

void UpdateCursorState() {
  static bool lastState = false;
  bool currentState = IsCursorVisible();

  if (currentState != lastState) {
    g_cursorVisible = currentState;

    if (currentState) {

      DWORD now = (DWORD)GetTickCount64();
      g_isInventoryOpen =
          (g_lastEKeyTime > g_lastEscKeyTime) && (now - g_lastEKeyTime < 200);
    } else {
      g_isInventoryOpen = false;
    }

    lastState = currentState;
  }
}

void UpdateAimbotKeyState() {
  if (aimbotKeyMode) {
    aimbotKeyPressed = (GetAsyncKeyState(aimbotKey) & 0x8000) != 0;
  }
}

void drawCornerLine(float x1, float y1, float z1, float x2, float y2, float z2,
                    float cornerFrac) {
  float dx = x2 - x1, dy = y2 - y1, dz = z2 - z1;

  glVertex3f(x1, y1, z1);
  glVertex3f(x1 + dx * cornerFrac, y1 + dy * cornerFrac, z1 + dz * cornerFrac);

  glVertex3f(x2 - dx * cornerFrac, y2 - dy * cornerFrac, z2 - dz * cornerFrac);
  glVertex3f(x2, y2, z2);
}

void draw(Position &position, Radius radius, Color boxColor, bool drawESP,
          bool drawTracer, bool isPlayerESP = false) {
  if (position.modelview.empty() || position.projection.empty()) {
    return;
  }

  for (size_t i = 0; i < position.modelview.size(); i += 16) {
    if (i + 15 >= position.modelview.size() ||
        i + 15 >= position.projection.size()) {
      break;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(position.projection.data() + i);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(position.modelview.data() + i);

    if (drawESP) {

      if (g_currentShowOutline) {
        glLineWidth(g_currentOutlineWidth);
        glColor4f(g_currentOutlineColor.x, g_currentOutlineColor.y,
                  g_currentOutlineColor.z, g_currentOutlineColor.w);

        float hx = radius.x / 2, hy = radius.y / 2, hz = radius.z / 2;

        glBegin(GL_LINES);
        if (g_currentBoxStyle == 1) {

          float cf = g_currentCornerLength;

          drawCornerLine(-hx, -hy, hz, hx, -hy, hz, cf);
          drawCornerLine(-hx, hy, hz, hx, hy, hz, cf);
          drawCornerLine(-hx, -hy, hz, -hx, hy, hz, cf);
          drawCornerLine(hx, -hy, hz, hx, hy, hz, cf);

          drawCornerLine(-hx, -hy, -hz, hx, -hy, -hz, cf);
          drawCornerLine(-hx, hy, -hz, hx, hy, -hz, cf);
          drawCornerLine(-hx, -hy, -hz, -hx, hy, -hz, cf);
          drawCornerLine(hx, -hy, -hz, hx, hy, -hz, cf);

          drawCornerLine(-hx, -hy, hz, -hx, -hy, -hz, cf);
          drawCornerLine(hx, -hy, hz, hx, -hy, -hz, cf);
          drawCornerLine(-hx, hy, hz, -hx, hy, -hz, cf);
          drawCornerLine(hx, hy, hz, hx, hy, -hz, cf);
        } else {

          glVertex3f(hx, -hy, hz);
          glVertex3f(hx, hy, hz);
          glVertex3f(-hx, -hy, hz);
          glVertex3f(hx, -hy, hz);
          glVertex3f(-hx, -hy, hz);
          glVertex3f(-hx, hy, hz);
          glVertex3f(-hx, hy, hz);
          glVertex3f(hx, hy, hz);
          glVertex3f(hx, hy, hz);
          glVertex3f(hx, hy, -hz);
          glVertex3f(hx, hy, -hz);
          glVertex3f(hx, -hy, -hz);
          glVertex3f(hx, hy, -hz);
          glVertex3f(-hx, hy, -hz);
          glVertex3f(-hx, hy, -hz);
          glVertex3f(-hx, hy, hz);
          glVertex3f(-hx, hy, -hz);
          glVertex3f(-hx, -hy, -hz);
          glVertex3f(-hx, -hy, -hz);
          glVertex3f(hx, -hy, -hz);
          glVertex3f(-hx, -hy, -hz);
          glVertex3f(-hx, -hy, hz);
          glVertex3f(hx, -hy, -hz);
          glVertex3f(hx, -hy, hz);
        }
        glEnd();
      }

      if (g_currentShowFill || (isPlayerESP && espUseImage && espImageLoaded && espImageTexture)) {
        float hx = radius.x / 2, hy = radius.y / 2, hz = radius.z / 2;

        if (isPlayerESP && espUseImage && espImageLoaded && espImageTexture) {

          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, espImageTexture);

          GLint boundTexture;
          glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
          if (boundTexture != (GLint)espImageTexture) {

            glDisable(GL_TEXTURE_2D);
          } else {

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glColor4f(1.0f, 1.0f, 1.0f, g_currentShowFill ? g_currentFillColor.w : 1.0f);

            float texLeft = 0.0f, texRight = 1.0f, texTop = 0.0f, texBottom = 1.0f;

            if (!espImageStretch) {

              float imgAspect = (float)espImageWidth / (float)espImageHeight;
              float boxAspect = (radius.x * radius.z) / (radius.y * radius.y);

              if (imgAspect > boxAspect) {

                float adjustedHeight = 1.0f / imgAspect;
                texTop = (1.0f - adjustedHeight) * 0.5f;
                texBottom = texTop + adjustedHeight;
              } else {

                float adjustedWidth = imgAspect;
                texLeft = (1.0f - adjustedWidth) * 0.5f;
                texRight = texLeft + adjustedWidth;
              }
            }

            glBegin(GL_QUADS);
            glTexCoord2f(texRight, texTop);    glVertex3f( hx,  hy, -hz);
            glTexCoord2f(texRight, texBottom); glVertex3f( hx, -hy, -hz);
            glTexCoord2f(texLeft,  texBottom); glVertex3f(-hx, -hy, -hz);
            glTexCoord2f(texLeft,  texTop);    glVertex3f(-hx,  hy, -hz);
            glEnd();

            glBegin(GL_QUADS);
            glTexCoord2f(texLeft,  texTop);    glVertex3f( hx,  hy,  hz);
            glTexCoord2f(texLeft,  texBottom); glVertex3f( hx, -hy,  hz);
            glTexCoord2f(texRight, texBottom); glVertex3f(-hx, -hy,  hz);
            glTexCoord2f(texRight, texTop);    glVertex3f(-hx,  hy,  hz);
            glEnd();

            glBegin(GL_QUADS);
            glTexCoord2f(texRight, texTop);    glVertex3f(hx,  hy, -hz);
            glTexCoord2f(texRight, texBottom); glVertex3f(hx, -hy, -hz);
            glTexCoord2f(texLeft,  texBottom); glVertex3f(hx, -hy,  hz);
            glTexCoord2f(texLeft,  texTop);    glVertex3f(hx,  hy,  hz);
            glEnd();

            glBegin(GL_QUADS);
            glTexCoord2f(texLeft,  texTop);    glVertex3f(-hx,  hy,  hz);
            glTexCoord2f(texLeft,  texBottom); glVertex3f(-hx, -hy,  hz);
            glTexCoord2f(texRight, texBottom); glVertex3f(-hx, -hy, -hz);
            glTexCoord2f(texRight, texTop);    glVertex3f(-hx,  hy, -hz);
            glEnd();

            glBegin(GL_QUADS);
            glTexCoord2f(texRight, texTop);    glVertex3f( hx, hy, -hz);
            glTexCoord2f(texRight, texBottom); glVertex3f( hx, hy,  hz);
            glTexCoord2f(texLeft,  texBottom); glVertex3f(-hx, hy,  hz);
            glTexCoord2f(texLeft,  texTop);    glVertex3f(-hx, hy, -hz);
            glEnd();

            glBegin(GL_QUADS);
            glTexCoord2f(texRight, texBottom); glVertex3f( hx, -hy,  hz);
            glTexCoord2f(texRight, texTop);    glVertex3f( hx, -hy, -hz);
            glTexCoord2f(texLeft,  texTop);    glVertex3f(-hx, -hy, -hz);
            glTexCoord2f(texLeft,  texBottom); glVertex3f(-hx, -hy,  hz);
            glEnd();

            glDisable(GL_TEXTURE_2D);
          }
        } else if (g_currentShowFill) {

          glColor4f(g_currentFillColor.x, g_currentFillColor.y,
                    g_currentFillColor.z, g_currentFillColor.w);

          glBegin(GL_POLYGON);
          glVertex3f(hx, hy, -hz);
          glVertex3f(hx, -hy, -hz);
          glVertex3f(-hx, -hy, -hz);
          glVertex3f(-hx, hy, -hz);
          glEnd();

          glBegin(GL_POLYGON);
          glVertex3f(hx, hy, hz);
          glVertex3f(hx, -hy, hz);
          glVertex3f(-hx, -hy, hz);
          glVertex3f(-hx, hy, hz);
          glEnd();

          glBegin(GL_POLYGON);
          glVertex3f(hx, hy, -hz);
          glVertex3f(hx, -hy, -hz);
          glVertex3f(hx, -hy, hz);
          glVertex3f(hx, hy, hz);
          glEnd();

          glBegin(GL_POLYGON);
          glVertex3f(-hx, hy, hz);
          glVertex3f(-hx, -hy, hz);
          glVertex3f(-hx, -hy, -hz);
          glVertex3f(-hx, hy, -hz);
          glEnd();

          glBegin(GL_POLYGON);
          glVertex3f(hx, -hy, hz);
          glVertex3f(hx, -hy, -hz);
          glVertex3f(-hx, -hy, -hz);
          glVertex3f(-hx, -hy, hz);
          glEnd();

          glBegin(GL_POLYGON);
          glVertex3f(hx, hy, -hz);
          glVertex3f(hx, hy, hz);
          glVertex3f(-hx, hy, hz);
          glVertex3f(-hx, hy, -hz);
          glEnd();
        }
      }
    }

    if (drawTracer) {
      glLoadIdentity();
      glLineWidth(tracerWidth);
      glColor4f(tracerColor.x, tracerColor.y, tracerColor.z, tracerColor.w);
      glBegin(GL_LINES);

      float originY = 0.0f;
      if (tracerOrigin == 0)
        originY = 0.5f;
      else if (tracerOrigin == 1)
        originY = 0.0f;
      else if (tracerOrigin == 2)
        originY = -0.5f;
      glVertex3f(0, originY, -0.1F);
      if (i + 14 < position.modelview.size()) {
        glVertex3f((position.modelview.data() + i)[12],
                   (position.modelview.data() + i)[13],
                   (position.modelview.data() + i)[14]);
      }
      glEnd();
    }
  }
}

void savePosition(Position &position, float offsetX, float offsetY,
                  float offsetZ) {
  float projection[16]{};
  glGetFloatv(GL_PROJECTION_MATRIX, projection);

  float modelview[16]{};
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

  float m3[4]{};
  for (int i = 0; i < 4; ++i) {
    m3[i] = modelview[i] * offsetX + modelview[i + 4] * offsetY +
            modelview[i + 8] * offsetZ + modelview[i + 12];
  }

  memcpy(modelview + 12, m3, sizeof(m3));

  position.projection.insert(position.projection.end(), std::begin(projection),
                             std::end(projection));
  position.modelview.insert(position.modelview.end(), std::begin(modelview),
                            std::end(modelview));
}

LRESULT CALLBACK hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (g_isUnloading || !g_originalWndProc) {
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  switch (msg) {
  case WM_SIZE:
  case WM_DISPLAYCHANGE:
  case WM_WINDOWPOSCHANGED:
    g_fullscreenDetected = true;
    break;

  case WM_SETCURSOR:
    if (showMenu && g_imguiInitialized) {
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      return TRUE;
    }
    break;
  }

  if (g_imguiInitialized && showMenu) {
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

    switch (msg) {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
      return 0;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:

      if (g_isCapturingKey && msg == WM_KEYDOWN) {
        UpdateKeyCapture((int)wParam);
        return 0;
      }

      if (!showMenu && msg == WM_KEYDOWN) {

      }

      if (wParam != VK_INSERT) {
        return 0;
      }
      break;
    }
  }

  if (msg == WM_LBUTTONDOWN && !showMenu && hitSelectEnabled &&
      !g_isUnloading) {

    if (hitSelectSwordOnly && !g_holdingSword) {
      return CallWindowProc(g_originalWndProc, hWnd, msg, wParam, lParam);
    }

    static std::mt19937 s_hsRng(std::random_device{}());
    bool shouldCancel = false;
    DWORD now = (DWORD)GetTickCount64();

    if (g_hsAimingAtEntity) {

      bool targetCanTakeDamage;
      if (hitSelectUseServerAttackTime) {

        targetCanTakeDamage = (g_hsTargetHurtTime == 0);

        if (g_hsTargetJustDamaged) {
          g_hsTargetJustDamaged = false;
          targetCanTakeDamage = true;
        }
      } else {

        targetCanTakeDamage = (g_hsTargetHurtResist <= 10);
      }

      if (hitSelectWaitForFirstHit > 0 && !g_hsFirstHitReceived) {
        DWORD elapsed = (g_hsCombatStartTime > 0) ? (now - g_hsCombatStartTime) : 0;
        if (elapsed < (DWORD)hitSelectWaitForFirstHit) {
          shouldCancel = true;
        }
      }

      if (!shouldCancel && hitSelectHitLaterInTrades > 0 &&
          g_hsFirstHitReceived) {
        DWORD sinceDamage = (g_hsLastDamageTime > 0) ? (now - g_hsLastDamageTime) : 0xFFFFFFFF;
        if (sinceDamage < (DWORD)hitSelectHitLaterInTrades) {
          shouldCancel = true;
        }
      }

      if (shouldCancel && hitSelectPauseDuration > 0 && g_hsLastAttackTime > 0) {
        DWORD sinceLastAttack = now - g_hsLastAttackTime;
        if (sinceLastAttack >= (DWORD)hitSelectPauseDuration) {
          shouldCancel = false;
        }
      }

      if (!shouldCancel) {
        if (hitSelectMode == 0) {

          if (!targetCanTakeDamage) {
            std::uniform_int_distribution<int> chanceDist(1, 100);
            if (chanceDist(s_hsRng) <= hitSelectCancelRateInCombat) {
              shouldCancel = true;
            }
          }
        } else if (hitSelectMode == 1) {

          bool onGround = g_hsPlayerOnGround;

          if (!targetCanTakeDamage) {

            std::uniform_int_distribution<int> chanceDist(1, 100);
            if (chanceDist(s_hsRng) <= hitSelectCancelRateInCombat) {
              shouldCancel = true;
            }
          } else if (onGround) {

          } else {

            bool isFalling = (g_hsMotionY < -0.08);

            if (hitSelectDisableDuringKB && g_hsTakingKB)
              isFalling = true;

            if (hitSelectOnlyWhileDamaged && !g_hsFirstHitReceived)
              isFalling = true;

            if (!isFalling) {
              std::uniform_int_distribution<int> chanceDist(1, 100);
              if (chanceDist(s_hsRng) <= hitSelectCancelRateInCombat) {
                shouldCancel = true;
              }
            }
          }
        }
      }

      if (shouldCancel && hitSelectPauseDuration > 0 && g_hsLastAttackTime > 0) {
        DWORD sinceLastAttack = now - g_hsLastAttackTime;
        if (sinceLastAttack >= (DWORD)hitSelectPauseDuration) {
          shouldCancel = false;
        }
      }

      if (!shouldCancel) {
        g_hsLastAttackTime = now;
        if (g_hsCombatStartTime == 0)
          g_hsCombatStartTime = now;
      }
    } else {

      if (hitSelectCancelRateMissed > 0) {
        std::uniform_int_distribution<int> chanceDist(1, 100);
        if (chanceDist(s_hsRng) <= hitSelectCancelRateMissed) {
          shouldCancel = true;
        }
      }
    }

    if (shouldCancel) {
      if (hitSelectFakeSwing) {
        g_hsNeedFakeSwing = true;
      }
      return 0;
    }
  }

  return CallWindowProc(g_originalWndProc, hWnd, msg, wParam, lParam);
}

void ReinitializeImGuiIfNeeded(HWND window) {
  if (g_isUnloading || !g_imguiInitialized)
    return;

  bool needsReinit = false;

  if (window != g_imguiHwnd) {
    needsReinit = true;
  }

  bool isFullscreen = IsWindowFullscreen(window);
  if (isFullscreen != g_wasFullscreen) {
    g_wasFullscreen = isFullscreen;
    needsReinit = true;
  }

  if (needsReinit) {

    if (g_wndProcHooked && g_originalWndProc && g_imguiHwnd) {
      SetWindowLongPtr(g_imguiHwnd, GWLP_WNDPROC, (LONG_PTR)g_originalWndProc);
      g_wndProcHooked = false;
      g_originalWndProc = nullptr;
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplWin32_Init(window);
    ImGui_ImplOpenGL2_Init();
    g_imguiHwnd = window;
    g_themeApplied = false;
  }
}

void InitializeKeyBinds() {
  g_keybinds.clear();
  g_keybinds.push_back({clickerHotkeyCode, "LeftClicker", &clickerEnabled, false,
                        "Left Clicker Toggle"});

  g_keybinds.push_back({0, "Aim Assist", &aimbotEnabled, false, "Aim Assist Toggle"});
  g_keybinds.push_back({0, "Blockhit", &blockhitEnabled, false, "Blockhit Toggle"});
  g_keybinds.push_back({0, "Reach", &reachEnabled, false, "Reach Toggle"});
  g_keybinds.push_back({0, "Hit Select", &hitSelectEnabled, false, "Hit Select Toggle"});
  g_keybinds.push_back({0, "STap", &sTapEnabled, false, "S-Tap Toggle"});
  g_keybinds.push_back({0, "WTap", &wTapEnabled, false, "W-Tap Toggle"});
  g_keybinds.push_back({0, "BowAimbot", &bowAimbotEnabled, false, "Bow Aimbot Toggle"});

  g_keybinds.push_back({0, "Velocity", &velocityEnabled, false, "Velocity Toggle"});
  g_keybinds.push_back({0, "Sprint", &sprintEnabled, false, "Sprint Toggle"});
  g_keybinds.push_back({0, "SprintReset", &sprintResetEnabled, false, "Sprint Reset Toggle"});
  g_keybinds.push_back({0, "NoFall", &noFallEnabled, false, "No Fall Toggle"});
  g_keybinds.push_back({0, "InvWalk", &invWalkEnabled, false, "Inv Walk Toggle"});
  g_keybinds.push_back({0, "JumpReset", &jumpResetEnabled, false, "Jump Reset Toggle"});
  g_keybinds.push_back({0, "NoJumpDelay", &noJumpDelayEnabled, false, "No Jump Delay Toggle"});
  g_keybinds.push_back({0, "Scaffold", &scaffoldEnabled, false, "Scaffold Toggle"});
  g_keybinds.push_back({0, "Step", &stepEnabled, false, "Step Toggle"});
  g_keybinds.push_back({0, "Spider", &spiderEnabled, false, "Spider Toggle"});
  g_keybinds.push_back({0, "SafeWalk", &safeWalkEnabled, false, "SafeWalk Toggle"});
  g_keybinds.push_back({0, "ReverseStep", &reverseStepEnabled, false, "Reverse Step Toggle"});
  g_keybinds.push_back({0, "BouncySlime", &bouncySlimeEnabled, false, "BouncySlime Toggle"});
  g_keybinds.push_back({0, "FastStop", &fastStopEnabled, false, "FastStop Toggle"});
  g_keybinds.push_back({0, "Glide", &glideEnabled, false, "Glide Toggle"});
  g_keybinds.push_back({0, "Flight", &flightEnabled, false, "Flight Toggle"});
  g_keybinds.push_back({0, "AirJump", &airJumpEnabled, false, "Air Jump Toggle"});
  g_keybinds.push_back({0, "HighJump", &highJumpEnabled, false, "High Jump Toggle"});

  g_keybinds.push_back({0, "ESP", &espEnabled, false, "ESP Toggle"});
  g_keybinds.push_back({0, "Tracer", &tracerEnabled, false, "Tracer Toggle"});
  g_keybinds.push_back({0, "XRay", &xrayEnabled, false, "XRay Toggle"});
  g_keybinds.push_back({0, "StorageESP", &chestEspEnabled, false, "StorageESP Toggle"});
  g_keybinds.push_back({0, "Chams", &chamsEnabled, false, "Chams Toggle"});
  g_keybinds.push_back({0, "TargetHUD", &targetHudEnabled, false, "TargetHUD Toggle"});
  g_keybinds.push_back({0, "Freecam", &freecamEnabled, false, "Freecam Toggle"});
  g_keybinds.push_back({0, "FullBright", &fullBrightEnabled, false, "FullBright Toggle"});
  g_keybinds.push_back({0, "NoHurtCam", &noHurtCamEnabled, false, "NoHurtCam Toggle"});
  g_keybinds.push_back({0, "Zoom", &zoomEnabled, false, "Zoom Toggle"});
  g_keybinds.push_back({0, "ArmorESP", &armorEspEnabled, false, "ArmorESP Toggle"});
  g_keybinds.push_back({0, "ItemESP", &itemEspEnabled, false, "ItemESP Toggle"});
  g_keybinds.push_back({0, "HoleESP", &holeEspEnabled, false, "HoleESP Toggle"});

  g_keybinds.push_back({0, "UIPresets", &uiPresetsEnabled, false, "UI Presets Toggle"});

  g_keybinds.push_back({0, "Blink", &blinkEnabled, false, "Blink Toggle"});
  g_keybinds.push_back({0, "Timer", &timerEnabled, false, "Timer Toggle"});
  g_keybinds.push_back({0, "FakeLag", &fakeLagEnabled, false, "Fake Lag Toggle"});

  g_keybinds.push_back({0, "RightClicker", &rightClickerEnabled, false, "Right Clicker Toggle"});
  g_keybinds.push_back({0, "FastPlace", &fastPlaceEnabled, false, "Fast Place Toggle"});
  g_keybinds.push_back({0, "Throwpot", &throwpotEnabled, false, "Throwpot Toggle"});
  g_keybinds.push_back({0, "AutoSoup", &autosoupEnabled, false, "AutoSoup Toggle"});
  g_keybinds.push_back({0, "Refill", &refillEnabled, false, "Refill Toggle"});
  g_keybinds.push_back({0, "AutoTool", &autoToolEnabled, false, "Auto Tool Toggle"});
  g_keybinds.push_back({0, "BridgeAssist", &bridgeAssistEnabled, false, "Bridge Assist Toggle"});
  g_keybinds.push_back({0, "Teams", &teamsEnabled, false, "Teams Toggle"});
  g_keybinds.push_back({0, "Friends", &friendsEnabled, false, "Friends Toggle"});
  g_keybinds.push_back({0, "AutoSword", &autoSwordEnabled, false, "Auto Sword Toggle"});
  g_keybinds.push_back({0, "AntiVoid", &antiVoidEnabled, false, "Anti Void Toggle"});
  g_keybinds.push_back({0, "MCFriend", &middleClickFriendEnabled, false, "MC Friend Toggle"});
  g_keybinds.push_back({0, "AntiBot", &antiBotEnabled, false, "Anti Bot Toggle"});
  g_keybinds.push_back({0, "AutoArmor", &autoArmorEnabled, false, "Auto Armor Toggle"});
  g_keybinds.push_back({0, "InvManager", &inventoryManagerEnabled, false, "Inv Manager Toggle"});

  g_keybinds.push_back({0, "BreadCrumbs", &breadCrumbsEnabled, false, "BreadCrumbs Toggle"});
  g_keybinds.push_back({0, "AntiAFK", &antiAfkEnabled, false, "AntiAFK Toggle"});
  g_keybinds.push_back({0, "AntiBadFX", &antiBadEffectsEnabled, false, "Anti Bad Effects Toggle"});
  g_keybinds.push_back({0, "Panic", &panicEnabled, false, "Panic Toggle"});
  g_keybinds.push_back({0, "PingSpoof", &pingSpoofModEnabled, false, "Ping Spoof Toggle"});
  g_keybinds.push_back({0, "Reconnect", &reconnectEnabled, false, "Reconnect Toggle"});
  g_keybinds.push_back({0, "StaffNotif", &staffNotifierEnabled, false, "Staff Notifier Toggle"});
  g_keybinds.push_back({0, "NoRotate", &noRotateEnabled, false, "NoRotate Toggle"});
  g_keybinds.push_back({0, "AntiCactus", &antiCactusEnabled, false, "AntiCactus Toggle"});
}

void StartKeyCapture(int index) {
  g_isCapturingKey = true;
  g_capturingIndex = index;
}

void StopKeyCapture() {
  g_isCapturingKey = false;
  g_capturingIndex = -1;
}

void UpdateKeyCapture(int key) {
  if (g_isCapturingKey && g_capturingIndex >= 0 &&
      g_capturingIndex < g_keybinds.size()) {
    g_keybinds[g_capturingIndex].virtualKey = key;
    g_keybinds[g_capturingIndex].isWaitingForKey = false;

    StopKeyCapture();
  }
}

void UpdateEnabledModulesList() {
  enabledModules.clear();

  if (clickerEnabled) enabledModules.push_back("Left Clicker");
  if (aimbotEnabled) enabledModules.push_back("Aim Assist");
  if (blockhitEnabled) enabledModules.push_back("Blockhit");
  if (reachEnabled) enabledModules.push_back("Reach");
  if (hitSelectEnabled) enabledModules.push_back("Hit Select");
  if (sTapEnabled) enabledModules.push_back("S-Tap");
  if (wTapEnabled) enabledModules.push_back("W-Tap");
  if (bowAimbotEnabled) enabledModules.push_back("Bow Aimbot");

  if (velocityEnabled) enabledModules.push_back("Velocity");
  if (sprintEnabled) enabledModules.push_back("Sprint");
  if (sprintResetEnabled) enabledModules.push_back("Sprint Reset");
  if (noFallEnabled) enabledModules.push_back("No Fall");
  if (invWalkEnabled) enabledModules.push_back("Inv Walk");
  if (jumpResetEnabled) enabledModules.push_back("Jump Reset");
  if (noJumpDelayEnabled) enabledModules.push_back("No Jump Delay");
  if (scaffoldEnabled) enabledModules.push_back("Scaffold");
  if (stepEnabled) enabledModules.push_back("Step");
  if (spiderEnabled) enabledModules.push_back("Spider");
  if (safeWalkEnabled) enabledModules.push_back("SafeWalk");
  if (reverseStepEnabled) enabledModules.push_back("Reverse Step");
  if (bouncySlimeEnabled) enabledModules.push_back("BouncySlime");
  if (fastStopEnabled) enabledModules.push_back("FastStop");
  if (glideEnabled) enabledModules.push_back("Glide");
  if (flightEnabled) enabledModules.push_back("Flight");
  if (airJumpEnabled) enabledModules.push_back("Air Jump");
  if (highJumpEnabled) enabledModules.push_back("High Jump");

  if (espEnabled) enabledModules.push_back("ESP");
  if (tracerEnabled) enabledModules.push_back("Tracer");
  if (xrayEnabled) enabledModules.push_back("XRay");
  if (chestEspEnabled) enabledModules.push_back("StorageESP");
  if (chamsEnabled) enabledModules.push_back("Chams");
  if (targetHudEnabled) enabledModules.push_back("TargetHUD");
  if (freecamEnabled) enabledModules.push_back("Freecam");
  if (fullBrightEnabled) enabledModules.push_back("FullBright");
  if (noHurtCamEnabled) enabledModules.push_back("NoHurtCam");
  if (zoomEnabled) enabledModules.push_back("Zoom");
  if (armorEspEnabled) enabledModules.push_back("ArmorESP");
  if (itemEspEnabled) enabledModules.push_back("ItemESP");
  if (holeEspEnabled) enabledModules.push_back("HoleESP");

  if (uiPresetsEnabled) enabledModules.push_back("UI Presets");

  if (blinkEnabled) enabledModules.push_back("Blink");
  if (timerEnabled) enabledModules.push_back("Timer");
  if (fakeLagEnabled) enabledModules.push_back("Fake Lag");

  if (rightClickerEnabled) enabledModules.push_back("Right Clicker");
  if (fastPlaceEnabled) enabledModules.push_back("Fast Place");
  if (throwpotEnabled) enabledModules.push_back("Throwpot");
  if (autosoupEnabled) enabledModules.push_back("AutoSoup");
  if (refillEnabled) enabledModules.push_back("Refill");
  if (autoToolEnabled) enabledModules.push_back("Auto Tool");
  if (bridgeAssistEnabled) enabledModules.push_back("Bridge Assist");
  if (teamsEnabled) enabledModules.push_back("Teams");
  if (friendsEnabled) enabledModules.push_back("Friends");
  if (autoSwordEnabled) enabledModules.push_back("Auto Sword");
  if (antiVoidEnabled) enabledModules.push_back("Anti Void");
  if (middleClickFriendEnabled) enabledModules.push_back("MC Friend");
  if (antiBotEnabled) enabledModules.push_back("Anti Bot");
  if (autoArmorEnabled) enabledModules.push_back("Auto Armor");
  if (inventoryManagerEnabled) enabledModules.push_back("Inv Manager");

  if (breadCrumbsEnabled) enabledModules.push_back("BreadCrumbs");
  if (antiAfkEnabled) enabledModules.push_back("AntiAFK");
  if (antiBadEffectsEnabled) enabledModules.push_back("Anti Bad Effects");
  if (panicEnabled) enabledModules.push_back("Panic");
  if (pingSpoofModEnabled) enabledModules.push_back("Ping Spoof");
  if (reconnectEnabled) enabledModules.push_back("Reconnect");
  if (staffNotifierEnabled) enabledModules.push_back("Staff Notifier");
  if (noRotateEnabled) enabledModules.push_back("NoRotate");
  if (antiCactusEnabled) enabledModules.push_back("AntiCactus");
}

void executeClick(bool isLeft, HWND targetWindow) {
  PostMessage(targetWindow, isLeft ? WM_LBUTTONDOWN : WM_RBUTTONDOWN, 0, 0);
  Sleep(1);
  PostMessage(targetWindow, isLeft ? WM_LBUTTONUP : WM_RBUTTONUP, 0, 0);
  totalClicks++;
  if (isLeft && sprintResetEnabled) {
    g_lastAttackTime = (DWORD)GetTickCount64();
  }
}

void executeRightClick(HWND targetWindow, int holdTimeMs) {
  PostMessage(targetWindow, WM_RBUTTONDOWN, 0, 0);
  Sleep(holdTimeMs);
  PostMessage(targetWindow, WM_RBUTTONUP, 0, 0);
}

double getRandomizedCps(double baseCps, bool randomize, double randomAmount) {
  if (!randomize) {
    return baseCps;
  }

  static std::random_device rd;
  static std::mt19937 gen(rd());

  double minCps = (std::max)(1.0, baseCps - randomAmount);
  double maxCps = (std::min)(20.0, baseCps + randomAmount);

  std::uniform_real_distribution<double> dis(minCps, maxCps);
  return dis(gen);
}

bool shouldApplyExhaust() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 100);
  int roll = dis(gen);
  return roll <= clickerExhaustChance;
}

bool shouldApplySpike() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 100);
  int roll = dis(gen);
  return roll <= clickerSpikeChance;
}

static jfieldID TryFieldN(JNIEnv *e, jclass c, const char **names, int count,
                          const char *sig) {
  for (int i = 0; i < count; i++) {
    jfieldID f = e->GetFieldID(c, names[i], sig);
    if (f)
      return f;
    e->ExceptionClear();
  }
  return nullptr;
}

static jmethodID TryMethodN(JNIEnv *e, jclass c, const char **names, int count,
                            const char *sig) {
  for (int i = 0; i < count; i++) {
    jmethodID m = e->GetMethodID(c, names[i], sig);
    if (m)
      return m;
    e->ExceptionClear();
  }
  return nullptr;
}

static jmethodID TryStaticMethodN(JNIEnv *e, jclass c, const char **names,
                                  int count, const char *sig) {
  for (int i = 0; i < count; i++) {
    jmethodID m = e->GetStaticMethodID(c, names[i], sig);
    if (m)
      return m;
    e->ExceptionClear();
  }
  return nullptr;
}

static jclass LoadMCClass(JNIEnv *env, const char **dotNames, int nameCount) {
  if (!g_mcClassLoader || !g_loadClassMethod)
    return nullptr;
  for (int i = 0; i < nameCount; i++) {
    jstring jName = env->NewStringUTF(dotNames[i]);
    jclass cls = (jclass)env->CallObjectMethod(g_mcClassLoader,
                                               g_loadClassMethod, jName);
    env->DeleteLocalRef(jName);
    if (cls && !env->ExceptionCheck())
      return cls;
    env->ExceptionClear();
  }
  return nullptr;
}

static bool FindMCClassLoader(JNIEnv *env, const char *testClassName) {

  jclass threadClass = env->FindClass("java/lang/Thread");
  jclass mapClass = env->FindClass("java/util/Map");
  jclass setClass = env->FindClass("java/util/Set");
  jclass clLoaderClass = env->FindClass("java/lang/ClassLoader");
  if (!threadClass || !mapClass || !setClass || !clLoaderClass) {
    env->ExceptionClear();
    return false;
  }

  jmethodID getAllTraces = env->GetStaticMethodID(
      threadClass, "getAllStackTraces", "()Ljava/util/Map;");
  jmethodID keySet = env->GetMethodID(mapClass, "keySet", "()Ljava/util/Set;");
  jmethodID toArray =
      env->GetMethodID(setClass, "toArray", "()[Ljava/lang/Object;");
  jmethodID getContextCL = env->GetMethodID(
      threadClass, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
  g_loadClassMethod = env->GetMethodID(clLoaderClass, "loadClass",
                                       "(Ljava/lang/String;)Ljava/lang/Class;");

  if (!getAllTraces || !keySet || !toArray || !getContextCL ||
      !g_loadClassMethod) {
    env->ExceptionClear();
    return false;
  }

  jobject traceMap = env->CallStaticObjectMethod(threadClass, getAllTraces);
  if (!traceMap || env->ExceptionCheck()) {
    env->ExceptionClear();
    return false;
  }

  jobject keys = env->CallObjectMethod(traceMap, keySet);
  jobjectArray threadArr = (jobjectArray)env->CallObjectMethod(keys, toArray);
  env->DeleteLocalRef(keys);
  env->DeleteLocalRef(traceMap);
  if (!threadArr) {
    env->ExceptionClear();
    return false;
  }

  jstring testName = env->NewStringUTF(testClassName);
  jint threadCount = env->GetArrayLength(threadArr);

  for (jint i = 0; i < threadCount; i++) {
    jobject thread = env->GetObjectArrayElement(threadArr, i);
    if (!thread)
      continue;

    jobject cl = env->CallObjectMethod(thread, getContextCL);
    env->DeleteLocalRef(thread);
    if (!cl || env->ExceptionCheck()) {
      env->ExceptionClear();
      continue;
    }

    jclass mcTest =
        (jclass)env->CallObjectMethod(cl, g_loadClassMethod, testName);
    if (mcTest && !env->ExceptionCheck()) {

      g_mcClassLoader = env->NewGlobalRef(cl);
      g_mcClass = (jclass)env->NewGlobalRef(mcTest);
      env->DeleteLocalRef(mcTest);
      env->DeleteLocalRef(cl);
      env->DeleteLocalRef(testName);
      env->DeleteLocalRef(threadArr);
      env->DeleteLocalRef(threadClass);
      env->DeleteLocalRef(mapClass);
      env->DeleteLocalRef(setClass);
      env->DeleteLocalRef(clLoaderClass);
      return true;
    }
    env->ExceptionClear();
    env->DeleteLocalRef(cl);
  }

  env->DeleteLocalRef(testName);
  env->DeleteLocalRef(threadArr);
  env->DeleteLocalRef(threadClass);
  env->DeleteLocalRef(mapClass);
  env->DeleteLocalRef(setClass);
  env->DeleteLocalRef(clLoaderClass);
  return false;
}

bool InitJNIReach(JNIEnv **envOut) {
  HMODULE jvmDll = GetModuleHandleA("jvm.dll");
  if (!jvmDll) {
    reachStatus = "jvm.dll not found";
    return false;
  }

  typedef jint(JNICALL * PFN)(JavaVM **, jsize, jsize *);
  auto fnGetVMs = (PFN)GetProcAddress(jvmDll, "JNI_GetCreatedJavaVMs");
  if (!fnGetVMs) {
    reachStatus = "JNI_GetCreatedJavaVMs not found";
    return false;
  }

  jsize vmCount = 0;
  if (fnGetVMs(&g_jvm, 1, &vmCount) != JNI_OK || vmCount == 0) {
    reachStatus = "No JVM found";
    return false;
  }

  JNIEnv *env = nullptr;
  jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
  if (res == JNI_EDETACHED) {
    if (g_jvm->AttachCurrentThreadAsDaemon((void **)&env, nullptr) != JNI_OK) {
      reachStatus = "Failed to attach thread";
      return false;
    }
  } else if (res != JNI_OK) {
    reachStatus = "GetEnv failed";
    return false;
  }

  if (!FindMCClassLoader(env, "net.minecraft.client.Minecraft")) {
    reachStatus = "Minecraft class not found (scanned all thread classloaders)";
    return false;
  }

  const char *gmNames[] = {"func_71410_x", "getMinecraft"};
  g_getMinecraft = TryStaticMethodN(env, g_mcClass, gmNames, 2,
                                    "()Lnet/minecraft/client/Minecraft;");
  if (!g_getMinecraft) {

    jclass classRef = env->FindClass("java/lang/Class");
    jmethodID getDeclaredMethods = env->GetMethodID(
        classRef, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
    jmethodID classGetName =
        env->GetMethodID(classRef, "getName", "()Ljava/lang/String;");
    jobjectArray methods =
        (jobjectArray)env->CallObjectMethod(g_mcClass, getDeclaredMethods);
    env->DeleteLocalRef(classRef);

    if (methods) {
      jclass methodClass = env->FindClass("java/lang/reflect/Method");
      jmethodID getReturnType =
          env->GetMethodID(methodClass, "getReturnType", "()Ljava/lang/Class;");
      jmethodID getModifiers =
          env->GetMethodID(methodClass, "getModifiers", "()I");
      jmethodID getParamTypes = env->GetMethodID(
          methodClass, "getParameterTypes", "()[Ljava/lang/Class;");
      jmethodID getName =
          env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");

      jstring mcClassName =
          (jstring)env->CallObjectMethod(g_mcClass, classGetName);
      const char *mcCN = env->GetStringUTFChars(mcClassName, nullptr);
      std::string retSig = "()L";
      for (const char *p = mcCN; *p; p++)
        retSig += (*p == '.') ? '/' : *p;
      retSig += ";";
      env->ReleaseStringUTFChars(mcClassName, mcCN);
      env->DeleteLocalRef(mcClassName);

      jint len = env->GetArrayLength(methods);
      for (jint i = 0; i < len && !g_getMinecraft; i++) {
        jobject m = env->GetObjectArrayElement(methods, i);
        jint mods = env->CallIntMethod(m, getModifiers);
        if ((mods & 0x8) && (mods & 0x1)) {
          jobjectArray params =
              (jobjectArray)env->CallObjectMethod(m, getParamTypes);
          if (env->GetArrayLength(params) == 0) {
            jclass retType = (jclass)env->CallObjectMethod(m, getReturnType);
            if (env->IsSameObject(retType, g_mcClass)) {
              jstring mName = (jstring)env->CallObjectMethod(m, getName);
              const char *mn = env->GetStringUTFChars(mName, nullptr);
              env->ExceptionClear();
              g_getMinecraft =
                  env->GetStaticMethodID(g_mcClass, mn, retSig.c_str());
              if (!g_getMinecraft)
                env->ExceptionClear();
              env->ReleaseStringUTFChars(mName, mn);
              env->DeleteLocalRef(mName);
            }
            env->DeleteLocalRef(retType);
          }
          env->DeleteLocalRef(params);
        }
        env->DeleteLocalRef(m);
      }
      env->DeleteLocalRef(methods);
      env->DeleteLocalRef(methodClass);
    }
  }
  if (!g_getMinecraft) {
    reachStatus = "getMinecraft not found";
    return false;
  }

  auto findFieldByType = [&](jclass owner, jclass targetType) -> jfieldID {
    jclass classRef = env->FindClass("java/lang/Class");
    if (!classRef) {
      env->ExceptionClear();
      return nullptr;
    }

    jmethodID getDeclaredFields = env->GetMethodID(
        classRef, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
    jmethodID classGetName =
        env->GetMethodID(classRef, "getName", "()Ljava/lang/String;");
    env->DeleteLocalRef(classRef);

    jobjectArray fields =
        (jobjectArray)env->CallObjectMethod(owner, getDeclaredFields);
    if (!fields || env->ExceptionCheck()) {
      env->ExceptionClear();
      return nullptr;
    }

    jclass fieldClass = env->FindClass("java/lang/reflect/Field");
    jmethodID getType =
        env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
    jmethodID fGetName =
        env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");

    jfieldID result = nullptr;
    jint flen = env->GetArrayLength(fields);

    for (jint i = 0; i < flen && !result; i++) {
      jobject f = env->GetObjectArrayElement(fields, i);
      jclass fType = (jclass)env->CallObjectMethod(f, getType);

      if (env->IsSameObject(fType, targetType)) {
        jstring fName = (jstring)env->CallObjectMethod(f, fGetName);
        const char *fn = env->GetStringUTFChars(fName, nullptr);
        jstring typeName =
            (jstring)env->CallObjectMethod(targetType, classGetName);
        const char *tn = env->GetStringUTFChars(typeName, nullptr);

        std::string sig = "L";
        for (const char *p = tn; *p; p++)
          sig += (*p == '.') ? '/' : *p;
        sig += ";";

        result = env->GetFieldID(owner, fn, sig.c_str());
        if (!result)
          env->ExceptionClear();

        env->ReleaseStringUTFChars(typeName, tn);
        env->DeleteLocalRef(typeName);
        env->ReleaseStringUTFChars(fName, fn);
        env->DeleteLocalRef(fName);
      }

      env->DeleteLocalRef(fType);
      env->DeleteLocalRef(f);
    }

    env->DeleteLocalRef(fields);
    env->DeleteLocalRef(fieldClass);
    return result;
  };

  const char *espNames[] = {"net.minecraft.client.entity.EntityPlayerSP"};
  jclass espClass = LoadMCClass(env, espNames, 1);
  if (!espClass) {
    reachStatus = "EntityPlayerSP class not found";
    return false;
  }

  const char *wcNames[] = {"net.minecraft.client.multiplayer.WorldClient"};
  jclass wcClass = LoadMCClass(env, wcNames, 1);
  if (!wcClass) {
    reachStatus = "WorldClient class not found";
    return false;
  }

  const char *pcmpNames[] = {
      "net.minecraft.client.multiplayer.PlayerControllerMP"};
  jclass pcmpLocal = LoadMCClass(env, pcmpNames, 1);
  if (!pcmpLocal) {
    reachStatus = "PlayerControllerMP not found";
    return false;
  }
  g_pcmpClass = (jclass)env->NewGlobalRef(pcmpLocal);
  env->DeleteLocalRef(pcmpLocal);

  const char *entNames[] = {"net.minecraft.entity.Entity"};
  jclass entLocal = LoadMCClass(env, entNames, 1);
  if (!entLocal) {
    reachStatus = "Entity class not found";
    return false;
  }
  g_entityClass = (jclass)env->NewGlobalRef(entLocal);
  env->DeleteLocalRef(entLocal);

  const char *worldNames[] = {"net.minecraft.world.World"};
  jclass wldLocal = LoadMCClass(env, worldNames, 1);
  if (!wldLocal) {
    reachStatus = "World class not found";
    return false;
  }
  g_worldClass = (jclass)env->NewGlobalRef(wldLocal);
  env->DeleteLocalRef(wldLocal);

  const char *elbNames[] = {"net.minecraft.entity.EntityLivingBase"};
  jclass elbClass = LoadMCClass(env, elbNames, 1);

  const char *epNames[] = {"net.minecraft.entity.player.EntityPlayer"};
  jclass epClass = LoadMCClass(env, epNames, 1);

  const char *tpNames[] = {"field_71439_g", "thePlayer"};
  g_fThePlayer = TryFieldN(env, g_mcClass, tpNames, 2,
                           "Lnet/minecraft/client/entity/EntityPlayerSP;");
  if (!g_fThePlayer)
    g_fThePlayer = findFieldByType(g_mcClass, espClass);
  if (!g_fThePlayer) {
    reachStatus = "thePlayer not found";
    return false;
  }

  const char *twNames[] = {"field_71441_e", "theWorld"};
  g_fTheWorld = TryFieldN(env, g_mcClass, twNames, 2,
                          "Lnet/minecraft/client/multiplayer/WorldClient;");
  if (!g_fTheWorld)
    g_fTheWorld = findFieldByType(g_mcClass, wcClass);
  if (!g_fTheWorld) {
    reachStatus = "theWorld not found";
    return false;
  }

  const char *pcNames[] = {"field_71442_b", "playerController"};
  g_fPlayerController =
      TryFieldN(env, g_mcClass, pcNames, 2,
                "Lnet/minecraft/client/multiplayer/PlayerControllerMP;");
  if (!g_fPlayerController)
    g_fPlayerController = findFieldByType(g_mcClass, g_pcmpClass);
  if (!g_fPlayerController) {
    reachStatus = "playerController not found";
    return false;
  }

  env->DeleteLocalRef(espClass);
  env->DeleteLocalRef(wcClass);

  const char *pxNames[] = {"field_70165_t", "posX"};
  g_fPosX = TryFieldN(env, g_entityClass, pxNames, 2, "D");
  const char *pyNames[] = {"field_70163_u", "posY"};
  g_fPosY = TryFieldN(env, g_entityClass, pyNames, 2, "D");
  const char *pzNames[] = {"field_70161_v", "posZ"};
  g_fPosZ = TryFieldN(env, g_entityClass, pzNames, 2, "D");
  const char *deadNames[] = {"field_70128_L", "isDead"};
  g_fIsDead = TryFieldN(env, g_entityClass, deadNames, 2, "Z");
  const char *yawNames[] = {"field_70177_z", "rotationYaw"};
  g_fRotYaw = TryFieldN(env, g_entityClass, yawNames, 2, "F");
  const char *pitchNames[] = {"field_70125_A", "rotationPitch"};
  g_fRotPitch = TryFieldN(env, g_entityClass, pitchNames, 2, "F");
  const char *prevYawNames[] = {"field_70126_B", "prevRotationYaw"};
  g_fPrevRotYaw = TryFieldN(env, g_entityClass, prevYawNames, 2, "F");
  const char *prevPitchNames[] = {"field_70127_C", "prevRotationPitch"};
  g_fPrevRotPitch = TryFieldN(env, g_entityClass, prevPitchNames, 2, "F");
  if (env->ExceptionCheck())
    env->ExceptionClear();
  const char *eyeNames[] = {"func_70047_e", "getEyeHeight"};
  g_mGetEyeHeight = TryMethodN(env, g_entityClass, eyeNames, 2, "()F");

  if (!g_fPosX || !g_fPosY || !g_fPosZ || !g_fIsDead || !g_fRotYaw ||
      !g_fRotPitch || !g_mGetEyeHeight) {
    reachStatus = "Entity fields not found";
    return false;
  }

  const char *mxNames[] = {"field_70159_w", "motionX"};
  g_fMotionX = TryFieldN(env, g_entityClass, mxNames, 2, "D");
  const char *myNames[] = {"field_70181_x", "motionY"};
  g_fMotionY = TryFieldN(env, g_entityClass, myNames, 2, "D");
  const char *mzNames[] = {"field_70179_y", "motionZ"};
  g_fMotionZ = TryFieldN(env, g_entityClass, mzNames, 2, "D");
  const char *ogNames[] = {"field_70122_E", "onGround"};
  g_fOnGround = TryFieldN(env, g_entityClass, ogNames, 2, "Z");

  if (env->ExceptionCheck())
    env->ExceptionClear();

  if (g_fMotionX && g_fMotionY && g_fMotionZ) {
    g_velocityJniReady = true;
    velocityStatus = "Ready";
  } else {
    velocityStatus = "Motion fields not found";
  }

  const char *ssNames[] = {"func_70031_b", "setSprinting"};
  g_mSetSprinting = TryMethodN(env, g_entityClass, ssNames, 2, "(Z)V");
  if (env->ExceptionCheck())
    env->ExceptionClear();
  g_sprintJniReady = (g_mSetSprinting != nullptr);

  {
    const char *isSpNames[] = {"isSprinting", "field_70015_d"};
    g_fIsSprinting = TryFieldN(env, g_entityClass, isSpNames, 2, "Z");
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  {

    const char *fdNames[] = {"field_70143_R", "fallDistance", "aG", "aH", "aI", "R"};
    g_fFallDistance = TryFieldN(env, g_entityClass, fdNames, 6, "F");
    if (env->ExceptionCheck())
      env->ExceptionClear();

    if (!g_fFallDistance) {
      jclass classClass = env->FindClass("java/lang/Class");
      jclass fieldClass = env->FindClass("java/lang/reflect/Field");
      if (classClass && fieldClass) {
        jmethodID getDeclaredFields = env->GetMethodID(classClass, "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
        jmethodID getName = env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
        jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
        jmethodID setAccessible = env->GetMethodID(fieldClass, "setAccessible", "(Z)V");
        if (getDeclaredFields && getName && getType) {
          jobjectArray fields = (jobjectArray)env->CallObjectMethod(g_entityClass, getDeclaredFields);
          if (fields && !env->ExceptionCheck()) {
            jsize count = env->GetArrayLength(fields);
            jclass floatType = env->FindClass("java/lang/Float");

            jclass floatPrimClass = nullptr;
            {
              jfieldID typeField = env->GetStaticFieldID(floatType ? floatType : env->FindClass("java/lang/Float"), "TYPE", "Ljava/lang/Class;");
              if (typeField && !env->ExceptionCheck()) {
                floatPrimClass = (jclass)env->GetStaticObjectField(env->FindClass("java/lang/Float"), typeField);
              }
              if (env->ExceptionCheck()) env->ExceptionClear();
            }

            for (jsize i = 0; i < count && !g_fFallDistance; i++) {
              jobject f = env->GetObjectArrayElement(fields, i);
              if (!f) continue;
              jclass fType = (jclass)env->CallObjectMethod(f, getType);

              bool isFloat = false;
              if (fType && floatPrimClass) {
                isFloat = env->IsSameObject(fType, floatPrimClass);
              }
              if (isFloat) {
                jstring nameStr = (jstring)env->CallObjectMethod(f, getName);
                if (nameStr) {
                  const char *nameChars = env->GetStringUTFChars(nameStr, nullptr);
                  if (nameChars) {

                    if (strstr(nameChars, "fallDist") || strstr(nameChars, "fall_dist") ||
                        strcmp(nameChars, "fallDistance") == 0) {
                      if (setAccessible) env->CallVoidMethod(f, setAccessible, JNI_TRUE);
                      if (env->ExceptionCheck()) env->ExceptionClear();
                      g_fFallDistance = env->FromReflectedField(f);
                      if (env->ExceptionCheck()) { env->ExceptionClear(); g_fFallDistance = nullptr; }
                    }
                    env->ReleaseStringUTFChars(nameStr, nameChars);
                  }
                  env->DeleteLocalRef(nameStr);
                }
              }
              if (fType) env->DeleteLocalRef(fType);
              env->DeleteLocalRef(f);
            }
            if (floatPrimClass) env->DeleteLocalRef(floatPrimClass);
            if (floatType) env->DeleteLocalRef(floatType);
            env->DeleteLocalRef(fields);
          }
          if (env->ExceptionCheck()) env->ExceptionClear();
        }
      }
      if (env->ExceptionCheck()) env->ExceptionClear();
    }
  }

  {

    const char *espDotNames[] = {"net.minecraft.client.entity.EntityPlayerSP"};
    jclass espClass2 = LoadMCClass(env, espDotNames, 1);
    if (espClass2) {
      const char *sqNames[] = {"field_71174_a", "sendQueue"};
      g_fSendQueue = TryFieldN(env, espClass2, sqNames, 2,
          "Lnet/minecraft/client/network/NetHandlerPlayClient;");
      if (env->ExceptionCheck()) env->ExceptionClear();
      env->DeleteLocalRef(espClass2);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();

    const char *nhpcDotNames[] = {"net.minecraft.client.network.NetHandlerPlayClient"};
    jclass nhpcClass = LoadMCClass(env, nhpcDotNames, 1);
    if (nhpcClass) {
      const char *atsNames[] = {"func_147297_a", "addToSendQueue"};
      g_mAddToSendQueue = TryMethodN(env, nhpcClass, atsNames, 2,
          "(Lnet/minecraft/network/Packet;)V");
      if (env->ExceptionCheck()) env->ExceptionClear();
      env->DeleteLocalRef(nhpcClass);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();

    const char *c03DotNames[] = {"net.minecraft.network.play.client.C03PacketPlayer"};
    jclass c03Local = LoadMCClass(env, c03DotNames, 1);
    if (c03Local) {
      g_c03PacketClass = (jclass)env->NewGlobalRef(c03Local);
      g_c03PacketInit = env->GetMethodID(c03Local, "<init>", "(Z)V");
      if (env->ExceptionCheck()) { env->ExceptionClear(); g_c03PacketInit = nullptr; }
      env->DeleteLocalRef(c03Local);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();

    g_noFallJniReady = g_fSendQueue && g_mAddToSendQueue && g_c03PacketClass && g_c03PacketInit;
  }

  {
    const char *sneakNames[] = {"func_70093_af", "isSneaking"};
    g_mIsSneaking = TryMethodN(env, g_entityClass, sneakNames, 2, "()Z");
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  {
    const char *elbNames[] = {"net.minecraft.entity.EntityLivingBase"};
    jclass elbLocal = LoadMCClass(env, elbNames, 1);
    if (elbLocal) {
      g_entityLivingClass = (jclass)env->NewGlobalRef(elbLocal);

      const char *yhNames[] = {"field_70759_as", "rotationYawHead"};
      g_fRotYawHead = TryFieldN(env, elbLocal, yhNames, 2, "F");
      if (env->ExceptionCheck()) env->ExceptionClear();
      const char *phNames[] = {"field_70758_at", "rotationPitchHead"};
      g_fRotPitchHead = TryFieldN(env, elbLocal, phNames, 2, "F");
      if (env->ExceptionCheck()) env->ExceptionClear();
      const char *ryoNames[] = {"field_70761_aq", "renderYawOffset"};
      g_fRenderYawOffset = TryFieldN(env, elbLocal, ryoNames, 2, "F");
      if (env->ExceptionCheck()) env->ExceptionClear();

      env->DeleteLocalRef(elbLocal);
      const char *ghNames[] = {"func_110143_aJ", "getHealth"};
      g_mGetHealth = TryMethodN(env, g_entityLivingClass, ghNames, 2, "()F");
      if (env->ExceptionCheck()) env->ExceptionClear();
      const char *gmhNames[] = {"func_110138_aP", "getMaxHealth"};
      g_mGetMaxHealth = TryMethodN(env, g_entityLivingClass, gmhNames, 2, "()F");
      if (env->ExceptionCheck()) env->ExceptionClear();

      const char *geisNames[] = {"func_71124_b", "getEquipmentInSlot"};
      g_mGetEquipmentInSlot = TryMethodN(env, g_entityLivingClass, geisNames, 2,
          "(I)Lnet/minecraft/item/ItemStack;");
      if (env->ExceptionCheck()) env->ExceptionClear();
    }
    if (env->ExceptionCheck()) env->ExceptionClear();

    {
      const char *isNames[] = {"net.minecraft.item.ItemStack"};
      jclass isLocal = LoadMCClass(env, isNames, 1);
      if (isLocal) {
        const char *gmdNames[] = {"func_77958_k", "getMaxDamage"};
        g_mGetMaxDamage = TryMethodN(env, isLocal, gmdNames, 2, "()I");
        if (env->ExceptionCheck()) env->ExceptionClear();
        const char *gidNames[] = {"func_77952_i", "getItemDamage"};
        g_mGetItemDamage = TryMethodN(env, isLocal, gidNames, 2, "()I");
        if (env->ExceptionCheck()) env->ExceptionClear();
        env->DeleteLocalRef(isLocal);
      }
      if (env->ExceptionCheck()) env->ExceptionClear();
    }

    const char *epNames[] = {"net.minecraft.entity.player.EntityPlayer"};
    jclass epLocal = LoadMCClass(env, epNames, 1);
    if (epLocal) {
      g_entityPlayerClass = (jclass)env->NewGlobalRef(epLocal);
      const char *gavNames[] = {"func_70658_aO", "getTotalArmorValue"};
      g_mGetArmorValue = TryMethodN(env, epLocal, gavNames, 2, "()I");
      if (env->ExceptionCheck()) env->ExceptionClear();
      const char *giudNames[] = {"func_71052_bv", "getItemInUseDuration"};
      g_mGetItemInUseDuration = TryMethodN(env, epLocal, giudNames, 2, "()I");
      if (env->ExceptionCheck()) env->ExceptionClear();
      env->DeleteLocalRef(epLocal);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();

    const char *gnNames[] = {"func_70005_c_", "getName"};
    g_mGetName = TryMethodN(env, g_entityClass, gnNames, 2,
                            "()Ljava/lang/String;");
    if (env->ExceptionCheck()) env->ExceptionClear();
  }

  {

    const char *gsNames[] = {"field_71474_y", "gameSettings"};
    g_fGameSettings = TryFieldN(env, g_mcClass, gsNames, 2, "Lnet/minecraft/client/settings/GameSettings;");
    if (env->ExceptionCheck()) env->ExceptionClear();

    if (g_fGameSettings) {

      const char *gsDotNames[] = {"net.minecraft.client.settings.GameSettings"};
      jclass gsClass = LoadMCClass(env, gsDotNames, 1);
      if (gsClass) {
        const char *kfNames[] = {"field_74351_w", "keyBindForward"};
        g_fKeyBindForward = TryFieldN(env, gsClass, kfNames, 2, "Lnet/minecraft/client/settings/KeyBinding;");
        if (env->ExceptionCheck()) env->ExceptionClear();

        const char *kbNames[] = {"field_74368_y", "keyBindBack"};
        g_fKeyBindBack = TryFieldN(env, gsClass, kbNames, 2, "Lnet/minecraft/client/settings/KeyBinding;");
        if (env->ExceptionCheck()) env->ExceptionClear();

        const char *klNames[] = {"field_74370_x", "keyBindLeft"};
        g_fKeyBindLeft = TryFieldN(env, gsClass, klNames, 2, "Lnet/minecraft/client/settings/KeyBinding;");
        if (env->ExceptionCheck()) env->ExceptionClear();

        const char *krNames[] = {"field_74366_z", "keyBindRight"};
        g_fKeyBindRight = TryFieldN(env, gsClass, krNames, 2, "Lnet/minecraft/client/settings/KeyBinding;");
        if (env->ExceptionCheck()) env->ExceptionClear();

        const char *kjNames[] = {"field_74314_A", "keyBindJump"};
        g_fKeyBindJump = TryFieldN(env, gsClass, kjNames, 2, "Lnet/minecraft/client/settings/KeyBinding;");
        if (env->ExceptionCheck()) env->ExceptionClear();

        const char *ksNames[] = {"field_74311_E", "keyBindSneak"};
        g_fKeyBindSneak = TryFieldN(env, gsClass, ksNames, 2, "Lnet/minecraft/client/settings/KeyBinding;");
        if (env->ExceptionCheck()) env->ExceptionClear();

        const char *kspNames[] = {"field_74312_F", "keyBindSprint"};
        g_fKeyBindSprint = TryFieldN(env, gsClass, kspNames, 2, "Lnet/minecraft/client/settings/KeyBinding;");
        if (env->ExceptionCheck()) env->ExceptionClear();

        env->DeleteLocalRef(gsClass);
      }
      if (env->ExceptionCheck()) env->ExceptionClear();
    }

    const char *kbDotNames[] = {"net.minecraft.client.settings.KeyBinding"};
    jclass kbClass = LoadMCClass(env, kbDotNames, 1);
    if (kbClass) {
      const char *pressedNames[] = {"field_74513_e", "pressed"};
      g_fKeyBindPressed = TryFieldN(env, kbClass, pressedNames, 2, "Z");
      if (env->ExceptionCheck()) env->ExceptionClear();
      env->DeleteLocalRef(kbClass);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();

    g_invWalkJniReady = g_fGameSettings && g_fKeyBindForward && g_fKeyBindPressed;
  }

  {

    const char *bpDotNames[] = {"net.minecraft.util.BlockPos"};
    jclass bpLocal = LoadMCClass(env, bpDotNames, 1);
    if (bpLocal) {
      g_blockPosClass = (jclass)env->NewGlobalRef(bpLocal);
      g_blockPosInit = env->GetMethodID(bpLocal, "<init>", "(III)V");
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(bpLocal);
    }

    if (g_blockPosClass) {
      const char *gbsNames[] = {"func_180495_p", "getBlockState"};
      g_mGetBlockState = TryMethodN(env, g_worldClass, gbsNames, 2,
                                    "(Lnet/minecraft/util/BlockPos;)Lnet/minecraft/block/state/IBlockState;");
      if (env->ExceptionCheck())
        env->ExceptionClear();
    }

    if (g_blockPosClass) {
      const char *iabNames[] = {"func_175623_d", "isAirBlock"};
      g_mIsAirBlock = TryMethodN(env, g_worldClass, iabNames, 2,
                                 "(Lnet/minecraft/util/BlockPos;)Z");
      if (env->ExceptionCheck()) env->ExceptionClear();
    }

    const char *ibsDotNames[] = {"net.minecraft.block.state.IBlockState"};
    jclass ibsClass = LoadMCClass(env, ibsDotNames, 1);
    if (ibsClass) {
      const char *gbNames[] = {"func_177230_c", "getBlock"};
      g_mGetBlock = TryMethodN(env, ibsClass, gbNames, 2,
                               "()Lnet/minecraft/block/Block;");
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(ibsClass);
    }

    const char *isDotNames[] = {"net.minecraft.item.ItemStack"};
    jclass isClass2 = LoadMCClass(env, isDotNames, 1);
    if (isClass2) {
      const char *gsvbNames[] = {"func_150997_a", "getStrVsBlock"};
      g_mGetStrVsBlock = TryMethodN(env, isClass2, gsvbNames, 2,
                                    "(Lnet/minecraft/block/Block;)F");
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(isClass2);
    }
  }

  const char *elNames[] = {"field_72996_f", "loadedEntityList"};
  g_fEntityList = TryFieldN(env, g_worldClass, elNames, 2, "Ljava/util/List;");
  if (!g_fEntityList) {
    reachStatus = "loadedEntityList not found";
    return false;
  }

  jclass lst = env->FindClass("java/util/List");
  if (!lst) {
    env->ExceptionClear();
    reachStatus = "List class not found";
    return false;
  }
  g_mListSize = env->GetMethodID(lst, "size", "()I");
  g_mListGet = env->GetMethodID(lst, "get", "(I)Ljava/lang/Object;");
  env->DeleteLocalRef(lst);
  if (!g_mListSize || !g_mListGet) {
    reachStatus = "List methods not found";
    return false;
  }

  const char *atkNames[] = {"func_78764_a", "attackEntity"};
  g_mAttackEntity = TryMethodN(env, g_pcmpClass, atkNames, 2,
                               "(Lnet/minecraft/entity/player/"
                               "EntityPlayer;Lnet/minecraft/entity/Entity;)V");
  if (!g_mAttackEntity && epClass) {

    jclass classRef = env->FindClass("java/lang/Class");
    jmethodID getDeclaredMethods = env->GetMethodID(
        classRef, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
    jobjectArray methods =
        (jobjectArray)env->CallObjectMethod(g_pcmpClass, getDeclaredMethods);
    env->DeleteLocalRef(classRef);
    if (methods) {
      jclass methodClass = env->FindClass("java/lang/reflect/Method");
      jmethodID getParamTypes = env->GetMethodID(
          methodClass, "getParameterTypes", "()[Ljava/lang/Class;");
      jmethodID mGetName =
          env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
      jmethodID getReturnType =
          env->GetMethodID(methodClass, "getReturnType", "()Ljava/lang/Class;");
      jclass voidClass = env->FindClass("java/lang/Void");
      jfieldID typeField =
          env->GetStaticFieldID(voidClass, "TYPE", "Ljava/lang/Class;");
      jclass voidType = (jclass)env->GetStaticObjectField(voidClass, typeField);

      jint mlen = env->GetArrayLength(methods);
      for (jint i = 0; i < mlen && !g_mAttackEntity; i++) {
        jobject m = env->GetObjectArrayElement(methods, i);
        jclass retT = (jclass)env->CallObjectMethod(m, getReturnType);
        if (env->IsSameObject(retT, voidType)) {
          jobjectArray params =
              (jobjectArray)env->CallObjectMethod(m, getParamTypes);
          if (env->GetArrayLength(params) == 2) {
            jclass p0 = (jclass)env->GetObjectArrayElement(params, 0);
            jclass p1 = (jclass)env->GetObjectArrayElement(params, 1);
            if (env->IsAssignableFrom(epClass, p0) &&
                env->IsAssignableFrom(g_entityClass, p1)) {
              jstring mName = (jstring)env->CallObjectMethod(m, mGetName);
              const char *mn = env->GetStringUTFChars(mName, nullptr);

              jmethodID getNameM = env->GetMethodID(
                  env->GetObjectClass(p0), "getName", "()Ljava/lang/String;");
              jstring p0Name = (jstring)env->CallObjectMethod(p0, getNameM);
              jstring p1Name = (jstring)env->CallObjectMethod(p1, getNameM);
              const char *p0n = env->GetStringUTFChars(p0Name, nullptr);
              const char *p1n = env->GetStringUTFChars(p1Name, nullptr);
              std::string sig = "(L";
              for (const char *c = p0n; *c; c++)
                sig += (*c == '.') ? '/' : *c;
              sig += ";L";
              for (const char *c = p1n; *c; c++)
                sig += (*c == '.') ? '/' : *c;
              sig += ";)V";
              g_mAttackEntity = env->GetMethodID(g_pcmpClass, mn, sig.c_str());
              if (!g_mAttackEntity)
                env->ExceptionClear();
              env->ReleaseStringUTFChars(p1Name, p1n);
              env->DeleteLocalRef(p1Name);
              env->ReleaseStringUTFChars(p0Name, p0n);
              env->DeleteLocalRef(p0Name);
              env->ReleaseStringUTFChars(mName, mn);
              env->DeleteLocalRef(mName);
            }
            env->DeleteLocalRef(p0);
            env->DeleteLocalRef(p1);
          }
          env->DeleteLocalRef(params);
        }
        env->DeleteLocalRef(retT);
        env->DeleteLocalRef(m);
      }
      env->DeleteLocalRef(voidClass);
      env->DeleteLocalRef(voidType);
      env->DeleteLocalRef(methods);
      env->DeleteLocalRef(methodClass);
    }
  }
  if (!g_mAttackEntity) {
    reachStatus = "attackEntity not found";
    return false;
  }

  const char *brNames[] = {"field_78770_f", "blockReachDistance"};
  g_fBlockReach = TryFieldN(env, g_pcmpClass, brNames, 2, "F");
  if (g_fBlockReach) {
    printf("[REACH DEBUG] Successfully resolved g_fBlockReach field\n");
  } else {
    printf("[REACH DEBUG] Failed to resolve g_fBlockReach field\n");
  }

  if (elbClass) {
    const char *swingNames[] = {"func_71038_i", "swingItem"};
    g_mSwingItem = TryMethodN(env, elbClass, swingNames, 2, "()V");
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  const char *mopDotNames[] = {"net.minecraft.util.MovingObjectPosition"};
  jclass mopClass = LoadMCClass(env, mopDotNames, 1);
  if (mopClass) {

    const char *omoNames[] = {"field_71476_x", "objectMouseOver"};
    g_fObjectMouseOver = TryFieldN(env, g_mcClass, omoNames, 2,
                                   "Lnet/minecraft/util/MovingObjectPosition;");
    if (!g_fObjectMouseOver) {

      g_fObjectMouseOver = findFieldByType(g_mcClass, mopClass);
    }
    if (env->ExceptionCheck())
      env->ExceptionClear();

    const char *ehNames[] = {"field_72308_g", "entityHit"};
    g_fEntityHit =
        TryFieldN(env, mopClass, ehNames, 2, "Lnet/minecraft/entity/Entity;");
    if (!g_fEntityHit) {
      g_fEntityHit = findFieldByType(mopClass, g_entityClass);
    }
    if (env->ExceptionCheck())
      env->ExceptionClear();

    env->DeleteLocalRef(mopClass);
  }

  const char *hrtNames[] = {"field_70172_ad", "hurtResistantTime"};
  g_fHurtResistantTime = TryFieldN(env, g_entityClass, hrtNames, 2, "I");
  if (env->ExceptionCheck())
    env->ExceptionClear();

  if (elbClass) {
    const char *htNames[] = {"field_70737_aN", "hurtTime"};
    g_fHurtTime = TryFieldN(env, elbClass, htNames, 2, "I");
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  if (epClass) {
    const char *gceNames[] = {"func_71045_bC", "getCurrentEquippedItem"};
    g_mGetCurrentItem = TryMethodN(env, epClass, gceNames, 2,
                                   "()Lnet/minecraft/item/ItemStack;");
    if (env->ExceptionCheck())
      env->ExceptionClear();

    if (!g_mGetCurrentItem && elbClass) {
      const char *ghiNames[] = {"func_70694_bm", "getHeldItem"};
      g_mGetCurrentItem = TryMethodN(env, elbClass, ghiNames, 2,
                                     "()Lnet/minecraft/item/ItemStack;");
      if (env->ExceptionCheck())
        env->ExceptionClear();
    }

    if (!g_mGetCurrentItem) {
      const char *isNames[] = {"net.minecraft.item.ItemStack"};
      jclass isClass = LoadMCClass(env, isNames, 1);
      if (isClass) {

        jclass classRef = env->FindClass("java/lang/Class");
        jmethodID getDeclaredMethods = env->GetMethodID(
            classRef, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
        jobjectArray methods =
            (jobjectArray)env->CallObjectMethod(epClass, getDeclaredMethods);
        env->DeleteLocalRef(classRef);
        if (methods) {
          jclass methodClass = env->FindClass("java/lang/reflect/Method");
          jmethodID getRetType = env->GetMethodID(methodClass, "getReturnType",
                                                  "()Ljava/lang/Class;");
          jmethodID getParamTypes = env->GetMethodID(
              methodClass, "getParameterTypes", "()[Ljava/lang/Class;");
          jmethodID mGetName =
              env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
          jmethodID classGetName =
              env->GetMethodID(env->FindClass("java/lang/Class"), "getName",
                               "()Ljava/lang/String;");

          jint mlen = env->GetArrayLength(methods);
          for (jint i = 0; i < mlen && !g_mGetCurrentItem; i++) {
            jobject m = env->GetObjectArrayElement(methods, i);
            jobjectArray params =
                (jobjectArray)env->CallObjectMethod(m, getParamTypes);
            if (env->GetArrayLength(params) == 0) {
              jclass retT = (jclass)env->CallObjectMethod(m, getRetType);
              if (env->IsSameObject(retT, isClass)) {
                jstring mName = (jstring)env->CallObjectMethod(m, mGetName);
                const char *mn = env->GetStringUTFChars(mName, nullptr);

                jstring typeName =
                    (jstring)env->CallObjectMethod(isClass, classGetName);
                const char *tn = env->GetStringUTFChars(typeName, nullptr);
                std::string sig = "()L";
                for (const char *p = tn; *p; p++)
                  sig += (*p == '.') ? '/' : *p;
                sig += ";";
                g_mGetCurrentItem = env->GetMethodID(epClass, mn, sig.c_str());
                if (!g_mGetCurrentItem)
                  env->ExceptionClear();
                env->ReleaseStringUTFChars(typeName, tn);
                env->DeleteLocalRef(typeName);
                env->ReleaseStringUTFChars(mName, mn);
                env->DeleteLocalRef(mName);
              }
              env->DeleteLocalRef(retT);
            }
            env->DeleteLocalRef(params);
            env->DeleteLocalRef(m);
          }
          env->DeleteLocalRef(methods);
          env->DeleteLocalRef(methodClass);
        }
        env->DeleteLocalRef(isClass);
      }
    }
  }

  const char *itemStackNames[] = {"net.minecraft.item.ItemStack"};
  jclass itemStackClass = LoadMCClass(env, itemStackNames, 1);
  if (itemStackClass) {
    const char *giNames[] = {"func_77973_b", "getItem"};
    g_mStackGetItem = TryMethodN(env, itemStackClass, giNames, 2,
                                 "()Lnet/minecraft/item/Item;");
    if (!g_mStackGetItem) {

      const char *itemNames[] = {"net.minecraft.item.Item"};
      jclass itemClass = LoadMCClass(env, itemNames, 1);
      if (itemClass) {
        jclass classRef = env->FindClass("java/lang/Class");
        jmethodID getDeclaredMethods = env->GetMethodID(
            classRef, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");
        jobjectArray methods = (jobjectArray)env->CallObjectMethod(
            itemStackClass, getDeclaredMethods);
        env->DeleteLocalRef(classRef);
        if (methods) {
          jclass methodClass = env->FindClass("java/lang/reflect/Method");
          jmethodID getRetType = env->GetMethodID(methodClass, "getReturnType",
                                                  "()Ljava/lang/Class;");
          jmethodID getParamTypes = env->GetMethodID(
              methodClass, "getParameterTypes", "()[Ljava/lang/Class;");
          jmethodID mGetName =
              env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
          jmethodID classGetName =
              env->GetMethodID(env->FindClass("java/lang/Class"), "getName",
                               "()Ljava/lang/String;");

          jint mlen = env->GetArrayLength(methods);
          for (jint i = 0; i < mlen && !g_mStackGetItem; i++) {
            jobject m = env->GetObjectArrayElement(methods, i);
            jobjectArray params =
                (jobjectArray)env->CallObjectMethod(m, getParamTypes);
            if (env->GetArrayLength(params) == 0) {
              jclass retT = (jclass)env->CallObjectMethod(m, getRetType);
              if (env->IsSameObject(retT, itemClass)) {
                jstring mName = (jstring)env->CallObjectMethod(m, mGetName);
                const char *mn = env->GetStringUTFChars(mName, nullptr);
                jstring typeName =
                    (jstring)env->CallObjectMethod(itemClass, classGetName);
                const char *tn = env->GetStringUTFChars(typeName, nullptr);
                std::string sig = "()L";
                for (const char *p = tn; *p; p++)
                  sig += (*p == '.') ? '/' : *p;
                sig += ";";
                g_mStackGetItem =
                    env->GetMethodID(itemStackClass, mn, sig.c_str());
                if (!g_mStackGetItem)
                  env->ExceptionClear();
                env->ReleaseStringUTFChars(typeName, tn);
                env->DeleteLocalRef(typeName);
                env->ReleaseStringUTFChars(mName, mn);
                env->DeleteLocalRef(mName);
              }
              env->DeleteLocalRef(retT);
            }
            env->DeleteLocalRef(params);
            env->DeleteLocalRef(m);
          }
          env->DeleteLocalRef(methods);
          env->DeleteLocalRef(methodClass);
        }
        env->DeleteLocalRef(itemClass);
      }
    }
    if (env->ExceptionCheck())
      env->ExceptionClear();

    const char *gdnNames[] = {"func_82833_r", "getDisplayName"};
    g_mStackGetDisplayName = TryMethodN(env, itemStackClass, gdnNames, 2,
                                        "()Ljava/lang/String;");
    if (env->ExceptionCheck())
      env->ExceptionClear();
    env->DeleteLocalRef(itemStackClass);
  }

  const char *iswDotNames[] = {"net.minecraft.item.ItemSword"};
  jclass iswLocal = LoadMCClass(env, iswDotNames, 1);
  if (iswLocal) {
    g_itemSwordClass = (jclass)env->NewGlobalRef(iswLocal);
    env->DeleteLocalRef(iswLocal);
  }
  if (env->ExceptionCheck())
    env->ExceptionClear();

  g_swordCheckReady =
      (g_mGetCurrentItem && g_mStackGetItem && g_itemSwordClass);

  {
    const char *bowNames[] = {"net.minecraft.item.ItemBow"};
    jclass c = LoadMCClass(env, bowNames, 1);
    if (c) { g_itemBowClass = (jclass)env->NewGlobalRef(c); env->DeleteLocalRef(c); }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }
  {
    const char *pearlNames[] = {"net.minecraft.item.ItemEnderPearl"};
    jclass c = LoadMCClass(env, pearlNames, 1);
    if (c) { g_itemEnderPearlClass = (jclass)env->NewGlobalRef(c); env->DeleteLocalRef(c); }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }
  {
    const char *snowNames[] = {"net.minecraft.item.ItemSnowball"};
    jclass c = LoadMCClass(env, snowNames, 1);
    if (c) { g_itemSnowballClass = (jclass)env->NewGlobalRef(c); env->DeleteLocalRef(c); }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }
  {
    const char *eggNames[] = {"net.minecraft.item.ItemEgg"};
    jclass c = LoadMCClass(env, eggNames, 1);
    if (c) { g_itemEggClass = (jclass)env->NewGlobalRef(c); env->DeleteLocalRef(c); }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }

  {

    const char *elbDotNames2[] = {"net.minecraft.entity.EntityLivingBase"};
    jclass elb2 = LoadMCClass(env, elbDotNames2, 1);
    if (elb2) {
      const char *jtNames[] = {"field_70773_bE", "jumpTicks", "bE"};
      g_fJumpTicks = TryFieldN(env, elb2, jtNames, 3, "I");
      if (env->ExceptionCheck()) env->ExceptionClear();
      env->DeleteLocalRef(elb2);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }
  {

    const char *shNames[] = {"field_70138_W", "stepHeight"};
    g_fStepHeight = TryFieldN(env, g_entityClass, shNames, 2, "F");
    if (g_fStepHeight) {
      printf("[STEP DEBUG] Successfully resolved g_fStepHeight field\n");
    } else {
      printf("[STEP DEBUG] Failed to resolve g_fStepHeight field\n");
    }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }
  {

    const char *ibNames[] = {"net.minecraft.item.ItemBlock"};
    jclass c = LoadMCClass(env, ibNames, 1);
    if (c) { g_itemBlockClass = (jclass)env->NewGlobalRef(c); env->DeleteLocalRef(c); }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }
  {

    const char *c04Names[] = {"net.minecraft.network.play.client.C03PacketPlayer$C04PacketPlayerPosition"};
    jclass c04 = LoadMCClass(env, c04Names, 1);
    if (c04) {
      g_c03PosClass = (jclass)env->NewGlobalRef(c04);
      g_c03PosInit = env->GetMethodID(c04, "<init>", "(DDDZ)V");
      if (env->ExceptionCheck()) { env->ExceptionClear(); g_c03PosInit = nullptr; }
      env->DeleteLocalRef(c04);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }

  {
    const char *rcdtNames[] = {"field_71467_ac", "rightClickDelayTimer"};
    g_fRightClickDelayTimer = TryFieldN(env, g_mcClass, rcdtNames, 2, "I");
    if (env->ExceptionCheck())
      env->ExceptionClear();
    if (g_fRightClickDelayTimer) {
      g_fastPlaceJniReady = true;
    }
  }

  {
    const char *rcmNames[] = {"func_147121_ag", "rightClickMouse"};

    g_mRightClickMouse = TryMethodN(env, g_mcClass, rcmNames, 2, "()V");
    if (env->ExceptionCheck()) env->ExceptionClear();

    if (!g_mRightClickMouse) {
      jclass classRef = env->FindClass("java/lang/Class");
      jmethodID getDeclaredMethod = env->GetMethodID(classRef, "getDeclaredMethod",
          "(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
      jclass methodClass = env->FindClass("java/lang/reflect/Method");
      jmethodID setAccessible = env->GetMethodID(methodClass, "setAccessible", "(Z)V");
      if (getDeclaredMethod && setAccessible) {
        for (int i = 0; i < 2 && !g_mRightClickMouse; i++) {
          jstring name = env->NewStringUTF(rcmNames[i]);
          jobject method = env->CallObjectMethod(g_mcClass, getDeclaredMethod, name, nullptr);
          if (method && !env->ExceptionCheck()) {
            env->CallVoidMethod(method, setAccessible, JNI_TRUE);
            if (!env->ExceptionCheck()) {
              g_mRightClickMouse = env->FromReflectedMethod(method);
            }
            env->DeleteLocalRef(method);
          }
          if (env->ExceptionCheck()) env->ExceptionClear();
          env->DeleteLocalRef(name);
        }
      }
      if (env->ExceptionCheck()) env->ExceptionClear();
      if (classRef) env->DeleteLocalRef(classRef);
      if (methodClass) env->DeleteLocalRef(methodClass);
    }
  }

  {
    const char *rbcNames[] = {"func_178890_a", "onPlayerRightClick"};
    g_mOnPlayerRightClick = TryMethodN(
        env, g_pcmpClass, rbcNames, 2,
        "(Lnet/minecraft/client/entity/EntityPlayerSP;"
        "Lnet/minecraft/client/multiplayer/WorldClient;"
        "Lnet/minecraft/item/ItemStack;"
        "Lnet/minecraft/util/BlockPos;"
        "Lnet/minecraft/util/EnumFacing;"
        "Lnet/minecraft/util/Vec3;)Z");
    if (env->ExceptionCheck()) env->ExceptionClear();

    const char *v3Names[] = {"net.minecraft.util.Vec3"};
    jclass v3Local = LoadMCClass(env, v3Names, 1);
    if (v3Local) {
      g_vec3Class = (jclass)env->NewGlobalRef(v3Local);
      g_vec3Init = env->GetMethodID(v3Local, "<init>", "(DDD)V");
      if (env->ExceptionCheck()) {
        env->ExceptionClear();
        g_vec3Init = nullptr;
      }
      env->DeleteLocalRef(v3Local);
    }

    const char *efNames[] = {"net.minecraft.util.EnumFacing"};
    jclass efLocal = LoadMCClass(env, efNames, 1);
    if (efLocal) {
      g_enumFacingClass = (jclass)env->NewGlobalRef(efLocal);

      jclass classClass = env->FindClass("java/lang/Class");
      jmethodID mGetEnumConstants =
          classClass
              ? env->GetMethodID(classClass, "getEnumConstants", "()[Ljava/lang/Object;")
              : nullptr;
      if (mGetEnumConstants) {
        jobject arrObj = env->CallObjectMethod(g_enumFacingClass, mGetEnumConstants);
        if (arrObj && !env->ExceptionCheck()) {
          jobjectArray arr = (jobjectArray)arrObj;
          jsize n = env->GetArrayLength(arr);
          for (jsize i = 0; i < n && i < 6; i++) {
            jobject e = env->GetObjectArrayElement(arr, i);
            if (e && !env->ExceptionCheck()) {
              g_enumFacingByIndex[i] = env->NewGlobalRef(e);
              env->DeleteLocalRef(e);
            } else if (env->ExceptionCheck()) {
              env->ExceptionClear();
            }
          }
          env->DeleteLocalRef(arrObj);
        } else if (env->ExceptionCheck()) {
          env->ExceptionClear();
        }
      } else if (env->ExceptionCheck()) {
        env->ExceptionClear();
      }
      if (classClass) env->DeleteLocalRef(classClass);
      env->DeleteLocalRef(efLocal);
    }
    if (env->ExceptionCheck()) env->ExceptionClear();
  }

  if (elbClass) {
    const char *hpNames[] = {"field_70180_af", "health"};
    g_fHealth = TryFieldN(env, elbClass, hpNames, 2, "F");
    if (env->ExceptionCheck())
      env->ExceptionClear();
    env->DeleteLocalRef(elbClass);
    elbClass = nullptr;
  }

  const char *invDotNames[] = {"net.minecraft.entity.player.InventoryPlayer"};
  jclass invClass = LoadMCClass(env, invDotNames, 1);
  if (invClass) {
    g_inventoryClass = (jclass)env->NewGlobalRef(invClass);

    if (epClass) {
      const char *invFieldNames[] = {"field_71071_by", "inventory"};

      g_fInventory = TryFieldN(env, epClass, invFieldNames, 2,
                               "Lnet/minecraft/entity/player/InventoryPlayer;");
      if (!g_fInventory)
        g_fInventory = findFieldByType(epClass, invClass);
      if (env->ExceptionCheck())
        env->ExceptionClear();
    }

    const char *ciNames[] = {"field_70461_c", "currentItem"};
    g_fCurrentItemIdx = TryFieldN(env, invClass, ciNames, 2, "I");
    if (env->ExceptionCheck())
      env->ExceptionClear();

    const char *gssNames[] = {"func_70301_a", "getStackInSlot"};
    g_mGetStackInSlot = TryMethodN(env, invClass, gssNames, 2,
                                   "(I)Lnet/minecraft/item/ItemStack;");
    if (env->ExceptionCheck())
      env->ExceptionClear();

    env->DeleteLocalRef(invClass);
  }

  {
    const char *suiNames[] = {"func_78769_a", "sendUseItem"};
    g_mSendUseItem = TryMethodN(env, g_pcmpClass, suiNames, 2,
                                "(Lnet/minecraft/entity/player/EntityPlayer;"
                                "Lnet/minecraft/world/World;"
                                "Lnet/minecraft/item/ItemStack;)Z");
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  {
    const char *ipDotNames[] = {"net.minecraft.item.ItemPotion"};
    jclass ipLocal = LoadMCClass(env, ipDotNames, 1);
    if (ipLocal) {
      g_itemPotionClass = (jclass)env->NewGlobalRef(ipLocal);

      const char *isNames[] = {"func_77831_g", "isSplash"};
      g_mIsSplash = TryStaticMethodN(env, ipLocal, isNames, 2, "(I)Z");
      if (!g_mIsSplash) {

        g_mIsSplash = TryMethodN(env, ipLocal, isNames, 2, "(I)Z");
      }
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(ipLocal);
    }
  }

  {
    const char *itemDotNames[] = {"net.minecraft.item.Item"};
    jclass itemLocal = LoadMCClass(env, itemDotNames, 1);
    if (itemLocal) {
      g_itemClass = (jclass)env->NewGlobalRef(itemLocal);
      env->DeleteLocalRef(itemLocal);
    }
  }

  {
    const char *epDotNames[] = {"net.minecraft.entity.player.EntityPlayer"};
    jclass epClass2 = LoadMCClass(env, epDotNames, 1);
    if (epClass2) {
      const char *doiNames[] = {"func_71018_a", "dropOneItem"};
      g_mDropOneItem = TryMethodN(env, epClass2, doiNames, 2,
                                  "(Z)Lnet/minecraft/entity/item/EntityItem;");
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(epClass2);
    }

    {
        const char* gsNames[] = { "net.minecraft.client.gui.GuiScreen" };
        jclass guiScreenTmp = LoadMCClass(env, gsNames, 1);
        if (guiScreenTmp) {
            const char* dgsNames[] = { "func_147108_a", "displayGuiScreen" };
            g_mDisplayGuiScreen = TryMethodN(env, g_mcClass, dgsNames, 2,
                "(Lnet/minecraft/client/gui/GuiScreen;)V");
            if (env->ExceptionCheck()) env->ExceptionClear();
            env->DeleteLocalRef(guiScreenTmp);
        }
    }

    {
        const char* epDotNames[] = { "net.minecraft.entity.player.EntityPlayer" };
        jclass epTmp = LoadMCClass(env, epDotNames, 1);
        if (epTmp) {
            const char* csNames[] = { "func_71053_j", "closeScreen" };
            g_mCloseScreen = TryMethodN(env, epTmp, csNames, 2, "()V");
            if (env->ExceptionCheck()) env->ExceptionClear();
            env->DeleteLocalRef(epTmp);
        }
    }
  }

  *envOut = env;
  g_jniReady = true;

  return true;
}

static void reachThreadInner() {
  printf("[REACH DEBUG] Thread inner started\n");

  printf("[REACH DEBUG] Initializing JNI directly...\n");

  JNIEnv *env = nullptr;
  printf("[REACH DEBUG] Calling InitJNIReach...\n");
  if (!InitJNIReach(&env) || !env) {
    printf("[REACH DEBUG] InitJNIReach failed\n");
    return;
  }
  printf("[REACH DEBUG] InitJNIReach succeeded, setting g_jniReady=true\n");

  g_jniReady = true;
  reachStatus = "Ready";

  if (g_isUnloading) {
    printf("[REACH DEBUG] Unloading flag set, exiting\n");
    return;
  }

  LARGE_INTEGER freq, lastHit;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&lastHit);

  jclass livingBaseClass = nullptr;
  {
    const char *elbNames[] = {"net.minecraft.entity.EntityLivingBase"};
    jclass local = LoadMCClass(env, elbNames, 1);
    if (local) {
      livingBaseClass = (jclass)env->NewGlobalRef(local);
      env->DeleteLocalRef(local);
    }
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  bool wasEnabled = false;

  while (!g_isUnloading) {
    if (env->ExceptionCheck())
      env->ExceptionClear();

    if (!reachEnabled) {
      if (wasEnabled && g_fBlockReach) {

        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (mc) {
          jobject pc = env->GetObjectField(mc, g_fPlayerController);
          if (pc) {
            env->SetFloatField(pc, g_fBlockReach, 4.5f);
            env->DeleteLocalRef(pc);
          }
          env->DeleteLocalRef(mc);
        }
      }
      wasEnabled = false;
      Sleep(100);
      continue;
    }

    wasEnabled = true;

    if (showMenu || g_cursorVisible) {
      static int menuDebugCounter = 0;
      if (++menuDebugCounter % 100 == 0) {
        printf("[REACH DEBUG] Blocked by menu=%s, cursor=%s\n",
               showMenu ? "true" : "false", g_cursorVisible ? "true" : "false");
      }
      Sleep(50);
      continue;
    }
    if (reachSwordOnly && !g_holdingSword) {
      static int swordDebugCounter = 0;
      if (++swordDebugCounter % 100 == 0) {
        printf("[REACH DEBUG] Blocked by sword requirement: holding=%s\n",
               g_holdingSword ? "true" : "false");
      }
      Sleep(50);
      continue;
    }

    bool active = true;
    if (reachAttackOnly)
      active = active && (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    if (reachSprintOnly)
      active = active && g_sprintPending;
    if (reachOnlyMoving) {
      bool mv = (GetAsyncKeyState('W') & 0x8000) ||
                (GetAsyncKeyState('A') & 0x8000) ||
                (GetAsyncKeyState('S') & 0x8000) ||
                (GetAsyncKeyState('D') & 0x8000);
      active = active && mv;
    }
    if (reachWhileJumping)
      active = active && (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    if (!active) {
      static int activeDebugCounter = 0;
      if (++activeDebugCounter % 100 == 0) {
        printf("[REACH DEBUG] Inactive conditions: attackOnly=%s(LMB=%s), sprintOnly=%s(%s), movingOnly=%s, jumping=%s\n",
               reachAttackOnly ? "true" : "false", (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ? "held" : "not held",
               reachSprintOnly ? "true" : "false", g_sprintPending ? "sprinting" : "not sprinting",
               reachOnlyMoving ? "true" : "false",
               reachWhileJumping ? "true" : "false");
      }
      Sleep(10);
      continue;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double elapsed = (double)(now.QuadPart - lastHit.QuadPart) * 1000.0 /
                     (double)freq.QuadPart;
    if (elapsed < 500.0) {
      Sleep(5);
      continue;
    }

    {
      std::uniform_int_distribution<int> chanceDist(0, 99);
      if (chanceDist(g_reachRng) >= reachChance) {
        Sleep(10);
        continue;
      }
    }

    {
      double mean = ((double)reachMin + (double)reachMax) / 2.0;
      double stdDev = ((double)reachMax - (double)reachMin) / 4.0;
      double newTarget = mean + ReachNextGaussian() * stdDev;
      newTarget = (std::max)((double)reachMin, (std::min)((double)reachMax, newTarget));
      g_reachTarget = newTarget;
    }

    {
      LARGE_INTEGER now;
      QueryPerformanceCounter(&now);
      if (g_reachLastTime.QuadPart != 0) {
        double dt = (double)(now.QuadPart - g_reachLastTime.QuadPart) / (double)freq.QuadPart;

        const double omega = 2.0 * 3.14159265358979323846 * 2.5;
        const double dampingRatio = 1.0;
        double expVal = exp(-dampingRatio * omega * dt);
        double delta = g_reachCurrent - g_reachTarget;
        g_reachCurrent = g_reachTarget + (delta + (g_reachVelocity + omega * delta) * dt) * expVal;
        g_reachVelocity = (g_reachVelocity - omega * (omega * delta + 2.0 * dampingRatio * omega * g_reachVelocity) * dt) * expVal;
      } else {
        g_reachCurrent = g_reachTarget;
      }
      g_reachLastTime = now;
    }

    float effectiveReach = (float)g_reachCurrent;

    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (!mc || env->ExceptionCheck()) {
      env->ExceptionClear();
      Sleep(50);
      continue;
    }

    jobject player = env->GetObjectField(mc, g_fThePlayer);
    jobject world = env->GetObjectField(mc, g_fTheWorld);
    jobject pctrl = env->GetObjectField(mc, g_fPlayerController);

    if (!player || !world || !pctrl) {
      if (player) env->DeleteLocalRef(player);
      if (world) env->DeleteLocalRef(world);
      if (pctrl) env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    if (g_fBlockReach) {
      float oldReach = env->GetFloatField(pctrl, g_fBlockReach);
      env->SetFloatField(pctrl, g_fBlockReach, effectiveReach);

      static int reachDebugCounter = 0;
      if (++reachDebugCounter % 20 == 0) {
        printf("[REACH DEBUG] Set reach: %.2f -> %.2f (target: %.2f)\n", oldReach, effectiveReach, effectiveReach);
      }
    } else {
      printf("[REACH DEBUG] g_fBlockReach is null!\n");
    }

    double px = env->GetDoubleField(player, g_fPosX);
    double py = env->GetDoubleField(player, g_fPosY);
    double pz = env->GetDoubleField(player, g_fPosZ);
    float eyeH = env->CallFloatMethod(player, g_mGetEyeHeight);
    float yaw = env->GetFloatField(player, g_fRotYaw);
    float pitch = env->GetFloatField(player, g_fRotPitch);

    if (env->ExceptionCheck()) {
      env->ExceptionClear();
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(world);
      env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    const double PI = 3.14159265358979323846;
    double eyeX = px;
    double eyeY = py + (double)eyeH;
    double eyeZ = pz;
    double yawR = yaw * PI / 180.0;
    double pitchR = pitch * PI / 180.0;
    double lookX = -sin(yawR) * cos(pitchR);
    double lookY = -sin(pitchR);
    double lookZ = cos(yawR) * cos(pitchR);

    jobject entList = env->GetObjectField(world, g_fEntityList);
    if (!entList || env->ExceptionCheck()) {
      env->ExceptionClear();
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(world);
      env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    jint count = env->CallIntMethod(entList, g_mListSize);
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
      env->DeleteLocalRef(entList);
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(world);
      env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    std::vector<jobject> entitySnapshot;
    entitySnapshot.reserve((std::min)(count, (jint)200));

    for (jint i = 0; i < count && i < 200; i++) {
      jobject ent = env->CallObjectMethod(entList, g_mListGet, i);
      if (env->ExceptionCheck()) {
        env->ExceptionClear();
        break;
      }
      if (!ent)
        continue;

      if (livingBaseClass && !env->IsInstanceOf(ent, livingBaseClass)) {
        env->DeleteLocalRef(ent);
        continue;
      }
      if (env->IsSameObject(ent, player)) {
        env->DeleteLocalRef(ent);
        continue;
      }

      jobject gref = env->NewGlobalRef(ent);
      env->DeleteLocalRef(ent);
      if (gref)
        entitySnapshot.push_back(gref);
    }
    env->DeleteLocalRef(entList);

    jobject bestTarget = nullptr;
    double bestAngle = -1.0;
    double bestWorldDist = 0.0;

    for (jobject ent : entitySnapshot) {

      jboolean dead = env->GetBooleanField(ent, g_fIsDead);
      if (env->ExceptionCheck()) {
        env->ExceptionClear();
        continue;
      }
      if (dead)
        continue;

      double ex = env->GetDoubleField(ent, g_fPosX);
      double ey = env->GetDoubleField(ent, g_fPosY) + 1.0;
      double ez = env->GetDoubleField(ent, g_fPosZ);
      if (env->ExceptionCheck()) {
        env->ExceptionClear();
        continue;
      }

      double dx = ex - eyeX, dy = ey - eyeY, dz = ez - eyeZ;
      double wDist = sqrt(dx * dx + dy * dy + dz * dz);

      double entReach = (double)effectiveReach;
      if (reachMotionCompensation && g_fMotionX && g_fMotionZ) {
        double emx = env->GetDoubleField(ent, g_fMotionX);
        double emz = env->GetDoubleField(ent, g_fMotionZ);
        if (!env->ExceptionCheck()) {
          double motSq = emx * emx + emz * emz;

          double motFactor = cbrt(motSq) * 0.15;
          entReach = (std::min)((double)reachMax, entReach + motFactor);
        } else { env->ExceptionClear(); }
      }

      if (wDist < 0.5 || wDist > entReach)
        continue;

      double invD = 1.0 / wDist;
      double dot =
          (dx * invD) * lookX + (dy * invD) * lookY + (dz * invD) * lookZ;
      if (dot < 0.97)
        continue;

      if (dot > bestAngle) {
        bestTarget = ent;
        bestAngle = dot;
        bestWorldDist = wDist;
      }
    }

    if (bestTarget && bestWorldDist > 3.0) {

      jboolean stillDead = env->GetBooleanField(bestTarget, g_fIsDead);
      if (env->ExceptionCheck()) {
        env->ExceptionClear();
        stillDead = JNI_TRUE;
      }

      if (!stillDead) {
        env->CallVoidMethod(pctrl, g_mAttackEntity, player, bestTarget);
        bool attackOk = !env->ExceptionCheck();
        if (!attackOk)
          env->ExceptionClear();
        else {

          if (sTapEnabled) {
            STap_OnAttack((float)bestWorldDist);
            sTapActive = true;
          }
          if (wTapEnabled) {

            jint targetHash = env->CallIntMethod(bestTarget, env->GetMethodID(env->GetObjectClass(bestTarget), "hashCode", "()I"));
            if (env->ExceptionCheck()) { env->ExceptionClear(); targetHash = 0; }
            WTap_OnAttack((int)targetHash);
            wTapActive = true;
          }
        }

        if (attackOk && targetHudEnabled) {
          printf("[DEBUG] KillAura attack successful, getting target data...\n");

          const char* targetName = "";
          float targetHp = -1.f;
          float targetMaxHp = -1.f;

          if (g_mGetName) {
            jstring jName = (jstring)env->CallObjectMethod(bestTarget, g_mGetName);
            if (jName && !env->ExceptionCheck()) {
              targetName = env->GetStringUTFChars(jName, nullptr);
              if (!targetName) {
                env->ExceptionClear();
                targetName = "";
              }
              printf("[DEBUG] Got entity name: %s\n", targetName);
            } else {
              env->ExceptionClear();
              printf("[DEBUG] Failed to get entity name\n");
            }
          } else {
            printf("[DEBUG] g_mGetName is null\n");
          }

          if (g_mGetHealth) {
            targetHp = env->CallFloatMethod(bestTarget, g_mGetHealth);
            if (env->ExceptionCheck()) {
              env->ExceptionClear();
              targetHp = -1.f;
              printf("[DEBUG] Failed to get entity health\n");
            } else {
              printf("[DEBUG] Got entity health: %.1f\n", targetHp);
            }
          } else {
            printf("[DEBUG] g_mGetHealth is null\n");
          }

          if (g_mGetMaxHealth) {
            targetMaxHp = env->CallFloatMethod(bestTarget, g_mGetMaxHealth);
            if (env->ExceptionCheck()) {
              env->ExceptionClear();
              targetMaxHp = -1.f;
              printf("[DEBUG] Failed to get entity max health\n");
            } else {
              printf("[DEBUG] Got entity max health: %.1f\n", targetMaxHp);
            }
          } else {
            printf("[DEBUG] g_mGetMaxHealth is null\n");
          }

          NotifyTargetHit(targetName, targetHp, targetMaxHp);
          printf("[DEBUG] NotifyTargetHit called\n");

          if (g_mGetName && targetName && targetName[0] != '\0') {
            jstring jName = (jstring)env->CallObjectMethod(bestTarget, g_mGetName);
            if (jName) {
              env->ReleaseStringUTFChars(jName, targetName);
              env->DeleteLocalRef(jName);
            }
          }
        }

        if (attackOk && g_mSwingItem) {
          env->CallVoidMethod(player, g_mSwingItem);
          if (env->ExceptionCheck())
            env->ExceptionClear();
        }
      }
      QueryPerformanceCounter(&lastHit);
    }

    for (jobject gref : entitySnapshot) {
      env->DeleteGlobalRef(gref);
    }

    env->DeleteLocalRef(player);
    env->DeleteLocalRef(world);
    env->DeleteLocalRef(pctrl);
    env->DeleteLocalRef(mc);

    Sleep(10);
  }

  if (g_fBlockReach && g_jvm) {
    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (mc) {
      jobject pc = env->GetObjectField(mc, g_fPlayerController);
      if (pc) {
        env->SetFloatField(pc, g_fBlockReach, 4.5f);
        env->DeleteLocalRef(pc);
      }
      env->DeleteLocalRef(mc);
    }
  }
  if (livingBaseClass)
    env->DeleteGlobalRef(livingBaseClass);
  g_jvm->DetachCurrentThread();
}

void reachThread() {
  printf("[REACH DEBUG] Thread started\n");
  __try {
    reachThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    reachStatus = "Thread crashed (SEH)";
    printf("[REACH DEBUG] Thread crashed with SEH exception\n");
  }
}

static void velocityThreadInner() {

  while (!g_jniReady && !g_isUnloading) {
    Sleep(500);
  }
  if (g_isUnloading)
    return;

  if (!g_velocityJniReady || !g_jvm || !g_mcClass || !g_getMinecraft ||
      !g_fThePlayer || !g_fMotionX || !g_fMotionY || !g_fMotionZ) {
    velocityStatus = "JNI fields missing";
    return;
  }

  JNIEnv *env = nullptr;
  jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
  if (res == JNI_EDETACHED) {
    if (g_jvm->AttachCurrentThreadAsDaemon((void **)&env, nullptr) != JNI_OK) {
      velocityStatus = "Failed to attach thread";
      return;
    }
  } else if (res != JNI_OK || !env) {
    velocityStatus = "GetEnv failed";
    return;
  }

  static std::mt19937 rng(std::random_device{}());

  double prevMotionX = 0.0, prevMotionY = 0.0, prevMotionZ = 0.0;
  bool hasPrev = false;

  velocityStatus = "Active";

  while (!g_isUnloading) {
    if (!velocityEnabled) {
      hasPrev = false;
      Sleep(50);
      continue;
    }

    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (!mc || env->ExceptionCheck()) {
      env->ExceptionClear();
      hasPrev = false;
      Sleep(50);
      continue;
    }

    jobject player = env->GetObjectField(mc, g_fThePlayer);
    if (!player) {
      env->DeleteLocalRef(mc);
      hasPrev = false;
      Sleep(50);
      continue;
    }

    if (velocityOnlyOnGround && g_fOnGround) {
      jboolean onGround = env->GetBooleanField(player, g_fOnGround);
      if (!onGround) {
        env->DeleteLocalRef(player);
        env->DeleteLocalRef(mc);
        Sleep(5);
        continue;
      }
    }

    if (velocityOnlyWhileMoving) {
      bool moving = (GetAsyncKeyState('W') & 0x8000) ||
                    (GetAsyncKeyState('A') & 0x8000) ||
                    (GetAsyncKeyState('S') & 0x8000) ||
                    (GetAsyncKeyState('D') & 0x8000);
      if (!moving) {
        env->DeleteLocalRef(player);
        env->DeleteLocalRef(mc);
        hasPrev = false;
        Sleep(5);
        continue;
      }
    }

    double motionX = env->GetDoubleField(player, g_fMotionX);
    double motionY = env->GetDoubleField(player, g_fMotionY);
    double motionZ = env->GetDoubleField(player, g_fMotionZ);

    if (hasPrev) {

      double deltaX = motionX - prevMotionX;
      double deltaZ = motionZ - prevMotionZ;
      double deltaY = motionY - prevMotionY;
      double horizDelta2 = deltaX * deltaX + deltaZ * deltaZ;

      bool knockbackDetected = (horizDelta2 > 0.01) && (deltaY > 0.05);

      if (knockbackDetected) {

        bool applyReduction = true;
        if (velocityChance < 100.0f) {
          std::uniform_real_distribution<float> chanceDist(0.0f, 100.0f);
          applyReduction = chanceDist(rng) <= velocityChance;
        }

        if (applyReduction) {
          float hMul = velocityHorizontal / 100.0f;
          float vMul = velocityVertical / 100.0f;

          double newMotionX = prevMotionX + deltaX * hMul;
          double newMotionY = prevMotionY + deltaY * vMul;
          double newMotionZ = prevMotionZ + deltaZ * hMul;

          env->SetDoubleField(player, g_fMotionX, newMotionX);
          env->SetDoubleField(player, g_fMotionY, newMotionY);
          env->SetDoubleField(player, g_fMotionZ, newMotionZ);
          if (env->ExceptionCheck())
            env->ExceptionClear();

          motionX = newMotionX;
          motionY = newMotionY;
          motionZ = newMotionZ;
        }
      }
    }

    prevMotionX = motionX;
    prevMotionY = motionY;
    prevMotionZ = motionZ;
    hasPrev = true;

    env->DeleteLocalRef(player);
    env->DeleteLocalRef(mc);

    Sleep(5);
  }

  if (g_jvm)
    g_jvm->DetachCurrentThread();
}

void velocityThread() {
  __try {
    velocityThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    velocityStatus = "Thread crashed (SEH)";
  }
}

static void sprintThreadInner() {
  while (!g_jniReady && !g_isUnloading)
    Sleep(500);
  if (g_isUnloading)
    return;

  while (!g_isUnloading) {
    if (!sprintEnabled || showMenu || g_cursorVisible) {
      g_sprintPending = false;
      Sleep(50);
      continue;
    }
    bool moving = (GetAsyncKeyState('W') & 0x8000) != 0;
    g_sprintPending = moving;
    Sleep(20);
  }
  g_sprintPending = false;
}

void sprintThread() {
  __try {
    sprintThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {

  }
}

static void sprintResetThreadInner() {
  while (!g_jniReady && !g_isUnloading)
    Sleep(500);
  if (g_isUnloading)
    return;

  JNIEnv *env = nullptr;
  if (g_jvm) {
    jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
    if (res == JNI_EDETACHED) {
      if (g_jvm->AttachCurrentThreadAsDaemon((void **)&env, nullptr) != JNI_OK)
        return;
    } else if (res != JNI_OK) {
      return;
    }
  } else {
    return;
  }

  std::random_device rd;
  std::mt19937 rng(rd());

  bool lmbWasDown = false;

  while (!g_isUnloading) {
    if (!sprintResetEnabled || showMenu || g_cursorVisible) {
      Sleep(50);
      continue;
    }

    bool lmbDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool justClicked = (lmbDown && !lmbWasDown);
    lmbWasDown = lmbDown;

    if (!justClicked && g_lastAttackTime == 0) {
      Sleep(5);
      continue;
    }

    if (justClicked) {
      g_lastAttackTime = (DWORD)GetTickCount64();
    }

    DWORD lastAtk = g_lastAttackTime;
    if (lastAtk == 0) {
      Sleep(5);
      continue;
    }

    if (sprintResetWeaponOnly && !g_holdingSword) {
      Sleep(10);
      continue;
    }

    int delay = sprintResetDelay;
    int stopDur = sprintResetStopDuration;
    if (sprintResetRandomize) {
      std::uniform_int_distribution<int> delayJitter(-30, 30);
      std::uniform_int_distribution<int> stopJitter(-10, 15);
      delay = (std::max)(20, delay + delayJitter(rng));
      stopDur = (std::max)(10, stopDur + stopJitter(rng));
    }

    DWORD elapsed = (DWORD)GetTickCount64() - lastAtk;
    if (elapsed < (DWORD)delay) {
      Sleep(5);
      continue;
    }

    g_lastAttackTime = 0;

    bool holdingW = (GetAsyncKeyState('W') & 0x8000) != 0;
    if (!holdingW) {
      Sleep(10);
      continue;
    }

    HWND wnd = g_targetWindow;
    if (!wnd) continue;

    if (sprintResetMode == 0) {

      INPUT inp[2] = {};
      inp[0].type = INPUT_KEYBOARD;
      inp[0].ki.wVk = 'W';
      inp[0].ki.dwFlags = KEYEVENTF_KEYUP;
      SendInput(1, &inp[0], sizeof(INPUT));
      Sleep(stopDur);
      inp[1].type = INPUT_KEYBOARD;
      inp[1].ki.wVk = 'W';
      inp[1].ki.dwFlags = 0;
      SendInput(1, &inp[1], sizeof(INPUT));
    } else if (sprintResetMode == 1) {

      INPUT inp[2] = {};
      inp[0].type = INPUT_KEYBOARD;
      inp[0].ki.wVk = VK_LSHIFT;
      inp[0].ki.dwFlags = 0;
      SendInput(1, &inp[0], sizeof(INPUT));
      Sleep(stopDur);
      inp[1].type = INPUT_KEYBOARD;
      inp[1].ki.wVk = VK_LSHIFT;
      inp[1].ki.dwFlags = KEYEVENTF_KEYUP;
      SendInput(1, &inp[1], sizeof(INPUT));
    } else if (sprintResetMode == 2) {

      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        if (player) {
          if (g_mSetSprinting) {
            env->CallVoidMethod(player, g_mSetSprinting, (jboolean)JNI_FALSE);
            if (env->ExceptionCheck()) env->ExceptionClear();
          } else if (g_fIsSprinting) {
            env->SetBooleanField(player, g_fIsSprinting, JNI_FALSE);
            if (env->ExceptionCheck()) env->ExceptionClear();
          }
          Sleep(stopDur);
          if (g_mSetSprinting) {
            env->CallVoidMethod(player, g_mSetSprinting, (jboolean)JNI_TRUE);
            if (env->ExceptionCheck()) env->ExceptionClear();
          } else if (g_fIsSprinting) {
            env->SetBooleanField(player, g_fIsSprinting, JNI_TRUE);
            if (env->ExceptionCheck()) env->ExceptionClear();
          }
          env->DeleteLocalRef(player);
        }
        env->DeleteLocalRef(mc);
      } else {
        if (env->ExceptionCheck()) env->ExceptionClear();
      }
    }

    Sleep(100);
  }

  if (g_jvm)
    g_jvm->DetachCurrentThread();
}

void sprintResetThread() {
  __try {
    sprintResetThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {

  }
}

static void autoToolThreadInner() {
  while (!g_jniReady && !g_isUnloading)
    Sleep(500);
  if (g_isUnloading)
    return;

  JNIEnv *env = nullptr;
  if (g_jvm) {
    jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
    if (res == JNI_EDETACHED) {
      if (g_jvm->AttachCurrentThreadAsDaemon((void **)&env, nullptr) != JNI_OK)
        return;
    } else if (res != JNI_OK) {
      return;
    }
  } else {
    return;
  }

  bool jniOk = g_mGetStackInSlot && g_fInventory && g_fCurrentItemIdx &&
               g_mGetBlockState && g_mGetBlock && g_mGetStrVsBlock &&
               g_fObjectMouseOver;

  int originalSlot = -1;
  bool isAimingBlock = false;
  DWORD lastBlockAimTime = 0;

  while (!g_isUnloading) {
    if (!autoToolEnabled || showMenu || !jniOk) {
      isAimingBlock = false;
      lastBlockAimTime = 0;
      originalSlot = -1;
      Sleep(50);
      continue;
    }

    if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {

      if (autoToolSwitchBack && originalSlot >= 0) {
        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (mc && !env->ExceptionCheck()) {
          jobject player = env->GetObjectField(mc, g_fThePlayer);
          if (player) {
            jobject inv = env->GetObjectField(player, g_fInventory);
            if (inv) {
              env->SetIntField(inv, g_fCurrentItemIdx, originalSlot);
              if (env->ExceptionCheck()) env->ExceptionClear();
              env->DeleteLocalRef(inv);
            }
            env->DeleteLocalRef(player);
          }
          env->DeleteLocalRef(mc);
        } else {
          if (env->ExceptionCheck()) env->ExceptionClear();
        }
      }
      isAimingBlock = false;
      lastBlockAimTime = 0;
      originalSlot = -1;
      Sleep(20);
      continue;
    }

    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (!mc || env->ExceptionCheck()) {
      if (env->ExceptionCheck()) env->ExceptionClear();
      Sleep(20);
      continue;
    }

    jobject player = env->GetObjectField(mc, g_fThePlayer);
    if (!player) {
      env->DeleteLocalRef(mc);
      Sleep(20);
      continue;
    }

    jobject mop = env->GetObjectField(mc, g_fObjectMouseOver);
    bool aimingBlock = false;
    if (mop) {

      if (g_fEntityHit) {
        jobject entHit = env->GetObjectField(mop, g_fEntityHit);
        aimingBlock = (entHit == nullptr);
        if (entHit) env->DeleteLocalRef(entHit);
      }
      env->DeleteLocalRef(mop);
    }

    if (!aimingBlock) {
      isAimingBlock = false;
      lastBlockAimTime = 0;
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(mc);
      Sleep(20);
      continue;
    }

    if (!isAimingBlock) {
      isAimingBlock = true;
      lastBlockAimTime = (DWORD)GetTickCount64();
    }

    if ((DWORD)GetTickCount64() - lastBlockAimTime < (DWORD)autoToolActivationDelay) {
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(mc);
      Sleep(10);
      continue;
    }

    if (autoToolSneakOnly && g_mIsSneaking) {
      jboolean sneaking = env->CallBooleanMethod(player, g_mIsSneaking);
      if (env->ExceptionCheck()) { env->ExceptionClear(); sneaking = JNI_FALSE; }
      if (!sneaking) {
        env->DeleteLocalRef(player);
        env->DeleteLocalRef(mc);
        Sleep(20);
        continue;
      }
    }

    jobject inv = env->GetObjectField(player, g_fInventory);
    if (!inv) {
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(mc);
      Sleep(20);
      continue;
    }

    int curSlot = env->GetIntField(inv, g_fCurrentItemIdx);
    if (originalSlot < 0)
      originalSlot = curSlot;

    jobject world = env->GetObjectField(mc, g_fTheWorld);
    if (!world) {
      env->DeleteLocalRef(inv);
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(mc);
      Sleep(20);
      continue;
    }

    jobject mop2 = env->GetObjectField(mc, g_fObjectMouseOver);
    jobject blockObj = nullptr;
    if (mop2) {

      static jmethodID g_mGetBlockPos = nullptr;
      static bool triedBlockPos = false;
      if (!triedBlockPos) {
        const char *gbpNames[] = {"func_178782_a", "getBlockPos"};
        jclass mopClass = env->GetObjectClass(mop2);
        if (mopClass) {
          g_mGetBlockPos = TryMethodN(env, mopClass, gbpNames, 2,
                                      "()Lnet/minecraft/util/BlockPos;");
          if (env->ExceptionCheck()) env->ExceptionClear();
          env->DeleteLocalRef(mopClass);
        }
        triedBlockPos = true;
      }

      if (g_mGetBlockPos) {
        jobject blockPos = env->CallObjectMethod(mop2, g_mGetBlockPos);
        if (blockPos && !env->ExceptionCheck()) {

          jobject blockState = env->CallObjectMethod(world, g_mGetBlockState, blockPos);
          if (blockState && !env->ExceptionCheck()) {

            blockObj = env->CallObjectMethod(blockState, g_mGetBlock);
            if (env->ExceptionCheck()) { env->ExceptionClear(); blockObj = nullptr; }
            env->DeleteLocalRef(blockState);
          } else {
            if (env->ExceptionCheck()) env->ExceptionClear();
          }
          env->DeleteLocalRef(blockPos);
        } else {
          if (env->ExceptionCheck()) env->ExceptionClear();
        }
      }
      env->DeleteLocalRef(mop2);
    }

    if (blockObj) {

      float bestSpeed = 0.0f;
      int bestSlot = curSlot;

      for (int i = 0; i < 9; i++) {
        jobject stack = env->CallObjectMethod(inv, g_mGetStackInSlot, (jint)i);
        if (stack && !env->ExceptionCheck() && !env->IsSameObject(stack, nullptr)) {
          float speed = env->CallFloatMethod(stack, g_mGetStrVsBlock, blockObj);
          if (env->ExceptionCheck()) { env->ExceptionClear(); speed = 1.0f; }
          if (speed > bestSpeed) {
            bestSpeed = speed;
            bestSlot = i;
          }
          env->DeleteLocalRef(stack);
        } else {
          if (env->ExceptionCheck()) env->ExceptionClear();

          if (autoToolAllowFists && 1.0f > bestSpeed) {
            bestSpeed = 1.0f;
            bestSlot = i;
          }
        }
      }

      if (bestSlot != curSlot && bestSpeed > 1.0f) {
        env->SetIntField(inv, g_fCurrentItemIdx, bestSlot);
        if (env->ExceptionCheck()) env->ExceptionClear();
      }

      env->DeleteLocalRef(blockObj);
    }

    env->DeleteLocalRef(world);
    env->DeleteLocalRef(inv);
    env->DeleteLocalRef(player);
    env->DeleteLocalRef(mc);

    Sleep(50);
  }

  if (g_jvm)
    g_jvm->DetachCurrentThread();
}

void autoToolThread() {
  __try {
    autoToolThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {

  }
}

void clickerThread() {
  clickerThreadStatus = "Running";
  LARGE_INTEGER freq, lastClickTime, now;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&lastClickTime);

  static std::random_device rd;
  static std::mt19937 gen(rd());

  int clicksThisSecond = 0;
  DWORD secondStart = static_cast<DWORD>(GetTickCount64());

  double nextIntervalMs = 1000.0 / (std::max)(1, clickerCpsInt);
  bool wasActive = false;
  double clickDrift = 0.0;
  int clicksSinceDrift = 0;

  while (!g_isUnloading) {

    if (!clickerEnabled || showMenu) {
      clickerThreadStatus = !clickerEnabled ? "Disabled" : "Menu open";
      currentCps = 0.0;
      clicksThisSecond = 0;
      wasActive = false;
      Sleep(50);
      continue;
    }

    HWND targetWnd = GetForegroundWindow();
    if (!targetWnd || !g_targetWindow || targetWnd != g_targetWindow) {
      clickerThreadStatus =
          !g_targetWindow ? "No Minecraft window" : "Window not focused";
      wasActive = false;
      Sleep(50);
      continue;
    }

    bool active = false;
    switch (clickerActivationMode) {
    case 0:
      active = true;
      break;
    case 1:
      active = clickerActivationKeyHeld;
      break;
    case 2:
      active = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
      break;
    }

    if (!active) {
      clickerThreadStatus = "Waiting for activation";
      wasActive = false;
      currentCps = 0.0;
      clicksThisSecond = 0;
      Sleep(10);
      continue;
    }

    static DWORD lastCursorVisibleTime = 0;
    if (g_cursorVisible) {
      lastCursorVisibleTime = (DWORD)GetTickCount64();
      if (!clickerAllowInInventory || !g_isInventoryOpen) {
        clickerThreadStatus = g_isInventoryOpen ? "Inventory (blocked)"
                                                : "Cursor visible (chat/menu)";
        wasActive = false;
        currentCps = 0.0;
        clicksThisSecond = 0;
        Sleep(10);
        continue;
      }
    } else if (lastCursorVisibleTime > 0) {

      if ((DWORD)GetTickCount64() - lastCursorVisibleTime < 150) {
        clickerThreadStatus = "Cursor recently visible";
        wasActive = false;
        currentCps = 0.0;
        clicksThisSecond = 0;
        Sleep(10);
        continue;
      } else {
        lastCursorVisibleTime = 0;
      }
    }

    if (clickerSwordOnly && !g_holdingSword) {
      clickerThreadStatus = "Waiting for sword";
      wasActive = false;
      currentCps = 0.0;
      clicksThisSecond = 0;
      Sleep(10);
      continue;
    }

    clickerThreadStatus = "Clicking";

    if (!wasActive) {
      QueryPerformanceCounter(&lastClickTime);
      secondStart = static_cast<DWORD>(GetTickCount64());
      clicksThisSecond = 0;
      clickDrift = 0.0;
      clicksSinceDrift = 0;

      double initCps = (double)(std::max)(1, clickerCpsInt);
      if (clickerEnableRandomization) {
        double sigma = clickerRandomizationAmount * 0.45;
        std::normal_distribution<double> norm(initCps, sigma);
        initCps = norm(gen);

        std::uniform_real_distribution<double> jitter(-8.0, 8.0);
        double jitterMs = jitter(gen);
        initCps = (std::max)(1.0, (std::min)(25.0, initCps));
        nextIntervalMs = (std::max)(20.0, 1000.0 / initCps + jitterMs);
      } else {
        nextIntervalMs = 1000.0 / initCps;
      }
      if (clickerExhaustMode && shouldApplyExhaust()) {
        nextIntervalMs *= 1.0 + clickerExhaustDropCps / initCps;
      }
      if (clickerSpikeMode && shouldApplySpike()) {
        nextIntervalMs *= (std::max)(0.3, 1.0 - clickerSpikeIncreaseCps / initCps);
      }
      wasActive = true;
    }

    QueryPerformanceCounter(&now);
    double elapsedMs = (double)(now.QuadPart - lastClickTime.QuadPart) *
                       1000.0 / (double)freq.QuadPart;

    if (elapsedMs >= nextIntervalMs) {

      executeClick(true, targetWnd);
      QueryPerformanceCounter(&lastClickTime);
      clicksThisSecond++;

      double baseCps = (double)(std::max)(1, clickerCpsInt);
      double cps = baseCps;

      if (clickerEnableRandomization) {

        clicksSinceDrift++;
        if (clicksSinceDrift >= 5 + (int)(gen() % 8)) {
          std::normal_distribution<double> driftDist(0.0, clickerRandomizationAmount * 0.3);
          clickDrift = driftDist(gen);
          clicksSinceDrift = 0;
        }

        double sigma = clickerRandomizationAmount * 0.45;
        std::normal_distribution<double> norm(baseCps + clickDrift, sigma);
        cps = norm(gen);
        cps = (std::max)(1.0, (std::min)(25.0, cps));

        std::uniform_real_distribution<double> jitter(-10.0, 10.0);
        double jitterMs = jitter(gen);

        double burstMod = 0.0;
        std::uniform_int_distribution<int> patternRoll(0, 99);
        int roll = patternRoll(gen);
        if (roll < 8) {

          std::uniform_real_distribution<double> burstDist(-30.0, -15.0);
          burstMod = burstDist(gen);
        } else if (roll < 14) {

          std::uniform_real_distribution<double> pauseDist(20.0, 50.0);
          burstMod = pauseDist(gen);
        }

        double intervalMs = 1000.0 / cps + jitterMs + burstMod;
        nextIntervalMs = (std::max)(20.0, intervalMs);
      } else {
        nextIntervalMs = 1000.0 / cps;
      }

      if (clickerExhaustMode && shouldApplyExhaust()) {
        nextIntervalMs *= 1.0 + clickerExhaustDropCps / baseCps;
      }

      if (clickerSpikeMode && shouldApplySpike()) {
        nextIntervalMs *= (std::max)(0.3, 1.0 - clickerSpikeIncreaseCps / baseCps);
      }

      currentCps = 1000.0 / nextIntervalMs;
    }

    DWORD nowTick = static_cast<DWORD>(GetTickCount64());
    if (nowTick - secondStart >= 1000) {
      currentCps = (double)clicksThisSecond;
      clicksThisSecond = 0;
      secondStart = nowTick;
    }

    Sleep(1);
  }
}

void rightClickerThread() {
  rightClickerThreadStatus = "Running";
  LARGE_INTEGER freq, lastClickTime, now;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&lastClickTime);

  static std::mt19937 rcGen(std::random_device{}());
  double nextIntervalMs = 1000.0 / (std::max)(1, rightClickerCpsInt);
  int clicksThisSecond = 0;
  DWORD secondStart = static_cast<DWORD>(GetTickCount64());
  bool wasActive = false;

  while (!g_isUnloading) {
    if (!rightClickerEnabled || showMenu) {
      rightClickerThreadStatus = !rightClickerEnabled ? "Disabled" : "Menu open";
      rightClickerCurrentCps = 0.0;
      clicksThisSecond = 0;
      wasActive = false;
      Sleep(50);
      continue;
    }

    HWND targetWnd = g_targetWindow;
    if (!targetWnd || !IsWindow(targetWnd) ||
        GetForegroundWindow() != targetWnd) {
      rightClickerThreadStatus = "Waiting for window";
      Sleep(50);
      continue;
    }

    if (!rightClickerAllowInInventory && (g_cursorVisible || g_isInventoryOpen)) {
      rightClickerThreadStatus = "Inventory open";
      Sleep(50);
      continue;
    }

    bool isActive = false;
    switch (rightClickerActivationMode) {
    case 0:
      isActive = true;
      break;
    case 1:
      isActive = rightClickerActivationKey != 0 &&
                 (GetAsyncKeyState(rightClickerActivationKey) & 0x8000) != 0;
      break;
    case 2:
      isActive = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
      break;
    }

    if (!isActive) {
      rightClickerThreadStatus = "Waiting";
      rightClickerCurrentCps = 0.0;
      clicksThisSecond = 0;
      wasActive = false;
      Sleep(10);
      continue;
    }

    rightClickerThreadStatus = "Active";

    if (!wasActive) {
      QueryPerformanceCounter(&lastClickTime);
      double cpsCap = rightClickerBlatant ? 50.0 : 25.0;
      double cps = (double)(std::max)(1, rightClickerCpsInt);
      if (rightClickerEnableRandomization) {
        double lo = (std::max)(1.0, cps - rightClickerRandomizationAmount);
        double hi = (std::min)(cpsCap, cps + rightClickerRandomizationAmount);
        std::uniform_real_distribution<double> dist(lo, hi);
        cps = dist(rcGen);
      }
      nextIntervalMs = 1000.0 / cps;
      wasActive = true;
    }

    QueryPerformanceCounter(&now);
    double elapsedMs = (double)(now.QuadPart - lastClickTime.QuadPart) *
                       1000.0 / (double)freq.QuadPart;

    if (elapsedMs >= nextIntervalMs) {
      executeClick(false, targetWnd);
      QueryPerformanceCounter(&lastClickTime);
      clicksThisSecond++;
      rightClickerTotalClicks++;

      double cpsCap = rightClickerBlatant ? 50.0 : 25.0;
      double cps = (double)(std::max)(1, rightClickerCpsInt);
      if (rightClickerEnableRandomization) {
        double lo = (std::max)(1.0, cps - rightClickerRandomizationAmount);
        double hi = (std::min)(cpsCap, cps + rightClickerRandomizationAmount);
        std::uniform_real_distribution<double> dist(lo, hi);
        cps = dist(rcGen);
      }
      nextIntervalMs = 1000.0 / cps;
    }

    DWORD nowTick = static_cast<DWORD>(GetTickCount64());
    if (nowTick - secondStart >= 1000) {
      rightClickerCurrentCps = (double)clicksThisSecond;
      clicksThisSecond = 0;
      secondStart = nowTick;
    }

    Sleep(1);
  }
}

void fastPlaceThread() {
  JNIEnv *env = nullptr;
  bool attached = false;

  while (!g_isUnloading) {
    if (!fastPlaceEnabled || !g_fastPlaceJniReady || !g_jvm) {
      Sleep(50);
      continue;
    }

    if (!attached) {
      jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
      if (res == JNI_EDETACHED) {
        res = g_jvm->AttachCurrentThread((void **)&env, nullptr);
      }
      if (res != JNI_OK || !env) {
        Sleep(500);
        continue;
      }
      attached = true;
    }

    if (showMenu || g_cursorVisible || g_isInventoryOpen) {
      Sleep(50);
      continue;
    }

    if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
      Sleep(10);
      continue;
    }

    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (mc && !env->ExceptionCheck()) {

      jint curTimer = env->GetIntField(mc, g_fRightClickDelayTimer);
      if (!env->ExceptionCheck() && curTimer > fastPlaceTickDelay) {
        env->SetIntField(mc, g_fRightClickDelayTimer, fastPlaceTickDelay);
      }
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(mc);
    } else {
      if (env->ExceptionCheck())
        env->ExceptionClear();
    }

    Sleep(10);
  }

  if (attached && g_jvm) {
    g_jvm->DetachCurrentThread();
  }
}

void blockhitThread() {
  static std::random_device rd;
  static std::mt19937 gen(rd());

  DWORD lastBlockTime = static_cast<DWORD>(GetTickCount64());

  while (!g_isUnloading) {
    if (!blockhitEnabled) {
      Sleep(100);
      continue;
    }

    if (showMenu) {
      Sleep(100);
      continue;
    }

    HWND minecraftWindow = GetForegroundWindow();
    if (!minecraftWindow) {
      Sleep(100);
      continue;
    }

    if (blockhitSwordOnly && !g_holdingSword) {
      Sleep(50);
      continue;
    }

    if (blockhitOnlyWhileClicking) {
      bool isLeftClicking = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
      if (!isLeftClicking) {
        Sleep(50);
        continue;
      }
    }

    DWORD currentTime = static_cast<DWORD>(GetTickCount64());

    std::uniform_int_distribution<> delayDis(blockhitDelayMin,
                                             blockhitDelayMax);
    int requiredDelay = delayDis(gen);

    if (currentTime - lastBlockTime >= static_cast<DWORD>(requiredDelay)) {
      std::uniform_int_distribution<> chanceDis(1, 100);
      int roll = chanceDis(gen);

      if (roll <= blockhitBlockChance) {
        std::uniform_int_distribution<> holdDis(blockhitHoldLengthMin,
                                                blockhitHoldLengthMax);
        int holdTime = holdDis(gen);

        executeRightClick(minecraftWindow, holdTime);
        lastBlockTime = currentTime;
      }
    }

    Sleep(10);
  }
}

static void hitSelectThreadInner() {

  while (!g_jniReady && !g_isUnloading)
    Sleep(500);
  if (g_isUnloading)
    return;

  JNIEnv *env = nullptr;
  bool jniAvailable = false;
  if (g_jvm && g_mcClass && g_getMinecraft && g_fThePlayer) {
    jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
    if (res == JNI_EDETACHED) {
      if (g_jvm->AttachCurrentThreadAsDaemon((void **)&env, nullptr) == JNI_OK)
        jniAvailable = true;
    } else if (res == JNI_OK && env) {
      jniAvailable = true;
    }
  }

  if (!jniAvailable || !env) {
    hitSelectStatus = "JNI not available";
    return;
  }

  hitSelectStatus = "Active";
  double prevMotionY = 0.0;
  bool hasPrevMotion = false;
  int prevHurtTime = 0;

  while (!g_isUnloading) {

    if (!hitSelectEnabled && !g_swordCheckReady) {
      hitSelectStatus = "Disabled";
      g_hsTargetHurtResist = 0;
      g_hsTargetHurtTime = 0;
      g_hsAimingAtEntity = false;
      g_hsTakingKB = false;
      g_hsFirstHitReceived = false;
      g_hsCombatStartTime = 0;
      g_hsTargetJustDamaged = false;
      hasPrevMotion = false;
      prevHurtTime = 0;
      Sleep(50);
      continue;
    }

    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (!mc || env->ExceptionCheck()) {
      if (env->ExceptionCheck())
        env->ExceptionClear();
      Sleep(50);
      continue;
    }

    jobject player = env->GetObjectField(mc, g_fThePlayer);
    if (!player) {
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    if (g_swordCheckReady) {
      jobject itemStack = env->CallObjectMethod(player, g_mGetCurrentItem);
      if (itemStack && !env->ExceptionCheck()) {
        jobject item = env->CallObjectMethod(itemStack, g_mStackGetItem);
        if (item && !env->ExceptionCheck()) {
          g_holdingSword = env->IsInstanceOf(item, g_itemSwordClass);
          env->DeleteLocalRef(item);
        } else {
          g_holdingSword = false;
          if (env->ExceptionCheck())
            env->ExceptionClear();
        }
        env->DeleteLocalRef(itemStack);
      } else {
        g_holdingSword = false;
        if (env->ExceptionCheck())
          env->ExceptionClear();
      }
    }

    if (hitSelectEnabled) {
      hitSelectStatus = "Running";
      DWORD now = (DWORD)GetTickCount64();

      double motionY = 0.0;
      bool playerOnGround = true;
      if (g_fMotionY)
        motionY = env->GetDoubleField(player, g_fMotionY);
      if (g_fOnGround)
        playerOnGround = (bool)env->GetBooleanField(player, g_fOnGround);

      g_hsPlayerOnGround = playerOnGround;
      g_hsMotionY = motionY;

      if (hasPrevMotion) {
        double deltaY = motionY - prevMotionY;
        if (deltaY > 0.1 && !playerOnGround) {
          g_hsTakingKB = true;
          g_hsLastDamageTime = now;
          g_hsFirstHitReceived = true;
        } else if (playerOnGround) {
          g_hsTakingKB = false;
        }
      }
      prevMotionY = motionY;
      hasPrevMotion = true;

      bool wasAiming = g_hsAimingAtEntity;
      g_hsAimingAtEntity = false;
      g_hsTargetHurtResist = 0;
      g_hsTargetHurtTime = 0;

      if (g_fObjectMouseOver && g_fEntityHit && g_fHurtResistantTime) {
        jobject mouseOver = env->GetObjectField(mc, g_fObjectMouseOver);
        if (mouseOver && !env->ExceptionCheck()) {
          jobject entityHit = env->GetObjectField(mouseOver, g_fEntityHit);
          if (entityHit && !env->ExceptionCheck()) {
            g_hsAimingAtEntity = true;
            g_hsLastAimTime = now;

            g_hsTargetHurtResist =
                env->GetIntField(entityHit, g_fHurtResistantTime);
            if (env->ExceptionCheck())
              env->ExceptionClear();

            int curHurtTime = 0;
            if (g_fHurtTime) {
              curHurtTime = env->GetIntField(entityHit, g_fHurtTime);
              if (env->ExceptionCheck())
                env->ExceptionClear();
            }
            g_hsTargetHurtTime = curHurtTime;

            if (curHurtTime > prevHurtTime && curHurtTime > 0) {
              g_hsTargetJustDamaged = true;
            }
            prevHurtTime = curHurtTime;

            if (g_hsCombatStartTime == 0)
              g_hsCombatStartTime = now;

            env->DeleteLocalRef(entityHit);
          } else {
            if (env->ExceptionCheck())
              env->ExceptionClear();
          }
          env->DeleteLocalRef(mouseOver);
        } else {
          if (env->ExceptionCheck())
            env->ExceptionClear();
        }
      }

      if (!g_hsAimingAtEntity) {
        if (g_hsLastAimTime > 0 && (now - g_hsLastAimTime) > 3000) {
          g_hsFirstHitReceived = false;
          g_hsCombatStartTime = 0;
          g_hsLastAttackTime = 0;
          g_hsTargetJustDamaged = false;
          prevHurtTime = 0;
        }
      }

      if (g_hsNeedFakeSwing) {
        g_hsNeedFakeSwing = false;
        if (g_mSwingItem) {
          env->CallVoidMethod(player, g_mSwingItem);
          if (env->ExceptionCheck())
            env->ExceptionClear();
        }
      }
    } else {
      g_hsTargetHurtResist = 0;
      g_hsTargetHurtTime = 0;
      g_hsAimingAtEntity = false;
      g_hsFirstHitReceived = false;
      g_hsCombatStartTime = 0;
      g_hsTargetJustDamaged = false;
      hasPrevMotion = false;
      prevHurtTime = 0;
    }

    env->DeleteLocalRef(player);
    env->DeleteLocalRef(mc);

    Sleep(5);
  }

  hitSelectStatus = "Stopped";
  g_jvm->DetachCurrentThread();
}

void hitSelectThread() {
  __try {
    hitSelectThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    hitSelectStatus = "Thread crashed (SEH)";
  }
}

static void throwpotThreadInner() {
  while (!g_jniReady && !g_isUnloading)
    Sleep(500);
  if (g_isUnloading)
    return;

  int waitAttempts = 0;
  while ((!g_fInventory || !g_fCurrentItemIdx || !g_mGetStackInSlot ||
          !g_mSendUseItem) &&
         !g_isUnloading && waitAttempts < 60) {
    Sleep(500);
    waitAttempts++;
  }
  if (!g_fInventory || !g_fCurrentItemIdx || !g_mGetStackInSlot ||
      !g_mSendUseItem) {
    throwpotStatus = "JNI fields missing";
    return;
  }
  if (!g_jvm) {
    throwpotStatus = "No JVM";
    return;
  }

  JNIEnv *env = nullptr;
  jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
  if (res == JNI_EDETACHED)
    g_jvm->AttachCurrentThreadAsDaemon((void **)&env, nullptr);
  if (!env) {
    throwpotStatus = "Attach failed";
    return;
  }

  jmethodID mGetIdFromItem = nullptr;
  if (g_itemClass) {
    const char *gifNames[] = {"func_150891_b", "getIdFromItem"};
    mGetIdFromItem = TryStaticMethodN(env, g_itemClass, gifNames, 2,
                                      "(Lnet/minecraft/item/Item;)I");
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  jmethodID mGetItemDamage = nullptr;
  {
    const char *isNames[] = {"net.minecraft.item.ItemStack"};
    jclass isClass = LoadMCClass(env, isNames, 1);
    if (isClass) {
      const char *dmgNames[] = {"func_77960_j", "getItemDamage"};
      mGetItemDamage = TryMethodN(env, isClass, dmgNames, 2, "()I");
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(isClass);
    }
  }

  throwpotStatus = "Active";

  while (!g_isUnloading) {
    if (!throwpotEnabled) {
      Sleep(50);
      continue;
    }

    static bool lastKeyState = false;
    bool keyDown = g_throwpotKeyHeld;
    bool justPressed = keyDown && !lastKeyState;
    lastKeyState = keyDown;

    if (!justPressed) {
      Sleep(10);
      continue;
    }
    if (g_cursorVisible) {
      continue;
    }
    if (throwpotSwordOnly && !g_holdingSword) {
      continue;
    }

    DWORD now = (DWORD)GetTickCount64();
    if (now - g_throwpotLastThrow < (DWORD)throwpotCooldownMs) {
      continue;
    }

    if (!g_mcClass || !g_getMinecraft || !g_fThePlayer) {
      continue;
    }

    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (!mc || env->ExceptionCheck()) {
      env->ExceptionClear();
      continue;
    }

    jobject player = env->GetObjectField(mc, g_fThePlayer);
    jobject world = env->GetObjectField(mc, g_fTheWorld);
    jobject pctrl = env->GetObjectField(mc, g_fPlayerController);

    if (!player || !world || !pctrl) {
      if (player)
        env->DeleteLocalRef(player);
      if (world)
        env->DeleteLocalRef(world);
      if (pctrl)
        env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      continue;
    }

    jobject inventory = env->GetObjectField(player, g_fInventory);
    if (!inventory || env->ExceptionCheck()) {
      env->ExceptionClear();
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(world);
      env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      continue;
    }

    jint originalSlot = env->GetIntField(inventory, g_fCurrentItemIdx);
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
      originalSlot = 0;
    }

    std::vector<int> potSlots;
    for (int s = 0; s <= 8; s++) {
      jobject stack = env->CallObjectMethod(inventory, g_mGetStackInSlot, s);
      if (!stack || env->ExceptionCheck()) {
        env->ExceptionClear();
        continue;
      }

      bool isSplashHealth = false;
      if (g_mStackGetItem && mGetIdFromItem && mGetItemDamage) {
        jobject item = env->CallObjectMethod(stack, g_mStackGetItem);
        if (item && !env->ExceptionCheck()) {
          jint itemId =
              env->CallStaticIntMethod(g_itemClass, mGetIdFromItem, item);
          if (!env->ExceptionCheck() && itemId == 373) {
            jint damage = env->CallIntMethod(stack, mGetItemDamage);
            if (!env->ExceptionCheck()) {

              bool isSplash = (damage & 0x4000) != 0;
              bool isHarm = ((damage & 0xFF) == 0x24 &&
                             (damage & 0x40) != 0);
              if (isSplash && !isHarm) {
                isSplashHealth = true;
              }
            } else
              env->ExceptionClear();
          } else if (env->ExceptionCheck())
            env->ExceptionClear();
          env->DeleteLocalRef(item);
        } else if (env->ExceptionCheck())
          env->ExceptionClear();
      }
      env->DeleteLocalRef(stack);
      if (isSplashHealth)
        potSlots.push_back(s);
    }

    if (potSlots.empty()) {
      env->DeleteLocalRef(inventory);
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(world);
      env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      throwpotStatus = "No pots found";
      continue;
    }

    throwpotStatus = "Throwing";

    int toThrow = (int)potSlots.size() < throwpotPotCount ? (int)potSlots.size()
                                                          : throwpotPotCount;

    for (int i = 0; i < toThrow && !g_isUnloading; i++) {
      Sleep(throwpotSwitchDelay);
      env->SetIntField(inventory, g_fCurrentItemIdx, potSlots[i]);
      if (env->ExceptionCheck()) {
        env->ExceptionClear();
        break;
      }

      Sleep(throwpotThrowDelay);

      jobject heldStack =
          env->CallObjectMethod(inventory, g_mGetStackInSlot, potSlots[i]);
      if (heldStack && !env->ExceptionCheck()) {
        env->CallBooleanMethod(pctrl, g_mSendUseItem, player, world, heldStack);
        if (env->ExceptionCheck())
          env->ExceptionClear();
        env->DeleteLocalRef(heldStack);
      } else if (env->ExceptionCheck())
        env->ExceptionClear();
    }

    Sleep(throwpotSwitchDelay);
    env->SetIntField(inventory, g_fCurrentItemIdx, originalSlot);
    if (env->ExceptionCheck())
      env->ExceptionClear();

    env->DeleteLocalRef(inventory);
    env->DeleteLocalRef(player);
    env->DeleteLocalRef(world);
    env->DeleteLocalRef(pctrl);
    env->DeleteLocalRef(mc);

    g_throwpotLastThrow = (DWORD)GetTickCount64();
    throwpotStatus = "Active";

    Sleep(10);
  }

  throwpotStatus = "Stopped";
  g_jvm->DetachCurrentThread();
}

void throwpotThread() {
  __try {
    throwpotThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    throwpotStatus = "Thread crashed";
  }
}

static void autosoupThreadInner() {
  while (!g_jniReady && !g_isUnloading)
    Sleep(500);
  if (g_isUnloading)
    return;

  int waitAttempts = 0;
  while ((!g_fInventory || !g_fCurrentItemIdx || !g_mGetStackInSlot ||
          !g_mSendUseItem) &&
         !g_isUnloading && waitAttempts < 60) {
    Sleep(500);
    waitAttempts++;
  }
  if (!g_fInventory || !g_fCurrentItemIdx || !g_mGetStackInSlot ||
      !g_mSendUseItem) {
    autosoupStatus = "JNI fields missing";
    return;
  }
  if (!g_jvm) {
    autosoupStatus = "No JVM";
    return;
  }

  JNIEnv *env = nullptr;
  jint res = g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8);
  if (res == JNI_EDETACHED)
    g_jvm->AttachCurrentThreadAsDaemon((void **)&env, nullptr);
  if (!env) {
    autosoupStatus = "Attach failed";
    return;
  }

  jmethodID mGetIdFromItem = nullptr;
  if (g_itemClass) {
    const char *gifNames[] = {"func_150891_b", "getIdFromItem"};
    mGetIdFromItem = TryStaticMethodN(env, g_itemClass, gifNames, 2,
                                      "(Lnet/minecraft/item/Item;)I");
    if (env->ExceptionCheck())
      env->ExceptionClear();
  }

  autosoupStatus = "Active";

  while (!g_isUnloading) {
    if (!autosoupEnabled || !g_autosoupKeyHeld) {
      Sleep(20);
      continue;
    }
    if (g_cursorVisible) {
      Sleep(20);
      continue;
    }
    if (autosoupSwordOnly && !g_holdingSword) {
      Sleep(20);
      continue;
    }

    DWORD now = (DWORD)GetTickCount64();
    if (now - g_autosoupLastEat < (DWORD)autosoupCooldownMs) {
      Sleep(10);
      continue;
    }

    if (!g_mcClass || !g_getMinecraft || !g_fThePlayer) {
      Sleep(100);
      continue;
    }

    jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
    if (!mc || env->ExceptionCheck()) {
      env->ExceptionClear();
      Sleep(50);
      continue;
    }

    jobject player = env->GetObjectField(mc, g_fThePlayer);
    jobject world = env->GetObjectField(mc, g_fTheWorld);
    jobject pctrl = env->GetObjectField(mc, g_fPlayerController);

    if (!player || !world || !pctrl) {
      if (player)
        env->DeleteLocalRef(player);
      if (world)
        env->DeleteLocalRef(world);
      if (pctrl)
        env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    jobject inventory = env->GetObjectField(player, g_fInventory);
    if (!inventory || env->ExceptionCheck()) {
      env->ExceptionClear();
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(world);
      env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    jint originalSlot = env->GetIntField(inventory, g_fCurrentItemIdx);
    if (env->ExceptionCheck()) {
      env->ExceptionClear();
      originalSlot = 0;
    }

    int foundSlot = -1;
    for (int s = 0; s <= 8 && foundSlot < 0; s++) {
      jobject stack = env->CallObjectMethod(inventory, g_mGetStackInSlot, s);
      if (!stack || env->ExceptionCheck()) {
        env->ExceptionClear();
        continue;
      }

      if (g_mStackGetItem && mGetIdFromItem) {
        jobject item = env->CallObjectMethod(stack, g_mStackGetItem);
        if (item && !env->ExceptionCheck()) {
          jint itemId =
              env->CallStaticIntMethod(g_itemClass, mGetIdFromItem, item);
          if (!env->ExceptionCheck() && itemId == 282)
            foundSlot = s;
          else if (env->ExceptionCheck())
            env->ExceptionClear();
          env->DeleteLocalRef(item);
        } else if (env->ExceptionCheck())
          env->ExceptionClear();
      }
      env->DeleteLocalRef(stack);
    }

    if (foundSlot < 0) {
      env->DeleteLocalRef(inventory);
      env->DeleteLocalRef(player);
      env->DeleteLocalRef(world);
      env->DeleteLocalRef(pctrl);
      env->DeleteLocalRef(mc);
      Sleep(50);
      continue;
    }

    Sleep(autosoupSwitchDelay);
    env->SetIntField(inventory, g_fCurrentItemIdx, foundSlot);
    if (env->ExceptionCheck())
      env->ExceptionClear();

    Sleep(autosoupEatDelay);

    jobject heldStack =
        env->CallObjectMethod(inventory, g_mGetStackInSlot, foundSlot);
    if (heldStack && !env->ExceptionCheck()) {
      env->CallBooleanMethod(pctrl, g_mSendUseItem, player, world, heldStack);
      if (env->ExceptionCheck())
        env->ExceptionClear();
      env->DeleteLocalRef(heldStack);
    } else if (env->ExceptionCheck())
      env->ExceptionClear();

    if (autosoupDropBowls && g_mDropOneItem) {
      Sleep(20);
      env->CallObjectMethod(player, g_mDropOneItem, (jboolean)JNI_FALSE);
      if (env->ExceptionCheck())
        env->ExceptionClear();
    }

    Sleep(autosoupSwitchDelay);
    env->SetIntField(inventory, g_fCurrentItemIdx, originalSlot);
    if (env->ExceptionCheck())
      env->ExceptionClear();

    env->DeleteLocalRef(inventory);
    env->DeleteLocalRef(player);
    env->DeleteLocalRef(world);
    env->DeleteLocalRef(pctrl);
    env->DeleteLocalRef(mc);

    g_autosoupLastEat = (DWORD)GetTickCount64();
    Sleep(10);
  }

  autosoupStatus = "Stopped";
  g_jvm->DetachCurrentThread();
}

void autosoupThread() {
  __try {
    autosoupThreadInner();
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    autosoupStatus = "Thread crashed";
  }
}

static void refillThreadInner() {

    while (!g_jniReady && !g_isUnloading)
        Sleep(500);
    if (g_isUnloading)
        return;

    int waitAttempts = 0;
    while ((!g_fInventory || !g_fCurrentItemIdx || !g_mGetStackInSlot) &&
        !g_isUnloading && waitAttempts < 60) {
        Sleep(500);
        waitAttempts++;
    }

    if (!g_fInventory || !g_fCurrentItemIdx || !g_mGetStackInSlot) {
        refillStatus = "JNI fields missing";
        return;
    }
    if (!g_jvm) {
        refillStatus = "No JVM";
        return;
    }

    JNIEnv* env = nullptr;
    jint res = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_8);
    if (res == JNI_EDETACHED)
        g_jvm->AttachCurrentThreadAsDaemon((void**)&env, nullptr);
    if (!env) {
        refillStatus = "Attach failed";
        return;
    }

    jmethodID mGetIdFromItem = nullptr;
    if (g_itemClass) {
        const char* gifNames[] = { "func_150891_b", "getIdFromItem" };
        mGetIdFromItem = TryStaticMethodN(env, g_itemClass, gifNames, 2,
            "(Lnet/minecraft/item/Item;)I");
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    jmethodID mGetItemDamage = nullptr;
    {
        const char* isNames[] = { "net.minecraft.item.ItemStack" };
        jclass isClass = LoadMCClass(env, isNames, 1);
        if (isClass) {
            const char* dmgNames[] = { "func_77960_j", "getItemDamage" };
            mGetItemDamage = TryMethodN(env, isClass, dmgNames, 2, "()I");
            if (env->ExceptionCheck()) env->ExceptionClear();
            env->DeleteLocalRef(isClass);
        }
    }

    jmethodID mWindowClick = nullptr;
    if (g_pcmpClass) {

        const char* wcNames[] = { "func_78753_a", "windowClick" };

        mWindowClick = TryMethodN(env, g_pcmpClass, wcNames, 2,
            "(IIIILnet/minecraft/entity/player/EntityPlayer;)"
            "Lnet/minecraft/item/ItemStack;");
        if (!mWindowClick) {
            if (env->ExceptionCheck()) env->ExceptionClear();

            mWindowClick = TryMethodN(env, g_pcmpClass, wcNames, 2,
                "(IIIILnet/minecraft/entity/player/EntityPlayer;)Ljava/lang/Object;");
            if (env->ExceptionCheck()) env->ExceptionClear();
        }
    }

    jclass guiContainerClass = nullptr;
    {
        const char* gcNames[] = { "net.minecraft.client.gui.inventory.GuiContainer" };
        jclass local = LoadMCClass(env, gcNames, 1);
        if (local) {
            guiContainerClass = (jclass)env->NewGlobalRef(local);
            env->DeleteLocalRef(local);
        }
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    jclass guiScreenClass = nullptr;
    {
        const char* gsNames[] = { "net.minecraft.client.gui.GuiScreen" };
        jclass local = LoadMCClass(env, gsNames, 1);
        if (local) {
            guiScreenClass = (jclass)env->NewGlobalRef(local);
            env->DeleteLocalRef(local);
        }
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    jfieldID fCurrentScreen = nullptr;
    if (g_mcClass && guiScreenClass) {

        const char* csNames[] = { "field_71462_r", "currentScreen" };

        fCurrentScreen = TryFieldN(env, g_mcClass, csNames, 2,
            "Lnet/minecraft/client/gui/GuiScreen;");
        if (!fCurrentScreen) {
            if (env->ExceptionCheck()) env->ExceptionClear();

            jclass classRef = env->FindClass("java/lang/Class");
            jmethodID getDeclaredFields = env->GetMethodID(classRef,
                "getDeclaredFields", "()[Ljava/lang/reflect/Field;");
            jobjectArray fields = (jobjectArray)env->CallObjectMethod(g_mcClass, getDeclaredFields);
            env->DeleteLocalRef(classRef);
            if (fields) {
                jclass fieldClass = env->FindClass("java/lang/reflect/Field");
                jmethodID getType = env->GetMethodID(fieldClass, "getType", "()Ljava/lang/Class;");
                jmethodID fGetName = env->GetMethodID(fieldClass, "getName", "()Ljava/lang/String;");
                jmethodID classGetName = env->GetMethodID(env->FindClass("java/lang/Class"),
                    "getName", "()Ljava/lang/String;");
                jint flen = env->GetArrayLength(fields);
                for (jint i = 0; i < flen && !fCurrentScreen; i++) {
                    jobject f = env->GetObjectArrayElement(fields, i);
                    jclass fType = (jclass)env->CallObjectMethod(f, getType);
                    if (env->IsAssignableFrom(fType, guiScreenClass) ||
                        env->IsSameObject(fType, guiScreenClass)) {
                        jstring fName = (jstring)env->CallObjectMethod(f, fGetName);
                        const char* fn = env->GetStringUTFChars(fName, nullptr);
                        jstring typeName = (jstring)env->CallObjectMethod(fType, classGetName);
                        const char* tn = env->GetStringUTFChars(typeName, nullptr);
                        std::string sig = "L";
                        for (const char* p = tn; *p; p++) sig += (*p == '.') ? '/' : *p;
                        sig += ";";
                        fCurrentScreen = env->GetFieldID(g_mcClass, fn, sig.c_str());
                        if (!fCurrentScreen) env->ExceptionClear();
                        env->ReleaseStringUTFChars(typeName, tn);
                        env->DeleteLocalRef(typeName);
                        env->ReleaseStringUTFChars(fName, fn);
                        env->DeleteLocalRef(fName);
                    }
                    env->DeleteLocalRef(fType);
                    env->DeleteLocalRef(f);
                }
                env->DeleteLocalRef(fields);
                env->DeleteLocalRef(fieldClass);
            }
        }
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    jfieldID fInventorySlots = nullptr;
    jclass containerClass = nullptr;
    jmethodID mGetWindowId = nullptr;
    jmethodID mGetSlotCount = nullptr;
    jmethodID mGetSlotByIndex = nullptr;
    jfieldID fWindowId = nullptr;
    jfieldID fInventoryItemStacks = nullptr;

    {
        const char* contNames[] = { "net.minecraft.inventory.Container" };
        jclass local = LoadMCClass(env, contNames, 1);
        if (local) {
            containerClass = (jclass)env->NewGlobalRef(local);
            env->DeleteLocalRef(local);
        }
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    if (guiContainerClass && containerClass) {

        const char* isNames[] = { "field_147002_h", "inventorySlots" };
        fInventorySlots = TryFieldN(env, guiContainerClass, isNames, 2,
            "Lnet/minecraft/inventory/Container;");
        if (!fInventorySlots) {
            if (env->ExceptionCheck()) env->ExceptionClear();
        }

        const char* widNames[] = { "field_75152_c", "windowId" };
        fWindowId = TryFieldN(env, containerClass, widNames, 2, "I");
        if (env->ExceptionCheck()) env->ExceptionClear();

        const char* gsNames2[] = { "func_75139_a", "getSlot" };
        mGetSlotByIndex = TryMethodN(env, containerClass, gsNames2, 2,
            "(I)Lnet/minecraft/inventory/Slot;");
        if (env->ExceptionCheck()) env->ExceptionClear();

    }

    jclass slotClass = nullptr;
    jmethodID mSlotGetStack = nullptr;
    {
        const char* slotNames[] = { "net.minecraft.inventory.Slot" };
        jclass local = LoadMCClass(env, slotNames, 1);
        if (local) {
            slotClass = (jclass)env->NewGlobalRef(local);
            const char* gsNames[] = { "func_75217_a", "getStack" };
            mSlotGetStack = TryMethodN(env, local, gsNames, 2,
                "()Lnet/minecraft/item/ItemStack;");
            if (env->ExceptionCheck()) env->ExceptionClear();

            env->DeleteLocalRef(local);
        }
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    jfieldID fSlotNumber = nullptr;
    if (slotClass) {
        const char* snNames[] = { "field_75222_d", "slotNumber" };
        fSlotNumber = TryFieldN(env, slotClass, snNames, 2, "I");
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    jclass guiInventoryClass = nullptr;
    {
        const char* giNames[] = { "net.minecraft.client.gui.inventory.GuiInventory" };
        jclass local = LoadMCClass(env, giNames, 1);
        if (local) {
            guiInventoryClass = (jclass)env->NewGlobalRef(local);
            env->DeleteLocalRef(local);
        }
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    static std::mt19937 rng(std::random_device{}());
    refillStatus = "Active";
    bool lastKeyState = false;

    while (!g_isUnloading) {
        if (!refillEnabled) {
            refillStatus = "Disabled";
            lastKeyState = false;
            Sleep(50);
            continue;
        }
        if (showMenu) {
            refillStatus = "Menu open";
            Sleep(50);
            continue;
        }

        bool keyDown = g_refillKeyHeld;
        bool justPressed = keyDown && !lastKeyState;
        lastKeyState = keyDown;

        if (!justPressed) {
            refillStatus = "Waiting for key";
            Sleep(20);
            continue;
        }

        if (!g_mcClass || !g_getMinecraft || !g_fThePlayer) {
            refillStatus = "JNI not ready";
            Sleep(100);
            continue;
        }

        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (!mc || env->ExceptionCheck()) {
            if (env->ExceptionCheck()) env->ExceptionClear();
            Sleep(50);
            continue;
        }

        jobject player = env->GetObjectField(mc, g_fThePlayer);
        if (!player) { env->DeleteLocalRef(mc); Sleep(50); continue; }

        jobject pctrl = env->GetObjectField(mc, g_fPlayerController);
        jobject world = env->GetObjectField(mc, g_fTheWorld);

        auto getScreen = [&]() -> jobject {
            if (!fCurrentScreen) return nullptr;
            jobject cs = env->GetObjectField(mc, fCurrentScreen);
            if (env->ExceptionCheck()) { env->ExceptionClear(); return nullptr; }
            return cs;
            };

        auto isInventoryScreen = [&](jobject cs) -> bool {
            if (!cs) return false;
            bool inv = guiInventoryClass && env->IsInstanceOf(cs, guiInventoryClass);
            bool cont = !inv && guiContainerClass && env->IsInstanceOf(cs, guiContainerClass);
            return inv || cont;
            };

        jobject currentScreen = getScreen();
        bool alreadyOpen = isInventoryScreen(currentScreen);
        if (currentScreen) env->DeleteLocalRef(currentScreen);

        bool needToCloseAfter = false;

        if (!alreadyOpen) {

            bool opened = false;
            if (g_mDisplayGuiScreen && guiInventoryClass) {
                jmethodID guiInvCtor = env->GetMethodID(guiInventoryClass, "<init>",
                    "(Lnet/minecraft/entity/player/EntityPlayer;)V");
                if (!env->ExceptionCheck() && guiInvCtor) {
                    jobject guiInv = env->NewObject(guiInventoryClass, guiInvCtor, player);
                    if (guiInv && !env->ExceptionCheck()) {
                        env->CallVoidMethod(mc, g_mDisplayGuiScreen, guiInv);
                        env->DeleteLocalRef(guiInv);
                        if (!env->ExceptionCheck()) {
                            opened = true;
                            needToCloseAfter = true;
                        }
                    }
                    if (env->ExceptionCheck()) env->ExceptionClear();
                }
                else {
                    if (env->ExceptionCheck()) env->ExceptionClear();
                }
            }

            if (!opened && g_targetWindow) {
                PostMessage(g_targetWindow, WM_KEYDOWN, 'E', 0);
                Sleep(20);
                PostMessage(g_targetWindow, WM_KEYUP, 'E', 0);
                needToCloseAfter = true;
            }
            Sleep(refillDelayAfterOpen > 0 ? refillDelayAfterOpen : 150);
        }

        currentScreen = getScreen();
        bool screenIsInventory = isInventoryScreen(currentScreen);

        if (!screenIsInventory) {
            refillStatus = "Failed to open inventory";
            if (currentScreen) env->DeleteLocalRef(currentScreen);
            currentScreen = nullptr;
            if (pctrl) env->DeleteLocalRef(pctrl);
            if (world) env->DeleteLocalRef(world);
            env->DeleteLocalRef(player);
            env->DeleteLocalRef(mc);
            Sleep(200);
            continue;
        }

        jobject inventory = env->GetObjectField(player, g_fInventory);
        if (!inventory || env->ExceptionCheck()) {
            if (env->ExceptionCheck()) env->ExceptionClear();
            if (currentScreen) env->DeleteLocalRef(currentScreen);
            if (pctrl) env->DeleteLocalRef(pctrl);
            if (world) env->DeleteLocalRef(world);
            env->DeleteLocalRef(player);
            env->DeleteLocalRef(mc);
            Sleep(50);
            continue;
        }

        int freeSlotCount = 0;
        for (int s = 0; s < 9; s++) {
            jobject stack = env->CallObjectMethod(inventory, g_mGetStackInSlot, s);
            if (env->ExceptionCheck()) { env->ExceptionClear(); freeSlotCount++; continue; }
            if (!stack) { freeSlotCount++; continue; }
            env->DeleteLocalRef(stack);
        }

        if (freeSlotCount == 0) {
            refillStatus = "No free hotbar slots";
            env->DeleteLocalRef(inventory);
            if (currentScreen) env->DeleteLocalRef(currentScreen);
            if (pctrl) env->DeleteLocalRef(pctrl);
            if (world) env->DeleteLocalRef(world);
            env->DeleteLocalRef(player);
            env->DeleteLocalRef(mc);
            if (needToCloseAfter && g_targetWindow) {
                PostMessage(g_targetWindow, WM_KEYDOWN, VK_ESCAPE, 0);
                Sleep(30);
                PostMessage(g_targetWindow, WM_KEYUP, VK_ESCAPE, 0);
            }
            Sleep(200);
            continue;
        }

        jobject container = nullptr;
        if (currentScreen && fInventorySlots && guiContainerClass &&
            env->IsInstanceOf(currentScreen, guiContainerClass)) {
            container = env->GetObjectField(currentScreen, fInventorySlots);
            if (env->ExceptionCheck()) env->ExceptionClear();
        }

        if (!container) {
            refillStatus = "No container";
            env->DeleteLocalRef(inventory);
            if (currentScreen) env->DeleteLocalRef(currentScreen);
            if (pctrl) env->DeleteLocalRef(pctrl);
            if (world) env->DeleteLocalRef(world);
            env->DeleteLocalRef(player);
            env->DeleteLocalRef(mc);
            if (needToCloseAfter && g_targetWindow) {
                PostMessage(g_targetWindow, WM_KEYDOWN, VK_ESCAPE, 0);
                Sleep(30);
                PostMessage(g_targetWindow, WM_KEYUP, VK_ESCAPE, 0);
            }
            Sleep(100);
            continue;
        }

        int windowId = 0;
        if (fWindowId) {
            windowId = env->GetIntField(container, fWindowId);
            if (env->ExceptionCheck()) { env->ExceptionClear(); windowId = 0; }
        }

        std::vector<int> refillableContainerSlots;

        for (int containerSlot = 9; containerSlot <= 35; containerSlot++) {
            jobject stack = nullptr;
            if (mGetSlotByIndex && slotClass && mSlotGetStack) {
                jobject slot = env->CallObjectMethod(container, mGetSlotByIndex, containerSlot);
                if (slot && !env->ExceptionCheck()) {
                    stack = env->CallObjectMethod(slot, mSlotGetStack);
                    env->DeleteLocalRef(slot);
                }
                else {
                    if (env->ExceptionCheck()) env->ExceptionClear();
                    continue;
                }
            }
            else {
                stack = env->CallObjectMethod(inventory, g_mGetStackInSlot, containerSlot);
            }

            if (!stack || env->ExceptionCheck()) {
                if (env->ExceptionCheck()) env->ExceptionClear();
                if (stack) env->DeleteLocalRef(stack);
                continue;
            }

            bool isRefillable = false;

            if (g_mStackGetItem && mGetIdFromItem && g_itemClass) {
                jobject item = env->CallObjectMethod(stack, g_mStackGetItem);
                if (item && !env->ExceptionCheck()) {
                    jint itemId = env->CallStaticIntMethod(g_itemClass, mGetIdFromItem, item);
                    if (env->ExceptionCheck()) { env->ExceptionClear(); itemId = -1; }

                    if (refillSoup && itemId == 282) {
                        isRefillable = true;
                    }

                    if (refillPotion && !isRefillable && itemId == 373 && mGetItemDamage) {
                        jint damage = env->CallIntMethod(stack, mGetItemDamage);
                        if (env->ExceptionCheck()) { env->ExceptionClear(); damage = 0; }
                        bool isSplash = (damage & 0x4000) != 0;
                        if (isSplash) {
                            if (refillUseNonHealthPotions) {
                                isRefillable = true;
                            }
                            else {
                                int dmgByte = damage & 0xFF;
                                bool isNegative = ((dmgByte & 0x0F) == 0x02) ||
                                    ((dmgByte & 0x0F) == 0x0C) ||
                                    ((dmgByte & 0x0F) == 0x08);
                                if (!isNegative) isRefillable = true;
                            }
                        }
                    }
                    env->DeleteLocalRef(item);
                }
                else {
                    if (env->ExceptionCheck()) env->ExceptionClear();
                }
            }

            env->DeleteLocalRef(stack);
            if (isRefillable) refillableContainerSlots.push_back(containerSlot);
        }

        if (refillableContainerSlots.empty()) {
            refillStatus = "No refillable items";
            env->DeleteLocalRef(container);
            env->DeleteLocalRef(inventory);
            if (currentScreen) env->DeleteLocalRef(currentScreen);
            if (pctrl) env->DeleteLocalRef(pctrl);
            if (world) env->DeleteLocalRef(world);
            env->DeleteLocalRef(player);
            env->DeleteLocalRef(mc);
            if (needToCloseAfter && player && g_mCloseScreen) {
                Sleep(50);
                env->CallVoidMethod(player, g_mCloseScreen);
                if (env->ExceptionCheck()) env->ExceptionClear();
            }
            if (refillDisableOnComplete) refillEnabled = false;
            Sleep(200);
            continue;
        }

        if (refillRandomSlots)
            std::shuffle(refillableContainerSlots.begin(), refillableContainerSlots.end(), rng);

        refillStatus = "Refilling...";

        auto checkWindowOpen = [&]() -> bool {
            jobject cs = getScreen();
            if (!cs) return false;
            bool open = isInventoryScreen(cs);
            env->DeleteLocalRef(cs);
            return open;
            };

        int itemsRefilled = 0;
        int prevSlot = -1;

        for (int containerSlot : refillableContainerSlots) {
            if (itemsRefilled >= freeSlotCount) break;
            if (!checkWindowOpen()) break;
            if (!mWindowClick || !pctrl) break;

            int mouseButton = 0;
            int clickMode = 1;

            if (refillMouseButton == 1) {
                mouseButton = 1;
            }
            else if (refillMouseButton == 2) {
                mouseButton = 0;
            }

            int baseDelay = refillSpeed;
            if (refillSmartSpeed && prevSlot >= 0) {
                int slotDist = abs(containerSlot - prevSlot);
                if (slotDist > 10) baseDelay += (slotDist * 3);
            }

            env->CallObjectMethod(pctrl, mWindowClick,
                (jint)windowId, (jint)containerSlot,
                (jint)mouseButton, (jint)clickMode, player);
            if (env->ExceptionCheck()) { env->ExceptionClear(); break; }

            if (refillMouseButton == 2) {
                std::uniform_int_distribution<int> quickJitter(5, 15);
                Sleep(quickJitter(rng));
                env->CallObjectMethod(pctrl, mWindowClick,
                    (jint)windowId, (jint)containerSlot,
                    (jint)1, (jint)1, player);
                if (env->ExceptionCheck()) env->ExceptionClear();
            }

            prevSlot = containerSlot;
            itemsRefilled++;

            std::uniform_int_distribution<int> jitter(0, 10);
            Sleep(baseDelay + jitter(rng));
        }

        if (refillDelayBeforeClose > 0 && checkWindowOpen()) {
            std::uniform_int_distribution<int> jitter(0, 20);
            Sleep(refillDelayBeforeClose + jitter(rng));
        }

        bool shouldClose = refillCloseOnComplete || needToCloseAfter;
        if (shouldClose && checkWindowOpen()) {
            Sleep(refillDelayBeforeClose > 0 ? refillDelayBeforeClose : 50);
            if (checkWindowOpen()) {
                if (g_mCloseScreen && player) {
                    env->CallVoidMethod(player, g_mCloseScreen);
                    if (env->ExceptionCheck()) env->ExceptionClear();
                }
                else if (g_targetWindow) {
                    PostMessage(g_targetWindow, WM_KEYDOWN, VK_ESCAPE, 0);
                    Sleep(40);
                    PostMessage(g_targetWindow, WM_KEYUP, VK_ESCAPE, 0);
                }
            }
        }

        env->DeleteLocalRef(container);
        env->DeleteLocalRef(inventory);
        if (currentScreen) env->DeleteLocalRef(currentScreen);
        if (pctrl) env->DeleteLocalRef(pctrl);
        if (world) env->DeleteLocalRef(world);
        env->DeleteLocalRef(player);
        env->DeleteLocalRef(mc);

        refillStatus = "Done";
        if (refillDisableOnComplete) refillEnabled = false;

        Sleep(500);
    }

    if (guiContainerClass) env->DeleteGlobalRef(guiContainerClass);
    if (guiScreenClass)    env->DeleteGlobalRef(guiScreenClass);
    if (containerClass)    env->DeleteGlobalRef(containerClass);
    if (slotClass)         env->DeleteGlobalRef(slotClass);
    if (guiInventoryClass) env->DeleteGlobalRef(guiInventoryClass);

    refillStatus = "Stopped";
    if (g_jvm) g_jvm->DetachCurrentThread();
}

void refillThread() {
    __try {
        refillThreadInner();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        refillStatus = "Thread crashed (SEH)";
    }
}

std::string VirtualKeyToString(int vk) {
  if (vk == 0)
    return "Not Set";

  switch (vk) {
  case VK_NUMPAD0:
    return "Numpad 0";
  case VK_NUMPAD1:
    return "Numpad 1";
  case VK_NUMPAD2:
    return "Numpad 2";
  case VK_NUMPAD3:
    return "Numpad 3";
  case VK_NUMPAD4:
    return "Numpad 4";
  case VK_NUMPAD5:
    return "Numpad 5";
  case VK_NUMPAD6:
    return "Numpad 6";
  case VK_NUMPAD7:
    return "Numpad 7";
  case VK_NUMPAD8:
    return "Numpad 8";
  case VK_NUMPAD9:
    return "Numpad 9";
  case VK_F1:
    return "F1";
  case VK_F2:
    return "F2";
  case VK_F3:
    return "F3";
  case VK_F4:
    return "F4";
  case VK_F5:
    return "F5";
  case VK_F6:
    return "F6";
  case VK_F7:
    return "F7";
  case VK_F8:
    return "F8";
  case VK_F9:
    return "F9";
  case VK_F10:
    return "F10";
  case VK_F11:
    return "F11";
  case VK_F12:
    return "F12";
  case VK_SPACE:
    return "Space";
  case VK_RETURN:
    return "Enter";
  case VK_ESCAPE:
    return "Escape";
  case VK_TAB:
    return "Tab";
  case VK_LSHIFT:
    return "Left Shift";
  case VK_RSHIFT:
    return "Right Shift";
  case VK_LCONTROL:
    return "Left Ctrl";
  case VK_RCONTROL:
    return "Right Ctrl";
  case VK_LMENU:
    return "Left Alt";
  case VK_RMENU:
    return "Right Alt";
  default:
    if (vk >= 0x41 && vk <= 0x5A) {
      return std::string(1, (char)vk);
    }
    return "Key " + std::to_string(vk);
  }
}

void DrawHud() {

}

LRESULT __stdcall keyHandler(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode < 0 || g_isUnloading) {
    return CallNextHookEx(keyHookHandle, nCode, wParam, lParam);
  }

  if (g_isCapturingKey && g_capturingIndex >= 0 &&
      g_capturingIndex < static_cast<int>(g_keybinds.size())) {
    if (!(lParam & 0x80000000)) {
      if (wParam != VK_ESCAPE && wParam != VK_RETURN && wParam != VK_INSERT) {
        g_keybinds[g_capturingIndex].virtualKey = (int)wParam;
        g_keybinds[g_capturingIndex].isWaitingForKey = false;
        g_isCapturingKey = false;
        g_capturingIndex = -1;
        return CallNextHookEx(keyHookHandle, nCode, wParam, lParam);
      } else if (wParam == VK_ESCAPE) {
        g_keybinds[g_capturingIndex].isWaitingForKey = false;
        g_isCapturingKey = false;
        g_capturingIndex = -1;
        return CallNextHookEx(keyHookHandle, nCode, wParam, lParam);
      }
    }
  }

  if (g_capturingClickerKey && !(lParam & 0x80000000)) {
    if (wParam != VK_ESCAPE && wParam != VK_RETURN && wParam != VK_INSERT) {
      clickerActivationKey = (int)wParam;
      g_capturingClickerKey = false;
      return CallNextHookEx(keyHookHandle, nCode, wParam, lParam);
    } else if (wParam == VK_ESCAPE) {
      g_capturingClickerKey = false;
      return CallNextHookEx(keyHookHandle, nCode, wParam, lParam);
    }
  }

  if (clickerActivationKey != 0 && wParam == (WPARAM)clickerActivationKey) {
    clickerActivationKeyHeld = !(lParam & 0x80000000);
  }

  if (!g_cursorVisible) {
    if (wParam == (WPARAM)throwpotTriggerKey)
      g_throwpotKeyHeld = !(lParam & 0x80000000);
    if (wParam == (WPARAM)autosoupTriggerKey)
      g_autosoupKeyHeld = !(lParam & 0x80000000);
    if (wParam == (WPARAM)refillTriggerKey)
      g_refillKeyHeld = !(lParam & 0x80000000);
  } else {

    g_throwpotKeyHeld = false;
    g_autosoupKeyHeld = false;
    g_refillKeyHeld = false;
  }

  if (!(lParam & 0x80000000)) {
    if (wParam == 'E')
      g_lastEKeyTime = (DWORD)GetTickCount64();
    if (wParam == VK_ESCAPE)
      g_lastEscKeyTime = (DWORD)GetTickCount64();
  }

  if ((lParam & 0x80000000) && !g_cursorVisible) {
      for (auto& keybind : g_keybinds) {
          if (keybind.virtualKey != 0 && keybind.virtualKey == (int)wParam &&
              keybind.targetVariable) {

              if (keybind.targetVariable == &refillEnabled && (int)wParam == refillTriggerKey)   continue;
              if (keybind.targetVariable == &throwpotEnabled && (int)wParam == throwpotTriggerKey) continue;
              if (keybind.targetVariable == &autosoupEnabled && (int)wParam == autosoupTriggerKey) continue;

              aimLockOnActive = false;
              aimLockLostFrames = 0;
              extern jobject g_aimLockedEntity;
              if (g_aimLockedEntity) {
                  if (g_jvm) {
                      JNIEnv* e = nullptr;
                      if (g_jvm->GetEnv((void**)&e, JNI_VERSION_1_8) == JNI_OK && e)
                          e->DeleteGlobalRef(g_aimLockedEntity);
                  }
                  g_aimLockedEntity = nullptr;
              }
              *(keybind.targetVariable) = !*(keybind.targetVariable);
              break;
          }
      }
  }

  if (aimbotKeyMode && wParam == static_cast<WPARAM>(aimbotKey)) {
    aimbotKeyPressed = !(lParam & 0x80000000);
  }

  return CallNextHookEx(keyHookHandle, nCode, wParam, lParam);
}

void ApplyModernTheme() {
  ImGuiStyle &style = ImGui::GetStyle();

  style.WindowPadding = ImVec2(0, 0);
  style.WindowRounding = UILayout::WINDOW_ROUNDING;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(UILayout::WINDOW_W, UILayout::WINDOW_H);

  style.ChildRounding = UILayout::CARD_ROUNDING;
  style.ChildBorderSize = 0.0f;

  style.PopupRounding = UILayout::CARD_ROUNDING;
  style.PopupBorderSize = 1.0f;

  style.FramePadding = ImVec2(8, 4);
  style.FrameRounding = UILayout::FRAME_ROUNDING;
  style.FrameBorderSize = 0.0f;

  style.ItemSpacing = ImVec2(UILayout::ITEM_SPACING, UILayout::ITEM_SPACING);
  style.ItemInnerSpacing = ImVec2(6, 4);

  style.IndentSpacing = 14.0f;
  style.ScrollbarSize = 4.0f;
  style.ScrollbarRounding = 4.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = UILayout::GRAB_ROUNDING;
  style.TabRounding = UILayout::CARD_ROUNDING;
  style.TabBorderSize = 0.0f;

  ImVec4 *colors = style.Colors;

  colors[ImGuiCol_WindowBg] = UITheme::BG_BASE;
  colors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
  colors[ImGuiCol_PopupBg] = UITheme::BG_SIDEBAR;

  colors[ImGuiCol_Border] = UITheme::BG_CARD_BORDER;
  colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

  colors[ImGuiCol_Text] = UITheme::TEXT_PRIMARY;
  colors[ImGuiCol_TextDisabled] = UITheme::TEXT_DISABLED;
  colors[ImGuiCol_TextSelectedBg] = UITheme::ACCENT_DIM;

  colors[ImGuiCol_FrameBg] = UITheme::BUTTON_NORMAL;
  colors[ImGuiCol_FrameBgHovered] = UITheme::BUTTON_HOVER;
  colors[ImGuiCol_FrameBgActive] = UITheme::BUTTON_ACTIVE;

  colors[ImGuiCol_TitleBg] = UITheme::BG_SIDEBAR;
  colors[ImGuiCol_TitleBgActive] = UITheme::BG_SIDEBAR;
  colors[ImGuiCol_TitleBgCollapsed] = UITheme::BG_SIDEBAR;

  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 0.25f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.55f, 0.50f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.75f, 0.75f, 0.80f, 0.70f);

  colors[ImGuiCol_CheckMark] = UITheme::ACCENT;
  colors[ImGuiCol_SliderGrab] = UITheme::ACCENT;
  colors[ImGuiCol_SliderGrabActive] = UITheme::ACCENT_HOVER;

  colors[ImGuiCol_Button] = UITheme::BUTTON_NORMAL;
  colors[ImGuiCol_ButtonHovered] = UITheme::BUTTON_HOVER;
  colors[ImGuiCol_ButtonActive] = UITheme::BUTTON_ACTIVE;

  colors[ImGuiCol_Header] = UITheme::ITEM_SELECTED;
  colors[ImGuiCol_HeaderHovered] = UITheme::ITEM_HOVER;
  colors[ImGuiCol_HeaderActive] = UITheme::ACCENT_DIM;

  colors[ImGuiCol_Separator] = UITheme::SEPARATOR;
  colors[ImGuiCol_SeparatorHovered] = UITheme::ACCENT;
  colors[ImGuiCol_SeparatorActive] = UITheme::ACCENT_HOVER;

  colors[ImGuiCol_ResizeGrip] = UITheme::ACCENT_DIM;
  colors[ImGuiCol_ResizeGripHovered] = UITheme::ACCENT;
  colors[ImGuiCol_ResizeGripActive] = UITheme::ACCENT_HOVER;

  colors[ImGuiCol_Tab] = UITheme::BUTTON_NORMAL;
  colors[ImGuiCol_TabHovered] = UITheme::BUTTON_HOVER;
  colors[ImGuiCol_TabActive] = UITheme::ITEM_SELECTED;
  colors[ImGuiCol_TabUnfocused] = UITheme::BUTTON_NORMAL;
  colors[ImGuiCol_TabUnfocusedActive] = UITheme::ITEM_SELECTED;
}

bool SidebarItem(const char *label, bool selected) {
  ImGui::PushID(label);
  float availW = ImGui::GetContentRegionAvail().x;
  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImDrawList *dl = ImGui::GetWindowDrawList();

  ImGuiID id = ImGui::GetID("##si");
  float anim = AnimLerp(id, selected ? 1.0f : 0.0f, 0.14f);

  ImGui::InvisibleButton("##si", ImVec2(availW, UILayout::SIDEBAR_ITEM_H));
  bool hovered = ImGui::IsItemHovered();
  bool clicked = ImGui::IsItemClicked();

  ImVec2 rEnd(pos.x + availW, pos.y + UILayout::SIDEBAR_ITEM_H);

  if (anim > 0.02f) {
    ImVec4 selBg =
        UITheme::Lerp(ImVec4(0, 0, 0, 0), UITheme::ITEM_SELECTED, anim);
    dl->AddRectFilled(pos, rEnd, ImGui::ColorConvertFloat4ToU32(selBg), 6.0f);
    dl->AddRect(
        pos, rEnd,
        UITheme::ColorWithAlpha(UITheme::ITEM_SELECTED_BORDER, 0.5f * anim),
        6.0f, 0, 1.0f);
  } else if (hovered) {
    dl->AddRectFilled(
        pos, rEnd, ImGui::ColorConvertFloat4ToU32(UITheme::ITEM_HOVER), 6.0f);
  }

  ImVec4 textCol =
      UITheme::Lerp(UITheme::TEXT_SECONDARY, UITheme::TEXT_PRIMARY, anim);
  if (hovered && anim < 0.5f)
    textCol = UITheme::TEXT_PRIMARY;

  float textY =
      pos.y + (UILayout::SIDEBAR_ITEM_H - ImGui::GetTextLineHeight()) * 0.5f;
  dl->AddText(ImVec2(pos.x + 10, textY),
              ImGui::ColorConvertFloat4ToU32(textCol), label);

  if (anim > 0.3f) {
    float dotY = pos.y + UILayout::SIDEBAR_ITEM_H * 0.5f;
    dl->AddCircleFilled(ImVec2(pos.x + 4, dotY), 2.0f,
                        UITheme::ColorWithAlpha(UITheme::ACCENT, anim), 8);
  }

  ImGui::PopID();
  return clicked;
}

bool CustomCheckbox(const char *label, bool *value) {
  ImGui::PushID(label);
  float availW = ImGui::GetContentRegionAvail().x;
  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImDrawList *dl = ImGui::GetWindowDrawList();

  float sz = UILayout::CHECKBOX_SIZE;
  float rowH = (std::max)(sz + 2.0f, ImGui::GetTextLineHeight() + 6.0f);

  ImGui::InvisibleButton("##row", ImVec2(availW, rowH));
  bool hovered = ImGui::IsItemHovered();
  bool clicked = ImGui::IsItemClicked();
  if (clicked)
    *value = !*value;

  const char *labelEnd = strstr(label, "##");
  float textY = pos.y + (rowH - ImGui::GetTextLineHeight()) * 0.5f;
  dl->AddText(ImVec2(pos.x, textY),
              ImGui::ColorConvertFloat4ToU32(UITheme::TEXT_PRIMARY), label,
              labelEnd);

  float rightPad = 8.0f;
  float cbX = pos.x + availW - sz - rightPad;
  float cbY = pos.y + (rowH - sz) * 0.5f;
  ImVec2 cbMin(cbX, cbY);
  ImVec2 cbMax(cbX + sz, cbY + sz);

  ImGuiID id = ImGui::GetID("##cb");
  float anim = AnimLerp(id, *value ? 1.0f : 0.0f, 0.10f);

  ImVec4 boxBg(0.06f, 0.06f, 0.08f, 0.75f);
  if (hovered) {
    boxBg.x += 0.03f;
    boxBg.y += 0.03f;
    boxBg.z += 0.03f;
  }
  dl->AddRectFilled(cbMin, cbMax, ImGui::ColorConvertFloat4ToU32(boxBg), 4.0f);

  if (anim > 0.01f) {
    float inset = sz * (1.0f - anim) * 0.3f;
    ImVec2 glowMin(cbX + inset, cbY + inset);
    ImVec2 glowMax(cbX + sz - inset, cbY + sz - inset);

    ImVec4 glowCol = UITheme::ACCENT;
    glowCol.w = anim * 0.85f;
    dl->AddRectFilled(glowMin, glowMax, ImGui::ColorConvertFloat4ToU32(glowCol),
                      3.0f);

    float glowA = anim * 0.20f;
    for (float g = 3.0f; g > 0.0f; g -= 1.0f) {
      float a = glowA * (1.0f - g / 3.0f);
      dl->AddRect(ImVec2(cbMin.x - g, cbMin.y - g),
                  ImVec2(cbMax.x + g, cbMax.y + g),
                  UITheme::ColorWithAlpha(UITheme::ACCENT, a), 4.0f + g * 0.5f,
                  0, 1.0f);
    }
  }

  ImVec4 borderCol = UITheme::Lerp(
      ImVec4(0.14f, 0.14f, 0.18f, 0.55f),
      ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.70f),
      anim);
  dl->AddRect(cbMin, cbMax, ImGui::ColorConvertFloat4ToU32(borderCol), 4.0f, 0,
              1.0f);

  ImGui::PopID();
  return clicked;
}

bool BeginAnimatedSection(const char *id, bool visible) {
  float &anim = g_subSettingsAnim[id];
  float target = visible ? 1.0f : 0.0f;
  float dt = ImGui::GetIO().DeltaTime;
  anim += (target - anim) * (1.0f - expf(-12.0f * dt));
  if (fabsf(anim - target) < 0.005f)
    anim = target;
  if (anim < 0.01f)
    return false;

  ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * anim);
  float offsetY = (1.0f - anim) * -8.0f;
  ImVec2 cp = ImGui::GetCursorPos();
  ImGui::SetCursorPos(ImVec2(cp.x, cp.y + offsetY));
  return true;
}

void EndAnimatedSection() {
  ImGui::PopStyleVar();
}

bool CustomSlider(const char *label, float *value, float min, float max,
                  const char *format = "%.1f") {
  ImGui::PushID(label);
  float availW = ImGui::GetContentRegionAvail().x;
  float rightPad = 8.0f;
  float trackW = availW - rightPad;

  char valBuf[64];
  snprintf(valBuf, sizeof(valBuf), format, *value);
  const char *labelHashF = strstr(label, "##");
  ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
  if (labelHashF)
    ImGui::TextUnformatted(label, labelHashF);
  else
    ImGui::Text("%s", label);
  ImGui::PopStyleColor();
  ImGui::SameLine(trackW - ImGui::CalcTextSize(valBuf).x);
  ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_PRIMARY);
  ImGui::Text("%s", valBuf);
  ImGui::PopStyleColor();

  float trackH = 6.0f;
  float grabR = 6.0f;
  float rowH = grabR * 2 + 4.0f;
  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImDrawList *dl = ImGui::GetWindowDrawList();

  ImGui::InvisibleButton("##track", ImVec2(trackW, rowH));
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  bool changed = false;
  if (active) {
    float mx = ImGui::GetIO().MousePos.x;
    float newFrac = (mx - pos.x) / trackW;
    newFrac = newFrac < 0.0f ? 0.0f : (newFrac > 1.0f ? 1.0f : newFrac);
    float newVal = min + (max - min) * newFrac;
    if (newVal != *value) {
      *value = newVal;
      changed = true;
    }
  }

  float frac = (max > min) ? ((*value - min) / (max - min)) : 0.0f;
  float trackY = pos.y + (rowH - trackH) * 0.5f;
  float fillX = pos.x + trackW * frac;

  ImVec2 tMin(pos.x, trackY);
  ImVec2 tMax(pos.x + trackW, trackY + trackH);
  dl->AddRectFilled(tMin, tMax,
                    ImGui::ColorConvertFloat4ToU32(UITheme::SLIDER_BG),
                    trackH * 0.5f);

  if (frac > 0.005f) {
    dl->AddRectFilled(tMin, ImVec2(fillX, trackY + trackH),
                      ImGui::ColorConvertFloat4ToU32(UITheme::ACCENT),
                      trackH * 0.5f);

    dl->AddRectFilled(tMin, ImVec2(fillX, trackY + trackH),
                      UITheme::ColorWithAlpha(UITheme::ACCENT, 0.12f),
                      trackH * 0.5f);
  }

  float grabY = pos.y + rowH * 0.5f;
  ImGuiID gId = ImGui::GetID("##grab");
  float grabAnim = AnimLerp(gId, (hovered || active) ? 1.0f : 0.0f, 0.15f);
  float curGrabR = grabR + grabAnim * 2.0f;

  if (grabAnim > 0.05f) {
    for (float g = 4.0f; g > 0.0f; g -= 1.5f) {
      float a = grabAnim * 0.12f * (1.0f - g / 4.0f);
      dl->AddCircleFilled(ImVec2(fillX, grabY), curGrabR + g,
                          UITheme::ColorWithAlpha(UITheme::ACCENT, a), 16);
    }
  }
  dl->AddCircleFilled(ImVec2(fillX, grabY), curGrabR,
                      ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 0.95f)),
                      16);

  if (grabAnim > 0.2f) {
    dl->AddCircleFilled(
        ImVec2(fillX, grabY), curGrabR * 0.4f * grabAnim,
        UITheme::ColorWithAlpha(UITheme::ACCENT, 0.6f * grabAnim), 12);
  }

  ImGui::PopID();
  return changed;
}

bool CustomSliderInt(const char *label, int *value, int min, int max,
                     const char *format = "%d") {
  ImGui::PushID(label);
  float availW = ImGui::GetContentRegionAvail().x;
  float rightPad = 8.0f;
  float trackW = availW - rightPad;

  char valBuf[64];
  snprintf(valBuf, sizeof(valBuf), format, *value);
  const char *labelHashI = strstr(label, "##");
  ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
  if (labelHashI)
    ImGui::TextUnformatted(label, labelHashI);
  else
    ImGui::Text("%s", label);
  ImGui::PopStyleColor();
  ImGui::SameLine(trackW - ImGui::CalcTextSize(valBuf).x);
  ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_PRIMARY);
  ImGui::Text("%s", valBuf);
  ImGui::PopStyleColor();

  float trackH = 6.0f;
  float grabR = 6.0f;
  float rowH = grabR * 2 + 4.0f;
  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImDrawList *dl = ImGui::GetWindowDrawList();

  ImGui::InvisibleButton("##track", ImVec2(trackW, rowH));
  bool hovered = ImGui::IsItemHovered();
  bool active = ImGui::IsItemActive();

  bool changed = false;
  if (active) {
    float mx = ImGui::GetIO().MousePos.x;
    float newFrac = (mx - pos.x) / trackW;
    newFrac = newFrac < 0.0f ? 0.0f : (newFrac > 1.0f ? 1.0f : newFrac);
    int newVal = min + (int)((float)(max - min) * newFrac + 0.5f);
    if (newVal != *value) {
      *value = newVal;
      changed = true;
    }
  }

  float frac =
      (max > min) ? ((float)(*value - min) / (float)(max - min)) : 0.0f;
  float trackY = pos.y + (rowH - trackH) * 0.5f;
  float fillX = pos.x + trackW * frac;

  ImVec2 tMin(pos.x, trackY);
  ImVec2 tMax(pos.x + trackW, trackY + trackH);
  dl->AddRectFilled(tMin, tMax,
                    ImGui::ColorConvertFloat4ToU32(UITheme::SLIDER_BG),
                    trackH * 0.5f);

  if (frac > 0.005f) {
    dl->AddRectFilled(tMin, ImVec2(fillX, trackY + trackH),
                      ImGui::ColorConvertFloat4ToU32(UITheme::ACCENT),
                      trackH * 0.5f);
    dl->AddRectFilled(tMin, ImVec2(fillX, trackY + trackH),
                      UITheme::ColorWithAlpha(UITheme::ACCENT, 0.12f),
                      trackH * 0.5f);
  }

  float grabY = pos.y + rowH * 0.5f;
  ImGuiID gId = ImGui::GetID("##grab");
  float grabAnim = AnimLerp(gId, (hovered || active) ? 1.0f : 0.0f, 0.15f);
  float curGrabR = grabR + grabAnim * 2.0f;

  if (grabAnim > 0.05f) {
    for (float g = 4.0f; g > 0.0f; g -= 1.5f) {
      float a = grabAnim * 0.12f * (1.0f - g / 4.0f);
      dl->AddCircleFilled(ImVec2(fillX, grabY), curGrabR + g,
                          UITheme::ColorWithAlpha(UITheme::ACCENT, a), 16);
    }
  }
  dl->AddCircleFilled(ImVec2(fillX, grabY), curGrabR,
                      ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 0.95f)),
                      16);
  if (grabAnim > 0.2f) {
    dl->AddCircleFilled(
        ImVec2(fillX, grabY), curGrabR * 0.4f * grabAnim,
        UITheme::ColorWithAlpha(UITheme::ACCENT, 0.6f * grabAnim), 12);
  }

  ImGui::PopID();
  return changed;
}

bool ModernToggle(const char *label, bool *value,
                  const char *description = nullptr) {
  return CustomCheckbox(label, value);
}

void SectionHeader(const char *text) {
  ImGui::Dummy(ImVec2(0, 8));
  ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
  ImGui::Text("%s", text);
  ImGui::PopStyleColor();
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0, 4));

  ImGui::Dummy(ImVec2(0, UILayout::ITEM_SPACING + 2));
}

void LoadESPImage(const char* filePath) {
  if (espImageTexture) {
    glDeleteTextures(1, &espImageTexture);
    espImageTexture = 0;
  }

  espImageLoaded = false;

  if (!filePath || strlen(filePath) == 0) return;

  int channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(filePath, &espImageWidth, &espImageHeight, &channels, 4);

  if (!data) {

    return;
  }

  glGenTextures(1, &espImageTexture);
  if (!espImageTexture) {
    stbi_image_free(data);
    return;
  }

  glBindTexture(GL_TEXTURE_2D, espImageTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, espImageWidth, espImageHeight, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    glDeleteTextures(1, &espImageTexture);
    espImageTexture = 0;
    return;
  }

  espImageLoaded = true;
}

void OpenESPImageDialog() {
  OPENFILENAMEA ofn;
  char fileName[MAX_PATH] = "";

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = g_targetWindow;
  ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg;*.jfif;*.bmp;*.tga;*.webp\0All Files\0*.*\0";
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  if (GetOpenFileNameA(&ofn)) {
    strcpy_s(espImagePath, sizeof(espImagePath), fileName);
    LoadESPImage(espImagePath);
  }
}

void RenderModulePanel(const char *title, std::function<void()> renderFunc) {
  ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, UILayout::CARD_ROUNDING);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                      ImVec2(UILayout::PADDING, UILayout::PADDING));
  ImGui::PushStyleColor(ImGuiCol_ChildBg, UITheme::BG_CARD);
  ImGui::PushStyleColor(ImGuiCol_Border, UITheme::BG_CARD_BORDER);
  ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

  ImGui::BeginChild("##ModulePanel", ImVec2(0, 0), true);

  ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_PRIMARY);
  ImGui::Text("%s", title);
  ImGui::PopStyleColor();

  ImGui::Dummy(ImVec2(0, 3));
  ImVec2 sepPos = ImGui::GetCursorScreenPos();
  float sepW = ImGui::GetContentRegionAvail().x;
  ImDrawList *dl = ImGui::GetWindowDrawList();
  dl->AddLine(sepPos, ImVec2(sepPos.x + sepW, sepPos.y),
              ImGui::ColorConvertFloat4ToU32(UITheme::SEPARATOR), 1.0f);
  ImGui::Dummy(ImVec2(0, UILayout::ITEM_SPACING + 2));

  if (renderFunc)
    renderFunc();

  ImGui::EndChild();

  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar(3);
}

void RenderModuleKeybind(bool *targetVariable, const char *moduleName) {
  ImGui::Dummy(ImVec2(0, 4));
  SectionHeader("Keybind");

  int keybindIdx = -1;
  for (size_t i = 0; i < g_keybinds.size(); i++) {
    if (g_keybinds[i].targetVariable == targetVariable) {
      keybindIdx = (int)i;
      break;
    }
  }

  if (keybindIdx >= 0) {
    KeyBind &kb = g_keybinds[keybindIdx];
    std::string keyStr = VirtualKeyToString(kb.virtualKey);

    ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
    ImGui::Text("Toggle Key");
    ImGui::PopStyleColor();
    ImGui::SameLine();

    if (kb.isWaitingForKey) {
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
      ImGui::Text("Press any key...");
      ImGui::PopStyleColor();
    } else {
      ImGui::Text("[%s]", keyStr.c_str());
    }
    ImGui::SameLine();

    char btnId[64];
    snprintf(btnId, sizeof(btnId), "Set##kb_%s", moduleName);
    if (ImGui::Button(btnId)) {
      for (auto &k : g_keybinds)
        k.isWaitingForKey = false;
      g_isCapturingKey = false;
      kb.isWaitingForKey = true;
      g_isCapturingKey = true;
      g_capturingIndex = keybindIdx;
    }

    if (kb.virtualKey != 0) {
      ImGui::SameLine();
      char clearId[64];
      snprintf(clearId, sizeof(clearId), "Clear##kb_%s", moduleName);
      if (ImGui::Button(clearId)) {
        kb.virtualKey = 0;
      }
    }
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_DISABLED);
    ImGui::Text("No keybind slot available");
    ImGui::PopStyleColor();
  }
}

void RenderKeybindMenu() {
  ImGui::Text("Click 'Set' to change a key");
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0, 10));

  for (size_t i = 0; i < g_keybinds.size(); i++) {
    KeyBind &keybind = g_keybinds[i];

    ImGui::Text("%s:", keybind.displayName.c_str());
    ImGui::SameLine();

    if (keybind.isWaitingForKey) {
      ImGui::Text("Press any key... (ESC to cancel)");
    } else {
      std::string keyText = VirtualKeyToString(keybind.virtualKey);
      if (keybind.virtualKey == 0) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::Text("[%s]", keyText.c_str());
        ImGui::PopStyleColor();
      } else {
        ImGui::Text("[%s]", keyText.c_str());
      }
    }

    ImGui::SameLine();

    std::string buttonId = "Set##" + std::to_string(i);
    if (ImGui::Button(buttonId.c_str())) {
      for (auto &kb : g_keybinds) {
        kb.isWaitingForKey = false;
      }
      g_isCapturingKey = false;

      keybind.isWaitingForKey = true;
      g_isCapturingKey = true;
      g_capturingIndex = static_cast<int>(i);
    }

    if (keybind.virtualKey != 0) {
      ImGui::SameLine();
      std::string clearId = "Clear##" + std::to_string(i);
      if (ImGui::Button(clearId.c_str())) {
        keybind.virtualKey = 0;
      }
    }

    if (keybind.targetVariable) {
      ImGui::SameLine();
      ImGui::Text("(%s)", *(keybind.targetVariable) ? "ON" : "OFF");
    }
  }

  ImGui::Separator();
  ImGui::Dummy(ImVec2(0, 10));

  if (ImGui::Button("Reset All Keybinds")) {
    for (auto &keybind : g_keybinds) {
      keybind.virtualKey = 0;
      keybind.isWaitingForKey = false;
    }
    g_isCapturingKey = false;
    g_capturingIndex = -1;
  }

  ImGui::SameLine();

  if (ImGui::Button("Clear All")) {
    InitializeKeyBinds();
    g_isCapturingKey = false;
    g_capturingIndex = -1;
  }
}

static std::string GetModuleInfoString(const std::string &moduleName) {
  if (moduleName == "Left Clicker") {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d CPS", clickerCpsInt);
    return buf;
  }
  if (moduleName == "Right Clicker") {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d CPS", rightClickerCpsInt);
    return buf;
  }
  if (moduleName == "Aim Assist") {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", aimSpeedInt);
    return buf;
  }
  if (moduleName == "Reach") {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.1f", (reachMin + reachMax) * 0.5f);
    return buf;
  }
  if (moduleName == "Velocity") {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.0f%%", velocityHorizontal);
    return buf;
  }
  if (moduleName == "Blockhit") {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d%%", blockhitBlockChance);
    return buf;
  }
  if (moduleName == "Hit Select") {
    return (hitSelectMode == 0) ? "Burst" : "Criticals";
  }
  if (moduleName == "Sprint Reset") {
    const char *modes[] = {"WTap", "Sneak", "NoStop"};
    if (sprintResetMode >= 0 && sprintResetMode <= 2)
      return modes[sprintResetMode];
  }
  if (moduleName == "Inv Walk") {
    return (invWalkMode == 0) ? "Vanilla" : "Spoof";
  }
  return "";
}

static ImVec4 HSVtoRGB(float h, float s, float v) {
  float r, g, b;
  ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
  return ImVec4(r, g, b, 1.0f);
}

void RenderArrayList() {
  if (!arraylistEnabled || !g_imguiInitialized)
    return;

  UpdateEnabledModulesList();
  if (enabledModules.empty())
    return;

  ImGuiIO &io = ImGui::GetIO();
  ImDrawList *fg = ImGui::GetForegroundDrawList();

  float prevFS = ImGui::GetFont()->Scale;
  ImGui::GetFont()->Scale = arraylistFontScale;
  ImGui::PushFont(ImGui::GetFont());

  struct ALEntry {
    std::string name;
    std::string info;
    std::string full;
  };
  std::vector<ALEntry> entries;
  for (auto &mod : enabledModules) {
    ALEntry e;
    e.name = mod;
    if (arraylistShowInfo) {
      e.info = GetModuleInfoString(mod);
    }
    e.full = e.name;
    if (!e.info.empty()) {
      e.full += " | " + e.info;
    }
    entries.push_back(e);
  }

  std::sort(entries.begin(), entries.end(),
            [](const ALEntry &a, const ALEntry &b) {
              return ImGui::CalcTextSize(a.full.c_str()).x >
                     ImGui::CalcTextSize(b.full.c_str()).x;
            });

  float padding = 16.0f;
  float itemH = ImGui::GetTextLineHeight() + 16.0f;
  float margin = 6.0f;
  float screenW = io.DisplaySize.x;
  float screenH = io.DisplaySize.y;
  float barW = arraylistBarWidth;

  std::vector<float> itemWidths(entries.size());
  for (int i = 0; i < (int)entries.size(); i++) {
    ImVec2 fullSz = ImGui::CalcTextSize(entries[i].full.c_str());
    itemWidths[i] = fullSz.x + padding * 2 + barW + 4.0f;
  }

  for (int i = 0; i < (int)entries.size(); i++) {
    const ALEntry &e = entries[i];

    ImVec2 nameSz = ImGui::CalcTextSize(e.name.c_str());
    float boxW = itemWidths[i];

    float boxX, boxY;
    switch (arraylistPosition) {
    case 0:
      boxX = screenW - boxW - margin;
      boxY = margin + i * itemH;
      break;
    case 1:
      boxX = margin;
      boxY = margin + i * itemH;
      break;
    case 2:
      boxX = screenW - boxW - margin;
      boxY = screenH - margin - ((int)entries.size() - i) * itemH;
      break;
    case 3:
      boxX = margin;
      boxY = screenH - margin - ((int)entries.size() - i) * itemH;
      break;
    default:
      boxX = screenW - boxW - margin;
      boxY = margin + i * itemH;
      break;
    }

    ImVec2 bMin(boxX, boxY);
    ImVec2 bMax(boxX + boxW, boxY + itemH);

    float rounding = arraylistRoundedCorners ? 5.0f : 0.0f;
    int totalItems = (int)entries.size();
    bool isFirst = (i == 0);
    bool isLast = (i == totalItems - 1);
    float prevW = isFirst ? 0.0f : itemWidths[i - 1];
    float nextW = isLast ? 0.0f : itemWidths[i + 1];
    bool rightAnchored = (arraylistPosition == 0 || arraylistPosition == 2);

    bool roundTL = false, roundTR = false, roundBL = false, roundBR = false;
    if (arraylistRoundedCorners) {
      if (rightAnchored) {

        roundTR = isFirst;
        roundBR = isLast;

        roundTL = isFirst || (boxW > prevW + 0.5f);
        roundBL = isLast || (boxW > nextW + 0.5f);
      } else {

        roundTL = isFirst;
        roundBL = isLast;
        roundTR = isFirst || (boxW > prevW + 0.5f);
        roundBR = isLast || (boxW > nextW + 0.5f);
      }
    }

    ImDrawFlags cornerFlags = ImDrawFlags_RoundCornersNone;
    if (roundTL) cornerFlags |= ImDrawFlags_RoundCornersTopLeft;
    if (roundTR) cornerFlags |= ImDrawFlags_RoundCornersTopRight;
    if (roundBL) cornerFlags |= ImDrawFlags_RoundCornersBottomLeft;
    if (roundBR) cornerFlags |= ImDrawFlags_RoundCornersBottomRight;

    if (arraylistBackground) {
      fg->AddRectFilled(bMin, bMax,
                        ImGui::ColorConvertFloat4ToU32(
                            ImVec4(0.015f, 0.015f, 0.020f, arraylistBgAlpha)),
                        rounding, cornerFlags);
    }

    ImVec4 barColor = arraylistAccentColor;
    if (arraylistRainbow) {
      float hue =
          fmodf(g_globalTime * arraylistRainbowSpeed * 0.3f + i * 0.08f, 1.0f);
      barColor = HSVtoRGB(hue, 0.65f, 1.0f);
    }

    if (arraylistAccentBar && barW > 0.1f) {

      ImDrawFlags barFlags = ImDrawFlags_RoundCornersNone;
      if (arraylistRoundedCorners) {
        if (rightAnchored) {
          if (roundTR) barFlags |= ImDrawFlags_RoundCornersTopRight;
          if (roundBR) barFlags |= ImDrawFlags_RoundCornersBottomRight;
        } else {
          if (roundTL) barFlags |= ImDrawFlags_RoundCornersTopLeft;
          if (roundBL) barFlags |= ImDrawFlags_RoundCornersBottomLeft;
        }
      }
      if (rightAnchored) {
        fg->AddRectFilled(ImVec2(bMax.x - barW, bMin.y), bMax,
                          ImGui::ColorConvertFloat4ToU32(barColor),
                          rounding, barFlags);
      } else {
        fg->AddRectFilled(bMin, ImVec2(bMin.x + barW, bMax.y),
                          ImGui::ColorConvertFloat4ToU32(barColor),
                          rounding, barFlags);
      }
    }

    if (!isLast) {
      fg->AddLine(ImVec2(bMin.x, bMax.y), ImVec2(bMax.x, bMax.y),
                  ImGui::ColorConvertFloat4ToU32(
                      ImVec4(barColor.x, barColor.y, barColor.z, 0.06f)),
                  1.0f);
    }

    float textOffsetX =
        (arraylistPosition == 1 || arraylistPosition == 3) ? barW + 4.0f : 0.0f;
    float textY = boxY + (itemH - nameSz.y) * 0.5f;
    fg->AddText(ImVec2(boxX + padding + textOffsetX, textY),
                ImGui::ColorConvertFloat4ToU32(arraylistTextColor),
                e.name.c_str());

    if (!e.info.empty() && arraylistShowInfo) {
      std::string sep = " | ";
      float nameW = nameSz.x;
      float sepW = ImGui::CalcTextSize(sep.c_str()).x;

      fg->AddText(ImVec2(boxX + padding + textOffsetX + nameW, textY),
                  ImGui::ColorConvertFloat4ToU32(ImVec4(
                      arraylistInfoColor.x, arraylistInfoColor.y,
                      arraylistInfoColor.z, arraylistInfoColor.w * 0.5f)),
                  sep.c_str());

      fg->AddText(ImVec2(boxX + padding + textOffsetX + nameW + sepW, textY),
                  ImGui::ColorConvertFloat4ToU32(arraylistInfoColor),
                  e.info.c_str());
    }
  }

  ImGui::PopFont();
  ImGui::GetFont()->Scale = prevFS;
}

void RenderWatermark() {
  if (!arraylistWatermark || !g_imguiInitialized)
    return;

  ImGuiIO &io = ImGui::GetIO();
  ImDrawList *fg = ImGui::GetForegroundDrawList();

  float prevFS = ImGui::GetFont()->Scale;
  ImGui::GetFont()->Scale = arraylistWmFontScale;
  ImGui::PushFont(ImGui::GetFont());

  std::string wmText;
  if (arraylistWmShowName) {
    wmText += g_clientName;
  }
  if (arraylistWmShowVersion) {
    if (!wmText.empty()) wmText += " ";
    wmText += g_clientVersion;
  }
  if (arraylistWmShowFps) {
    if (!wmText.empty()) wmText += " ";
    int fps = (int)(io.Framerate + 0.5f);
    wmText += std::to_string(fps) + "FPS";
  }
  if (wmText.empty() && !arraylistWmShowKeybinds) {
    ImGui::PopFont();
    ImGui::GetFont()->Scale = prevFS;
    return;
  }

  float margin = 10.0f;
  float padX = 14.0f, padY = 6.0f;
  float rounding = 8.0f;
  bool onLeft = (arraylistWmPosition == 0);
  ImU32 bgCol = ImGui::ColorConvertFloat4ToU32(arraylistWmBgColor);
  ImU32 borderCol = ImGui::ColorConvertFloat4ToU32(arraylistWmBorderColor);
  ImU32 textCol = ImGui::ColorConvertFloat4ToU32(arraylistWmTextColor);
  ImU32 dimCol = ImGui::ColorConvertFloat4ToU32(ImVec4(
      arraylistWmTextColor.x, arraylistWmTextColor.y,
      arraylistWmTextColor.z, arraylistWmTextColor.w * 0.45f));

  float bottomY = margin;
  float anchorX = margin;

  if (!wmText.empty()) {
    ImVec2 textSz = ImGui::CalcTextSize(wmText.c_str());
    float boxW = textSz.x + padX * 2;
    float boxH = textSz.y + padY * 2;
    float boxX = onLeft ? margin : (io.DisplaySize.x - boxW - margin);
    float boxY = margin;

    ImVec2 bMin(boxX, boxY);
    ImVec2 bMax(boxX + boxW, boxY + boxH);

    for (int s = 3; s >= 1; s--) {
      float expand = (float)s * 2.0f;
      fg->AddRectFilled(
          ImVec2(bMin.x - expand, bMin.y - expand),
          ImVec2(bMax.x + expand, bMax.y + expand),
          ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.04f * s)),
          rounding + expand * 0.3f);
    }

    fg->AddRectFilled(bMin, bMax, bgCol, rounding);
    fg->AddRect(bMin, bMax, borderCol, rounding, 0, 1.0f);
    fg->AddText(ImVec2(boxX + padX, boxY + padY), textCol, wmText.c_str());

    anchorX = boxX;
    bottomY = bMax.y + 4.0f;
  }

  if (arraylistWmShowKeybinds && !g_keybinds.empty()) {
    struct KBEntry { std::string name; std::string key; bool active; };
    std::vector<KBEntry> entries;
    for (auto &kb : g_keybinds) {
      if (kb.virtualKey <= 0) continue;
      char buf[32] = {};
      UINT scanCode = MapVirtualKeyA(kb.virtualKey, MAPVK_VK_TO_VSC);
      std::string keyName;
      if (GetKeyNameTextA(scanCode << 16, buf, sizeof(buf)))
        keyName = buf;
      else
        keyName = "?";
      bool active = kb.targetVariable ? *kb.targetVariable : false;
      entries.push_back({kb.displayName.empty() ? kb.name : kb.displayName, keyName, active});
    }
    if (!entries.empty()) {
      float lineH = ImGui::CalcTextSize("A").y + 4.0f;
      float maxNameW = 0, maxKeyW = 0;
      for (auto &e : entries) {
        float nw = ImGui::CalcTextSize(e.name.c_str()).x;
        float kw = ImGui::CalcTextSize(e.key.c_str()).x;
        if (nw > maxNameW) maxNameW = nw;
        if (kw > maxKeyW) maxKeyW = kw;
      }
      float cardPadX = 12.0f, cardPadY = 8.0f;
      float gap = 20.0f;
      float cardW = cardPadX * 2 + maxNameW + gap + maxKeyW + 12.0f;
      float cardH = cardPadY * 2 + lineH * (float)entries.size();
      float cardX = anchorX;
      float cardY = bottomY;

      ImVec2 cMin(cardX, cardY);
      ImVec2 cMax(cardX + cardW, cardY + cardH);

      for (int s = 2; s >= 1; s--) {
        float expand = (float)s * 2.0f;
        fg->AddRectFilled(
            ImVec2(cMin.x - expand, cMin.y - expand),
            ImVec2(cMax.x + expand, cMax.y + expand),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.03f * s)),
            rounding + expand * 0.3f);
      }

      fg->AddRectFilled(cMin, cMax, bgCol, rounding);
      fg->AddRect(cMin, cMax, borderCol, rounding, 0, 1.0f);

      float yOff = cardY + cardPadY;
      for (auto &e : entries) {
        ImU32 nameCol = e.active ? textCol : dimCol;
        float dotR = 3.0f;
        ImU32 dotCol = e.active
            ? ImGui::ColorConvertFloat4ToU32(ImVec4(0.45f, 0.85f, 0.45f, 1.0f))
            : ImGui::ColorConvertFloat4ToU32(ImVec4(0.35f, 0.35f, 0.38f, 1.0f));
        fg->AddCircleFilled(
            ImVec2(cardX + cardPadX + dotR, yOff + lineH * 0.5f),
            dotR, dotCol);

        fg->AddText(ImVec2(cardX + cardPadX + dotR * 2 + 6.0f, yOff), nameCol, e.name.c_str());

        std::string keyStr = "[" + e.key + "]";
        float keyW = ImGui::CalcTextSize(keyStr.c_str()).x;
        fg->AddText(ImVec2(cardX + cardW - cardPadX - keyW, yOff), dimCol, keyStr.c_str());

        yOff += lineH;
      }
    }
  }

  ImGui::PopFont();
  ImGui::GetFont()->Scale = prevFS;
}

void RenderTargetHUD() {
    if (!targetHudEnabled || !g_imguiInitialized) {
        printf("[DEBUG] Not rendering: enabled=%s, imgui=%s\n",
               targetHudEnabled ? "true" : "false",
               g_imguiInitialized ? "true" : "false");
        return;
    }

    float now = (float)GetTickCount64() / 1000.0f;
    float timeSinceHit = now - g_targetHudLastAttackTime;

    if (timeSinceHit > g_hudDisplayTime) {
        printf("[DEBUG] Not rendering: time since hit=%.2fs, limit=%.1fs\n",
               timeSinceHit, g_hudDisplayTime);
        return;
    }

    printf("[DEBUG] Rendering Target HUD! Time since hit: %.2fs\n", timeSinceHit);
    printf("[DEBUG] Cached data: name='%s', hp=%.1f, maxHp=%.1f\n",
           g_cachedTargetName, g_cachedHp, g_cachedMaxHp);

    ImVec2 screen = ImGui::GetIO().DisplaySize;
    ImVec2 crosshair(screen.x * 0.5f, screen.y * 0.5f);

    ESPOverlayData* target = nullptr;
    float bestDist = FLT_MAX;

    for (auto& ov : g_espOverlaysReady) {
        if (!ov.valid) continue;
        float cx = (ov.boxLeft + ov.boxRight)  * 0.5f;
        float cy = (ov.boxTop  + ov.boxBottom) * 0.5f;
        float dx = cx - crosshair.x;
        float dy = cy - crosshair.y;
        float d  = dx * dx + dy * dy;
        if (d < bestDist) {
            bestDist = d;
            target = &ov;
        }
    }

    if (target && target->valid) {
        if (target->name[0] != '\0')
            strncpy_s(g_cachedTargetName, target->name, 31);
        g_cachedHp     = target->hp;
        g_cachedMaxHp  = target->maxHp;
        g_targetHpFrac = target->hpFrac;
    }

    float lerpSpeed = 0.12f;
    g_displayedHpFrac += (g_targetHpFrac - g_displayedHpFrac) * lerpSpeed;
    if (fabsf(g_displayedHpFrac - g_targetHpFrac) < 0.001f)
        g_displayedHpFrac = g_targetHpFrac;
    g_displayedHpFrac = max(0.0f, min(1.0f, g_displayedHpFrac));

    float scale        = targetHudScale;
    float headSize     = 36.0f * scale;
    float padding      = 9.0f  * scale;
    float barHeight    = 5.0f  * scale;
    float nameFontSize = 13.0f * scale;
    float subFontSize  = nameFontSize * 0.78f;
    float rounding     = 7.0f  * scale;
    float headGap      = 8.0f  * scale;

    ImFont* font = ImGui::GetFont();

    ImVec2 nameDim = font->CalcTextSizeA(nameFontSize, FLT_MAX, 0, g_cachedTargetName);
    float  textColW = max(nameDim.x, 72.0f * scale);

    float cardW = padding + headSize + headGap + textColW + padding;
    float cardH = padding + headSize + (padding * 0.5f) + barHeight + padding;

    float anchorX = screen.x * 0.04f + targetHudX;
    float anchorY = screen.y * 0.5f  + targetHudY - cardH * 0.5f;

    anchorX = max(4.0f, min(anchorX, screen.x - cardW - 4.0f));
    anchorY = max(4.0f, min(anchorY, screen.y - cardH - 4.0f));

    ImVec2 p0(anchorX,         anchorY);
    ImVec2 p1(anchorX + cardW, anchorY + cardH);

    ImDrawList* dl = ImGui::GetBackgroundDrawList();

    dl->AddRectFilled(p0, p1, IM_COL32(20, 20, 20, 210), rounding);

    dl->AddRect(p0, p1, IM_COL32(255, 255, 255, 28), rounding, 0, 1.0f);

    float headX = anchorX + padding;
    float headY = anchorY + padding;
    ImVec2 headMin(headX,            headY);
    ImVec2 headMax(headX + headSize, headY + headSize);

    if (target && target->skinTexture != 0) {

        ImVec2 uvMin(8.0f / 64.0f,  8.0f / 64.0f);
        ImVec2 uvMax(16.0f / 64.0f, 16.0f / 64.0f);
        dl->AddImageRounded((ImTextureID)(intptr_t)target->skinTexture,
                            headMin, headMax,
                            uvMin, uvMax,
                            IM_COL32(255, 255, 255, 255),
                            rounding * 0.5f);

        ImVec2 hatUVMin(40.0f / 64.0f, 8.0f / 64.0f);
        ImVec2 hatUVMax(48.0f / 64.0f, 16.0f / 64.0f);
        dl->AddImageRounded((ImTextureID)(intptr_t)target->skinTexture,
                            headMin, headMax,
                            hatUVMin, hatUVMax,
                            IM_COL32(255, 255, 255, 255),
                            rounding * 0.5f);
    } else {

        dl->AddRectFilled(headMin, headMax, IM_COL32(38, 38, 38, 255), rounding * 0.5f);
        dl->AddRect(headMin, headMax, IM_COL32(255, 255, 255, 35), rounding * 0.5f, 0, 1.0f);

        ImVec2 qSize = font->CalcTextSizeA(nameFontSize, FLT_MAX, 0, "?");
        dl->AddText(font, nameFontSize,
                    ImVec2(headX + (headSize - qSize.x) * 0.5f,
                           headY + (headSize - qSize.y) * 0.5f),
                    IM_COL32(255, 255, 255, 80), "?");
    }

    float textX = headX + headSize + headGap;
    float textY = headY + (headSize - nameDim.y - subFontSize - 3.0f * scale) * 0.5f;

    dl->AddText(font, nameFontSize,
                ImVec2(textX, textY),
                IM_COL32(255, 255, 255, 255),
                g_cachedTargetName);

    char hpStr[24];
    sprintf_s(hpStr, sizeof(hpStr), "%.0f / %.0f HP", g_cachedHp, g_cachedMaxHp);
    dl->AddText(font, subFontSize,
                ImVec2(textX, textY + nameDim.y + 3.0f * scale),
                IM_COL32(180, 180, 180, 180),
                hpStr);

    float barMargin = padding;
    float barY0 = anchorY + cardH - padding - barHeight;
    float barX0 = anchorX + barMargin;
    float barX1 = anchorX + cardW - barMargin;
    float barW  = barX1 - barX0;
    float barR  = barHeight * 0.5f;

    dl->AddRectFilled(ImVec2(barX0, barY0),
                      ImVec2(barX1, barY0 + barHeight),
                      IM_COL32(45, 45, 45, 220),
                      barR);

    float fillW = barW * g_displayedHpFrac;
    if (fillW > barR * 2.0f) {
        dl->AddRectFilled(ImVec2(barX0, barY0),
                          ImVec2(barX0 + fillW, barY0 + barHeight),
                          IM_COL32(240, 240, 240, 220),
                          barR);
    } else if (fillW > 0.5f) {

        dl->AddRectFilled(ImVec2(barX0, barY0),
                          ImVec2(barX0 + max(fillW, 2.0f), barY0 + barHeight),
                          IM_COL32(240, 240, 240, 220),
                          1.0f);
    }

    dl->AddRect(ImVec2(barX0, barY0),
                ImVec2(barX1, barY0 + barHeight),
                IM_COL32(255, 255, 255, 35),
                barR, 0, 0.8f);
}

void DrawCatIcon(ImDrawList *dl, ImVec2 c, int idx, ImU32 col) {
  float r = 6.0f;
  switch (idx) {
  case 0:
    dl->AddCircleFilled(ImVec2(c.x - r * 0.45f, c.y - r * 0.45f), r * 0.25f, col, 8);
    dl->AddCircleFilled(ImVec2(c.x + r * 0.45f, c.y - r * 0.45f), r * 0.25f, col, 8);
    dl->AddCircleFilled(ImVec2(c.x - r * 0.45f, c.y + r * 0.45f), r * 0.25f, col, 8);
    dl->AddCircleFilled(ImVec2(c.x + r * 0.45f, c.y + r * 0.45f), r * 0.25f, col, 8);
    break;
  case 1:
    dl->AddLine(ImVec2(c.x - r, c.y - r), ImVec2(c.x + r, c.y + r), col, 1.8f);
    dl->AddLine(ImVec2(c.x + r, c.y - r), ImVec2(c.x - r, c.y + r), col, 1.8f);
    break;
  case 2:
    dl->AddTriangleFilled(ImVec2(c.x + r, c.y),
                          ImVec2(c.x - r * 0.5f, c.y - r * 0.7f),
                          ImVec2(c.x - r * 0.5f, c.y + r * 0.7f), col);
    break;
  case 3:
    dl->AddCircle(c, r * 0.75f, col, 12, 1.8f);
    dl->AddCircleFilled(c, r * 0.3f, col, 8);
    break;
  case 4:
    for (int i = 0; i < 3; i++) {
      float bx = c.x + (i - 1) * 5.0f;
      float bh = 4.0f + i * 4.0f;
      dl->AddRectFilled(ImVec2(bx - 1.5f, c.y + r - bh),
                        ImVec2(bx + 1.5f, c.y + r), col, 1.0f);
    }
    break;
  case 5:
    dl->AddLine(ImVec2(c.x - r * 0.6f, c.y - r * 0.3f),
                ImVec2(c.x + r * 0.6f, c.y + r * 0.3f), col, 2.0f);
    dl->AddLine(ImVec2(c.x - r * 0.6f, c.y + r * 0.3f),
                ImVec2(c.x + r * 0.6f, c.y - r * 0.3f), col, 2.0f);
    dl->AddCircleFilled(ImVec2(c.x - r * 0.6f, c.y - r * 0.3f), r * 0.2f, col,
                        8);
    dl->AddCircleFilled(ImVec2(c.x + r * 0.6f, c.y + r * 0.3f), r * 0.2f, col,
                        8);
    break;
  case 6:
    dl->AddCircleFilled(ImVec2(c.x - r * 0.3f, c.y), r * 0.15f, col, 8);
    dl->AddCircleFilled(ImVec2(c.x, c.y), r * 0.15f, col, 8);
    dl->AddCircleFilled(ImVec2(c.x + r * 0.3f, c.y), r * 0.15f, col, 8);
    break;
  case 7:
    for (int i = 0; i < 6; i++) {
      float a1 = (float)i * 3.14159f * 2.0f / 6.0f;
      float a2 = (float)(i + 1) * 3.14159f * 2.0f / 6.0f;
      dl->AddLine(ImVec2(c.x + cosf(a1) * r, c.y + sinf(a1) * r),
                  ImVec2(c.x + cosf(a2) * r, c.y + sinf(a2) * r), col, 1.8f);
    }
    dl->AddCircleFilled(c, r * 0.25f, col, 8);
    break;
  case 8:
    dl->AddCircle(c, r * 0.85f, col, 16, 1.8f);
    dl->AddCircleFilled(ImVec2(c.x - r * 0.35f, c.y - r * 0.2f), r * 0.18f, col, 6);
    dl->AddCircleFilled(ImVec2(c.x + r * 0.1f, c.y - r * 0.4f), r * 0.18f, col, 6);
    dl->AddCircleFilled(ImVec2(c.x + r * 0.4f, c.y - r * 0.05f), r * 0.18f, col, 6);
    break;
  }
}

bool *GetModuleEnabledPtr(int cat, int mod) {
  if (cat == 1) {
    if (mod == 0) return &aimbotEnabled;
    if (mod == 1) return &clickerEnabled;
    if (mod == 2) return &blockhitEnabled;
    if (mod == 3) return &reachEnabled;
    if (mod == 4) return &hitSelectEnabled;
    if (mod == 5) return &sTapEnabled;
    if (mod == 6) return &wTapEnabled;
    if (mod == 7) return &bowAimbotEnabled;
  } else if (cat == 2) {
    if (mod == 0) return &velocityEnabled;
    if (mod == 1) return &sprintEnabled;
    if (mod == 2) return &sprintResetEnabled;
    if (mod == 3) return &noFallEnabled;
    if (mod == 4) return &invWalkEnabled;
    if (mod == 5) return &jumpResetEnabled;
    if (mod == 6) return &noJumpDelayEnabled;
    if (mod == 7) return &scaffoldEnabled;
    if (mod == 8) return &stepEnabled;
    if (mod == 9) return &spiderEnabled;
    if (mod == 10) return &safeWalkEnabled;
    if (mod == 11) return &reverseStepEnabled;
    if (mod == 12) return &bouncySlimeEnabled;
    if (mod == 13) return &fastStopEnabled;
    if (mod == 14) return &glideEnabled;
    if (mod == 15) return &flightEnabled;
    if (mod == 16) return &airJumpEnabled;
    if (mod == 17) return &highJumpEnabled;
  } else if (cat == 3) {
    if (mod == 0) return &espEnabled;
    if (mod == 1) return &tracerEnabled;
    if (mod == 2) return &xrayEnabled;
    if (mod == 3) return &storageEspEnabled;
    if (mod == 4) return &arraylistEnabled;
    if (mod == 5) return &chamsEnabled;
    if (mod == 6) return &targetHudEnabled;
    if (mod == 7) return &freecamEnabled;
    if (mod == 8) return &fullBrightEnabled;
    if (mod == 9) return &noHurtCamEnabled;
    if (mod == 10) return &zoomEnabled;
    if (mod == 11) return &armorEspEnabled;
    if (mod == 12) return &itemEspEnabled;
    if (mod == 13) return &holeEspEnabled;
  } else if (cat == 4) {
    if (mod == 0) return &blinkEnabled;
    if (mod == 1) return &timerEnabled;
    if (mod == 2) return &fakeLagEnabled;
  } else if (cat == 5) {
    if (mod == 0) return &rightClickerEnabled;
    if (mod == 1) return &fastPlaceEnabled;
    if (mod == 2) return &throwpotEnabled;
    if (mod == 3) return &autosoupEnabled;
    if (mod == 4) return &refillEnabled;
    if (mod == 5) return &autoToolEnabled;
    if (mod == 6) return &bridgeAssistEnabled;
    if (mod == 7) return &teamsEnabled;
    if (mod == 8) return &friendsEnabled;
    if (mod == 9) return &autoSwordEnabled;
    if (mod == 10) return &antiVoidEnabled;
    if (mod == 11) return &middleClickFriendEnabled;
    if (mod == 12) return &antiBotEnabled;
    if (mod == 13) return &autoArmorEnabled;
    if (mod == 14) return &inventoryManagerEnabled;
  } else if (cat == 6) {
    if (mod == 0) return &breadCrumbsEnabled;
    if (mod == 1) return &antiAfkEnabled;
    if (mod == 2) return &antiBadEffectsEnabled;
    if (mod == 3) return &panicEnabled;
    if (mod == 4) return &pingSpoofModEnabled;
    if (mod == 5) return &reconnectEnabled;
    if (mod == 6) return &staffNotifierEnabled;
    if (mod == 7) return &noRotateEnabled;
    if (mod == 8) return &antiCactusEnabled;
  } else if (cat == 8) {
    if (mod == 2) return &uiPresetsEnabled;
  }
  return nullptr;
}

void RenderModuleSettings(int cat, int mod) {
  if (cat < 0 || mod < 0)
    return;

  if (cat == 1) {
      if (mod == 0) {
          CustomCheckbox("Enabled", &aimbotEnabled);
          ImGui::Dummy(ImVec2(0, 4));

          SectionHeader("Mode");
          const char* modeItems[] = { "Single", "Switch" };
          ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
          ImGui::Text("Mode");
          ImGui::PopStyleColor();
          ImGui::SameLine();
          ImGui::PushItemWidth(120);
          if (ImGui::Combo("##AimMode", &aimAssistMode, modeItems, 2)) {
              aimLockOnActive = false;
              aimLockLostFrames = 0;
          }
          ImGui::PopItemWidth();

          if (aimAssistMode == 1) {
              ImGui::Dummy(ImVec2(0, 4));
              SectionHeader("Switch Criteria");
              const char* criteriaItems[] = { "Distance", "Angle" };
              ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
              ImGui::Text("Switch By");
              ImGui::PopStyleColor();
              ImGui::SameLine();
              ImGui::PushItemWidth(120);
              ImGui::Combo("##SwitchCriteria", &aimSwitchCriteria, criteriaItems, 2);
              ImGui::PopItemWidth();
              CustomSlider("Switch Time", &aimSwitchTime, 0.5f, 5.0f, "%.1f s");
          }

          ImGui::Dummy(ImVec2(0, 4));

          SectionHeader("Activation");
          const char* activationItems[] = { "Always", "Click Only", "Mouse Movement", "Sprinting" };
          ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
          ImGui::Text("Activate");
          ImGui::PopStyleColor();
          ImGui::SameLine();
          ImGui::PushItemWidth(140);
          ImGui::Combo("##AimActivation", &aimActivation, activationItems, 4);
          ImGui::PopItemWidth();

          ImGui::Dummy(ImVec2(0, 4));

          SectionHeader("Aim Settings");
          CustomSlider("Distance", &aimDistance, 1.0f, 20.0f, "%.1f blocks");
          CustomSliderInt("Speed", &aimSpeedInt, 1, 90, "%d");
          CustomSlider("Angle", &aimAngleDeg, 1.0f, 180.0f, "%.0f\xc2\xb0");

          ImGui::Dummy(ImVec2(0, 4));
          SectionHeader("Options");
          CustomCheckbox("Vertical Aim", &aimVerticalAim);

          if (aimVerticalAim) {
              CustomSlider("Vertical Speed", &aimVerticalSpeed, 1.0f, 90.0f, "%.0f");
              CustomSlider("Height Target", &aimHeightFactor, 0.0f, 1.0f, "%.2f");
          }

          CustomCheckbox("Lock On Target", &aimLockOnTarget);
          CustomCheckbox("Aim Nearest", &aimNearest);
          CustomCheckbox("Disable in Liquid", &aimDisableInLiquid);
          CustomCheckbox("Sword Only", &aimbotSwordOnly);

          RenderModuleKeybind(&aimbotEnabled, "AimAssist");
      }
      else if (mod == 1) {
      CustomCheckbox("Enabled", &clickerEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Activation Mode");
      const char *activationModeItems[] = {"Toggle", "Hold Key", "Hold LMB"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Mode");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##ClickerMode", &clickerActivationMode, activationModeItems,
                   3);
      ImGui::PopItemWidth();

      if (clickerActivationMode == 1) {
        ImGui::Dummy(ImVec2(0, 2));
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::Text("Hold Key");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (g_capturingClickerKey) {
          ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
          ImGui::Text("Press any key...");
          ImGui::PopStyleColor();
        } else {
          ImGui::Text("[%s]",
                      clickerActivationKey != 0
                          ? VirtualKeyToString(clickerActivationKey).c_str()
                          : "Not Set");
        }
        ImGui::SameLine();
        if (ImGui::Button("Set##ClickerHoldKey")) {
          g_capturingClickerKey = true;
        }
        if (clickerActivationKey != 0) {
          ImGui::SameLine();
          if (ImGui::Button("Clear##ClickerHoldKey")) {
            clickerActivationKey = 0;
          }
        }
      }

      CustomCheckbox("Allow in Inventory", &clickerAllowInInventory);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Click Settings");
      CustomSliderInt("CPS", &clickerCpsInt, 1, 25, "%d CPS");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Actual: %.1f CPS | Total: %d", currentCps, totalClicks);
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Randomization");
      CustomCheckbox("CPS Randomization", &clickerEnableRandomization);
      if (BeginAnimatedSection("lcRand", clickerEnableRandomization)) {
        float randFloat = (float)clickerRandomizationAmount;
        if (CustomSlider("Variance", &randFloat, 0.5f, 8.0f, "+/- %.1f")) {
          clickerRandomizationAmount = randFloat;
        }
        EndAnimatedSection();
      }

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Spike");
      CustomCheckbox("Spike Mode", &clickerSpikeMode);
      if (BeginAnimatedSection("lcSpike", clickerSpikeMode)) {
        CustomSliderInt("Chance##spike", &clickerSpikeChance, 1, 100, "%d%%");
        float spikeFloat = (float)clickerSpikeIncreaseCps;
        if (CustomSlider("CPS Boost", &spikeFloat, 0.5f, 10.0f, "+%.1f")) {
          clickerSpikeIncreaseCps = spikeFloat;
        }
        EndAnimatedSection();
      }

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Exhaust");
      CustomCheckbox("Exhaust Mode", &clickerExhaustMode);
      if (BeginAnimatedSection("lcExhaust", clickerExhaustMode)) {
        CustomSliderInt("Chance##exhaust", &clickerExhaustChance, 1, 100,
                        "%d%%");
        float dropFloat = (float)clickerExhaustDropCps;
        if (CustomSlider("CPS Drop", &dropFloat, 0.5f, 5.0f, "-%.1f")) {
          clickerExhaustDropCps = dropFloat;
        }
        EndAnimatedSection();
      }

      CustomCheckbox("Sword Only##Clicker", &clickerSwordOnly);

      RenderModuleKeybind(&clickerEnabled, "LeftClicker");
    } else if (mod == 2) {
      CustomCheckbox("Enabled", &blockhitEnabled);
      CustomCheckbox("Only While Clicking", &blockhitOnlyWhileClicking);
      SectionHeader("Timing");
      CustomSliderInt("Delay Min", &blockhitDelayMin, 50, 500, "%d ms");
      CustomSliderInt("Delay Max", &blockhitDelayMax, 100, 1000, "%d ms");
      CustomSliderInt("Block Chance", &blockhitBlockChance, 1, 100, "%d%%");
      CustomSliderInt("Hold Min", &blockhitHoldLengthMin, 10, 200, "%d ms");
      CustomSliderInt("Hold Max", &blockhitHoldLengthMax, 50, 500, "%d ms");

      CustomCheckbox("Sword Only##Blockhit", &blockhitSwordOnly);

      RenderModuleKeybind(&blockhitEnabled, "Blockhit");
    } else if (mod == 3) {
      CustomCheckbox("Enabled", &reachEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Reach Distance");
      CustomSlider("Min Reach", &reachMin, 3.0f, 6.0f, "%.2f blocks");
      CustomSlider("Max Reach", &reachMax, 3.0f, 6.0f, "%.2f blocks");
      if (reachMin > reachMax)
        reachMin = reachMax;
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Randomizes between %.2f - %.2f per hit", reachMin, reachMax);
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Conditions");
      CustomCheckbox("Attack Only", &reachAttackOnly);
      CustomCheckbox("Sprint Only", &reachSprintOnly);
      CustomCheckbox("While Jumping", &reachWhileJumping);
      CustomCheckbox("Only Moving", &reachOnlyMoving);
      CustomCheckbox("Disable in Liquid", &reachDisableInLiquid);
      CustomCheckbox("Sword Only##Reach", &reachSwordOnly);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Advanced");
      CustomSliderInt("Chance##Reach", &reachChance, 0, 100, "%d%%");
      CustomCheckbox("Motion Compensation", &reachMotionCompensation);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Gaussian distribution + damped spring interpolation for smooth, natural-looking reach values.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&reachEnabled, "Reach");
    } else if (mod == 4) {
      CustomCheckbox("Enabled", &hitSelectEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Mode");
      const char *hsModeItems[] = {"Burst", "Criticals"};
      ImGui::PushItemWidth(-1);
      ImGui::Combo("##HSMode", &hitSelectMode, hsModeItems, 2);
      ImGui::PopItemWidth();

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Timing");
      CustomSliderInt("Pause Duration", &hitSelectPauseDuration, 50, 1000,
                      "%d ms");
      CustomSliderInt("Wait for First Hit", &hitSelectWaitForFirstHit, 0, 1000,
                      "%d ms");
      CustomSliderInt("Hit Later in Trades", &hitSelectHitLaterInTrades, 0, 500,
                      "%d ms");

      ImGui::Dummy(ImVec2(0, 4));

      bool isCritMode = (hitSelectMode == 1);
      if (BeginAnimatedSection("hsCrit", isCritMode)) {
        SectionHeader("Criticals");
        CustomCheckbox("Disable During KB", &hitSelectDisableDuringKB);
        CustomCheckbox("Only While Damaged", &hitSelectOnlyWhileDamaged);
        ImGui::Dummy(ImVec2(0, 4));
        EndAnimatedSection();
      }

      SectionHeader("Advanced");
      CustomCheckbox("Use Server Attack Time", &hitSelectUseServerAttackTime);
      CustomCheckbox("Fake Swing", &hitSelectFakeSwing);

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Cancel Rate");
      CustomSliderInt("In Combat", &hitSelectCancelRateInCombat, 0, 100,
                      "%d%%");
      CustomSliderInt("Missed Swings", &hitSelectCancelRateMissed, 0, 100,
                      "%d%%");

      ImGui::Dummy(ImVec2(0, 4));
      CustomCheckbox("Sword Only##HitSelect", &hitSelectSwordOnly);

      RenderModuleKeybind(&hitSelectEnabled, "Hit Select");
    } else if (mod == 5) {
      CustomCheckbox("Enabled", &sTapEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("S-Tap Settings");
      CustomSlider("Chance", &sTapConfig.chance, 0.0f, 100.0f, "%.1f%%");
      CustomSliderInt("Min Delay", &sTapConfig.delayMin, 10, 300, "%d ms");
      CustomSliderInt("Max Delay", &sTapConfig.delayMax, 10, 500, "%d ms");
      if (sTapConfig.delayMin > sTapConfig.delayMax) sTapConfig.delayMin = sTapConfig.delayMax;
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Sprint release: %d - %d ms (%d - %d ticks)", sTapConfig.delayMin, sTapConfig.delayMax,
                  (std::max)(1, sTapConfig.delayMin / 50), (std::max)(1, sTapConfig.delayMax / 50));
      ImGui::PopStyleColor();
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Conditions");
      CustomCheckbox("Adaptive Delay", &sTapConfig.adaptiveDelay);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Scales delay by distance to target (closer = shorter delay).");
      ImGui::PopStyleColor();
      CustomCheckbox("Only While Sprinting", &sTapConfig.onlyWhileSprinting);
      CustomCheckbox("Require Sword", &sTapConfig.requireSword);
      CustomCheckbox("Anti-Bot Check", &sTapConfig.antiBotCheck);

      RenderModuleKeybind(&sTapEnabled, "S-Tap");
    } else if (mod == 6) {
      CustomCheckbox("Enabled", &wTapEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("W-Tap Settings");
      CustomSlider("Chance", &wTapConfig.chance, 0.0f, 100.0f, "%.1f%%");
      CustomSliderInt("Min Duration", &wTapConfig.durationMin, 1, 10, "%d ticks");
      CustomSliderInt("Max Duration", &wTapConfig.durationMax, 1, 10, "%d ticks");
      if (wTapConfig.durationMin > wTapConfig.durationMax) wTapConfig.durationMin = wTapConfig.durationMax;
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("W release: %d - %d ticks (%d - %d ms)", wTapConfig.durationMin, wTapConfig.durationMax,
                  wTapConfig.durationMin * 50, wTapConfig.durationMax * 50);
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Mode");
      CustomCheckbox("Legit Mode", &wTapConfig.legitMode);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped(wTapConfig.legitMode ? "Simulates W key release (visible to game)." : "Sends sprint packets silently (server-side only).");
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Conditions");
      CustomCheckbox("Only While Sprinting", &wTapConfig.onlyWhileSprinting);
      CustomCheckbox("Only Moving Forward", &wTapConfig.onlyForward);
      CustomCheckbox("Require Sword", &wTapConfig.requireSword);
      CustomCheckbox("Anti-Bot Check", &wTapConfig.antiBotCheck);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Advanced");
      CustomCheckbox("Adaptive Chance", &wTapConfig.adaptiveChance);
      if (wTapConfig.adaptiveChance) {
        CustomSliderInt("Combo Threshold", &wTapConfig.comboThreshold, 1, 10, "%d hits");
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::TextWrapped("Chance ramps up with consecutive hits on the same target.");
        ImGui::PopStyleColor();
      }

      RenderModuleKeybind(&wTapEnabled, "W-Tap");
    } else if (mod == 7) {
      CustomCheckbox("Enabled", &bowAimbotEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Aiming Settings");
      CustomSlider("FOV", &bowAimbotConfig.fov, 10.0f, 360.0f, "%.0f\xC2\xB0");
      CustomSlider("Range", &bowAimbotConfig.range, 10.0f, 128.0f, "%.0f blocks");
      CustomSlider("Smoothing", &bowAimbotConfig.smoothing, 1.0f, 20.0f, "%.1f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Lower = faster aim. GCD-corrected to match mouse sensitivity.");
      ImGui::PopStyleColor();
      CustomSlider("Prediction", &bowAimbotConfig.predictionStrength, 0.0f, 2.0f, "%.2f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Predicts target movement based on velocity. Uses real arrow physics.");
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Targeting");
      const char *filterModes[] = {"FOV (closest angle)", "Closest Distance", "Lowest Health"};
      ImGui::PushItemWidth(-1);
      ImGui::Combo("##BowFilter", &bowAimbotConfig.filterMode, filterModes, 3);
      ImGui::PopItemWidth();
      CustomCheckbox("Only Players", &bowAimbotConfig.onlyPlayers);
      CustomCheckbox("Ignore Friends", &bowAimbotConfig.ignoreFriends);
      CustomCheckbox("Require Bow", &bowAimbotConfig.requireBow);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Auto Shoot");
      CustomCheckbox("Auto Shoot", &bowAimbotConfig.autoShoot);
      if (bowAimbotConfig.autoShoot) {
        CustomSlider("Charge Threshold", &bowAimbotConfig.chargeThreshold, 0.1f, 1.0f, "%.0f%%");
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::TextWrapped("Releases bow when charge reaches this level.");
        ImGui::PopStyleColor();
      }

      RenderModuleKeybind(&bowAimbotEnabled, "Bow Aimbot");
    } else if (mod == 5) {
      CustomCheckbox("Enabled", &hitSelectEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Mode");
      const char *hsModeItems[] = {"Burst", "Criticals"};
      ImGui::PushItemWidth(-1);
      ImGui::Combo("##HSMode", &hitSelectMode, hsModeItems, 2);
      ImGui::PopItemWidth();

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Timing");
      CustomSliderInt("Pause Duration", &hitSelectPauseDuration, 50, 1000,
                      "%d ms");
      CustomSliderInt("Wait for First Hit", &hitSelectWaitForFirstHit, 0, 1000,
                      "%d ms");
      CustomSliderInt("Hit Later in Trades", &hitSelectHitLaterInTrades, 0, 500,
                      "%d ms");

      ImGui::Dummy(ImVec2(0, 4));

      bool isCritMode = (hitSelectMode == 1);
      if (BeginAnimatedSection("hsCrit", isCritMode)) {
        SectionHeader("Criticals");
        CustomCheckbox("Disable During KB", &hitSelectDisableDuringKB);
        CustomCheckbox("Only While Damaged", &hitSelectOnlyWhileDamaged);
        ImGui::Dummy(ImVec2(0, 4));
        EndAnimatedSection();
      }

      SectionHeader("Advanced");
      CustomCheckbox("Use Server Attack Time", &hitSelectUseServerAttackTime);
      CustomCheckbox("Fake Swing", &hitSelectFakeSwing);

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Cancel Rate");
      CustomSliderInt("In Combat", &hitSelectCancelRateInCombat, 0, 100,
                      "%d%%");
      CustomSliderInt("Missed Swings", &hitSelectCancelRateMissed, 0, 100,
                      "%d%%");

      ImGui::Dummy(ImVec2(0, 4));
      CustomCheckbox("Sword Only##HitSelect", &hitSelectSwordOnly);

      RenderModuleKeybind(&hitSelectEnabled, "Hit Select");
    }
  } else if (cat == 2) {
    if (mod == 0) {
      CustomCheckbox("Enabled", &velocityEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Knockback Reduction");
      CustomSlider("Horizontal", &velocityHorizontal, 0.0f, 100.0f, "%.0f%%");
      CustomSlider("Vertical", &velocityVertical, 0.0f, 100.0f, "%.0f%%");

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Conditions");
      CustomSlider("Chance", &velocityChance, 0.0f, 100.0f, "%.0f%%");
      CustomCheckbox("Only While Moving", &velocityOnlyWhileMoving);
      CustomCheckbox("Only On Ground", &velocityOnlyOnGround);

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::PopStyleColor();

      RenderModuleKeybind(&velocityEnabled, "Velocity");
    } else if (mod == 1) {
      CustomCheckbox("Enabled", &sprintEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::PopStyleColor();

      RenderModuleKeybind(&sprintEnabled, "Sprint");
    } else if (mod == 2) {
      CustomCheckbox("Enabled", &sprintResetEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Mode");
      const char *srModeItems[] = {"WTap", "Sneak", "NoStop"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Mode");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##SRMode", &sprintResetMode, srModeItems, 3);
      ImGui::PopItemWidth();

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Timing");
      CustomSliderInt("Delay After Attack", &sprintResetDelay, 20, 500, "%d ms");
      CustomSliderInt("Stop Duration", &sprintResetStopDuration, 10, 200, "%d ms");
      CustomCheckbox("Randomize", &sprintResetRandomize);

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Conditions");
      CustomCheckbox("Wait for Damage", &sprintResetWaitForDamage);
      CustomCheckbox("Holding Weapon Only", &sprintResetWeaponOnly);

      RenderModuleKeybind(&sprintResetEnabled, "SprintReset");
    } else if (mod == 3) {
      CustomCheckbox("Enabled", &noFallEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Negates fall damage by resetting fall distance to 0.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&noFallEnabled, "NoFall");
    } else if (mod == 4) {
      CustomCheckbox("Enabled", &invWalkEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Mode");
      const char *iwModeItems[] = {"Vanilla", "Spoof"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Mode");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##IWMode", &invWalkMode, iwModeItems, 2);
      ImGui::PopItemWidth();

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Lets you move while your inventory is open.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&invWalkEnabled, "InvWalk");
    } else if (mod == 5) {
      CustomCheckbox("Enabled", &jumpResetEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Settings");
      CustomSliderInt("Chance##JR", &jumpResetChance, 0, 100, "%d%%");
      CustomCheckbox("Only When Hurt##JR", &jumpResetOnlyWhenHurt);

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Automatically jumps when you receive knockback while grounded, resetting your velocity.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&jumpResetEnabled, "JumpReset");
    } else if (mod == 6) {
      CustomCheckbox("Enabled", &noJumpDelayEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Removes the delay between jumps, allowing you to jump instantly after landing.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&noJumpDelayEnabled, "NoJumpDelay");
    } else if (mod == 7) {
      CustomCheckbox("Enabled", &scaffoldEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Settings");
      CustomCheckbox("Legit Mode##Scaffold", &scaffoldLegitMode);
      CustomCheckbox("Auto Block##Scaffold", &scaffoldAutoBlock);
      CustomSliderInt("Delay##Scaffold", &scaffoldDelayMs, 0, 250, "%d ms");
      CustomSlider("Min Dist##Scaffold", &scaffoldMinDist, 0.0f, 0.25f, "%.2f");

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Automatically places blocks under your feet while moving, sprinting, and sprint-jumping. Legit mode silently smooths head/body look toward placement.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&scaffoldEnabled, "Scaffold");
    } else if (mod == 8) {
      CustomCheckbox("Enabled", &stepEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Settings");
      const char *stepModeItems[] = {"Vanilla", "Motion"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Mode");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##StepMode", &stepMode, stepModeItems, 2);
      ImGui::PopItemWidth();

      CustomSlider("Height##Step", &stepHeight, 0.625f, 10.0f, "%.1f blocks");
      CustomCheckbox("Reverse Step", &stepReverseStep);

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Automatically steps up blocks. Vanilla mode sets step height directly.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&stepEnabled, "Step");
    } else if (mod == 9) {
      CustomCheckbox("Enabled", &spiderEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("Speed", &spiderSpeed, 0.05f, 0.5f, "%.2f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Climb walls like a spider."); ImGui::PopStyleColor();
      RenderModuleKeybind(&spiderEnabled, "Spider");
    } else if (mod == 10) {
      CustomCheckbox("Enabled", &safeWalkEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Prevents you from walking off block edges."); ImGui::PopStyleColor();
      RenderModuleKeybind(&safeWalkEnabled, "SafeWalk");
    } else if (mod == 11) {
      CustomCheckbox("Enabled", &reverseStepEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("Height", &reverseStepHeight, 0.5f, 5.0f, "%.1f blocks");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Instantly step down blocks."); ImGui::PopStyleColor();
      RenderModuleKeybind(&reverseStepEnabled, "ReverseStep");
    } else if (mod == 12) {
      CustomCheckbox("Enabled", &bouncySlimeEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Bounce higher on slime blocks."); ImGui::PopStyleColor();
      RenderModuleKeybind(&bouncySlimeEnabled, "BouncySlime");
    } else if (mod == 13) {
      CustomCheckbox("Enabled", &fastStopEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Instantly stop moving when releasing keys."); ImGui::PopStyleColor();
      RenderModuleKeybind(&fastStopEnabled, "FastStop");
    } else if (mod == 14) {
      CustomCheckbox("Enabled", &glideEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("Fall Speed", &glideSpeed, 0.01f, 0.1f, "%.3f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Slow your falling speed like a glider."); ImGui::PopStyleColor();
      RenderModuleKeybind(&glideEnabled, "Glide");
    } else if (mod == 15) {
      CustomCheckbox("Enabled", &flightEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      const char *flightModes[] = {"Vanilla", "Smooth"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::Text("Mode"); ImGui::PopStyleColor();
      ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##FlightMode", &flightMode, flightModes, 2); ImGui::PopItemWidth();
      CustomSlider("Speed##Flight", &flightSpeed, 0.1f, 10.0f, "%.1f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Fly in survival mode."); ImGui::PopStyleColor();
      RenderModuleKeybind(&flightEnabled, "Flight");
    } else if (mod == 16) {
      CustomCheckbox("Enabled", &airJumpEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Jump while in the air."); ImGui::PopStyleColor();
      RenderModuleKeybind(&airJumpEnabled, "AirJump");
    } else if (mod == 17) {
      CustomCheckbox("Enabled", &highJumpEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("Motion", &highJumpMotion, 0.42f, 1.5f, "%.2f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Jump higher than normal. Default vanilla is 0.42."); ImGui::PopStyleColor();
      RenderModuleKeybind(&highJumpEnabled, "HighJump");
    }
  } else if (cat == 3) {
    if (mod == 0) {
      CustomCheckbox("Enabled", &espEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Box Style");
      const char *boxStyleItems[] = {"Full", "Corners", "2D"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Style");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##ESPBoxStyle", &espBoxStyle, boxStyleItems, 3);
      ImGui::PopItemWidth();
      if (espBoxStyle == 1) {
        CustomSlider("Corner Length", &espCornerLength, 0.05f, 0.50f, "%.2f");
      }

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Outline");
      CustomCheckbox("Show Outline", &espShowOutline);
      if (espShowOutline) {
        CustomSlider("Outline Width", &espOutlineWidth, 0.5f, 6.0f, "%.1f px");
        ImGui::Text("Outline Color:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##ESPOutline", (float *)&espOutlineColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar);
      }

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Custom Image");
      CustomCheckbox("Use Custom Image", &espUseImage);
      if (espUseImage) {
        ImGui::Text("Path: %s", espImagePath[0] ? espImagePath : "None");
        if (espImageLoaded)
          ImGui::Text("Loaded (%dx%d)", espImageWidth, espImageHeight);
        else
          ImGui::TextColored(ImVec4(1,0,0,1), "No image loaded");
        CustomCheckbox("Stretch to Fit", &espImageStretch);
        if (ImGui::Button("Browse..."))
          OpenESPImageDialog();
      }

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Fill");
      CustomCheckbox("Show Fill", &espShowFill);
      if (espShowFill) {
        ImGui::Text("Fill Color:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##ESPFill", (float *)&espFillColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar);
      }

      RenderModuleKeybind(&espEnabled, "ESP");
    } else if (mod == 1) {
      CustomCheckbox("Enabled", &tracerEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Line Settings");
      CustomSlider("Line Width", &tracerWidth, 0.5f, 6.0f, "%.1f px");

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Origin");
      const char *originItems[] = {"Bottom", "Center", "Top"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Origin");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##TracerOrigin", &tracerOrigin, originItems, 3);
      ImGui::PopItemWidth();

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Color");
      ImGui::Text("Tracer Color:");
      ImGui::SameLine();
      ImGui::ColorEdit4("##TracerColor", (float *)&tracerColor,
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoLabel |
                            ImGuiColorEditFlags_AlphaBar);

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Extra");
      CustomCheckbox("Show Distance", &tracerShowDistance);
      if (tracerShowDistance) {
        ImGui::Text("Distance Color:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##TracerDistColor", (float *)&tracerDistanceColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel);
      }

      RenderModuleKeybind(&tracerEnabled, "Tracer");
    } else if (mod == 2) {
      CustomCheckbox("Enabled", &xrayEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Rendering");
      CustomSlider("Opacity", &xrayOpacity, 0.1f, 1.0f, "%.2f");

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Block Filters");
      CustomCheckbox("Show Ores", &xrayShowOres);
      CustomCheckbox("Show Chests", &xrayShowChests);
      CustomCheckbox("Show Spawners", &xrayShowSpawners);

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.7f, 0.2f, 1.0f));
      ImGui::PopStyleColor();
      RenderModuleKeybind(&xrayEnabled, "XRay");
    } else if (mod == 3) {
      CustomCheckbox("Enabled", &chestEspEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Outline");
      CustomCheckbox("Show Outline##Chest", &chestShowOutline);
      if (chestShowOutline) {
        CustomSlider("Outline Width##Chest", &chestOutlineWidth, 0.5f, 6.0f,
                     "%.1f px");
        ImGui::Text("Outline Color:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##ChestOutline", (float *)&chestOutlineColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar);
      }

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Fill");
      CustomCheckbox("Show Fill##Chest", &chestShowFill);
      if (chestShowFill) {
        ImGui::Text("Fill Color:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##ChestFill", (float *)&chestFillColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar);
      }

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Tracers");
      CustomCheckbox("Show Tracers##Chest", &chestShowTracers);
      if (chestShowTracers) {
        CustomSlider("Tracer Width##Chest", &chestTracerWidth, 0.5f, 6.0f,
                     "%.1f px");
        ImGui::Text("Tracer Color:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##ChestTracerColor", (float *)&chestTracerColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar);
      }

      RenderModuleKeybind(&chestEspEnabled, "ChestESP");
    } else if (mod == 4) {
      CustomCheckbox("Enabled", &arraylistEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Display");
      CustomSlider("Font Scale", &arraylistFontScale, 0.8f, 2.0f, "%.2f");
      CustomSlider("Background Alpha", &arraylistBgAlpha, 0.0f, 1.0f, "%.2f");
      CustomSlider("Bar Width", &arraylistBarWidth, 0.0f, 8.0f, "%.1f px");
      CustomCheckbox("Show Module Info", &arraylistShowInfo);
      CustomCheckbox("Show Background", &arraylistBackground);
      CustomCheckbox("Show Accent Bar", &arraylistAccentBar);
      CustomCheckbox("Rounded Corners", &arraylistRoundedCorners);

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Position");
      const char *posItems[] = {"Top Right", "Top Left", "Bottom Right",
                                "Bottom Left"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Anchor");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##ALPos", &arraylistPosition, posItems, 4);
      ImGui::PopItemWidth();

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Colors");
      ImGui::Text("Text:");
      ImGui::SameLine();
      ImGui::ColorEdit4("##ALText", (float *)&arraylistTextColor,
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoLabel);
      ImGui::Text("Info Text:");
      ImGui::SameLine();
      ImGui::ColorEdit4("##ALInfo", (float *)&arraylistInfoColor,
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoLabel);
      ImGui::Text("Accent Bar:");
      ImGui::SameLine();
      ImGui::ColorEdit4("##ALAccent", (float *)&arraylistAccentColor,
                        ImGuiColorEditFlags_NoInputs |
                            ImGuiColorEditFlags_NoLabel);

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Rainbow");
      CustomCheckbox("Rainbow Bar", &arraylistRainbow);
      if (BeginAnimatedSection("alRainbow", arraylistRainbow)) {
        CustomSlider("Speed", &arraylistRainbowSpeed, 0.1f, 5.0f, "%.1f");
        EndAnimatedSection();
      }

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Watermark");
      CustomCheckbox("Watermark##AL", &arraylistWatermark);
      if (BeginAnimatedSection("wmSettings", arraylistWatermark)) {
        CustomCheckbox("Client Name##WM", &arraylistWmShowName);
        CustomCheckbox("Version##WM", &arraylistWmShowVersion);
        CustomCheckbox("FPS##WM", &arraylistWmShowFps);
        CustomCheckbox("Keybinds##WM", &arraylistWmShowKeybinds);

        ImGui::Dummy(ImVec2(0, 4));

        const char *wmPosItems[] = {"Top Left", "Top Right"};
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::Text("Position:");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::Combo("##WMPos", &arraylistWmPosition, wmPosItems, 2);
        ImGui::PopItemWidth();

        CustomSlider("Font Scale##WM", &arraylistWmFontScale, 0.7f, 2.0f, "%.2f");

        ImGui::Dummy(ImVec2(0, 4));

        ImGui::Text("Text Color:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##WMTextCol", (float *)&arraylistWmTextColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel);
        ImGui::Text("Background:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##WMBgCol", (float *)&arraylistWmBgColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar);
        ImGui::Text("Border:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##WMBorderCol", (float *)&arraylistWmBorderColor,
                          ImGuiColorEditFlags_NoInputs |
                              ImGuiColorEditFlags_NoLabel |
                              ImGuiColorEditFlags_AlphaBar);
        EndAnimatedSection();
      }

      ImGui::Dummy(ImVec2(0, 8));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.18f, 0.8f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            ImVec4(0.25f, 0.25f, 0.30f, 0.9f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            ImVec4(0.10f, 0.10f, 0.12f, 1.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, UILayout::CARD_ROUNDING);
      if (ImGui::Button("Reset to Default", ImVec2(140, 28))) {
        arraylistFontScale = 1.28f;
        arraylistBgAlpha = 0.90f;
        arraylistBarWidth = 3.0f;
        arraylistShowInfo = true;
        arraylistRainbow = false;
        arraylistRainbowSpeed = 1.0f;
        arraylistAccentColor = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
        arraylistTextColor = ImVec4(0.93f, 0.93f, 0.96f, 1.0f);
        arraylistInfoColor = ImVec4(0.50f, 0.50f, 0.56f, 1.0f);
        arraylistPosition = 0;
        arraylistBackground = true;
        arraylistAccentBar = true;
        arraylistRoundedCorners = false;
      }
      ImGui::PopStyleVar();
      ImGui::PopStyleColor(3);
    } else if (mod == 5) {
      CustomCheckbox("Enabled", &chamsEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Makes players visible through walls.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&chamsEnabled, "Chams");
    } else if (mod == 6) {
      CustomCheckbox("Enabled", &targetHudEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Style");
      CustomSlider("Scale", &targetHudScale, 0.5f, 2.0f, "%.1f");
      SectionHeader("Position");
      CustomSlider("X Offset", &targetHudX, -600.0f, 600.0f, "%.0f");
      CustomSlider("Y Offset", &targetHudY, -400.0f, 400.0f, "%.0f");
      SectionHeader("Timing");
      CustomSlider("Show Duration", &g_hudDisplayTime, 1.0f, 15.0f, "%.1fs");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Call NotifyTargetHit() from your hit/damage logic, not from mouse click.");
      ImGui::PopStyleColor();
      RenderModuleKeybind(&targetHudEnabled, "TargetHUD");
    } else if (mod == 7) {
      CustomCheckbox("Enabled", &freecamEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("Speed", &freecamSpeed, 0.1f, 5.0f, "%.1f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Detach camera and fly around freely."); ImGui::PopStyleColor();
      RenderModuleKeybind(&freecamEnabled, "Freecam");
    } else if (mod == 8) {
      CustomCheckbox("Enabled", &fullBrightEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      const char *fbModes[] = {"Gamma", "Night Vision"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::Text("Mode"); ImGui::PopStyleColor();
      ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##FBMode", &fullBrightMode, fbModes, 2); ImGui::PopItemWidth();
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("See in the dark. Gamma sets brightness to max, Night Vision applies potion effect."); ImGui::PopStyleColor();
      RenderModuleKeybind(&fullBrightEnabled, "FullBright");
    } else if (mod == 9) {
      CustomCheckbox("Enabled", &noHurtCamEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Removes the screen shake when taking damage."); ImGui::PopStyleColor();
      RenderModuleKeybind(&noHurtCamEnabled, "NoHurtCam");
    } else if (mod == 10) {
      CustomCheckbox("Enabled", &zoomEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("FOV", &zoomFov, 5.0f, 90.0f, "%.0f");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Hold key to zoom in. Default key: C"); ImGui::PopStyleColor();
      RenderModuleKeybind(&zoomEnabled, "Zoom");
    } else if (mod == 11) {
      CustomCheckbox("Enabled", &armorEspEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Shows armor pieces on players."); ImGui::PopStyleColor();
      RenderModuleKeybind(&armorEspEnabled, "ArmorESP");
    } else if (mod == 12) {
      CustomCheckbox("Enabled", &itemEspEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Color");
      ImGui::Text("Item Color:"); ImGui::SameLine();
      ImGui::ColorEdit4("##ItemESPCol", (float*)&itemEspColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Highlights dropped items in the world."); ImGui::PopStyleColor();
      RenderModuleKeybind(&itemEspEnabled, "ItemESP");
    } else if (mod == 13) {
      CustomCheckbox("Enabled", &holeEspEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Colors");
      ImGui::Text("Safe:"); ImGui::SameLine();
      ImGui::ColorEdit4("##HoleSafe", (float*)&holeEspSafeColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
      ImGui::Text("Unsafe:"); ImGui::SameLine();
      ImGui::ColorEdit4("##HoleUnsafe", (float*)&holeEspUnsafeColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Highlights safe holes (bedrock) and unsafe holes (obsidian)."); ImGui::PopStyleColor();
      RenderModuleKeybind(&holeEspEnabled, "HoleESP");
    }
  } else if (cat == 4) {
    if (mod == 0) {
      CustomCheckbox("Enabled", &blinkEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Holds all outgoing packets. Release by disabling. Your character will teleport to your actual position."); ImGui::PopStyleColor();
      RenderModuleKeybind(&blinkEnabled, "Blink");
    } else if (mod == 1) {
      CustomCheckbox("Enabled", &timerEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("Speed", &timerSpeed, 0.1f, 5.0f, "%.1fx");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Changes game tick speed. 1.0 is normal, 2.0 is double speed."); ImGui::PopStyleColor();
      RenderModuleKeybind(&timerEnabled, "Timer");
    } else if (mod == 2) {
      CustomCheckbox("Enabled", &fakeLagEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSliderInt("Delay##FakeLag", &fakeLagDelay, 50, 1000, "%d ms");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Simulates lag by delaying packets. Makes you appear to teleport to other players."); ImGui::PopStyleColor();
      RenderModuleKeybind(&fakeLagEnabled, "FakeLag");
    }
  } else if (cat == 5) {
    if (mod == 0) {
      CustomCheckbox("Enabled", &rightClickerEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Activation");
      const char *rcActItems[] = {"Toggle", "Hold Key", "Hold RMB"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Mode");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::PushItemWidth(120);
      ImGui::Combo("##RCActMode", &rightClickerActivationMode, rcActItems, 3);
      ImGui::PopItemWidth();

      if (rightClickerActivationMode == 1) {
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::Text("Hold Key");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::Text("[%s]", VirtualKeyToString(rightClickerActivationKey).c_str());
        ImGui::SameLine();
        if (g_capturingRightClickerKey) {
          ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
          ImGui::Text("Press any key...");
          ImGui::PopStyleColor();
          for (int vk = 1; vk < 256; vk++) {
            if (GetAsyncKeyState(vk) & 0x8000) {
              rightClickerActivationKey = vk;
              g_capturingRightClickerKey = false;
              break;
            }
          }
        } else {
          if (ImGui::Button("Set##RCHoldKey"))
            g_capturingRightClickerKey = true;
        }
        if (rightClickerActivationKey != 0) {
          ImGui::SameLine();
          if (ImGui::Button("Clear##RCHoldKey")) {
            rightClickerActivationKey = 0;
          }
        }
      }

      CustomCheckbox("Allow in Inventory", &rightClickerAllowInInventory);
      CustomCheckbox("Blatant", &rightClickerBlatant);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Click Settings");
      int rcMaxCps = rightClickerBlatant ? 50 : 25;
      if (rightClickerCpsInt > rcMaxCps) rightClickerCpsInt = rcMaxCps;
      CustomSliderInt("CPS", &rightClickerCpsInt, 1, rcMaxCps, "%d CPS");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Actual: %.1f CPS | Total: %d", rightClickerCurrentCps, rightClickerTotalClicks);
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Randomization");
      CustomCheckbox("CPS Randomization", &rightClickerEnableRandomization);
      if (BeginAnimatedSection("rcRand", rightClickerEnableRandomization)) {
        float rcRandFloat = (float)rightClickerRandomizationAmount;
        if (CustomSlider("Variance", &rcRandFloat, 0.5f, 5.0f, "+/- %.1f")) {
          rightClickerRandomizationAmount = rcRandFloat;
        }
        EndAnimatedSection();
      }

      RenderModuleKeybind(&rightClickerEnabled, "RightClicker");
    } else if (mod == 1) {
      CustomCheckbox("Enabled", &fastPlaceEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Settings");
      CustomSliderInt("Tick Delay", &fastPlaceTickDelay, 0, 3, "%d ticks");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text(fastPlaceTickDelay == 0 ? "Instant placement" : "%d tick delay", fastPlaceTickDelay);
      ImGui::PopStyleColor();

      RenderModuleKeybind(&fastPlaceEnabled, "FastPlace");
    } else if (mod == 2) {
      CustomCheckbox("Enabled", &throwpotEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Trigger Key");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Hold Key");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::Text("[%s]", VirtualKeyToString(throwpotTriggerKey).c_str());
      ImGui::SameLine();

      static bool capturingThrowpotKey = false;
      if (capturingThrowpotKey) {
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
        ImGui::Text("Press any key...");
        ImGui::PopStyleColor();
        for (int vk = 1; vk < 256; vk++) {
          if (GetAsyncKeyState(vk) & 0x8000) {
            throwpotTriggerKey = vk;
            capturingThrowpotKey = false;
            break;
          }
        }
      } else {
        if (ImGui::Button("Set##ThrowpotKey"))
          capturingThrowpotKey = true;
      }

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Throw Settings");
      CustomSliderInt("Pots Per Press", &throwpotPotCount, 1, 3, "%d pots");
      CustomSliderInt("Switch Delay", &throwpotSwitchDelay, 10, 250, "%d ms");
      CustomSliderInt("Throw Delay", &throwpotThrowDelay, 10, 250, "%d ms");
      CustomSliderInt("Cooldown", &throwpotCooldownMs, 50, 2000, "%d ms");
      CustomCheckbox("Sword Only##Throwpot", &throwpotSwordOnly);

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::PopStyleColor();

      RenderModuleKeybind(&throwpotEnabled, "Throwpot");
    } else if (mod == 3) {
      CustomCheckbox("Enabled", &autosoupEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Trigger Key");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("Hold Key");
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::Text("[%s]", VirtualKeyToString(autosoupTriggerKey).c_str());
      ImGui::SameLine();

      static bool capturingAutosoupKey = false;
      if (capturingAutosoupKey) {
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
        ImGui::Text("Press any key...");
        ImGui::PopStyleColor();
        for (int vk = 1; vk < 256; vk++) {
          if (GetAsyncKeyState(vk) & 0x8000) {
            autosoupTriggerKey = vk;
            capturingAutosoupKey = false;
            break;
          }
        }
      } else {
        if (ImGui::Button("Set##AutosoupKey"))
          capturingAutosoupKey = true;
      }

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Eat Settings");
      CustomSliderInt("Switch Delay", &autosoupSwitchDelay, 10, 250, "%d ms");
      CustomSliderInt("Eat Delay", &autosoupEatDelay, 10, 250, "%d ms");
      CustomSliderInt("Cooldown", &autosoupCooldownMs, 50, 1000, "%d ms");
      CustomCheckbox("Drop Bowls", &autosoupDropBowls);
      CustomCheckbox("Sword Only##AutoSoup", &autosoupSwordOnly);

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::PopStyleColor();

      RenderModuleKeybind(&autosoupEnabled, "AutoSoup");
    }
    else if (mod == 4) {
        CustomCheckbox("Enabled", &refillEnabled);
        ImGui::Dummy(ImVec2(0, 4));

        SectionHeader("Trigger Key");
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::Text("Hold Key");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::Text("[%s]", VirtualKeyToString(refillTriggerKey).c_str());
        ImGui::SameLine();

        static bool capturingRefillKey = false;
        if (capturingRefillKey) {
            ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
            ImGui::Text("Press any key...");
            ImGui::PopStyleColor();
            for (int vk = 1; vk < 256; vk++) {
                if (GetAsyncKeyState(vk) & 0x8000) {
                    refillTriggerKey = vk;
                    capturingRefillKey = false;
                    break;
                }
            }
        }
        else {
            if (ImGui::Button("Set##RefillKey"))
                capturingRefillKey = true;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0, 4));
        SectionHeader("Items");
        CustomCheckbox("Soup", &refillSoup);
        CustomCheckbox("Potion", &refillPotion);
        if (refillPotion) {
            CustomCheckbox("Use non-health potions", &refillUseNonHealthPotions);
        }

        ImGui::Dummy(ImVec2(0, 4));
        SectionHeader("Mouse Button");
        const char* mouseButtonItems[] = { "Left", "Right", "Left + Right" };
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
        ImGui::Text("Simulate");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushItemWidth(120);
        ImGui::Combo("##RefillMouseButton", &refillMouseButton, mouseButtonItems, 3);
        ImGui::PopItemWidth();

        ImGui::Dummy(ImVec2(0, 4));
        SectionHeader("Timing");
        CustomSliderInt("Delay After Open", &refillDelayAfterOpen, 0, 500, "%d ms");
        CustomSliderInt("Delay Before Close", &refillDelayBeforeClose, 0, 500, "%d ms");
        CustomSliderInt("Speed", &refillSpeed, 10, 200, "%d ms");
        CustomCheckbox("Smart Speed", &refillSmartSpeed);

        ImGui::Dummy(ImVec2(0, 4));
        SectionHeader("Options");
        CustomCheckbox("Close on Complete", &refillCloseOnComplete);
        CustomCheckbox("Disable on Complete", &refillDisableOnComplete);
        CustomCheckbox("Random Slots", &refillRandomSlots);

        RenderModuleKeybind(&refillEnabled, "Refill");
    } else if (mod == 5) {
      CustomCheckbox("Enabled", &autoToolEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Timing");
      CustomSliderInt("Activation Delay", &autoToolActivationDelay, 0, 500, "%d ms");

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Options");
      CustomCheckbox("Switch Back When Done", &autoToolSwitchBack);
      CustomCheckbox("Sneak Only", &autoToolSneakOnly);

      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Allow While Using");
      CustomCheckbox("Sword##AT", &autoToolAllowSword);
      CustomCheckbox("Tool##AT", &autoToolAllowTool);
      CustomCheckbox("Fists##AT", &autoToolAllowFists);
      CustomCheckbox("Other##AT", &autoToolAllowOther);

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Automatically selects the best tool when mining a block.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&autoToolEnabled, "AutoTool");
    } else if (mod == 6) {
      CustomCheckbox("Enabled", &bridgeAssistEnabled);
      ImGui::Dummy(ImVec2(0, 4));

      SectionHeader("Conditions");
      CustomCheckbox("RMB Only", &bridgeAssistRmbOnly);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Only active while holding right mouse button.");
      ImGui::PopStyleColor();

      CustomCheckbox("Backward Only", &bridgeAssistBackwardOnly);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Only when walking backward (S key).");
      ImGui::PopStyleColor();

      CustomCheckbox("Check Pitch", &bridgeAssistCheckPitch);
      if (bridgeAssistCheckPitch) {
        float pitchVal = bridgeAssistPitchThreshold;
        if (CustomSlider("Min Pitch", &pitchVal, 30.0f, 90.0f, "%.0f deg")) {
          bridgeAssistPitchThreshold = pitchVal;
        }
      }

      CustomCheckbox("Blocks Only", &bridgeAssistBlocksOnly);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Only when holding a block item.");
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Timing");
      CustomSliderInt("Delay", &bridgeAssistDelay, 0, 100, "%d ms");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Extra delay after sneaking (0-50ms random added).");
      ImGui::PopStyleColor();

      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Automatically sneaks at block edges to prevent falling. Great for bridging practice.");
      ImGui::PopStyleColor();

      RenderModuleKeybind(&bridgeAssistEnabled, "BridgeAssist");
    } else if (mod == 7) {
      CustomCheckbox("Enabled", &teamsEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      const char *teamModes[] = {"Color", "Armor"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::Text("Detection"); ImGui::PopStyleColor();
      ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##TeamMode", &teamsMode, teamModes, 2); ImGui::PopItemWidth();
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Detects teammates by name color or armor color. Prevents targeting them."); ImGui::PopStyleColor();
      RenderModuleKeybind(&teamsEnabled, "Teams");
    } else if (mod == 8) {
      CustomCheckbox("Enabled", &friendsEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Friend system. Prevents targeting friends. Use Middle Click Friend to add."); ImGui::PopStyleColor();
      RenderModuleKeybind(&friendsEnabled, "Friends");
    } else if (mod == 9) {
      CustomCheckbox("Enabled", &autoSwordEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Automatically selects the best sword when attacking a player."); ImGui::PopStyleColor();
      RenderModuleKeybind(&autoSwordEnabled, "AutoSword");
    } else if (mod == 10) {
      CustomCheckbox("Enabled", &antiVoidEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSlider("Distance", &antiVoidDistance, 1.0f, 20.0f, "%.0f blocks");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Teleports you back when falling into the void."); ImGui::PopStyleColor();
      RenderModuleKeybind(&antiVoidEnabled, "AntiVoid");
    } else if (mod == 11) {
      CustomCheckbox("Enabled", &middleClickFriendEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Middle click a player to add/remove them from your friends list."); ImGui::PopStyleColor();
      RenderModuleKeybind(&middleClickFriendEnabled, "MCFriend");
    } else if (mod == 12) {
      CustomCheckbox("Enabled", &antiBotEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      const char *abModes[] = {"Advanced", "Simple"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::Text("Mode"); ImGui::PopStyleColor();
      ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##ABMode", &antiBotMode, abModes, 2); ImGui::PopItemWidth();
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Filters out fake bot players from targeting. Advanced checks tab list, Simple checks names."); ImGui::PopStyleColor();
      RenderModuleKeybind(&antiBotEnabled, "AntiBot");
    } else if (mod == 13) {
      CustomCheckbox("Enabled", &autoArmorEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Automatically equips the best armor from your inventory."); ImGui::PopStyleColor();
      RenderModuleKeybind(&autoArmorEnabled, "AutoArmor");
    } else if (mod == 14) {
      CustomCheckbox("Enabled", &inventoryManagerEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSliderInt("Click Delay", &invManagerDelay, 50, 500, "%d ms");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Automatically sorts and cleans your inventory. Drops junk and organizes items."); ImGui::PopStyleColor();
      RenderModuleKeybind(&inventoryManagerEnabled, "InvManager");
    }

  } else if (cat == 6) {
    if (mod == 0) {
      CustomCheckbox("Enabled", &breadCrumbsEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Color");
      ImGui::Text("Trail Color:"); ImGui::SameLine();
      ImGui::ColorEdit4("##BCCol", (float*)&breadCrumbsColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar);
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Leaves a colored trail behind you as you walk."); ImGui::PopStyleColor();
      RenderModuleKeybind(&breadCrumbsEnabled, "BreadCrumbs");
    } else if (mod == 1) {
      CustomCheckbox("Enabled", &antiAfkEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      const char *afkModes[] = {"Spin", "Jump", "Walk"};
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::Text("Mode"); ImGui::PopStyleColor();
      ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##AFKMode", &antiAfkMode, afkModes, 3); ImGui::PopItemWidth();
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Prevents AFK kick by performing actions automatically."); ImGui::PopStyleColor();
      RenderModuleKeybind(&antiAfkEnabled, "AntiAFK");
    } else if (mod == 2) {
      CustomCheckbox("Enabled", &antiBadEffectsEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Removes negative potion effects like slowness, blindness, etc."); ImGui::PopStyleColor();
      RenderModuleKeybind(&antiBadEffectsEnabled, "AntiBadFX");
    } else if (mod == 3) {
      CustomCheckbox("Enabled", &panicEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Instantly disables all modules. Press the keybind in an emergency."); ImGui::PopStyleColor();
      RenderModuleKeybind(&panicEnabled, "Panic");
    } else if (mod == 4) {
      CustomCheckbox("Enabled", &pingSpoofModEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSliderInt("Amount##PS", &pingSpoofModAmount, 0, 1000, "%d ms");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Spoofs your ping to appear higher or lower to the server."); ImGui::PopStyleColor();
      RenderModuleKeybind(&pingSpoofModEnabled, "PingSpoof");
    } else if (mod == 5) {
      CustomCheckbox("Enabled", &reconnectEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Settings");
      CustomSliderInt("Delay##RC", &reconnectDelay, 1000, 30000, "%d ms");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Automatically reconnects to the server when disconnected."); ImGui::PopStyleColor();
      RenderModuleKeybind(&reconnectEnabled, "Reconnect");
    } else if (mod == 6) {
      CustomCheckbox("Enabled", &staffNotifierEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Notifies you when a staff member joins the server or vanishes."); ImGui::PopStyleColor();
      RenderModuleKeybind(&staffNotifierEnabled, "StaffNotif");
    } else if (mod == 7) {
      CustomCheckbox("Enabled", &noRotateEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Blocks server-forced rotation packets. Prevents the server from changing your look direction."); ImGui::PopStyleColor();
      RenderModuleKeybind(&noRotateEnabled, "NoRotate");
    } else if (mod == 8) {
      CustomCheckbox("Enabled", &antiCactusEnabled);
      ImGui::Dummy(ImVec2(0, 4));
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY); ImGui::TextWrapped("Prevents damage from cactus blocks."); ImGui::PopStyleColor();
      RenderModuleKeybind(&antiCactusEnabled, "AntiCactus");
    }

  } else if (cat == 7) {
    if (mod == 0) {
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::Text("See you soon!");
      ImGui::PopStyleColor();
      ImGui::Dummy(ImVec2(0, 12));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.12f, 0.75f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                            ImVec4(0.8f, 0.15f, 0.18f, 0.85f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                            ImVec4(0.4f, 0.05f, 0.08f, 1.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, UILayout::CARD_ROUNDING);
      if (ImGui::Button("Unload", ImVec2(140, 32))) {
        g_unloadRequested = true;
      }
      ImGui::PopStyleVar();
      ImGui::PopStyleColor(3);
    }
  } else if (cat == 8) {
    if (mod == 0) {
      const ImGuiColorEditFlags cFlags =
          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel |
          ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;

      SectionHeader("Backgrounds");
      ImGui::Text("Base");        ImGui::SameLine(); ImGui::ColorEdit4("##cBgBase",   (float*)&UITheme::BG_BASE, cFlags);
      ImGui::Text("Sidebar");     ImGui::SameLine(); ImGui::ColorEdit4("##cBgSide",   (float*)&UITheme::BG_SIDEBAR, cFlags);
      ImGui::Text("Card");        ImGui::SameLine(); ImGui::ColorEdit4("##cBgCard",   (float*)&UITheme::BG_CARD, cFlags);
      ImGui::Text("Card Border"); ImGui::SameLine(); ImGui::ColorEdit4("##cBgCardB",  (float*)&UITheme::BG_CARD_BORDER, cFlags);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Text");
      ImGui::Text("Primary");     ImGui::SameLine(); ImGui::ColorEdit4("##cTxtPri",   (float*)&UITheme::TEXT_PRIMARY, cFlags);
      ImGui::Text("Secondary");   ImGui::SameLine(); ImGui::ColorEdit4("##cTxtSec",   (float*)&UITheme::TEXT_SECONDARY, cFlags);
      ImGui::Text("Disabled");    ImGui::SameLine(); ImGui::ColorEdit4("##cTxtDis",   (float*)&UITheme::TEXT_DISABLED, cFlags);
      ImGui::Text("Category");    ImGui::SameLine(); ImGui::ColorEdit4("##cTxtCat",   (float*)&UITheme::TEXT_CATEGORY, cFlags);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Accent");
      ImGui::Text("Accent");      ImGui::SameLine(); ImGui::ColorEdit4("##cAccent",   (float*)&UITheme::ACCENT, cFlags);
      ImGui::Text("Hover");       ImGui::SameLine(); ImGui::ColorEdit4("##cAccHov",   (float*)&UITheme::ACCENT_HOVER, cFlags);
      ImGui::Text("Dim");         ImGui::SameLine(); ImGui::ColorEdit4("##cAccDim",   (float*)&UITheme::ACCENT_DIM, cFlags);
      ImGui::Text("Glow");        ImGui::SameLine(); ImGui::ColorEdit4("##cGlow",     (float*)&UITheme::GLOW, cFlags);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Items");
      ImGui::Text("Hover");       ImGui::SameLine(); ImGui::ColorEdit4("##cItmHov",   (float*)&UITheme::ITEM_HOVER, cFlags);
      ImGui::Text("Selected");    ImGui::SameLine(); ImGui::ColorEdit4("##cItmSel",   (float*)&UITheme::ITEM_SELECTED, cFlags);
      ImGui::Text("Sel Border");  ImGui::SameLine(); ImGui::ColorEdit4("##cItmSelB",  (float*)&UITheme::ITEM_SELECTED_BORDER, cFlags);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Controls");
      ImGui::Text("Toggle Off");  ImGui::SameLine(); ImGui::ColorEdit4("##cTogOff",   (float*)&UITheme::TOGGLE_OFF, cFlags);
      ImGui::Text("Toggle On");   ImGui::SameLine(); ImGui::ColorEdit4("##cTogOn",    (float*)&UITheme::TOGGLE_ON, cFlags);
      ImGui::Text("Slider BG");   ImGui::SameLine(); ImGui::ColorEdit4("##cSldrBg",   (float*)&UITheme::SLIDER_BG, cFlags);
      ImGui::Text("Separator");   ImGui::SameLine(); ImGui::ColorEdit4("##cSep",      (float*)&UITheme::SEPARATOR, cFlags);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Buttons");
      ImGui::Text("Normal");      ImGui::SameLine(); ImGui::ColorEdit4("##cBtnNorm",  (float*)&UITheme::BUTTON_NORMAL, cFlags);
      ImGui::Text("Hover");       ImGui::SameLine(); ImGui::ColorEdit4("##cBtnHov",   (float*)&UITheme::BUTTON_HOVER, cFlags);
      ImGui::Text("Active");      ImGui::SameLine(); ImGui::ColorEdit4("##cBtnAct",   (float*)&UITheme::BUTTON_ACTIVE, cFlags);

      ImGui::Dummy(ImVec2(0, 4));
      SectionHeader("Animations");
      CustomCheckbox("Dot Network##anim", &g_animDots);
      CustomCheckbox("Border Glow##anim", &g_animBreathGlow);
      CustomSlider("Dot Speed##anim", &g_animDotSpeed, 0.0f, 3.0f, "%.1f");
      CustomSlider("Dot Opacity##anim", &g_animDotOpacity, 0.0f, 2.0f, "%.1f");
      CustomSlider("Glow Intensity##anim", &g_animGlowIntensity, 0.0f, 3.0f, "%.1f");

      ImGui::Dummy(ImVec2(0, 12));
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.18f, 0.8f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.30f, 0.9f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.10f, 0.10f, 0.12f, 1.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

      if (ImGui::Button(g_presetSaving ? "Saving..." : "Save as Preset", ImVec2(140, 28)) && !g_presetSaving) {
        PresetFromCurrent(g_presetCustom);
        strncpy(g_presetCustom.name, "Custom", sizeof(g_presetCustom.name));
        g_hasCustomPreset = true;
        g_activePresetIdx = 2;

        CreateThread(nullptr, 0, PresetSaveThread, nullptr, 0, nullptr);
      }
      if (g_presetSaveStatus[0] != '\0' && !g_presetSaving) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
        ImGui::Text("%s", g_presetSaveStatus);
        ImGui::PopStyleColor();
      }
      ImGui::SameLine();
      if (ImGui::Button("Reset to Default", ImVec2(140, 28))) {
        UITheme::ResetToDefaults();
        g_animDots = true; g_animBreathGlow = true;
        g_animDotSpeed = 1.0f; g_animDotOpacity = 1.0f; g_animGlowIntensity = 1.0f;
        g_activePresetIdx = 0;
      }

      ImGui::PopStyleVar();
      ImGui::PopStyleColor(3);
    } else if (mod == 1) {
      SectionHeader("Theme Presets");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Select a preset to apply. Save your custom colors from the Colors module.");
      ImGui::PopStyleColor();
      ImGui::Dummy(ImVec2(0, 8));

      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));

      {
        bool isSel = (g_activePresetIdx == 0);
        if (isSel) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.18f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.25f));
        } else {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.08f, 0.08f, 0.10f, 0.8f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.12f, 0.12f, 0.15f, 0.9f));
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.06f, 0.08f, 1.0f));
        if (ImGui::Button(isSel ? "Dark  [Active]" : "Dark", ImVec2(-1, 34))) {
          ApplyPreset(g_presetDark);
          g_activePresetIdx = 0;
        }
        ImGui::PopStyleColor(3);
      }

      ImGui::Dummy(ImVec2(0, 2));

      {
        bool isSel = (g_activePresetIdx == 1);
        if (isSel) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.18f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.25f));
        } else {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.08f, 0.08f, 0.10f, 0.8f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.12f, 0.12f, 0.15f, 0.9f));
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.06f, 0.08f, 1.0f));
        if (ImGui::Button(isSel ? "Light  [Active]" : "Light", ImVec2(-1, 34))) {
          ApplyPreset(g_presetLight);
          g_activePresetIdx = 1;
        }
        ImGui::PopStyleColor(3);
      }

      if (g_hasCustomPreset) {
        ImGui::Dummy(ImVec2(0, 2));
        bool isSel = (g_activePresetIdx == 2);
        if (isSel) {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.18f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.25f));
        } else {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.08f, 0.08f, 0.10f, 0.8f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.12f, 0.12f, 0.15f, 0.9f));
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.06f, 0.08f, 1.0f));
        if (ImGui::Button(isSel ? "Custom  [Active]" : "Custom", ImVec2(-1, 34))) {
          ApplyPreset(g_presetCustom);
          g_activePresetIdx = 2;
        }
        ImGui::PopStyleColor(3);
      }

      ImGui::PopStyleVar(2);
    } else if (mod == 2) {
      SectionHeader("UI Style Presets");
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_SECONDARY);
      ImGui::TextWrapped("Click a preset to apply it immediately.");
      ImGui::PopStyleColor();
      ImGui::Dummy(ImVec2(0, 8));

      ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));

      struct UIPresetEntry { const char* label; int idx; };
      const UIPresetEntry uiPresets[] = {
        { "Vengeance (Default)", 0 },
        { "Vape V4 Style",       1 },
        { "X-Client Style",      2 },
        { "Slinky Style",        3 },
      };
      for (int pi = 0; pi < 4; pi++) {
        bool isSel = (uiPresetsSelected == uiPresets[pi].idx);
        if (isSel) {
          ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.18f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y, UITheme::ACCENT.z, 0.25f));
        } else {
          ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.08f, 0.08f, 0.10f, 0.8f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.12f, 0.12f, 0.15f, 0.9f));
        }
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.06f, 0.08f, 1.0f));

        char btnLabel[64];
        snprintf(btnLabel, sizeof(btnLabel), isSel ? "%s  [Active]" : "%s", uiPresets[pi].label);
        if (ImGui::Button(btnLabel, ImVec2(-1, 34))) {
          uiPresetsSelected = uiPresets[pi].idx;
          g_useCustomUI = (uiPresetsSelected != 0);
          if (uiPresetsSelected == 0) showMenu = false;
          else showMenu = false;
        }
        ImGui::PopStyleColor(3);
        if (pi < 3) ImGui::Dummy(ImVec2(0, 2));
      }

      ImGui::PopStyleVar(2);
    }
  }
}

void RenderModernMenu() {

  if (g_useCustomUI && uiPresetsSelected != 0)
  {
    if (!showMenu && g_menuFadeAnim < 0.01f)
      return;

    ImGuiIO &io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##CustomUI", nullptr,
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoInputs);

    ImDrawList *dl = ImGui::GetWindowDrawList();

    switch (uiPresetsSelected)
    {
      case 1:
        UIVapeV4::RenderUI(nullptr, nullptr, 0);
        break;
      case 2:
        UIXClient::RenderUI(nullptr, nullptr, 0);
        break;
      case 3:
        UISlinky::RenderUI(nullptr, nullptr, 0);
        break;
    }

    ImGui::End();
    ImGui::PopStyleVar();
    return;
  }

  if ((!showMenu && g_menuFadeAnim < 0.01f) || !g_imguiInitialized)
    return;

  ImGuiIO &io = ImGui::GetIO();
  ImVec2 ds = io.DisplaySize;

  if (!g_themeApplied) {
    BuildAllCategory();
    g_themeApplied = true;
  }
  ApplyModernTheme();

  UpdateEnabledModulesList();

  const float WIN_W = 760.0f, WIN_H = 500.0f;
  const float TITLE_H = 38.0f;
  const float TAB_H = 40.0f;
  const float PAD = 14.0f;
  const float CARD_H = 90.0f;
  const float CARD_GAP = 10.0f;
  const float SETTINGS_W = 320.0f;

  float sc = 0.92f + 0.08f * g_menuScaleAnim;
  float scaledW = WIN_W * sc;
  float scaledH = WIN_H * sc;
  ImGui::SetNextWindowPos(ImVec2(ds.x * 0.5f, ds.y * 0.5f), ImGuiCond_Once,
                          ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(scaledW, scaledH));

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleColor(ImGuiCol_WindowBg,
                        ImVec4(0.048f, 0.048f, 0.055f, 0.99f));
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.08f, 0.08f, 0.10f, 0.4f));

  ImGui::Begin("##MainMenu", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

  ImVec2 wp = ImGui::GetWindowPos();
  ImVec2 ws = ImGui::GetWindowSize();
  ImDrawList *dl = ImGui::GetWindowDrawList();

  g_globalTime += io.DeltaTime;
  g_breathPhase += io.DeltaTime * 2.2f;

  if (g_animDots) {
    const int NODE_COUNT = 40;
    const float CONNECT_DIST = 0.12f;
    if (!g_nodesInit) {
      g_netNodes.resize(NODE_COUNT);
      std::mt19937 prng(12345);
      std::uniform_real_distribution<float> d01(0.05f, 0.95f);
      std::uniform_real_distribution<float> dv(-0.008f, 0.008f);
      std::uniform_real_distribution<float> dSz(1.2f, 2.8f);
      for (auto &nd : g_netNodes) {
        nd.x = d01(prng);
        nd.y = d01(prng);
        nd.vx = dv(prng);
        nd.vy = dv(prng);
        nd.sz = dSz(prng);
      }
      g_nodesInit = true;
    }

    for (auto &nd : g_netNodes) {
      nd.x += nd.vx * io.DeltaTime * g_animDotSpeed;
      nd.y += nd.vy * io.DeltaTime * g_animDotSpeed;
      if (nd.x < 0.0f) {
        nd.x = 0.0f;
        nd.vx = fabsf(nd.vx);
      }
      if (nd.x > 1.0f) {
        nd.x = 1.0f;
        nd.vx = -fabsf(nd.vx);
      }
      if (nd.y < 0.0f) {
        nd.y = 0.0f;
        nd.vy = fabsf(nd.vy);
      }
      if (nd.y > 1.0f) {
        nd.y = 1.0f;
        nd.vy = -fabsf(nd.vy);
      }
    }

    float fade = g_menuFadeAnim;
    for (int i = 0; i < NODE_COUNT; i++) {
      float px1 = wp.x + g_netNodes[i].x * ws.x;
      float py1 = wp.y + g_netNodes[i].y * ws.y;
      for (int j = i + 1; j < NODE_COUNT; j++) {
        float dx = g_netNodes[i].x - g_netNodes[j].x;
        float dy = g_netNodes[i].y - g_netNodes[j].y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < CONNECT_DIST) {
          float px2 = wp.x + g_netNodes[j].x * ws.x;
          float py2 = wp.y + g_netNodes[j].y * ws.y;
          float lineA = (1.0f - dist / CONNECT_DIST) * 0.12f * fade * g_animDotOpacity;
          dl->AddLine(ImVec2(px1, py1), ImVec2(px2, py2),
                      ImGui::ColorConvertFloat4ToU32(
                          ImVec4(0.45f, 0.45f, 0.52f, lineA)),
                      1.0f);
        }
      }
    }
    for (int i = 0; i < NODE_COUNT; i++) {
      float px = wp.x + g_netNodes[i].x * ws.x;
      float py = wp.y + g_netNodes[i].y * ws.y;
      float dotA = 0.18f * fade * g_animDotOpacity;
      dl->AddCircleFilled(
          ImVec2(px, py), g_netNodes[i].sz,
          ImGui::ColorConvertFloat4ToU32(ImVec4(0.50f, 0.50f, 0.58f, dotA)), 8);
    }
  }

  if (g_animBreathGlow) {
    float breathAlpha = (0.06f + 0.05f * sinf(g_breathPhase)) * g_animGlowIntensity;
    for (float gb = 4.0f; gb > 0.0f; gb -= 1.0f) {
      float a = breathAlpha * (1.0f - gb / 4.0f) * g_menuFadeAnim;
      dl->AddRect(ImVec2(wp.x - gb, wp.y - gb),
                  ImVec2(wp.x + ws.x + gb, wp.y + ws.y + gb),
                  UITheme::ColorWithAlpha(UITheme::GLOW, a), 10.0f + gb, 0,
                  1.2f);
    }
  }

  dl->AddRectFilled(
      wp, ImVec2(wp.x + ws.x, wp.y + TITLE_H),
      ImGui::ColorConvertFloat4ToU32(ImVec4(0.040f, 0.040f, 0.048f, 1.0f)),
      10.0f, ImDrawFlags_RoundCornersTop);
  dl->AddLine(
      ImVec2(wp.x, wp.y + TITLE_H), ImVec2(wp.x + ws.x, wp.y + TITLE_H),
      ImGui::ColorConvertFloat4ToU32(ImVec4(0.10f, 0.10f, 0.13f, 0.5f)));

  float prevScale = ImGui::GetFont()->Scale;
  ImGui::GetFont()->Scale = 1.3f;
  ImGui::PushFont(ImGui::GetFont());
  ImVec2 brandSz = ImGui::CalcTextSize("Vengeance");
  float brandX = wp.x + PAD;
  float brandY = wp.y + (TITLE_H - brandSz.y) * 0.5f;
  dl->AddText(ImVec2(brandX, brandY),
              ImGui::ColorConvertFloat4ToU32(UITheme::ACCENT), "Vengeance");
  ImGui::PopFont();
  ImGui::GetFont()->Scale = prevScale;

  if (g_loggedInUser[0] != '\0') {
    char welcomeBuf[192];
    snprintf(welcomeBuf, sizeof(welcomeBuf), "Welcome back, %s", g_loggedInUser);
    ImVec2 wSz = ImGui::CalcTextSize(welcomeBuf);
    float wX = wp.x + ws.x - PAD - 20.0f - 10.0f - wSz.x;
    float wY = wp.y + (TITLE_H - wSz.y) * 0.5f;
    dl->AddText(ImVec2(wX, wY),
                ImGui::ColorConvertFloat4ToU32(UITheme::TEXT_SECONDARY), welcomeBuf);
  }

  {
    float btnSz = 20.0f;
    ImVec2 btnPos(wp.x + ws.x - PAD - btnSz, wp.y + (TITLE_H - btnSz) * 0.5f);
    ImGui::SetCursorScreenPos(btnPos);
    ImGui::InvisibleButton("##close", ImVec2(btnSz, btnSz));
    bool hov = ImGui::IsItemHovered();
    if (ImGui::IsItemClicked())
      showMenu = false;
    ImU32 xCol =
        hov ? ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.3f, 0.3f, 1.0f))
            : ImGui::ColorConvertFloat4ToU32(UITheme::TEXT_SECONDARY);
    ImVec2 cc(btnPos.x + btnSz * 0.5f, btnPos.y + btnSz * 0.5f);
    dl->AddLine(ImVec2(cc.x - 5, cc.y - 5), ImVec2(cc.x + 5, cc.y + 5), xCol,
                2.0f);
    dl->AddLine(ImVec2(cc.x + 5, cc.y - 5), ImVec2(cc.x - 5, cc.y + 5), xCol,
                2.0f);
  }

  float tabY = wp.y + TITLE_H;
  dl->AddRectFilled(
      ImVec2(wp.x, tabY), ImVec2(wp.x + ws.x, tabY + TAB_H),
      ImGui::ColorConvertFloat4ToU32(ImVec4(0.038f, 0.038f, 0.045f, 1.0f)));
  dl->AddLine(
      ImVec2(wp.x, tabY + TAB_H), ImVec2(wp.x + ws.x, tabY + TAB_H),
      ImGui::ColorConvertFloat4ToU32(ImVec4(0.10f, 0.10f, 0.13f, 0.5f)));

  auto RenderTab = [&](int ci, float tabX) {
    float target = (ci == g_selectedCategory) ? 1.0f : 0.0f;
    g_tabAnims[ci] += (target - g_tabAnims[ci]) * 0.22f;
    if (fabsf(g_tabAnims[ci] - target) < 0.005f)
      g_tabAnims[ci] = target;
    float anim =
        g_tabAnims[ci] * g_tabAnims[ci] * (3.0f - 2.0f * g_tabAnims[ci]);

    float iconW = 32.0f;
    float textW = ImGui::CalcTextSize(g_categories[ci].name).x + 8.0f;
    float curW = iconW + textW * anim;
    float tH = 30.0f;
    float ty = tabY + (TAB_H - tH) * 0.5f;

    ImGui::SetCursorScreenPos(ImVec2(tabX, ty));
    char tabId[32];
    snprintf(tabId, sizeof(tabId), "##tab%d", ci);
    ImGui::InvisibleButton(tabId, ImVec2(curW, tH));
    bool tabHov = ImGui::IsItemHovered();
    if (ImGui::IsItemClicked()) {
      if (g_selectedCategory != ci) {
        g_cardAppearTimer = 0.0f;
        g_lastCategory = ci;
      }
      g_selectedCategory = ci;
      g_openModuleCat = -1;
      g_openModuleIdx = -1;
    }

    if (anim > 0.05f) {
      dl->AddRectFilled(ImVec2(tabX, ty), ImVec2(tabX + curW, ty + tH),
                        ImGui::ColorConvertFloat4ToU32(
                            ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y,
                                   UITheme::ACCENT.z, 0.12f * anim)),
                        6.0f);
      dl->AddRectFilled(ImVec2(tabX, ty + tH - 2), ImVec2(tabX + curW, ty + tH),
                        ImGui::ColorConvertFloat4ToU32(
                            ImVec4(UITheme::ACCENT.x, UITheme::ACCENT.y,
                                   UITheme::ACCENT.z, 0.8f * anim)),
                        1.0f);
      for (float tg = 4.0f; tg > 0.0f; tg -= 1.0f) {
        float ta = 0.06f * anim * (1.0f - tg / 4.0f);
        dl->AddRectFilled(ImVec2(tabX, ty + tH - 2 - tg),
                          ImVec2(tabX + curW, ty + tH + tg),
                          UITheme::ColorWithAlpha(UITheme::GLOW, ta), 2.0f);
      }
    } else if (tabHov) {
      dl->AddRectFilled(ImVec2(tabX, ty), ImVec2(tabX + curW, ty + tH),
                        ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 0.04f)),
                        6.0f);
    }

    float &iconAnim = g_iconAppearAnim[ci];
    float iconTarget =
        (ci == g_selectedCategory) ? 1.0f : (tabHov ? 0.7f : 0.4f);
    iconAnim += (iconTarget - iconAnim) * 0.18f;
    float iconScale = 0.6f + 0.4f * iconAnim;
    ImVec4 iconColV =
        UITheme::Lerp(UITheme::TEXT_DISABLED, UITheme::ACCENT, iconAnim);
    ImU32 iconCol = ImGui::ColorConvertFloat4ToU32(iconColV);

    bool iconRight = (ci == g_appearanceCatIdx);
    if (iconRight) {

      if (anim > 0.05f) {
        ImVec4 tc = UITheme::TEXT_PRIMARY;
        tc.w = anim;
        float textY = ty + (tH - ImGui::GetTextLineHeight()) * 0.5f;
        dl->AddText(ImVec2(tabX + 4.0f, textY),
                    ImGui::ColorConvertFloat4ToU32(tc), g_categories[ci].name);
      }
      float iconX = tabX + textW * anim + 4.0f + iconW * 0.5f - 4.0f;
      ImVec2 iconC(iconX, ty + tH * 0.5f);
      ImVec2 scaledC(iconC.x, iconC.y + (1.0f - iconScale) * 3.0f);
      DrawCatIcon(dl, scaledC, ci, iconCol);
    } else {

      ImVec2 iconC(tabX + iconW * 0.5f, ty + tH * 0.5f);
      ImVec2 scaledC(iconC.x, iconC.y + (1.0f - iconScale) * 3.0f);
      DrawCatIcon(dl, scaledC, ci, iconCol);
      if (anim > 0.05f) {
        ImVec4 tc = UITheme::TEXT_PRIMARY;
        tc.w = anim;
        float textY = ty + (tH - ImGui::GetTextLineHeight()) * 0.5f;
        dl->AddText(ImVec2(tabX + iconW, textY),
                    ImGui::ColorConvertFloat4ToU32(tc), g_categories[ci].name);
      }
    }

    return curW;
  };

  float tabX = wp.x + PAD;
  for (int ci = 0; ci < (int)g_categories.size(); ci++) {
    if (ci == g_appearanceCatIdx) continue;
    float curW = RenderTab(ci, tabX);
    tabX += curW + 6.0f;
  }

  {
    int ci = g_appearanceCatIdx;
    float iconW = 32.0f;
    float textW = ImGui::CalcTextSize(g_categories[ci].name).x + 8.0f;
    float maxW = iconW + textW;
    float rightX = wp.x + ws.x - PAD - maxW;
    RenderTab(ci, rightX);
  }

  float contentY = tabY + TAB_H;
  float contentH = ws.y - TITLE_H - TAB_H;

  bool settingsOpen = (g_openModuleCat >= 0 && g_openModuleIdx >= 0);
  float sTarget = settingsOpen ? 1.0f : 0.0f;
  float sLambda = settingsOpen ? 10.0f : 14.0f;
  g_settingsPanelAnim += (sTarget - g_settingsPanelAnim) * (1.0f - expf(-sLambda * io.DeltaTime));
  if (fabsf(g_settingsPanelAnim - sTarget) < 0.005f)
    g_settingsPanelAnim = sTarget;
  if (!settingsOpen && g_settingsPanelAnim < 0.01f)
    g_settingsPanelAnim = 0.0f;

  float settingsSlide = SETTINGS_W * g_settingsPanelAnim;
  float cardsAreaW = ws.x - settingsSlide;

  ImGui::SetCursorScreenPos(ImVec2(wp.x, contentY));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(PAD, PAD));
  ImGui::BeginChild("##Cards", ImVec2(cardsAreaW, contentH), false,
                    ImGuiWindowFlags_AlwaysUseWindowPadding);
  ImGui::PopStyleVar();

  auto &cat = g_categories[g_selectedCategory];
  if (cat.modules.empty()) {
    ImGui::Dummy(ImVec2(0, contentH * 0.35f));
    const char *emptyMsg = "No modules in this category";
    float tw = ImGui::CalcTextSize(emptyMsg).x;
    ImGui::SetCursorPosX((cardsAreaW - tw) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_DISABLED);
    ImGui::Text("%s", emptyMsg);
    ImGui::PopStyleColor();
  } else {
    int cols = 2;
    float availW = cardsAreaW - PAD * 2;
    float cardW = (availW - CARD_GAP * (cols - 1)) / cols;

    for (int mi = 0; mi < (int)cat.modules.size(); mi++) {
      int col = mi % cols;
      if (col > 0)
        ImGui::SameLine(0, CARD_GAP);

      float cardDelay = mi * 0.07f;
      float cardRaw = (g_cardAppearTimer - cardDelay) / 0.30f;
      if (cardRaw < 0.0f)
        cardRaw = 0.0f;
      if (cardRaw > 1.0f)
        cardRaw = 1.0f;

      float t1 = 1.0f - cardRaw;
      float springT = 1.0f - t1 * t1 * t1 * t1 * t1;
      float bounce = sinf(cardRaw * 3.14159f * 2.0f) * 0.08f * (1.0f - cardRaw) * (1.0f - cardRaw);
      float cardAlpha = springT;
      float cardSlideY = (1.0f - springT + bounce) * 30.0f;

      ImGui::PushID(mi);
      ImVec2 cardPos = ImGui::GetCursorScreenPos();
      cardPos.y += cardSlideY;

      ImGui::InvisibleButton("##card", ImVec2(cardW, CARD_H));
      bool cardHov = ImGui::IsItemHovered();
      bool cardClk = ImGui::IsItemClicked();

      int cardKey = g_selectedCategory * 100 + mi;
      float &pressAnim = g_cardPressAnim[cardKey];
      float &rippleAnim = g_cardRippleAnim[cardKey];
      if (cardClk) {
        pressAnim = 1.0f;
        rippleAnim = 0.0f;
        g_cardRipplePos[cardKey] = ImGui::GetIO().MousePos;

        int clickCat = g_selectedCategory;
        int clickMod = mi;
        if (g_selectedCategory == 0 && mi < (int)g_allModuleMap.size()) {
          clickCat = g_allModuleMap[mi].realCat;
          clickMod = g_allModuleMap[mi].realMod;
        }
        if (g_openModuleCat == clickCat && g_openModuleIdx == clickMod) {
          g_openModuleCat = -1;
          g_openModuleIdx = -1;
        } else {
          g_openModuleCat = clickCat;
          g_openModuleIdx = clickMod;
        }
      }

      float cdt = io.DeltaTime;
      pressAnim *= expf(-18.0f * cdt);
      if (pressAnim < 0.01f) pressAnim = 0.0f;

      rippleAnim += cdt * 3.5f;
      if (rippleAnim > 1.0f) rippleAnim = 1.0f;

      int checkCat = g_selectedCategory;
      int checkMod = mi;
      if (g_selectedCategory == 0 && mi < (int)g_allModuleMap.size()) {
        checkCat = g_allModuleMap[mi].realCat;
        checkMod = g_allModuleMap[mi].realMod;
      }
      bool isOpen = (g_openModuleCat == checkCat && g_openModuleIdx == checkMod);

      float pressScale = 1.0f - pressAnim * 0.015f;
      float cw = cardW * pressScale;
      float ch = CARD_H * pressScale;
      float offsetX = (cardW - cw) * 0.5f;
      float offsetY = (CARD_H - ch) * 0.5f;
      ImVec2 scaledCardPos(cardPos.x + offsetX, cardPos.y + offsetY);
      ImVec2 scaledCardEnd(scaledCardPos.x + cw, scaledCardPos.y + ch);

      float &hGlow = g_cardHoverGlow[cardKey];
      float hTarget = cardHov ? 1.0f : 0.0f;
      hGlow += (hTarget - hGlow) * (1.0f - expf(-14.0f * cdt));
      if (fabsf(hGlow - hTarget) < 0.005f) hGlow = hTarget;

      ImVec4 cardBg = ImVec4(
          0.058f + 0.014f * hGlow + (isOpen ? 0.007f : 0.0f),
          0.058f + 0.014f * hGlow + (isOpen ? 0.007f : 0.0f),
          0.068f + 0.017f * hGlow + (isOpen ? 0.012f : 0.0f),
          (0.96f + 0.04f * hGlow) * cardAlpha);

      ImDrawList *cdl = ImGui::GetWindowDrawList();
      cdl->AddRectFilled(scaledCardPos, scaledCardEnd,
                         ImGui::ColorConvertFloat4ToU32(cardBg), 8.0f);

      if (rippleAnim > 0.0f && rippleAnim < 1.0f) {
        ImVec2 rPos = g_cardRipplePos[cardKey];
        float maxR = sqrtf(cw * cw + ch * ch) * 0.6f;
        float curR = maxR * rippleAnim;
        float rippleAlpha = (1.0f - rippleAnim) * 0.12f * cardAlpha;
        cdl->PushClipRect(scaledCardPos, scaledCardEnd, true);
        cdl->AddCircleFilled(rPos, curR,
            UITheme::ColorWithAlpha(UITheme::ACCENT, rippleAlpha), 32);
        cdl->PopClipRect();
      }

      if (hGlow > 0.02f) {
        for (float hg = 6.0f; hg > 0.0f; hg -= 1.5f) {
          float ha = 0.03f * hGlow * cardAlpha * (1.0f - hg / 6.0f);
          cdl->AddRect(ImVec2(scaledCardPos.x - hg, scaledCardPos.y - hg),
                       ImVec2(scaledCardEnd.x + hg, scaledCardEnd.y + hg),
                       UITheme::ColorWithAlpha(UITheme::GLOW, ha),
                       8.0f + hg * 0.5f, 0, 1.0f);
        }
      }

      float borderAccent = isOpen ? 0.45f : (hGlow * 0.20f);
      ImVec4 bCol = ImVec4(
          0.08f + (UITheme::ACCENT.x - 0.08f) * borderAccent,
          0.08f + (UITheme::ACCENT.y - 0.08f) * borderAccent,
          0.11f + (UITheme::ACCENT.z - 0.11f) * borderAccent,
          (0.35f + 0.20f * hGlow + (isOpen ? 0.10f : 0.0f)) * cardAlpha);
      cdl->AddRect(scaledCardPos, scaledCardEnd, ImGui::ColorConvertFloat4ToU32(bCol), 8.0f,
                   0, 1.0f);

      if (isOpen && cardAlpha > 0.1f) {
        for (float og = 5.0f; og > 0.0f; og -= 1.5f) {
          float oa = 0.04f * cardAlpha * (1.0f - og / 5.0f);
          cdl->AddRect(ImVec2(scaledCardPos.x - og, scaledCardPos.y - og),
                       ImVec2(scaledCardEnd.x + og, scaledCardEnd.y + og),
                       UITheme::ColorWithAlpha(UITheme::ACCENT, oa), 8.0f + og,
                       0, 1.0f);
        }
      }

      float tp = 12.0f;
      ImVec4 nameCol = UITheme::TEXT_PRIMARY;
      nameCol.w *= cardAlpha;
      cdl->AddText(ImVec2(cardPos.x + tp, cardPos.y + tp),
                   ImGui::ColorConvertFloat4ToU32(nameCol),
                   cat.modules[mi].name);

      ImVec4 descCol = UITheme::TEXT_SECONDARY;
      descCol.w *= cardAlpha;
      cdl->AddText(ImVec2(cardPos.x + tp,
                          cardPos.y + tp + ImGui::GetTextLineHeight() + 4),
                   ImGui::ColorConvertFloat4ToU32(descCol),
                   cat.modules[mi].desc);

      bool *enabled = GetModuleEnabledPtr(checkCat, checkMod);
      if (enabled) {
        float dotR = 4.0f;
        ImVec2 dotP(scaledCardEnd.x - tp - dotR, cardPos.y + tp + dotR);

        float &dotAnim = g_dotEnabledAnim[cardKey];
        float dotTarget = *enabled ? 1.0f : 0.0f;
        dotAnim += (dotTarget - dotAnim) * (1.0f - expf(-10.0f * cdt));
        if (fabsf(dotAnim - dotTarget) < 0.005f) dotAnim = dotTarget;

        float pulse = 0.0f;
        if (dotAnim > 0.01f) {
          pulse = sinf(g_globalTime * 2.5f) * 0.3f + 0.7f;
        }

        if (dotAnim > 0.01f) {

          cdl->AddCircleFilled(
              dotP, dotR * (0.6f + 0.4f * dotAnim),
              UITheme::ColorWithAlpha(UITheme::ACCENT, dotAnim * cardAlpha), 12);

          float glowR = dotR + 2.0f + pulse * 2.0f;
          cdl->AddCircleFilled(
              dotP, glowR,
              UITheme::ColorWithAlpha(UITheme::ACCENT, 0.12f * dotAnim * cardAlpha), 12);
        }
        if (dotAnim < 0.99f) {

          ImVec4 disCol = UITheme::TEXT_DISABLED;
          disCol.w *= (1.0f - dotAnim) * cardAlpha;
          cdl->AddCircle(dotP, dotR, ImGui::ColorConvertFloat4ToU32(disCol), 12,
                         1.5f);
        }
      }

      static char kbBuf[32];
      const char *kbLabel = "NONE";
      {
        bool *modPtr = GetModuleEnabledPtr(checkCat, checkMod);
        if (modPtr) {
          for (auto &kb : g_keybinds) {
            if (kb.targetVariable == modPtr && kb.virtualKey != 0) {
              std::string ks = VirtualKeyToString(kb.virtualKey);
              snprintf(kbBuf, sizeof(kbBuf), "%s", ks.c_str());
              kbLabel = kbBuf;
              break;
            }
          }
        }
      }
      float kbY = scaledCardEnd.y - ImGui::GetTextLineHeight() - 10.0f;
      ImVec2 kbPos(cardPos.x + tp, kbY);
      ImVec2 kbSz = ImGui::CalcTextSize(kbLabel);
      ImVec2 kbPad(6, 3);
      cdl->AddRectFilled(
          ImVec2(kbPos.x - kbPad.x, kbPos.y - kbPad.y),
          ImVec2(kbPos.x + kbSz.x + kbPad.x, kbPos.y + kbSz.y + kbPad.y),
          ImGui::ColorConvertFloat4ToU32(
              ImVec4(0.06f, 0.06f, 0.08f, 0.7f * cardAlpha)),
          4.0f);
      cdl->AddRect(
          ImVec2(kbPos.x - kbPad.x, kbPos.y - kbPad.y),
          ImVec2(kbPos.x + kbSz.x + kbPad.x, kbPos.y + kbSz.y + kbPad.y),
          ImGui::ColorConvertFloat4ToU32(
              ImVec4(0.12f, 0.12f, 0.16f, 0.4f * cardAlpha)),
          4.0f, 0, 1.0f);
      ImVec4 kbCol = UITheme::TEXT_DISABLED;
      kbCol.w *= cardAlpha;
      cdl->AddText(kbPos, ImGui::ColorConvertFloat4ToU32(kbCol), kbLabel);

      ImGui::PopID();

      if (col == cols - 1 || mi == (int)cat.modules.size() - 1) {
        ImGui::Dummy(ImVec2(0, CARD_GAP));
      }
    }
  }

  ImGui::EndChild();

  if (g_settingsPanelAnim > 0.01f) {
    float panelX = wp.x + ws.x - settingsSlide;

    dl->AddRectFilled(
        ImVec2(panelX, contentY), ImVec2(wp.x + ws.x, wp.y + ws.y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.042f, 0.042f, 0.050f, 0.99f)),
        0.0f);

    dl->AddLine(ImVec2(panelX, contentY), ImVec2(panelX, wp.y + ws.y),
                ImGui::ColorConvertFloat4ToU32(
                    ImVec4(0.08f, 0.08f, 0.10f, 0.5f * g_settingsPanelAnim)));

    for (float lg = 4.0f; lg > 0.0f; lg -= 1.0f) {
      float la = 0.015f * g_settingsPanelAnim * (1.0f - lg / 4.0f);
      dl->AddLine(ImVec2(panelX - lg, contentY),
                  ImVec2(panelX - lg, wp.y + ws.y),
                  UITheme::ColorWithAlpha(UITheme::GLOW, la), 1.0f);
    }

    ImGui::SetCursorScreenPos(ImVec2(panelX + 10, contentY + 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 6));
    ImGui::BeginChild("##SettingsPanel",
                      ImVec2(settingsSlide - 12, contentH - 10), false,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::PopStyleVar();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.06f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.03f));
    ImGui::PushStyleColor(ImGuiCol_Text, UITheme::ACCENT);
    if (ImGui::Button("<-##back", ImVec2(30, 24))) {
      g_openModuleCat = -1;
      g_openModuleIdx = -1;
    }
    ImGui::PopStyleColor(4);

    ImGui::SameLine();
    if (g_openModuleCat >= 0 && g_openModuleIdx >= 0 &&
        g_openModuleCat < (int)g_categories.size() &&
        g_openModuleIdx < (int)g_categories[g_openModuleCat].modules.size()) {
      ImGui::PushStyleColor(ImGuiCol_Text, UITheme::TEXT_PRIMARY);
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
      ImGui::Text("%s",
                  g_categories[g_openModuleCat].modules[g_openModuleIdx].name);
      ImGui::PopStyleColor();
    }

    ImGui::Dummy(ImVec2(0, 4));
    ImVec2 sepP = ImGui::GetCursorScreenPos();
    float sepW = ImGui::GetContentRegionAvail().x;
    ImGui::GetWindowDrawList()->AddLine(
        sepP, ImVec2(sepP.x + sepW, sepP.y),
        ImGui::ColorConvertFloat4ToU32(UITheme::SEPARATOR));
    ImGui::Dummy(ImVec2(0, 8));

    RenderModuleSettings(g_openModuleCat, g_openModuleIdx);

    ImGui::EndChild();
  }

  ImGui::End();
  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar(3);
}

BOOL __stdcall mySwapBuffers(HDC hDC) {
  if (g_isUnloading) {
    glDepthFunc(GL_LEQUAL);
    return pSwapBuffers(hDC);
  }

  if (!hDC) {
    return FALSE;
  }

  static bool initialized = false;
  if (!initialized) {
    InitializeKeyBinds();
    initialized = true;
  }

  extern volatile DWORD g_renderFrameIndex;
  g_renderFrameIndex++;

  HWND window = WindowFromDC(hDC);

  if (!window || !IsWindow(window)) {
    return pSwapBuffers(hDC);
  }

  g_targetWindow = window;

  ReinitializeImGuiIfNeeded(window);
  g_fullscreenDetected = false;

  static bool insertPressed = false;
  if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
    if (!insertPressed) {
      showMenu = !showMenu;
      if (showMenu) {
        g_menuScaleAnim = 0.0f;
        g_cardAppearTimer = 0.0f;
      } else {

        g_cursorVisible = false;
        g_isInventoryOpen = false;
      }
      insertPressed = true;
    }
  } else {
    insertPressed = false;
  }

  if (g_sprintPending && sprintEnabled && g_jniReady && g_jvm &&
      !g_isUnloading) {
    JNIEnv *env = nullptr;
    if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        if (player) {
          if (g_fIsSprinting) {
            env->SetBooleanField(player, g_fIsSprinting, JNI_TRUE);
            if (env->ExceptionCheck())
              env->ExceptionClear();
          } else if (g_mSetSprinting) {
            env->CallVoidMethod(player, g_mSetSprinting, (jboolean)JNI_TRUE);
            if (env->ExceptionCheck())
              env->ExceptionClear();
          }
          env->DeleteLocalRef(player);
        }
        env->DeleteLocalRef(mc);
      } else {
        if (env->ExceptionCheck())
          env->ExceptionClear();
      }
    }
  }

  if (noFallEnabled && g_jniReady && g_jvm && !g_isUnloading && g_fFallDistance) {
    static DWORD lastNoFallSpoof = 0;
    JNIEnv *env = nullptr;
    if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        if (player) {
          float fd = env->GetFloatField(player, g_fFallDistance);
          if (env->ExceptionCheck()) {
            env->ExceptionClear();
            fd = 0.0f;
          }

          env->SetFloatField(player, g_fFallDistance, 0.0f);
          if (env->ExceptionCheck())
            env->ExceptionClear();

          bool onGround = false;
          if (g_fOnGround) {
            onGround = env->GetBooleanField(player, g_fOnGround) == JNI_TRUE;
            if (env->ExceptionCheck()) { env->ExceptionClear(); onGround = false; }
          }
          double motY = 0.0;
          if (g_fMotionY) {
            motY = env->GetDoubleField(player, g_fMotionY);
            if (env->ExceptionCheck()) { env->ExceptionClear(); motY = 0.0; }
          }

          if (!onGround && (fd > 1.75f || motY < -0.35)) {

            if (g_noFallJniReady) {
              DWORD now = (DWORD)GetTickCount64();
              if (now - lastNoFallSpoof >= 25) {
                jobject sendQueue = env->GetObjectField(player, g_fSendQueue);
                if (sendQueue && !env->ExceptionCheck()) {
                  jobject packet =
                      env->NewObject(g_c03PacketClass, g_c03PacketInit,
                                     (jboolean)JNI_TRUE);
                  if (!packet || env->ExceptionCheck()) {
                    if (env->ExceptionCheck()) env->ExceptionClear();
                    if (g_c03PosClass && g_c03PosInit) {
                      double px = env->GetDoubleField(player, g_fPosX);
                      double py = env->GetDoubleField(player, g_fPosY);
                      double pz = env->GetDoubleField(player, g_fPosZ);
                      if (env->ExceptionCheck()) {
                        env->ExceptionClear();
                        px = py = pz = 0.0;
                      }
                      packet = env->NewObject(g_c03PosClass, g_c03PosInit,
                                              (jdouble)px, (jdouble)py,
                                              (jdouble)pz, (jboolean)JNI_TRUE);
                      if (env->ExceptionCheck()) {
                        env->ExceptionClear();
                        packet = nullptr;
                      }
                    }
                  }
                  if (packet) {
                    env->CallVoidMethod(sendQueue, g_mAddToSendQueue, packet);
                    if (env->ExceptionCheck())
                      env->ExceptionClear();
                    env->DeleteLocalRef(packet);
                    lastNoFallSpoof = now;
                  } else if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                  }
                  env->DeleteLocalRef(sendQueue);
                } else if (env->ExceptionCheck()) {
                  env->ExceptionClear();
                }
              }
            }
          }
          env->DeleteLocalRef(player);
        }
        env->DeleteLocalRef(mc);
      } else {
        if (env && env->ExceptionCheck()) env->ExceptionClear();
      }
    }
  }

  if (jumpResetEnabled && g_jniReady && g_jvm && !g_isUnloading &&
      g_fOnGround && g_fMotionY && g_fMotionX) {
    static double prevMotY = 0;
    static bool wasOnGround = true;
    static bool kbDetected = false;
    static bool jumpSent = false;
    static int jumpReleaseTick = 0;

    JNIEnv *env = nullptr;
    if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        if (player) {
          bool onGround = env->GetBooleanField(player, g_fOnGround) == JNI_TRUE;
          if (env->ExceptionCheck()) { env->ExceptionClear(); onGround = false; }
          double motY = env->GetDoubleField(player, g_fMotionY);
          if (env->ExceptionCheck()) { env->ExceptionClear(); motY = 0; }
          double motX = env->GetDoubleField(player, g_fMotionX);
          if (env->ExceptionCheck()) { env->ExceptionClear(); motX = 0; }
          double motZ = env->GetDoubleField(player, g_fMotionZ);
          if (env->ExceptionCheck()) { env->ExceptionClear(); motZ = 0; }

          double horizSpeed = sqrt(motX * motX + motZ * motZ);
          int hurtTime = 0;
          if (g_fHurtTime) {
            hurtTime = env->GetIntField(player, g_fHurtTime);
            if (env->ExceptionCheck()) { env->ExceptionClear(); hurtTime = 0; }
          }

          if (wasOnGround && motY > 0.1 && horizSpeed > 0.05 && !kbDetected && !jumpSent &&
              (!jumpResetOnlyWhenHurt || hurtTime > 0)) {
            int roll = rand() % 100;
            if (roll < jumpResetChance)
              kbDetected = true;
          }

          if (jumpSent) {
            jumpReleaseTick++;
            if (jumpReleaseTick >= 2) {
              if (g_fGameSettings && g_fKeyBindJump && g_fKeyBindPressed) {
                jobject gs = env->GetObjectField(mc, g_fGameSettings);
                if (gs && !env->ExceptionCheck()) {
                  jobject jumpKb = env->GetObjectField(gs, g_fKeyBindJump);
                  if (jumpKb && !env->ExceptionCheck()) {
                    bool realKey = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
                    env->SetBooleanField(jumpKb, g_fKeyBindPressed, realKey ? JNI_TRUE : JNI_FALSE);
                    if (env->ExceptionCheck()) env->ExceptionClear();
                    env->DeleteLocalRef(jumpKb);
                  }
                  env->DeleteLocalRef(gs);
                }
              }
              jumpSent = false;
              kbDetected = false;
            }
          }

          if (kbDetected && onGround && !jumpSent) {
            if (g_fGameSettings && g_fKeyBindJump && g_fKeyBindPressed) {
              jobject gs = env->GetObjectField(mc, g_fGameSettings);
              if (gs && !env->ExceptionCheck()) {
                jobject jumpKb = env->GetObjectField(gs, g_fKeyBindJump);
                if (jumpKb && !env->ExceptionCheck()) {
                  env->SetBooleanField(jumpKb, g_fKeyBindPressed, JNI_TRUE);
                  if (env->ExceptionCheck()) env->ExceptionClear();
                  env->DeleteLocalRef(jumpKb);
                }
                env->DeleteLocalRef(gs);
              }
            }
            jumpSent = true;
            jumpReleaseTick = 0;
          }

          wasOnGround = onGround;
          prevMotY = motY;
          env->DeleteLocalRef(player);
        }
        env->DeleteLocalRef(mc);
      } else {
        if (env && env->ExceptionCheck()) env->ExceptionClear();
      }
    }
  }

  if (invWalkEnabled && g_cursorVisible && !showMenu && !g_isUnloading &&
      g_invWalkJniReady && g_jniReady && g_jvm) {
    JNIEnv *iwEnv = nullptr;
    if (g_jvm->GetEnv((void **)&iwEnv, JNI_VERSION_1_8) == JNI_OK && iwEnv) {
      jobject mc = iwEnv->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !iwEnv->ExceptionCheck()) {
        jobject gs = iwEnv->GetObjectField(mc, g_fGameSettings);
        if (gs && !iwEnv->ExceptionCheck()) {

          struct { jfieldID field; int vk; } bindings[] = {
            {g_fKeyBindForward, 'W'},
            {g_fKeyBindBack, 'S'},
            {g_fKeyBindLeft, 'A'},
            {g_fKeyBindRight, 'D'},
            {g_fKeyBindJump, VK_SPACE},
            {g_fKeyBindSneak, VK_LSHIFT},
          };
          for (auto &b : bindings) {
            if (!b.field) continue;
            jobject kb = iwEnv->GetObjectField(gs, b.field);
            if (kb && !iwEnv->ExceptionCheck()) {
              bool keyHeld = (GetAsyncKeyState(b.vk) & 0x8000) != 0;
              iwEnv->SetBooleanField(kb, g_fKeyBindPressed, keyHeld ? JNI_TRUE : JNI_FALSE);
              if (iwEnv->ExceptionCheck()) iwEnv->ExceptionClear();
              iwEnv->DeleteLocalRef(kb);
            } else {
              if (iwEnv->ExceptionCheck()) iwEnv->ExceptionClear();
            }
          }
          iwEnv->DeleteLocalRef(gs);
        } else {
          if (iwEnv->ExceptionCheck()) iwEnv->ExceptionClear();
        }
        iwEnv->DeleteLocalRef(mc);
      } else {
        if (iwEnv && iwEnv->ExceptionCheck()) iwEnv->ExceptionClear();
      }
    }
  }

  if (noJumpDelayEnabled && g_jniReady && g_jvm && !g_isUnloading && g_fJumpTicks) {
    JNIEnv *env = nullptr;
    if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        bool jumpHeld = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
        if (player && jumpHeld) {
          env->SetIntField(player, g_fJumpTicks, 0);
          if (env->ExceptionCheck()) env->ExceptionClear();
          env->DeleteLocalRef(player);
        } else if (player) {
          env->DeleteLocalRef(player);
        }
        env->DeleteLocalRef(mc);
      } else {
        if (env && env->ExceptionCheck()) env->ExceptionClear();
      }
    }
  }

  if (g_jniReady && g_jvm && !g_isUnloading && g_fStepHeight) {
    JNIEnv *env = nullptr;
    if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        if (player) {
          if (stepEnabled) {
            env->SetFloatField(player, g_fStepHeight, stepHeight);

            static int stepDebugCounter = 0;
            if (++stepDebugCounter % 60 == 0) {
              float currentStep = env->GetFloatField(player, g_fStepHeight);
              printf("[STEP DEBUG] Enabled: stepHeight=%.2f, current=%.2f\n", stepHeight, currentStep);
            }
            stepStatus = "Active";
          } else {

            float current = env->GetFloatField(player, g_fStepHeight);
            if (env->ExceptionCheck()) { env->ExceptionClear(); current = 0.625f; }
            if (current > 0.625f) {
              env->SetFloatField(player, g_fStepHeight, 0.625f);

              printf("[STEP DEBUG] Reset stepHeight from %.2f to 0.625f\n", current);
            }
            stepStatus = "Disabled";
          }
          if (env->ExceptionCheck()) env->ExceptionClear();
          env->DeleteLocalRef(player);
        } else {
          printf("[STEP DEBUG] Failed to get player object\n");
          stepStatus = "No player";
        }
        env->DeleteLocalRef(mc);
      } else {
        printf("[STEP DEBUG] Failed to get Minecraft object\n");
        stepStatus = "No Minecraft";
      }
    } else {
      printf("[STEP DEBUG] JNI not ready or failed to get env\n");
      stepStatus = "JNI not ready";
    }
  } else {
    static int stepMissingDebugCounter = 0;
    if (++stepMissingDebugCounter % 300 == 0) {
      printf("[STEP DEBUG] Missing: g_jniReady=%s, g_jvm=%p, g_fStepHeight=%p\n",
             g_jniReady ? "true" : "false", g_jvm, g_fStepHeight);
    }
    if (!g_jniReady) stepStatus = "JNI not ready";
    else if (!g_jvm) stepStatus = "No JVM";
    else if (!g_fStepHeight) stepStatus = "Field not found";
    else stepStatus = "Unloading";
  }

  if (sTapEnabled && sTapActive) {
    int sTapAction = STap_OnTick();
    if (sTapAction == 1 || sTapAction == 2) {
      JNIEnv *env = nullptr;
      if (g_jvm && g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env && g_jniReady) {
        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (mc && !env->ExceptionCheck()) {
          jobject gameSettings = env->GetObjectField(mc, g_fGameSettings);
          if (gameSettings && !env->ExceptionCheck() && g_fKeyBindSprint && g_fKeyBindPressed) {
            jobject keyBindSprint = env->GetObjectField(gameSettings, g_fKeyBindSprint);
            if (keyBindSprint && !env->ExceptionCheck()) {
              env->SetBooleanField(keyBindSprint, g_fKeyBindPressed, sTapAction == 2 ? JNI_TRUE : JNI_FALSE);
              if (env->ExceptionCheck()) env->ExceptionClear();
              env->DeleteLocalRef(keyBindSprint);
            }
            env->DeleteLocalRef(gameSettings);
          }
          env->DeleteLocalRef(mc);
        }
      }
      if (sTapAction == 2) sTapActive = false;
    }
    if (!g_sTapModule.is_active()) sTapActive = false;
  }

  if (wTapEnabled && wTapActive) {
    int wTapAction = WTap_OnTick();
    if (wTapAction == 1 || wTapAction == 2) {
      JNIEnv *env = nullptr;
      if (g_jvm && g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env && g_jniReady) {
        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (mc && !env->ExceptionCheck()) {
          jobject gameSettings = env->GetObjectField(mc, g_fGameSettings);
          if (gameSettings && !env->ExceptionCheck() && g_fKeyBindForward && g_fKeyBindPressed) {
            jobject keyBindForward = env->GetObjectField(gameSettings, g_fKeyBindForward);
            if (keyBindForward && !env->ExceptionCheck()) {
              env->SetBooleanField(keyBindForward, g_fKeyBindPressed, wTapAction == 2 ? JNI_TRUE : JNI_FALSE);
              if (env->ExceptionCheck()) env->ExceptionClear();
              env->DeleteLocalRef(keyBindForward);
            }
            env->DeleteLocalRef(gameSettings);
          }
          env->DeleteLocalRef(mc);
        }
      }
      if (wTapAction == 2) wTapActive = false;
    }
    if (!g_wTapModule.is_active()) wTapActive = false;
  }

  if (bowAimbotEnabled && g_jniReady && g_jvm && !g_isUnloading && !g_cursorVisible) {
    JNIEnv *env = nullptr;
    if (g_jvm && g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        jobject world = env->GetObjectField(mc, g_fTheWorld);
        if (player && world && !env->ExceptionCheck()) {

          bool holdingBow = false;
          bool usingItem = false;
          int chargeTicks = 0;
          if (g_mGetCurrentItem && g_mStackGetItem) {
            jobject stack = env->CallObjectMethod(player, g_mGetCurrentItem);
            if (stack && !env->ExceptionCheck()) {
              jobject item = env->CallObjectMethod(stack, g_mStackGetItem);
              if (item && !env->ExceptionCheck()) {
                jclass bowClass = env->FindClass("net/minecraft/item/ItemBow");
                if (bowClass && !env->ExceptionCheck()) {
                  holdingBow = env->IsInstanceOf(item, bowClass);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); holdingBow = false; }
                  env->DeleteLocalRef(bowClass);
                }
                env->DeleteLocalRef(item);
              } else if (env->ExceptionCheck()) env->ExceptionClear();
              env->DeleteLocalRef(stack);
            } else if (env->ExceptionCheck()) env->ExceptionClear();
          }

          if (holdingBow) {
            jclass plClass = env->GetObjectClass(player);
            if (plClass) {
              jmethodID mIsUsing = env->GetMethodID(plClass, "func_71039_bw", "()Z");
              if (!mIsUsing || env->ExceptionCheck()) { env->ExceptionClear(); mIsUsing = env->GetMethodID(plClass, "isUsingItem", "()Z"); }
              if (mIsUsing && !env->ExceptionCheck()) {
                usingItem = env->CallBooleanMethod(player, mIsUsing) == JNI_TRUE;
                if (env->ExceptionCheck()) { env->ExceptionClear(); usingItem = false; }
              }
              if (usingItem) {
                jmethodID mGetDuration = env->GetMethodID(plClass, "func_71057_bx", "()I");
                if (!mGetDuration || env->ExceptionCheck()) { env->ExceptionClear(); mGetDuration = env->GetMethodID(plClass, "getItemInUseDuration", "()I"); }
                if (mGetDuration && !env->ExceptionCheck()) {
                  chargeTicks = (int)env->CallIntMethod(player, mGetDuration);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); chargeTicks = 0; }
                }
              }
              env->DeleteLocalRef(plClass);
            }
          }

          if (holdingBow && usingItem && chargeTicks > 0) {

            double px = env->GetDoubleField(player, g_fPosX);
            double py = env->GetDoubleField(player, g_fPosY);
            double pz = env->GetDoubleField(player, g_fPosZ);
            float eyeH = env->CallFloatMethod(player, g_mGetEyeHeight);
            float pYaw = env->GetFloatField(player, g_fRotYaw);
            float pPitch = env->GetFloatField(player, g_fRotPitch);
            if (env->ExceptionCheck()) env->ExceptionClear();

            jobject entList = env->GetObjectField(world, g_fEntityList);
            if (entList && !env->ExceptionCheck()) {
              jint count = env->CallIntMethod(entList, g_mListSize);
              if (env->ExceptionCheck()) { env->ExceptionClear(); count = 0; }

              jobject bestBowTarget = nullptr;
              double bestBowScore = 1e9;

              for (jint i = 0; i < count && i < 200; i++) {
                jobject ent = env->CallObjectMethod(entList, g_mListGet, i);
                if (!ent || env->ExceptionCheck()) { if (env->ExceptionCheck()) env->ExceptionClear(); continue; }
                if (env->IsSameObject(ent, player)) { env->DeleteLocalRef(ent); continue; }

                jboolean dead = env->GetBooleanField(ent, g_fIsDead);
                if (env->ExceptionCheck()) { env->ExceptionClear(); env->DeleteLocalRef(ent); continue; }
                if (dead) { env->DeleteLocalRef(ent); continue; }

                double ex = env->GetDoubleField(ent, g_fPosX);
                double ey = env->GetDoubleField(ent, g_fPosY);
                double ez = env->GetDoubleField(ent, g_fPosZ);
                if (env->ExceptionCheck()) { env->ExceptionClear(); env->DeleteLocalRef(ent); continue; }

                double ddx = ex - px, ddz = ez - pz, ddy = ey - py;
                double dist = sqrt(ddx*ddx + ddy*ddy + ddz*ddz);
                if (dist > bowAimbotConfig.range || dist < 1.0) { env->DeleteLocalRef(ent); continue; }

                if (!BowAimbot_InFov(pYaw, px, pz, ex, ez)) { env->DeleteLocalRef(ent); continue; }

                double score = dist;
                if (bowAimbotConfig.filterMode == 0) {
                  float tYaw = (float)(atan2(ddz, ddx) * 180.0 / M_PI) - 90.0f;
                  float diff = tYaw - pYaw;
                  while (diff > 180.0f) diff -= 360.0f;
                  while (diff < -180.0f) diff += 360.0f;
                  score = fabs(diff);
                } else if (bowAimbotConfig.filterMode == 2 && g_fHealth) {
                  float hp = env->GetFloatField(ent, g_fHealth);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); hp = 20.0f; }
                  score = hp;
                }

                if (score < bestBowScore) {
                  if (bestBowTarget) env->DeleteLocalRef(bestBowTarget);
                  bestBowTarget = ent;
                  bestBowScore = score;
                } else {
                  env->DeleteLocalRef(ent);
                }
              }
              env->DeleteLocalRef(entList);

              if (bestBowTarget) {
                double tx = env->GetDoubleField(bestBowTarget, g_fPosX);
                double ty = env->GetDoubleField(bestBowTarget, g_fPosY);
                double tz = env->GetDoubleField(bestBowTarget, g_fPosZ);
                float tEyeH = env->CallFloatMethod(bestBowTarget, g_mGetEyeHeight);
                double tmx = 0.0, tmz = 0.0;
                if (g_fMotionX && g_fMotionZ) {
                  tmx = env->GetDoubleField(bestBowTarget, g_fMotionX);
                  tmz = env->GetDoubleField(bestBowTarget, g_fMotionZ);
                }
                if (env->ExceptionCheck()) env->ExceptionClear();

                BowPrediction pred = BowAimbot_Calculate(px, py, pz, eyeH, tx, ty, tz, tEyeH, tmx, tmz, chargeTicks);

                if (pred.valid) {

                  float outYaw, outPitch;
                  float sens = 0.5f;
                  BowAimbot_SmoothAim(pYaw, pPitch, pred.yaw, pred.pitch, sens, outYaw, outPitch);

                  env->SetFloatField(player, g_fRotYaw, outYaw);
                  env->SetFloatField(player, g_fRotPitch, outPitch);
                  if (g_fPrevRotYaw) env->SetFloatField(player, g_fPrevRotYaw, outYaw);
                  if (g_fPrevRotPitch) env->SetFloatField(player, g_fPrevRotPitch, outPitch);
                  if (g_fRotYawHead) env->SetFloatField(player, g_fRotYawHead, outYaw);
                  if (env->ExceptionCheck()) env->ExceptionClear();
                }

                env->DeleteLocalRef(bestBowTarget);
              }
            }
          }
          if (player) env->DeleteLocalRef(player);
          if (world) env->DeleteLocalRef(world);
        } else {
          if (player) env->DeleteLocalRef(player);
          if (world) env->DeleteLocalRef(world);
        }
        env->DeleteLocalRef(mc);
      }
    }
  }

  if (scaffoldEnabled && g_jniReady && g_jvm && !g_isUnloading && !g_cursorVisible &&
      g_blockPosClass && g_blockPosInit && g_targetWindow) {
    static DWORD lastScaffoldPlace = 0;
    DWORD now = (DWORD)GetTickCount64();
    if (true) {
      JNIEnv *env = nullptr;
      if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (mc && !env->ExceptionCheck()) {
          jobject player = env->GetObjectField(mc, g_fThePlayer);
          if (player) {

            bool holdingBlock = false;
            if (g_mGetCurrentItem && g_mStackGetItem && g_itemBlockClass) {
              jobject stack = env->CallObjectMethod(player, g_mGetCurrentItem);
              if (stack && !env->ExceptionCheck()) {
                jobject item = env->CallObjectMethod(stack, g_mStackGetItem);
                if (item && !env->ExceptionCheck()) {
                  holdingBlock = env->IsInstanceOf(item, g_itemBlockClass);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); holdingBlock = false; }
                  env->DeleteLocalRef(item);
                } else { if (env->ExceptionCheck()) env->ExceptionClear(); }
                env->DeleteLocalRef(stack);
              } else { if (env->ExceptionCheck()) env->ExceptionClear(); }
            }

            if (!holdingBlock && scaffoldAutoBlock && g_fInventory && g_fCurrentItemIdx &&
                g_mGetStackInSlot && g_itemBlockClass) {
              jobject inv = env->GetObjectField(player, g_fInventory);
              if (inv && !env->ExceptionCheck()) {
                for (int i = 0; i < 9 && !holdingBlock; i++) {
                  jobject st = env->CallObjectMethod(inv, g_mGetStackInSlot, (jint)i);
                  if (st && !env->ExceptionCheck()) {
                    jobject it = env->CallObjectMethod(st, g_mStackGetItem);
                    if (it && !env->ExceptionCheck()) {
                      if (env->IsInstanceOf(it, g_itemBlockClass)) {
                        env->SetIntField(inv, g_fCurrentItemIdx, i);
                        if (env->ExceptionCheck()) env->ExceptionClear();
                        holdingBlock = true;
                      }
                      env->DeleteLocalRef(it);
                    } else if (env->ExceptionCheck()) {
                      env->ExceptionClear();
                    }
                    env->DeleteLocalRef(st);
                  } else if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                  }
                }
                env->DeleteLocalRef(inv);
              } else if (env->ExceptionCheck()) {
                env->ExceptionClear();
              }
            }

            if (holdingBlock && g_mIsAirBlock) {
              jobject world = env->GetObjectField(mc, g_fTheWorld);
              if (world && !env->ExceptionCheck()) {
                double px = env->GetDoubleField(player, g_fPosX);
                double py = env->GetDoubleField(player, g_fPosY);
                double pz = env->GetDoubleField(player, g_fPosZ);
                if (env->ExceptionCheck()) env->ExceptionClear();

                bool onGround = false;
                if (g_fOnGround) {
                  onGround = env->GetBooleanField(player, g_fOnGround) == JNI_TRUE;
                  if (env->ExceptionCheck()) { env->ExceptionClear(); onGround = false; }
                }
                double motX = 0.0, motY = 0.0, motZ = 0.0;
                if (g_fMotionX) {
                  motX = env->GetDoubleField(player, g_fMotionX);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); motX = 0.0; }
                }
                if (g_fMotionY) {
                  motY = env->GetDoubleField(player, g_fMotionY);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); motY = 0.0; }
                }
                if (g_fMotionZ) {
                  motZ = env->GetDoubleField(player, g_fMotionZ);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); motZ = 0.0; }
                }
                bool airborne = !onGround;
                double speed = sqrt(motX * motX + motZ * motZ);
                bool sprintJumping = airborne && speed > 0.15;

                int by = (int)floor(py) - 1;

                if (airborne && py - floor(py) > 0.9)
                  by = (int)floor(py);

                float fracY = (float)(py - floor(py));

                struct CandPos { int cx, cz; };
                CandPos candidates[4];
                int numCand = 0;

                double lead = sprintJumping ? 2.5 : (airborne ? 1.8 : 1.2);
                int predBx = (int)floor(px + motX * lead);
                int predBz = (int)floor(pz + motZ * lead);
                candidates[numCand++] = {predBx, predBz};

                if (sprintJumping) {
                  int midBx = (int)floor(px + motX * 1.2);
                  int midBz = (int)floor(pz + motZ * 1.2);
                  if (midBx != predBx || midBz != predBz)
                    candidates[numCand++] = {midBx, midBz};
                }

                int curBx = (int)floor(px);
                int curBz = (int)floor(pz);
                if (curBx != predBx || curBz != predBz)
                  candidates[numCand++] = {curBx, curBz};

                if (speed > 0.05) {
                  int behBx = (int)floor(px - motX * 0.5);
                  int behBz = (int)floor(pz - motZ * 0.5);
                  candidates[numCand++] = {behBx, behBz};
                }

                int bx = predBx, bz = predBz;
                bool needsBlock = false;
                for (int ci = 0; ci < numCand && !needsBlock; ci++) {
                  int tx = candidates[ci].cx, tz = candidates[ci].cz;
                  jobject testPos = env->NewObject(g_blockPosClass, g_blockPosInit, (jint)tx, (jint)by, (jint)tz);
                  if (testPos && !env->ExceptionCheck()) {
                    bool isAir = env->CallBooleanMethod(world, g_mIsAirBlock, testPos) == JNI_TRUE;
                    if (env->ExceptionCheck()) { env->ExceptionClear(); isAir = false; }
                    env->DeleteLocalRef(testPos);
                    if (isAir && (fracY >= scaffoldMinDist || airborne)) {
                      bx = tx; bz = tz;
                      needsBlock = true;
                    }
                  } else if (env->ExceptionCheck()) { env->ExceptionClear(); }
                }

                if (needsBlock) {

                  struct Dir { int dx, dy, dz; };
                  Dir dirs[] = {
                    {0, -1, 0},
                    {0, 0, -1},
                    {0, 0, 1},
                    {1, 0, 0},
                    {-1, 0, 0},
                    {1, 0, -1},
                    {-1, 0, 1},
                    {1, 0, 1},
                    {-1, 0, -1},
                  };

                  int foundX = 0, foundY = 0, foundZ = 0;
                  int faceX = 0, faceY = 0, faceZ = 0;
                  bool foundSolid = false;

                  for (auto &d : dirs) {
                    int cx = bx + d.dx, cy = by + d.dy, cz = bz + d.dz;
                    jobject checkPos = env->NewObject(g_blockPosClass, g_blockPosInit, (jint)cx, (jint)cy, (jint)cz);
                    if (checkPos && !env->ExceptionCheck()) {
                      bool air = env->CallBooleanMethod(world, g_mIsAirBlock, checkPos) == JNI_TRUE;
                      if (env->ExceptionCheck()) { env->ExceptionClear(); air = true; }
                      env->DeleteLocalRef(checkPos);
                      if (!air) {
                        foundX = cx; foundY = cy; foundZ = cz;
                        faceX = -d.dx; faceY = -d.dy; faceZ = -d.dz;
                        foundSolid = true;
                        break;
                      }
                    } else { if (env->ExceptionCheck()) env->ExceptionClear(); }
                  }

                  if (foundSolid) {
                    float savedYaw = env->GetFloatField(player, g_fRotYaw);
                    float savedPitch = env->GetFloatField(player, g_fRotPitch);
                    if (env->ExceptionCheck()) env->ExceptionClear();
                    bool canPlaceNow =
                        (now - lastScaffoldPlace) >=
                        (DWORD)(std::max)(0, scaffoldDelayMs);

                    double targetX = foundX + 0.5 + faceX * 0.5;
                    double targetY = foundY + 0.5 + faceY * 0.5;
                    double targetZ = foundZ + 0.5 + faceZ * 0.5;
                    double eyeY = py + 1.62;
                    double ddx = targetX - px;
                    double ddy = targetY - eyeY;
                    double ddz = targetZ - pz;
                    double horiz = sqrt(ddx * ddx + ddz * ddz);
                    float placeYaw = (float)(atan2(ddz, ddx) * 180.0 / M_PI) - 90.0f;
                    float placePitch = (float)(-atan2(ddy, horiz) * 180.0 / M_PI);
                    if (placePitch > 90.0f) placePitch = 90.0f;
                    if (placePitch < -90.0f) placePitch = -90.0f;

                    if (!scaffoldLegitMode) {
                      env->SetFloatField(player, g_fRotYaw, placeYaw);
                      env->SetFloatField(player, g_fRotPitch, placePitch);
                      if (env->ExceptionCheck()) env->ExceptionClear();
                    }

                    bool placed = false;
                    if (canPlaceNow) {

                      if (g_mOnPlayerRightClick && g_vec3Class && g_vec3Init &&
                          g_fPlayerController && g_fInventory && g_fCurrentItemIdx) {
                        jobject pcmp = env->GetObjectField(mc, g_fPlayerController);
                        jobject inv = env->GetObjectField(player, g_fInventory);
                        if (pcmp && inv && !env->ExceptionCheck()) {
                          int curSlot = env->GetIntField(inv, g_fCurrentItemIdx);
                          if (env->ExceptionCheck()) { env->ExceptionClear(); curSlot = -1; }
                          jobject heldStack = nullptr;
                          if (curSlot >= 0 && g_mGetStackInSlot) {
                            heldStack = env->CallObjectMethod(inv, g_mGetStackInSlot, (jint)curSlot);
                            if (env->ExceptionCheck()) { env->ExceptionClear(); heldStack = nullptr; }
                          }

                          int facingIdx = 1;
                          if (faceY < 0) facingIdx = 0;
                          else if (faceY > 0) facingIdx = 1;
                          else if (faceZ < 0) facingIdx = 2;
                          else if (faceZ > 0) facingIdx = 3;
                          else if (faceX < 0) facingIdx = 4;
                          else if (faceX > 0) facingIdx = 5;
                          jobject sideFacing = g_enumFacingByIndex[facingIdx];

                          jobject clickPos = env->NewObject(g_blockPosClass, g_blockPosInit,
                                                            (jint)foundX, (jint)foundY,
                                                            (jint)foundZ);
                          jobject hitVec = env->NewObject(g_vec3Class, g_vec3Init,
                                                          (jdouble)targetX, (jdouble)targetY,
                                                          (jdouble)targetZ);

                          if (heldStack && clickPos && hitVec && sideFacing && !env->ExceptionCheck()) {
                            jboolean ok = env->CallBooleanMethod(
                                pcmp, g_mOnPlayerRightClick, player, world, heldStack,
                                clickPos, sideFacing, hitVec);
                            if (env->ExceptionCheck()) {
                              env->ExceptionClear();
                              ok = JNI_FALSE;
                            }
                            placed = (ok == JNI_TRUE);
                            if (placed && g_mSwingItem) {
                              env->CallVoidMethod(player, g_mSwingItem);
                              if (env->ExceptionCheck()) env->ExceptionClear();
                            }
                          } else if (env->ExceptionCheck()) {
                            env->ExceptionClear();
                          }

                          if (heldStack) env->DeleteLocalRef(heldStack);
                          if (clickPos) env->DeleteLocalRef(clickPos);
                          if (hitVec) env->DeleteLocalRef(hitVec);
                        } else if (env->ExceptionCheck()) {
                          env->ExceptionClear();
                        }
                        if (inv) env->DeleteLocalRef(inv);
                        if (pcmp) env->DeleteLocalRef(pcmp);
                      }

                      if (!placed) {
                        env->SetFloatField(player, g_fRotYaw, placeYaw);
                        env->SetFloatField(player, g_fRotPitch, placePitch);
                        if (env->ExceptionCheck()) env->ExceptionClear();
                        if (g_mRightClickMouse) {
                          env->CallVoidMethod(mc, g_mRightClickMouse);
                          if (env->ExceptionCheck()) env->ExceptionClear();
                          placed = true;
                        } else {
                          PostMessage(g_targetWindow, WM_RBUTTONDOWN, MK_RBUTTON, 0);
                          Sleep(50);
                          PostMessage(g_targetWindow, WM_RBUTTONUP, 0, 0);
                          placed = true;
                        }
                      }
                    }

                    if (scaffoldLegitMode) {
                      g_scaffLegitYaw = placeYaw;
                      g_scaffLegitPitch = placePitch;
                      g_scaffLegitActive = true;
                    }

                    env->SetFloatField(player, g_fRotYaw, savedYaw);
                    env->SetFloatField(player, g_fRotPitch, savedPitch);
                    if (env->ExceptionCheck()) env->ExceptionClear();

                    if (placed)
                      lastScaffoldPlace = now;

                  }
                }
                env->DeleteLocalRef(world);
              }

              if (scaffoldLegitMode && g_scaffLegitActive) {
                g_scaffSmHeadYaw += (g_scaffLegitYaw - g_scaffSmHeadYaw) * 0.35f;
                g_scaffSmHeadPitch += (g_scaffLegitPitch - g_scaffSmHeadPitch) * 0.35f;
                if (g_fRotYawHead) {
                  env->SetFloatField(player, g_fRotYawHead, g_scaffSmHeadYaw);
                  if (env->ExceptionCheck()) env->ExceptionClear();
                }
                if (g_fRotPitchHead) {
                  env->SetFloatField(player, g_fRotPitchHead, g_scaffSmHeadPitch);
                  if (env->ExceptionCheck()) env->ExceptionClear();
                }
                if (g_fRenderYawOffset) {
                  env->SetFloatField(player, g_fRenderYawOffset, g_scaffSmHeadYaw);
                  if (env->ExceptionCheck()) env->ExceptionClear();
                }
              }
            }
            env->DeleteLocalRef(player);
          }
          env->DeleteLocalRef(mc);
        } else {
          if (env && env->ExceptionCheck()) env->ExceptionClear();
        }
      }
    }
  } else {

    g_scaffLegitActive = false;
  }

  if (bridgeAssistEnabled && g_jniReady && g_jvm && !g_isUnloading &&
      g_blockPosClass && g_blockPosInit && g_mIsAirBlock &&
      g_fKeyBindSneak && g_fKeyBindPressed) {
    static bool baSneaking = false;
    static DWORD baLastRmbTime = 0;

    bool baActive = true;

    if (bridgeAssistRmbOnly) {
      if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        baLastRmbTime = (DWORD)GetTickCount64();
      if (!(GetAsyncKeyState(VK_RBUTTON) & 0x8000) && (DWORD)GetTickCount64() - baLastRmbTime > 500)
        baActive = false;
    }

    if (bridgeAssistBackwardOnly && !(GetAsyncKeyState('S') & 0x8000))
      baActive = false;

    if (baActive) {
      JNIEnv *env = nullptr;
      if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (mc && !env->ExceptionCheck()) {
          jobject player = env->GetObjectField(mc, g_fThePlayer);
          if (player) {

            if (bridgeAssistCheckPitch) {
              float pitch = env->GetFloatField(player, g_fRotPitch);
              if (env->ExceptionCheck()) { env->ExceptionClear(); pitch = 0; }
              if (pitch < bridgeAssistPitchThreshold) baActive = false;
            }

            if (baActive && bridgeAssistBlocksOnly && g_mGetCurrentItem && g_mStackGetItem && g_itemBlockClass) {
              jobject stack = env->CallObjectMethod(player, g_mGetCurrentItem);
              if (stack && !env->ExceptionCheck()) {
                jobject item = env->CallObjectMethod(stack, g_mStackGetItem);
                if (item && !env->ExceptionCheck()) {
                  if (!env->IsInstanceOf(item, g_itemBlockClass)) baActive = false;
                  if (env->ExceptionCheck()) env->ExceptionClear();
                  env->DeleteLocalRef(item);
                } else { if (env->ExceptionCheck()) env->ExceptionClear(); baActive = false; }
                env->DeleteLocalRef(stack);
              } else { if (env->ExceptionCheck()) env->ExceptionClear(); baActive = false; }
            }

            if (baActive) {
              bool onGround = false;
              if (g_fOnGround) {
                onGround = env->GetBooleanField(player, g_fOnGround) == JNI_TRUE;
                if (env->ExceptionCheck()) { env->ExceptionClear(); onGround = false; }
              }

              double px = env->GetDoubleField(player, g_fPosX);
              double py = env->GetDoubleField(player, g_fPosY);
              double pz = env->GetDoubleField(player, g_fPosZ);
              if (env->ExceptionCheck()) env->ExceptionClear();

              int bx = (int)floor(px), by = (int)floor(py) - 1, bz = (int)floor(pz);
              jobject belowPos = env->NewObject(g_blockPosClass, g_blockPosInit, (jint)bx, (jint)by, (jint)bz);
              jobject below2Pos = env->NewObject(g_blockPosClass, g_blockPosInit, (jint)bx, (jint)(by - 1), (jint)bz);
              bool belowAir = false, below2Air = false;
              if (belowPos && !env->ExceptionCheck()) {
                belowAir = env->CallBooleanMethod(env->GetObjectField(mc, g_fTheWorld), g_mIsAirBlock, belowPos) == JNI_TRUE;
                if (env->ExceptionCheck()) { env->ExceptionClear(); belowAir = false; }
                env->DeleteLocalRef(belowPos);
              } else if (env->ExceptionCheck()) env->ExceptionClear();
              if (below2Pos && !env->ExceptionCheck()) {
                below2Air = env->CallBooleanMethod(env->GetObjectField(mc, g_fTheWorld), g_mIsAirBlock, below2Pos) == JNI_TRUE;
                if (env->ExceptionCheck()) { env->ExceptionClear(); below2Air = false; }
                env->DeleteLocalRef(below2Pos);
              } else if (env->ExceptionCheck()) env->ExceptionClear();

              jobject gs = env->GetObjectField(mc, g_fGameSettings);
              jobject sneakKb = gs ? env->GetObjectField(gs, g_fKeyBindSneak) : nullptr;
              if (env->ExceptionCheck()) { env->ExceptionClear(); sneakKb = nullptr; }

              if (sneakKb && onGround && belowAir && below2Air) {
                env->SetBooleanField(sneakKb, g_fKeyBindPressed, JNI_TRUE);
                if (env->ExceptionCheck()) env->ExceptionClear();
                baSneaking = true;
                if (bridgeAssistDelay > 0)
                  Sleep(bridgeAssistDelay + (rand() % 50));
              } else if (sneakKb && onGround && baSneaking) {
                env->SetBooleanField(sneakKb, g_fKeyBindPressed, JNI_FALSE);
                if (env->ExceptionCheck()) env->ExceptionClear();
                baSneaking = false;
              }

              if (sneakKb) env->DeleteLocalRef(sneakKb);
              if (gs) env->DeleteLocalRef(gs);
            } else if (baSneaking) {

              jobject gs = env->GetObjectField(mc, g_fGameSettings);
              jobject sneakKb = gs ? env->GetObjectField(gs, g_fKeyBindSneak) : nullptr;
              if (env->ExceptionCheck()) { env->ExceptionClear(); sneakKb = nullptr; }
              if (sneakKb) {
                env->SetBooleanField(sneakKb, g_fKeyBindPressed, JNI_FALSE);
                if (env->ExceptionCheck()) env->ExceptionClear();
                env->DeleteLocalRef(sneakKb);
              }
              if (gs) env->DeleteLocalRef(gs);
              baSneaking = false;
            }
            env->DeleteLocalRef(player);
          }
          env->DeleteLocalRef(mc);
        } else if (env && env->ExceptionCheck()) env->ExceptionClear();
      }
    } else if (baSneaking) {

      JNIEnv *env = nullptr;
      if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
        jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (mc && !env->ExceptionCheck()) {
          jobject gs = env->GetObjectField(mc, g_fGameSettings);
          jobject sneakKb = gs ? env->GetObjectField(gs, g_fKeyBindSneak) : nullptr;
          if (env->ExceptionCheck()) { env->ExceptionClear(); sneakKb = nullptr; }
          if (sneakKb) {
            env->SetBooleanField(sneakKb, g_fKeyBindPressed, JNI_FALSE);
            if (env->ExceptionCheck()) env->ExceptionClear();
            env->DeleteLocalRef(sneakKb);
          }
          if (gs) env->DeleteLocalRef(gs);
          env->DeleteLocalRef(mc);
        } else if (env && env->ExceptionCheck()) env->ExceptionClear();
      }
      baSneaking = false;
    }
  }

  if (espEnabled && (espShowName || espShowHealthBar || espShowArmor || espBoxStyle == 2) &&
      g_jniReady && g_jvm && !g_isUnloading &&
      g_fEntityList && g_mListSize && g_mListGet && g_mGetHealth &&
      g_mGetMaxHealth && g_entityPlayerClass) {
    JNIEnv *env = nullptr;
    if (g_jvm->GetEnv((void **)&env, JNI_VERSION_1_8) == JNI_OK && env) {
      jobject mc = env->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
      if (mc && !env->ExceptionCheck()) {
        jobject player = env->GetObjectField(mc, g_fThePlayer);
        jobject world = env->GetObjectField(mc, g_fTheWorld);
        if (player && world) {
          jobject entList = env->GetObjectField(world, g_fEntityList);
          if (entList && !env->ExceptionCheck()) {
            jint count = env->CallIntMethod(entList, g_mListSize);
            if (env->ExceptionCheck()) { env->ExceptionClear(); count = 0; }

            if (g_fPosX && g_fPosY && g_fPosZ) {
              g_espLocalPX = env->GetDoubleField(player, g_fPosX);
              if (env->ExceptionCheck()) { env->ExceptionClear(); g_espLocalPX = 0; }
              g_espLocalPY = env->GetDoubleField(player, g_fPosY);
              if (env->ExceptionCheck()) { env->ExceptionClear(); g_espLocalPY = 0; }
              g_espLocalPZ = env->GetDoubleField(player, g_fPosZ);
              if (env->ExceptionCheck()) { env->ExceptionClear(); g_espLocalPZ = 0; }
            }

            g_ntEntityData.clear();
            for (jint i = 0; i < count && i < 200; i++) {
              jobject ent = env->CallObjectMethod(entList, g_mListGet, i);
              if (!ent || env->ExceptionCheck()) { env->ExceptionClear(); continue; }

              if (!env->IsInstanceOf(ent, g_entityPlayerClass)) {
                env->DeleteLocalRef(ent);
                continue;
              }

              if (env->IsSameObject(ent, player)) {
                env->DeleteLocalRef(ent);
                continue;
              }

              NTEntityData data{};
              data.maxHealth = 20.0f;
              data.health = 20.0f;
              data.armor = 0;
              data.name[0] = '\0';
              data.weapon[0] = '\0';
              data.hasPos = false;

              if (g_fPosX && g_fPosY && g_fPosZ) {
                data.posX = env->GetDoubleField(ent, g_fPosX);
                if (env->ExceptionCheck()) { env->ExceptionClear(); data.posX = 0; }
                data.posY = env->GetDoubleField(ent, g_fPosY);
                if (env->ExceptionCheck()) { env->ExceptionClear(); data.posY = 0; }
                data.posZ = env->GetDoubleField(ent, g_fPosZ);
                if (env->ExceptionCheck()) { env->ExceptionClear(); data.posZ = 0; }
                data.hasPos = true;
              }

              float hp = env->CallFloatMethod(ent, g_mGetHealth);
              if (env->ExceptionCheck()) { env->ExceptionClear(); hp = 0; }
              float maxHp = env->CallFloatMethod(ent, g_mGetMaxHealth);
              if (env->ExceptionCheck()) { env->ExceptionClear(); maxHp = 20; }

              data.health = hp;
              data.maxHealth = maxHp > 0 ? maxHp : 20.0f;

              if (g_mGetArmorValue) {
                data.armor = env->CallIntMethod(ent, g_mGetArmorValue);
                if (env->ExceptionCheck()) { env->ExceptionClear(); data.armor = 0; }
              }

              for (int sl = 0; sl < 4; sl++) {
                data.armorSlots[sl] = ArmorSlotData{};
                if (!g_mGetEquipmentInSlot) continue;
                jobject stack = env->CallObjectMethod(ent, g_mGetEquipmentInSlot, (jint)(sl + 1));
                if (env->ExceptionCheck()) { env->ExceptionClear(); continue; }
                if (!stack) continue;
                data.armorSlots[sl].equipped = true;

                if (g_mGetMaxDamage && g_mGetItemDamage) {
                  int maxDmg = env->CallIntMethod(stack, g_mGetMaxDamage);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); maxDmg = 0; }
                  int curDmg = env->CallIntMethod(stack, g_mGetItemDamage);
                  if (env->ExceptionCheck()) { env->ExceptionClear(); curDmg = 0; }
                  data.armorSlots[sl].maxDurability = maxDmg;
                  data.armorSlots[sl].durability = maxDmg - curDmg;
                }

                if (g_mStackGetDisplayName) {
                  jstring dn = (jstring)env->CallObjectMethod(stack, g_mStackGetDisplayName);
                  if (dn && !env->ExceptionCheck()) {
                    const char *dc = env->GetStringUTFChars(dn, nullptr);
                    if (dc) {
                      strncpy_s(data.armorSlots[sl].displayName, sizeof(data.armorSlots[sl].displayName), dc, _TRUNCATE);
                      env->ReleaseStringUTFChars(dn, dc);
                    }
                    env->DeleteLocalRef(dn);
                  } else if (env->ExceptionCheck()) { env->ExceptionClear(); }
                }
                env->DeleteLocalRef(stack);
              }

              if (g_mGetName) {
                jstring nm = (jstring)env->CallObjectMethod(ent, g_mGetName);
                if (nm && !env->ExceptionCheck()) {
                  const char *nchars = env->GetStringUTFChars(nm, nullptr);
                  if (nchars) {
                    int wi = 0;
                    for (int ri = 0; nchars[ri] != '\0' && wi < (int)sizeof(data.name) - 1; ri++) {
                      if (nchars[ri] == '\xC2' && nchars[ri + 1] == '\xA7') {
                        if (nchars[ri + 2] != '\0') ri += 2;
                        continue;
                      }
                      if (nchars[ri] == '\xA7') {
                        if (nchars[ri + 1] != '\0') ri += 1;
                        continue;
                      }
                      data.name[wi++] = nchars[ri];
                    }
                    data.name[wi] = '\0';
                    env->ReleaseStringUTFChars(nm, nchars);
                  }
                  env->DeleteLocalRef(nm);
                } else if (env->ExceptionCheck()) {
                  env->ExceptionClear();
                }
              }

              if (g_mGetCurrentItem && g_mStackGetDisplayName) {
                jobject st = env->CallObjectMethod(ent, g_mGetCurrentItem);
                if (st && !env->ExceptionCheck()) {
                  jstring wn = (jstring)env->CallObjectMethod(st, g_mStackGetDisplayName);
                  if (wn && !env->ExceptionCheck()) {
                    const char *wchars = env->GetStringUTFChars(wn, nullptr);
                    if (wchars) {
                      strncpy_s(data.weapon, sizeof(data.weapon), wchars, _TRUNCATE);
                      env->ReleaseStringUTFChars(wn, wchars);
                    }
                    env->DeleteLocalRef(wn);
                  } else if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                  }
                  env->DeleteLocalRef(st);
                } else if (env->ExceptionCheck()) {
                  env->ExceptionClear();
                }
              }

              g_ntEntityData.push_back(data);
              env->DeleteLocalRef(ent);
            }
            g_ntEntityDataReady = g_ntEntityData;
            env->DeleteLocalRef(entList);
          } else { if (env->ExceptionCheck()) env->ExceptionClear(); }
          if (player) env->DeleteLocalRef(player);
          if (world) env->DeleteLocalRef(world);
        } else {
          if (player) env->DeleteLocalRef(player);
          if (world) env->DeleteLocalRef(world);
        }
        env->DeleteLocalRef(mc);
      } else { if (env && env->ExceptionCheck()) env->ExceptionClear(); }
    }
  }

  DrawHud();
  UpdateCursorState();
  UpdateAimbotKeyState();

  if (g_unloadRequested && !g_isUnloading) {
    UnloadDLL();
    return pSwapBuffers(hDC);
  }

  if (!g_imguiInitialized) {
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigWindowsMoveFromTitleBarOnly = false;
    io.IniFilename = nullptr;

    ImFontConfig fontCfg;
    fontCfg.OversampleH = 2;
    fontCfg.OversampleV = 1;
    ImFont *font = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\segoeui.ttf", 18.0f, &fontCfg);
    if (!font) {
      io.Fonts->AddFontDefault();
    }

    ImGui_ImplWin32_Init(window);
    ImGui_ImplOpenGL2_Init();
    g_imguiInitialized = true;
    g_imguiHwnd = window;
  }

  if (!g_wndProcHooked) {
    g_originalWndProc =
        (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
    g_wndProcHooked = true;
  }

  float fadeDt = ImGui::GetIO().DeltaTime;

  float fadeTarget = showMenu ? 1.0f : 0.0f;
  float fadeLambda = 8.0f;
  g_menuFadeAnim += (fadeTarget - g_menuFadeAnim) * (1.0f - expf(-fadeLambda * fadeDt));
  if (fabsf(g_menuFadeAnim - fadeTarget) < 0.003f)
    g_menuFadeAnim = fadeTarget;

  if (showMenu) {
    g_menuScaleAnim += (1.0f - g_menuScaleAnim) * (1.0f - expf(-10.0f * fadeDt));
    if (g_menuScaleAnim > 0.997f)
      g_menuScaleAnim = 1.0f;
    g_cardAppearTimer += fadeDt;
  } else {
    g_menuScaleAnim = 1.0f;
  }

  if (g_imguiInitialized) {
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (showMenu || g_menuFadeAnim > 0.01f) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_menuFadeAnim);
      RenderModernMenu();
      ImGui::PopStyleVar();
    }

    RenderArrayList();
    RenderWatermark();
    RenderTargetHUD();

    if (!espEnabled) g_espOverlaysReady.clear();

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    if (draw_data) {
      ImGui_ImplOpenGL2_RenderDrawData(draw_data);
    }
  }

  static DWORD windowThread = 0;
  if (!windowThread && !g_isUnloading) {
    windowThread = GetWindowThreadProcessId(window, nullptr);
    if (windowThread) {
      g_keyDispatcherThread = CreateThread(
          nullptr, 0, keyDispatcher, reinterpret_cast<LPVOID>(&windowThread), 0,
          &g_keyDispatcherThreadId);

      g_moduleThreadCount = 0;

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            clickerThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            rightClickerThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            fastPlaceThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            blockhitThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            reachThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            velocityThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            sprintThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            hitSelectThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_threadsSpawned = true;

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            throwpotThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            autosoupThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            refillThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            sprintResetThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_moduleThreads[g_moduleThreadCount++] = CreateThread(
          nullptr, 0,
          [](LPVOID) -> DWORD {
            autoToolThread();
            return 0;
          },
          nullptr, 0, nullptr);

      g_ipcThread = CreateThread(nullptr, 0, IpcListenerThread, nullptr, 0, nullptr);
      if (g_ipcThread)
        g_moduleThreads[g_moduleThreadCount++] = g_ipcThread;

      HANDLE hPresetLoad = CreateThread(nullptr, 0, PresetLoadThread, nullptr, 0, nullptr);
      if (hPresetLoad)
        g_moduleThreads[g_moduleThreadCount++] = hPresetLoad;
    }
  }

  if (wallhacks) {
    glDepthFunc(GL_ALWAYS);
  }
  sInventory = false;

  glDepthFunc(GL_LEQUAL);
  if (xrayEnabled) {
    glDepthFunc(GL_ALWAYS);
  }

  return pSwapBuffers(hDC);
}

void __stdcall myglOrtho(double left, double right, double bottom, double top,
                         double zNear, double zFar) {
  if (g_isUnloading) {
    if (pglOrtho)
      pglOrtho(left, right, bottom, top, zNear, zFar);
    return;
  }

  if (zNear == 1000 && zFar == 3000) {
    sInventory = true;

    GLint currentMatrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &currentMatrixMode);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (espEnabled || tracerEnabled) {
      g_currentOutlineColor = espOutlineColor;
      g_currentFillColor = espFillColor;
      g_currentOutlineWidth = espOutlineWidth;
      g_currentShowFill = espShowFill;
      g_currentShowOutline = espShowOutline;
      g_currentBoxStyle = espBoxStyle;
      g_currentCornerLength = espCornerLength;
      bool draw3DBoxes = espEnabled && espBoxStyle != 2;
      draw(players, Radius(0.8F, 2.0F, 0.8F), Color(0, 0, 0), draw3DBoxes,
           tracerEnabled, true);
    }
    if (chestEspEnabled) {
      g_currentOutlineColor = chestOutlineColor;
      g_currentFillColor = chestFillColor;
      g_currentOutlineWidth = chestOutlineWidth;
      g_currentShowFill = chestShowFill;
      g_currentShowOutline = chestShowOutline;
      g_currentBoxStyle = 0;
      g_currentCornerLength = 0.25f;
      draw(chest, Radius(1.0F, 1.0F, 1.0F), Color(0, 0, 0), true, false, false);
      draw(largeChest, Radius(2.0F, 1.0F, 1.0F), Color(0, 0, 0), true, false, false);
      if (chestShowTracers) {
        ImVec4 savedTracerColor = tracerColor;
        float savedTracerWidth = tracerWidth;
        tracerColor = chestTracerColor;
        tracerWidth = chestTracerWidth;
        draw(chest, Radius(1.0F, 1.0F, 1.0F), Color(0, 0, 0), false, true, false);
        draw(largeChest, Radius(2.0F, 1.0F, 1.0F), Color(0, 0, 0), false, true, false);
        tracerColor = savedTracerColor;
        tracerWidth = savedTracerWidth;
      }
    }

    bool espShouldProject = espEnabled && !players.modelview.empty() && !players.projection.empty();
    if (espShouldProject) {
      size_t numPlayers = players.modelview.size() / 16;
      std::vector<NTEntityData> ntSnap = g_ntEntityDataReady;

      std::vector<bool> ntUsed(ntSnap.size(), false);

      GLint viewport[4]{0, 0, 0, 0};
      glGetIntegerv(GL_VIEWPORT, viewport);
      if (viewport[2] <= 0 || viewport[3] <= 0)
        viewport[2] = 1920, viewport[3] = 1080;

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      g_espOverlays.clear();

      const float hw = 0.4f, hh = 1.0f, hd = 0.4f;
      double lpx = g_espLocalPX, lpy = g_espLocalPY, lpz = g_espLocalPZ;

      for (size_t idx = 0; idx < numPlayers; idx++) {
        size_t off = idx * 16;
        if (off + 15 >= players.modelview.size() || off + 15 >= players.projection.size())
          break;

        const float *mv = players.modelview.data() + off;
        const float *pr = players.projection.data() + off;

        auto transformPoint = [&](float lx, float ly, float lz, float &sx,
                                  float &sy) -> bool {
          float wx = mv[0]*lx + mv[4]*ly + mv[8]*lz + mv[12];
          float wy = mv[1]*lx + mv[5]*ly + mv[9]*lz + mv[13];
          float wz = mv[2]*lx + mv[6]*ly + mv[10]*lz + mv[14];
          float ww = mv[3]*lx + mv[7]*ly + mv[11]*lz + mv[15];
          float cx = pr[0]*wx + pr[4]*wy + pr[8]*wz + pr[12]*ww;
          float cy = pr[1]*wx + pr[5]*wy + pr[9]*wz + pr[13]*ww;
          float cw = pr[3]*wx + pr[7]*wy + pr[11]*wz + pr[15]*ww;
          if (cw <= 0.001f) return false;
          float ndcX = cx / cw, ndcY = cy / cw;
          if (ndcX < -1.5f || ndcX > 1.5f || ndcY < -1.5f || ndcY > 1.5f) return false;
          sx = (ndcX * 0.5f + 0.5f) * viewport[2];
          sy = (1.0f - (ndcY * 0.5f + 0.5f)) * viewport[3];
          return true;
        };

        float minSX = FLT_MAX, maxSX = -FLT_MAX, minSY = FLT_MAX, maxSY = -FLT_MAX;
        int projected = 0;
        for (int cy2 = 0; cy2 < 2; cy2++)
          for (int cx2 = 0; cx2 < 2; cx2++)
            for (int cz = 0; cz < 2; cz++) {
              float lx = cx2 ? hw : -hw, ly = cy2 ? hh : -hh, lz = cz ? hd : -hd;
              float sx2, sy2;
              if (transformPoint(lx, ly, lz, sx2, sy2)) {
                if (sx2 < minSX) minSX = sx2; if (sx2 > maxSX) maxSX = sx2;
                if (sy2 < minSY) minSY = sy2; if (sy2 > maxSY) maxSY = sy2;
                projected++;
              }
            }
        if (projected < 2) continue;

        float boxTop = minSY, boxBottom = maxSY, boxLeft = minSX, boxRight = maxSX;
        float boxH = boxBottom - boxTop, boxW = boxRight - boxLeft;
        if (boxH < 18.0f) { float m = (boxTop+boxBottom)*0.5f; boxTop = m-9; boxBottom = m+9; boxH = 18; }
        if (boxW < 10.0f) { float m = (boxLeft+boxRight)*0.5f; boxLeft = m-5; boxRight = m+5; boxW = 10; }

        if (espUseImage && espImageLoaded && espImageTexture) {
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, espImageTexture);

          GLint boundTexture;
          glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
          if (boundTexture == (GLint)espImageTexture) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            float texLeft = 0.0f, texRight = 1.0f, texTop = 0.0f, texBottom = 1.0f;
            float adjLeft = boxLeft, adjRight = boxRight, adjTop = boxTop, adjBottom = boxBottom;

            if (!espImageStretch) {
              float imgAspect = (float)espImageWidth / (float)espImageHeight;
              float boxAspect = boxW / boxH;

              if (imgAspect > boxAspect) {
                float scaledH = boxW / imgAspect;
                float yOff = (boxH - scaledH) * 0.5f;
                adjTop    += yOff;
                adjBottom -= yOff;
              } else {
                float scaledW = boxH * imgAspect;
                float xOff = (boxW - scaledW) * 0.5f;
                adjLeft  += xOff;
                adjRight -= xOff;
              }
            }

            glColor4f(1.0f, 1.0f, 1.0f, espShowFill ? espFillColor.w : 1.0f);
            glBegin(GL_QUADS);
            glTexCoord2f(texLeft,  texTop);    glVertex2f(adjLeft,  adjTop);
            glTexCoord2f(texRight, texTop);    glVertex2f(adjRight, adjTop);
            glTexCoord2f(texRight, texBottom); glVertex2f(adjRight, adjBottom);
            glTexCoord2f(texLeft,  texBottom); glVertex2f(adjLeft,  adjBottom);
            glEnd();

            glDisable(GL_TEXTURE_2D);
          }
        } else if (espShowFill) {

          glColor4f(espFillColor.x, espFillColor.y, espFillColor.z, espFillColor.w);
          glBegin(GL_QUADS);
          glVertex2f(boxLeft,  boxTop);
          glVertex2f(boxRight, boxTop);
          glVertex2f(boxRight, boxBottom);
          glVertex2f(boxLeft,  boxBottom);
          glEnd();
        }

        if (espBoxStyle == 2) {
          if (espShowOutline) {
            glLineWidth(espOutlineWidth);
            glColor4f(espOutlineColor.x, espOutlineColor.y, espOutlineColor.z, espOutlineColor.w);
            glBegin(GL_LINE_LOOP);
            glVertex2f(boxLeft,  boxTop);
            glVertex2f(boxRight, boxTop);
            glVertex2f(boxRight, boxBottom);
            glVertex2f(boxLeft,  boxBottom);
            glEnd();
          }
        }

        float mvDist = sqrtf(mv[12]*mv[12] + mv[13]*mv[13] + mv[14]*mv[14]);

        int bestNT = -1;
        float bestDist = FLT_MAX;
        for (size_t ni = 0; ni < ntSnap.size(); ni++) {
          if (ntUsed[ni] || !ntSnap[ni].hasPos) continue;
          float relX = (float)(ntSnap[ni].posX - lpx);
          float relY = (float)(ntSnap[ni].posY - lpy);
          float relZ = (float)(ntSnap[ni].posZ - lpz);
          float worldDist = sqrtf(relX*relX + relY*relY + relZ*relZ);
          float diff = fabsf(mvDist - worldDist);
          if (diff < bestDist) { bestDist = diff; bestNT = (int)ni; }
        }

        ESPOverlayData ovr{};
        ovr.boxLeft = boxLeft; ovr.boxRight = boxRight;
        ovr.boxTop = boxTop; ovr.boxBottom = boxBottom;
        ovr.skinTexture = 0;
        ovr.valid = true;

        if (bestNT >= 0 && bestDist < 10000.0f) {
          ntUsed[bestNT] = true;
          const NTEntityData &nt = ntSnap[bestNT];
          ovr.hp = nt.health; ovr.maxHp = nt.maxHealth; ovr.armor = nt.armor;
          if (nt.name[0]) strncpy_s(ovr.name, sizeof(ovr.name), nt.name, _TRUNCATE);
          else strncpy_s(ovr.name, sizeof(ovr.name), "Player", _TRUNCATE);
          if (nt.weapon[0]) strncpy_s(ovr.weapon, sizeof(ovr.weapon), nt.weapon, _TRUNCATE);
          else ovr.weapon[0] = '\0';
          memcpy(ovr.armorSlots, nt.armorSlots, sizeof(ovr.armorSlots));
        } else {
          ovr.hp = 20; ovr.maxHp = 20; ovr.armor = 0;
          strncpy_s(ovr.name, sizeof(ovr.name), "Player", _TRUNCATE);
          ovr.weapon[0] = '\0';
        }
        ovr.hpFrac = ovr.maxHp > 0 ? ovr.hp / ovr.maxHp : 1.0f;
        if (ovr.hpFrac > 1.0f) ovr.hpFrac = 1.0f;
        if (ovr.hpFrac < 0.0f) ovr.hpFrac = 0.0f;

        g_espOverlays.push_back(ovr);
      }
      g_espOverlaysReady = g_espOverlays;
    }

    UpdateCursorState();

    bool shouldAim = aimbotEnabled && !entity.modelview.empty() &&
                     !entity.projection.empty() && !g_cursorVisible &&
                     !g_isUnloading && g_targetWindow &&
                     IsWindow(g_targetWindow) &&
                     GetForegroundWindow() == g_targetWindow &&
                     (!aimbotKeyMode || aimbotKeyPressed) &&
                     (!aimbotSwordOnly || g_holdingSword);

    static DWORD lastCursorVisibleTime = 0;
    if (g_cursorVisible) {
      lastCursorVisibleTime = (DWORD)GetTickCount64();
      shouldAim = false;
    } else if (lastCursorVisibleTime > 0) {

      if ((DWORD)GetTickCount64() - lastCursorVisibleTime < 100) {
        shouldAim = false;
      } else {
        lastCursorVisibleTime = 0;
      }
    }

    if (shouldAim) {
      switch (aimActivation) {
      case 0:
        break;
      case 1:
        shouldAim = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        break;
      case 2: {
        POINT curPos;
        GetCursorPos(&curPos);
        float mdx = fabsf((float)curPos.x - aimLastMouseX);
        float mdy = fabsf((float)curPos.y - aimLastMouseY);
        shouldAim = (mdx > 1.0f || mdy > 1.0f);
        aimLastMouseX = (float)curPos.x;
        aimLastMouseY = (float)curPos.y;
        break;
      }
      case 3:
        shouldAim = (GetAsyncKeyState('W') & 0x8000) &&
                    (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        break;
      }
    }

    if (shouldAim && g_jniReady && g_jvm) {
      JNIEnv *aimEnv = nullptr;
      do {
        if (g_jvm->GetEnv((void **)&aimEnv, JNI_VERSION_1_8) != JNI_OK)
          break;

        jobject mc = aimEnv->CallStaticObjectMethod(g_mcClass, g_getMinecraft);
        if (!mc || aimEnv->ExceptionCheck()) {
          aimEnv->ExceptionClear();
          break;
        }

        jobject player = aimEnv->GetObjectField(mc, g_fThePlayer);
        jobject world = aimEnv->GetObjectField(mc, g_fTheWorld);
        aimEnv->DeleteLocalRef(mc);

        if (!player || !world) {
          if (player)
            aimEnv->DeleteLocalRef(player);
          if (world)
            aimEnv->DeleteLocalRef(world);
          break;
        }

        double px = aimEnv->GetDoubleField(player, g_fPosX);
        double py = aimEnv->GetDoubleField(player, g_fPosY);
        double pz = aimEnv->GetDoubleField(player, g_fPosZ);
        float eyeH = aimEnv->CallFloatMethod(player, g_mGetEyeHeight);
        float curYaw = aimEnv->GetFloatField(player, g_fRotYaw);
        float curPitch = aimEnv->GetFloatField(player, g_fRotPitch);
        if (aimEnv->ExceptionCheck()) {
          aimEnv->ExceptionClear();
          aimEnv->DeleteLocalRef(player);
          aimEnv->DeleteLocalRef(world);
          break;
        }

        double eyeX = px, eyeY = py + eyeH, eyeZ = pz;

        auto wrapTo180 = [](float v) -> float {
          while (v > 180.f)
            v -= 360.f;
          while (v < -180.f)
            v += 360.f;
          return v;
        };

        auto getAngles = [&](double tx, double ty, double tz, float *outYaw,
                             float *outPitch) {
          double dx = tx - eyeX;
          double dy = ty - eyeY;
          double dz = tz - eyeZ;
          double horiz = sqrt(dx * dx + dz * dz);
          *outYaw = (float)(atan2(dz, dx) * 180.0 / M_PI) - 90.f;
          *outPitch = (float)(-atan2(dy, horiz) * 180.0 / M_PI);
        };

        auto angularDist = [&](float tYaw, float tPitch) -> float {
          float dy = fabsf(wrapTo180(tYaw - curYaw));
          float dp = fabsf(wrapTo180(tPitch - curPitch));
          return dy + dp;
        };

        struct Candidate {
          jobject entity;
          float tYaw;
          float tPitch;
          double dist3d;
          float angDist;
        };
        std::vector<Candidate> candidates;

        jobject entList = aimEnv->GetObjectField(world, g_fEntityList);
        if (entList && !aimEnv->ExceptionCheck()) {
          jint count = aimEnv->CallIntMethod(entList, g_mListSize);
          if (aimEnv->ExceptionCheck()) {
            aimEnv->ExceptionClear();
            count = 0;
          }

          static jclass s_livingClass = nullptr;
          if (!s_livingClass && g_mcClassLoader && g_loadClassMethod) {
            const char *elbN[] = {"net.minecraft.entity.EntityLivingBase"};
            jclass loc = LoadMCClass(aimEnv, elbN, 1);
            if (loc)
              s_livingClass = (jclass)aimEnv->NewGlobalRef(loc);
            if (loc)
              aimEnv->DeleteLocalRef(loc);
          }

          for (jint i = 0; i < count && i < 200; i++) {
            jobject ent = aimEnv->CallObjectMethod(entList, g_mListGet, i);
            if (!ent || aimEnv->ExceptionCheck()) {
              aimEnv->ExceptionClear();
              continue;
            }
            if (s_livingClass && !aimEnv->IsInstanceOf(ent, s_livingClass)) {
              aimEnv->DeleteLocalRef(ent);
              continue;
            }
            if (aimEnv->IsSameObject(ent, player)) {
              aimEnv->DeleteLocalRef(ent);
              continue;
            }
            jboolean dead = aimEnv->GetBooleanField(ent, g_fIsDead);
            if (aimEnv->ExceptionCheck()) {
              aimEnv->ExceptionClear();
              aimEnv->DeleteLocalRef(ent);
              continue;
            }
            if (dead) {
              aimEnv->DeleteLocalRef(ent);
              continue;
            }

            double ex = aimEnv->GetDoubleField(ent, g_fPosX);
            double ey = aimEnv->GetDoubleField(ent, g_fPosY);
            double ez = aimEnv->GetDoubleField(ent, g_fPosZ);
            if (aimEnv->ExceptionCheck()) {
              aimEnv->ExceptionClear();
              aimEnv->DeleteLocalRef(ent);
              continue;
            }

            float entEyeH = aimEnv->CallFloatMethod(ent, g_mGetEyeHeight);
            if (aimEnv->ExceptionCheck()) {
              aimEnv->ExceptionClear();
              entEyeH = 1.62f;
            }
            double aimY = ey + entEyeH * aimHeightFactor;

            double dx = ex - px, dy2 = aimY - py, dz = ez - pz;
            double dist3d = sqrt(dx * dx + dy2 * dy2 + dz * dz);
            if (dist3d > (double)aimDistance) {
              aimEnv->DeleteLocalRef(ent);
              continue;
            }

            float tYaw, tPitch;
            getAngles(ex, aimY, ez, &tYaw, &tPitch);

            float yawDelta = fabsf(wrapTo180(tYaw - curYaw));
            if (yawDelta > aimAngleDeg) {
              aimEnv->DeleteLocalRef(ent);
              continue;
            }

            candidates.push_back(
                {ent, tYaw, tPitch, dist3d, angularDist(tYaw, tPitch)});
          }
          aimEnv->DeleteLocalRef(entList);
        } else {
          if (aimEnv->ExceptionCheck())
            aimEnv->ExceptionClear();
        }

        jobject chosenEnt = nullptr;
        float chosenYaw = 0, chosenPitch = 0;

        if (aimAssistMode == 0) {
          if (aimLockOnActive && g_aimLockedEntity) {
            bool stillAlive = false;
            for (auto &c : candidates) {
              if (aimEnv->IsSameObject(c.entity, g_aimLockedEntity)) {
                chosenEnt = c.entity;
                chosenYaw = c.tYaw;
                chosenPitch = c.tPitch;
                stillAlive = true;
                aimLockLostFrames = 0;
                break;
              }
            }
            if (!stillAlive) {
              aimLockLostFrames++;
              bool shouldRelease = aimLockOnTarget ? (aimLockLostFrames > 600)
                                                   : (aimLockLostFrames > 120);
              if (shouldRelease) {
                aimEnv->DeleteGlobalRef(g_aimLockedEntity);
                g_aimLockedEntity = nullptr;
                aimLockOnActive = false;
                aimLockLostFrames = 0;
                aimSmoothingActive = false;
                aimLastFrameTime.QuadPart = 0;
              }
            }
          }

          if (!aimLockOnActive || !g_aimLockedEntity) {
            float bestScore = FLT_MAX;
            for (auto &c : candidates) {
              float score = aimNearest ? (float)c.dist3d : c.angDist;
              if (score < bestScore) {
                bestScore = score;
                chosenEnt = c.entity;
                chosenYaw = c.tYaw;
                chosenPitch = c.tPitch;
              }
            }
            if (chosenEnt) {
              if (g_aimLockedEntity)
                aimEnv->DeleteGlobalRef(g_aimLockedEntity);
              g_aimLockedEntity = aimEnv->NewGlobalRef(chosenEnt);
              aimLockOnActive = true;
              aimLockLostFrames = 0;
            }
          }
        } else {
          static DWORD s_lastSwitchTick = 0;

          float bestScore = FLT_MAX;
          jobject bestEnt = nullptr;
          float bestYaw = 0, bestPitch = 0;
          for (auto &c : candidates) {
            float score =
                (aimSwitchCriteria == 0) ? (float)c.dist3d : c.angDist;
            if (score < bestScore) {
              bestScore = score;
              bestEnt = c.entity;
              bestYaw = c.tYaw;
              bestPitch = c.tPitch;
            }
          }

          if (bestEnt) {
            bool isSameTarget =
                g_aimLockedEntity &&
                aimEnv->IsSameObject(bestEnt, g_aimLockedEntity);
            bool switchAllowed = !aimLockOnTarget &&
                                 ((DWORD)GetTickCount64() - s_lastSwitchTick) >=
                                     (DWORD)(aimSwitchTime * 1000.f);

            if (!aimLockOnActive || !g_aimLockedEntity || isSameTarget ||
                switchAllowed) {
              if (!isSameTarget && switchAllowed && g_aimLockedEntity) {
                aimEnv->DeleteGlobalRef(g_aimLockedEntity);
                g_aimLockedEntity = aimEnv->NewGlobalRef(bestEnt);
                s_lastSwitchTick = (DWORD)GetTickCount64();
              } else if (!g_aimLockedEntity) {
                g_aimLockedEntity = aimEnv->NewGlobalRef(bestEnt);
                s_lastSwitchTick = (DWORD)GetTickCount64();
              }
              aimLockOnActive = true;
              aimLockLostFrames = 0;
            }

            for (auto &c : candidates) {
              if (g_aimLockedEntity &&
                  aimEnv->IsSameObject(c.entity, g_aimLockedEntity)) {
                chosenEnt = c.entity;
                chosenYaw = c.tYaw;
                chosenPitch = c.tPitch;
                break;
              }
            }
            if (!chosenEnt) {
              aimLockLostFrames++;
              if (!aimLockOnTarget && aimLockLostFrames > 120) {
                aimEnv->DeleteGlobalRef(g_aimLockedEntity);
                g_aimLockedEntity = nullptr;
                aimLockOnActive = false;
                aimLockLostFrames = 0;
                aimSmoothingActive = false;
                aimLastFrameTime.QuadPart = 0;
              }
            }
          } else {
            aimLockLostFrames++;
            if (!aimLockOnTarget && aimLockLostFrames > 120) {
              if (g_aimLockedEntity) {
                aimEnv->DeleteGlobalRef(g_aimLockedEntity);
                g_aimLockedEntity = nullptr;
              }
              aimLockOnActive = false;
              aimLockLostFrames = 0;
              aimSmoothingActive = false;
              aimLastFrameTime.QuadPart = 0;
            }
          }
        }

        if (chosenEnt && aimLockOnActive) {

          LARGE_INTEGER now, freq;
          QueryPerformanceCounter(&now);
          QueryPerformanceFrequency(&freq);
          float dt = 0.016f;
          if (aimLastFrameTime.QuadPart > 0 && freq.QuadPart > 0) {
            dt = (float)(now.QuadPart - aimLastFrameTime.QuadPart) /
                 (float)freq.QuadPart;
            if (dt <= 0.0f)
              dt = 0.001f;
            if (dt > 0.1f)
              dt = 0.1f;
          }
          aimLastFrameTime = now;

          double tgtX = aimEnv->GetDoubleField(chosenEnt, g_fPosX);
          double tgtZ = aimEnv->GetDoubleField(chosenEnt, g_fPosZ);
          float tgtEyeH = aimEnv->CallFloatMethod(chosenEnt, g_mGetEyeHeight);
          if (aimEnv->ExceptionCheck()) {
            aimEnv->ExceptionClear();
            tgtEyeH = 1.62f;
          }
          double tgtY = aimEnv->GetDoubleField(chosenEnt, g_fPosY) +
                        tgtEyeH * aimHeightFactor;

          if (aimSmoothingActive) {
            const float posLerp =
                1.0f - expf(-20.0f * dt);
            tgtX = aimPrevTargetX + (tgtX - aimPrevTargetX) * posLerp;
            tgtY = aimPrevTargetY + (tgtY - aimPrevTargetY) * posLerp;
            tgtZ = aimPrevTargetZ + (tgtZ - aimPrevTargetZ) * posLerp;
          }
          aimPrevTargetX = tgtX;
          aimPrevTargetY = tgtY;
          aimPrevTargetZ = tgtZ;
          aimSmoothingActive = true;

          float smYaw, smPitch;
          getAngles(tgtX, tgtY, tgtZ, &smYaw, &smPitch);

          float yawDelta = wrapTo180(smYaw - curYaw);
          float pitchDelta = wrapTo180(smPitch - curPitch);

          float totalDelta = fabsf(yawDelta) + fabsf(pitchDelta);
          float deadZone = 0.15f * (30.0f / (float)(std::max)(aimSpeedInt, 1));
          if (deadZone < 0.05f) deadZone = 0.05f;
          if (totalDelta > deadZone) {

              float hLambda = powf((float)aimSpeedInt / 30.0f, 1.5f) * 12.0f;
              float hAlpha = 1.0f - expf(-hLambda * dt);

              float newYaw = curYaw + yawDelta * hAlpha;
              float newPitch = curPitch;

              if (aimVerticalAim) {
                  float vLambda = powf((float)aimVerticalSpeed / 30.0f, 1.5f) * 12.0f;
                  float vAlpha = 1.0f - expf(-vLambda * dt);
                  newPitch = curPitch + pitchDelta * vAlpha;
                  if (newPitch > 90.f)  newPitch = 90.f;
                  if (newPitch < -90.f) newPitch = -90.f;
              }

              aimEnv->SetFloatField(player, g_fRotYaw, newYaw);
              if (aimVerticalAim)
                  aimEnv->SetFloatField(player, g_fRotPitch, newPitch);

              float prevBlend = 0.4f + 0.3f * (hAlpha);
              if (g_fPrevRotYaw) {
                  float prevYaw = aimEnv->GetFloatField(player, g_fPrevRotYaw);
                  float blendedPrevYaw = prevYaw + (newYaw - prevYaw) * prevBlend;
                  aimEnv->SetFloatField(player, g_fPrevRotYaw, blendedPrevYaw);
              }
              if (g_fPrevRotPitch && aimVerticalAim) {
                  float prevPitch = aimEnv->GetFloatField(player, g_fPrevRotPitch);
                  float blendedPrevPitch = prevPitch + (newPitch - prevPitch) * prevBlend;
                  aimEnv->SetFloatField(player, g_fPrevRotPitch, blendedPrevPitch);
              }
              if (aimEnv->ExceptionCheck()) aimEnv->ExceptionClear();
          }
        } else {

          aimSmoothingActive = false;
          aimLastFrameTime.QuadPart = 0;
        }

        for (auto &c : candidates)
          aimEnv->DeleteLocalRef(c.entity);

        aimEnv->DeleteLocalRef(player);
        aimEnv->DeleteLocalRef(world);
      } while (false);
    }

    entity.modelview.clear();
    entity.projection.clear();
    players.modelview.clear();
    players.projection.clear();
    chest.modelview.clear();
    chest.projection.clear();
    largeChest.modelview.clear();
    largeChest.projection.clear();

    for (auto& ref : g_playerEntityRefs) {
      if (ref.entityRef) {

      }
    }
    g_playerEntityRefs.clear();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();

    glMatrixMode(currentMatrixMode);
  }

  if (pglOrtho)
    pglOrtho(left, right, bottom, top, zNear, zFar);
}

void __stdcall myglTranslatef(float x, float y, float z) {
  if (pglTranslatef)
    pglTranslatef(x, y, z);

  if (sInventory || g_isUnloading) {
    return;
  }

  if (x == 0.5f && y == 0.4375f && z == 0.9375f) {
    savePosition(chest, 0.0F, 0.0625F, -0.4375F);
  }

  if (x == 1.0f && y == 0.4375f && z == 0.9375f) {
    savePosition(largeChest, 0.0F, 0.0625F, -0.4375F);
  }
}

void __stdcall myglScalef(float x, float y, float z) {
  if (pglScalef)
    pglScalef(x, y, z);

  if (sInventory || g_isUnloading) {
    return;
  }

  if (fabsf(x - 1.0f) < 0.01f && fabsf(y - (-1.0f)) < 0.01f &&
      fabsf(z - (-1.0f)) < 0.01f) {
    sChest = true;
  }

  if (fabsf(x - 0.9375f) < 0.01f && fabsf(y - 0.9375f) < 0.01f &&
      fabsf(z - 0.9375f) < 0.01f) {
    sPlayer = true;

    extern volatile DWORD g_renderFrameIndex;
    static DWORD s_lastFrame = 0xFFFFFFFF;
    static int s_scaleCount = 0;

    extern float g_f5SelfViewZ;
    extern float g_f5SelfViewX;

    if (g_renderFrameIndex != s_lastFrame) {
      s_lastFrame = g_renderFrameIndex;
      s_scaleCount = 0;
      g_f5SelfViewZ = -999.0f;
      g_f5SelfViewX = -999.0f;
    }
    s_scaleCount++;

    float mv[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    float candX = mv[0] * 0.0f + mv[4] * (-1.0f) + mv[8] * 0.0f + mv[12];
    float candZ = mv[2] * 0.0f + mv[6] * (-1.0f) + mv[10] * 0.0f + mv[14];

    if (s_scaleCount == 1) {
      g_f5SelfViewX = candX;
      g_f5SelfViewZ = candZ;

      glGetFloatv(GL_PROJECTION_MATRIX, g_camProjection);
      memcpy(g_camModelview, mv, sizeof(g_camModelview));
      g_camMatricesValid = true;
    }

    bool skipLocalFirstPerson =
        (s_scaleCount == 1 && fabsf(candX) < 0.18f && fabsf(candZ) < 0.18f);

    bool skipLocalThirdPerson = (g_f5SelfViewX > -900.0f && g_f5SelfViewZ > -900.0f &&
                                 fabsf(candX - g_f5SelfViewX) < 0.1f &&
                                 fabsf(candZ - g_f5SelfViewZ) < 0.1f);

    if (!skipLocalFirstPerson && !skipLocalThirdPerson && chamsEnabled && !g_isUnloading) {

      glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);
      sChamsActive = true;
    }

    if (!skipLocalFirstPerson && !skipLocalThirdPerson) {
      savePosition(players, 0.0F, -1.0F, 0.0F);
      savePosition(entity, 0.0F, -1.0F, 0.0F);
    }
  } else if (x > 0.1f && y > 0.1f && z > 0.1f) {
    savePosition(entity, 0.0F, -1.0F, 0.0F);
  }
}

void __stdcall myglEnable(unsigned int cap) {
  if (g_isUnloading) {
    if (pglEnable)
      pglEnable(cap);
    return;
  }

  if (pglEnable)
    pglEnable(cap);

  if (cap == GL_LIGHTING && sNickname) {
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1100000);
    sNickname = false;
  }
}

void __stdcall myglDisable(unsigned int cap) {
  if (g_isUnloading) {
    if (pglDisable)
      pglDisable(cap);
    return;
  }

  if (pglDisable)
    pglDisable(cap);

  if (cap == 0x0000803A) {
    if (sChest || sPlayer) {

      if (sPlayer && sChamsActive) {
        glPopAttrib();
        sChamsActive = false;
      }
      if (sChest) {
        sChest = false;
      }
      if (sPlayer) {
        sPlayer = false;
      }
      glDisable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(1, 1100000);
    }
  }
}

BOOL InitializeHooks() {
  HMODULE openglHandle = nullptr;
  int attempts = 0;
  while (!openglHandle && attempts < 50) {
    openglHandle = GetModuleHandleW(L"opengl32.dll");
    if (!openglHandle) {
      Sleep(100);
      attempts++;
    }
  }

  if (!openglHandle) {
    return FALSE;
  }

  pglEnable =
      reinterpret_cast<T_glEnable>(GetProcAddress(openglHandle, "glEnable"));
  pglDisable =
      reinterpret_cast<T_glDisable>(GetProcAddress(openglHandle, "glDisable"));
  HMODULE gdi32Handle = GetModuleHandleW(L"gdi32.dll");
  if (!gdi32Handle)
    gdi32Handle = LoadLibraryW(L"gdi32.dll");
  pSwapBuffers = reinterpret_cast<T_SwapBuffers>(
      gdi32Handle ? GetProcAddress(gdi32Handle, "SwapBuffers") : nullptr);
  pglOrtho =
      reinterpret_cast<T_glOrtho>(GetProcAddress(openglHandle, "glOrtho"));
  pglScalef =
      reinterpret_cast<T_glScalef>(GetProcAddress(openglHandle, "glScalef"));
  pglTranslatef = reinterpret_cast<T_glTranslatef>(
      GetProcAddress(openglHandle, "glTranslatef"));

  if (!pglEnable || !pglDisable || !pSwapBuffers || !pglOrtho || !pglScalef ||
      !pglTranslatef) {
    return FALSE;
  }

  DetourRestoreAfterWith();

  if (DetourTransactionBegin() != NO_ERROR) {
    return FALSE;
  }

  if (DetourUpdateThread(GetCurrentThread()) != NO_ERROR) {
    DetourTransactionAbort();
    return FALSE;
  }

  LONG error = NO_ERROR;
  error |= DetourAttach(reinterpret_cast<void **>(&pglEnable), myglEnable);
  error |= DetourAttach(reinterpret_cast<void **>(&pglDisable), myglDisable);
  error |= DetourAttach(reinterpret_cast<void **>(&pglOrtho), myglOrtho);
  error |=
      DetourAttach(reinterpret_cast<void **>(&pSwapBuffers), mySwapBuffers);
  error |=
      DetourAttach(reinterpret_cast<void **>(&pglTranslatef), myglTranslatef);
  error |= DetourAttach(reinterpret_cast<void **>(&pglScalef), myglScalef);

  if (error != NO_ERROR) {
    DetourTransactionAbort();
    return FALSE;
  }

  return DetourTransactionCommit() == NO_ERROR;
}

BOOL __stdcall DllMain(HINSTANCE handle, DWORD reason, LPVOID reserved) {
  switch (reason) {
  case DLL_PROCESS_ATTACH: {
    g_hModule = handle;
    DisableThreadLibraryCalls(handle);

    HANDLE initThread = CreateThread(
        nullptr, 0,
        [](LPVOID param) -> DWORD {
          Sleep(1000);
          return InitializeHooks() ? 0 : 1;
        },
        nullptr, 0, nullptr);

    if (initThread) {
      CloseHandle(initThread);
    }

    return TRUE;
  }

  case DLL_PROCESS_DETACH: {
    if (!reserved && !g_isUnloading) {

      UnloadDLL();
    }

    return TRUE;
  }
  }

  return TRUE;
}