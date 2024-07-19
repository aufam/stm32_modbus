#include "Ethernet/socket.h"
#include "modbus/tcp/client.h"
#include "etl/keywords.h"

auto modbus::tcp::Client::create_request(etl::Vector<uint8_t> data) -> etl::Future<etl::Vector<uint8_t>> {
    wizchip::Stream s;
    s << [data=modbus::api::encode(mv | data)]() { return data.iter(); };
    return wizchip::tcp::Client::request(mv | s)
        .then([](etl::Vector<uint8_t> data) {
            return modbus::api::decode(etl::move(data));
        });
}
