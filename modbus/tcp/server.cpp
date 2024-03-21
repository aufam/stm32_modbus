#include "modbus/tcp/server.h"
#include "Ethernet/socket.h"
#include "etl/keywords.h"


int modbus::tcp::Server::on_init(int socket_number) {
    return ::listen(socket_number);
}

int modbus::tcp::Server::on_listen(int) {
    return SOCK_OK;
}

int modbus::tcp::Server::on_established(int socket_number) {
    if (_msg_send.data == nullptr) {
        // Interrupt clear
        if (getSn_IR(socket_number) & Sn_IR_CON)
            setSn_IR(socket_number, Sn_IR_CON);

        size_t len = getSn_RX_RSR(socket_number);
        if (len == 0) 
            return SOCK_ERROR;
        
        if (len > WIZCHIP_BUFFER_LENGTH) 
            len = WIZCHIP_BUFFER_LENGTH;

        len = ::recv(socket_number, wizchip::Ethernet::rxData, len);
        wizchip::Ethernet::rxData[len] = '\0';

        auto decoded = modbus::api::decode(wizchip::Ethernet::rxData, len);
        _socket_number = socket_number;
        callbackTask(decoded, len);
        
        etl::this_thread::sleep(1ms); // sleep briefly in case the _msg_send is already available
    } 
    if (_msg_send.data) {
        ::send(socket_number, (uint8_t*)_msg_send.data, _msg_send.len);
        // Check the TX socket buffer for End of HTTP response sends
        while (getSn_TX_FSR(socket_number) != (getSn_TxMAX(socket_number))) {}
        _msg_send = {};
    }

    return SOCK_OK;
}

int modbus::tcp::Server::on_close_wait(int socket_number) {
    return ::disconnect(socket_number);
}

int modbus::tcp::Server::on_closed(int socket_number) {
    if (_is_running) {
        // init socket again
        return ::socket(socket_number, Sn_MR_TCP, port, 0x00);
    } else {
        deallocate(socket_number);
    return SOCK_OK;
    }
}

void modbus::tcp::Server::_send(const uint8_t* data, size_t len) {
    _msg_send.data = data;
    _msg_send.len = len;
}

auto modbus::tcp::Server::start() -> modbus::tcp::Server& {
    _is_running = true;
    allocate(_number_of_socket);
    return *this;
}

auto modbus::tcp::Server::stop() -> modbus::tcp::Server& {
    _is_running = false;
    return *this;
}