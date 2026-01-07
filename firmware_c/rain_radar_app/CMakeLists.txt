cmake_minimum_required(VERSION 3.12)

# Auto-detect PICO_BOARD from build directory name if not set
if(NOT DEFINED PICO_BOARD)
    get_filename_component(BUILD_DIR_NAME ${CMAKE_BINARY_DIR} NAME)
    if(BUILD_DIR_NAME MATCHES "pico2_w")
        set(PICO_BOARD pico2_w)
        message(STATUS "Auto-detected PICO_BOARD=pico2_w from build directory: ${BUILD_DIR_NAME}")
    elseif(BUILD_DIR_NAME MATCHES "pico_w")
        set(PICO_BOARD pico_w)
        message(STATUS "Auto-detected PICO_BOARD=pico_w from build directory: ${BUILD_DIR_NAME}")
    else()
        # Default to pico2_w if no match
        set(PICO_BOARD pico2_w)
        message(WARNING "Could not detect board from build directory '${BUILD_DIR_NAME}', defaulting to PICO_BOARD=pico2_w")
        message(STATUS "Tip: Name your build directory with 'pico_w' or 'pico2_w' for auto-detection, or set -DPICO_BOARD=<board> explicitly")
    endif()
endif()

message(STATUS "Building for PICO_BOARD=${PICO_BOARD}")

# Change your executable name to something creative!
set(NAME rain_radar_${PICO_BOARD})

include(pico_sdk_import.cmake)
include(pimoroni_pico_import.cmake)

# Gooey boilerplate
project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add your source files
add_executable(${NAME}
    main.cpp
    wifi_setup.cpp
    http_client_util.cpp
    data_fetching.cpp
    battery.cpp
)

target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Include required libraries
# This assumes `pimoroni-pico` is stored alongside your project
include(common/pimoroni_i2c)
include(common/pimoroni_bus)
include(libraries/bitmap_fonts/bitmap_fonts)
include(libraries/hershey_fonts/hershey_fonts)
include(libraries/inky_frame_7/inky_frame_7)
include(drivers/fatfs/fatfs)
include(drivers/sdcard/sdcard)

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
    pico_cyw43_arch_lwip_threadsafe_background 
    pico_stdlib
    inky_frame_7
    hardware_pwm
    hardware_spi
    hardware_i2c
    hardware_flash
    hardware_adc
    fatfs
    sdcard
    pico_graphics
    pico_lwip_http
    pico_lwip_mbedtls
    pico_mbedtls
)

pico_enable_stdio_usb(${NAME} 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
