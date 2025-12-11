#ifndef SC_API_AUDIO_AUDIO_CAPTURER_H
#define SC_API_AUDIO_AUDIO_CAPTURER_H

// clang-format off
#include <Windows.h>
#include <initguid.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>
// clang-format on

#include <span>

class AudioCapturer {
public:
    struct Format {
        size_t sample_rate;
        size_t channels;
    };

    AudioCapturer();
    ~AudioCapturer();

    std::span<float> tryGetSamples();

    Format getFormat();

private:
    void releaseSamples();

    IMMDeviceEnumerator* enumerator    = nullptr;
    IMMDevice*           renderDevice  = nullptr;
    IAudioClient*        audioClient   = nullptr;
    IAudioCaptureClient* captureClient = nullptr;
    WAVEFORMATEX*        format        = nullptr;

    UINT32 numFrames                   = 0;
    float* samples_;
};

#endif  // SC_API_AUDIO_AUDIO_CAPTURER_H
