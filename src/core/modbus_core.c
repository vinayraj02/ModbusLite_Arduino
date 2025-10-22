// core/modbus_core.c
#include "modbus.h"
#include <string.h>

// Conditional include: only for Linux builds, not Arduino
#ifndef ARDUINO
#include <arpa/inet.h> // for htons on desktop builds
#endif

uint16_t modbus_crc16(const uint8_t *buf, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)buf[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}

static void build_mbap(ModbusFrame *frame, uint8_t unit, uint16_t pdu_len)
{
    frame->header.unit_id = unit;
}

/* Build Read Holding Registers request (PDU) */
int modbus_build_read_holding_registers(ModbusFrame *frame, uint8_t unit,
                                        uint16_t start_addr, uint16_t quantity)
{
    if (!frame) return -1;
    memset(frame, 0, sizeof(*frame));
    frame->pdu.function_code = MODBUS_FC_READ_HOLD_REGS;
    frame->pdu.data[0] = (start_addr >> 8) & 0xFF;
    frame->pdu.data[1] = start_addr & 0xFF;
    frame->pdu.data[2] = (quantity >> 8) & 0xFF;
    frame->pdu.data[3] = quantity & 0xFF;
    frame->pdu.data_len = 4;
    build_mbap(frame, unit, frame->pdu.data_len + 1);
    return 0;
}

/* Build Write Single Register request */
int modbus_build_write_single_register(ModbusFrame *frame, uint8_t unit,
                                       uint16_t reg_addr, uint16_t value)
{
    if (!frame) return -1;
    memset(frame, 0, sizeof(*frame));
    frame->pdu.function_code = MODBUS_FC_WRITE_SINGLE_REG;
    frame->pdu.data[0] = (reg_addr >> 8) & 0xFF;
    frame->pdu.data[1] = reg_addr & 0xFF;
    frame->pdu.data[2] = (value >> 8) & 0xFF;
    frame->pdu.data[3] = value & 0xFF;
    frame->pdu.data_len = 4;
    build_mbap(frame, unit, frame->pdu.data_len + 1);
    return 0;
}

/* Read Coils */
int modbus_build_read_coils(ModbusFrame *frame, uint8_t unit,
                            uint16_t start_addr, uint16_t quantity)
{
    if (!frame) return -1;
    memset(frame, 0, sizeof(*frame));
    frame->pdu.function_code = MODBUS_FC_READ_COILS;
    frame->pdu.data[0] = (start_addr >> 8) & 0xFF;
    frame->pdu.data[1] = start_addr & 0xFF;
    frame->pdu.data[2] = (quantity >> 8) & 0xFF;
    frame->pdu.data[3] = quantity & 0xFF;
    frame->pdu.data_len = 4;
    build_mbap(frame, unit, frame->pdu.data_len + 1);
    return 0;
}

/* Write Single Coil */
int modbus_build_write_single_coil(ModbusFrame *frame, uint8_t unit,
                                   uint16_t coil_addr, bool state)
{
    if (!frame) return -1;
    memset(frame, 0, sizeof(*frame));
    frame->pdu.function_code = MODBUS_FC_WRITE_SINGLE_COIL;
    frame->pdu.data[0] = (coil_addr >> 8) & 0xFF;
    frame->pdu.data[1] = coil_addr & 0xFF;
    frame->pdu.data[2] = state ? 0xFF : 0x00;
    frame->pdu.data[3] = 0x00;
    frame->pdu.data_len = 4;
    build_mbap(frame, unit, frame->pdu.data_len + 1);
    return 0;
}
