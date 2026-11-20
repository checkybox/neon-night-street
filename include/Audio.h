#ifndef AUDIO_H
#define AUDIO_H

#include <glm/glm.hpp>

class Audio {
public:
    static bool init();
    static void cleanup();
    static void update(bool isWalking, bool isSprinting, float deltaTime,
                       const glm::vec3& camPos = glm::vec3(0.0f),
                       const glm::vec3& camFront = glm::vec3(0.0f, 0.0f, -1.0f),
                       const glm::vec3& camRight = glm::vec3(1.0f, 0.0f, 0.0f),
                       float neonFlicker = 1.0f,
                       float barFlicker = 1.0f);
    static void setRainVolume(float volume);
    static void triggerArcSpark();
    static void triggerBreakerTrip();
    static void triggerPowerRestore();
    static void setCinematicMusic(bool enabled);
    static void setBlackout(bool active);
};

#endif
