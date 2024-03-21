# Modbus library for STM32

## Requirements
* C++17
* cmake minimum version 3.7
* STM32CubeMx generated code
* [Embedded template library](https://github.com/aufam/etl/tree/FreeRTOS)
* [HAL peripheral interface](https://github.com/aufam/stm32_hal_interface.git)

## How to use
* Clone this repo to your STM32 project folder. For example:
```bash
git clone https://github.com/aufam/stm32_modbus.git your_project_path/Middlewares/Third_Party/stm32_modbus
```
* Add these line to your project CMakeLists.txt:
```cmake
add_subdirectory(Middlewares/Third_Party/stm32_modbus)
target_link_libraries(${PROJECT_NAME}.elf modbus)
```
* (Optional) Add the submodule:
```bash
git submodule add https://github.com/aufam/stm32_modbus.git your_project_path/Middlewares/Third_Party/stm32_modbus
```
