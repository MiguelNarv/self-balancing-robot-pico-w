# Source files in Source subdir to compile
set(SOURCES_TO_ADD  
        ${CMAKE_CURRENT_LIST_DIR}/src/main.c
        ${CMAKE_CURRENT_LIST_DIR}/src/I2C/I2C_usr.c
        ${CMAKE_CURRENT_LIST_DIR}/src/TCPIP/TCPIP_usr.c
        )

if(SYSVIEW_ENABLED)
        set(SOURCES_TO_ADD ${SOURCES_TO_ADD}
                ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/SEGGER/SEGGER_SYSVIEW.c
                ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/Sample/FreeRTOS/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c
                ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/SEGGER/SEGGER_RTT.c
                ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/Sample/FreeRTOS/SEGGER_SYSVIEW_FreeRTOS.c
                )
endif()

add_executable(Source "${SOURCES_TO_ADD}")

# Definition to enable SysView in source
if(SYSVIEW_ENABLED)
        target_compile_definitions(Source PUBLIC "USE_SYSVIEW=${SYSVIEW_ENABLED}")
endif()

target_compile_definitions(Source PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )

if(SYSVIEW_ENABLED)
        target_link_libraries(Source SYSVIEW_PORT)
endif()

# Pull in common dependencies
target_link_libraries(Source 
        # ARM cmsis library ports
        cmsis_core_headers
        cmsis_core
        # Pico related ports
        pico_stdlib
        hardware_i2c
        pico_cyw43_arch_lwip_poll
        # Other libraries ports
        FreeRTOS-Kernel-Heap4
        LWIP_PORT    
        FREERTOS_PORT
        )

# Create map/bin/hex file etc
pico_add_extra_outputs(Source)

target_include_directories(Source
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/inc 
        )