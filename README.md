**ModbusLite for Arduino Opta**

**ModbusLite_Arduino** is a lightweight, industrial-grade Modbus SDK built specifically for the **Arduino Opta** platform. It brings the reliability and structure of our Linux-based Modbus SDK into the embedded Opta environment — with support for both **Modbus TCP** and **Modbus RTU** communication.

**🧠 Overview**

ModbusLite_Arduino provides a consistent and portable API that allows the Opta to operate as:

- 🖧 **Modbus TCP Master or Slave** over Ethernet
- 🔌 **Modbus RTU Master or Slave** over RS485
- ⚙️ A future-ready hybrid bridge between TCP ↔ RTU networks

It was developed to ensure **stable, crash-free operation** on industrial Opta PLCs while maintaining full interoperability with the existing **Linux ModbusLite SDK**.

**🏭 Industrial Motivation**

In industrial automation, you often have a mix of devices:

- Field sensors and actuators speaking **Modbus RTU (RS485)**
- Gateways and controllers using **Modbus TCP (Ethernet)**
- Edge devices like Raspberry Pi or Linux-based controllers running custom logic

The Opta sits perfectly in between — acting as:

- A **Modbus master** collecting data from remote devices
- A **slave** exposing local process variables
- Or a **bridge**, forwarding Modbus data between fieldbus and network domains

However, standard Arduino Modbus libraries are **too heavy**, **unstable**, or **inflexible** for real industrial use. **ModbusLite_Arduino** was designed to solve that.

**🚀 Key Features**

- ✅ **Full Modbus/TCP and Modbus/RTU compliance**
- ✅ **Optimized for Opta’s mbed OS stack** — no crashes or watchdog resets
- ✅ **Unified ModbusLite API** shared with the Linux SDK
- ✅ **Lightweight memory footprint** suitable for real-time control
- ✅ **Asynchronous-ready architecture** for future expansions
- ✅ **Cross-platform interoperability** with Linux and other controllers

**🔧 Supported Modes**

| **Mode** | **Description** | **Typical Use Case** |
| --- | --- | --- |
| **Modbus TCP Master** | Opta polls Modbus TCP slaves over Ethernet | Control or data aggregation |
| **Modbus TCP Slave** | Opta exposes holding/input registers to TCP masters | PLC data access |
| **Modbus RTU Master** | Opta sends Modbus frames over RS485 | Fieldbus device control |
| **Modbus RTU Slave** | Opta responds to serial Modbus queries | Local register interface |

**🔩 Example Scenarios**

**1️⃣ Opta TCP Master ↔ Opta TCP Slave**

Two Optas exchange register data over Ethernet. Used to simulate control between nodes or test register mapping.

**2️⃣ Opta TCP Master ↔ Linux Modbus SDK Slave**

Demonstrates interoperability with industrial gateways or simulators.

**3️⃣ Opta RTU Master ↔ Opta RTU Slave**

RS485-based communication between PLCs and field sensors.

**4️⃣ Future: Opta Hybrid Bridge**

TCP master → Opta → RTU slave Opta acts as a gateway between Ethernet and serial Modbus networks.

**🧩 Integration with Linux SDK**

This Arduino SDK directly mirrors the Linux ModbusLite SDK’s internal architecture:

| **Layer** | **Arduino SDK** | **Linux SDK** | **Purpose** |
| --- | --- | --- | --- |
| **Core Context** | modbus_context.h | Same | Shared PDU/ADU state machine |
| **TCP Transport** | EthernetClient, EthernetServer | BSD sockets | Identical protocol framing |
| **RTU Transport** | Serial1 (RS485) | /dev/ttySx | CRC-compatible data exchange |
| **High-Level API** | ModbusLite | ModbusLite | Same class interface |

This ensures full cross-platform communication between **Opta** and **Linux-based gateways**.

**🧱 Repository Structure**

text

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

**⚙️ Requirements**

- **Arduino IDE 2.3.2+**
- **Opta board support package (Arduino-mbed 4.4.1+)**
- **Ethernet library (included with Opta BSP)**
- **RS485 shield or built-in port (Opta RS485 variant)**

**🧰 Installation**

1.  Clone or download this repository into your Arduino libraries folder:

bash

git clone https://github.com/&lt;your-org&gt;/ModbusLite_Arduino.git

1.  Open Arduino IDE → File → Examples → ModbusLite_Arduino
2.  Select your Opta board and upload one of the example sketches.

**🧪 Testing**

Use any of the following setups:

- 🖥️ **QModMaster / Modbus Poll** for PC-based TCP testing
- 🧩 **Raspberry Pi** with ModbusLite SDK as the TCP or RTU counterpart
- ⚙️ **Two Opta boards** for end-to-end industrial testing

**🧮 Example Output**

**TCP Master Example**

text

\=== Opta Modbus TCP Master ===

Local IP: 192.168.1.177

Connecting to Modbus slave...

\[OK\] Connected to Modbus slave.

Read Holding Registers:

Reg\[0\] = 29

Reg\[1\] = 10

Reg\[2\] = 20

Reg\[3\] = 30

Reg\[4\] = 40

\[OK\] Write success.

**RTU Slave Example**

text

\=== Opta Modbus RTU Slave ===

\[INFO\] RS485 initialized and listening...

\[INFO\] HoldingReg\[0\] = 42

\[INFO\] HoldingReg\[0\] = 43

**📜 License**

This library is licensed under the MIT License. See the LICENSE file for full details.

**🤝 Contribution**

Pull requests are welcome! If you find an issue, open a GitHub issue with:

- Your Opta board type
- Example sketch name
- Short description of the problem

All contributions that improve stability, performance, or Modbus compliance are encouraged.

**🧭 Future Development**

- 🔄 Hybrid TCP↔RTU gateway mode
- 🧰 Modbus diagnostics and hex frame monitor
- ⚡ Async callback support
- ☁️ Integration with Opta’s secure MQTT edge gateway layer

**🤵 Maintained By**

**VK**

Reliable • Lightweight • Cross-Platform

**🏢 About ModbusLite SDK Family**

The ModbusLite SDK family is a suite of robust, interoperable libraries designed for seamless Modbus communication across embedded and Linux environments.

- **ModbusLite_Arduino**: Tailored for Arduino Opta — lightweight and optimized for real-time industrial PLCs.
- **ModbusLite Linux SDK**: Full-featured for gateways, edge servers, and custom automation stacks on Raspberry Pi, Ubuntu, and beyond.

Both share a unified core architecture, ensuring **zero-friction interoperability**. Deploy Opta as a field controller alongside Linux-based orchestration — or bridge them effortlessly for hybrid networks. Built for reliability in mission-critical automation.

[Explore the Linux SDK](https://github.com/%3Cyour-org%3E/ModbusLite_Linux) | [Join the Family](mailto:contact@modbuslite.com)
