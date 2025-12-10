#include "audio_player/Resampler.h"

#include <audio_player/Samples.h>

Resampler::Resampler() {

}

std::vector<float> Resampler::resample(std::span<float> samples) {
    std::vector<double> samples_double;
    for (float sample : samples) {
            samples_double.push_back((double)sample);
    }

    double* downsampled_buffer = nullptr;
    size_t  downsampled_len =
        resampler_.process(samples_double.data(), static_cast<int>(samples_double.size()), downsampled_buffer);

    std::vector<float> samples_out;
    for (int i = 0; i < downsampled_len; i++) {
        samples_out.push_back((float)downsampled_buffer[i]);
    }

    return samples_out;
}
