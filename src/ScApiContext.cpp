#include "ScApiContext.h"

#include <chrono>

#include "sc-api/device_info.h"
#include "sc-api/events.h"
#include "sc-api/ffb.h"

ScApiContext::ScApiContext(const sc_api::ApiUserInformation& api_user_info)
    : control_enabler_(&api_,
                       sc_api::Session::ControlFlag::control_sim_data |
                           sc_api::Session::ControlFlag::control_ffb_effects |
                           sc_api::Session::ControlFlag::control_telemetry,
                       api_user_info.display_name, api_user_info) {
    api_event_queue_ = api_.createEventQueue();
    time_stamp_      = sc_api::core::Clock::now();

    auto start_time  = std::chrono::system_clock::now();
    auto timeout     = start_time + std::chrono::minutes(1);
    while (true) {
        // Wait for session to connect and control to be available
        // So basically wait to be connected to tuner
        if (std::chrono::system_clock::now() > timeout) {
            throw std::runtime_error("Timed out waiting for session to connect");
        }

        auto event = api_event_queue_->tryPop();
        if (!event.has_value()) continue;
        if (const auto* ev = std::get_if<sc_api::event::SessionStateChanged>(&*event)) {
            if (ev->state == sc_api::SessionState::connected_control) {
                // TODO  add led data too
                if (ev->session && (ev->control_flags & sc_api::Session::control_ffb_effects) != 0u) {
                    session_ = ev->session;
                    break;
                }
            }
        }
    }
}

std::vector<OutputDevice*> ScApiContext::getConnectedDevices() {
    sc_api::core::device_info::FullInfoPtr full_device_info = session_->getDeviceInfo();

    for (const sc_api::device_info::DeviceInfo& device_info : *full_device_info) {
        bool new_device = true;
        for (auto dev : devices_) {
            if (dev->getDeviceInfo()->getUid() == device_info.getUid()) {
                new_device = false;
                break;
            }
        }

        if (!new_device) continue;

        auto createPipeline = [&](sc_api::OffsetType pipeline_offset_type) {
            std::unique_ptr<sc_api::FfbPipeline> pipeline;
            sc_api::PipelineConfig               config;
            config.offset_type = pipeline_offset_type;
            pipeline           = std::make_unique<sc_api::FfbPipeline>(session_, device_info.getSessionId());
            pipeline->configure(config);
            return pipeline;
        };

        // TODO music pipeline type here

        if (device_info.hasFeedbackType(sc_api::device_info::FeedbackType::active_pedal)) {
            auto* device = new OutputDevice(device_info.shared_from_this(),
                                            std::move(createPipeline(sc_api::OffsetType::force_N)));
            devices_.push_back(device);
        } else if (device_info.hasFeedbackType(sc_api::device_info::FeedbackType::wheelbase)) {
            auto* device = new OutputDevice(device_info.shared_from_this(),
                                            std::move(createPipeline(sc_api::OffsetType::torque_Nm)));
            devices_.push_back(device);
        }
    }

    return devices_;
}

void ScApiContext::syncTimeStamp() {
    if (sc_api::core::Clock::now() > time_stamp_ + std::chrono::microseconds(100000)) {
        time_stamp_ = sc_api::core::Clock::now() + std::chrono::microseconds(10000);
    }

    sample_time_ = std::chrono::microseconds(1000000) / 20000;  // TODO samplerate

    if (time_stamp_ < sc_api::core::Clock::now() + std::chrono::microseconds(10000)) {
        sample_time_ += std::chrono::microseconds(20);
    } else if (sc_api::core::Clock::now() + std::chrono::microseconds(10000) > time_stamp_) {
        sample_time_ -= std::chrono::microseconds(20);
    }
}

void ScApiContext::updateTimeStamp(size_t sample_count) { time_stamp_ += sample_time_ * sample_count; }