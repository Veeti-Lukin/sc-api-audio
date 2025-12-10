#ifndef AUDIO_PLAYER_STEREO_CONVERTER_H
#define AUDIO_PLAYER_STEREO_CONVERTER_H

#include <span>
#include <vector>

#include "Samples.h"

class StereoConverter {
public:
    void process(std::span<float> samples);

    std::vector<float> left() { return samples_.left; }
    std::vector<float> right() { return samples_.right; }
    std::vector<float> mono() { return samples_.mono; }

    //std::vector<float> pan(float pan);

private:
    Samples samples_;
};

#endif  // AUDIO_PLAYER_STEREO_CONVERTER_H
