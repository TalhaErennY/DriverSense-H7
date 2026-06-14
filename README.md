# Project Chimera

### Bare-Metal STM32H7 Telemetry Engine with TinyML-Based Driving Behavior Detection

Project Chimera is an ongoing embedded systems project that combines bare-metal STM32H7Sxx driver development, real-time Ethernet telemetry processing and TinyML-based driving behavior detection.

The project starts with a custom register-level driver layer for the STM32H7Sxx microcontroller family. This driver layer provides the foundation for receiving telemetry data from a PC over Ethernet, processing it on the STM32 board and displaying driving information, system status and warnings on an external screen.

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
* Raw Ethernet frame transmission test
* Ethernet II frame parsing
* IPv4 / UDP payload extraction
* ARP request parsing
* ARP reply generation
* Verified ARP resolution from PC:

  * `192.168.1.50 -> 02:00:00:00:00:01`

## Current Milestone

The Ethernet driver can currently receive raw Ethernet frames, parse ARP requests and transmit ARP replies. The STM32 board can announce its MAC address to a PC through ARP, allowing the PC to resolve the STM32 IP address on the local network.

At this stage, the board is able to participate in basic Ethernet communication at the raw frame level. This forms the foundation for UDP telemetry reception and future real-time dashboard functionality.

## Planned Features

* UART driver
* SPI driver
* ICMP echo reply support
* UDP packet handling
* Real-time telemetry packet parser
* OLED dashboard interface
* Warning and status display system
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

## Project Goal

The long-term goal of Project Chimera is to create a compact real-time embedded dashboard system that receives vehicle telemetry data from a PC, processes it on an STM32-based device and uses TinyML to detect driving behavior or unsafe vehicle conditions.

The project is intended to demonstrate low-level embedded software development, Ethernet communication, real-time data processing and embedded machine learning on a resource-constrained microcontroller platform.

