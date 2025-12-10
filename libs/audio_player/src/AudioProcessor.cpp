#include "audio_player/AudioProcessor.h"

void AudioProcessor::process(std::span<float> samples) {
    applyGain(samples);
}
void AudioProcessor::applyGain(std::span<float> samples) {
    for (float& sample : samples) {
        sample *= gain_;
    }
}

