#include "audio_player/StereoConverter.h"

std::vector<float> StereoConverter::pan(std::span<float> samples, float pan) {
    std::vector<float> out;
    for (int i = 0; i < samples.size(); i++) {
        if (i % 2 == 0) {
            out.push_back(samples[i] * (1.0f - pan));
        } else {
            out.back() += samples[i] * pan;
        }
    }

    return out;
}
