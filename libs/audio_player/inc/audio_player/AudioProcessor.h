#ifndef AUDIO_PLAYER_AUDIO_PROCESSOR_H
#define AUDIO_PLAYER_AUDIO_PROCESSOR_H

#include <span>
#include "Samples.h"

class AudioProcessor {
public:
    void process(std::span<float> samples);

private:
    void applyGain(std::span<float> samples);

    float gain_ = 5.0f;
};

#endif  // AUDIO_PLAYER_AUDIO_PROCESSOR_H
