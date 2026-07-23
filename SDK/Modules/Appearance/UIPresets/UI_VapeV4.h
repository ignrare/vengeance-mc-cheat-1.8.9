#pragma once

void RenderModuleSettings(int cat, int mod);

#include "imgui.h"
#include "imgui_internal.h"
#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>

extern bool aimbotEnabled, clickerEnabled, blockhitEnabled, reachEnabled;
extern bool hitSelectEnabled, sTapEnabled, wTapEnabled, bowAimbotEnabled;
extern bool velocityEnabled, sprintEnabled, sprintResetEnabled, noFallEnabled;
extern bool invWalkEnabled, jumpResetEnabled, noJumpDelayEnabled, scaffoldEnabled;
extern bool stepEnabled, spiderEnabled, safeWalkEnabled, reverseStepEnabled;
extern bool bouncySlimeEnabled, fastStopEnabled, glideEnabled, flightEnabled;
extern bool airJumpEnabled, highJumpEnabled;
extern bool espEnabled, tracerEnabled, xrayEnabled, storageEspEnabled;
extern bool arraylistEnabled, chamsEnabled, targetHudEnabled, freecamEnabled;
extern bool fullBrightEnabled, noHurtCamEnabled, zoomEnabled;
extern bool armorEspEnabled, itemEspEnabled, holeEspEnabled;
extern bool blinkEnabled, timerEnabled, fakeLagEnabled;
extern bool rightClickerEnabled, fastPlaceEnabled, throwpotEnabled, autosoupEnabled;
extern bool refillEnabled, autoToolEnabled, bridgeAssistEnabled, teamsEnabled;
extern bool friendsEnabled, autoSwordEnabled, antiVoidEnabled;
extern bool middleClickFriendEnabled, antiBotEnabled, autoArmorEnabled;
extern bool inventoryManagerEnabled;
extern bool breadCrumbsEnabled, antiAfkEnabled, antiBadEffectsEnabled, panicEnabled;
extern bool pingSpoofModEnabled, reconnectEnabled, staffNotifierEnabled;
extern bool noRotateEnabled, antiCactusEnabled;
extern bool uiPresetsEnabled, g_useCustomUI;
extern int  uiPresetsSelected;

extern float  aimDistance, aimAngleDeg, aimVerticalSpeed, aimHeightFactor;
extern int    aimSpeedInt;
extern bool   aimVerticalAim, aimLockOnTarget, aimNearest, aimDisableInLiquid;
extern int    clickerCpsInt, clickerSpikeChance, clickerExhaustChance;
extern bool   clickerEnableRandomization, clickerSpikeMode, clickerExhaustMode;
extern double clickerRandomizationAmount, clickerSpikeIncreaseCps, clickerExhaustDropCps;
extern int    blockhitDelayMin, blockhitDelayMax, blockhitBlockChance;
extern int    blockhitHoldLengthMin, blockhitHoldLengthMax;
extern float  reachMin, reachMax;
extern int    reachChance;
extern bool   reachAttackOnly, reachSprintOnly, reachWhileJumping;
extern bool   reachOnlyMoving, reachDisableInLiquid, reachMotionCompensation;
extern float  stepHeight, spiderSpeed;

extern float  velocityHorizontal, velocityVertical, velocityChance;
extern bool   velocityOnlyWhileMoving, velocityOnlyOnGround;
extern int    jumpResetChance; extern bool jumpResetOnlyWhenHurt;
extern bool   scaffoldLegitMode, scaffoldAutoBlock;
extern int    scaffoldDelayMs; extern float scaffoldMinDist;
extern float  glideSpeed, flightSpeed, reverseStepHeight, highJumpMotion;
extern int    flightMode;

extern float  timerSpeed; extern int fakeLagDelay;

