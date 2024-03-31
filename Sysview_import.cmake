# Define FreeRtos library with its included directories.
add_library(SYSVIEW_PORT INTERFACE)
target_include_directories(SYSVIEW_PORT
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/Source/lib/SEGGER/SystemView/Sample/FreeRTOS
        ${CMAKE_CURRENT_LIST_DIR}/Source/lib/SEGGER/SystemView/SEGGER
        ${CMAKE_CURRENT_LIST_DIR}/Source/lib/SEGGER/SystemView/Config
        ${CMAKE_CURRENT_LIST_DIR}/Source/inc
    )