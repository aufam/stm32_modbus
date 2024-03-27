#include "modbus/rtu/client.h"
#include "etl/keywords.h"

fun modbus::rtu::Client::init() -> void {
    que.init();
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->rxCallbackList.push(etl::bind<&Client::rxCallback>(this));
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->rxCallbackList.push(etl::bind<&Client::rxCallback>(this));
    }
    #endif
}

fun modbus::rtu::Client::deinit() -> void {
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->rxCallbackList.pop(etl::bind<&Client::rxCallback>(this));
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->rxCallbackList.pop(etl::bind<&Client::rxCallback>(this));
    }
    #endif
}

void modbus::rtu::Client::_send(const uint8_t* data, size_t len) {
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->transmitBlocking(data, len);
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->transmit(data, len);
    }
    #endif
}