#include <ModbusLite.h>

#define RS485_SERIAL Serial1
#define RS485_BAUD   9600
#define RS485_DE_PIN D9  // RS485 driver enable pin

ModbusLite* modbus = nullptr;  // pointer, not global instance

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\n=== Opta Modbus RTU Master (Safe Init) ===");

  // Setup RS485 transceiver
  pinMode(RS485_DE_PIN, OUTPUT);
  digitalWrite(RS485_DE_PIN, LOW);  // RX mode initially

  RS485_SERIAL.begin(RS485_BAUD, SERIAL_8N1);
  delay(500);

  Serial.println("[INFO] RS485 serial initialized.");

  // Construct Modbus object *after* serial is up
  // This avoids static/global constructor crash
  modbus = new ModbusLite("rtu-safe", RS485_BAUD);
  if (!modbus) {
    Serial.println("[FATAL] Failed to allocate ModbusLite!");
    while (1);
  }

  Serial.println("[OK] ModbusLite object created safely.");

  uint16_t regs[4] = {0};

  // Try to read holding registers
  Serial.println("Attempting Modbus RTU read...");
  bool success = modbus->readHoldingRegisters(0, 4, regs);

  if (success) {
    Serial.println("Read Holding Registers:");
    for (int i = 0; i < 4; ++i) {
      Serial.print("  Reg[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(regs[i]);
    }
  } else {
    Serial.println("[WARN] Read failed. Check RS485 wiring or slave address.");
  }

  // Optional: write test value
  delay(1000);
  Serial.println("Writing Reg[10] = 555...");
  if (modbus->writeSingleRegister(10, 555)) {
    Serial.println("[OK] Write success.");
  } else {
    Serial.println("[WARN] Write failed.");
  }
}

void loop() {
  if (modbus) modbus->poll();
  delay(5);
}
