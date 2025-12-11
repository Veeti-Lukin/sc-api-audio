#include "audio_player/OutputDevice.h"

#include <algorithm>

OutputDevice::OutputDevice(sc_api::core::device_info::DeviceInfoPtr device_info,
                           std::unique_ptr<sc_api::FfbPipeline>     pipeline_handle)
    : device_info_(std::move(device_info)), pipeline_handle_(std::move(pipeline_handle)) {}

void OutputDevice::stream(std::vector<float> samples, sc_api::Clock::time_point time_stamp,
                          sc_api::Clock::duration sample_time) {
    mutex_.lock();

    std::vector<float> panned_samples = stereo_converter.pan(samples, pan_);
    audio_processor.process({panned_samples.data(), panned_samples.size()});
    std::vector<float> downsampled_samples = resampler.resample(panned_samples);

    mutex_.unlock();

    size_t remaining = downsampled_samples.size();
    size_t offset    = 0;
    while (remaining > 0) {
        // Calculate the size of the current chunk (max 256)
        size_t chunk_size = (std::min)(remaining, static_cast<size_t>(256));
        // Stream the current chunk
        pipeline_handle_->generateEffect(time_stamp, sample_time, downsampled_samples.data() + offset, chunk_size);

        // Update remaining samples and offset for the next chunk
        remaining -= chunk_size;
        offset += chunk_size;
    }
}

void OutputDevice::setPan(float pan) {
    mutex_.lock();
    pan_ = pan;
    mutex_.unlock();
}
