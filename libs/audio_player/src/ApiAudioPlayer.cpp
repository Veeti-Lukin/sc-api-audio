#include "audio_player/ApiAudioPlayer.h"

#include "sc-api/core/device_info.h"
#include "sc-api/core/events.h"
#include "sc-api/core/time.h"

ApiAudioPlayer::ApiAudioPlayer() {
    user_info_.author         = "Simucube";
    user_info_.display_name   = "SC Music Player";
    user_info_.type           = "tool";
    user_info_.version_string = "0.1";

    time_stamp_               = sc_api::core::Clock::now();
    api_event_queue_          = api_.createEventQueue();

    control_enabler_          = std::make_unique<sc_api::core::NoAuthControlEnabler>(
        &api_,
        sc_api::core::Session::ControlFlag::control_sim_data | sc_api::core::Session::ControlFlag::control_ffb_effects |
            sc_api::core::Session::ControlFlag::control_telemetry,
        "sc-music_player", user_info_);

}

static int i = 0;

void ApiAudioPlayer::stream(std::span<float> samples, uint32_t sample_rate) {
    i++;
    if (i % 1000) {
        checkEvents();
    }

    if (sc_api::core::Clock::now() > time_stamp_ + std::chrono::microseconds(100000) ) {
        time_stamp_ = sc_api::core::Clock::now() + std::chrono::microseconds(10000);
    }
    sc_api::core::Clock::duration sample_time = std::chrono::microseconds(1000000) / sample_rate;

    if (time_stamp_ < sc_api::core::Clock::now() + std::chrono::microseconds(10000)) {
        sample_time += std::chrono::microseconds(20);
    } else if (sc_api::core::Clock::now() + std::chrono::microseconds(10000) > time_stamp_) {
        sample_time -= std::chrono::microseconds(20);
    }

    const size_t MAX_CHUNK_SIZE = 256;
    size_t remaining = samples.size();
    size_t offset = 0;

    while (remaining > 0) {
        size_t chunk_size = std::min(remaining, MAX_CHUNK_SIZE);

        for (auto handle : pipeline_handles_) {
            handle->generateEffect(time_stamp_, sample_time, samples.data() + offset,
                                  static_cast<uint32_t>(chunk_size));
        }

        // Update timestamp for the processed chunk
        time_stamp_ += sample_time * chunk_size;

        // Update remaining samples and offset
        remaining -= chunk_size;
        offset += chunk_size;
    }

}

void ApiAudioPlayer::checkEvents() {
    while (auto event = api_event_queue_->tryPop()) {
        // Wait for session to connect and control to be available
        if (const auto* ev = std::get_if<sc_api::core::session_event::SessionStateChanged>(&*event)) {
            if (ev->state == sc_api::core::SessionState::connected_control ||
                ev->state == sc_api::core::SessionState::connected_monitor) {
                if (ev->session && (ev->control_flags & sc_api::core::Session::control_ffb_effects) != 0u) {
                    session_ = ev->session;
                    configurePipelines();
                }
            }
        }
    }
}

void ApiAudioPlayer::configurePipelines() {
    if (pipelines_created_) return;
    if (!session_) return;
    auto device_info = session_->getDeviceInfo();

    for (const sc_api::core::device_info::DeviceInfo& device : *device_info) {
        std::string uid         = std::string(device.getUid());

        auto createPipeline = [&](sc_api::core::OffsetType pipeline_offset_type) {
            std::unique_ptr<sc_api::core::FfbPipeline> pipelineT;

            sc_api::core::PipelineConfig configT;
            configT.offset_type = pipeline_offset_type;
            pipelineT           = std::make_unique<sc_api::core::FfbPipeline>(session_, device.getSessionId());
            pipelineT->configure(configT);

            pipeline_handles_.push_back(std::move(pipelineT));
        };

        if (uid.starts_with("sc3"))
            createPipeline(sc_api::core::OffsetType::torque_Nm);
        else if (uid.starts_with("scap"))
            createPipeline(sc_api::core::OffsetType::force_N);
    }

    pipelines_created_ = true;
}
