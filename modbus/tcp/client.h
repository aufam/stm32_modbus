#ifndef PROJECT_MODBUS_TCP_CLIENT_H
#define PROJECT_MODBUS_TCP_CLIENT_H

#include "modbus/api_client.h"
#include "wizchip/ethernet.h"
#include "etl/string.h"

namespace Project::modbus::tcp {

    class Client : public api::Client, public wizchip::Socket {
    public:
        struct Args {
            wizchip::Ethernet& ethernet;
            etl::StringView host;
            int port;
            etl::Time timeout = etl::time::seconds(1);
        };

        explicit Client(Args args) 
            : api::Client(0xFF, etl::bind<&Client::_send>(this))
            , wizchip::Socket({.ethernet=args.ethernet, .port=port})
        {}

    protected:
        int on_init(int socket_number) override;
        int on_listen(int socket_number) override;
        int on_established(int socket_number) override;
        int on_close_wait(int socket_number) override;
        int on_closed(int socket_number) override;

        enum {
            STATE_START,
            STATE_ESTABLISHED,
            STATE_STOP,
        };

        int state = STATE_START;
        etl::StringView host;

        modbus::api::Message _msg_send; 
        void _send(const uint8_t* data, size_t len);
    };
}

#endif