#include "modbus/rtu/client.h"
#include "etl/keywords.h"

fun modbus::rtu::Client::init() -> void {
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

fun modbus::rtu::Client::rxCallback(const uint8_t* data, size_t len) -> void {
    var begin = data;
    val total = len;

    for (; len >= 4; --len) {
        var decoded = modbus::api::decode(data, len);
        if (not decoded) 
            continue;
        
        if (decoded[0] == server_address)
            received_data.set(etl::iter(decoded, decoded + len));

        val len_total = len + 2;
        return len_total == total ? void() : rxCallback(begin + len_total, total - len_total);
    }
}

auto modbus::rtu::Client::create_request(etl::Vector<uint8_t> data) -> etl::Future<etl::Vector<uint8_t>> {
    #ifdef HAL_UART_MODULE_ENABLED
    if (uart) {
        uart->transmitBlocking(data.data(), data.len());
    }
    #endif
    #ifdef HAL_PCD_MODULE_ENABLED
    if (usb) {
        usb->transmit(data.data(), data.len());
    }
    #endif

    return received_data.get_future().then([](etl::Iter<const uint8_t*> data) {
        return etl::vectorize(data);
    });
}