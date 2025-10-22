# ModbusLite for Arduino Opta

**ModbusLite_Arduino** is a lightweight, industrial-grade Modbus SDK built specifically for the **Arduino Opta** platform.  
It brings the reliability and structure of our Linux-based Modbus SDK into the embedded Opta environment — with support for both **Modbus TCP** and **Modbus RTU** communication.

---

## 🧠 Overview

ModbusLite_Arduino provides a consistent and portable API that allows the Opta to operate as:

- 🖧 **Modbus TCP Master or Slave** over Ethernet  
- 🔌 **Modbus RTU Master or Slave** over RS485  
- ⚙️ A future-ready hybrid bridge between TCP ↔ RTU networks  

It was developed to ensure **stable, crash-free operation** on industrial Opta PLCs while maintaining full interoperability with the existing **Linux ModbusLite SDK**.

---

## 🏭 Industrial Motivation

In industrial automation, you often have a mix of devices:
- Field sensors and actuators speaking **Modbus RTU (RS485)**  
- Gateways and controllers using **Modbus TCP (Ethernet)**  
- Edge devices like Raspberry Pi or Linux-based controllers running custom logic

The Opta sits perfectly in between — acting as:
- A **Modbus master** collecting data from remote devices  
- A **slave** exposing local process variables  
- Or a **bridge**, forwarding Modbus data between fieldbus and network domains  

However, standard Arduino Modbus libraries are **too heavy**, **unstable**, or **inflexible** for real industrial use.  
**ModbusLite_Arduino** was designed to solve that.

---

## 🚀 Key Features

- ✅ **Full Modbus/TCP and Modbus/RTU compliance**
- ✅ **Optimized for Opta’s mbed OS stack** — no crashes or watchdog resets  
- ✅ **Unified ModbusLite API** shared with the Linux SDK  
- ✅ **Lightweight memory footprint** suitable for real-time control  
- ✅ **Asynchronous-ready architecture** for future expansions  
- ✅ **Cross-platform interoperability** with Linux and other controllers  

---

## 🔧 Supported Modes

| Mode | Description | Typical Use Case |
|------|--------------|------------------|
| **Modbus TCP Master** | Opta polls Modbus TCP slaves over Ethernet | Control or data aggregation |
| **Modbus TCP Slave** | Opta exposes holding/input registers to TCP masters | PLC data access |
| **Modbus RTU Master** | Opta sends Modbus frames over RS485 | Fieldbus device control |
| **Modbus RTU Slave** | Opta responds to serial Modbus queries | Local register interface |

---

## 🔩 Example Scenarios

### 1️⃣ Opta TCP Master ↔ Opta TCP Slave  
Two Optas exchange register data over Ethernet.  
Used to simulate control between nodes or test register mapping.

### 2️⃣ Opta TCP Master ↔ Linux Modbus SDK Slave  
Demonstrates interoperability with industrial gateways or simulators.

### 3️⃣ Opta RTU Master ↔ Opta RTU Slave  
RS485-based communication between PLCs and field sensors.

### 4️⃣ Future: Opta Hybrid Bridge  
TCP master → Opta → RTU slave  
Opta acts as a gateway between Ethernet and serial Modbus networks.

---

## 🧩 Integration with Linux SDK

This Arduino SDK directly mirrors the Linux ModbusLite SDK’s internal architecture:

| Layer | Arduino SDK | Linux SDK | Purpose |
|-------|--------------|------------|----------|
| **Core Context** | `modbus_context.h` | Same | Shared PDU/ADU state machine |
| **TCP Transport** | `EthernetClient`, `EthernetServer` | BSD sockets | Identical protocol framing |
| **RTU Transport** | `Serial1 (RS485)` | `/dev/ttySx` | CRC-compatible data exchange |
| **High-Level API** | `ModbusLite` | `ModbusLite` | Same class interface |

This ensures full cross-platform communication between **Opta** and **Linux-based gateways**.

---

## 🧱 Repository Structure

ModbusLite_Arduino/
├── src/
│ ├── ModbusLite.cpp
│ ├── ModbusLite.h
│ └── core/
│ ├── modbus_context.cpp
│ ├── modbus_tcp.cpp
│ ├── modbus_rtu.cpp
│ ├── modbus_server.cpp
│ ├── modbus_core.c
│ └── modbus_register_map.cpp
├── examples/
│ ├── Opta_TCP_Master/
│ ├── Opta_TCP_Slave/
│ ├── Opta_RTU_Master/
│ └── Opta_RTU_Slave/
└── README.md


---

## ⚙️ Requirements

- **Arduino IDE 2.3.2+**
- **Opta board support package (Arduino-mbed 4.4.1+)**
- **Ethernet library (included with Opta BSP)**  
- **RS485 shield or built-in port (Opta RS485 variant)**

---

## 🧰 Installation

1. Clone or download this repository into your Arduino `libraries` folder:
   ```bash
   git clone https://github.com/<your-org>/ModbusLite_Arduino.git
