# Project configuration
cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(PICO_BOARD pico_w)

# SystemView enable/disable variable
set(SYSVIEW_ENABLED TRUE)

# You can set here the wifi credentials you want to connect with if not defined in environment variables
set(WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")
set(WIFI_SSID "$ENV{WIFI_SSID}")

# FreeRtos kernel path definition
set(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_LIST_DIR}/Source/lib/FreeRTOS-Kernel")
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/configs/FreeRTOS-Kernel" CACHE STRING "Local Config")

# Set config files paths to be used
# lwip config files
include(lwip_import.cmake) 
# FreeRTOS config files
include_directories("${FREERTOS_CONFIG_FILE_DIRECTORY}") 

# Init the pico sdk.
pico_sdk_init()

# Initialize all other external libs & dependencies
# FreeRtos
include(FreeRTOS_Kernel_import.cmake)

if(SYSVIEW_ENABLED)
     # SEGGER system view
    include(Sysview_import.cmake) 
endif ()

# Add the next subdir
add_subdirectory(Source)