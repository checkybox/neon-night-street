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
static std::atomic<float> g_barFlicker(1.0f);
static std::atomic<float> g_neonSpatialGainL(0.0f), g_neonSpatialGainR(0.0f);
static float g_humPhase60 = 0.0f, g_humPhase120 = 0.0f, g_humPhase180 = 0.0f;
static float g_barHumPhase1 = 0.0f, g_barHumPhase2 = 0.0f;

static std::vector<float> g_barMusicSamples, g_hotelMusicSamples, g_ramenChatterSamples;
static std::vector<float> g_konbiniChimeSamples, g_konbiniCashSamples;
static std::vector<float> g_powerOutageSamples, g_powerRestoreSamples;

static std::atomic<float> g_barSpatialGainL(0.0f), g_barSpatialGainR(0.0f);
static std::atomic<float> g_hotelSpatialGainL(0.0f), g_hotelSpatialGainR(0.0f);
static std::atomic<float> g_ramenSpatialGainL(0.0f), g_ramenSpatialGainR(0.0f);
static std::atomic<float> g_konbiniSpatialGainL(0.0f), g_konbiniSpatialGainR(0.0f);

static std::atomic<bool> g_triggerKonbiniChime(false), g_triggerKonbiniCash(false), g_triggerArcSpark(false);
static std::atomic<bool> g_triggerBreakerTrip(false), g_triggerPowerRestore(false), g_cinematicMusic(false), g_isBlackout(false);

static float g_arcSparkTimeLeft = 0.0f;
static float g_arcPhase1 = 0.0f, g_arcPhase2 = 0.0f, g_arcPhase3 = 0.0f;
static float g_breakerTripTimeLeft = 0.0f, g_breakerPhase = 0.0f;
static float g_powerRestoreTimeLeft = 0.0f, g_powerRestorePhase = 0.0f;

