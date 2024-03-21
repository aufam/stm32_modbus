file(READ version.txt MODBUS_VERSION)
message("MODBUS_VERSION : ${MODBUS_VERSION}")

option(MODBUS_ENABLE_RTU "Enable RTU" ON)
option(MODBUS_ENABLE_TCP "Enable TCP" ON)

# sources
file(GLOB MODBUS_SOURCES modbus/*.*)

if (MODBUS_ENABLE_RTU)
    file(GLOB_RECURSE MODBUS_RTU_SOURCES modbus/rtu/*.*)
endif()

if (MODBUS_ENABLE_TCP)
    file(GLOB_RECURSE MODBUS_TCP_SOURCES modbus/tcp/*.*)
endif()

add_library(modbus ${MODBUS_SOURCES} ${MODBUS_RTU_SOURCES} ${MODBUS_TCP_SOURCES})

# include dirs
target_include_directories(modbus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# defines
target_compile_definitions(modbus PUBLIC -DMODBUS_VERSION="${MODBUS_VERSION}")

# depends
if (MODBUS_ENABLE_RTU)
    target_link_libraries(modbus periph)
endif()

if (MODBUS_ENABLE_TCP)
    target_link_libraries(modbus wizchip)
endif()