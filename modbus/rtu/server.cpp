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


fun modbus::rtu::Server::rxCallback(const uint8_t* data, size_t len) -> void {
    var begin = data;
    val total = len;

    for (; len >= 4; --len) {
        var decoded = api::decode(data, len);
        if (not decoded) 
            continue;
        
        if (decoded[0] == server_address) {
            etl::async<&Server::rxCallbackTask>(this, etl::iter(decoded, decoded + len));
        }

        val len_total = len + 2;
        return len_total == total ? void() : rxCallback(begin + len_total, total - len_total);
    }
}

fun modbus::rtu::Server::rxCallbackTask(etl::Iter<const uint8_t*> data) -> void {
    var res = create_response(etl::vectorize(data));
    res = api::encode(etl::move(res));
    
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->transmitBlocking(res.data(), res.len());
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->transmitBlocking(res.data(), res.len());
    }
    #endif
}