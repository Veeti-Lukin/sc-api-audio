#ifndef AUDIO_PLAYER_AUDIO_PROCESSOR_H
#define AUDIO_PLAYER_AUDIO_PROCESSOR_H

#include <mutex>
#include <span>

#include "Samples.h"
#include "iir.h"

class AudioProcessor {
public:
    AudioProcessor();

    void process(std::span<float> samples);

    void setGain(float gain);

    void setEqEnabled(bool enabled);

    void setEqBand(int band, float frequency, float gain, float q);
    void setEqLowPassCutoff(float cutoff);
    void setEqHighPassCutoff(float cutoff);

private:
    void applyGain(std::span<float> samples);
    void equalize(std::span<float> samples);

    float gain_      = 2.0f;

    bool eq_enabled_ = true;

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

    std::mutex mutex_;
};

#endif  // AUDIO_PLAYER_AUDIO_PROCESSOR_H
