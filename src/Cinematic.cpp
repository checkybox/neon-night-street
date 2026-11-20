#include "Cinematic.h"
#include "Audio.h"
#include <cmath>
#include <algorithm>

Cinematic::Cinematic()
    : blackoutState(BlackoutState::Normal),
      blackoutFailTimer(0.0f),
      blackoutFailDuration(1.4f),
      blackoutTimer(0.0f),
      blackoutRestoreTimer(0.0f),
      blackoutRestoreDuration(3.6f),
      cutsceneActive(false),
      cutsceneTime(0.0f),
      cutsceneTotalDuration(44.5f),
      currentCamPos(0.0f),
      currentYaw(-90.0f),
      currentPitch(0.0f),
      savedPlayerPos(0.0f),
      savedPlayerYaw(-90.0f),
      savedPlayerPitch(0.0f) {}

void Cinematic::toggleBlackout() {
    if (blackoutState == BlackoutState::Normal) {
        blackoutState = BlackoutState::Failing;
        blackoutFailTimer = 0.0f;
        blackoutTimer = 0.0f;
        Audio::triggerBreakerTrip();
    } else {
        blackoutState = BlackoutState::Restoring;
        blackoutRestoreTimer = 0.0f;
        Audio::triggerPowerRestore();
    }
}

void Cinematic::setBlackout(bool active) {
    if (active && blackoutState != BlackoutState::Blackout && blackoutState != BlackoutState::Failing) {
        blackoutState = BlackoutState::Failing;
        blackoutFailTimer = 0.0f;
        blackoutTimer = 0.0f;
        Audio::triggerBreakerTrip();
    } else if (!active && blackoutState != BlackoutState::Normal) {
        blackoutState = BlackoutState::Restoring;
        blackoutRestoreTimer = 0.0f;
        Audio::triggerPowerRestore();
    }
}

float Cinematic::getLightMultiplier(size_t lightIndex, const glm::vec3& lightPos) const {
    if (blackoutState == BlackoutState::Normal) return 1.0f;
    if (blackoutState == BlackoutState::Blackout) return 0.0f;

    if (blackoutState == BlackoutState::Failing) {
        float t = blackoutFailTimer;
        float zNorm = std::max(0.0f, std::min(1.0f, (lightPos.z + 65.0f) / 110.0f));
        float shutoff = 0.15f + (1.0f - zNorm) * 0.85f + std::fmod(static_cast<float>(lightIndex) * 0.19f, 0.35f);
        if (t > shutoff) return 0.0f;
        float dt = shutoff - t;
        if (dt < 0.20f) {
            return (std::fmod(dt * 30.0f, 1.0f) > 0.45f) ? 1.4f : 0.0f;
        }
        return 1.0f;
    }

    float t = blackoutRestoreTimer;
    float zNorm = std::max(0.0f, std::min(1.0f, (lightPos.z + 65.0f) / 110.0f));
    float delay = 0.32f + zNorm * 2.1f + std::fmod(static_cast<float>(lightIndex) * 0.27f, 0.42f);

    if (t < delay) return 0.0f;
    float dt = t - delay;
    if (dt < 0.07f) return 1.45f;
    if (dt < 0.15f) return 0.06f;
    if (dt < 0.24f) return 1.30f;
    if (dt < 0.31f) return 0.20f;
    if (dt < 0.42f) return 0.80f + (dt - 0.31f) * 1.8f;
    return 1.0f;
}

float Cinematic::getNeonMultiplier() const {
    if (blackoutState == BlackoutState::Normal) return 1.0f;
    if (blackoutState == BlackoutState::Blackout) return 0.0f;

    if (blackoutState == BlackoutState::Failing) {
        float t = blackoutFailTimer;
        if (t > 0.65f) return 0.0f;
        if (t > 0.45f) return ((static_cast<int>(t * 50.0f) % 2 == 0) ? 1.6f : 0.0f);
        if (t > 0.25f) return 0.3f;
        return 1.0f;
    }

    float t = blackoutRestoreTimer;
    if (t < 1.05f) return 0.0f;
    float dt = t - 1.05f;
    if (dt < 0.08f) return 1.55f;
    if (dt < 0.18f) return 0.05f;
    if (dt < 0.28f) return 1.35f;
    if (dt < 0.36f) return 0.22f;
    if (dt < 0.50f) return 0.80f + (dt - 0.36f) * 1.42f;
    return 1.0f;
}

