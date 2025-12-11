#include "AudioCapturer.h"

#include <cassert>

// Manual GUID definitions for MSVC for some reason these work automatically on MINGW but not on MSVC
#ifdef _MSC_VER
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID   IID_IMMDeviceEnumerator  = __uuidof(IMMDeviceEnumerator);
const IID   IID_IAudioClient         = __uuidof(IAudioClient);
const IID   IID_IAudioCaptureClient  = __uuidof(IAudioCaptureClient);
#endif

AudioCapturer::AudioCapturer() {
    HRESULT hr;

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
}

AudioCapturer::~AudioCapturer() {
    releaseSamples();

    audioClient->Stop();
    captureClient->Release();
    audioClient->Release();
    renderDevice->Release();
    enumerator->Release();
    CoUninitialize();
}

std::span<float> AudioCapturer::tryGetSamples() {
    releaseSamples();

    BYTE* data;
    DWORD flags;

    HRESULT hr;
    hr = captureClient->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);
    if (FAILED(hr)) return {};

    samples_           = reinterpret_cast<float*>(data);
    size_t sampleCount = numFrames * format->nChannels;

    return {samples_, sampleCount};
}

AudioCapturer::Format AudioCapturer::getFormat() {
    return {.sample_rate = format->nSamplesPerSec, .channels = format->nChannels};
}

void AudioCapturer::releaseSamples() {
    HRESULT hr;
    hr = captureClient->ReleaseBuffer(numFrames);
    assert(SUCCEEDED(hr));
}