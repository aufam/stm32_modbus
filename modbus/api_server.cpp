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

fun modbus::api::Server::create_response(etl::Vector<uint8_t> data) -> etl::Vector<uint8_t> {
    switch (data[1]) {
        case FunctionCode::READ_COILS: 
        case FunctionCode::READ_DISCRETE_INPUTS: {
            const uint16_t start_register = data[2] << 8 | data[3];
            const uint16_t length = data[4] << 8 | data[5];

            var buffer = etl::vector_reserve<uint8_t>(2 + 1 + length + 2);
            buffer.append(data[0]); // address
            buffer.append(data[1]); // function code
            buffer.append(length); // length

            val &getter = data[1] == FunctionCode::READ_COILS ? coil_getters : discrete_input_getters;
            int cnt = 0;

            for (val i in etl::range<uint16_t>(start_register, start_register + length)) {
                if (getter.has(i)) {
                    buffer.append(getter[i]());
                    ++cnt;
                }
            }

            buffer[2] = cnt; // actual length
            return buffer;
        }
        case FunctionCode::READ_HOLDING_REGISTERS:
        case FunctionCode::READ_INPUT_REGISTERS: {
            const uint16_t start_register = data[2] << 8 | data[3];
            const uint16_t length = data[4] << 8 | data[5];

            var buffer = etl::vector_reserve<uint8_t>(2 + 1 + length * 2 + 2);
            buffer.append(data[0]); // address
            buffer.append(data[1]); // function code
            buffer.append(length * 2); // length

            val &getter = data[1] == FunctionCode::READ_HOLDING_REGISTERS ? holding_register_getters : analog_input_getters;
            int cnt = 0;

            for (val i in etl::range<uint16_t>(start_register, start_register + length)) {
                if (getter.has(i)) {
                    val value = getter[i]();
                    buffer.append((value >> 8) & 0xFF);
                    buffer.append((value >> 0) & 0xFF);
                    cnt += 2;
                }
            }

            buffer[2] = cnt; // actual length
            return buffer;
        }
        case FunctionCode::WRITE_SINGLE_COIL: {
            const uint16_t start_register = data[2] << 8 | data[3];
            const uint16_t value = data[4] << 8 | data[5];

            if (value == 0xFF00 and coil_setters.has(start_register)) {
                coil_setters[start_register](true);
            }
            if (value == 0x0000 and coil_setters.has(start_register)) {
                coil_setters[start_register](false);
            }

            return data;
        }
        case FunctionCode::WRITE_SINGLE_REGISTER: {
            const uint16_t start_register = data[2] << 8 | data[3];
            const uint16_t value = data[4] << 8 | data[5];

            if (holding_register_setters.has(start_register)) {
                holding_register_setters[start_register](value);
            }

            return data;
        }
        case FunctionCode::READ_EXCEPTION_STATUS: {
            var buffer = etl::vector_reserve<uint8_t>(2 + 1 + 2);
            buffer.append(data[0]);
            buffer.append(data[1]);
            buffer.append(exception_status_getter ? exception_status_getter() : 0x00);

            return buffer;
        }
        case FunctionCode::DIAGNOSTIC: {
            val sub_function = data[2];
            val diagnose = diagnostic_getters ? diagnostic_getters[sub_function]() : etl::vector<uint8_t>();
            
            var buffer = etl::vector_reserve<uint8_t>(2 + diagnose.len() + 2);
            buffer.append(data[0]);
            buffer.append(data[1]);
            buffer.append(diagnose);

            return buffer;
        }
        case FunctionCode::WRITE_MULTIPLE_COILS: {
            const uint16_t start_register = data[2] << 8 | data[3];
            const uint16_t length = data[4] << 8 | data[5];

            const size_t byte_count = data[6];
            if (byte_count != data.len() - 7)
                return data;
            
            int bit_index = 0;
            var buf_index = 7;
            for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
                if (not coil_setters.has(reg)) 
                    continue;
                
                coil_setters[reg]((1 << bit_index++) & data[buf_index]);
                if (bit_index == 8) {
                    bit_index = 0;
                    ++buf_index;
                }
            }

            return data;
        }
        case FunctionCode::WRITE_MULTIPLE_REGISTERS: {
            const uint16_t start_register = data[2] << 8 | data[3];
            const uint16_t length = data[4] << 8 | data[5];

            const size_t byte_count = data[6];
            if (byte_count != data.len() - 7)
                return data;
            
            var buf_index = 7;
            for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
                if (not holding_register_setters.has(reg)) 
                    continue;
                
                holding_register_setters[reg](data[buf_index] << 8 | data[buf_index + 1]);
                buf_index += 2;
            }

            return data;
        }
    }

    return data;
}
