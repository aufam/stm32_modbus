#ifndef PROJECT_MODBUS_TCP_SERVER_H
#define PROJECT_MODBUS_TCP_SERVER_H

#include "modbus/api_server.h"
#include "wizchip/tcp/server.h"

namespace Project::modbus::tcp {
    class Server : public api::Server, public wizchip::tcp::Server {
    public:
        Server() : api::Server(0xFF), wizchip::tcp::Server() {}

    protected:
        wizchip::Stream response(int socket_number, etl::Vector<uint8_t> data) override;
    };
}

#endif