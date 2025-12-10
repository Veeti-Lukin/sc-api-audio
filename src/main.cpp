// clang-format off
#include <Windows.h>
#include <initguid.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>
// clang-format on

// Manual GUID definitions for MSVC for some reason these work automatically on MINGW but not on MSVC
#ifdef _MSC_VER
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
#endif

#include <QApplication>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <thread>

#include "gui/MainWindow.h"
#include "utils/ThreadSafeRingBuffer.h"

#include "audio_player/ApiAudioPlayer.h"
#include "audio_player/Resampler.h"

// =====================================================
// WASAPI loopback capture thread
// =====================================================
void audioCaptureThread(utils::ThreadSafeRingBuffer<float>& ring, std::atomic<bool>& stopFlag) {
    HRESULT              hr;
    IMMDeviceEnumerator* enumerator    = nullptr;
    IMMDevice*           renderDevice  = nullptr;
    IAudioClient*        audioClient   = nullptr;
    IAudioCaptureClient* captureClient = nullptr;
    WAVEFORMATEX*        format        = nullptr;

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Get default render (output) device
    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator);
    assert(SUCCEEDED(hr));

    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &renderDevice);
    assert(SUCCEEDED(hr));

    hr = renderDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&audioClient);
    assert(SUCCEEDED(hr));

    hr = audioClient->GetMixFormat(&format);
    assert(SUCCEEDED(hr));

    hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, format, nullptr);
    assert(SUCCEEDED(hr));

    hr = audioClient->GetService(IID_IAudioCaptureClient, (void**)&captureClient);
    assert(SUCCEEDED(hr));

    hr = audioClient->Start();
    assert(SUCCEEDED(hr));

    ApiAudioPlayer audio_player;
    Resampler resampler_;

    printf("Audio capture started...\n");

    while (!stopFlag) {
        UINT32 packetLength = 0;
        hr                  = captureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr)) break;

        if (packetLength == 0) {
            Sleep(1);
            continue;
        }

        BYTE*  data;
        UINT32 numFrames;
        DWORD  flags;
        hr = captureClient->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);
        if (FAILED(hr)) break;

        float* samples     = reinterpret_cast<float*>(data);
        size_t sampleCount = numFrames * format->nChannels;

        // Push samples into ring buffer
        ring.push({samples, sampleCount});

        // Audio player
        std::vector<float> downsampled_samples = resampler_.resample({samples, sampleCount});
        if (downsampled_samples.size() > 0) {
            audio_player.stream({downsampled_samples.data(), downsampled_samples.size()}, resampler_.getSampleRate());
        }

        hr = captureClient->ReleaseBuffer(numFrames);
        if (FAILED(hr)) break;
    }

    audioClient->Stop();
    captureClient->Release();
    audioClient->Release();
    renderDevice->Release();
    enumerator->Release();
    CoUninitialize();

    printf("Audio capture stopped.\n");
}

// =====================================================
// Main: read from ring buffer and display RMS
// =====================================================
int main(int argc, char* argv[]) {
    constexpr size_t RING_CAPACITY =
        48000 * 1;  // 1 sec of audio // TODO determine run time from captured device's format
    utils::ThreadSafeRingBuffer<float> ring(RING_CAPACITY);

    std::atomic<bool> stopCaptureThread = false;

    std::thread captureThread(audioCaptureThread, std::ref(ring), std::ref(stopCaptureThread));

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("SC-API Audio");
    QApplication::setStyle("Fusion");

    gui::MainWindow main_window(ring, 48000);
    main_window.show();

    QApplication::exec();

    /*while (true) {
        float sample;
        if (ring.pop(sample)) {
            std::printf("%f\n", sample);
        }
    }*/
    stopCaptureThread = true;

    captureThread.join();
    return 0;
}