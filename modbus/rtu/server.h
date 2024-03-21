#ifndef PROJECT_MODBUS_RTU_SERVER_H
#define PROJECT_MODBUS_RTU_SERVER_H

#include "modbus/api_server.h"
#include "periph/uart.h"
#include "periph/usb.h"

namespace Project::modbus::rtu {

    class Server : public api::Server {
    public:
        #ifdef HAL_UART_MODULE_ENABLED
        struct ArgsUART { periph::UART& uart; int server_address; };
        explicit Server(ArgsUART args) : api::Server(
            args.server_address, 
            etl::bind<&Server::_send>(this)
        ), uart(&args.uart)
        {}
        #endif

        #ifdef HAL_PCD_MODULE_ENABLED
        struct ArgsUSB { periph::USBD& usb; int server_address; };
        explicit Server(ArgsUSB args) : api::Server(
            args.server_address, 
            etl::bind<&Server::_send>(this)
        ), usb(&args.usb)
        {}
        #endif

        void init();
        void deinit();

    protected:
        #ifdef HAL_UART_MODULE_ENABLED
        periph::UART* uart = nullptr;
        #endif

        #ifdef HAL_PCD_MODULE_ENABLED
        periph::USBD* usb = nullptr;
        #endif

        void _send(const uint8_t* data, size_t len);
    };
}

#endif