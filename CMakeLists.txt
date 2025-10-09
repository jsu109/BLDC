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

set(PICO_BOARD pico CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(BLDC_testApp C CXX ASM)
pico_sdk_init()

# === Add main application source ===
add_executable(BLDC_testApp
    src/main.c
)

# === Platform headers only (e.g. sysType.h) ===
add_library(platform_headers INTERFACE)
target_include_directories(platform_headers INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/lib/platform
)

# === Platform abstraction layer ===
add_library(platform INTERFACE)

if(PICO_BOARD STREQUAL "pico")
    add_library(rp2040 STATIC
        lib/platform/rp2040/rp2040_spi.c
        lib/platform/rp2040/rp2040_gpio.c
        lib/platform/rp2040/rp2040_pwm.c
        lib/platform/rp2040/rp2040_timer.c
        lib/platform/rp2040/rp2040_adc.c
    )

    target_include_directories(rp2040 PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/lib/platform/rp2040
        ${CMAKE_CURRENT_LIST_DIR}/src/gpio
        ${CMAKE_CURRENT_LIST_DIR}/src/spi
        ${CMAKE_CURRENT_LIST_DIR}/src/pwm
        ${CMAKE_CURRENT_LIST_DIR}/src/timer
        ${CMAKE_CURRENT_LIST_DIR}/src/adc


    )

    target_link_libraries(rp2040
        hardware_spi
        hardware_adc
        hardware_gpio
        hardware_pwm
        hardware_timer
        platform_headers
    )

    # platform "implements" rp2040
    target_link_libraries(platform INTERFACE rp2040)
else()
    message(FATAL_ERROR "Unsupported platform: ${PICO_BOARD}")
endif()

# === Add AS5048A driver ===
add_library(drivers_AS5048A STATIC
    lib/drivers/AS5048A/AS5048A.c
)

target_include_directories(drivers_AS5048A PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/drivers/AS5048A
    ${CMAKE_CURRENT_LIST_DIR}/src/encoder
)

target_link_libraries(drivers_AS5048A
    platform
    pico_stdlib
    platform_headers
)

# === Add DRV8317 driver ===
add_library(drivers_DRV8317 STATIC
    lib/drivers/DRV8317/DRV8317.c
)

target_include_directories(drivers_DRV8317 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/drivers/DRV8317
)

target_link_libraries(drivers_DRV8317
    platform
    pico_stdlib
    platform_headers
)

# === Add gpio_hal ===
add_library(gpio_hal STATIC
    src/gpio/gpio_hal.c
)

target_include_directories(gpio_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/gpio
)

target_link_libraries(gpio_hal
    platform
    pico_stdlib
    platform_headers
)

# === Add adc_hal ===
add_library(adc_hal STATIC
    src/adc/adc_hal.c
)

target_include_directories(adc_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/adc
)

target_link_libraries(adc_hal
    platform
    pico_stdlib
    platform_headers
)

# === Add spi_hal ===
add_library(spi_hal STATIC
    src/spi/spi_hal.c
)

target_include_directories(spi_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/spi
)

target_link_libraries(spi_hal
    platform
    pico_stdlib
    platform_headers
)

# === Add pwm_hal ===
add_library(pwm_hal STATIC
    src/pwm/pwm_hal.c
)

target_include_directories(pwm_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/pwm
)

target_link_libraries(pwm_hal
    platform
    pico_stdlib
    platform_headers
)

# === Add timer_hal ===
add_library(timer_hal STATIC
    src/timer/timer_hal.c
)

target_include_directories(timer_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/timer
)

target_link_libraries(timer_hal
    platform
    pico_stdlib
    platform_headers
)

# === Add encoder_hal ===
add_library(encoder_hal STATIC
    src/encoder/encoder_hal.c
)

target_include_directories(encoder_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/encoder
    ${CMAKE_CURRENT_LIST_DIR}/src/spi
    ${CMAKE_CURRENT_LIST_DIR}/src/timer
)

target_link_libraries(encoder_hal
    gpio_hal
    spi_hal
    timer_hal
    drivers_AS5048A
    pico_stdlib
)

# === Add motor_hal ===
add_library(motor_hal STATIC
    src/motor_control/motor_hal.c
)

target_include_directories(motor_hal PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/motor_control
)

target_link_libraries(motor_hal
    encoder_hal
    spi_hal
    hardware_gpio
)


# === Add transform ===
add_library(transform STATIC
    src/motor_control/transform.c
)

target_include_directories(transform PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/motor_control
)

target_link_libraries(transform
    
)

# === Add motor_control ===
add_library(motor_control STATIC
    src/motor_control/motor_control.c
)

target_include_directories(motor_control PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src/motor_control
)

target_link_libraries(motor_control
    motor_hal
    transform
)

# === Link everything to the main app ===
target_link_libraries(BLDC_testApp
    pico_stdlib
    hardware_adc
    motor_control
    motor_hal
    encoder_hal
    pwm_hal
    timer_hal
    transform
)

# === Output settings ===
pico_enable_stdio_uart(BLDC_testApp 0)
pico_enable_stdio_usb(BLDC_testApp 1)
pico_add_extra_outputs(BLDC_testApp)
