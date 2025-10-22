#include <Ethernet.h>
#include <ModbusLite.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0x50, 0xA9, 0xCC };
IPAddress localIp(192, 168, 1, 175);

EthernetServer server(502);
EthernetClient client;

// Each slave maintains its own context
ModbusContext slaveCtx;

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("\n=== Opta Modbus TCP Slave ===");

  Ethernet.begin(mac, localIp);
  delay(2000);  // Let PHY and lwIP settle
  Serial.print("Local IP: ");
  Serial.println(Ethernet.localIP());

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("[ERROR] Ethernet link is down! Check cable.");
    while (1) delay(500);
  }

  // Start listening on port 502
  server.begin();
  Serial.println("Listening for Modbus TCP master connections on port 502...");

  // Initialize Modbus context
  memset(&slaveCtx, 0, sizeof(slaveCtx));
  slaveCtx.mode = MODBUS_MODE_TCP;
  slaveCtx.unit_id = 1;  // default Modbus slave ID
  modbus_register_map_init(&slaveCtx);

  // Initialize some demo registers
  for (int i = 0; i < 10; i++)
    modbus_set_register(&slaveCtx, i, i * 10);
}

void handleClient() {
  if (!client || !client.connected()) {
    EthernetClient newClient = server.available();
    if (newClient) {
      client.stop();  // close old connection
      client = newClient;
      Serial.print("[Client] Connected from ");
      Serial.println(client.remoteIP());
    }
    return;
  }

  if (client.available() < 8) return;  // minimal MBAP header size

  // Read request into buffer
  uint8_t reqBuf[MODBUS_MAX_ADU + 8];
  int len = client.read(reqBuf, sizeof(reqBuf));
  if (len < 8) return;

  // Decode Modbus frame
  ModbusFrame req = {0}, res = {0};
  req.adu_len = len;
  memcpy(req.adu, reqBuf, len);

  // Extract fields safely
  req.header.unit_id = reqBuf[6];
  req.pdu.function_code = reqBuf[7];
  req.pdu.data_len = len - 8;
  memcpy(req.pdu.data, &reqBuf[8], req.pdu.data_len);

  // Process request
  if (modbus_handle_request(&slaveCtx, &req, &res) == 0) {
    uint8_t outBuf[MODBUS_MAX_ADU + 8];
    size_t pos = 0;

    // Build MBAP header
    outBuf[pos++] = reqBuf[0]; // Transaction ID high
    outBuf[pos++] = reqBuf[1]; // Transaction ID low
    outBuf[pos++] = 0x00;      // Protocol high
    outBuf[pos++] = 0x00;      // Protocol low
    uint16_t lenOut = res.pdu.data_len + 2; // unit_id + function + data
    outBuf[pos++] = (lenOut >> 8) & 0xFF;
    outBuf[pos++] = lenOut & 0xFF;

    // Modbus PDU
    outBuf[pos++] = res.header.unit_id;
    outBuf[pos++] = res.pdu.function_code;
    memcpy(&outBuf[pos], res.pdu.data, res.pdu.data_len);
    pos += res.pdu.data_len;

    client.write(outBuf, pos);
    client.flush();
  }
}

void loop() {
  handleClient();

  // Simulate data update periodically
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    uint16_t val = modbus_get_register(&slaveCtx, 0);
    modbus_set_register(&slaveCtx, 0, val + 1);
  }
}
