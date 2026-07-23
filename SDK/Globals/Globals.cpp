#include "Globals.h"
#include "../SDK/GlText/gltext.h"

GL::Font glFont;
HHOOK keyHookHandle = nullptr;

T_glEnable pglEnable = nullptr;
T_glDisable pglDisable = nullptr;
T_glOrtho pglOrtho = nullptr;
T_glScalef pglScalef = nullptr;
T_glTranslatef pglTranslatef = nullptr;
T_SwapBuffers pSwapBuffers = nullptr;

Vector2D screenCenter;
std::vector<Vector2D> entityScreenPositions;

bool showMenu = true;
bool espEnabled = false;
bool xrayEnabled = false;
bool chestEspEnabled = false;
bool tracerEnabled = false;
bool chamsEnabled = false;
bool nametagsEnabled = false;
bool trajectoriesEnabled = false;
bool arraylistEnabled = false;
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

ImVec4 g_currentOutlineColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
ImVec4 g_currentFillColor = ImVec4(1.0f, 0.2f, 0.2f, 0.20f);
float g_currentOutlineWidth = 2.0f;
bool g_currentShowFill = true;
bool g_currentShowOutline = true;
int g_currentBoxStyle = 0;
float g_currentCornerLength = 0.25f;

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
bool aimLockOnTarget = false;
float aimLockedWorldX = 0.0f, aimLockedWorldY = 0.0f, aimLockedWorldZ = 0.0f;
int aimLockLostFrames = 0;
int aimActivation = 0;
bool aimDisableInLiquid = false;
bool aimVerticalAim = false;
int aimSwitchCriteria = 0;
float aimSwitchTime = 2.0f;
float aimLastSwitchTime = 0.0f;
float aimLastMouseX = 0.0f;
float aimLastMouseY = 0.0f;
LARGE_INTEGER aimLastFrameTime = {0};
bool aimSmoothingActive = false;
double aimPrevTargetX = 0.0, aimPrevTargetY = 0.0, aimPrevTargetZ = 0.0;
float g_f5SelfViewZ = -999.0f;
float g_f5SelfViewX = -999.0f;
jobject g_aimLockedEntity = nullptr;
DWORD g_aimLockLostTick = 0;
bool aimbotSwordOnly = false;

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
bool clickerSwordOnly = false;
const char *clickerThreadStatus = "Not started";

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
jfieldID g_fRightClickDelayTimer = nullptr;
bool g_fastPlaceJniReady = false;

bool blockhitEnabled = false;
bool blockhitOnlyWhileClicking = true;
int blockhitDelayMin = 100;
int blockhitDelayMax = 200;
int blockhitBlockChance = 80;
int blockhitHoldLengthMin = 50;
int blockhitHoldLengthMax = 100;
bool blockhitSwordOnly = false;
const char *blockhitThreadStatus = "Not started";

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
const char *hitSelectStatus = "Not started";
bool hitSelectSwordOnly = false;

volatile int g_hsTargetHurtResist = 0;
volatile int g_hsTargetHurtTime = 0;
volatile bool g_hsAimingAtEntity = false;
volatile bool g_hsPlayerOnGround = true;
volatile double g_hsMotionY = 0.0;
volatile bool g_hsTakingKB = false;
volatile bool g_hsNeedFakeSwing = false;
volatile bool g_hsFirstHitReceived = false;
volatile DWORD g_hsLastDamageTime = 0;
volatile DWORD g_hsCombatStartTime = 0;
volatile DWORD g_hsLastAttackTime = 0;
volatile DWORD g_hsLastAimTime = 0;
volatile bool g_hsTargetJustDamaged = false;
volatile bool g_sprintPending = false;
volatile bool g_holdingSword = false;

bool reachEnabled = false;
float reachMin = 3.0f;
float reachMax = 3.5f;
bool reachSprintOnly = false;
bool reachAttackOnly = true;
bool reachDisableInLiquid = false;
bool reachOnlyMoving = false;
bool reachWhileJumping = false;
const char *reachStatus = "Not initialized";
bool reachSwordOnly = false;

bool velocityEnabled = false;
float velocityHorizontal = 80.0f;
float velocityVertical = 100.0f;
float velocityChance = 100.0f;
bool velocityOnlyWhileMoving = false;
bool velocityOnlyOnGround = false;
bool g_velocityJniReady = false;
const char *velocityStatus = "Not initialized";

