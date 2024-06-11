#ifndef PROJECT_MODBUS_API_CLIENT_H
#define PROJECT_MODBUS_API_CLIENT_H

#include "modbus/api.h"
#include "etl/mutex.h"

namespace Project::modbus::api {
    class Client {
    public:
        explicit Client(int server_address) : server_address(server_address) {}

        etl::Future<etl::Vector<bool>> ReadCoils(uint16_t register_address, uint16_t n_register);
        etl::Future<etl::Vector<bool>> ReadDiscreteInputs(uint16_t register_address, uint16_t n_register);
        
        etl::Future<etl::Vector<uint16_t>> ReadHoldingRegisters(uint16_t register_address, uint16_t n_register);
        etl::Future<etl::Vector<uint16_t>> ReadInputRegisters(uint16_t register_address, uint16_t n_register);
        
        etl::Future<bool> WriteSingleCoil(uint16_t register_address, bool value);
        etl::Future<uint16_t> WriteSingleRegister(uint16_t register_address, uint16_t value);
        
        etl::Future<uint8_t> ReadExceptionStatus();
        etl::Future<etl::Vector<uint8_t>> Diagnostic(uint8_t sub_function);

        etl::Future<void> WriteMultipleCoils(uint16_t register_address, uint16_t n_register, etl::Vector<bool> values);
        etl::Future<void> WriteMultipleRegisters(uint16_t register_address, uint16_t n_register, etl::Vector<uint16_t> values);
    
    protected:        
        virtual etl::Future<etl::Vector<uint8_t>> create_request(etl::Vector<uint8_t> data) = 0;

        int server_address;
    };
}

#endif // PROJECT_MODBUS_API_CLIENT_H