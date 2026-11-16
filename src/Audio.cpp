#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "Audio.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <atomic>
#include <vector>

static ma_device g_audioDevice;
static bool g_audioInitialized = false;
static std::atomic<bool> g_isWalking(false);
static std::atomic<bool> g_isSprinting(false);
static std::atomic<float> g_rainVolume(0.28f);

static float g_rainFiltL1 = 0.0f, g_rainFiltL2 = 0.0f, g_rainFiltL3 = 0.0f;
static float g_rainFiltR1 = 0.0f, g_rainFiltR2 = 0.0f, g_rainFiltR3 = 0.0f;

static std::vector<float> g_footstepSamples;
static std::atomic<bool> g_triggerFootstep(false);
static std::atomic<float> g_nextStepPitch(1.0f);
static std::atomic<float> g_nextStepVolume(0.28f);

static float g_footstepPlayPos = 999999.0f;
static float g_footstepPitch = 1.0f;
static float g_footstepVolume = 0.28f;
static bool g_footstepAlt = false;
static float g_stepAccumulator = 0.0f;

static float g_footstepTimeLeft = 0.0f;
static float g_footstepPhaseBass = 0.0f;
static float g_footstepPhaseSnap = 0.0f;

static float g_thunderTimeLeft = 0.0f;
static float g_thunderPhase1 = 0.0f;
static float g_thunderPhase2 = 0.0f;
static float g_thunderTimer = 8.0f;

static void audioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    (void)pDevice;
    (void)pInput;
    float* out = static_cast<float*>(pOutput);
    float rainVol = g_rainVolume.load();

    if (g_triggerFootstep.load()) {
        g_triggerFootstep.store(false);
        g_footstepPlayPos = 0.0f;
        g_footstepPitch = g_nextStepPitch.load();
        g_footstepVolume = g_nextStepVolume.load();
    }

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        float whiteL = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        float whiteR = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;

        // Multi-pole low-pass filtering for soft organic rain patter without white noise hiss
        g_rainFiltL1 = g_rainFiltL1 * 0.94f + whiteL * 0.06f;
        g_rainFiltL2 = g_rainFiltL2 * 0.91f + g_rainFiltL1 * 0.09f;
        g_rainFiltL3 = g_rainFiltL3 * 0.88f + g_rainFiltL2 * 0.12f;

        g_rainFiltR1 = g_rainFiltR1 * 0.94f + whiteR * 0.06f;
        g_rainFiltR2 = g_rainFiltR2 * 0.91f + g_rainFiltR1 * 0.09f;
        g_rainFiltR3 = g_rainFiltR3 * 0.88f + g_rainFiltR2 * 0.12f;

        float sampleL = g_rainFiltL3 * rainVol;
        float sampleR = g_rainFiltR3 * rainVol;

        // Distant rolling thunder rumble
        if (g_thunderTimeLeft > 0.0f) {
            float t = 3.4f - g_thunderTimeLeft;
            float env = (t < 0.45f) ? (t / 0.45f) : std::exp(-0.75f * (t - 0.45f));
            float rumbleL = (std::sin(g_thunderPhase1) * 0.65f + std::sin(g_thunderPhase2) * 0.35f) * (0.85f + 0.15f * whiteL) * env * 0.28f;
            float rumbleR = (std::sin(g_thunderPhase1 * 1.04f) * 0.60f + std::sin(g_thunderPhase2 * 0.98f) * 0.40f) * (0.85f + 0.15f * whiteR) * env * 0.28f;

            sampleL += rumbleL;
            sampleR += rumbleR;

            g_thunderPhase1 += 6.2831853f * 46.0f / 48000.0f;
            g_thunderPhase2 += 6.2831853f * 62.0f / 48000.0f;
            g_thunderTimeLeft -= 1.0f / 48000.0f;
        }

        // Authentic recorded footstep playback with linear resampling
        if (!g_footstepSamples.empty()) {
            size_t idx = static_cast<size_t>(g_footstepPlayPos);
            if (idx + 1 < g_footstepSamples.size()) {
                float frac = g_footstepPlayPos - static_cast<float>(idx);
                float s = g_footstepSamples[idx] * (1.0f - frac) + g_footstepSamples[idx + 1] * frac;
                float stepAudio = s * g_footstepVolume;
                sampleL += stepAudio;
                sampleR += stepAudio;
                g_footstepPlayPos += g_footstepPitch;
            }
        } else if (g_footstepTimeLeft > 0.0f) {
            // Procedural fallback
            float t = 0.11f - g_footstepTimeLeft;
            float snap = std::sin(g_footstepPhaseSnap) * std::exp(-75.0f * t) * 0.32f;
            float bass = std::sin(g_footstepPhaseBass) * std::exp(-28.0f * t) * 0.55f;
            float step = snap + bass;

            sampleL += step;
            sampleR += step;

            g_footstepPhaseSnap += 6.2831853f * 360.0f / 48000.0f;
            g_footstepPhaseBass += 6.2831853f * 90.0f / 48000.0f;
            g_footstepTimeLeft -= 1.0f / 48000.0f;
        }

        out[i * 2 + 0] = sampleL;
        out[i * 2 + 1] = sampleR;
    }
}

