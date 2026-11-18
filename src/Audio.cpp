#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "Audio.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <atomic>
#include <vector>
#include <string>
#include <algorithm>

static ma_device g_audioDevice;
static bool g_audioInitialized = false;
static std::atomic<bool> g_isWalking(false);
static std::atomic<bool> g_isSprinting(false);
static std::atomic<float> g_rainVolume(0.38f);

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

static std::atomic<float> g_neonFlicker(1.0f);
static std::atomic<float> g_neonSpatialGainL(0.0f);
static std::atomic<float> g_neonSpatialGainR(0.0f);

static float g_humPhase60 = 0.0f;
static float g_humPhase120 = 0.0f;
static float g_humPhase180 = 0.0f;

static std::vector<float> g_barMusicSamples;
static std::vector<float> g_hotelMusicSamples;
static std::vector<float> g_ramenChatterSamples;
static std::vector<float> g_konbiniChimeSamples;
static std::vector<float> g_konbiniCashSamples;

static std::atomic<float> g_barSpatialGainL(0.0f);
static std::atomic<float> g_barSpatialGainR(0.0f);
static std::atomic<float> g_hotelSpatialGainL(0.0f);
static std::atomic<float> g_hotelSpatialGainR(0.0f);
static std::atomic<float> g_ramenSpatialGainL(0.0f);
static std::atomic<float> g_ramenSpatialGainR(0.0f);
static std::atomic<float> g_konbiniSpatialGainL(0.0f);
static std::atomic<float> g_konbiniSpatialGainR(0.0f);

static std::atomic<bool> g_triggerKonbiniChime(false);
static std::atomic<bool> g_triggerKonbiniCash(false);

static size_t g_barPlayPos = 0;
static float g_barLp = 0.0f;
static size_t g_hotelPlayPos = 0;
static size_t g_ramenPlayPos = 0;
static size_t g_konbiniChimePlayPos = 99999999;
static size_t g_konbiniCashPlayPos = 99999999;
static float g_konbiniTimer = 10.0f;
static bool g_konbiniAlt = false;

