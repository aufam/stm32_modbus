#include "modbus/tcp/client.h"
#include "Ethernet/socket.h"
#include "etl/keywords.h"


int modbus::tcp::Client::on_init(int) {
    return SOCK_OK;
}

int modbus::tcp::Client::on_listen(int) {
    return SOCK_OK;
}

int modbus::tcp::Client::on_established(int socket_number) {
    // Interrupt clear
    if (getSn_IR(socket_number) & Sn_IR_CON)
        setSn_IR(socket_number, Sn_IR_CON);

    if (state == STATE_START) {
        // send request
        ::send(socket_number, (uint8_t*) (_msg_send.data), _msg_send.len);

        // Check the TX socket buffer for End of HTTP response sends
        while (getSn_TX_FSR(socket_number) != (getSn_TxMAX(socket_number))) {}

        state = STATE_ESTABLISHED;
    }

    if (state == STATE_ESTABLISHED) {
        // receive response
        size_t len = getSn_RX_RSR(socket_number);
        
        if (len > WIZCHIP_BUFFER_LENGTH) 
            len = WIZCHIP_BUFFER_LENGTH;

        len = ::recv(socket_number, wizchip::Ethernet::rxData, len);
        wizchip::Ethernet::rxData[len] = '\0';

        // TODO: is this necessary?
        if (len > 0) {
            rxCallback(wizchip::Ethernet::rxData, len);
            state = STATE_STOP;
        }
    }

    return SOCK_OK;
}

int modbus::tcp::Client::on_close_wait(int socket_number) {
    return ::disconnect(socket_number);
}

int modbus::tcp::Client::on_closed(int socket_number) {
    if (state == STATE_START) {
        auto ip = host.split<5>(".");
        uint8_t addr[] = { (uint8_t) ip[0].to_int(), (uint8_t) ip[1].to_int(), (uint8_t) ip[2].to_int(), 247 };
        ::socket(socket_number, Sn_MR_TCP, port, Sn_MR_ND);
        return ::connect(socket_number, addr, port);
    }
    if (state == STATE_STOP) {
        deallocate(socket_number);
        return SOCK_OK;
    }
    else { // TODO: ??
        return SOCK_OK;
    }
}

void modbus::tcp::Client::_send(const uint8_t* data, size_t len) {
    _msg_send.data = data;
    _msg_send.len = len;
    allocate(1);
}