extern int    throwpotPotCount, throwpotSwitchDelay, throwpotThrowDelay, throwpotCooldownMs;
extern bool   throwpotSwordOnly;
extern int    autosoupSwitchDelay, autosoupEatDelay, autosoupCooldownMs;
extern bool   autosoupDropBowls, autosoupSwordOnly;
extern bool   refillSoup, refillPotion, refillSmartSpeed, refillCloseOnComplete, refillDisableOnComplete, refillRandomSlots;
extern int    refillDelayAfterOpen, refillDelayBeforeClose, refillSpeed;
extern int    autoToolActivationDelay;
extern bool   autoToolSwitchBack, autoToolSneakOnly, autoToolAllowSword, autoToolAllowTool, autoToolAllowFists;
extern bool   bridgeAssistRmbOnly, bridgeAssistBackwardOnly, bridgeAssistBlocksOnly;
extern float  bridgeAssistPitchThreshold; extern int bridgeAssistDelay;
extern float  antiVoidDistance; extern int antiBotMode, invManagerDelay;
extern int    pingSpoofModAmount, reconnectDelay;

extern float  arraylistFontScale, arraylistBgAlpha;
extern bool   arraylistShowInfo, arraylistRainbow;
extern int    arraylistPosition;
extern float  freecamSpeed, zoomFov;
extern int    fullBrightMode;

extern int    hitSelectMode, hitSelectPauseDuration, hitSelectWaitForFirstHit;
extern int    hitSelectCancelRateInCombat, hitSelectCancelRateMissed;
extern bool   hitSelectDisableDuringKB, hitSelectOnlyWhileDamaged;
extern bool   hitSelectUseServerAttackTime, hitSelectFakeSwing, hitSelectSwordOnly;

