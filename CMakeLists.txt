# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(BLDC_testApp C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# === Add executable with src files ===
file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
)

add_executable(BLDC_testApp ${APP_SOURCES})

pico_set_program_name(BLDC_testApp "BLDC_testApp")
pico_set_program_version(BLDC_testApp "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(BLDC_testApp 0)
pico_enable_stdio_usb(BLDC_testApp 1)

# Add the required Pico libraries
target_link_libraries(BLDC_testApp
    pico_stdlib
    hardware_pwm
    hardware_spi
    hardware_gpio
)

# Add include directories
target_include_directories(BLDC_testApp PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/lib
)

# Generate .uf2, .bin, etc.
pico_add_extra_outputs(BLDC_testApp)
