#ifndef AUDIO_PLAYER_STEREO_CONVERTER_H
#define AUDIO_PLAYER_STEREO_CONVERTER_H

#include <span>
#include <vector>

class StereoConverter {
public:
    // pan: 0 = full left, 1.0 full right
    static std::vector<float> pan(std::span<float> samples, float pan);
};

#endif  // AUDIO_PLAYER_STEREO_CONVERTER_H