bool sprintEnabled = false;
bool g_sprintJniReady = false;

bool sprintResetEnabled = false;
int sprintResetMode = 0;
int sprintResetDelay = 250;
int sprintResetStopDuration = 50;
bool sprintResetRandomize = true;
bool sprintResetWaitForDamage = false;
bool sprintResetWeaponOnly = false;
volatile bool g_sprintResetPending = false;
volatile DWORD g_lastAttackTime = 0;

bool speedEnabled = false;
float speedMultiplier = 1.5f;
bool flyEnabled = false;
float flySpeed = 1.0f;
bool noFallEnabled = false;
bool invWalkEnabled = false;
int invWalkMode = 0;
bool g_invWalkJniReady = false;
bool g_noFallJniReady = false;
bool jumpResetEnabled = false;
int jumpResetChance = 100;
bool jumpResetOnlyWhenHurt = true;
bool noJumpDelayEnabled = false;
bool scaffoldEnabled = false;
bool scaffoldTower = false;
bool scaffoldRotate = true;
bool scaffoldLegitMode = false;
bool scaffoldAutoBlock = true;
int scaffoldDelayMs = 60;
float scaffoldMinDist = 0.0f;
bool stepEnabled = false;
int stepMode = 0;
float stepHeight = 1.0f;
bool stepReverseStep = false;

float nametagsSize = 0.5f;
bool nametagsShowHealth = true;
bool nametagsShowArmor = true;
bool nametagsShowWeapon = true;

float trajectoriesLineWidth = 1.5f;
int trajectoriesMaxTicks = 240;
bool trajectoriesLandingCross = true;

bool throwpotEnabled = false;
int throwpotTriggerKey = VK_LSHIFT;
int throwpotPotCount = 2;
int throwpotSwitchDelay = 50;
int throwpotThrowDelay = 50;
int throwpotCooldownMs = 300;
bool throwpotSwordOnly = false;
bool g_throwpotKeyHeld = false;
DWORD g_throwpotLastThrow = 0;
const char *throwpotStatus = "Not started";

bool autosoupEnabled = false;
int autosoupTriggerKey = VK_LCONTROL;
int autosoupSwitchDelay = 50;
int autosoupEatDelay = 50;
int autosoupCooldownMs = 200;
bool autosoupDropBowls = true;
bool autosoupSwordOnly = false;
bool g_autosoupKeyHeld = false;
DWORD g_autosoupLastEat = 0;
const char *autosoupStatus = "Not started";

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
bool g_refillKeyHeld = false;
const char *refillStatus = "Not started";

bool autoToolEnabled = false;
int autoToolActivationDelay = 0;
bool autoToolSwitchBack = true;
bool autoToolAllowSword = false;
bool autoToolAllowTool = true;
bool autoToolAllowFists = true;
bool autoToolAllowOther = false;
bool autoToolSneakOnly = false;

bool noPacketEnabled = false;
bool pingSpoofEnabled = false;
int pingSpoofAmount = 50;

float arraylistFontScale = 1.28f;
float arraylistBgAlpha = 0.90f;
float arraylistBarWidth = 3.0f;
bool arraylistShowInfo = true;
bool arraylistRainbow = false;
float arraylistRainbowSpeed = 1.0f;
ImVec4 arraylistAccentColor = ImVec4(0.88f, 0.88f, 0.95f, 1.0f);
ImVec4 arraylistTextColor = ImVec4(0.93f, 0.93f, 0.96f, 1.0f);
ImVec4 arraylistInfoColor = ImVec4(0.50f, 0.50f, 0.56f, 1.0f);
int arraylistPosition = 0;
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
const char *g_clientName = "Vengeance";
const char *g_clientVersion = "v1.0.8";

std::vector<NTEntityData> g_ntEntityData;
std::vector<NTEntityData> g_ntEntityDataReady;
std::vector<ESPOverlayData> g_espOverlays;
std::vector<ESPOverlayData> g_espOverlaysReady;
std::vector<EntityRefData> g_playerEntityRefs;
std::vector<EntityRefData> g_playerEntityRefsReady;

