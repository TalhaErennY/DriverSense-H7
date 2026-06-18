# DriverSense-H7

### Bare-Metal STM32H7 Telemetry Engine with Ethernet, OLED Dashboard and Future TinyML-Based Driving Behavior Detection

DriverSense-H7 is an ongoing embedded systems project that combines register-level STM32H7Sxx driver development, real-time Ethernet telemetry processing, an SSD1306-based embedded dashboard and future TinyML-based driving behavior detection.

The project starts with a custom bare-metal driver layer for the STM32H7Sxx microcontroller family. This driver layer provides the foundation for receiving telemetry data from a PC over Ethernet, processing it directly on the STM32 board and displaying live driving information, system status and warnings on an external OLED screen.

The current prototype can participate in basic IPv4 Ethernet communication using a custom bare-metal Ethernet stack. It can parse ARP requests, transmit ARP replies, process ICMP Echo Requests, respond to ping requests and receive UDP telemetry packets. A Forza Data Out packet parser and compact SSD1306 dashboard interface are also under active development.

The long-term goal is to integrate TinyML inference into the telemetry pipeline. By analyzing real-time driving data, the system aims to classify driving behavior, detect abnormal vehicle responses and generate warnings for potentially unsafe driving conditions.

---

## Current Progress

### Core Driver Layer

* STM32H7Sxx memory map definitions
* RCC peripheral clock control
* GPIO initialization and deinitialization
* GPIO input/output APIs
* BSRR-based GPIO pin write
* ODR-based GPIO pin toggle
* EXTI interrupt support
* I2C driver support
* SSD1306 OLED driver and screen test code

---

### Ethernet Driver Progress

* RMII GPIO configuration

* Ethernet peripheral clock configuration

* PHY address detection

* PHY link status and auto-negotiation handling

* Ethernet MAC address configuration

* Ethernet DMA RX descriptor ring

* Ethernet DMA TX descriptor ring

* MPU-based non-cacheable memory region for Ethernet descriptors and RX buffers

* Separate cacheable TX buffer section with D-cache clean before DMA transmission

* MTL RX/TX queue configuration

* Raw Ethernet frame transmission

* Ethernet II frame parsing

* IPv4 packet filtering for local IP address

* IPv4 / UDP payload extraction

* ARP request parsing

* ARP reply generation

* ICMP Echo Request parsing

* ICMP Echo Reply generation

* IPv4 and ICMP checksum calculation

* Verified ARP resolution from PC:

  * `192.168.1.50 -> 02:00:00:00:00:01`

* Verified ICMP ping response from PC:

  * `Reply from 192.168.1.50`

---

### UDP and Forza Telemetry Progress

* UDP packet reception over the custom Ethernet stack
* UDP destination port filtering
* Forza Data Out packet size validation
* Fixed-size Forza telemetry packet parser
* Little-endian integer and float field extraction
* Speed conversion from m/s to km/h
* Current RPM and maximum RPM extraction
* Gear, throttle, brake, clutch, handbrake and steering extraction
* Tire slip value extraction
* Fuel level extraction
* Boost value extraction
* Distance traveled extraction
* Tire temperature extraction
* Reduced dashboard data structure for display logic
* Basic warning level generation from tire slip values

---

### SSD1306 OLED Dashboard Progress

* SSD1306 initialization over I2C
* Framebuffer-based drawing
* 5x7 bitmap font rendering
* Large 7-segment-style number rendering
* Rectangle, filled rectangle, line and bar drawing helpers
* Startup information screen
* Network status screen
* Waiting-for-Forza UDP screen
* Live Forza dashboard prototype
* Compact turbo and fuel indicators
* Dynamic gear display
* Distance display
* RPM arc with Max RPM based scaling
* Large speed display
* Shift-up and shift-down indicators
* Bottom status and warning bar

---

## Current Milestone

The project has reached a working bare-metal Ethernet communication milestone.

The STM32 board can currently receive raw Ethernet frames, parse ARP requests, transmit ARP replies and respond to ICMP Echo Requests. This confirms that the board can participate in basic IP-level Ethernet communication using a custom Ethernet stack without relying on a high-level networking library.

In addition, the project can receive UDP packets, validate Forza Data Out packet size, parse telemetry fields and display selected live vehicle data on an SSD1306 OLED dashboard.

This milestone forms the foundation for a real-time embedded telemetry dashboard and future TinyML-based driving behavior analysis.

---

## Current Test Configuration

### Network

* STM32 IPv4 address: `192.168.1.50`
* STM32 MAC address: `02:00:00:00:00:01`
* UDP telemetry port: `5005`

### Verified Communication

* ARP resolution from PC to STM32
* ICMP ping response from STM32
* UDP payload reception
* Forza Data Out packet parsing
* OLED dashboard update from parsed telemetry data

---

## Next Milestone

The next development step is to improve the real-time dashboard pipeline and prepare the telemetry layer for higher-level driving behavior analysis.

Planned next steps:

* Improve UDP payload validation
* Add UDP checksum validation using IPv4 pseudo-header
* Improve Forza Data Out parser robustness
* Refine OLED dashboard layout and icons
* Add better warning indicators based on telemetry values
* Add gear, RPM, slip and temperature based driving warnings
* Add simple telemetry state machine
* Prepare data structure for future TinyML inference
* Add lightweight logging/debug counters for telemetry events

---

## Planned Features

* UART driver
* SPI driver
* UDP checksum validation
* Real-time Forza telemetry packet parser improvements
* Improved OLED dashboard interface
* Warning and status display system
* FreeRTOS-based task architecture
* Minimal service-oriented telemetry layer
* TinyML-based driving behavior classification
* Telemetry-based anomaly and unsafe driving detection
* PC-to-STM32 telemetry communication over Ethernet
* Optional telemetry logging for offline analysis

---

## Target Platform

* STM32H7Sxx microcontroller family
* NUCLEO-H7S3L8 development board
* Bare-metal C
* RMII Ethernet interface
* LAN8742A Ethernet PHY
* SSD1306 OLED display
* Future TinyML inference on embedded target

---

## Project Goal

The long-term goal of DriverSense-H7 is to create a compact real-time embedded dashboard system that receives vehicle telemetry data from a PC, processes it on an STM32-based device and uses TinyML to detect driving behavior or unsafe vehicle conditions.

The project is intended to demonstrate low-level embedded software development, custom Ethernet communication, real-time data processing, embedded dashboard design and embedded machine learning on a resource-constrained microcontroller platform.
