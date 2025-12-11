include(shader_util.cmake)

# Does some automation for the qt projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src/ui")

# Generate cpp header files from the shaders
glsl_to_cpp_header(src/WaveFormVisualizerWidget_vertex_shader.glsl K_VERTEX_SHADER_SOURCE VERTEX_SHADER_HEADER)
glsl_to_cpp_header(src/WaveFormVisualizerWidget_fragment_shader.glsl K_FRAGMENT_SHADER_SOURCE FRAGMENT_SHADER_HEADER)

add_library(audio_control_gui STATIC
        inc/audio_control_gui/AudioControlWidget.h
        src/AudioControlWidget.cpp

        src/WaveFormVisualizerWidget.h
        src/WaveFormVisualizerWidget.cpp
        ${VERTEX_SHADER_HEADER}
        ${FRAGMENT_SHADER_HEADER}

        src/mEqBandWidget.cpp
        inc/audio_control_gui/mEqBandWidget.h
        src/ui/mEqBandWidget.ui
        src/mEqWidget.cpp
        inc/audio_control_gui/mEqWidget.h
        src/ui/mEqWidget.ui

)

target_link_libraries(audio_control_gui PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::OpenGLWidgets

        #audio_player # For OutputDevice

        utils
        audio_player
)

target_include_directories(audio_control_gui
        PUBLIC inc
        PRIVATE
        src #So that qt auto uic finds the internal widgets
        ${CMAKE_CURRENT_BINARY_DIR} # for generated shader header files
)