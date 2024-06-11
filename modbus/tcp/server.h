#ifndef PROJECT_MODBUS_TCP_SERVER_H
#define PROJECT_MODBUS_TCP_SERVER_H

#include "modbus/api_server.h"
#include "wizchip/tcp/server.h"

namespace Project::modbus::tcp {
    class Server : public api::Server, public wizchip::tcp::Server {
    public:
        struct Args {
            int port;
        };

        explicit Server(Args args) 
            : api::Server(0xFF)
            , wizchip::tcp::Server(wizchip::SocketServer::Args{
                .port=args.port, 
                .response_function=etl::bind<&modbus::tcp::Server::_response_function>(this)
            })
        {}

    protected:
        etl::Vector<uint8_t> _response_function(etl::Vector<uint8_t> data);
    };
}

#endif