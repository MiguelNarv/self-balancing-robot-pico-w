# Source files in Source subdir to compile.
add_executable(Source
        ${CMAKE_CURRENT_LIST_DIR}/src/main.c
        ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/SEGGER/SEGGER_SYSVIEW.c
        ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/Sample/FreeRTOS/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c
        ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/SEGGER/SEGGER_RTT.c
        ${CMAKE_CURRENT_LIST_DIR}/lib/SEGGER/SystemView/Sample/FreeRTOS/SEGGER_SYSVIEW_FreeRTOS.c
        )
        
# Pull in common dependencies
target_link_libraries(Source 
                # ARM cmsis library ports.
                cmsis_core_headers
                cmsis_core
                # Pico related ports.
                pico_stdlib 
                pico_cyw43_arch_lwip_threadsafe_background
                # Other libraries ports.
                FreeRTOS-Kernel-Heap4
                LWIP_PORT    
                FREERTOS_PORT 
                SYSVIEW_PORT                                                       
                )

# Create map/bin/hex file etc.
pico_add_extra_outputs(Source)

target_include_directories(Source
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
            
        )