volatile int g_trajHeldItemType = 0;
volatile float g_trajYaw = 0, g_trajPitch = 0;
volatile float g_trajEyeX = 0, g_trajEyeY = 0, g_trajEyeZ = 0;
volatile float g_trajBowPower = 1.0f;
float g_camProjection[16] = {};
float g_camModelview[16] = {};
bool g_camMatricesValid = false;

float g_menuFadeAnim = 0.0f;
float g_menuScaleAnim = 0.0f;
int g_lastCategory = 0;
float g_cardAppearTimer = 0.0f;
bool g_themeApplied = false;
std::vector<NetworkNode> g_netNodes;
bool g_nodesInit = false;
float g_globalTime = 0.0f;
float g_breathPhase = 0.0f;
std::map<int, float> g_cardBounce;
std::map<int, float> g_cardHoverGlow;
std::map<int, float> g_cardPressAnim;
std::map<int, float> g_cardRippleAnim;
std::map<int, ImVec2> g_cardRipplePos;
std::map<std::string, float> g_elementAppear;
std::map<int, float> g_iconAppearAnim;
std::map<int, float> g_dotEnabledAnim;
std::map<std::string, float> g_subSettingsAnim;

bool showCombatWindow = false;
bool showVisualsWindow = false;
bool showMovementWindow = false;
bool showNetworkWindow = false;
bool showUtilitiesWindow = false;
bool showSettingsWindow = false;

std::vector<std::string> enabledModules;

std::vector<CategoryInfo> g_categories = {
    {"Combat",
     {{"Aim Assist", "Aim assistance"},
      {"Left Clicker", "CPS, Pattern"},
      {"Blockhit", "Auto block"},
      {"Reach", "Extended range"},
      {"Hit Select", "Filter clicks"}}},
    {"Movement",
     {{"Velocity", "Knockback control"},
      {"Sprint", "Always sprint"},
      {"Sprint Reset", "Max knockback hits"},
      {"No Fall", "Negate fall damage"},
      {"Inv Walk", "Move in inventory"},
      {"Jump Reset", "Auto jump on KB"},
      {"No Jump Delay", "Remove jump cooldown"},
      {"Scaffold", "Auto place blocks"},
      {"Step", "Step up blocks"}}},
    {"Visuals",
     {{"ESP", "Player boxes"},
      {"Tracer", "Player lines"},
      {"XRay", "See through"},
      {"Chest ESP", "Chest highlight"},
      {"ArrayList", "Module list"},
      {"Chams", "Players through walls"}}},
    {"Network", {}},
    {"Utilities",
     {{"Right Clicker", "Right click CPS"},
      {"Fast Place", "No place delay"},
      {"Throwpot", "Auto throw potions"},
      {"AutoSoup", "Auto eat soup"},
      {"Refill", "Auto refill hotbar"},
      {"Auto Tool", "Auto select tool"}}},
    {"Settings", {{"Unload", "Unload Vengeance"}}}};

int g_selectedCategory = 0;
int g_openModuleCat = -1;
int g_openModuleIdx = -1;
float g_settingsPanelAnim = 0.0f;
float g_tabAnims[6] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

std::map<ImGuiID, float> g_animState;

std::vector<KeyBind> g_keybinds;
bool g_isCapturingKey = false;
int g_capturingIndex = -1;

bool g_unloadRequested = false;
bool g_isUnloading = false;
HMODULE g_hModule = nullptr;
HANDLE g_keyDispatcherThread = nullptr;
DWORD g_keyDispatcherThreadId = 0;
HANDLE g_moduleThreads[16] = {nullptr};
int g_moduleThreadCount = 0;
CRITICAL_SECTION g_unloadCS;
bool g_unloadCSInitialized = false;
bool g_threadsSpawned = false;

bool g_imguiInitialized = false;
HWND g_imguiHwnd = nullptr;
bool g_wndProcHooked = false;
WNDPROC g_originalWndProc = nullptr;
HWND g_targetWindow = nullptr;
bool g_fullscreenDetected = false;
RECT g_lastWindowRect = {0};
bool g_wasFullscreen = false;
bool g_cursorCaptured = false;
volatile DWORD g_renderFrameIndex = 0;
POINT g_lastCursorPos = {0};
bool g_blockMouseMovement = false;
bool g_cursorVisible = false;
DWORD g_lastEKeyTime = 0;
DWORD g_lastEscKeyTime = 0;
bool g_isInventoryOpen = false;

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

