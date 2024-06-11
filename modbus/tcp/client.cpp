#include "modbus/tcp/client.h"
#include "Ethernet/socket.h"
#include "etl/keywords.h"

auto modbus::tcp::Client::create_request(etl::Vector<uint8_t> data) -> etl::Future<etl::Vector<uint8_t>> {
    return wizchip::tcp::Client::request(modbus::api::encode(etl::move(data)))
        .then([](etl::Vector<uint8_t> data) {
            return modbus::api::decode(etl::move(data));
        });
}