namespace UIVapeV4
{

static constexpr ImU32 C_BG   = IM_COL32( 16, 16, 16,245);
static constexpr ImU32 C_HDR  = IM_COL32( 24, 24, 24,255);
static constexpr ImU32 C_BORD = IM_COL32( 48, 48, 48,255);
static constexpr ImU32 C_BRDH = IM_COL32( 64, 64, 64,255);
static constexpr ImU32 C_ROWH = IM_COL32( 30, 30, 30,255);
static constexpr ImU32 C_DIV  = IM_COL32( 36, 36, 36,255);
static constexpr ImU32 C_TXT  = IM_COL32(215,215,215,255);
static constexpr ImU32 C_DIM  = IM_COL32(120,120,120,255);
static constexpr ImU32 C_HINT = IM_COL32( 62, 62, 62,255);
static constexpr ImU32 C_TOF  = IM_COL32( 50, 50, 50,255);
static constexpr ImU32 C_TON  = IM_COL32(200,200,200,255);
static constexpr ImU32 C_SLF  = IM_COL32(185,185,185,255);
static constexpr ImU32 C_SLB  = IM_COL32( 38, 38, 38,255);
static constexpr ImU32 C_SHD  = IM_COL32(  0,  0,  0, 70);

static ImU32 Lerp4(ImU32 a, ImU32 b, float t) {
    if (t <= 0.f) return a; if (t >= 1.f) return b;
    auto L=[](int x,int y,float t){return (int)(x+(y-x)*t);};
    return IM_COL32(L(a&0xFF,b&0xFF,t),L((a>>8)&0xFF,(b>>8)&0xFF,t),
                    L((a>>16)&0xFF,(b>>16)&0xFF,t),L((a>>24)&0xFF,(b>>24)&0xFF,t));
}
inline bool Hov(ImVec2 p,ImVec2 a,ImVec2 b){return p.x>=a.x&&p.x<=b.x&&p.y>=a.y&&p.y<=b.y;}

static struct DragState{int ci,m,s;float x,w,lo,hi;void*p;int tp;bool active;}g_drag{};

struct ModDef{
    const char*name;bool*en;int nc;
    const char*cl[8];void*cp[8];int ct[8];float lo[8],hi[8];const char*cf[8];
};

static bool s_cEn=false, s_pEn=false;

static ModDef catCombat[]={
    {"AimAssist",&aimbotEnabled,4,
     {"Distance","Speed","Angle","Vertical"},
     {&aimDistance,&aimSpeedInt,&aimAngleDeg,&aimVerticalAim},
     {0,1,0,2},{1,1,1,0},{20,90,180,1},{"%.1f","%d","%.0f",nullptr}},
    {"AutoClicker",&clickerEnabled,3,
     {"CPS","Randomize","Variance"},
     {&clickerCpsInt,&clickerEnableRandomization,&clickerRandomizationAmount},
     {1,2,3},{1,0,0},{25,1,5},{"%d",nullptr,"%.1f"}},
    {"BlockHit",&blockhitEnabled,3,
     {"Delay Min","Delay Max","Chance"},
     {&blockhitDelayMin,&blockhitDelayMax,&blockhitBlockChance},
     {1,1,1},{50,100,1},{500,1000,100},{"%d ms","%d ms","%d%%"}},
    {"Reach",&reachEnabled,3,
     {"Min","Max","Chance"},
     {&reachMin,&reachMax,&reachChance},
     {0,0,1},{3,3,0},{6,6,100},{"%.2f","%.2f","%d%%"}},
    {"HitSelect",&hitSelectEnabled,6,
     {"Mode","Pause","Wait1stHit","ServerAtk","FakeSwing","SwordOnly"},
     {&hitSelectMode,&hitSelectPauseDuration,&hitSelectWaitForFirstHit,
      &hitSelectUseServerAttackTime,&hitSelectFakeSwing,&hitSelectSwordOnly},
     {1,1,1,2,2,2},{0,50,0,0,0,0},{1,1000,1000,1,1,1},{"%d","%d ms","%d ms",nullptr,nullptr,nullptr}},
    {"S-Tap",&sTapEnabled,5,
     {"Chance","Min Delay","Max Delay","Adaptive","Sprint Only"},
     {&sTapConfig.chance,&sTapConfig.delayMin,&sTapConfig.delayMax,
      &sTapConfig.adaptiveDelay,&sTapConfig.onlyWhileSprinting},
     {0,1,1,2,2},{0,10,10,0,0},{100,300,500,1,1},{"%.0f%%","%d ms","%d ms",nullptr,nullptr}},
    {"W-Tap",&wTapEnabled,5,
     {"Chance","Min Ticks","Max Ticks","Legit","Adaptive"},
     {&wTapConfig.chance,&wTapConfig.durationMin,&wTapConfig.durationMax,
      &wTapConfig.legitMode,&wTapConfig.adaptiveChance},
     {0,1,1,2,2},{0,1,1,0,0},{100,10,10,1,1},{"%.0f%%","%d","%d",nullptr,nullptr}},
    {"BowAimbot",&bowAimbotEnabled,6,
     {"FOV","Range","Smoothing","Prediction","AutoShoot","OnlyPlayers"},
     {&bowAimbotConfig.fov,&bowAimbotConfig.range,&bowAimbotConfig.smoothing,
      &bowAimbotConfig.predictionStrength,&bowAimbotConfig.autoShoot,&bowAimbotConfig.onlyPlayers},
     {0,0,0,0,2,2},{10,10,1,0,0,0},{360,128,20,2,1,1},{"%.0f\xC2\xB0","%.0f","%.1f","%.2f",nullptr,nullptr}},
};
static ModDef catMovement[]={
    {"Velocity",&velocityEnabled,5,
     {"Horizontal","Vertical","Chance","Mov Only","Gnd Only"},
     {&velocityHorizontal,&velocityVertical,&velocityChance,&velocityOnlyWhileMoving,&velocityOnlyOnGround},
     {0,0,0,2,2},{0,0,0,0,0},{100,100,100,1,1},{"%.0f%%","%.0f%%","%.0f%%",nullptr,nullptr}},
    {"Sprint",&sprintEnabled,0,{},{},{},{},{},{}},
    {"SprintReset",&sprintResetEnabled,0,{},{},{},{},{},{}},
    {"NoFall",&noFallEnabled,0,{},{},{},{},{},{}},
    {"InvWalk",&invWalkEnabled,0,{},{},{},{},{},{}},
    {"JumpReset",&jumpResetEnabled,2,
     {"Chance","On Hurt"},{&jumpResetChance,&jumpResetOnlyWhenHurt},
     {1,2},{0,0},{100,1},{"%d%%",nullptr}},
    {"NoJumpDelay",&noJumpDelayEnabled,0,{},{},{},{},{},{}},
    {"Scaffold",&scaffoldEnabled,3,
     {"Delay","Legit","AutoBlk"},
     {&scaffoldDelayMs,&scaffoldLegitMode,&scaffoldAutoBlock},
     {1,2,2},{0,0,0},{500,1,1},{"%d ms",nullptr,nullptr}},
    {"Step",&stepEnabled,1,{"Height"},{&stepHeight},{0},{1},{2},{"%.1f"}},
    {"Spider",&spiderEnabled,1,{"Speed"},{&spiderSpeed},{0},{0},{1},{"%.2f"}},
    {"SafeWalk",&safeWalkEnabled,0,{},{},{},{},{},{}},
    {"ReverseStep",&reverseStepEnabled,1,{"Height"},{&reverseStepHeight},{0},{0.5f},{2},{"%.1f"}},
    {"BouncySlime",&bouncySlimeEnabled,0,{},{},{},{},{},{}},
    {"FastStop",&fastStopEnabled,0,{},{},{},{},{},{}},
    {"Glide",&glideEnabled,1,{"Speed"},{&glideSpeed},{0},{0.01f},{0.1f},{"%.3f"}},
    {"Flight",&flightEnabled,1,{"Speed"},{&flightSpeed},{0},{0.1f},{10},{"%.1f"}},
    {"AirJump",&airJumpEnabled,0,{},{},{},{},{},{}},
    {"HighJump",&highJumpEnabled,1,{"Motion"},{&highJumpMotion},{0},{0.42f},{2},{"%.2f"}},
};
static ModDef catVisuals[]={
    {"ESP",&espEnabled,0,{},{},{},{},{},{}},
    {"Tracers",&tracerEnabled,0,{},{},{},{},{},{}},
    {"XRay",&xrayEnabled,0,{},{},{},{},{},{}},
    {"StorageESP",&storageEspEnabled,0,{},{},{},{},{},{}},
    {"ArrayList",&arraylistEnabled,4,
     {"FntScale","BgAlpha","ShowInf","Rainbow"},
     {&arraylistFontScale,&arraylistBgAlpha,&arraylistShowInfo,&arraylistRainbow},
     {0,0,2,2},{0.5f,0,0,0},{2,1,1,1},{"%.2f","%.2f",nullptr,nullptr}},
    {"Chams",&chamsEnabled,0,{},{},{},{},{},{}},
    {"TargetHUD",&targetHudEnabled,0,{},{},{},{},{},{}},
    {"Freecam",&freecamEnabled,1,{"Speed"},{&freecamSpeed},{0},{0.05f},{2},{"%.2f"}},
    {"FullBright",&fullBrightEnabled,0,{},{},{},{},{},{}},
    {"NoHurtCam",&noHurtCamEnabled,0,{},{},{},{},{},{}},
    {"Zoom",&zoomEnabled,1,{"FOV"},{&zoomFov},{0},{10},{100},{"%.0f"}},
    {"ArmorESP",&armorEspEnabled,0,{},{},{},{},{},{}},
    {"ItemESP",&itemEspEnabled,0,{},{},{},{},{},{}},
    {"HoleESP",&holeEspEnabled,0,{},{},{},{},{},{}},
};
static ModDef catNetwork[]={
    {"Blink",&blinkEnabled,0,{},{},{},{},{},{}},
    {"Timer",&timerEnabled,1,{"Speed"},{&timerSpeed},{0},{0.1f},{5},{"%.2fx"}},
    {"FakeLag",&fakeLagEnabled,1,{"Delay"},{&fakeLagDelay},{1},{10},{500},{"%d ms"}},
};
static ModDef catUtils[]={
    {"RightClicker",&rightClickerEnabled,0,{},{},{},{},{},{}},
    {"FastPlace",&fastPlaceEnabled,0,{},{},{},{},{},{}},
    {"Throwpot",&throwpotEnabled,5,
     {"Pots","SwDelay","ThDelay","CoolMS","Sword"},
     {&throwpotPotCount,&throwpotSwitchDelay,&throwpotThrowDelay,&throwpotCooldownMs,&throwpotSwordOnly},
     {1,1,1,1,2},{1,50,50,500,0},{9,500,500,5000,1},{"%d","%d ms","%d ms","%d ms",nullptr}},
    {"AutoSoup",&autosoupEnabled,4,
     {"SwDelay","EatDelay","CoolMS","Sword"},
     {&autosoupSwitchDelay,&autosoupEatDelay,&autosoupCooldownMs,&autosoupSwordOnly},
     {1,1,1,2},{50,50,500,0},{500,500,5000,1},{"%d ms","%d ms","%d ms",nullptr}},
    {"Refill",&refillEnabled,5,
     {"OpenDly","CloseDly","Soup","Potion","Smart"},
     {&refillDelayAfterOpen,&refillDelayBeforeClose,&refillSoup,&refillPotion,&refillSmartSpeed},
     {1,1,2,2,2},{0,0,0,0,0},{1000,1000,1,1,1},{"%d ms","%d ms",nullptr,nullptr,nullptr}},
    {"AutoTool",&autoToolEnabled,3,
     {"ActDly","SwtBack","Sneak"},
     {&autoToolActivationDelay,&autoToolSwitchBack,&autoToolSneakOnly},
     {1,2,2},{0,0,0},{500,1,1},{"%d ms",nullptr,nullptr}},
    {"BridgeAssist",&bridgeAssistEnabled,4,
     {"Delay","RMB","Back","Blks"},
     {&bridgeAssistDelay,&bridgeAssistRmbOnly,&bridgeAssistBackwardOnly,&bridgeAssistBlocksOnly},
     {1,2,2,2},{0,0,0,0},{500,1,1,1},{"%d ms",nullptr,nullptr,nullptr}},
    {"Teams",&teamsEnabled,0,{},{},{},{},{},{}},
    {"Friends",&friendsEnabled,0,{},{},{},{},{},{}},
    {"AutoSword",&autoSwordEnabled,0,{},{},{},{},{},{}},
    {"AntiVoid",&antiVoidEnabled,1,{"Dist"},{&antiVoidDistance},{0},{1},{20},{"%.0f"}},
    {"MCFriend",&middleClickFriendEnabled,0,{},{},{},{},{},{}},
    {"AntiBot",&antiBotEnabled,0,{},{},{},{},{},{}},
    {"AutoArmor",&autoArmorEnabled,0,{},{},{},{},{},{}},
    {"InvManager",&inventoryManagerEnabled,1,{"Delay"},{&invManagerDelay},{1},{50},{500},{"%d ms"}},
};
static ModDef catMisc[]={
    {"BreadCrumbs",&breadCrumbsEnabled,0,{},{},{},{},{},{}},
    {"AntiAFK",&antiAfkEnabled,0,{},{},{},{},{},{}},
    {"AntiBadFX",&antiBadEffectsEnabled,0,{},{},{},{},{},{}},
    {"Panic",&panicEnabled,0,{},{},{},{},{},{}},
    {"PingSpoof",&pingSpoofModEnabled,1,{"Amount"},{&pingSpoofModAmount},{1},{0},{1000},{"%d ms"}},
    {"Reconnect",&reconnectEnabled,1,{"Delay"},{&reconnectDelay},{1},{1000},{30000},{"%d ms"}},
    {"StaffNotify",&staffNotifierEnabled,0,{},{},{},{},{},{}},
    {"NoRotate",&noRotateEnabled,0,{},{},{},{},{},{}},
    {"AntiCactus",&antiCactusEnabled,0,{},{},{},{},{},{}},
};
static ModDef catAppearance[]={
    {"Colors",&s_cEn,0,{},{},{},{},{},{}},
    {"Presets",&s_pEn,0,{},{},{},{},{},{}},
    {"UI Presets",&uiPresetsEnabled,1,
     {"Style"},{&uiPresetsSelected},{1},{0},{3},{"%d"}},
};

struct CatDef{const char*name;const char*sym;ModDef*mods;int count;};
static CatDef cats[]={
    {"Combat",    "C",catCombat,    8},
    {"Movement",  "M",catMovement, 18},
    {"Visuals",   "V",catVisuals,  14},
    {"Network",   "N",catNetwork,   3},
    {"Utilities", "U",catUtils,    15},
    {"Misc",      "X",catMisc,      9},
    {"Appearance","A",catAppearance,3},
};
static constexpr int CAT_COUNT=7, MAX_MODS=18;

static constexpr int V4_RMS[7] = {1,2,3,4,5,6,8};

static void V4PushTheme() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg,        ImVec4(0.06f,0.06f,0.06f,1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,         ImVec4(0.10f,0.10f,0.10f,1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  ImVec4(0.16f,0.16f,0.16f,1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,   ImVec4(0.22f,0.22f,0.22f,1.f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,       ImVec4(0.90f,0.90f,0.90f,1.f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,      ImVec4(0.80f,0.80f,0.80f,1.f));
    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.14f,0.14f,0.14f,1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(0.22f,0.22f,0.22f,1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(0.30f,0.30f,0.30f,1.f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,     ImVec4(0.04f,0.04f,0.04f,1.f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,   ImVec4(0.22f,0.22f,0.22f,1.f));
    ImGui::PushStyleColor(ImGuiCol_Text,            ImVec4(0.88f,0.88f,0.88f,1.f));
    ImGui::PushStyleColor(ImGuiCol_Separator,       ImVec4(0.25f,0.25f,0.25f,1.f));
}
static void V4PopTheme() { ImGui::PopStyleColor(13); }

struct ColState{
    ImVec2 pos;bool dragging;ImVec2 dragOff;bool visible;
    bool  exp[MAX_MODS];
    float hov[MAX_MODS],tog[MAX_MODS],ean[MAX_MODS];
};
static ColState  cols[CAT_COUNT];
static bool      initDone=false;
static ColState* pDrag=nullptr;

static void Init(){
    if(initDone)return;initDone=true;
    for(int i=0;i<CAT_COUNT;i++){cols[i]={};cols[i].pos={180.f+i*205.f,60.f};}
}

static void RenderColumn(ImDrawList*dl,ImVec2 ms,bool lc,int ci)
{
    ColState&col=cols[ci]; CatDef&cat=cats[ci];
    if(!col.visible)return;
    if(col.dragging){
        if(ImGui::IsMouseDown(0))col.pos={ms.x-col.dragOff.x,ms.y-col.dragOff.y};
        else{col.dragging=false;pDrag=nullptr;}
    }
    const float W=212.f,HH=32.f,RH=30.f,SH=28.f,SPX=12.f;
    const float sp=std::min(1.f,10.f*ImGui::GetIO().DeltaTime);

    const float EXP_H = 280.f;
    float totH=HH;
    for(int m=0;m<cat.count;m++){
        col.ean[m]+=(( col.exp[m]?1.f:0.f)-col.ean[m])*sp;
        totH+=RH+EXP_H*col.ean[m];
    }
    ImVec2 p0=col.pos,p1={p0.x+W,p0.y+totH};

    dl->AddRectFilled({p0.x+3,p0.y+4},{p1.x+3,p1.y+4},C_SHD,6.f);
    dl->AddRectFilled(p0,p1,C_BG,6.f);
    dl->AddRect(p0,p1,C_BORD,6.f,0,1.f);

    ImVec2 h1={p1.x,p0.y+HH};
    dl->AddRectFilled(p0,h1,C_HDR,6.f);
    dl->AddRectFilled({p0.x,p0.y+HH-6},h1,C_HDR,0.f);
    dl->AddLine({p0.x,h1.y},h1,C_BRDH,1.f);
    char sb[8];snprintf(sb,sizeof(sb),"[%s]",cat.sym);
    float sw=ImGui::CalcTextSize(sb).x;
    dl->AddText({p0.x+10,p0.y+9},C_HINT,sb);
    dl->AddText({p0.x+10+sw+6,p0.y+9},C_TXT,cat.name);
    for(int d=0;d<3;d++)dl->AddCircleFilled({p1.x-12,p0.y+8+d*5.5f},1.8f,C_HINT);

    if(Hov(ms,p0,h1)&&lc&&!pDrag){col.dragging=true;pDrag=&col;col.dragOff={ms.x-p0.x,ms.y-p0.y};}

    if(g_drag.active&&g_drag.ci==ci){
        if(ImGui::IsMouseDown(0)){
            float rel=std::max(0.f,std::min(1.f,(ms.x-g_drag.x)/g_drag.w));
            float v=g_drag.lo+rel*(g_drag.hi-g_drag.lo);
            if(g_drag.tp==0)*(float*)g_drag.p=v;
            else if(g_drag.tp==1)*(int*)g_drag.p=(int)v;
            else if(g_drag.tp==3)*(double*)g_drag.p=(double)v;
        }else g_drag.active=false;
    }

    float ry=p0.y+HH;
    for(int m=0;m<cat.count;m++){
        ModDef&md=cat.mods[m];
        bool en=md.en?*md.en:true;
        col.tog[m]+=((en?1.f:0.f)-col.tog[m])*sp;
        float expH=EXP_H*col.ean[m];
        ImVec2 r0={p0.x,ry},r1={p1.x,ry+RH};
        bool rHov=Hov(ms,r0,r1);
        col.hov[m]+=((rHov?1.f:0.f)-col.hov[m])*sp;

        if(col.hov[m]>0.01f||col.ean[m]>0.01f)
            dl->AddRectFilled(r0,{r1.x,r1.y+expH},Lerp4(C_BG,C_ROWH,col.hov[m]),0.f);

        dl->AddLine({p0.x+8,ry},{p1.x-8,ry},C_DIV,1.f);
        dl->AddCircleFilled({p0.x+12,ry+RH*.5f},3.f,Lerp4(C_HINT,C_TON,col.tog[m]));
        dl->AddText({p0.x+22,ry+(RH-13)*.5f},Lerp4(C_DIM,C_TXT,col.tog[m]),md.name);

        const float TW=30.f,TH=14.f;
        float tx=p1.x-10-TW,ty=ry+(RH-TH)*.5f;
        ImVec2 t0={tx,ty},t1={tx+TW,ty+TH};
        dl->AddRectFilled(t0,t1,Lerp4(C_TOF,C_TON,col.tog[m]),TH*.5f);
        dl->AddCircleFilled({tx+TH*.5f+col.tog[m]*(TW-TH),ty+TH*.5f},TH*.38f,
            Lerp4(IM_COL32(110,110,110,255),IM_COL32(28,28,28,255),col.tog[m]));
        if(md.en&&Hov(ms,t0,t1)&&lc)*md.en=!*md.en;

        if(md.nc>0){
            float bsz=14.f,bx=tx-bsz-5.f,by=ry+(RH-bsz)*.5f;
            ImVec2 b0={bx,by},b1={bx+bsz,by+bsz};
            bool bh=Hov(ms,b0,b1);
            dl->AddRectFilled(b0,b1,col.exp[m]?IM_COL32(52,52,52,255):(bh?IM_COL32(42,42,42,255):IM_COL32(32,32,32,255)),3.f);
            dl->AddRect(b0,b1,C_BORD,3.f,0,1.f);
            float cx=bx+bsz*.5f,cy=by+bsz*.5f;
            ImU32 pc=(bh||col.exp[m])?C_TXT:C_DIM;
            dl->AddLine({cx-3,cy},{cx+3,cy},pc,1.5f);
            if(!col.exp[m])dl->AddLine({cx,cy-3},{cx,cy+3},pc,1.5f);
            if(bh&&lc)col.exp[m]=!col.exp[m];
        }

        if(col.ean[m]>0.01f){
            float sH=col.ean[m]*EXP_H;
            float sY=ry+RH;
            char cid[24]; snprintf(cid,24,"##v4s%d_%d",ci,m);
            ImGui::SetCursorScreenPos({p0.x, sY});
            V4PushTheme();
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.f);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, col.ean[m]);
            if(ImGui::BeginChild(cid, {W, sH}, false,
                ImGuiWindowFlags_AlwaysVerticalScrollbar)){
                ImGui::SetCursorPos({6.f, 4.f});
                RenderModuleSettings(V4_RMS[ci], m);
            }
            ImGui::EndChild();
            ImGui::PopStyleVar(2); V4PopTheme();
        }
        ry+=RH+expH;
    }
}

static void RenderSidebar(ImDrawList*dl,ImVec2 ms,bool lc,ImVec2 disp)
{
    const float SW=138.f,PAD=14.f,LH=44.f,IH=30.f;
    const float totH=LH+CAT_COUNT*IH;
    const float sy0=(disp.y-totH)*.5f;
    ImVec2 p0={PAD,sy0},p1={PAD+SW,sy0+totH};

    dl->AddRectFilled({p0.x+3,p0.y+4},{p1.x+3,p1.y+4},C_SHD,7.f);
    dl->AddRectFilled(p0,p1,C_BG,7.f);
    dl->AddRect(p0,p1,C_BORD,7.f,0,1.f);

    ImVec2 lEnd={p1.x,p0.y+LH};
    dl->AddRectFilled(p0,lEnd,C_HDR,7.f);
    dl->AddRectFilled({p0.x,p0.y+LH-7},lEnd,C_HDR,0.f);
    dl->AddLine({p0.x,lEnd.y},lEnd,C_BRDH,1.f);
    dl->AddText({p0.x+12,p0.y+9},C_TXT,"VENGEANCE");
    dl->AddText({p0.x+12,p0.y+25},C_HINT,"Category View");

    float cy=p0.y+LH;
    for(int i=0;i<CAT_COUNT;i++){
        ImVec2 i0={p0.x,cy},i1={p1.x,cy+IH};
        bool hov=Hov(ms,i0,i1),vis=cols[i].visible;
        if(vis)dl->AddRectFilled(i0,i1,IM_COL32(36,36,36,255),2.f);
        else if(hov)dl->AddRectFilled(i0,i1,IM_COL32(28,28,28,255),2.f);
        char sb2[4];snprintf(sb2,sizeof(sb2),"%s",cats[i].sym);
        float sw2=ImGui::CalcTextSize(sb2).x;
        dl->AddText({i0.x+10,i0.y+(IH-13)*.5f},C_HINT,sb2);
        dl->AddText({i0.x+10+sw2+5,i0.y+(IH-13)*.5f},vis?C_TXT:C_DIM,cats[i].name);
        char cb2[6];snprintf(cb2,sizeof(cb2),"%d",cats[i].count);
        ImVec2 csz=ImGui::CalcTextSize(cb2);
        dl->AddText({p1.x-8-csz.x,i0.y+(IH-13)*.5f},C_HINT,cb2);
        dl->AddLine({i0.x+6,i1.y},{i1.x-6,i1.y},C_DIV,1.f);
        if(hov&&lc)cols[i].visible=!cols[i].visible;
        cy+=IH;
    }
}

inline void RenderUI(bool*[],const char*[],int)
{
    ImDrawList*dl=ImGui::GetWindowDrawList();
    ImVec2 disp=ImGui::GetIO().DisplaySize;
    ImVec2 ms=ImGui::GetIO().MousePos;
    bool   lc=ImGui::IsMouseClicked(0);
    if(!ImGui::IsMouseDown(0))g_drag.active=false;
    Init();
    RenderSidebar(dl,ms,lc,disp);
    for(int i=0;i<CAT_COUNT;i++)RenderColumn(dl,ms,lc,i);
}
}