static bool loadFootstepSample(const char* filePath) {
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 48000);
    ma_decoder decoder;
    if (ma_decoder_init_file(filePath, &config, &decoder) != MA_SUCCESS) {
        return false;
    }
    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    if (totalFrames > 0) {
        g_footstepSamples.resize(static_cast<size_t>(totalFrames));
        ma_uint64 framesRead = 0;
        ma_decoder_read_pcm_frames(&decoder, g_footstepSamples.data(), totalFrames, &framesRead);
    }
    ma_decoder_uninit(&decoder);
    return !g_footstepSamples.empty();
}

bool Audio::init() {
    const char* samplePaths[] = {
        "assets/audio/footstep_concrete.wav",
        "../assets/audio/footstep_concrete.wav",
        "../../assets/audio/footstep_concrete.wav"
    };
    for (const char* path : samplePaths) {
        if (loadFootstepSample(path)) {
            break;
        }
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = 2;
    config.sampleRate        = 48000;
    config.dataCallback      = audioDataCallback;

    if (ma_device_init(NULL, &config, &g_audioDevice) != MA_SUCCESS) {
        std::cerr << "[Audio] Failed to initialize audio playback device." << std::endl;
        return false;
    }

    if (ma_device_start(&g_audioDevice) != MA_SUCCESS) {
        std::cerr << "[Audio] Failed to start audio playback device." << std::endl;
        ma_device_uninit(&g_audioDevice);
        return false;
    }

    g_audioInitialized = true;
    return true;
}

void Audio::cleanup() {
    if (g_audioInitialized) {
        ma_device_stop(&g_audioDevice);
        ma_device_uninit(&g_audioDevice);
        g_audioInitialized = false;
    }
}

void Audio::update(bool isWalking, bool isSprinting, float deltaTime) {
    g_isWalking.store(isWalking);
    g_isSprinting.store(isSprinting);

    // Periodic thunder trigger
    g_thunderTimer -= deltaTime;
    if (g_thunderTimer <= 0.0f) {
        g_thunderTimeLeft = 3.4f;
        g_thunderPhase1 = 0.0f;
        g_thunderPhase2 = 0.0f;
        g_thunderTimer = 18.0f + static_cast<float>(rand() % 16);
    }

    // Footstep pacing: 0.33s for sprinting, 0.46s for walking
    if (isWalking) {
        g_stepAccumulator += deltaTime;
        float interval = isSprinting ? 0.33f : 0.46f;
        if (g_stepAccumulator >= interval) {
            g_stepAccumulator = 0.0f;
            g_footstepAlt = !g_footstepAlt;
            float pitch = isSprinting ? 1.06f : (g_footstepAlt ? 0.98f : 1.02f);
            float volume = isSprinting ? 0.40f : 0.28f;
            g_nextStepPitch.store(pitch);
            g_nextStepVolume.store(volume);
            g_triggerFootstep.store(true);
            g_footstepTimeLeft = 0.11f;
        }
    } else {
        g_stepAccumulator = 0.38f;
    }
}

void Audio::setRainVolume(float volume) {
    g_rainVolume.store(volume);
}
