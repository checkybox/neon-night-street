#ifndef CINEMATIC_H
#define CINEMATIC_H

#include <glm/glm.hpp>

enum class BlackoutState {
    Normal,
    Failing,
    Blackout,
    Restoring
};

class Cinematic {
public:
    Cinematic();

    void update(float deltaTime);

    void toggleBlackout();
    void setBlackout(bool active);
    bool isBlackout() const { return blackoutState == BlackoutState::Blackout || blackoutState == BlackoutState::Failing; }
    bool isRestoring() const { return blackoutState == BlackoutState::Restoring; }
    BlackoutState getBlackoutState() const { return blackoutState; }

    float getLightMultiplier(size_t lightIndex, const glm::vec3& lightPos) const;
    float getNeonMultiplier() const;
    float getDirLightMultiplier() const;

    void toggleCutscene(const glm::vec3& playerPos, float playerYaw, float playerPitch);
    void startCutscene(const glm::vec3& playerPos, float playerYaw, float playerPitch);
    void stopCutscene();
    bool isCutsceneActive() const { return cutsceneActive; }

    glm::vec3 getCameraPosition() const { return currentCamPos; }
    float getCameraYaw() const { return currentYaw; }
    float getCameraPitch() const { return currentPitch; }

    glm::vec3 getSavedPlayerPosition() const { return savedPlayerPos; }
    float getSavedPlayerYaw() const { return savedPlayerYaw; }
    float getSavedPlayerPitch() const { return savedPlayerPitch; }

private:
    BlackoutState blackoutState;
    float blackoutFailTimer;
    float blackoutFailDuration;
    float blackoutTimer;
    float blackoutRestoreTimer;
    float blackoutRestoreDuration;

    bool cutsceneActive;
    float cutsceneTime;
    float cutsceneTotalDuration;

    glm::vec3 currentCamPos;
    float currentYaw;
    float currentPitch;

    glm::vec3 savedPlayerPos;
    float savedPlayerYaw;
    float savedPlayerPitch;

    void updateCutsceneCamera(float t);
};

#endif
