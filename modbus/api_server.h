#ifndef PROJECT_MODBUS_API_SERVER_H
#define PROJECT_MODBUS_API_SERVER_H

#include "modbus/api.h"
#include "etl/unordered_map.h"

namespace Project::modbus::api {
    class Server {
    public:
        explicit Server(int server_address) : server_address(server_address) {}

        Server& CoilGetter(uint16_t register_address, std::function<bool()> getter);
        Server& CoilSetter(uint16_t register_address, std::function<void(bool)> setter);

        Server& HoldingRegisterGetter(uint16_t register_address, std::function<uint16_t()> getter);
        Server& HoldingRegisterSetter(uint16_t register_address, std::function<void(uint16_t)> setter);

        Server& DiscreteInputGetter(uint16_t register_address, std::function<bool()> getter);
        Server& AnalogInputGetter(uint16_t register_address, std::function<uint16_t()> getter);
        
        Server& ExceptionStatusGetter(std::function<uint8_t()> getter);
        Server& DiagnosticGetter(uint8_t sub_function, std::function<etl::Vector<uint8_t>()> getter);
    
    protected:
        etl::Vector<uint8_t> create_response(etl::Vector<uint8_t> data);

        int server_address;

        etl::UnorderedMap<uint16_t, std::function<bool()>> coil_getters;
        etl::UnorderedMap<uint16_t, std::function<void(bool)>> coil_setters;

        etl::UnorderedMap<uint16_t, std::function<uint16_t()>> holding_register_getters;
        etl::UnorderedMap<uint16_t, std::function<void(uint16_t)>> holding_register_setters;

        etl::UnorderedMap<uint16_t, std::function<bool()>> discrete_input_getters;
        etl::UnorderedMap<uint16_t, std::function<uint16_t()>> analog_input_getters;

        std::function<uint8_t()> exception_status_getter;

        etl::UnorderedMap<uint8_t, std::function<etl::Vector<uint8_t>()>> diagnostic_getters;
    };
}

#endif // PROJECT_MODBUS_API_SERVER_H