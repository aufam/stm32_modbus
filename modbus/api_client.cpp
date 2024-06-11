#include "modbus/api_client.h"
#include <etl/keywords.h>


fun modbus::api::Client::ReadCoils(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<bool>> {   
    auto data = etl::vector_reserve<uint8_t>(8);
    data.append(server_address); 
    data.append(READ_COILS); 
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append((n_register >> 8) & 0xFF);
    data.append((n_register >> 0) & 0xFF);

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<etl::Vector<bool>, osStatus_t> {
            if (data[2] != data.len() - 3)
                return etl::Err(osError);
            
            var buf = etl::vector_reserve<bool>(data.len() - 3);
            for (val byte in etl::iter(data.begin() + 3, data.end()))
                buf += byte;

            return etl::Ok(etl::move(buf));
        });
}

fun modbus::api::Client::ReadDiscreteInputs(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<bool>> {
    auto data = etl::vector_reserve<uint8_t>(8);
    data.append(server_address); 
    data.append(READ_DISCRETE_INPUTS); 
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append((n_register >> 8) & 0xFF);
    data.append((n_register >> 0) & 0xFF);

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<etl::Vector<bool>, osStatus_t> {
            if (data[2] != data.len() - 3)
                return etl::Err(osError);
            
            var buf = etl::vector_reserve<bool>(data.len() - 3);
            for (val byte in etl::iter(data.begin() + 3, data.end()))
                buf += byte;

            return etl::Ok(etl::move(buf));
        });
}

fun modbus::api::Client::ReadHoldingRegisters(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<uint16_t>> {
    auto data = etl::vector_reserve<uint8_t>(8);
    data.append(server_address); 
    data.append(READ_HOLDING_REGISTERS); 
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append((n_register >> 8) & 0xFF);
    data.append((n_register >> 0) & 0xFF);

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<etl::Vector<uint16_t>, osStatus_t> {
            if (data[2] != data.len() - 3)
                return etl::Err(osError);

            const int length = data[2] / 2;
            const int index_begin = 3;
            var buf = etl::vector_reserve<uint16_t>(length);
            
            for (int i in etl::range(length)) 
                buf += data[index_begin + i * 2] << 8 | data[index_begin + i * 2 + 1];
            
            return etl::Ok(etl::move(buf));
        });
}

fun modbus::api::Client::ReadInputRegisters(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<uint16_t>> {
    auto data = etl::vector_reserve<uint8_t>(8);
    data.append(server_address); 
    data.append(READ_INPUT_REGISTERS); 
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append((n_register >> 8) & 0xFF);
    data.append((n_register >> 0) & 0xFF);

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<etl::Vector<uint16_t>, osStatus_t> {
            if (data[2] != data.len() - 3)
                return etl::Err(osError);

            const int length = data[2] / 2;
            const int index_begin = 3;
            var buf = etl::vector_reserve<uint16_t>(length);
            
            for (int i in etl::range(length)) 
                buf += data[index_begin + i * 2] << 8 | data[index_begin + i * 2 + 1];
            
            return etl::Ok(etl::move(buf));
        });
}

fun modbus::api::Client::WriteSingleCoil(uint16_t register_address, bool value) -> etl::Future<bool> {
    auto data = etl::vector_reserve<uint8_t>(8);
    data.append(server_address); 
    data.append(WRITE_SINGLE_COIL); 
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append(value ? 0xFF : 0x00);
    data.append(0x00);

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<bool, osStatus_t> {
            if (data.len() < 5) return etl::Err(osError);
            return etl::Ok(data[4] == 0xFF);
        });
}

fun modbus::api::Client::WriteSingleRegister(uint16_t register_address, uint16_t value) -> etl::Future<uint16_t> {
    auto data = etl::vector_reserve<uint8_t>(8);
    data.append(server_address); 
    data.append(WRITE_SINGLE_REGISTER); 
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append((value >> 8) & 0xFF);
    data.append((value >> 0) & 0xFF);

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<uint16_t, osStatus_t> {
            if (data.len() != 6) return etl::Err(osError);
            return etl::Ok(data[4] << 8 | data[5]);
        });
}

fun modbus::api::Client::ReadExceptionStatus() -> etl::Future<uint8_t> { 
    auto data = etl::vector_reserve<uint8_t>(4);
    data.append(server_address); 
    data.append(READ_EXCEPTION_STATUS); 

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<uint8_t, osStatus_t> {
            if (data.len() < 3) return etl::Err(osError);
            return etl::Ok(data[2]);
        });
}

fun modbus::api::Client::Diagnostic(uint8_t sub_function) -> etl::Future<etl::Vector<uint8_t>> {
    auto data = etl::vector_reserve<uint8_t>(5);
    data.append(server_address); 
    data.append(DIAGNOSTIC); 
    data.append(sub_function); 

    return create_request(etl::move(data))
        .and_then([](etl::Vector<uint8_t> data) -> etl::Result<etl::Vector<uint8_t>, osStatus_t> {
            if (data.len() < 2) return etl::Err(osError);

            var buf = etl::vector_reserve<uint8_t>(data.len() - 2);
            for (val byte in etl::iter(data.begin() + 2, data.end()))
                buf += byte;
            
            return etl::Ok(etl::move(buf));
        });
}

fun modbus::api::Client::WriteMultipleCoils(uint16_t register_address, uint16_t n_register, etl::Vector<bool> values) -> etl::Future<void> {
    int byte_count = values.len() / 8 + 1;
    var data = etl::vector_reserve<uint8_t>(7 + byte_count + 2);

    data.append(server_address);
    data.append(WRITE_MULTIPLE_COILS);
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append((n_register >> 8) & 0xFF);
    data.append((n_register >> 0) & 0xFF);
    data.append(byte_count);

    uint8_t byte = 0;
    int cnt = 0;
    for (val bit in values) {
        byte |= bit << cnt++;
        if (cnt == 8) {
            data.append(byte);
            cnt = 0;
        }
    }
    if (cnt > 0 && cnt < 8) {
        data.append(byte);
    }

    return create_request(etl::move(data));
}

fun modbus::api::Client::WriteMultipleRegisters(uint16_t register_address, uint16_t n_register, etl::Vector<uint16_t> values) -> etl::Future<void> {
    int byte_count = values.len() * 2;
    var data = etl::vector_reserve<uint8_t>(7 + byte_count);
    
    data.reserve(7 + byte_count);
    data.append(server_address);
    data.append(WRITE_MULTIPLE_REGISTERS);
    data.append((register_address >> 8) & 0xFF);
    data.append((register_address >> 0) & 0xFF);
    data.append((n_register >> 8) & 0xFF);
    data.append((n_register >> 0) & 0xFF);
    data.append(byte_count);

    for (val reg in values) {
        data.append((reg >> 8) & 0xFF);
        data.append((reg >> 0) & 0xFF);
    }

    return create_request(etl::move(data));
}