static size_t g_barPlayPos = 0, g_hotelPlayPos = 0, g_ramenPlayPos = 0;
static float g_barLp = 0.0f;
static size_t g_konbiniChimePlayPos = 99999999, g_konbiniCashPlayPos = 99999999;
static size_t g_powerOutagePlayPos = 99999999, g_powerRestorePlayPos = 99999999;
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

    if (g_triggerKonbiniChime.load()) { g_triggerKonbiniChime.store(false); g_konbiniChimePlayPos = 0; }
    if (g_triggerKonbiniCash.load()) { g_triggerKonbiniCash.store(false); g_konbiniCashPlayPos = 0; }
    if (g_triggerArcSpark.load()) {
        g_triggerArcSpark.store(false);
        g_arcSparkTimeLeft = 0.85f;
        g_arcPhase1 = 0.0f; g_arcPhase2 = 0.0f; g_arcPhase3 = 0.0f;
    }
    if (g_triggerBreakerTrip.load()) {
        g_triggerBreakerTrip.store(false);
        g_powerRestorePlayPos = 99999999;
        g_powerRestoreTimeLeft = 0.0f;
        if (!g_powerOutageSamples.empty()) {
            g_powerOutagePlayPos = 0;
            g_breakerTripTimeLeft = 0.0f;
        } else {
            g_breakerTripTimeLeft = 0.65f;
            g_breakerPhase = 0.0f;
        }
    }
    if (g_triggerPowerRestore.load()) {
        g_triggerPowerRestore.store(false);
        g_powerOutagePlayPos = 99999999;
        g_breakerTripTimeLeft = 0.0f;
        if (!g_powerRestoreSamples.empty()) {
            g_powerRestorePlayPos = 0;
            g_powerRestoreTimeLeft = 0.0f;
        } else {
            g_powerRestoreTimeLeft = 3.6f;
            g_powerRestorePhase = 0.0f;
        }
    }

    bool isBlackout = g_isBlackout.load();
    float flickerVal = isBlackout ? 0.0f : g_neonFlicker.load();
    float humGainL = isBlackout ? 0.0f : g_neonSpatialGainL.load();
    float humGainR = isBlackout ? 0.0f : g_neonSpatialGainR.load();

    float barGainL = isBlackout ? 0.0f : g_barSpatialGainL.load();
    float barGainR = isBlackout ? 0.0f : g_barSpatialGainR.load();
    float hotelGainL = isBlackout ? 0.0f : g_hotelSpatialGainL.load();
    float hotelGainR = isBlackout ? 0.0f : g_hotelSpatialGainR.load();
    float ramenGainL = isBlackout ? 0.0f : g_ramenSpatialGainL.load();
    float ramenGainR = isBlackout ? 0.0f : g_ramenSpatialGainR.load();
    float konbiniGainL = isBlackout ? 0.0f : g_konbiniSpatialGainL.load();
    float konbiniGainR = isBlackout ? 0.0f : g_konbiniSpatialGainR.load();

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        float whiteL = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        float whiteR = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;

        // Multi-stage filtering for asphalt rain texture with tactile micro-droplet impacts
        g_rainFiltL1 = g_rainFiltL1 * 0.90f + whiteL * 0.10f;
        g_rainFiltL2 = g_rainFiltL2 * 0.86f + g_rainFiltL1 * 0.14f;
        g_rainFiltL3 = g_rainFiltL3 * 0.82f + g_rainFiltL2 * 0.18f;
        g_rainFiltR1 = g_rainFiltR1 * 0.90f + whiteR * 0.10f;
        g_rainFiltR2 = g_rainFiltR2 * 0.86f + g_rainFiltR1 * 0.14f;
        g_rainFiltR3 = g_rainFiltR3 * 0.82f + g_rainFiltR2 * 0.18f;

        float sampleL = (g_rainFiltL2 * 0.40f + g_rainFiltL3 * 0.60f) * rainVol;
        float sampleR = (g_rainFiltR2 * 0.40f + g_rainFiltR3 * 0.60f) * rainVol;
        if (rainVol > 0.05f && (rand() % 1000) < static_cast<int>(rainVol * 150.0f)) {
            float dropPip = ((rand() % 100) / 100.0f - 0.5f) * (rainVol * 0.28f);
            sampleL += dropPip; sampleR += dropPip * 0.90f;
        }

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

        // Global dielectric arc discharge crackles triggered during surge
        if (g_arcSparkTimeLeft > 0.0f) {
            float t = 0.85f - g_arcSparkTimeLeft;
            float env = (t < 0.04f) ? (t / 0.04f) : std::exp(-3.6f * (t - 0.04f));
            float buzz = std::sin(g_arcPhase1) * 0.45f + std::sin(g_arcPhase2) * 0.35f + std::sin(g_arcPhase3) * 0.20f;
            buzz = std::tanh(buzz * 2.2f);
            if ((rand() % 10) < 5) buzz += whiteL * 0.35f;
            float arcVol = isBlackout ? 0.16f : 0.42f;
            float arcAudio = buzz * env * arcVol;
            sampleL += arcAudio;
            sampleR += arcAudio * 0.95f;
            g_arcPhase1 += 6.2831853f * 120.0f / 48000.0f;
            g_arcPhase2 += 6.2831853f * 240.0f / 48000.0f;
            g_arcPhase3 += 6.2831853f * 360.0f / 48000.0f;
            g_arcSparkTimeLeft -= 1.0f / 48000.0f;
        }

        // Industrial breaker trip snap and power outage rumble
        if (g_powerOutagePlayPos < g_powerOutageSamples.size()) {
            float s = g_powerOutageSamples[g_powerOutagePlayPos++];
            sampleL += s;
            sampleR += s * 0.95f;
        } else if (g_breakerTripTimeLeft > 0.0f) {
            float t = 0.65f - g_breakerTripTimeLeft;
            float snap = std::sin(6.2831853f * 3200.0f * t) * std::exp(-90.0f * t) * 0.45f;
            float thud = std::sin(g_breakerPhase) * std::exp(-10.0f * t) * 0.62f;
            float breakerAudio = (snap + thud) * 0.46f;
            sampleL += breakerAudio;
            sampleR += breakerAudio;
            g_breakerPhase += 6.2831853f * (55.0f - 25.0f * t) / 48000.0f;
            g_breakerTripTimeLeft -= 1.0f / 48000.0f;
        }

        // Transformer spin up and electric power restoration
        if (g_powerRestorePlayPos < g_powerRestoreSamples.size()) {
            float s = g_powerRestoreSamples[g_powerRestorePlayPos++];
            sampleL += s;
            sampleR += s * 0.95f;
        } else if (g_powerRestoreTimeLeft > 0.0f) {
            float t = 3.6f - g_powerRestoreTimeLeft;
            float p = std::min(1.0f, t / 2.6f);
            float freq = 45.0f + 38.0f * (p * (2.0f - p));
            float hum = std::sin(g_powerRestorePhase) * 0.65f + std::sin(g_powerRestorePhase * 2.0f) * 0.35f;
            float env = (t < 0.5f) ? (t / 0.5f) : ((t > 2.4f) ? 0.5f * (1.0f + std::cos(3.14159265f * (t - 2.4f) / 1.2f)) : 1.0f);
            float restoreAudio = hum * env * 0.24f;
            for (float clickT : { 0.35f, 0.95f, 1.55f, 2.15f }) {
                float dt = t - clickT;
                if (dt > 0.0f && dt < 0.04f) {
                    restoreAudio += whiteL * std::exp(-120.0f * dt) * 0.16f;
                }
            }
            sampleL += restoreAudio;
            sampleR += restoreAudio;
            g_powerRestorePhase += 6.2831853f * freq / 48000.0f;
            g_powerRestoreTimeLeft -= 1.0f / 48000.0f;
        }

        // Spatialized electrical mains hum - active during surge and strictly muted during blackout
        if (!isBlackout && (humGainL > 0.0005f || humGainR > 0.0005f) && std::abs(flickerVal - 1.0f) > 0.03f && flickerVal > 0.05f) {
            float hum60  = std::sin(g_humPhase60) * 0.25f;
            float hum120 = std::sin(g_humPhase120) * 0.55f;
            float hum180 = std::sin(g_humPhase180) * 0.20f;
            float hum = (hum60 + hum120 + hum180) * 0.45f;
            float struggle = std::abs(flickerVal - 1.0f);
            float arcCrackle = (flickerVal < 0.40f || flickerVal > 1.20f) ? whiteL * 0.22f : 0.0f;
            float buzz = (hum * (0.8f + struggle * 0.7f) + arcCrackle) * 0.55f;
            sampleL += buzz * humGainL;
            sampleR += buzz * humGainR;

            g_humPhase60  += 6.2831853f * 60.0f  / 48000.0f;
            g_humPhase120 += 6.2831853f * 120.0f / 48000.0f;
            g_humPhase180 += 6.2831853f * 180.0f / 48000.0f;
        }

        // Spatialized neon bar flicker sound with mains hum and dielectric arcing
        float barFlickerVal = isBlackout ? 0.0f : g_barFlicker.load();
        if (!isBlackout && (barGainL > 0.0005f || barGainR > 0.0005f) && std::abs(barFlickerVal - 1.0f) > 0.03f && barFlickerVal > 0.02f) {
            float barHum = (std::sin(g_barHumPhase1) * 0.45f + std::sin(g_barHumPhase2) * 0.55f);
            float barArc = (barFlickerVal < 0.45f) ? whiteL * 0.35f : 0.0f;
            float barBuzz = (barHum * 0.35f + barArc) * 0.65f * std::abs(barFlickerVal - 1.0f);
            sampleL += barBuzz * barGainL;
            sampleR += barBuzz * barGainR;
            g_barHumPhase1 += 6.2831853f * 60.0f / 48000.0f;
            g_barHumPhase2 += 6.2831853f * 120.0f / 48000.0f;
        }

        // Bar music playback: full-fidelity stereo during cutscene movie, spatialized muffled outdoors
        if (g_cinematicMusic.load()) {
            if (!g_barMusicSamples.empty()) {
                float s = g_barMusicSamples[g_barPlayPos++];
                if (g_barPlayPos >= g_barMusicSamples.size()) g_barPlayPos = 0;
                sampleL += s * 0.40f;
                sampleR += s * 0.40f;
            }
        } else if (!g_barMusicSamples.empty() && (barGainL > 0.0005f || barGainR > 0.0005f)) {
            float s = g_barMusicSamples[g_barPlayPos++];
            if (g_barPlayPos >= g_barMusicSamples.size()) g_barPlayPos = 0;
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
    for (std::string prefix : { "", "../", "../../" }) {
        if (loadAudioFile((prefix + relativePath).c_str(), outBuffer)) return true;
    }
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
    loadSampleWithFallback("assets/audio/power_outage.wav", g_powerOutageSamples);
    loadSampleWithFallback("assets/audio/power_restore.wav", g_powerRestoreSamples);

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
                   float neonFlicker,
                   float barFlicker) {
    (void)camFront;
    g_isWalking.store(isWalking);
    g_isSprinting.store(isSprinting);

    // Spatial calculations for neon sign electrical fixtures
    glm::vec3 nPos1(-6.45f, 3.55f, -22.0f);
    glm::vec3 nPos2(-6.33f, 3.6f, 3.0f);
    glm::vec3 activeNeon = (glm::length(nPos1 - camPos) < glm::length(nPos2 - camPos)) ? nPos1 : nPos2;
    computeSpatialGains(activeNeon, camPos, camRight, 40.0f, 0.24f, g_neonSpatialGainL, g_neonSpatialGainR);
    g_neonFlicker.store(neonFlicker);
    g_barFlicker.store(barFlicker);

    // Spatial calculations for points of interest
    computeSpatialGains(glm::vec3(-6.45f, 1.8f, -22.0f), camPos, camRight, 24.0f, 0.35f, g_barSpatialGainL, g_barSpatialGainR);
    computeSpatialGains(glm::vec3(7.5f, 1.8f, -22.0f), camPos, camRight, 24.0f, 0.28f, g_hotelSpatialGainL, g_hotelSpatialGainR);
    computeSpatialGains(glm::vec3(6.8f, 1.8f, 2.8f), camPos, camRight, 26.0f, 0.70f, g_ramenSpatialGainL, g_ramenSpatialGainR);
    computeSpatialGains(glm::vec3(-6.8f, 1.8f, 3.0f), camPos, camRight, 22.0f, 0.24f, g_konbiniSpatialGainL, g_konbiniSpatialGainR);

    // Periodic konbini customer activity triggering
    g_konbiniTimer -= deltaTime;
    if (g_konbiniTimer <= 0.0f) {
        if (!g_isBlackout.load()) {
            float distToStore = glm::length(glm::vec3(-6.8f, 1.8f, 3.0f) - camPos);
            if (distToStore < 32.0f) {
                if (g_konbiniAlt) g_triggerKonbiniCash.store(true);
                else g_triggerKonbiniChime.store(true);
                g_konbiniAlt = !g_konbiniAlt;
            }
        }
        g_konbiniTimer = 11.0f + static_cast<float>(rand() % 9);
    }

    // Periodic thunder trigger
    g_thunderTimer -= deltaTime;
    if (g_thunderTimer <= 0.0f) {
        g_thunderTimeLeft = 3.4f;
        g_thunderPhase1 = 0.0f; g_thunderPhase2 = 0.0f;
        g_thunderTimer = 18.0f + static_cast<float>(rand() % 16);
    }

    if (isWalking) {
        g_stepAccumulator += deltaTime;
        float interval = isSprinting ? 0.33f : 0.46f;
        if (g_stepAccumulator >= interval) {
            g_stepAccumulator = 0.0f;
            g_footstepAlt = !g_footstepAlt;
            float pitchJitter = (((rand() % 100) / 100.0f) - 0.5f) * 0.12f;
            float volJitter = (((rand() % 100) / 100.0f) - 0.5f) * 0.06f;
            float pitch = (isSprinting ? 1.05f : 0.98f) + (g_footstepAlt ? -0.025f : 0.025f) + pitchJitter;
            float volume = (isSprinting ? 0.38f : 0.26f) + volJitter;
            g_nextStepPitch.store(pitch);
            g_nextStepVolume.store(volume);
            g_triggerFootstep.store(true);
            g_footstepTimeLeft = 0.11f;
        }
    } else {
        g_stepAccumulator = 0.38f;
    }
}

void Audio::setRainVolume(float volume) { g_rainVolume.store(volume); }
void Audio::triggerArcSpark() { g_triggerArcSpark.store(true); }
void Audio::triggerBreakerTrip() { g_triggerBreakerTrip.store(true); }
void Audio::triggerPowerRestore() { g_triggerPowerRestore.store(true); }
void Audio::setCinematicMusic(bool enabled) { g_cinematicMusic.store(enabled); }
void Audio::setBlackout(bool active) { g_isBlackout.store(active); }
