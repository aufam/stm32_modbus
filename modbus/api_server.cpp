#include "modbus/api_server.h"
#include "etl/scope_exit.h"
#include "etl/async.h"
#include <etl/keywords.h>


fun modbus::api::Server::CoilGetter(uint16_t register_address, std::function<bool()> getter) -> Server& {
    coil_getters[register_address] = etl::move(getter);
    return *this;
}

fun modbus::api::Server::CoilSetter(uint16_t register_address, std::function<void(bool)> setter) -> Server& {
    coil_setters[register_address] = etl::move(setter);
    return *this;
}

fun modbus::api::Server::HoldingRegisterGetter(uint16_t register_address, std::function<uint16_t()> getter) -> Server& {
    holding_register_getters[register_address] = etl::move(getter);
    return *this;
}

fun modbus::api::Server::HoldingRegisterSetter(uint16_t register_address, std::function<void(uint16_t)> setter) -> Server& {
    holding_register_setters[register_address] = etl::move(setter);
    return *this;
}

fun modbus::api::Server::DiscreteInputGetter(uint16_t register_address, std::function<bool()> getter) -> Server& {
    discrete_input_getters[register_address] = etl::move(getter);
    return *this;
}

fun modbus::api::Server::AnalogInputGetter(uint16_t register_address, std::function<uint16_t()> getter) -> Server& {
    analog_input_getters[register_address] = etl::move(getter);
    return *this;
}

fun modbus::api::Server::ExceptionStatusGetter(std::function<uint8_t()> getter) -> Server& {
    exception_status_getter = etl::move(getter);
    return *this;
}

fun modbus::api::Server::DiagnosticGetter(uint8_t sub_function, std::function<etl::Vector<uint8_t>()> getter) -> Server& {
    diagnostic_getters[sub_function] = etl::move(getter);
    return *this;
}

fun modbus::api::Server::response(const uint8_t* data, size_t len) const -> void {
    val checksum = modbus::api::crc(data, len);
    uint8_t res[len + 2];
    ::memcpy(res, data, len);
    res[len + 0] = (checksum >> 0) & 0xFF;
    res[len + 1] = (checksum >> 8) & 0xFF;

    send(res, len + 2);
}

fun modbus::api::Server::rxCallback(const uint8_t* data, size_t len) -> void {
    var begin = data;
    val total = len;

    for (; len >= 4; --len) {
        var decoded = decode(data, len);
        if (not decoded) 
            continue;
        
        if (decoded[0] == server_address) {
            etl::async<&Server::callbackTask>(this, etl::move(decoded), etl::move(len));
        }

        val len_total = len + 2;
        return len_total == total ? void() : rxCallback(begin + len_total, total - len_total);
    }
}

