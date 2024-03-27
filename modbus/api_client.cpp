#include "modbus/api_client.h"
#include <etl/keywords.h>


fun modbus::api::Client::rxCallback(const uint8_t* data, size_t len) -> void {
    var begin = data;
    val total = len;

    for (; len >= 4; --len) {
        var decoded = modbus::api::decode(data, len);
        if (not decoded) 
            continue;
        
        if (decoded[0] == server_address)
            que << Message{data, len};

        val len_total = len + 2;
        return len_total == total ? void() : rxCallback(begin + len_total, total - len_total);
    }
}

fun modbus::api::Client::request(const uint8_t* data, size_t len) -> etl::Future<Message> {
    return [this, data, len] (etl::Time timeout) -> etl::Result<Message, osStatus_t> {
        mtx.init();
        var lock = mtx.lock().await();

        val checksum = modbus::api::crc(data, len);
        uint8_t req[len + 2];
        ::memcpy(req, data, len);
        req[len + 0] = (checksum >> 0) & 0xFF;
        req[len + 1] = (checksum >> 8) & 0xFF;

        que.init();
        que.clear();
        send(req, len + 2);

        return que.pop().wait(timeout);
    };
}

fun modbus::api::Client::ReadCoils(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<bool>> {    
    return [this, register_address, n_register] (etl::Time timeout) -> etl::Result<etl::Vector<bool>, osStatus_t> {
        uint8_t req[6] = {};
        req[0] = server_address;
        req[1] = READ_COILS;
        req[2] = (register_address >> 8) & 0xFF;
        req[3] = (register_address >> 0) & 0xFF;
        req[4] = (n_register >> 8) & 0xFF;
        req[5] = (n_register >> 0) & 0xFF;

        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([](Message msg) -> etl::Result<etl::Vector<bool>, osStatus_t> {
                if (msg.data[2] != msg.len - 3)
                    return etl::Err(osError);
                
                var buf = etl::vector_reserve<bool>(msg.len - 3);
                for (val byte in etl::iter(msg.data + 3, msg.data + msg.len))
                    buf += byte;

                return etl::Ok(etl::move(buf));
            });
    };
}

fun modbus::api::Client::ReadDiscreteInputs(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<bool>> {
    return [this, register_address, n_register] (etl::Time timeout) -> etl::Result<etl::Vector<bool>, osStatus_t> {
        uint8_t req[6] = {};
        req[0] = server_address;
        req[1] = READ_DISCRETE_INPUTS;
        req[2] = (register_address >> 8) & 0xFF;
        req[3] = (register_address >> 0) & 0xFF;
        req[4] = (n_register >> 8) & 0xFF;
        req[5] = (n_register >> 0) & 0xFF;

        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([](Message msg) -> etl::Result<etl::Vector<bool>, osStatus_t> {
                if (msg.data[2] != msg.len - 3)
                    return etl::Err(osError);
                
                var buf = etl::vector_reserve<bool>(msg.len - 3);
                for (val byte in etl::iter(msg.data + 3, msg.data + msg.len))
                    buf += byte;

                return etl::Ok(etl::move(buf));
            });
    };
}

fun modbus::api::Client::ReadHoldingRegisters(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<uint16_t>> {
    return [this, register_address, n_register] (etl::Time timeout) -> etl::Result<etl::Vector<uint16_t>, osStatus_t> {
        uint8_t req[6] = {};
        req[0] = server_address;
        req[1] = READ_HOLDING_REGISTERS;
        req[2] = (register_address >> 8) & 0xFF;
        req[3] = (register_address >> 0) & 0xFF;
        req[4] = (n_register >> 8) & 0xFF;
        req[5] = (n_register >> 0) & 0xFF;

        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([](Message msg) -> etl::Result<etl::Vector<uint16_t>, osStatus_t> {
                if (msg.data[2] != msg.len - 3)
                    return etl::Err(osError);

                const int length = msg.data[2] / 2;
                const int index_begin = 3;
                var buf = etl::vector_reserve<uint16_t>(length);
                
                for (int i in etl::range(length)) 
                    buf += msg.data[index_begin + i * 2] << 8 | msg.data[index_begin + i * 2 + 1];
                
                return etl::Ok(etl::move(buf));
            });
    };
}

fun modbus::api::Client::ReadInputRegisters(uint16_t register_address, uint16_t n_register) -> etl::Future<etl::Vector<uint16_t>> {
    return [this, register_address, n_register] (etl::Time timeout) -> etl::Result<etl::Vector<uint16_t>, osStatus_t> {
        uint8_t req[6] = {};
        req[0] = server_address;
        req[1] = READ_INPUT_REGISTERS;
        req[2] = (register_address >> 8) & 0xFF;
        req[3] = (register_address >> 0) & 0xFF;
        req[4] = (n_register >> 8) & 0xFF;
        req[5] = (n_register >> 0) & 0xFF;
        
        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([](Message msg) -> etl::Result<etl::Vector<uint16_t>, osStatus_t> {
                if (msg.data[2] != msg.len - 3)
                    return etl::Err(osError);

                const int length = msg.data[2] / 2;
                const int index_begin = 3;
                var buf = etl::vector_reserve<uint16_t>(length);
                
                for (int i in etl::range(length)) 
                    buf += msg.data[index_begin + i * 2] << 8 | msg.data[index_begin + i * 2 + 1];
                
                return etl::Ok(etl::move(buf));
            });
    };
}

fun modbus::api::Client::WriteSingleCoil(uint16_t register_address, bool value) -> etl::Future<bool> {
    return [this, register_address, value] (etl::Time timeout) -> etl::Result<bool, osStatus_t> {
        uint8_t req[6] = {};
        req[0] = server_address;
        req[1] = WRITE_SINGLE_COIL;
        req[2] = (register_address >> 8) & 0xFF;
        req[3] = (register_address >> 0) & 0xFF;
        req[4] = value ? 0xFF : 0x00;
        req[5] = 0x00;
        
        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([](Message msg) -> etl::Result<bool, osStatus_t> {
                if (msg.len < 5) return etl::Err(osError);
                return etl::Ok(msg.data[4] == 0xFF);
            });
    };
}

fun modbus::api::Client::WriteSingleRegister(uint16_t register_address, uint16_t value) -> etl::Future<uint16_t> {
    return [this, register_address, value] (etl::Time timeout) -> etl::Result<uint16_t, osStatus_t> {
        uint8_t req[6] = {};
        req[0] = server_address;
        req[1] = WRITE_SINGLE_REGISTER;
        req[2] = (register_address >> 8) & 0xFF;
        req[3] = (register_address >> 0) & 0xFF;
        req[4] = (value >> 8) & 0xFF;
        req[5] = (value >> 0) & 0xFF;

        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([](Message msg) -> etl::Result<uint16_t, osStatus_t> {
                if (msg.len != 6) return etl::Err(osError);
                return etl::Ok(msg.data[4] << 8 | msg.data[5]);
            });
    };
}

fun modbus::api::Client::ReadExceptionStatus() -> etl::Future<uint8_t> { 
    return [this](etl::Time timeout) -> etl::Result<uint8_t, osStatus_t> {
        uint8_t req[2] = {};
        req[0] = server_address;
        req[1] = READ_EXCEPTION_STATUS;

        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([](Message msg) -> etl::Result<uint8_t, osStatus_t> {
                if (msg.len < 3) return etl::Err(osError);
                return etl::Ok(msg.data[2]);
            });
    };
}

fun modbus::api::Client::Diagnostic(uint8_t sub_function) -> etl::Future<etl::Vector<uint8_t>> {
    return [this, sub_function](etl::Time timeout) -> etl::Result<etl::Vector<uint8_t>, osStatus_t> {
        uint8_t req[3] = {};
        req[0] = server_address;
        req[1] = DIAGNOSTIC;
        req[2] = sub_function;

        return request(req, sizeof(req))
            .wait(timeout)
            .and_then([] (Message msg) -> etl::Result<etl::Vector<uint8_t>, osStatus_t> {
                if (msg.len < 2) return etl::Err(osError);

                var buf = etl::vector_reserve<uint8_t>(msg.len - 2);
                for (val byte in etl::iter(msg.data + 2, msg.data + msg.len))
                    buf += byte;
                
                return etl::Ok(etl::move(buf));
            });
    };
}

fun modbus::api::Client::WriteMultipleCoils(uint16_t register_address, uint16_t n_register, etl::Vector<bool> values) -> etl::Future<void> {
    return [this, register_address, n_register, values=etl::move(values)] (etl::Time timeout) -> etl::Result<void, osStatus_t> {
        int byte_count = values.len() / 8 + 1;
        var req = etl::vector_reserve<uint8_t>(7 + byte_count);

        req.append(server_address);
        req.append(WRITE_MULTIPLE_COILS);
        req.append((register_address >> 8) & 0xFF);
        req.append((register_address >> 0) & 0xFF);
        req.append((n_register >> 8) & 0xFF);
        req.append((n_register >> 0) & 0xFF);
        req.append(byte_count);

        uint8_t byte = 0;
        int cnt = 0;
        for (val bit in values) {
            byte |= bit << cnt++;
            if (cnt == 8) {
                req.append(byte);
                cnt = 0;
            }
        }
        if (cnt > 0 && cnt < 8) {
            req.append(byte);
        }

        return request(req.data(), req.len()).wait(timeout);
    };
}

fun modbus::api::Client::WriteMultipleRegisters(uint16_t register_address, uint16_t n_register, etl::Vector<uint16_t> values) -> etl::Future<void> {
    return [this, register_address, n_register, values=etl::move(values)] (etl::Time timeout) -> etl::Result<void, osStatus_t> {
        int byte_count = values.len() * 2;
        var req = etl::vector_reserve<uint8_t>(7 + byte_count);
        
        req.reserve(7 + byte_count);
        req.append(server_address);
        req.append(WRITE_MULTIPLE_REGISTERS);
        req.append((register_address >> 8) & 0xFF);
        req.append((register_address >> 0) & 0xFF);
        req.append((n_register >> 8) & 0xFF);
        req.append((n_register >> 0) & 0xFF);
        req.append(byte_count);

        for (val reg in values) {
            req.append((reg >> 8) & 0xFF);
            req.append((reg >> 0) & 0xFF);
        }
        
        return request(req.data(), req.len()).wait(timeout);
    };
}
