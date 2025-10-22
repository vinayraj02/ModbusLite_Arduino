#include "modbus_context.h"
//#include "modbus_core.c"
#include <string.h>
#include <Arduino.h>

int modbus_async_init(ModbusContext *ctx) {
    if (!ctx) return -1;
    memset(&ctx->async, 0, sizeof(ctx->async));
    ctx->async.state = MODBUS_STATE_IDLE;
    return 0;
}

int modbus_async_read_holding(ModbusContext *ctx, uint16_t addr, uint16_t qty, uint32_t timeout_ms, ModbusAsyncCallback cb) {
    if (!ctx) return -1;
    ctx->async.callback = cb;
    ctx->async.timeout_ms = timeout_ms;
    ctx->async.start_time = millis();
    ctx->async.state = MODBUS_STATE_REQUEST_SENT;

    modbus_build_read_holding_registers(&ctx->async.tx, ctx->unit_id, addr, qty);

    /* send immediately */
    if (ctx->mode == MODBUS_MODE_TCP) modbus_tcp_send(ctx, &ctx->async.tx);
    else modbus_rtu_send(ctx, &ctx->async.tx);

    ctx->async.state = MODBUS_STATE_WAITING_RESPONSE;
    return 0;
}

int modbus_async_poll(ModbusContext *ctx) {
    if (!ctx) return -1;
    if (ctx->async.state != MODBUS_STATE_WAITING_RESPONSE) return 0;

    int rc = -1;
    if (ctx->mode == MODBUS_MODE_TCP) rc = modbus_tcp_receive(ctx, &ctx->async.rx, 0);
    else rc = modbus_rtu_receive(ctx, &ctx->async.rx, 0);

    if (rc == 0) {
        ctx->async.state = MODBUS_STATE_COMPLETE;
        if (ctx->async.callback) ctx->async.callback(ctx, &ctx->async.rx, 0);
        return 0;
    }

    if (millis() - ctx->async.start_time > ctx->async.timeout_ms) {
        ctx->async.state = MODBUS_STATE_ERROR;
        if (ctx->async.callback) ctx->async.callback(ctx, NULL, -1);
    }
    return 0;
}