fun modbus::api::Server::callbackTask(const uint8_t* data, size_t len) -> void {
    if (len == 0)
        return;
    
    uint8_t decoded[len];
    memcpy(decoded, data, len);

    switch (decoded[1]) {
        case FunctionCode::READ_COILS: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t length = decoded[4] << 8 | decoded[5];

            uint8_t buffer[length + 3];
            buffer[0] = decoded[0];
            buffer[1] = decoded[1];

            int idx = 3;
            int cnt = 0;
            for (val i in etl::range<uint16_t>(start_register, start_register + length)) {
                if (coil_getters.has(i)) {
                    buffer[idx++] = coil_getters[i]();
                    ++cnt;
                }
            }

            buffer[2] = cnt;
            response(buffer, idx);
            break;
        }
        case FunctionCode::READ_DISCRETE_INPUTS: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t length = decoded[4] << 8 | decoded[5];

            uint8_t buffer[length + 3];
            buffer[0] = decoded[0];
            buffer[1] = decoded[1];

            int idx = 3;
            int cnt = 0;
            for (val i in etl::range<uint16_t>(start_register, start_register + length)) {
                if (discrete_input_getters.has(i)) {
                    buffer[idx++] = discrete_input_getters[i]();
                    ++cnt;
                }
            }

            buffer[2] = cnt;
            response(buffer, idx);
            break;
        }
        case FunctionCode::READ_HOLDING_REGISTERS: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t length = decoded[4] << 8 | decoded[5];

            uint8_t buffer[length * 2 + 3];
            buffer[0] = decoded[0];
            buffer[1] = decoded[1];

            int idx = 3;
            int cnt = 0;
            for (val i in etl::range<uint16_t>(start_register, start_register + length)) {
                if (holding_register_getters.has(i)) {
                    val value = holding_register_getters[i]();
                    buffer[idx++] = (value >> 8) & 0xFF;
                    buffer[idx++] = (value >> 0) & 0xFF;
                    cnt += 2;
                }
            }

            buffer[2] = cnt;
            response(buffer, idx);
            break;
        }
        case FunctionCode::READ_INPUT_REGISTERS: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t length = decoded[4] << 8 | decoded[5];

            uint8_t buffer[length * 2 + 3];
            buffer[0] = decoded[0];
            buffer[1] = decoded[1];

            int idx = 3;
            int cnt = 0;
            for (val i in etl::range<uint16_t>(start_register, start_register + length)) {
                if (analog_input_getters.has(i)) {
                    val value = analog_input_getters[i]();
                    buffer[idx++] = (value >> 8) & 0xFF;
                    buffer[idx++] = (value >> 0) & 0xFF;
                    cnt += 2;
                }
            }

            buffer[2] = cnt;
            response(buffer, idx);
            break;
        }
        case FunctionCode::WRITE_SINGLE_COIL: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t value = decoded[4] << 8 | decoded[5];

            if (value == 0xFF00 and coil_setters.has(start_register)) {
                coil_setters[start_register](true);
            }
            if (value == 0x0000 and coil_setters.has(start_register)) {
                coil_setters[start_register](false);
            }

            response(decoded, len);
            break;
        }
        case FunctionCode::WRITE_SINGLE_REGISTER: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t value = decoded[4] << 8 | decoded[5];

            if (holding_register_setters.has(start_register)) {
                holding_register_setters[start_register](value);
            }

            response(decoded, len);
            break;
        }
        case FunctionCode::READ_EXCEPTION_STATUS: {
            uint8_t buffer[3];
            buffer[0] = decoded[0];
            buffer[1] = decoded[1];
            buffer[2] = exception_status_getter();

            response(buffer, 3);
            break;
        }
        case FunctionCode::DIAGNOSTIC: {
            val sub_function = decoded[2];
            val diagnose = diagnostic_getters[sub_function]();

            uint8_t buffer[2 + diagnose.len()];
            buffer[0] = decoded[0];
            buffer[1] = decoded[1];
            ::memcpy(buffer + 2, diagnose.data(), diagnose.len());

            response(buffer, 2 + diagnose.len());
            break;
        }
        case FunctionCode::WRITE_MULTIPLE_COILS: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t length = decoded[4] << 8 | decoded[5];

            const size_t byte_count = decoded[6];
            if (byte_count != len - 7)
                break;
            
            int bit_index = 0;
            var buf_index = 7;
            for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
                if (not coil_setters.has(reg)) 
                    continue;
                
                coil_setters[reg]((1 << bit_index++) & decoded[buf_index]);
                if (bit_index == 8) {
                    bit_index = 0;
                    ++buf_index;
                }
            }

            response(decoded, 6);
            break;
        }
        case FunctionCode::WRITE_MULTIPLE_REGISTERS: {
            const uint16_t start_register = decoded[2] << 8 | decoded[3];
            const uint16_t length = decoded[4] << 8 | decoded[5];

            const size_t byte_count = decoded[6];
            if (byte_count != len - 7)
                break;
            
            var buf_index = 7;
            for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
                if (not holding_register_setters.has(reg)) 
                    continue;
                
                holding_register_setters[reg](decoded[buf_index] << 8 | decoded[buf_index + 1]);
                buf_index += 2;
            }

            response(decoded, 6);
            break;
        }
    }
}
