// clang-format off
#include <Windows.h>
#include <initguid.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>
// clang-format on

#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    HRESULT              hr;
    IMMDeviceEnumerator* enumerator    = nullptr;
    IMMDevice*           renderDevice  = nullptr;
    IAudioClient*        audioClient   = nullptr;
    IAudioCaptureClient* captureClient = nullptr;
    WAVEFORMATEX*        format        = nullptr;

    hr                                 = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    assert(SUCCEEDED(hr));

    // Get default render (speaker/headphone) device
    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator);
    assert(SUCCEEDED(hr));

    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &renderDevice);
    assert(SUCCEEDED(hr));

    // Activate the device
    hr = renderDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, (void**)&audioClient);
    assert(SUCCEEDED(hr));

    // Get the mix format (usually 48kHz float stereo)
    hr = audioClient->GetMixFormat(&format);
    assert(SUCCEEDED(hr));

    printf("Render mix format:\n");
    printf("  Channels       : %d\n", format->nChannels);
    printf("  Bits per sample: %d\n", format->wBitsPerSample);
    printf("  Sample rate    : %d\n\n", format->nSamplesPerSec);

    // Initialize loopback capture
    hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                 AUDCLNT_STREAMFLAGS_LOOPBACK,  // <-- loopback capture
                                 0, 0, format, nullptr);
    if (FAILED(hr)) {
        printf("Initialize failed: 0x%08X\n", hr);
        return -1;
    }

    // Get the capture service
    hr = audioClient->GetService(IID_IAudioCaptureClient, (void**)&captureClient);
    assert(SUCCEEDED(hr));

    // Start capturing
    hr = audioClient->Start();
    assert(SUCCEEDED(hr));

    printf("Capturing system audio (loopback)... Press Ctrl+C to stop.\n");

    while (true) {
        UINT32 packetLength = 0;
        hr                  = captureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr)) break;

        if (packetLength == 0) {
            Sleep(10);
            continue;
        }

        BYTE*  data;
        UINT32 numFrames;
        DWORD  flags;
        hr = captureClient->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);
        if (FAILED(hr)) break;

        float* samples     = reinterpret_cast<float*>(data);
        size_t sampleCount = numFrames * format->nChannels;

        // Compute volume (RMS)
        double sum         = 0.0;
        for (size_t i = 0; i < sampleCount; ++i) sum += samples[i] * samples[i];
        double rms   = sqrt(sum / sampleCount);

        // Draw a simple bar graph
        int barWidth = static_cast<int>(rms * 60);
        if (barWidth > 60) barWidth = 60;
        printf("\r[");
        for (int i = 0; i < 60; ++i) putchar(i < barWidth ? '#' : ' ');
        printf("] %.3f", rms);
        fflush(stdout);

        hr = captureClient->ReleaseBuffer(numFrames);
        if (FAILED(hr)) break;
    }

    // Cleanup
    audioClient->Stop();
    captureClient->Release();
    audioClient->Release();
    renderDevice->Release();
    enumerator->Release();
    CoUninitialize();

    return 0;
}