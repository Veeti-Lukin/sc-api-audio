#ifndef SC_API_AUDIO_SCAPICONTEXT_H
#define SC_API_AUDIO_SCAPICONTEXT_H

#include <chrono>

#include "audio_player/OutputDevice.h"
#include "sc-api/api.h"
#include "utils/ThreadSafeSingletonBase.h"

class ScApiContext : public utils::ThreadSafeSingletonBase<ScApiContext> {
    friend class utils::ThreadSafeSingletonBase<ScApiContext>;

public:
    ~ScApiContext() override = default;

    [[nodiscard]] static bool initApiSession(const sc_api::ApiUserInformation& api_user_info,
                                             std::chrono::seconds              timeout);

    std::vector<std::shared_ptr<OutputDevice>> getConnectedDevices();

    void syncTimeStamp();
    void updateTimeStamp(size_t sample_count);

    [[nodiscard]] sc_api::Clock::time_point getTimeStamp() const { return time_stamp_; }
    [[nodiscard]] sc_api::Clock::duration   getSampleTime() const { return sample_time_; }

private:
    ScApiContext() = default;
    void init(const sc_api::ApiUserInformation& api_user_info);
    bool waitForSessionConnection(std::chrono::seconds timeout);

    sc_api::Api                                   api_;
    std::unique_ptr<sc_api::Api::EventQueue>      api_event_queue_ = nullptr;
    std::unique_ptr<sc_api::NoAuthControlEnabler> control_enabler_ = nullptr;
    std::shared_ptr<sc_api::Session>              session_         = nullptr;

    sc_api::Clock::time_point time_stamp_;
    sc_api::Clock::duration   sample_time_;

    std::atomic<bool> is_initialized_{false};
};

#endif  // SC_API_AUDIO_SCAPICONTEXT_H
