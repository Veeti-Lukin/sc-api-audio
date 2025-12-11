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

#include "AudioCapturer.h"
#include "ScApiContext.h"
#include "audio_player/AudioProcessor.h"
#include "audio_player/Resampler.h"
#include "audio_player/Samples.h"
#include "audio_player/StereoConverter.h"
#include "gui/MainWindow.h"
#include "utils/ThreadSafeRingBuffer.h"

// =====================================================
// WASAPI loopback capture thread
// =====================================================
void audioCaptureThread(utils::ThreadSafeRingBuffer<float>& ring, std::atomic<bool>& stopFlag) {
    AudioCapturer audio_capturer;

    Resampler       resampler;
    AudioProcessor  audio_processor;
    StereoConverter stereo_converter;

    printf("Audio capture started...\n");

    while (!stopFlag) {
        std::span<float> samples = audio_capturer.tryGetSamples();
        if (samples.empty()) {
            Sleep(1);
            continue;
        }

        // Push samples into ring buffer
        ring.push(samples);

        // Audio player
        std::vector<std::shared_ptr<OutputDevice>> devices = ScApiContext::getInstance()->getConnectedDevices();

        // audio_processor.process({samples, sampleCount});

        // No other threads use the timestamp
        ScApiContext::getInstanceUnsafe().syncTimeStamp();

        for (const auto& device : devices) {
            device->stream(std::vector<float>(samples.begin(), samples.end()),
                           ScApiContext::getInstanceUnsafe().getTimeStamp(),
                           ScApiContext::getInstanceUnsafe().getSampleTime());
        }

        std::vector<float> downsampled_samples = resampler.resample(samples);
        std::vector<float> left_samples        = stereo_converter.pan(downsampled_samples, 0);
        ScApiContext::getInstanceUnsafe().updateTimeStamp(left_samples.size());
    }

    printf("Audio capture stopped.\n");
}

// =====================================================
// Main: read from ring buffer and display RMS
// =====================================================
int main(int argc, char* argv[]) {
    bool init_success = ScApiContext::initApiSession({.display_name   = "sc-api-audio??",
                                                      .type           = "Audio player",
                                                      .author         = "Simucube Hackers",
                                                      .version_string = "1.0"},
                                                     std::chrono::seconds(60));

    constexpr size_t RING_CAPACITY =
        48000 * 1;  // 1 sec of audio // TODO determine run time from captured device's format
    utils::ThreadSafeRingBuffer<float> ring(RING_CAPACITY);

    std::atomic<bool> stopCaptureThread = false;

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("SC-API Audio");
    QApplication::setStyle("Fusion");

    gui::MainWindow main_window(ring, 48000);

    std::thread captureThread(audioCaptureThread, std::ref(ring), std::ref(stopCaptureThread));

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