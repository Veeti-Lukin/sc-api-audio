#include "audio_player/OutputDevice.h"

OutputDevice::OutputDevice(sc_api::core::device_info::DeviceInfoPtr device_info,
                           std::unique_ptr<sc_api::FfbPipeline>     pipeline_handle)
    : device_info_(device_info), pipeline_handle_(std::move(pipeline_handle)) {}

void OutputDevice::stream(std::span<float> samples, sc_api::Clock::time_point time_stamp,
                          sc_api::Clock::duration sample_time) const {
    constexpr size_t K_MAX_CHUNK_SIZE = 256;
    size_t           remaining        = samples.size();
    size_t           offset           = 0;

    while (remaining > 0) {
        size_t chunk_size = std::min(remaining, K_MAX_CHUNK_SIZE);

        pipeline_handle_->generateEffect(time_stamp, sample_time, samples.data() + offset,
                                         static_cast<uint32_t>(chunk_size));

        // Update timestamp for the processed chunk
        time_stamp += sample_time * chunk_size;

        // Update remaining samples and offset
        remaining -= chunk_size;
        offset += chunk_size;
    }
}
