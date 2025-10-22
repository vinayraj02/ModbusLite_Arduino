// core/modbus_server.c
#include "modbus_context.h"
#include "modbus.h"
#include <string.h>

/* Very small request handler: only covers common FCs used in tests */
int modbus_handle_request(ModbusContext *ctx, const ModbusFrame *req, ModbusFrame *res) {
    if (!ctx || !req || !res) return -1;
    memset(res, 0, sizeof(*res));
    uint8_t fc = req->pdu.function_code;

    if (fc == MODBUS_FC_READ_HOLD_REGS) {
        uint16_t start = (req->pdu.data[0] << 8) | req->pdu.data[1];
        uint16_t qty   = (req->pdu.data[2] << 8) | req->pdu.data[3];
        /* build response PDU: byte count + registers */
        res->pdu.function_code = fc;
        res->pdu.data[0] = (uint8_t)(qty * 2);
        for (int i = 0; i < qty && i < MODBUS_MAX_REGISTERS; ++i) {
            uint16_t v = modbus_get_register(ctx, start + i);
            res->pdu.data[1 + i*2] = (v >> 8) & 0xFF;
            res->pdu.data[2 + i*2] = v & 0xFF;
        }
        res->pdu.data_len = 1 + (qty * 2);
        res->header.unit_id = req->header.unit_id;
        return 0;
    } else if (fc == MODBUS_FC_WRITE_SINGLE_REG) {
        uint16_t addr = (req->pdu.data[0] << 8) | req->pdu.data[1];
        uint16_t val  = (req->pdu.data[2] << 8) | req->pdu.data[3];
        modbus_set_register(ctx, addr, val);
        /* echo request as per spec */
        memcpy(res->pdu.data, req->pdu.data, 4);
        res->pdu.function_code = fc;
        res->pdu.data_len = 4;
        res->header.unit_id = req->header.unit_id;
        return 0;
    } else if (fc == MODBUS_FC_READ_COILS) {
        uint16_t start = (req->pdu.data[0] << 8) | req->pdu.data[1];
        uint16_t qty   = (req->pdu.data[2] << 8) | req->pdu.data[3];
        /* return minimal response: byte count + packed coil bytes (zeroed) */
        res->pdu.function_code = fc;
        uint8_t byteCount = (qty + 7) / 8;
        res->pdu.data[0] = byteCount;
        for (int i = 0; i < byteCount; ++i) res->pdu.data[1 + i] = 0x00;
        res->pdu.data_len = 1 + byteCount;
        res->header.unit_id = req->header.unit_id;
        return 0;
    }

    /* not supported: exception response could be built */
    return -2;
}