jfieldID g_fObjectMouseOver = nullptr;
jfieldID g_fEntityHit = nullptr;
jfieldID g_fHurtResistantTime = nullptr;
jfieldID g_fHurtTime = nullptr;

jmethodID g_mGetCurrentItem = nullptr;
jmethodID g_mStackGetItem = nullptr;
jmethodID g_mStackGetDisplayName = nullptr;
jclass g_itemSwordClass = nullptr;
bool g_swordCheckReady = false;

jfieldID g_fHealth = nullptr;
jfieldID g_fInventory = nullptr;
jfieldID g_fCurrentItemIdx = nullptr;
jmethodID g_mSendUseItem = nullptr;
jmethodID g_mGetStackInSlot = nullptr;
jmethodID g_mDropOneItem = nullptr;
jmethodID g_mDisplayGuiScreen = nullptr;
jmethodID g_mCloseScreen = nullptr;
jclass g_itemPotionClass = nullptr;
jmethodID g_mIsSplash = nullptr;
jclass g_inventoryClass = nullptr;
jclass g_itemClass = nullptr;

jfieldID g_fMotionX = nullptr;
jfieldID g_fMotionY = nullptr;
jfieldID g_fMotionZ = nullptr;
jfieldID g_fOnGround = nullptr;

jobject g_mcClassLoader = nullptr;
jmethodID g_loadClassMethod = nullptr;

jfieldID g_fIsSprinting = nullptr;
jmethodID g_mSetSprinting = nullptr;

jfieldID g_fGameSettings = nullptr;
jfieldID g_fKeyBindForward = nullptr;
jfieldID g_fKeyBindBack = nullptr;
jfieldID g_fKeyBindLeft = nullptr;
jfieldID g_fKeyBindRight = nullptr;
jfieldID g_fKeyBindJump = nullptr;
jfieldID g_fKeyBindSneak = nullptr;
jfieldID g_fKeyBindPressed = nullptr;

jfieldID g_fFallDistance = nullptr;
jfieldID g_fSendQueue = nullptr;
jmethodID g_mAddToSendQueue = nullptr;
jclass g_c03PacketClass = nullptr;
jmethodID g_c03PacketInit = nullptr;

jclass g_itemBowClass = nullptr;
jclass g_itemEnderPearlClass = nullptr;
jclass g_itemSnowballClass = nullptr;
jclass g_itemEggClass = nullptr;

jfieldID g_fJumpTicks = nullptr;
jfieldID g_fStepHeight = nullptr;
jclass g_c03PosClass = nullptr;
jmethodID g_c03PosInit = nullptr;
jclass g_itemBlockClass = nullptr;

jmethodID g_mAddScheduledTask = nullptr;
jclass g_callableClass = nullptr;

jmethodID g_mRightClickMouse = nullptr;
jmethodID g_mOnPlayerRightClick = nullptr;
jclass g_vec3Class = nullptr;
jmethodID g_vec3Init = nullptr;
jclass g_enumFacingClass = nullptr;
jobject g_enumFacingByIndex[6] = {nullptr, nullptr, nullptr,
                                  nullptr, nullptr, nullptr};

jmethodID g_mGetStrVsBlock = nullptr;
jmethodID g_mGetBlockState = nullptr;
jclass g_blockPosClass = nullptr;
jmethodID g_blockPosInit = nullptr;
jmethodID g_mGetBlock = nullptr;
jmethodID g_mIsAirBlock = nullptr;
jfieldID g_fIsSneaking = nullptr;
jmethodID g_mIsSneaking = nullptr;
jmethodID g_mGetHealth = nullptr;
jmethodID g_mGetMaxHealth = nullptr;
jclass g_entityLivingClass = nullptr;
jclass g_entityPlayerClass = nullptr;
jmethodID g_mGetName = nullptr;
jmethodID g_mGetArmorValue = nullptr;
jmethodID g_mGetItemInUseDuration = nullptr;
jmethodID g_mGetEquipmentInSlot = nullptr;
jmethodID g_mGetMaxDamage = nullptr;
jmethodID g_mGetItemDamage = nullptr;
