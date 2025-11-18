# Does some automation for the qt projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets)

if (NOT Qt6_FOUND)
    message(FATAL_ERROR "QT binaries not found")
endif ()


set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src/ui")

add_library(gui STATIC
        inc/gui/MainWindow.h
        src/MainWindow.cpp
        src/ui/MainWindow.ui

        src/WaveFormVisualizerWidget.h
        src/WaveFormVisualizerWidget.cpp
)

target_link_libraries(gui PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::OpenGLWidgets
        -lopengl32 # TODO remove when not needed anymore

        utils
)

target_include_directories(gui
        PUBLIC inc
        PRIVATE src #So that qt auto uic finds the internal widgets
)
