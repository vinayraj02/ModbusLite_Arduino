#include <Ethernet.h>
#include <ModbusLite.h>

byte mac[] = {0xA8, 0x61, 0x0A, 0x50, 0xA3, 0x1C};
IPAddress localIp(192, 168, 1, 177);

ModbusLite* modbus = nullptr;


void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("\n=== Opta Modbus TCP Master ===");

  // Start Ethernet and wait until link is actually ready
  Ethernet.begin(mac, localIp);
  delay(2000);  // allow PHY and lwIP to stabilize

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("[ERROR] Ethernet cable not connected.");
    while (1) delay(1000);
  }

  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());

  // Only now safely connect Modbus
  Serial.println("Connecting to Modbus slave...");
  modbus = new ModbusLite("192.168.1.175", 502);
  delay(500);

  if (!modbus->connected()) {
    Serial.println("[ERROR] Modbus connection failed!");
    while (1) delay(1000);
  }

  Serial.println("[OK] Connected to Modbus slave.");

  // Read registers
  uint16_t regs[5] = {0};
  if (modbus->readHoldingRegisters(0, 5, regs)) {
    Serial.println("Read Holding Registers:");
    for (int i = 0; i < 5; ++i) {
      Serial.print("  Reg[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(regs[i]);
    }
  } else {
    Serial.println("[WARN] Read failed!");
  }

  // Write example register
  Serial.println("Writing Reg[10] = 1234...");
  if (modbus->writeSingleRegister(10, 1234)) {
    Serial.println("[OK] Write success.");
  } else {
    Serial.println("[WARN] Write failed.");
  }
}

void loop() {
  // keep polling Modbus in case you add async later
  modbus->poll();
  delay(5);  // let mbed breathe (prevents watchdog reset)
}
