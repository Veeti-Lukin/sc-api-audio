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

    EQBand bands_[3];

    float low_pass_cutoff_  = 20000;
    float high_pass_cutoff_ = 20;

    Iir::Butterworth::LowPass<4>  low_pass_;
    Iir::Butterworth::HighPass<4> high_pass_;

    const float samplingrate = 48000;  // Hz
};

#endif  // AUDIO_PLAYER_AUDIO_PROCESSOR_H
