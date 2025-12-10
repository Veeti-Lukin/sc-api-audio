#ifndef APIAUDIOPLAYER_H
#define APIAUDIOPLAYER_H

#include <span>

#include "sc-api/device_info.h"
#include "sc-api/ffb.h"
#include "sc-api/time.h"

class OutputDevice {
public:
    OutputDevice(sc_api::core::device_info::DeviceInfoPtr device_info,
                 std::unique_ptr<sc_api::FfbPipeline>     pipeline_handle);

    void stream(std::span<float> samples, sc_api::Clock::time_point time_stamp,
                sc_api::Clock::duration sample_time) const;

    sc_api::core::device_info::DeviceInfoPtr getDeviceInfo() const { return device_info_; }

private:
    sc_api::core::device_info::DeviceInfoPtr   device_info_;
    std::unique_ptr<sc_api::core::FfbPipeline> pipeline_handle_;
};

#endif  // APIAUDIOPLAYER_H
