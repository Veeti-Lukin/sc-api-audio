#include "audio_player/StereoConverter.h"

std::vector<float> StereoConverter::left(std::span<float> samples) {
    std::vector<float> out;
    for (int i = 0; i < samples.size(); i++) {
        if (i % 2 == 0) {
            out.push_back(samples[i]);
        }
    }

    return out;
}

std::vector<float> StereoConverter::right(std::span<float> samples) {
    std::vector<float> out;
    for (int i = 0; i < samples.size(); i++) {
        if (i % 2 != 0) {
            out.push_back(samples[i]);
        }
    }
}

std::vector<float> StereoConverter::mono(std::span<float> samples) {
    std::vector<float> out;
    for (int i = 0; i < samples.size(); i++) {
        if (i % 2 == 0) {
            out.push_back(samples[i]);
        } else {
            out.back() += samples[i];
            out.back() /= 2.0f;
        }
    }

    return out;
}

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
