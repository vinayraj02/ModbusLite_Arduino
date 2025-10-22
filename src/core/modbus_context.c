// core/modbus_context.c
#include "modbus_context.h"
#include "modbus.h"
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

/* auto-select connect: if target starts with '/' treat as device (RTU) else IP string => TCP */
ModbusContext *modbus_connect(const char *target, int param) {
    ModbusContext *ctx = (ModbusContext*)calloc(1, sizeof(ModbusContext));
    if (!ctx) return NULL;
    ctx->unit_id = 1;

    if (target && target[0] == '/') {
        /* RTU */
        ctx->mode = MODBUS_MODE_RTU;
        strncpy(ctx->device, target, sizeof(ctx->device)-1);
        ctx->baud = param;
        if (modbus_rtu_open_ctx(ctx, target, param) < 0) {
            free(ctx); return NULL;
        }
        return ctx;
    } else {
        /* TCP */
        ctx->mode = MODBUS_MODE_TCP;
        strncpy(ctx->ip, target ? target : "0.0.0.0", sizeof(ctx->ip)-1);
        ctx->port = param;
        if (modbus_tcp_connect_ctx(ctx, ctx->ip, ctx->port) != 0) {
            free(ctx); return NULL;
        }
        modbus_async_init(ctx);
        return ctx;
    }
}

void modbus_close(ModbusContext *ctx) {
    if (!ctx) return;
    if (ctx->mode == MODBUS_MODE_TCP) {
        modbus_tcp_close_ctx(ctx);
    } else if (ctx->mode == MODBUS_MODE_RTU) {
        modbus_rtu_close_ctx(ctx);
    }
    free(ctx);
}

/* Blocking read holding registers (master) */
int modbus_read_holding_registers(ModbusContext *ctx, uint16_t addr, uint16_t count, uint16_t *dest) {
    if (!ctx || !dest) return -1;
    ModbusFrame req, res;
    modbus_build_read_holding_registers(&req, ctx->unit_id, addr, count);

    if (ctx->mode == MODBUS_MODE_TCP) {
        if (modbus_tcp_send(ctx, &req) < 0) return -1;
        if (modbus_tcp_receive(ctx, &res, 2000) < 0) return -1;
    } else {
        if (modbus_rtu_send(ctx, &req) < 0) return -1;
        if (modbus_rtu_receive(ctx, &res, 2000) < 0) return -1;
    }

    if (res.pdu.function_code != MODBUS_FC_READ_HOLD_REGS) return -2;
    int bytes = res.pdu.data[0];
    int regs = bytes / 2;
    for (int i = 0; i < regs && i < count; ++i) {
        dest[i] = ((uint16_t)res.pdu.data[1 + i*2] << 8) | res.pdu.data[2 + i*2];
    }
    return regs;
}

int modbus_write_single_register(ModbusContext *ctx, uint16_t addr, uint16_t value) {
    if (!ctx) return -1;
    ModbusFrame req, res;
    modbus_build_write_single_register(&req, ctx->unit_id, addr, value);

    if (ctx->mode == MODBUS_MODE_TCP) {
        if (modbus_tcp_send(ctx, &req) < 0) return -1;
        if (modbus_tcp_receive(ctx, &res, 2000) < 0) return -1;
    } else {
        if (modbus_rtu_send(ctx, &req) < 0) return -1;
        if (modbus_rtu_receive(ctx, &res, 2000) < 0) return -1;
    }
    return 0;
}

int modbus_read_coils(ModbusContext *ctx, uint16_t addr, uint16_t count, uint8_t *dest) {
    if (!ctx || !dest) return -1;
    ModbusFrame req, res;
    modbus_build_read_coils(&req, ctx->unit_id, addr, count);
    if (ctx->mode == MODBUS_MODE_TCP) {
        if (modbus_tcp_send(ctx, &req) < 0) return -1;
        if (modbus_tcp_receive(ctx, &res, 2000) < 0) return -1;
    } else {
        if (modbus_rtu_send(ctx, &req) < 0) return -1;
        if (modbus_rtu_receive(ctx, &res, 2000) < 0) return -1;
    }
    /* parse coil bytes */
    if (res.pdu.function_code != MODBUS_FC_READ_COILS) return -2;
    uint8_t byteCount = res.pdu.data[0];
    for (int i = 0; i < byteCount; ++i) dest[i] = res.pdu.data[1 + i];
    return byteCount * 8;
}

int modbus_write_single_coil(ModbusContext *ctx, uint16_t addr, bool state) {
    if (!ctx) return -1;
    ModbusFrame req, res;
    modbus_build_write_single_coil(&req, ctx->unit_id, addr, state);
    if (ctx->mode == MODBUS_MODE_TCP) {
        if (modbus_tcp_send(ctx, &req) < 0) return -1;
        if (modbus_tcp_receive(ctx, &res, 2000) < 0) return -1;
    } else {
        if (modbus_rtu_send(ctx, &req) < 0) return -1;
        if (modbus_rtu_receive(ctx, &res, 2000) < 0) return -1;
    }
    return 0;
}
