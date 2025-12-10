#ifndef AUDIO_PLAYER_SAMPLES_H
#define AUDIO_PLAYER_SAMPLES_H

#include <vector>

struct Samples {
    std::vector<float> mono;
    std::vector<float> left;
    std::vector<float> right;

    void clear() { mono.clear(); left.clear(); right.clear(); }
};

#endif