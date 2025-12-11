#ifndef AUDIO_PLAYER_STEREO_CONVERTER_H
#define AUDIO_PLAYER_STEREO_CONVERTER_H

#include <span>
#include <vector>

class StereoConverter {
public:
    std::vector<float> left(std::span<float> samples);
    std::vector<float> right(std::span<float> samples);
    std::vector<float> mono(std::span<float> samples);

    // pan: 0 = full left, 1.0 full right
    std::vector<float> pan(std::span<float> samples, float pan);
};

#endif  // AUDIO_PLAYER_STEREO_CONVERTER_H
