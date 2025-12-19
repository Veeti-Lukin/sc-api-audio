# Include FetchContent module
include(FetchContent)

# Declare and fetch the r8brain library
FetchContent_Declare(
        r8brain
        GIT_REPOSITORY https://github.com/avaneev/r8brain-free-src.git  # URL of the repository
        GIT_TAG b3f0753  # Use old commit, never versions would require some refactorung
)

# Fetch the dependency
FetchContent_MakeAvailable(r8brain)

# Add r8brain as a static library by manually specifying its source files
add_library(r8brain STATIC
        ${r8brain_SOURCE_DIR}/r8bbase.cpp
)

# Optionally set the include directories for r8brain
target_include_directories(r8brain PUBLIC ${r8brain_SOURCE_DIR})

add_library(audio_player
        src/OutputDevice.cpp
        src/Resampler.cpp
        src/AudioProcessor.cpp
        src/StereoConverter.cpp
)

target_include_directories(audio_player PUBLIC inc)

target_link_libraries(audio_player PUBLIC sc-api r8brain iir_static)