float Cinematic::getDirLightMultiplier() const {
    if (blackoutState == BlackoutState::Normal) return 1.0f;
    if (blackoutState == BlackoutState::Failing) {
        float p = std::min(1.0f, blackoutFailTimer / 1.4f);
        return 1.0f - 0.85f * p;
    }
    if (blackoutState == BlackoutState::Blackout) {
        float adapt = std::min(1.0f, blackoutTimer / 10.0f);
        float smooth = adapt * adapt * (3.0f - 2.0f * adapt);
        return 0.15f + 0.11f * smooth;
    }
    float p = std::min(1.0f, blackoutRestoreTimer / 3.0f);
    return 0.26f + 0.74f * (p * (2.0f - p));
}

void Cinematic::startCutscene(const glm::vec3& playerPos, float playerYaw, float playerPitch) {
    cutsceneActive = true;
    cutsceneTime = 0.0f;
    savedPlayerPos = playerPos;
    savedPlayerYaw = playerYaw;
    savedPlayerPitch = playerPitch;

    blackoutState = BlackoutState::Blackout;
    blackoutTimer = 0.0f;
    Audio::triggerBreakerTrip();
    Audio::setCinematicMusic(false);

    updateCutsceneCamera(0.0f);
}

void Cinematic::stopCutscene() {
    if (!cutsceneActive) return;
    cutsceneActive = false;
    Audio::setCinematicMusic(false);

    if (blackoutState != BlackoutState::Normal) {
        blackoutState = BlackoutState::Restoring;
        blackoutRestoreTimer = 1.5f;
    }
}

void Cinematic::toggleCutscene(const glm::vec3& playerPos, float playerYaw, float playerPitch) {
    if (cutsceneActive) {
        stopCutscene();
    } else {
        startCutscene(playerPos, playerYaw, playerPitch);
    }
}

void Cinematic::update(float deltaTime) {
    if (blackoutState == BlackoutState::Failing) {
        blackoutFailTimer += deltaTime;
        if (blackoutFailTimer >= blackoutFailDuration) {
            blackoutState = BlackoutState::Blackout;
            blackoutTimer = 0.0f;
        }
    } else if (blackoutState == BlackoutState::Blackout) {
        blackoutTimer += deltaTime;
    } else if (blackoutState == BlackoutState::Restoring) {
        blackoutRestoreTimer += deltaTime;
        if (blackoutRestoreTimer >= blackoutRestoreDuration) {
            blackoutState = BlackoutState::Normal;
            blackoutRestoreTimer = 0.0f;
        }
    }

    if (!cutsceneActive) return;

    cutsceneTime += deltaTime;

    if (cutsceneTime >= 1.2f && cutsceneTime - deltaTime < 1.2f) {
        blackoutState = BlackoutState::Restoring;
        blackoutRestoreTimer = 0.0f;
        Audio::triggerPowerRestore();
    }

    if (cutsceneTime >= 4.5f && cutsceneTime - deltaTime < 4.5f) {
        Audio::setCinematicMusic(true);
    }

    if (cutsceneTime >= cutsceneTotalDuration) {
        stopCutscene();
        return;
    }

    updateCutsceneCamera(cutsceneTime);
}

