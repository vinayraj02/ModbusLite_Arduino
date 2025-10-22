// ModbusLite.h
#pragma once
#include <Arduino.h>
extern "C" {
  #include "core/modbus_context.h"
}

class ModbusLite {
public:
    ModbusLite(const char* target, int param);
    ~ModbusLite();

    bool connected();
    bool readHoldingRegisters(uint16_t addr, uint16_t count, uint16_t* dest);
    bool writeSingleRegister(uint16_t addr, uint16_t value);
    bool readCoils(uint16_t addr, uint16_t count, uint8_t* dest);
    bool writeSingleCoil(uint16_t addr, bool state);

    /* Async support */
    bool beginReadAsync(uint16_t addr, uint16_t count, uint32_t timeout_ms,
                        void (*cb)(ModbusLite&, uint16_t*, int));
    void poll();

    void close();

private:
    ModbusContext *ctx;
    void (*userCb)(ModbusLite&, uint16_t*, int);
    static void internalAsyncCb(ModbusContext *c, const ModbusFrame *res, int status);
};
