#include "audio_player/AudioProcessor.h"

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
    equalize(samples);
    applyGain(samples);
}

void AudioProcessor::applyGain(std::span<float> samples) {
    for (float& sample : samples) {
        sample *= gain_;

        // Limit to gain
        if (sample > gain_) {
            sample = gain_;
        } else if (sample < -gain_) {
            sample = -gain_;
        }
    }
}

void AudioProcessor::equalize(std::span<float> samples) {
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
