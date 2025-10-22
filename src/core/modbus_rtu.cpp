// core/modbus_rtu.cpp
#include "modbus_context.h"
#include "modbus.h"
#include <Arduino.h>
#include <string.h>

#ifndef MODBUS_RTUSERIAL
#define MODBUS_RTUSERIAL Serial1
#endif

int modbus_rtu_open_ctx(ModbusContext *ctx, const char *device, int baud) {
    (void)ctx;
    (void)device;
    MODBUS_RTUSERIAL.begin(baud);
    delay(50);
    return 0;
}

int modbus_rtu_close_ctx(ModbusContext *ctx) {
    (void)ctx;
    MODBUS_RTUSERIAL.end();
    return 0;
}

int modbus_rtu_send(ModbusContext *ctx, const ModbusFrame *frame) {
    (void)ctx;
    /* Format RTU ADU: [Unit][PDU...] + CRClo CRChi */
    uint8_t buf[MODBUS_MAX_ADU + 4];
    size_t pos = 0;
    buf[pos++] = frame->header.unit_id;
    buf[pos++] = frame->pdu.function_code;
    if (frame->pdu.data_len) {
        memcpy(&buf[pos], frame->pdu.data, frame->pdu.data_len);
        pos += frame->pdu.data_len;
    }
    uint16_t crc = modbus_crc16(buf, pos);
    buf[pos++] = crc & 0xFF;
    buf[pos++] = (crc >> 8) & 0xFF;
    MODBUS_RTUSERIAL.write(buf, pos);
    MODBUS_RTUSERIAL.flush();
    return 0;
}

int modbus_rtu_receive(ModbusContext *ctx, ModbusFrame *frame, int timeout_ms) {
    (void)ctx;
    unsigned long start = millis();
    size_t idx = 0;
    while (millis() - start < (unsigned long)timeout_ms) {
        while (MODBUS_RTUSERIAL.available()) {
            int b = MODBUS_RTUSERIAL.read();
            if (b < 0) continue;
            frame->adu[idx++] = (uint8_t)b;
            if (idx >= 5) {
                /* Check CRC on collected bytes: must have at least 3 bytes payload + CRC */
                uint16_t crc_calc = modbus_crc16(frame->adu, idx - 2);
                uint16_t crc_recv = (uint16_t)frame->adu[idx - 2] | ((uint16_t)frame->adu[idx - 1] << 8);
                if (crc_calc == crc_recv) {
                    frame->adu_len = idx;
                    frame->header.unit_id = frame->adu[0];
                    frame->pdu.function_code = frame->adu[1];
                    frame->pdu.data_len = (idx - 4); /* unit + func + crc2 */
                    if (frame->pdu.data_len > 0) {
                        memcpy(frame->pdu.data, &frame->adu[2], frame->pdu.data_len);
                    }
                    return 0;
                }
            }
            if (idx >= sizeof(frame->adu)) break;
        }
    }
    return -1;
}
