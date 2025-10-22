// core/modbus.h
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MODBUS_MAX_PDU 256
#define MODBUS_MAX_ADU 260
#define MODBUS_MAX_REGISTERS 125
#define MODBUS_MAX_COILS 1024

/* Function codes */
#define MODBUS_FC_READ_COILS            0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS  0x02
#define MODBUS_FC_READ_HOLD_REGS        0x03
#define MODBUS_FC_READ_INPUT_REGS       0x04
#define MODBUS_FC_WRITE_SINGLE_COIL     0x05
#define MODBUS_FC_WRITE_SINGLE_REG      0x06
#define MODBUS_FC_WRITE_MULT_COILS      0x0F
#define MODBUS_FC_WRITE_MULT_REGS       0x10

typedef struct {
    uint8_t function_code;
    uint8_t data[MODBUS_MAX_PDU];
    size_t  data_len;
} ModbusPDU;

typedef struct {
    uint8_t unit_id;
    /* For TCP MBAP we won't pack it fully — minimal needs for Arduino wrapper */
} ModbusMBAP;

typedef struct {
    uint8_t adu[MODBUS_MAX_ADU + 8];
    size_t  adu_len;

    ModbusMBAP header;
    ModbusPDU pdu;
} ModbusFrame;

/* CRC */
uint16_t modbus_crc16(const uint8_t *buf, size_t len);

/* PDU builder helpers (small subset used by examples) */
int modbus_build_read_holding_registers(ModbusFrame *frame, uint8_t unit,
                                        uint16_t start_addr, uint16_t quantity);
int modbus_build_write_single_register(ModbusFrame *frame, uint8_t unit,
                                       uint16_t reg_addr, uint16_t value);
int modbus_build_read_coils(ModbusFrame *frame, uint8_t unit,
                            uint16_t start_addr, uint16_t quantity);
int modbus_build_write_single_coil(ModbusFrame *frame, uint8_t unit,
                                   uint16_t coil_addr, bool state);

#ifdef __cplusplus
}
#endif
