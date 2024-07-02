# STM32 Modbus
Modbus framework for STM32

## Requirements
* Arm GNU toolchain
* cmake minimum version 3.10
* STM32CubeMx generated code with:
    - FreeRTOS v2
    - SPI (for modbus TCP)
    - UART/USB (for modbus RTU)

## Dependencies
* [Embedded Template Library](https://github.com/aufam/etl/tree/FreeRTOS)
* [STM32 HAL Interface](https://github.com/aufam/stm32_hal_interface)
* [STM32 Wizchip](https://github.com/aufam/stm32_wizchip) (for modbus TCP)

## How to use
* Clone this repo to your STM32 project folder. For example:
```bash
git clone https://github.com/aufam/stm32_modbus.git <your_project_path>/Middlewares/Third_Party/stm32_modbus
```
* Or, if Git is configured, you can add this repo as a submodule:
```bash
git submodule add https://github.com/aufam/stm32_modbus.git <your_project_path>/Middlewares/Third_Party/stm32_modbus
git submodule update --init --recursive
```
* Add these line to your project CMakeLists.txt:
```cmake
# set(MODBUS_DISABLE_RTU) # uncomment to disable modbus RTU
# set(MODBUS_DISABLE_TCP) # uncomment to disable modbus TCP
add_subdirectory(Middlewares/Third_Party/stm32_modbus)
target_link_libraries(${PROJECT_NAME}.elf modbus)
```
