# ------------------------------ Project setup ------------------------------
cmake_minimum_required(VERSION 3.20.0)

project(sc-api-audio LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 11)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}  Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}  Version: ${CMAKE_C_COMPILER_VERSION}")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

if (NOT Qt6_FOUND)
    message(FATAL_ERROR "QT binaries not found")
endif ()

#-----------------------------------------------------------------------------

# ------------------------------ Structure setup ------------------------------
add_subdirectory(libs)
#------------------------------------------------------------------------------

# ----------------------------- Application setup ----------------------------

# Does some automation for the qt projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/ui")

add_executable(Simucube-api-audio-player
        src/main.cpp

        inc/ScApiContext.h
        src/ScApiContext.cpp
  		inc/AudioCapturer.h
        src/AudioCapturer.cpp
 		# ====== GUI ======
        inc/gui/MainWindow.h
        src/gui/MainWindow.cpp
        src/gui/ui/MainWindow.ui

        inc/gui/OutputDeviceControlWindow.h
        src/gui/OutputDeviceControlWindow.cpp
        src/gui/ui/OutputDeviceControlWindow.ui
)

target_include_directories(Simucube-api-audio-player PUBLIC inc)


target_link_libraries(Simucube-api-audio-player PUBLIC
        Qt6::Core
        Qt6::Widgets

        utils
        audio_player
        audio_control_gui
)
#-----------------------------------------------------------------------------