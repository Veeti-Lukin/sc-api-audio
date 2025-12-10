#include "audio_player/Resampler.h"


Resampler::Resampler() {

}

std::vector<float> Resampler::resample(std::span<float> samples) {
    std::vector<double> samples_double;
    for (int i = 0; i < samples.size(); i+=2) {
        samples_double.push_back((double)samples[i]);
    }

    double* downsampled_buffer = nullptr;
    size_t  downsampled_len =
        resampler_.process(samples_double.data(), static_cast<int>(samples_double.size()), downsampled_buffer);

    std::vector<float> downsampled_samples;
    for (int i = 0; i < downsampled_len; i++) {
        downsampled_samples.push_back((float)downsampled_buffer[i] * 4.0f);
    }

    return downsampled_samples;
}
