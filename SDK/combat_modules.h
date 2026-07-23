

#pragma once

#include <windows.h>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <deque>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct STapConfig {
    float chance = 100.0f;
    int delayMin = 30;
    int delayMax = 80;
    bool onlyWhileSprinting = true;
    bool requireSword = true;
    bool antiBotCheck = true;
    bool adaptiveDelay = true;
};

class STapModule {
private:
    std::mt19937 m_rng{std::random_device{}()};

    bool m_active = false;
    int m_tick = 0;
    int m_targetReleaseTicks = 1;
    ULONGLONG m_releaseTime = 0;
    bool m_sprintReleased = false;
    float m_lastTargetDist = 0.0f;

public:
    STapConfig config;

    void on_attack(float targetDist) {

        std::uniform_real_distribution<float> chanceDist(0.0f, 100.0f);
        if (chanceDist(m_rng) > config.chance) return;

        m_lastTargetDist = targetDist;
        m_active = true;
        m_tick = 0;
        m_sprintReleased = false;

        int delay;
        if (config.adaptiveDelay && targetDist > 0.0f) {
            float proportion = (std::min)(targetDist / 4.0f, 1.0f);
            delay = config.delayMin + (int)((config.delayMax - config.delayMin) * proportion);
        } else {
            std::uniform_int_distribution<int> delayDist(config.delayMin, config.delayMax);
            delay = delayDist(m_rng);
        }

        m_targetReleaseTicks = (std::max)(1, delay / 50);
    }

    int on_tick() {
        if (!m_active) return 0;

        m_tick++;

        if (m_tick == 1 && !m_sprintReleased) {
            m_sprintReleased = true;
            return 1;
        }

        if (m_tick > m_targetReleaseTicks && m_sprintReleased) {
            m_active = false;
            m_sprintReleased = false;
            m_tick = 0;
            return 2;
        }

        return 0;
    }

    void reset() {
        m_active = false;
        m_tick = 0;
        m_sprintReleased = false;
    }

    bool is_active() const { return m_active; }
};

struct WTapConfig {
    float chance = 100.0f;
    int durationMin = 1;
    int durationMax = 2;
    bool legitMode = true;
    bool onlyWhileSprinting = true;
    bool onlyForward = true;
    bool requireSword = true;
    bool antiBotCheck = true;
    bool adaptiveChance = false;
    int comboThreshold = 3;
};

class WTapModule {
private:
    std::mt19937 m_rng{std::random_device{}()};

    bool m_active = false;
    int m_tick = 0;
    int m_targetReleaseTicks = 1;
    bool m_keyReleased = false;

    int m_comboCount = 0;
    int m_lastTargetId = -1;

public:
    WTapConfig config;

    void on_attack(int targetId) {

        if (targetId != m_lastTargetId) {
            m_comboCount = 0;
            m_lastTargetId = targetId;
        }
        m_comboCount++;

        float effectiveChance = config.chance;
        if (config.adaptiveChance && m_comboCount <= config.comboThreshold) {
            effectiveChance *= 0.5f + (0.5f * ((float)m_comboCount / (float)config.comboThreshold));
        }

        std::uniform_real_distribution<float> chanceDist(0.0f, 100.0f);
        if (chanceDist(m_rng) > effectiveChance) return;

        m_active = true;
        m_tick = 0;
        m_keyReleased = false;

        std::uniform_int_distribution<int> durDist(config.durationMin, (std::max)(config.durationMin, config.durationMax));
        m_targetReleaseTicks = durDist(m_rng);
    }

    int on_tick() {
        if (!m_active) return 0;

        m_tick++;

        if (m_tick == 1 && !m_keyReleased) {
            m_keyReleased = true;
            return 1;
        }

        if (m_tick > m_targetReleaseTicks && m_keyReleased) {
            m_active = false;
            m_keyReleased = false;
            m_tick = 0;
            return 2;
        }

        return 0;
    }

    void reset() {
        m_active = false;
        m_tick = 0;
        m_keyReleased = false;
        m_comboCount = 0;
        m_lastTargetId = -1;
    }

    bool is_active() const { return m_active; }
};

struct BowAimbotConfig {
    float fov = 180.0f;
    float range = 64.0f;
    float predictionStrength = 1.0f;
    float smoothing = 5.0f;
    bool onlyPlayers = true;
    bool ignoreFriends = true;
    bool requireBow = true;
    bool autoShoot = false;
    float chargeThreshold = 0.9f;
    int filterMode = 0;
};

struct BowPrediction {
    double targetX, targetY, targetZ;
    float yaw, pitch;
    double distance;
    bool valid = false;
};

class BowAimbotModule {
private:
    std::mt19937 m_rng{std::random_device{}()};
    float m_currentYaw = 0.0f;
    float m_currentPitch = 0.0f;
    bool m_hasAimTarget = false;

public:
    BowAimbotConfig config;

