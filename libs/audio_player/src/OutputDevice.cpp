#include "audio_player/OutputDevice.h"

OutputDevice::OutputDevice(sc_api::core::device_info::DeviceInfoPtr device_info,
                           std::unique_ptr<sc_api::FfbPipeline>     pipeline_handle)
    : device_info_(std::move(device_info)), pipeline_handle_(std::move(pipeline_handle)) {}

void OutputDevice::stream(std::span<float> samples, sc_api::Clock::time_point time_stamp,
                          sc_api::Clock::duration sample_time) {
    if (sc_api::core::Clock::now() > time_stamp + std::chrono::microseconds(100000)) {
        time_stamp = sc_api::core::Clock::now() + std::chrono::microseconds(10000);
    }

    if (time_stamp < sc_api::core::Clock::now() + std::chrono::microseconds(10000)) {
        sample_time += std::chrono::microseconds(20);
    } else if (sc_api::core::Clock::now() + std::chrono::microseconds(10000) > time_stamp) {
        sample_time -= std::chrono::microseconds(20);
    }

    pipeline_handle_->generateEffect(time_stamp, sample_time, samples.data(), samples.size());
}
