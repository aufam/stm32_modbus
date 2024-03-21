#ifndef PROJECT_MODBUS_TCP_SERVER_H
#define PROJECT_MODBUS_TCP_SERVER_H

#include "modbus/api_server.h"
#include "wizchip/ethernet.h"
#include "etl/string.h"

namespace Project::modbus::tcp {
    class Server : public api::Server, public wizchip::Socket {
    public:
        explicit Server(Args args) 
            : api::Server(0xFF, etl::bind<&Server::_send>(this))
            , wizchip::Socket({.ethernet=args.ethernet, .port=port})
        {}
        
        Server& start();
        Server& stop();
        bool is_running() const { return _is_running; }

        ///< default: reserve 4 sockets from the ethernet
        int _number_of_socket = 4;

    protected:
        int on_init(int socket_number) override;
        int on_listen(int socket_number) override;
        int on_established(int socket_number) override;
        int on_close_wait(int socket_number) override;
        int on_closed(int socket_number) override;

        bool _is_running = false;
        int _socket_number = 0;
        modbus::api::Message _msg_send; 
        void _send(const uint8_t* data, size_t len);
    };
}

#endif