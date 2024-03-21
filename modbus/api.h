#ifndef PROJECT_MODBUS_API_H
#define PROJECT_MODBUS_API_H

#include "etl/async.h"
#include "etl/vector.h"
#include "etl/queue.h"
#include "etl/mutex.h"

namespace Project::modbus {
    enum FunctionCode {
        READ_COILS = 1,
        READ_DISCRETE_INPUTS = 2,
        READ_HOLDING_REGISTERS = 3,
        READ_INPUT_REGISTERS = 4,
        WRITE_SINGLE_COIL = 5,
        WRITE_SINGLE_REGISTER = 6,
        READ_EXCEPTION_STATUS = 7,
        DIAGNOSTIC = 8,
        WRITE_MULTIPLE_COILS = 15,
        WRITE_MULTIPLE_REGISTERS = 16,
    };
}

namespace Project::modbus::api {
    uint16_t crc(const uint8_t* data, size_t len);
    const uint8_t* decode(const uint8_t* data, size_t& len);

    struct Message {const uint8_t* data; size_t len; };
}

#endif