static void audioDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    (void)pDevice;
    (void)pInput;
    float* out = static_cast<float*>(pOutput);
    float rainVol = g_rainVolume.load();

    if (g_triggerFootstep.load()) {
        g_triggerFootstep.store(false);
        g_footstepPlayPos = static_cast<float>(rand() % 48);
        g_footstepPitch = g_nextStepPitch.load();
        g_footstepVolume = g_nextStepVolume.load();
    }

    if (g_triggerKonbiniChime.load()) {
        g_triggerKonbiniChime.store(false);
        g_konbiniChimePlayPos = 0;
    }
    if (g_triggerKonbiniCash.load()) {
        g_triggerKonbiniCash.store(false);
        g_konbiniCashPlayPos = 0;
    }

    float flickerVal = g_neonFlicker.load();
    float humGainL = g_neonSpatialGainL.load();
    float humGainR = g_neonSpatialGainR.load();

    float barGainL = g_barSpatialGainL.load();
    float barGainR = g_barSpatialGainR.load();
    float hotelGainL = g_hotelSpatialGainL.load();
    float hotelGainR = g_hotelSpatialGainR.load();
    float ramenGainL = g_ramenSpatialGainL.load();
    float ramenGainR = g_ramenSpatialGainR.load();
    float konbiniGainL = g_konbiniSpatialGainL.load();
    float konbiniGainR = g_konbiniSpatialGainR.load();

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        float whiteL = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        float whiteR = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;

        // Multi-pole low-pass filtering for organic rain patter with audible rainfall droplets
        g_rainFiltL1 = g_rainFiltL1 * 0.92f + whiteL * 0.08f;
        g_rainFiltL2 = g_rainFiltL2 * 0.89f + g_rainFiltL1 * 0.11f;
        g_rainFiltL3 = g_rainFiltL3 * 0.86f + g_rainFiltL2 * 0.14f;

        g_rainFiltR1 = g_rainFiltR1 * 0.92f + whiteR * 0.08f;
        g_rainFiltR2 = g_rainFiltR2 * 0.89f + g_rainFiltR1 * 0.11f;
        g_rainFiltR3 = g_rainFiltR3 * 0.86f + g_rainFiltR2 * 0.14f;

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

        // Subdued 3D spatialized electrical mains hum and dielectric arc crackles - active ONLY during surge
        if ((humGainL > 0.0005f || humGainR > 0.0005f) && std::abs(flickerVal - 1.0f) > 0.03f) {
            float hum60  = std::sin(g_humPhase60) * 0.25f;
            float hum120 = std::sin(g_humPhase120) * 0.55f;
            float hum180 = std::sin(g_humPhase180) * 0.20f;
            float hum = (hum60 + hum120 + hum180) * 0.45f;

            float struggle = std::abs(flickerVal - 1.0f);

            float arcCrackle = 0.0f;
            if (flickerVal < 0.40f || flickerVal > 1.20f) {
                arcCrackle = whiteL * 0.22f;
            }

            float buzz = (hum * (0.8f + struggle * 0.7f) + arcCrackle) * 0.055f;
            sampleL += buzz * humGainL;
            sampleR += buzz * humGainR;

            g_humPhase60  += 6.2831853f * 60.0f  / 48000.0f;
            g_humPhase120 += 6.2831853f * 120.0f / 48000.0f;
            g_humPhase180 += 6.2831853f * 180.0f / 48000.0f;
        }

        // Muffled bar music thumping through exterior facade
        if (!g_barMusicSamples.empty() && (barGainL > 0.0005f || barGainR > 0.0005f)) {
            float s = g_barMusicSamples[g_barPlayPos++];
            if (g_barPlayPos >= g_barMusicSamples.size()) g_barPlayPos = 0;
            // Single-pole low-pass filter at approximately 350 Hz for authentic wall muffling
            g_barLp = g_barLp * 0.88f + s * 0.12f;
            sampleL += g_barLp * barGainL;
            sampleR += g_barLp * barGainR;
        } else if (!g_barMusicSamples.empty()) {
            g_barPlayPos = (g_barPlayPos + 1) % g_barMusicSamples.size();
        }

        // Calm hotel elevator and lounge music
        if (!g_hotelMusicSamples.empty() && (hotelGainL > 0.0005f || hotelGainR > 0.0005f)) {
            float s = g_hotelMusicSamples[g_hotelPlayPos++];
            if (g_hotelPlayPos >= g_hotelMusicSamples.size()) g_hotelPlayPos = 0;
            sampleL += s * hotelGainL;
            sampleR += s * hotelGainR;
        } else if (!g_hotelMusicSamples.empty()) {
            g_hotelPlayPos = (g_hotelPlayPos + 1) % g_hotelMusicSamples.size();
        }

        // Ramen dining nook ambient customer chatter
        if (!g_ramenChatterSamples.empty() && (ramenGainL > 0.0005f || ramenGainR > 0.0005f)) {
            float s = g_ramenChatterSamples[g_ramenPlayPos++];
            if (g_ramenPlayPos >= g_ramenChatterSamples.size()) g_ramenPlayPos = 0;
            sampleL += s * ramenGainL;
            sampleR += s * ramenGainR;
        } else if (!g_ramenChatterSamples.empty()) {
            g_ramenPlayPos = (g_ramenPlayPos + 1) % g_ramenChatterSamples.size();
        }

        // Konbini entrance door chime
        if (g_konbiniChimePlayPos < g_konbiniChimeSamples.size()) {
            float s = g_konbiniChimeSamples[g_konbiniChimePlayPos++];
            sampleL += s * konbiniGainL;
            sampleR += s * konbiniGainR;
        }

        // Konbini cash register purchase event
        if (g_konbiniCashPlayPos < g_konbiniCashSamples.size()) {
            float s = g_konbiniCashSamples[g_konbiniCashPlayPos++] * 0.58f;
            sampleL += s * konbiniGainL;
            sampleR += s * konbiniGainR;
        }

        out[i * 2 + 0] = std::max(-1.0f, std::min(1.0f, sampleL));
        out[i * 2 + 1] = std::max(-1.0f, std::min(1.0f, sampleR));
    }
}

static bool loadAudioFile(const char* filePath, std::vector<float>& outBuffer) {
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 1, 48000);
    ma_decoder decoder;
    if (ma_decoder_init_file(filePath, &config, &decoder) != MA_SUCCESS) {
        return false;
    }
    const size_t chunkSize = 16384;
    std::vector<float> chunk(chunkSize);
    ma_uint64 framesRead = 0;
    while (ma_decoder_read_pcm_frames(&decoder, chunk.data(), chunkSize, &framesRead) == MA_SUCCESS && framesRead > 0) {
        outBuffer.insert(outBuffer.end(), chunk.begin(), chunk.begin() + framesRead);
    }
    ma_decoder_uninit(&decoder);
    return !outBuffer.empty();
}

static bool loadSampleWithFallback(const char* relativePath, std::vector<float>& outBuffer) {
    std::string p1 = relativePath;
    std::string p2 = std::string("../") + relativePath;
    std::string p3 = std::string("../../") + relativePath;
    if (loadAudioFile(p1.c_str(), outBuffer)) return true;
    if (loadAudioFile(p2.c_str(), outBuffer)) return true;
    if (loadAudioFile(p3.c_str(), outBuffer)) return true;
    return false;
}

