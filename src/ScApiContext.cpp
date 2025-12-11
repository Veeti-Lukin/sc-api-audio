#include "ScApiContext.h"

#include <cassert>
#include <chrono>

#include "sc-api/device_info.h"
#include "sc-api/events.h"
#include "sc-api/ffb.h"

bool ScApiContext::initApiSession(const sc_api::ApiUserInformation& api_user_info, std::chrono::seconds timeout) {
    getInstance()->init(api_user_info);
    return getInstanceUnsafe().waitForSessionConnection(timeout);
}

std::vector<std::shared_ptr<OutputDevice>> ScApiContext::getConnectedDevices() {
    assert(is_initialized_);

    static std::vector<std::shared_ptr<OutputDevice>> configured_devices;

    sc_api::core::device_info::FullInfoPtr full_device_info = session_->getDeviceInfo();

    //  Remove devices that are no longer connected
    configured_devices.erase(
        std::remove_if(configured_devices.begin(), configured_devices.end(),
                       [&full_device_info](const std::shared_ptr<OutputDevice>& dev) {
                           // Check if this device still exists in the current device list
                           for (const sc_api::device_info::DeviceInfo& device_info : *full_device_info) {
                               if (dev->getDeviceInfo()->getUid() == device_info.getUid()) {
                                   return false;  // Device still exists, keep it
                               }
                           }
                           return true;  // Device not found, remove it
                       }),
        configured_devices.end());

    // Add newly connected devices
    for (const sc_api::device_info::DeviceInfo& device_info : *full_device_info) {
        // Check if device already configured
        bool already_configured = false;
        for (const auto& dev : configured_devices) {
            if (dev->getDeviceInfo()->getUid() == device_info.getUid()) {
                already_configured = true;
                break;
            }
        }

        if (already_configured) {
            continue;  // Skip already configured devices
        }

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
            configured_devices.emplace_back(std::make_shared<OutputDevice>(
                device_info.shared_from_this(), std::move(createPipeline(sc_api::OffsetType::force_N))));
        } else if (device_info.hasFeedbackType(sc_api::device_info::FeedbackType::wheelbase)) {
            configured_devices.emplace_back(std::make_shared<OutputDevice>(
                device_info.shared_from_this(), std::move(createPipeline(sc_api::OffsetType::torque_Nm))));
        }
    }

    return configured_devices;
}

void ScApiContext::syncTimeStamp() {
    assert(is_initialized_);

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

void ScApiContext::init(const sc_api::ApiUserInformation& api_user_info) {
    // No double initialization
    assert(!is_initialized_);

    control_enabler_ = std::make_unique<sc_api::NoAuthControlEnabler>(
        &api_,
        sc_api::Session::ControlFlag::control_sim_data | sc_api::Session::ControlFlag::control_ffb_effects |
            sc_api::Session::ControlFlag::control_telemetry,
        api_user_info.display_name, api_user_info);
    api_event_queue_ = api_.createEventQueue();
    time_stamp_      = sc_api::core::Clock::now();
    is_initialized_  = true;
}

bool ScApiContext::waitForSessionConnection(std::chrono::seconds timeout) {
    assert(is_initialized_);

    auto start_time    = std::chrono::system_clock::now();
    auto timeout_point = start_time + timeout;

    while (true) {
        // Wait for session to connect and control to be available
        // So basically wait to be connected to tuner
        if (std::chrono::system_clock::now() > timeout_point) {
            return false;
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

    return true;
}