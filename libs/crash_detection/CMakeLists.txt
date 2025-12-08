add_library(crash_detection STATIC
        inc/crash_detection/crash_detection.h
        src/crash_detection.cpp

        src/async_signal_safe_io.h
        src/async_signal_safe_io.cpp
)
set_target_properties(crash_detection PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(crash_detection PUBLIC inc)

if (CMAKE_CXX_STANDARD VERSION_LESS 23)
    message(WARNING "lib crash_detection: C++23 required for stacktrace support -- disabling backtrace")
    target_compile_definitions(crash_detection PUBLIC LIB_CRASH_DETECTION_NO_BACKTRACE_SUPPORT)
    return()
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR MINGW)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
        message(FATAL_ERROR
                "lib crash_detection requires GCC 14+ for std::stacktrace "
                "(you have ${CMAKE_CXX_COMPILER_VERSION})"
        )
    else ()
        # TODO Will probably be supported fully without linking to experimental features in the future
        target_link_libraries(crash_detection PRIVATE stdc++exp)
    endif ()

elseif (MSVC)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.34")
        message(FATAL_ERROR
                "lib crash_detection requires MSVC 19.34+ for std::stacktrace "
                "(you have ${CMAKE_CXX_COMPILER_VERSION})"
        )
    endif ()
else ()
    message(FATAL_ERROR "Possibly unsupported compiler for crash_detection: ${CMAKE_CXX_COMPILER_ID}")
endif ()

