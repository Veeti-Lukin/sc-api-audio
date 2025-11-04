# ------------------------------ Project setup ------------------------------
cmake_minimum_required(VERSION 3.20.0)

project(sc-api-audio LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 11)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}  Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}  Version: ${CMAKE_C_COMPILER_VERSION}")
#-----------------------------------------------------------------------------

# ------------------------------ Structure setup ------------------------------
#------------------------------------------------------------------------------

# ----------------------------- Application setup ----------------------------
add_executable(Simucube-api-audio-player
        src/main.cpp
)
#-----------------------------------------------------------------------------