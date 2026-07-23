#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include <cmath>
#include <cstring>
#include <cstdio>
#include <algorithm>

void RenderModuleSettings(int cat, int mod);

extern bool sprintEnabled;
extern bool velocityEnabled;
extern bool arraylistEnabled;
extern bool blinkEnabled, timerEnabled, fakeLagEnabled;
extern bool teamsEnabled, friendsEnabled, autoSwordEnabled, antiVoidEnabled;
extern bool middleClickFriendEnabled, antiBotEnabled, autoArmorEnabled, inventoryManagerEnabled;
extern bool breadCrumbsEnabled, antiAfkEnabled, antiBadEffectsEnabled, panicEnabled;
extern bool pingSpoofModEnabled, reconnectEnabled, staffNotifierEnabled;
extern bool noRotateEnabled, antiCactusEnabled;

extern bool aimbotEnabled, clickerEnabled, blockhitEnabled, reachEnabled;
extern bool hitSelectEnabled, sTapEnabled, wTapEnabled, bowAimbotEnabled;
extern bool noFallEnabled, invWalkEnabled, jumpResetEnabled, noJumpDelayEnabled;
extern bool scaffoldEnabled, stepEnabled, spiderEnabled, safeWalkEnabled;
extern bool reverseStepEnabled, bouncySlimeEnabled, fastStopEnabled, glideEnabled;
extern bool flightEnabled, airJumpEnabled, highJumpEnabled, sprintResetEnabled;
extern bool espEnabled, tracerEnabled, xrayEnabled, storageEspEnabled, chamsEnabled;
extern bool targetHudEnabled, freecamEnabled, fullBrightEnabled, noHurtCamEnabled;
extern bool zoomEnabled, armorEspEnabled, itemEspEnabled, holeEspEnabled;
extern bool rightClickerEnabled, fastPlaceEnabled, throwpotEnabled, autosoupEnabled;
extern bool refillEnabled, autoToolEnabled, bridgeAssistEnabled;
extern bool uiPresetsEnabled, g_useCustomUI, showMenu;
extern int uiPresetsSelected;

