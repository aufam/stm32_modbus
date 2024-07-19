#include "modbus/tcp/server.h"
#include "etl/keywords.h"


auto modbus::tcp::Server::response(int, etl::Vector<uint8_t> data) -> wizchip::Stream {
    auto decoded = api::decode(etl::move(data));
    auto res = create_response(etl::move(decoded));

    wizchip::Stream s;
    return s << [encoded=api::encode(etl::move(res))]() { return etl::iter(encoded); };
}