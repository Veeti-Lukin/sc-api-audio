add_library(utils INTERFACE
        inc/utils/ThreadSafeRingBuffer.h
)
#set_target_properties(utils PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(utils INTERFACE inc)