static void computeSpatialGains(const glm::vec3& poiPos, const glm::vec3& camPos, const glm::vec3& camRight,
                                float maxDist, float baseVol,
                                std::atomic<float>& outL, std::atomic<float>& outR) {
    glm::vec3 toPoi = poiPos - camPos;
    float dist = glm::length(toPoi);
    if (dist >= maxDist) {
        outL.store(0.0f);
        outR.store(0.0f);
        return;
    }
    float norm = dist / maxDist;
    float gain = baseVol * (1.0f - norm) * (1.0f - norm);
    if (dist > 0.001f) {
        glm::vec3 dir = toPoi / dist;
        float pan = glm::dot(dir, camRight);
        pan = std::max(-1.0f, std::min(1.0f, pan));
        outL.store(gain * std::sqrt(0.5f * (1.0f - pan)));
        outR.store(gain * std::sqrt(0.5f * (1.0f + pan)));
    } else {
        outL.store(gain * 0.5f);
        outR.store(gain * 0.5f);
    }
}

bool Audio::init() {
    loadSampleWithFallback("assets/audio/footstep_concrete.wav", g_footstepSamples);
    loadSampleWithFallback("assets/audio/bar_music.wav", g_barMusicSamples);
    loadSampleWithFallback("assets/audio/hotel_lounge.mp3", g_hotelMusicSamples);
    if (!loadSampleWithFallback("assets/audio/ramen_chatter.wav", g_ramenChatterSamples)) {
        loadSampleWithFallback("assets/audio/ramen_chatter.mp3", g_ramenChatterSamples);
    }
    loadSampleWithFallback("assets/audio/konbini_chime.mp3", g_konbiniChimeSamples);
    loadSampleWithFallback("assets/audio/konbini_cash.mp3", g_konbiniCashSamples);

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = 2;
    config.sampleRate        = 48000;
    config.dataCallback      = audioDataCallback;

    if (ma_device_init(NULL, &config, &g_audioDevice) != MA_SUCCESS) {
        return false;
    }

    if (ma_device_start(&g_audioDevice) != MA_SUCCESS) {
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

void Audio::update(bool isWalking, bool isSprinting, float deltaTime,
                   const glm::vec3& camPos,
                   const glm::vec3& camFront,
                   const glm::vec3& camRight,
                   float neonFlicker) {
    (void)camFront;
    g_isWalking.store(isWalking);
    g_isSprinting.store(isSprinting);

    // 3D spatial calculations for neon sign electrical fixture
    computeSpatialGains(glm::vec3(-6.45f, 3.55f, -22.0f), camPos, camRight, 18.0f, 0.18f,
                        g_neonSpatialGainL, g_neonSpatialGainR);
    g_neonFlicker.store(neonFlicker);

    // 3D spatial calculations for Points of Interest
    computeSpatialGains(glm::vec3(-6.45f, 1.8f, -22.0f), camPos, camRight, 24.0f, 0.35f,
                        g_barSpatialGainL, g_barSpatialGainR);
    computeSpatialGains(glm::vec3(7.5f, 1.8f, -22.0f), camPos, camRight, 24.0f, 0.28f,
                        g_hotelSpatialGainL, g_hotelSpatialGainR);
    computeSpatialGains(glm::vec3(6.8f, 1.8f, 2.8f), camPos, camRight, 26.0f, 0.70f,
                        g_ramenSpatialGainL, g_ramenSpatialGainR);
    computeSpatialGains(glm::vec3(-6.8f, 1.8f, 3.0f), camPos, camRight, 22.0f, 0.24f,
                        g_konbiniSpatialGainL, g_konbiniSpatialGainR);

    // Periodic konbini customer activity triggering
    g_konbiniTimer -= deltaTime;
    if (g_konbiniTimer <= 0.0f) {
        float distToStore = glm::length(glm::vec3(-6.8f, 1.8f, 3.0f) - camPos);
        if (distToStore < 32.0f) {
            if (g_konbiniAlt) {
                g_triggerKonbiniCash.store(true);
            } else {
                g_triggerKonbiniChime.store(true);
            }
            g_konbiniAlt = !g_konbiniAlt;
        }
        g_konbiniTimer = 11.0f + static_cast<float>(rand() % 9);
    }

    // Periodic thunder trigger
    g_thunderTimer -= deltaTime;
    if (g_thunderTimer <= 0.0f) {
        g_thunderTimeLeft = 3.4f;
        g_thunderPhase1 = 0.0f;
        g_thunderPhase2 = 0.0f;
        g_thunderTimer = 18.0f + static_cast<float>(rand() % 16);
    }

    // Footstep pacing: 0.33s for sprinting, 0.46s for walking with natural randomization
    if (isWalking) {
        g_stepAccumulator += deltaTime;
        float interval = isSprinting ? 0.33f : 0.46f;
        if (g_stepAccumulator >= interval) {
            g_stepAccumulator = 0.0f;
            g_footstepAlt = !g_footstepAlt;
            float pitchJitter = (((rand() % 100) / 100.0f) - 0.5f) * 0.12f;
            float volJitter = (((rand() % 100) / 100.0f) - 0.5f) * 0.06f;
            float basePitch = isSprinting ? 1.05f : 0.98f;
            float altPitch = g_footstepAlt ? -0.025f : 0.025f;
            float pitch = basePitch + altPitch + pitchJitter;
            float baseVol = isSprinting ? 0.38f : 0.26f;
            float volume = baseVol + volJitter;
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
