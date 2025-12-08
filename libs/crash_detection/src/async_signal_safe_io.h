#ifndef SC_API_AUDIO_ASYNC_SIGNAL_SAFE_IO_H
#define SC_API_AUDIO_ASYNC_SIGNAL_SAFE_IO_H

#include <cstdint>

namespace crash_detection::async_signal_safe_io {

struct DateTime {
    size_t day    = 0;
    size_t month  = 0;
    size_t year   = 0;

    size_t hour   = 0;
    size_t minute = 0;
    size_t second = 0;
};

DateTime timeNow();

}  // namespace crash_detection::async_signal_safe_io

#endif  // SC_API_AUDIO_ASYNC_SIGNAL_SAFE_IO_H
