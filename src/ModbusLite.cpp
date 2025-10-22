// ModbusLite.cpp
#include "ModbusLite.h"
#include <string.h>

ModbusLite::ModbusLite(const char* target, int param) : ctx(nullptr), userCb(nullptr) {
    ctx = modbus_connect(target, param);
    if (!ctx) {
        Serial.print("[ModbusLite] connect failed: ");
        Serial.println(target);
    } else {
        Serial.print("[ModbusLite] connected to ");
        Serial.println(target);
    }
}

ModbusLite::~ModbusLite() {
    close();
}

bool ModbusLite::connected() {
    return ctx != nullptr;
}

bool ModbusLite::readHoldingRegisters(uint16_t addr, uint16_t count, uint16_t* dest) {
    if (!ctx) return false;
    int rc = modbus_read_holding_registers(ctx, addr, count, dest);
    return rc >= 0;
}

bool ModbusLite::writeSingleRegister(uint16_t addr, uint16_t value) {
    if (!ctx) return false;
    int rc = modbus_write_single_register(ctx, addr, value);
    return rc == 0;
}

bool ModbusLite::readCoils(uint16_t addr, uint16_t count, uint8_t* dest) {
    if (!ctx) return false;
    int rc = modbus_read_coils(ctx, addr, count, dest);
    return rc >= 0;
}

bool ModbusLite::writeSingleCoil(uint16_t addr, bool state) {
    if (!ctx) return false;
    int rc = modbus_write_single_coil(ctx, addr, state);
    return rc == 0;
}

void ModbusLite::close() {
    if (ctx) {
        modbus_close(ctx);
        ctx = nullptr;
        Serial.println("[ModbusLite] closed");
    }
}

/* Async wrapper */
bool ModbusLite::beginReadAsync(uint16_t addr, uint16_t count, uint32_t timeout_ms,
                                void (*cb)(ModbusLite&, uint16_t*, int)) {
    if (!ctx) return false;
    userCb = cb;
    return modbus_async_read_holding(ctx, addr, count, timeout_ms, internalAsyncCb) == 0;
}

void ModbusLite::poll() {
    if (ctx) modbus_async_poll(ctx);
}

/* static internal adapter */
void ModbusLite::internalAsyncCb(ModbusContext *c, const ModbusFrame *res, int status) {
    ModbusLite *self = (ModbusLite*)c->user_data;
    if (!self) return;
    if (status != 0 || !res) {
        if (self->userCb) {
            self->userCb(*self, NULL, -1);
        }
        return;
    }
    uint16_t buffer[MODBUS_MAX_REGISTERS];
    int bytes = res->pdu.data[0];
    int regs = bytes / 2;
    for (int i = 0; i < regs; ++i) {
        buffer[i] = (res->pdu.data[1 + i*2] << 8) | res->pdu.data[2 + i*2];
    }
    if (self->userCb) self->userCb(*self, buffer, regs);
}
