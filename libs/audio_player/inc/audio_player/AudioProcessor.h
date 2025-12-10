#ifndef AUDIO_PLAYER_AUDIO_PROCESSOR_H
#define AUDIO_PLAYER_AUDIO_PROCESSOR_H

#include <span>

#include "Samples.h"
#include "iir.h"

class AudioProcessor {
public:
    AudioProcessor();

    void process(std::span<float> samples);

private:
    void applyGain(std::span<float> samples);
    void equalize(std::span<float> samples);

    float gain_ = 5.0f;

    struct EQBand {
        float               frequency;
        float               gain;
        float               q;
        Iir::RBJ::BandPass2 filter;
    };

    const float samplingrate = 48000;  // Hz

    EQBand bands_[3];
};

#endif  // AUDIO_PLAYER_AUDIO_PROCESSOR_H
