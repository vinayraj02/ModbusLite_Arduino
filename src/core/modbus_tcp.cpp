// core/modbus_tcp.cpp
#include "modbus_context.h"
#include "modbus.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <string.h>

static EthernetServer *s_server = NULL;
static EthernetClient *alloc_client(EthernetClient *c) {
    /* helper to return pointer for storing in ctx->transport */
    return c;
}

/* Initialize context as TCP client (connect to ip:port) */
int modbus_tcp_connect_ctx(ModbusContext *ctx, const char *ip, int port) {
    if (!ctx) return -1;
    IPAddress remote;
    if (!remote.fromString(ip)) return -1;
    EthernetClient *client = new EthernetClient();
    if (!client->connect(remote, port)) {
        delete client;
        return -2;
    }
    ctx->transport = (void*)client;
    strncpy(ctx->ip, ip, sizeof(ctx->ip)-1);
    ctx->port = port;
    ctx->mode = MODBUS_MODE_TCP;
    return 0;
}

int modbus_tcp_close_ctx(ModbusContext *ctx) {
    if (!ctx || !ctx->transport) return -1;
    EthernetClient *client = (EthernetClient*)ctx->transport;
    client->stop();
    delete client;
    ctx->transport = NULL;
    return 0;
}

int modbus_tcp_send(ModbusContext *ctx, const ModbusFrame *frame) {
    if (!ctx || !ctx->transport) return -1;
    EthernetClient *client = (EthernetClient*)ctx->transport;
    if (!client->connected()) return -2;
    /* For Modbus TCP: send ADU = MBAP(7) + PDU. We'll craft simple MBAP: tid=0 pid=0 len=pdu_len+1 unitid */
    uint8_t out[MODBUS_MAX_ADU + 8];
    size_t pos = 0;
    /* Transaction ID */
    out[pos++] = 0x00;
    out[pos++] = 0x00;
    /* Protocol ID */
    out[pos++] = 0x00;
    out[pos++] = 0x00;
    /* Length (pdu_len + unit id) */
    uint16_t len = (uint16_t)(frame->pdu.data_len + 1);
    out[pos++] = (len >> 8) & 0xFF;
    out[pos++] = (len) & 0xFF;
    /* Unit ID */
    out[pos++] = frame->header.unit_id;
    /* Function code + data */
    out[pos++] = frame->pdu.function_code;
    if (frame->pdu.data_len) {
        memcpy(&out[pos], frame->pdu.data, frame->pdu.data_len);
        pos += frame->pdu.data_len;
    }
    client->write(out, pos);
    client->flush();
    return 0;
}

int modbus_tcp_receive(ModbusContext *ctx, ModbusFrame *frame, int timeout_ms) {
    if (!ctx || !ctx->transport) return -1;
    EthernetClient *client = (EthernetClient*)ctx->transport;
    unsigned long start = millis();
    size_t idx = 0;
    while (millis() - start < (unsigned long)timeout_ms) {
        while (client->available()) {
            int b = client->read();
            if (b < 0) continue;
            frame->adu[idx++] = (uint8_t)b;
            if (idx >= 8) {
                /* parse MBAP -> pdu start at offset 7 */
                uint16_t mbap_len = ((uint16_t)frame->adu[4] << 8) | frame->adu[5];
                size_t expected = 6 + mbap_len; /* full MBAP + unit/pdu */
                if (idx >= expected) {
                    /* fill frame */
                    frame->adu_len = expected;
                    frame->header.unit_id = frame->adu[6];
                    frame->pdu.function_code = frame->adu[7];
                    frame->pdu.data_len = expected - 8;
                    if (frame->pdu.data_len > 0) memcpy(frame->pdu.data, &frame->adu[8], frame->pdu.data_len);
                    return 0;
                }
            }
            if (idx >= sizeof(frame->adu)) break;
        }
    }
    return -1;
}

/* Simple server: listen on port and accept first client. Call once to start server. */
int modbus_tcp_listen_start(int port) {
    if (s_server) return 0;
    s_server = new EthernetServer(port);
    s_server->begin();
    return 0;
}

/* Accept and assign to ctx->transport if client available */
int modbus_tcp_accept_into_ctx(ModbusContext *ctx) {
    if (!s_server) return -1;
    EthernetClient client = s_server->available();
    if (!client) return -2;
    /* move it to heap */
    EthernetClient *heapClient = new EthernetClient(client);
    ctx->transport = heapClient;
    ctx->mode = MODBUS_MODE_TCP;
    return 0;
}