void Cinematic::updateCutsceneCamera(float t) {
    if (t < 4.5f) {
        // Low tracking shot along wet asphalt center line during blackout and sequential grid ignition
        float progress = t / 4.5f;
        currentCamPos = glm::vec3(0.0f, 0.45f, 32.0f - progress * 16.0f);
        currentYaw = -90.0f;
        currentPitch = 7.5f;
    } else if (t < 8.5f) {
        // Street-level dolly past parked cars and illuminated 24/7 konbini storefront clear of lamppost
        float progress = (t - 4.5f) / 4.0f;
        currentCamPos = glm::vec3(-2.2f, 1.4f, 16.0f) + progress * glm::vec3(-0.3f, 0.25f, -10.0f);
        currentYaw = -145.0f + progress * 25.0f;
        currentPitch = 4.0f;
    } else if (t < 12.5f) {
        // Konbini backside restricted service yard with wire fence, dumpsters, and amber service door
        float progress = (t - 8.5f) / 4.0f;
        currentCamPos = glm::vec3(-15.2f, 1.9f, -0.5f) + progress * glm::vec3(0.6f, 0.2f, 6.0f);
        currentYaw = 45.0f - progress * 30.0f;
        currentPitch = 3.0f;
    } else if (t < 16.5f) {
        // Wide avenue glide viewing ramen dining counter, paper lanterns, and overhead neon sign
        float progress = (t - 12.5f) / 4.0f;
        currentCamPos = glm::vec3(-0.8f, 2.2f, 9.5f) + progress * glm::vec3(1.4f, 0.3f, -12.5f);
        currentYaw = 35.0f - progress * 40.0f;
        currentPitch = 12.0f - progress * 4.0f;
    } else if (t < 20.5f) {
        // Clean road-lane framing of bar entrance neon signage clear of lamppost pole
        float progress = (t - 16.5f) / 4.0f;
        currentCamPos = glm::vec3(-2.2f, 1.6f, -15.5f) + progress * glm::vec3(-0.3f, 0.45f, -7.5f);
        currentYaw = -135.0f + progress * 20.0f;
        currentPitch = 14.0f - progress * 6.0f;
    } else if (t < 24.5f) {
        // Chinese izakaya shop with red awning, hanging glowing chochin lanterns, and sliding door (right-to-left sweep)
        float progress = (t - 20.5f) / 4.0f;
        currentCamPos = glm::vec3(-2.8f, 1.8f, -56.5f) + progress * glm::vec3(0.6f, -0.2f, 10.0f);
        currentYaw = 145.0f + progress * 70.0f;
        currentPitch = 10.0f - progress * 4.0f;
    } else if (t < 28.5f) {
        // Atmospheric cross-street corridor between ramen shop dining nook and hotel building
        float progress = (t - 24.5f) / 4.0f;
        currentCamPos = glm::vec3(-1.8f, 1.65f, 2.5f) + progress * glm::vec3(1.0f, 0.35f, -14.5f);
        currentYaw = 18.0f - progress * 32.0f;
        currentPitch = 8.0f - progress * 2.0f;
    } else if (t < 33.5f) {
        // Towering hotel facade head-on vertical pedestal rise showcasing the cyan HOTEL neon sign
        float progress = (t - 28.5f) / 5.0f;
        currentCamPos = glm::vec3(-2.0f, 5.5f, -47.0f) + progress * glm::vec3(0.8f, 8.5f, 0.2f);
        currentYaw = 2.0f - progress * 4.0f;
        currentPitch = 48.0f - progress * 26.0f;
    } else if (t < 38.5f) {
        // Left-to-right flyout capturing detailed vending machine, parked car, and pharmacy storefront
        float progress = (t - 33.5f) / 5.0f;
        currentCamPos = glm::vec3(-2.6f, 1.85f, 38.5f) + progress * glm::vec3(-0.6f, 0.40f, -9.5f);
        currentYaw = -155.0f + progress * 25.0f;
        currentPitch = 8.0f + progress * 4.0f;
    } else {
        // High cinematic crane descent overlooking entire avenue, wet reflections, and towering skyline
        float progress = (t - 38.5f) / 6.0f;
        currentCamPos = glm::vec3(5.5f, 15.5f, -28.0f) + progress * glm::vec3(-6.5f, -11.0f, 38.0f);
        currentYaw = 160.0f - progress * 50.0f;
        currentPitch = -22.0f + progress * 16.0f;
    }
}
