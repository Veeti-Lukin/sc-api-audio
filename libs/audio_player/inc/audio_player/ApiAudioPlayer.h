#ifndef APIAUDIOPLAYER_H
#define APIAUDIOPLAYER_H

#include <span>

#include "sc-api/api.h"
#include "sc-api/ffb.h"
#include "sc-api/sim_data.h"

class ApiAudioPlayer {
public:
    ApiAudioPlayer();

    void stream(std::span<float> samples, uint32_t sample_rate);

private:
    void checkEvents();
    void configurePipelines();

    sc_api::Api                                   api_;
    sc_api::ApiUserInformation                    user_info_;
    std::unique_ptr<sc_api::core::NoAuthControlEnabler> control_enabler_;
    std::unique_ptr<sc_api::core::Api::EventQueue>      api_event_queue_;

    std::shared_ptr<sc_api::core::Session> session_ = nullptr;

    std::vector<sc_api::core::device_info::DeviceInfoPtr>   output_devices_;
    std::vector<std::shared_ptr<sc_api::core::FfbPipeline>> pipeline_handles_;

    sc_api::core::Clock::time_point time_stamp_;

    bool pipelines_created_ = false;
};

#endif  // APIAUDIOPLAYER_H
