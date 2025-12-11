#ifndef APIAUDIOPLAYER_H
#define APIAUDIOPLAYER_H

#include "audio_player/AudioProcessor.h"
#include "audio_player/Resampler.h"
#include "audio_player/StereoConverter.h"
#include "sc-api/device_info.h"
#include "sc-api/ffb.h"
#include "sc-api/time.h"

class OutputDevice {
public:
    OutputDevice(sc_api::core::device_info::DeviceInfoPtr device_info,
                 std::unique_ptr<sc_api::FfbPipeline>     pipeline_handle);

    void stream(std::vector<float> samples, sc_api::Clock::time_point time_stamp, sc_api::Clock::duration sample_time);

    sc_api::core::device_info::DeviceInfoPtr getDeviceInfo() const { return device_info_; }

    AudioProcessor* getAudioProcessor() { return &audio_processor; }
    void            setPan(float pan);

private:
    sc_api::core::device_info::DeviceInfoPtr device_info_;
    std::shared_ptr<sc_api::FfbPipeline>     pipeline_handle_;

    Resampler       resampler;
    AudioProcessor  audio_processor;
    StereoConverter stereo_converter;

    std::mutex mutex_;

    float pan_ = 0.5f;
};

#endif  // APIAUDIOPLAYER_H
