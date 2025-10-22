// core/modbus_register_map.c
#include "modbus_context.h"
#include <string.h>

static uint16_t holding_regs[MODBUS_MAX_REGISTERS];
static uint16_t input_regs[MODBUS_MAX_REGISTERS];
static uint8_t coils[MODBUS_MAX_COILS / 8];
static uint8_t discrete_inputs[MODBUS_MAX_COILS / 8];

void modbus_register_map_init(ModbusContext *ctx) {
    (void)ctx;
    memset(holding_regs, 0, sizeof(holding_regs));
    memset(input_regs, 0, sizeof(input_regs));
    memset(coils, 0, sizeof(coils));
    memset(discrete_inputs, 0, sizeof(discrete_inputs));
}

/* Simple helpers */
void modbus_set_register(ModbusContext *ctx, uint16_t addr, uint16_t value) {
    (void)ctx;
    if (addr < MODBUS_MAX_REGISTERS) holding_regs[addr] = value;
}

uint16_t modbus_get_register(ModbusContext *ctx, uint16_t addr) {
    (void)ctx;
    if (addr < MODBUS_MAX_REGISTERS) return holding_regs[addr];
    return 0;
}