namespace UISlinky {

static constexpr ImU32 C_BG      = IM_COL32( 10, 10, 10,248);
static constexpr ImU32 C_PANEL   = IM_COL32( 18, 18, 18,255);
static constexpr ImU32 C_PANELBR = IM_COL32( 38, 38, 38,255);
static constexpr ImU32 C_CATSEL  = IM_COL32( 36, 36, 36,255);
static constexpr ImU32 C_CATHOV  = IM_COL32( 26, 26, 26,255);
static constexpr ImU32 C_CARDBG  = IM_COL32( 20, 20, 20,255);
static constexpr ImU32 C_CARDBR  = IM_COL32( 48, 48, 48,255);
static constexpr ImU32 C_DIV     = IM_COL32( 38, 38, 38,255);
static constexpr ImU32 C_TXT     = IM_COL32(230,230,230,255);
static constexpr ImU32 C_DIM     = IM_COL32(105,105,105,255);
static constexpr ImU32 C_WHITE   = IM_COL32(255,255,255,255);
static constexpr ImU32 C_TOGOFF  = IM_COL32( 45, 45, 45,255);
static constexpr ImU32 C_TOGON   = IM_COL32(210,210,210,255);
static constexpr ImU32 C_BORDER  = IM_COL32( 38, 38, 38,255);

static constexpr float WIN_W  = 680.f;
static constexpr float WIN_H  = 500.f;
static constexpr float CAT_W  = 110.f;
static constexpr float IP     =   4.f;
static constexpr float CG     =   4.f;
static constexpr float COL_W  = 220.f;
static constexpr float ROW_H  =  52.f;
static constexpr float EXP_MAX= 260.f;
static constexpr float CARD_R =   6.f;
static constexpr float TOG_W  =  30.f, TOG_H = 16.f;

static constexpr float LP_X0 = IP;
static constexpr float LP_X1 = IP + CAT_W;
static constexpr float LC_X0 = LP_X1 + CG;
static constexpr float RC_X0 = LC_X0 + COL_W + CG;
static constexpr float RP_X0 = RC_X0 + COL_W + CG;
static constexpr float RP_X1 = RP_X0 + CAT_W;

static const char* CATS[] = {
    "Combat","Visuals","Movement","Network",
    "Utilities","Misc","Settings","Appearance"
};
static constexpr int CAT_RMS[] = {1,3,2,4,5,6,7,8};

struct SkMod { const char* name; const char* desc; bool* en; };

static const SkMod M_COMBAT[] = {
    {"Aim Assist",    "Locks onto nearby players",     &aimbotEnabled},
    {"Auto Clicker",  "Automates left mouse clicks",   &clickerEnabled},
    {"Blockhit",      "Blocks between attacks",        &blockhitEnabled},
    {"Reach",         "Extends attack range",          &reachEnabled},
    {"Hit Select",    "Burst or critical hits",        &hitSelectEnabled},
    {"S-Tap",         "Sprint-tap knockback",          &sTapEnabled},
    {"W-Tap",         "W-tap knockback boost",         &wTapEnabled},
    {"Bow Aimbot",    "Leads targets for bow shots",   &bowAimbotEnabled},
};
static const SkMod M_VISUALS[] = {
    {"ESP",           "Player boxes through walls",    &espEnabled},
    {"Tracers",       "Lines to players",              &tracerEnabled},
    {"XRay",          "See through blocks",            &xrayEnabled},
    {"Storage ESP",   "Highlights chests/storages",    &storageEspEnabled},
    {"ArrayList",     "Enabled module list overlay",   &arraylistEnabled},
    {"Chams",         "Players visible through walls", &chamsEnabled},
    {"Target HUD",    "Combat target display",         &targetHudEnabled},
    {"Freecam",       "Detach and fly camera",         &freecamEnabled},
    {"FullBright",    "Max brightness at all times",   &fullBrightEnabled},
    {"No Hurt Cam",   "Removes damage screen shake",   &noHurtCamEnabled},
    {"Zoom",          "Hold key to zoom in",           &zoomEnabled},
    {"Armor ESP",     "Show armor on players",         &armorEspEnabled},
    {"Item ESP",      "Highlights dropped items",      &itemEspEnabled},
    {"Hole ESP",      "Shows bedrock safety holes",    &holeEspEnabled},
};
static const SkMod M_MOVEMENT[] = {
    {"Velocity",      "Reduces knockback taken",       &velocityEnabled},
    {"Sprint",        "Always sprint forward",         &sprintEnabled},
    {"Sprint Reset",  "Resets sprint on attack",       &sprintResetEnabled},
    {"No Fall",       "Negates fall damage",           &noFallEnabled},
    {"Inv Walk",      "Move while inventory open",     &invWalkEnabled},
    {"Jump Reset",    "Jumps on knockback",            &jumpResetEnabled},
    {"No Jump Delay", "Remove jump cooldown",          &noJumpDelayEnabled},
    {"Scaffold",      "Places blocks under feet",      &scaffoldEnabled},
    {"Step",          "Step up blocks instantly",      &stepEnabled},
    {"Spider",        "Climb walls",                   &spiderEnabled},
    {"Safe Walk",     "Prevents walking off edges",    &safeWalkEnabled},
    {"Reverse Step",  "Step down blocks instantly",    &reverseStepEnabled},
    {"Bouncy Slime",  "Higher slime block bounce",     &bouncySlimeEnabled},
    {"Fast Stop",     "Instant stop on key release",   &fastStopEnabled},
    {"Glide",         "Slow falling speed",            &glideEnabled},
    {"Flight",        "Fly in survival mode",          &flightEnabled},
    {"Air Jump",      "Jump while in the air",         &airJumpEnabled},
    {"High Jump",     "Jump higher than normal",       &highJumpEnabled},
};
static const SkMod M_NETWORK[] = {
    {"Blink",         "Hold and release packets",      &blinkEnabled},
    {"Timer",         "Change game tick speed",        &timerEnabled},
    {"Fake Lag",      "Delay outgoing packets",        &fakeLagEnabled},
};
static const SkMod M_UTIL[] = {
    {"Right Clicker", "Automates right mouse clicks",  &rightClickerEnabled},
    {"Fast Place",    "Reduce block place delay",      &fastPlaceEnabled},
    {"Throwpot",      "Auto throw splash potions",     &throwpotEnabled},
    {"Auto Soup",     "Auto eat mushroom soup",        &autosoupEnabled},
    {"Refill",        "Refill hotbar from inventory",  &refillEnabled},
    {"Auto Tool",     "Best tool auto-select",         &autoToolEnabled},
    {"Bridge Assist", "Sneak at block edges",          &bridgeAssistEnabled},
    {"Teams",         "Detect and skip teammates",     &teamsEnabled},
    {"Friends",       "Personal friend system",        &friendsEnabled},
    {"Auto Sword",    "Auto-select best sword",        &autoSwordEnabled},
    {"Anti Void",     "Prevent falling into void",     &antiVoidEnabled},
    {"MC Friend",     "Middle click to add friend",    &middleClickFriendEnabled},
    {"Anti Bot",      "Filter fake bot players",       &antiBotEnabled},
    {"Auto Armor",    "Equip best armor from inv",     &autoArmorEnabled},
    {"Inv Manager",   "Sort and clean inventory",      &inventoryManagerEnabled},
};
static const SkMod M_MISC[] = {
    {"BreadCrumbs",   "Leave a colored trail",         &breadCrumbsEnabled},
    {"Anti AFK",      "Prevent AFK disconnect",        &antiAfkEnabled},
    {"Anti Bad FX",   "Remove negative effects",       &antiBadEffectsEnabled},
    {"Panic",         "Disable all modules",           &panicEnabled},
    {"Ping Spoof",    "Fake your ping amount",         &pingSpoofModEnabled},
    {"Reconnect",     "Auto reconnect on disconnect",  &reconnectEnabled},
    {"Staff Notifier","Alert when staff joins",        &staffNotifierEnabled},
    {"No Rotate",     "Block forced rotations",        &noRotateEnabled},
    {"Anti Cactus",   "No cactus block damage",        &antiCactusEnabled},
};

static bool s_apEn[3] = {};
static const SkMod M_APPEARANCE[] = {
    {"Colors",   "Customize theme colors", &s_apEn[0]},
    {"Presets",  "Apply color presets",    &s_apEn[1]},
    {"UI Presets","Switch UI style",       &uiPresetsEnabled},
};
static constexpr int M_CNT[] = {8,14,18,3,15,9,0,3};
static const SkMod* M_ALL[] = {M_COMBAT,M_VISUALS,M_MOVEMENT,M_NETWORK,M_UTIL,M_MISC,nullptr,M_APPEARANCE};

static int   s_cat     = 0;
static int   s_prevCat = -1;
static bool  s_expOpen[2][32] = {};
static float s_expAnim[2][32] = {};
static float s_hovAnim[2][32] = {};
static float s_scroll [2]     = {};
static float s_scrollV[2]     = {};
static float s_togAnim[64]    = {};
static float s_catHov [8]     = {};

static inline ImU32 LC(ImU32 a, ImU32 b, float t) {
    t=t<0.f?0.f:t>1.f?1.f:t;
    int ar=(a>>0)&255,ag=(a>>8)&255,ab=(a>>16)&255,aa=(a>>24)&255;
    int br=(b>>0)&255,bg=(b>>8)&255,bb=(b>>16)&255,ba=(b>>24)&255;
    return IM_COL32(ar+(int)((br-ar)*t),ag+(int)((bg-ag)*t),
                   ab+(int)((bb-ab)*t),aa+(int)((ba-aa)*t));
}
static inline bool IH(ImVec2 a, ImVec2 b, ImVec2 p) {
    return p.x>=a.x&&p.x<=b.x&&p.y>=a.y&&p.y<=b.y;
}
static void DrawTog(ImDrawList* dl, float cx, float cy, float t) {
    float hw=TOG_W*.5f, hh=TOG_H*.5f;
    dl->AddRectFilled({cx-hw,cy-hh},{cx+hw,cy+hh}, LC(C_TOGOFF,C_TOGON,t), hh);
    dl->AddCircleFilled({cx-hw+hh+t*(TOG_W-TOG_H*2.f),cy}, hh-2.f, C_WHITE, 16);
}
static void PushSettTheme() {
    ImGui::PushStyleColor(ImGuiCol_ChildBg,        ImVec4(0.055f,0.055f,0.055f,1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,         ImVec4(0.10f, 0.10f, 0.10f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  ImVec4(0.16f, 0.16f, 0.16f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,   ImVec4(0.22f, 0.22f, 0.22f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,       ImVec4(0.90f, 0.90f, 0.90f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,      ImVec4(0.80f, 0.80f, 0.80f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Button,          ImVec4(0.14f, 0.14f, 0.14f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,   ImVec4(0.22f, 0.22f, 0.22f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,    ImVec4(0.30f, 0.30f, 0.30f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg,     ImVec4(0.04f, 0.04f, 0.04f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab,   ImVec4(0.22f, 0.22f, 0.22f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Text,            ImVec4(0.88f, 0.88f, 0.88f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_Separator,       ImVec4(0.22f, 0.22f, 0.22f, 1.f));
}
static void PopSettTheme() { ImGui::PopStyleColor(13); }

static void RenderCol(ImDrawList* dl, ImVec2 wp,
                      int col, float colOffX,
                      const SkMod* mods, int cnt, int rmsId,
                      float dt, ImVec2 ms, bool lc)
{
    float sp   = 14.f * dt;
    float cX0  = wp.x + colOffX;
    float cY0  = wp.y;
    float cH   = WIN_H;

    int itemCnt = 0;
    for (int i = col; i < cnt; i += 2) itemCnt++;

    float totalH = 0.f;
    for (int k = 0; k < itemCnt; k++) {
        int mi = col + k * 2;
        bool on = mods[mi].en ? *mods[mi].en : false;
        s_togAnim[mi]        += ((on ? 1.f : 0.f) - s_togAnim[mi]) * sp;
        s_expAnim[col][k]    += ((s_expOpen[col][k] ? 1.f : 0.f) - s_expAnim[col][k]) * sp;
        s_hovAnim[col][k]    += ((0.f) - s_hovAnim[col][k]) * sp;
        totalH += ROW_H + s_expAnim[col][k] * EXP_MAX + 3.f;
    }

    bool colHov = ms.x >= cX0 && ms.x < cX0 + COL_W && ms.y >= cY0 && ms.y < cY0 + cH;
    if (colHov) {
        float w = ImGui::GetIO().MouseWheel;
        if (fabsf(w) > 0.01f) s_scrollV[col] -= w * 28.f;
    }
    s_scroll[col]  += s_scrollV[col];
    s_scrollV[col] *= 0.80f;
    float maxSc = totalH - cH;
    if (s_scroll[col] < 0.f) s_scroll[col] = 0.f;
    if (maxSc > 0.f && s_scroll[col] > maxSc) s_scroll[col] = maxSc;
    else if (maxSc <= 0.f) s_scroll[col] = 0.f;

    dl->PushClipRect({cX0, cY0}, {cX0 + COL_W, cY0 + cH}, true);

    float ry = cY0 - s_scroll[col];
    for (int k = 0; k < itemCnt; k++) {
        int mi = col + k * 2;
        const SkMod& mod = mods[mi];
        float expH     = s_expAnim[col][k] * EXP_MAX;
        float rowTotH  = ROW_H + expH;
        float rx0 = cX0, rx1 = cX0 + COL_W;
        float ry0 = ry,  ry1 = ry0 + ROW_H;
        bool inView = (ry0 < cY0 + cH) && (ry0 + rowTotH > cY0);

        if (inView) {

            bool hdr = IH({rx0, ry0}, {rx1, ry1}, ms);
            s_hovAnim[col][k] = hdr ? 1.f : 0.f;

            if (s_expAnim[col][k] > 0.001f) {
                float a = s_expAnim[col][k];
                dl->AddRectFilled({rx0, ry0}, {rx1, ry0 + rowTotH},
                    IM_COL32(20, 20, 20, (int)(a * 235)), CARD_R);
                dl->AddRect({rx0, ry0}, {rx1, ry0 + rowTotH},
                    IM_COL32(50, 50, 50, (int)(a * 200)), CARD_R, 0, 1.f);
            } else if (hdr) {

                dl->AddRectFilled({rx0, ry0}, {rx1, ry1},
                    IM_COL32(255, 255, 255, 12), CARD_R);
            }

            dl->AddText(ImGui::GetFont(), 13.f,
                {rx0 + 10.f, ry0 + 11.f}, C_TXT, mod.name);
            dl->AddText(ImGui::GetFont(), 11.f,
                {rx0 + 10.f, ry0 + 29.f}, C_DIM, mod.desc);

            float tCX = rx1 - TOG_W * .5f - 10.f;
            float tCY = ry0 + ROW_H * .5f;
            DrawTog(dl, tCX, tCY, s_togAnim[mi]);

            float chX = rx1 - TOG_W - 24.f;
            float chY = ry0 + ROW_H * .5f;
            bool chH  = IH({chX - 7.f, chY - 9.f}, {chX + 7.f, chY + 9.f}, ms);
            ImU32 chC = chH ? C_TXT : C_DIM;
            if (s_expOpen[col][k]) {
                dl->AddLine({chX - 4.f, chY - 2.f}, {chX,       chY + 3.f}, chC, 1.5f);
                dl->AddLine({chX,       chY + 3.f}, {chX + 4.f, chY - 2.f}, chC, 1.5f);
            } else {
                dl->AddLine({chX - 2.f, chY - 4.f}, {chX + 3.f, chY      }, chC, 1.5f);
                dl->AddLine({chX + 3.f, chY      }, {chX - 2.f, chY + 4.f}, chC, 1.5f);
            }

            if (s_expAnim[col][k] > 0.01f)
                dl->AddLine({rx0 + 8.f, ry1 - 0.5f}, {rx1 - 8.f, ry1 - 0.5f},
                    IM_COL32(55, 55, 55, (int)(s_expAnim[col][k] * 255)), 1.f);

            if (lc && ry0 >= cY0 - 1.f && ry0 < cY0 + cH) {

                if (IH({tCX-TOG_W*.5f-3.f,tCY-TOG_H*.5f-3.f},
                        {tCX+TOG_W*.5f+3.f,tCY+TOG_H*.5f+3.f}, ms)) {
                    if (mod.en) *mod.en = !*mod.en;
                }

                else if (IH({chX-8.f,chY-10.f},{chX+8.f,chY+10.f}, ms))
                    s_expOpen[col][k] = !s_expOpen[col][k];

                else if (hdr)
                    s_expOpen[col][k] = !s_expOpen[col][k];
            }
        }

        if (s_expAnim[col][k] > 0.01f) {
            float sH = s_expAnim[col][k] * EXP_MAX;
            float sY = ry0 + ROW_H;
            if (sH > 4.f && sY < cY0 + cH && sY + sH > cY0) {
                dl->PopClipRect();
                ImGui::SetCursorScreenPos({rx0, sY});
                char cid[28]; snprintf(cid, 28, "##sk%d%d%d", col, k, s_cat);
                PushSettTheme();
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, CARD_R);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, s_expAnim[col][k]);
                if (ImGui::BeginChild(cid, {COL_W, sH}, false,
                    ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
                    ImGui::SetCursorPos({8.f, 6.f});
                    RenderModuleSettings(rmsId, mi);
                }
                ImGui::EndChild();
                ImGui::PopStyleVar(2);
                PopSettTheme();
                dl->PushClipRect({cX0, cY0}, {cX0 + COL_W, cY0 + cH}, true);
            }
        }

        ry += rowTotH + 3.f;
    }

    if (maxSc > 0.f && totalH > cH) {
        float bH = cH * (cH / totalH);
        float bY = cY0 + (s_scroll[col] / maxSc) * (cH - bH);
        dl->AddRectFilled({cX0 + COL_W - 4.f, bY},
                          {cX0 + COL_W - 1.f, bY + bH},
                          IM_COL32(60, 60, 60, 160), 2.f);
    }

    dl->PopClipRect();
}

static void RenderCatPanel(ImDrawList* dl, ImVec2 wp, float panelOffX,
                           int firstCat, ImVec2 ms, bool lc, float sp)
{
    float px0 = wp.x + panelOffX;
    float py0 = wp.y + 8.f;
    float px1 = px0 + CAT_W;
    float py1 = wp.y + WIN_H - 8.f;
    dl->AddRectFilled({px0, py0}, {px1, py1}, C_PANEL, 8.f);
    dl->AddRect({px0, py0}, {px1, py1}, C_PANELBR, 8.f, 0, 1.f);

    float cy   = py0 + 14.f;
    float BH   = 32.f, BW = CAT_W - 16.f, BR = 5.f;
    for (int i = 0; i < 4; i++) {
        int ci  = firstCat + i;
        float bx0 = px0 + 8.f, bx1 = bx0 + BW;
        float by0 = cy, by1 = cy + BH;
        bool hov = IH({bx0, by0}, {bx1, by1}, ms);
        s_catHov[ci] += ((hov ? 1.f : 0.f) - s_catHov[ci]) * sp;
        bool sel  = (s_cat == ci);
        ImU32 bg  = sel ? C_CATSEL : LC(C_PANEL, C_CATHOV, s_catHov[ci]);
        ImU32 tc  = sel ? C_WHITE  : LC(C_DIM,   C_TXT,    s_catHov[ci]);
        if (sel || s_catHov[ci] > 0.01f)
            dl->AddRectFilled({bx0, by0}, {bx1, by1}, bg, BR);
        if (sel)
            dl->AddRect({bx0, by0}, {bx1, by1}, C_PANELBR, BR, 0, 1.f);
        ImVec2 tsz = ImGui::CalcTextSize(CATS[ci]);
        dl->AddText({bx0 + (BW - tsz.x) * .5f, by0 + (BH - tsz.y) * .5f}, tc, CATS[ci]);
        if (hov && lc && s_cat != ci) s_cat = ci;
        cy += BH + 6.f;
    }
}

inline void RenderUI(bool*[], const char*[], int) {
    ImGuiIO& io   = ImGui::GetIO();
    float dt      = io.DeltaTime > 0.f ? io.DeltaTime : 0.016f;
    float sp      = 14.f * dt;
    ImVec2 ms     = io.MousePos;
    bool   lc     = ImGui::IsMouseClicked(0);

    float wx = (io.DisplaySize.x - WIN_W) * .5f;
    float wy = (io.DisplaySize.y - WIN_H) * .5f;
    ImGui::SetNextWindowPos({wx, wy});
    ImGui::SetNextWindowSize({WIN_W, WIN_H});
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   10.f);
    bool open = true;
    ImGui::Begin("##slinky", &open,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::PopStyleVar(3);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 wp      = ImGui::GetWindowPos();

    dl->AddRectFilled(wp, {wp.x + WIN_W, wp.y + WIN_H}, C_BG, 10.f);
    dl->AddRect(wp, {wp.x + WIN_W, wp.y + WIN_H}, C_BORDER, 10.f, 0, 1.f);

    if (s_cat != s_prevCat) {
        memset(s_expOpen, 0, sizeof(s_expOpen));
        memset(s_expAnim, 0, sizeof(s_expAnim));
        memset(s_scroll,  0, sizeof(s_scroll));
        memset(s_scrollV, 0, sizeof(s_scrollV));
        memset(s_hovAnim, 0, sizeof(s_hovAnim));
        s_prevCat = s_cat;
    }

    RenderCatPanel(dl, wp, LP_X0, 0, ms, lc, sp);
    RenderCatPanel(dl, wp, RP_X0, 4, ms, lc, sp);

    {
        const char* bk = "Default UI";
        ImVec2 bsz = ImGui::CalcTextSize(bk);
        float bw = bsz.x + 12.f, bh = 18.f;
        ImVec2 b0 = {wp.x + RP_X0 + CAT_W - bw - 6.f, wp.y + 10.f};
        ImVec2 b1 = {b0.x + bw, b0.y + bh};
        bool hb = IH(b0, b1, ms);
        dl->AddRectFilled(b0, b1, hb ? C_CATSEL : C_PANEL, 4.f);
        dl->AddRect(b0, b1, C_PANELBR, 4.f, 0, 1.f);
        dl->AddText({b0.x + 6.f, b0.y + (bh - bsz.y) * .5f}, hb ? C_TXT : C_DIM, bk);
        if (hb && lc) { g_useCustomUI = false; uiPresetsSelected = 0; showMenu = true; }
    }

    if (s_cat == 6) {
        float cx0 = wp.x + LC_X0;
        ImGui::SetCursorScreenPos({cx0, wp.y + 8.f});
        PushSettTheme();
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.f);
        if (ImGui::BeginChild("##slksett", {COL_W * 2.f + CG, WIN_H - 16.f}, false)) {
            ImGui::SetCursorPos({8.f, 8.f});
            RenderModuleSettings(7, 0);
        }
        ImGui::EndChild();
        ImGui::PopStyleVar(); PopSettTheme();
        ImGui::End(); return;
    }

    int cnt     = M_CNT[s_cat];
    const SkMod* mods = M_ALL[s_cat];
    int rmsId   = CAT_RMS[s_cat];
    if (cnt > 0 && mods) {
        RenderCol(dl, wp, 0, LC_X0, mods, cnt, rmsId, dt, ms, lc);
        RenderCol(dl, wp, 1, RC_X0, mods, cnt, rmsId, dt, ms, lc);
    }

    ImGui::End();
}

}
