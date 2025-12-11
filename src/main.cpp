#include <QApplication>
#include <QMessageBox>
#include <QProgressDialog>
#include <cmath>
#include <cstdio>
#include <future>
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
void audioCaptureThread(utils::ThreadSafeRingBuffer<float>& ring, std::atomic<bool>& stopFlag,
                        AudioProcessor& master_audio_processor) {
    AudioCapturer audio_capturer;

    Resampler       resampler;
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

        master_audio_processor.process(samples);

        // No other threads use the timestamp
        ScApiContext::getInstanceUnsafe().syncTimeStamp();

        for (const auto& device : devices) {
            device->stream(std::vector<float>(samples.begin(), samples.end()),
                           ScApiContext::getInstanceUnsafe().getTimeStamp(),
                           ScApiContext::getInstanceUnsafe().getSampleTime());
        }

        std::vector<float> left_samples        = stereo_converter.pan(samples, 0);
        std::vector<float> downsampled_samples = resampler.resample(left_samples);

        ScApiContext::getInstanceUnsafe().updateTimeStamp(downsampled_samples.size());
    }

    printf("Audio capture stopped.\n");
}

void connectToTuner() {
    // Create progress dialog with cancel button
    QProgressDialog progress("Connecting to Tuner...", "Cancel", 0, 0, nullptr);
    progress.setWindowTitle("Connecting");
    progress.setWindowModality(Qt::ApplicationModal);
    // Do not show progress dialog if it takes less than this time to connect to tuner
    progress.setMinimumDuration(1000);
    progress.show();
    QApplication::processEvents();

    std::future<bool> init_future = std::async(std::launch::async, []() {
        // You could check userCancelled periodically in initApiSession if needed
        return ScApiContext::initApiSession({.display_name   = "sc-api-audio",
                                             .type           = "Audio player",
                                             .author         = "Simucube Hackers",
                                             .version_string = "1.0"},
                                            std::chrono::seconds(60));
    });
    // Wait for completion while keeping UI responsive
    while (init_future.wait_for(std::chrono::milliseconds(50)) == std::future_status::timeout) {
        QApplication::processEvents();

        // Track if user cancelled waiting for connecting to tuner
        // Yes this is not "safe exit" because thread is left running
        if (progress.wasCanceled()) {
            std::exit(1);
        }
    }
    bool success = init_future.get();
    progress.close();

    if (!success) {
        // Show error dialog with OK button
        QMessageBox::critical(nullptr, "Connection Error",
                              "Failed to connect to Tuner.\n\nPlease make sure Simucube Tuner is running.",
                              QMessageBox::Ok);
        // WIll be called after user presses ok
        std::exit(1);
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication::setApplicationName("SC-API Audio");
    QApplication::setStyle("Fusion");
    QApplication app(argc, argv);

    connectToTuner();

    constexpr size_t RING_CAPACITY =
        48000 * 1;  // 1 sec of audio // TODO determine run time from captured device's format
    utils::ThreadSafeRingBuffer<float> ring(RING_CAPACITY);

    std::atomic<bool> stopCaptureThread = false;

    AudioProcessor master_audio_processor;
    master_audio_processor.setEqEnabled(false);

    gui::MainWindow main_window(ring, 48000, master_audio_processor);

    std::thread captureThread(audioCaptureThread, std::ref(ring), std::ref(stopCaptureThread),
                              std::ref(master_audio_processor));

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