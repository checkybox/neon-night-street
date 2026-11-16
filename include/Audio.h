#ifndef AUDIO_H
#define AUDIO_H

class Audio {
public:
    static bool init();
    static void cleanup();
    static void update(bool isWalking, bool isSprinting, float deltaTime);
    static void setRainVolume(float volume);
};

#endif
