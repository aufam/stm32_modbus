#ifndef PROJECT_MODBUS_TCP_CLIENT_H
#define PROJECT_MODBUS_TCP_CLIENT_H

#include "modbus/api_client.h"
#include "wizchip/tcp/client.h"

namespace Project::modbus::tcp {

    class Client : public api::Client, public wizchip::tcp::Client {
    public:
        explicit Client(wizchip::tcp::Client::Args args) 
            : api::Client(0xFF)
            , wizchip::tcp::Client(etl::move(args))
        {}

    protected:
        etl::Future<etl::Vector<uint8_t>> create_request(etl::Vector<uint8_t> data) override;
    };
}

#endif