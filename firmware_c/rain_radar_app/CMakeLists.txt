cmake_minimum_required(VERSION 3.12)


set(PICO_BOARD pico2_w)

# this can be set here or from the command line
if(NOT DEFINED PICO_BOARD)
    set(PICO_BOARD pico_w)
    # set(PICO_BOARD pico2_w)
    # message(FATAL_ERROR "PICO_BOARD must be set. Either uncomment one of the above lines or set it from command line (e.g., -DPICO_BOARD=pico2_w)")
endif()

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
