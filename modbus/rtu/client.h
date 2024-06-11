#ifndef PROJECT_MODBUS_RTU_CLIENT_H
#define PROJECT_MODBUS_RTU_CLIENT_H

#include "modbus/api_client.h"
#include "periph/uart.h"
#include "periph/usb.h"

namespace Project::modbus::rtu {

    class Client : public api::Client {
    public:
        #ifdef HAL_UART_MODULE_ENABLED
        struct ArgsUART { periph::UART& uart; int server_address; };

        explicit Client(ArgsUART args) 
            : api::Client(args.server_address)
            , uart(&args.uart)
        {}
        #endif

        #ifdef HAL_PCD_MODULE_ENABLED
        struct ArgsUSB { periph::USBD& usb; int server_address; };
        
        explicit Client(ArgsUSB args) 
            : api::Client(args.server_address, etl::bind<&Client::_send>(this))
            , usb(&args.usb)
        {}
        #endif

        void init();
        void deinit();

        #ifdef HAL_UART_MODULE_ENABLED
        periph::UART* uart = nullptr;
        #endif

        #ifdef HAL_PCD_MODULE_ENABLED
        periph::USBD* usb = nullptr;
        #endif

    protected:
        etl::Future<etl::Vector<uint8_t>> create_request(etl::Vector<uint8_t> data) override;
        void rxCallback(const uint8_t* data, size_t len);
        
        etl::Promise<etl::Iter<const uint8_t*>> received_data;
    };
}

#endif