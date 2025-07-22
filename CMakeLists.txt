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
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === VSCode Integration (Don't touch this section) ===
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
# ==========================================

set(PICO_BOARD pico CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(BLDC_testApp C CXX ASM)
pico_sdk_init()

# === Add application source ===
add_executable(BLDC_testApp
    src/main.c
)

# === Add rp2040 library (platform specific code) ===
add_library(rp2040 STATIC
    lib/rp2040/rp2040_spi.c
    # Add other RP2040-specific source files here as needed
)

target_include_directories(rp2040 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/rp2040
    ${CMAKE_CURRENT_LIST_DIR}/lib/rp2040/spi
    ${CMAKE_CURRENT_LIST_DIR}/src/spi
)

target_link_libraries(rp2040
    hardware_spi
)

# === Add spi_hal library (generic HAL interface) ===
add_library(spi_hal STATIC
    src/spi/spi_hal.c
)

target_include_directories(spi_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/spi
)

target_link_libraries(spi_hal
    rp2040
    pico_stdlib
)

# === Add motor_hal module ===
add_library(motor_hal STATIC
    src/motor_control/motor_hal.c
)
target_include_directories(motor_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/motor_control
)

# === Add encoder_hal module ===
add_library(encoder_hal STATIC
    src/encoder/encoder_hal.c
)
target_include_directories(encoder_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/encoder
)

# === Link dependencies to motor_hal ===
target_link_libraries(motor_hal
    encoder_hal
    pico_stdlib
    hardware_pwm
    spi_hal
    hardware_gpio
)

# === Add motor_control module ===
add_library(motor_control STATIC
    src/motor_control/motor_control.c
)

target_include_directories(motor_control PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/motor_control
)

target_link_libraries(motor_control
    motor_hal
)

# === Link everything to the main app ===
target_link_libraries(BLDC_testApp
    pico_stdlib
    motor_hal
    encoder_hal
)

# === Output settings ===
pico_enable_stdio_uart(BLDC_testApp 0)
pico_enable_stdio_usb(BLDC_testApp 1)
pico_add_extra_outputs(BLDC_testApp)
