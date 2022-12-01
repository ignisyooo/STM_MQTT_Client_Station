cmake_minimum_required(VERSION 3.12)
# ##############################################################################

project(F767_MQTT_Station)

enable_language(C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(EXECUTABLE ${CMAKE_PROJECT_NAME}.elf)

# ##############################################################################
set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Core/Startup/startup_stm32f767zitx.s)
set(FREERTOS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Third_Party/FreeRTOS/Source)
set(LWIP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Third_Party/LwIP)

set(CPU_PARAMETERS
    -mthumb
    # This needs attention to properly set for used MCU
    -mcpu=cortex-m7
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

# Headers
set(INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Logger
)

# FreeRTOS headers
set(FREERTOS_DIRECTORIES
    ${FREERTOS_PATH}/CMSIS_RTOS_V2
    ${FREERTOS_PATH}/include
    ${FREERTOS_PATH}/portable/GCC/ARM_CM7/r0p1
)

set(LWIP_DIRECTORIES
    ${LWIP_PATH}/src/include
    ${LWIP_PATH}/src/include/netif/ppp
    ${LWIP_PATH}/src/include/lwip/apps
    ${LWIP_PATH}/src/include/lwip
    ${LWIP_PATH}/src/include/lwip/apps
    ${LWIP_PATH}/src/include/lwip/priv
    ${LWIP_PATH}/src/include/lwip/prot
    ${LWIP_PATH}/src/include/netif
    ${LWIP_PATH}/src/include/compat/posix
    ${LWIP_PATH}/src/include/compat/posix/arpa
    ${LWIP_PATH}/src/include/compat/posix/net
    ${LWIP_PATH}/src/include/compat/posix/sys
    ${LWIP_PATH}/src/include/compat/stdc
    ${LWIP_PATH}/system
    ${LWIP_PATH}/system/arch
)

# Sources
file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/BSP/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/Logger/*.c
)

# FreeRTOS sources
file(GLOB_RECURSE FREERTOS_SOURCES
    ${FREERTOS_PATH}/*.c
)

file(GLOB_RECURSE LWIP_SOURCES
    ${LWIP_PATH}/*.c
)

# Executable files
add_executable(${EXECUTABLE}
    ${SOURCES}
    ${FREERTOS_SOURCES}
    ${LWIP_SOURCES}
    ${STARTUP_SCRIPT}
)

# Add header directories
target_include_directories(${EXECUTABLE} PRIVATE
    ${INCLUDE_DIRECTORIES}
    ${FREERTOS_DIRECTORIES}
    ${LWIP_DIRECTORIES}
)

target_compile_options(${EXECUTABLE} PRIVATE
    ${CPU_PARAMETERS}
    -g
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    $<$<CONFIG:Debug>:-Og>
)

target_link_options(${EXECUTABLE} PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32F767ZITX_FLASH.ld
    ${CPU_PARAMETERS}
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    --specs=nosys.specs
    -u _printf_float                # STDIO float formatting support
    -Wl,--start-group
    -lc
    -lm
    -lstdc++
    -lsupc++
    -Wl,--end-group
    -Wl,--print-memory-usage
)

target_compile_definitions(${EXECUTABLE} PRIVATE
    -DUSE_HAL_DRIVER
    -DSTM32F767xx
    -DFREERTOS
)

# # Print executable size
# add_custom_command(TARGET ${EXECUTABLE}
#     POST_BUILD
#     COMMAND arm-none-eabi-size ${EXECUTABLE}
# )

# Create hex/bin file
add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
)
