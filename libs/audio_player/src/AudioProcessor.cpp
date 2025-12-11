#include "audio_player/AudioProcessor.h"

#include <iostream>

#include "iir.h"

AudioProcessor::AudioProcessor() {
    bands_[0] = {60.0f, 1.0f, 0.707f};    // Low band
    bands_[1] = {250.0f, 1.0f, 0.707f};   // Mid band
    bands_[2] = {8000.0f, 1.0f, 0.707f};  // High band

    for (auto& band : bands_) {
        band.filter.setup(samplingrate, band.frequency, band.q);
    }

    low_pass_.setup(samplingrate, low_pass_cutoff_);
    high_pass_.setup(samplingrate, high_pass_cutoff_);
}

void AudioProcessor::process(std::span<float> samples) {
    mutex_.lock();
    equalize(samples);
    applyGain(samples);
    mutex_.unlock();
}

void AudioProcessor::setGain(float gain) {
    mutex_.lock();
    gain_ = gain;
    mutex_.unlock();
}

void AudioProcessor::applyGain(std::span<float> samples) {
    for (float& sample : samples) {
        sample *= gain_;

        // Limit to gain x2
        if (sample > 15.0f) {
            sample = 15.0f;
            std::cout << "Limiting gain" << std::endl;
        } else if (sample < -15.0f) {
            sample = -15.0f;
            std::cout << "Limiting gain" << std::endl;
        }
    }
}

void AudioProcessor::equalize(std::span<float> samples) {
    if (!eq_enabled_) return;

    // Only usable with single channel audio samples!
    for (float& sample : samples) {
        sample                = low_pass_.filter(sample);
        sample                = high_pass_.filter(sample);

        // Process through each band in parallel and sum the results
        float processedSample = 0.0f;
        for (auto& band : bands_) {
            // Apply the band-pass filter
            float filteredSample = band.filter.filter(sample);

            // Apply linear gain directly
            // Gain of 1.0 means no change, >1.0 is boost, <1.0 is cut
            // We subtract 1.0 because we'll add the original signal back later
            processedSample += filteredSample * (band.gain - 1.0f);
        }

        // Add the processed components to the original signal
        sample += processedSample;
    }
}

void AudioProcessor::setEqEnabled(bool enabled) {
    mutex_.lock();
    eq_enabled_ = enabled;
    mutex_.unlock();
}

void AudioProcessor::setEqBand(int band, float frequency, float gain, float q) {
    if (band < 0 || band >= 3) return;
    if (frequency < 20.0f || frequency > 20000.0f) return;

    mutex_.lock();
    bands_[band].frequency = frequency;
    bands_[band].gain      = gain;
    bands_[band].q         = q;

    bands_[band].filter.setup(samplingrate, bands_[band].frequency, bands_[band].q);

    mutex_.unlock();
}

void AudioProcessor::setEqLowPassCutoff(float cutoff) {
    if (cutoff < 20.0f || cutoff > 20000.0f) return;

    mutex_.lock();
    low_pass_cutoff_ = cutoff;
    low_pass_.setup(samplingrate, low_pass_cutoff_);

    mutex_.unlock();
}

void AudioProcessor::setEqHighPassCutoff(float cutoff) {
    if (cutoff < 20.0f || cutoff > 20000.0f) return;

    mutex_.lock();

    high_pass_cutoff_ = cutoff;
    high_pass_.setup(samplingrate, high_pass_cutoff_);

    mutex_.unlock();
}
