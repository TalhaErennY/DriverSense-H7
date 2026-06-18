# DriverSense-H7

### Bare-Metal STM32H7 Telemetry Engine with TinyML-Based Driving Behavior Detection

DriverSense-H7 is an ongoing embedded systems project that combines register-level STM32H7Sxx driver development, real-time Ethernet telemetry processing and future TinyML-based driving behavior detection.

The project starts with a custom bare-metal driver layer for the STM32H7Sxx microcontroller family. This driver layer provides the foundation for receiving telemetry data from a PC over Ethernet, processing it on the STM32 board and displaying driving information, system status and warnings on an external screen.

The long-term goal is to integrate TinyML inference into the telemetry pipeline. By analyzing real-time driving data, the system aims to classify driving behavior, detect abnormal vehicle responses and generate warnings for potentially unsafe driving conditions.

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
* SSD1306 OLED screen test code

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

* Raw Ethernet frame transmission test

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

## Current Milestone

The Ethernet stack can currently receive raw Ethernet frames, parse ARP requests, transmit ARP replies and respond to ICMP Echo Requests.

The STM32 board can announce its MAC address to a PC through ARP and can respond to ping requests over IPv4. This confirms that the board is now able to participate in basic IP-level Ethernet communication using a custom bare-metal Ethernet stack.

This milestone forms the foundation for UDP telemetry reception, Forza Data Out packet parsing and future real-time dashboard functionality.

## Next Milestone

The next development step is to receive and validate UDP telemetry packets from Forza Data Out.

Planned next steps:

* Improve UDP payload validation
* Add UDP checksum validation using IPv4 pseudo-header
* Parse fixed-size Forza Data Out packets
* Extract speed, RPM, gear, throttle, brake, steering and tire slip data
* Display live telemetry on an SSD1306 OLED dashboard
* Add simple warning indicators based on telemetry values

## Planned Features

* UART driver
* SPI driver
* UDP checksum validation
* Real-time Forza telemetry packet parser
* OLED dashboard interface
* Warning and status display system
* FreeRTOS-based task architecture
* Minimal service-oriented telemetry layer
* TinyML-based driving behavior classification
* Telemetry-based anomaly and unsafe driving detection
* PC-to-STM32 telemetry communication over Ethernet

## Target Platform

* STM32H7Sxx microcontroller family
* NUCLEO-H7S3L8 development board
* Bare-metal C
* RMII Ethernet interface
* SSD1306 OLED display
* TinyML inference on embedded target

## Network Configuration

Current test configuration:

* STM32 IPv4 address: `192.168.1.50`
* STM32 MAC address: `02:00:00:00:00:01`
* UDP telemetry port: `5005`

## Project Goal

The long-term goal of DriverSense-H7 is to create a compact real-time embedded dashboard system that receives vehicle telemetry data from a PC, processes it on an STM32-based device and uses TinyML to detect driving behavior or unsafe vehicle conditions.

The project is intended to demonstrate low-level embedded software development, Ethernet communication, real-time data processing and embedded machine learning on a resource-constrained microcontroller platform.
