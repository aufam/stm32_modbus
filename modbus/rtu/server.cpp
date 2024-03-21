#include "modbus/rtu/server.h"
#include "etl/keywords.h"

fun modbus::rtu::Server::init() -> void {
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->rxCallbackList.push(etl::bind<&Server::rxCallback>(this));
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->rxCallbackList.push(etl::bind<&Server::rxCallback>(this));
    }
    #endif
}

fun modbus::rtu::Server::deinit() -> void {
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->rxCallbackList.pop(etl::bind<&Server::rxCallback>(this));
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->rxCallbackList.pop(etl::bind<&Server::rxCallback>(this));
    }
    #endif
}

void modbus::rtu::Server::_send(const uint8_t* data, size_t len) {
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->transmitBlocking(data, len);
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->transmitBlocking(data, len);
    }
    #endif
}
