#include "audio_player/StereoConverter.h"

std::vector<float> StereoConverter::pan(std::span<float> samples, float pan) {
    std::vector<float> out;
    for (int i = 0; i < samples.size(); i++) {
        float sample = samples[i];
        if (i % 2 == 0) {
            float panned_sample = sample * (1.0f - pan);
            out.push_back(panned_sample);
        } else {
            float panned_sample = sample * pan;
            out.back() += panned_sample;
        }
    }

    return out;
}
