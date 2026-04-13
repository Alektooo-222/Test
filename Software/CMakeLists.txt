cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME VCP_F446)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C and CPP languages
enable_language(C CXX ASM)

add_subdirectory(etl)  # For etl library

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

add_subdirectory(swoTrace)
add_subdirectory(dwt)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${CMAKE_SOURCE_DIR}/Core/Src/usbd_cdc_interface.c
    ${CMAKE_SOURCE_DIR}/Core/Src/usbd_conf.c
    ${CMAKE_SOURCE_DIR}/Core/Src/usbd_desc.c
    ${CMAKE_SOURCE_DIR}/App/Src/application.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/application_callbacks.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/application_pwm.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/application_measure.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/application_transmit.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/pins.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/tables.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/tim.cpp

)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_SOURCE_DIR}/App/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    USE_USB_FS=1
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    etl::etl
    swoTrace
    dwt
    # Add user defined libraries
)
