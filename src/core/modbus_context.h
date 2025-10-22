// core/modbus_context.h
#pragma once
#include "modbus.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ModbusContext ModbusContext;

/* Async callback signature */
typedef void (*ModbusAsyncCallback)(ModbusContext *ctx, const ModbusFrame *res, int status);

typedef enum {
    MODBUS_STATE_IDLE = 0,
    MODBUS_STATE_REQUEST_SENT,
    MODBUS_STATE_WAITING_RESPONSE,
    MODBUS_STATE_COMPLETE,
    MODBUS_STATE_ERROR
} ModbusAsyncState_t;

typedef struct {
    ModbusFrame tx;
    ModbusFrame rx;
    uint32_t start_time;
    uint32_t timeout_ms;
    ModbusAsyncState_t state;
    ModbusAsyncCallback callback;
} ModbusAsyncControl;

typedef enum {
    MODBUS_MODE_NONE = 0,
    MODBUS_MODE_TCP,
    MODBUS_MODE_RTU
} ModbusMode;

struct ModbusContext {
    ModbusMode mode;
    int fd; /* not used on Arduino but kept for cross-compat */
    char ip[64];
    int port;
    char device[64];
    int baud;
    uint8_t unit_id;

    /* Async state */
    ModbusAsyncControl async;

    /* transport-specific client pointer (EthernetClient*, etc.) stored as void* */
    void *transport;

    /* user pointer for wrappers */
    void *user_data;
};

/* Connection helpers */
ModbusContext *modbus_connect(const char *target, int param); /* auto select: ip or device */
void modbus_close(ModbusContext *ctx);

/* High-level operations (blocking) */
int modbus_read_holding_registers(ModbusContext *ctx, uint16_t addr, uint16_t count, uint16_t *dest);
int modbus_write_single_register(ModbusContext *ctx, uint16_t addr, uint16_t value);
int modbus_read_coils(ModbusContext *ctx, uint16_t addr, uint16_t count, uint8_t *dest);
int modbus_write_single_coil(ModbusContext *ctx, uint16_t addr, bool state);

/* transport wrappers that accept context */
int modbus_tcp_connect_ctx(ModbusContext *ctx, const char *ip, int port);
int modbus_tcp_close_ctx(ModbusContext *ctx);
int modbus_tcp_send(ModbusContext *ctx, const ModbusFrame *frame);
int modbus_tcp_receive(ModbusContext *ctx, ModbusFrame *frame, int timeout_ms);

int modbus_rtu_open_ctx(ModbusContext *ctx, const char *device, int baud);
int modbus_rtu_close_ctx(ModbusContext *ctx);
int modbus_rtu_send(ModbusContext *ctx, const ModbusFrame *frame);
int modbus_rtu_receive(ModbusContext *ctx, ModbusFrame *frame, int timeout_ms);

/* async helpers */
int modbus_async_init(ModbusContext *ctx);
int modbus_async_read_holding(ModbusContext *ctx, uint16_t addr, uint16_t qty, uint32_t timeout_ms, ModbusAsyncCallback cb);
int modbus_async_poll(ModbusContext *ctx);

/* Register map (slave storage) */
void modbus_register_map_init(ModbusContext *ctx);
void modbus_set_register(ModbusContext *ctx, uint16_t addr, uint16_t value);
uint16_t modbus_get_register(ModbusContext *ctx, uint16_t addr);

/* Slave handler: process req->pdu and produce res->pdu then use context transport to send */
int modbus_handle_request(ModbusContext *ctx, const ModbusFrame *req, ModbusFrame *res);

#ifdef __cplusplus
}
#endif
