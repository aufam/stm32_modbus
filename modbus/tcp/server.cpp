#include "modbus/tcp/server.h"
#include "etl/keywords.h"


auto modbus::tcp::Server::_response_function(etl::Vector<uint8_t> data) -> etl::Vector<uint8_t> {
    auto decoded = api::decode(etl::move(data));
    auto res = create_response(etl::move(decoded));
    return api::encode(etl::move(res));
}