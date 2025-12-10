#ifndef AUDIO_PLAYER_RESAMPLER_H
#define AUDIO_PLAYER_RESAMPLER_H

#include <span>
#include <vector>

#include "CDSPResampler.h"

class Resampler {
public:
    Resampler();

    std::vector<float> resample(std::span<float> samples);

    int getSampleRate() { return k_output_sample_rate; }

private:
    const int k_input_sample_rate  = 48000;
    const int k_output_sample_rate = 20000;
    const int k_input_buffer_size  = 500;

    r8b::CDSPResampler24 resampler_ =
        r8b::CDSPResampler24(k_input_sample_rate, k_output_sample_rate, k_input_buffer_size);
};

#endif  // AUDIO_PLAYER_RESAMPLER_H
