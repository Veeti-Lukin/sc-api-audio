#ifndef SC_API_AUDIO_SCAPICONTEXT_H
#define SC_API_AUDIO_SCAPICONTEXT_H

#include "audio_player/OutputDevice.h"
#include "sc-api/api.h"

class ScApiContext {
public:
    ScApiContext(const sc_api::ApiUserInformation& api_user_info);
    ~ScApiContext() = default;

    std::vector<OutputDevice*> getConnectedDevices();

    void syncTimeStamp();
    void updateTimeStamp(size_t sample_count);

    sc_api::Clock::time_point getTimeStamp() const { return time_stamp_; }
    sc_api::Clock::duration   getSampleTime() const { return sample_time_; }

private:
    sc_api::Api                              api_;
    std::unique_ptr<sc_api::Api::EventQueue> api_event_queue_;
    sc_api::NoAuthControlEnabler             control_enabler_;
    std::shared_ptr<sc_api::Session>         session_ = nullptr;

    sc_api::Clock::time_point time_stamp_;
    sc_api::Clock::duration   sample_time_;

    std::vector<OutputDevice*> devices_;
};

#endif  // SC_API_AUDIO_SCAPICONTEXT_H
