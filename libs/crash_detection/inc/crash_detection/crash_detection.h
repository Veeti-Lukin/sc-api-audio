#ifndef LIB_CRASH_DETECTION_CRASH_DETECTION_H
#define LIB_CRASH_DETECTION_CRASH_DETECTION_H

#include <string_view>

namespace crash_detection {

void installCrashDetector(const std::string_view file_name);

}

#endif  // LIB_CRASH_DETECTION_CRASH_DETECTION_H
