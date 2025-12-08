// clang-format off
#include <Windows.h>
#include <initguid.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>
// clang-format on

#include <QApplication>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <thread>

#include "crash_detection/crash_detection.h"
#include "gui/MainWindow.h"
#include "utils/ThreadSafeRingBuffer.h"

// =====================================================
// WASAPI loopback capture thread
// =====================================================
void audioCaptureThread(utils::ThreadSafeRingBuffer<float>& ring) {
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

    printf("Audio capture started...\n");

    while (true) {
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

void crashingFunction() {
    int* i = nullptr;
    *i     = 1;
    /*
    volatile int i = 0;
    volatile int j = 1;

    volatile int k = j / i;
    std::printf("%d\n", k);*/
}
void deeperTestFunction() { crashingFunction(); }
void testFunction() { deeperTestFunction(); }
// =====================================================
// Main: read from ring buffer and display RMS
// =====================================================
int main(int argc, char* argv[]) {
    installCrashDetector();
    testFunction();

    constexpr size_t RING_CAPACITY =
        48000 * 1;  // 1 sec of audio // TODO determine run time from captured device's format
    utils::ThreadSafeRingBuffer<float> ring(RING_CAPACITY);

    std::thread captureThread(audioCaptureThread, std::ref(ring));

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("ServoCore DevTool");
    QApplication::setStyle("Fusion");

    gui::MainWindow main_window(ring, 48000);
    main_window.show();

    return QApplication::exec();

    while (true) {
        float sample;
        if (ring.pop(sample)) {
            std::printf("%f\n", sample);
        }
    }
    captureThread.join();
    return 0;
}