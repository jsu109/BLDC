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
# === Add SPI_HAL ===
add_library(spi_hal STATIC 
    src/spi/spi_hal.c
) 
target_link_directories(spi_hal PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src/spi
)

# === Add motor_control module ===
add_library(motor_control STATIC
    src/motor_control/motor_control.c
)

target_include_directories(motor_control PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/motor_control
)

target_link_libraries(motor_control
    motor_hal    # Link to HAL for low-level access
)

# === Add motor_hal module ===
add_library(motor_hal STATIC
    src/motor_control/motor_hal.c
)

target_include_directories(motor_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/motor_control
)

target_link_libraries(spi_hal
    pico_stdlib
    hardware_spi
)

target_link_libraries(motor_hal
    pico_stdlib
    hardware_pwm
    spi_hal
    hardware_gpio
)

# === Link everything to the main app ===
target_link_libraries(BLDC_testApp
    pico_stdlib
    motor_control
    
)

# === Output settings ===
pico_enable_stdio_uart(BLDC_testApp 0)
pico_enable_stdio_usb(BLDC_testApp 1)
pico_add_extra_outputs(BLDC_testApp)
