#include "audio_player/StereoConverter.h"

void StereoConverter::process(std::span<float> samples) {
    samples_.clear();

    for(int i = 0; i < samples.size(); i ++) {
        if (i % 2 == 0) {
            samples_.left.push_back(samples[i]);
            samples_.mono.push_back(samples[i]);
        }
        else {
            samples_.right.push_back(samples[i]);
            samples_.mono.back() += samples[i];
            samples_.mono.back() /= 2.0f;
        }
   }
}