    BowPrediction calculate(
        double playerX, double playerY, double playerZ, float playerEyeHeight,
        double targetX, double targetY, double targetZ, float targetEyeHeight,
        double targetMotionX, double targetMotionZ,
        int chargeTickCount) const
    {
        BowPrediction result;

        float power = (float)chargeTickCount / 20.0f;
        power = (power * power + power * 2.0f) / 3.0f;
        if (power > 1.0f) power = 1.0f;
        if (power < 0.1f) power = 0.1f;

        float arrowVelocity = power * 3.0f;
        const float gravity = 0.05f;

        double eyeX = playerX;
        double eyeY = playerY + (double)playerEyeHeight;
        double eyeZ = playerZ;

        float predMul = config.predictionStrength * arrowVelocity * 2.0f;
        double predX = targetX + targetMotionX * predMul;
        double predY = targetY + (double)targetEyeHeight * 0.8;
        double predZ = targetZ + targetMotionZ * predMul;

        double dx = predX - eyeX;
        double dy = predY - eyeY;
        double dz = predZ - eyeZ;
        double distXZ = std::sqrt(dx * dx + dz * dz);

        result.distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        result.targetX = predX;
        result.targetY = predY;
        result.targetZ = predZ;

        result.yaw = (float)(std::atan2(dz, dx) * 180.0 / M_PI) - 90.0f;

        if (distXZ > 0.001 && arrowVelocity > 0.01f) {
            double timeToTarget = distXZ / (double)arrowVelocity;
            double requiredVy = (dy + 0.5 * gravity * timeToTarget * timeToTarget) / timeToTarget;
            result.pitch = (float)(-(std::atan2(requiredVy, distXZ) * 180.0 / M_PI));
            result.pitch = (std::max)(-90.0f, (std::min)(90.0f, result.pitch));
            result.valid = true;
        } else {
            result.pitch = 0.0f;
            result.valid = false;
        }

        return result;
    }

    void smooth_aim(float playerYaw, float playerPitch, float targetYaw, float targetPitch,
                    float sensitivity, float& outYaw, float& outPitch) const
    {

        float f = sensitivity * 0.6f + 0.2f;
        float gcd = f * f * f * 8.0f;

        float deltaYaw = targetYaw - playerYaw;

        while (deltaYaw > 180.0f) deltaYaw -= 360.0f;
        while (deltaYaw < -180.0f) deltaYaw += 360.0f;
        float deltaPitch = targetPitch - playerPitch;

        float factor = 1.0f / (std::max)(1.0f, config.smoothing);

        float yawStep = deltaYaw * factor;
        float pitchStep = deltaPitch * factor;

        if (std::fabs(gcd) > 0.0001f) {
            yawStep = yawStep - std::fmod(yawStep, gcd);
            pitchStep = pitchStep - std::fmod(pitchStep, gcd);
        }

        outYaw = playerYaw + yawStep;
        outPitch = (std::max)(-90.0f, (std::min)(90.0f, playerPitch + pitchStep));
    }

    bool in_fov(float playerYaw, double playerX, double playerZ,
                double targetX, double targetZ) const
    {
        double dx = targetX - playerX;
        double dz = targetZ - playerZ;
        float targetYaw = (float)(std::atan2(dz, dx) * 180.0 / M_PI) - 90.0f;
        float diff = targetYaw - playerYaw;
        while (diff > 180.0f) diff -= 360.0f;
        while (diff < -180.0f) diff += 360.0f;
        return std::fabs(diff) <= config.fov / 2.0f;
    }

    void reset() {
        m_hasAimTarget = false;
    }
};

static STapModule g_sTapModule;
static WTapModule g_wTapModule;
static BowAimbotModule g_bowAimbotModule;

inline void STap_OnAttack(float dist) { g_sTapModule.on_attack(dist); }
inline int  STap_OnTick()             { return g_sTapModule.on_tick(); }
inline void STap_Reset()              { g_sTapModule.reset(); }

inline void WTap_OnAttack(int targetId) { g_wTapModule.on_attack(targetId); }
inline int  WTap_OnTick()               { return g_wTapModule.on_tick(); }
inline void WTap_Reset()                { g_wTapModule.reset(); }

inline BowPrediction BowAimbot_Calculate(
    double px, double py, double pz, float eyeH,
    double tx, double ty, double tz, float tEyeH,
    double tmx, double tmz, int chargeTicks)
{
    return g_bowAimbotModule.calculate(px, py, pz, eyeH, tx, ty, tz, tEyeH, tmx, tmz, chargeTicks);
}
inline void BowAimbot_SmoothAim(float pYaw, float pPitch, float tYaw, float tPitch,
                                 float sens, float& outYaw, float& outPitch)
{
    g_bowAimbotModule.smooth_aim(pYaw, pPitch, tYaw, tPitch, sens, outYaw, outPitch);
}
inline bool BowAimbot_InFov(float pYaw, double px, double pz, double tx, double tz)
{
    return g_bowAimbotModule.in_fov(pYaw, px, pz, tx, tz);
}
inline void BowAimbot_Reset() { g_bowAimbotModule.reset(); }
