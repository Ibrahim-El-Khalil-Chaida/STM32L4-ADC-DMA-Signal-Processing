# STM32L4 ADC DMA Signal Processing

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)  [![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)  [![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()

## Overview

**STM32L4 ADC DMA Signal Processing** is a complete reference implementation demonstrating how to achieve high-speed, real-time analog-to-digital conversion (ADC) on the STM32L476RG microcontroller. Leveraging timer-triggered sampling, circular DMA transfers, and ping-pong buffering, this project offloads continuous data acquisition from the CPU and provides a robust template for integrating custom DSP algorithms.

---

## 🚀 Features

- **High Sample Rates**  
  Configurable up to 100 kS/s (default), driven by a hardware timer trigger.  
- **Circular DMA**  
  Continuous, zero-overhead data movement from ADC to memory.  
- **Ping-Pong Buffering**  
  Half- and full-transfer callbacks enable batch processing without interrupt flooding.  
- **Modular C++ Design**  
  Encapsulated `AdcDmaHandler` class for initialization, start/stop, and `Process()` callbacks.  
- **Low-Power Operation**  
  `__WFI()` instruction puts the MCU to sleep between interrupts for maximum efficiency.  

---

## 📦 Hardware Requirements

- **STM32L476RG Nucleo-64** (e.g. NUCLEO-L476RG)  
- Analog input source connected to **PA0 / ADC1_IN5**  
- Optional: Serial (USART) or USB-CDC interface for processed data output  

---

## 🛠 Software Requirements

- **STM32CubeIDE** (or equivalent toolchain supporting HAL)  
- **STM32CubeL4 HAL** drivers (included as a submodule or via STM32CubeMX)  
- **CMake** (for out-of-tree builds; optional)  
- **GNU Arm Embedded Toolchain** (GCC)  

---

## 🔧 Installation

1. **Clone the repository**  
```bash
   git clone https://github.com/<your-username>/stm32l4-adc-dma-signal-processing.git
   cd stm32l4-adc-dma-signal-processing
````

2. **Initialize submodules** (if using HAL as a submodule)

```bash
   git submodule update --init --recursive
```
3. **Open in STM32CubeIDE**

   * Import as an existing STM32 project.
   * Verify that the `Drivers/STM32L4xx_HAL_Driver` path is correctly set.

---

## ▶️ Usage

* Build the project and flash to your Nucleo board via ST-Link.
* On startup, the ADC/DMA system begins sampling automatically.
* Processed results (e.g. averages, filter outputs) can be sent over UART or USB-CDC—customize the `AdcDmaHandler::Process()` method in `main.cpp`.
* To adjust parameters, modify the constants at the top of `main.cpp`:

  * `kSampleRate` (samples/sec)
  * `kBufferSize` (must be even for ping-pong)
  * `kAdcChannel` and sampling time

---

## ⚙️ Configuration

| Parameter      | Description                                     | Default         |
| -------------- | ----------------------------------------------- | --------------- |
| `kSampleRate`  | ADC sampling rate (samples per second)          | `100000`        |
| `kBufferSize`  | DMA circular buffer length (even)               | `1024`          |
| `kAdcChannel`  | ADC channel (e.g. `ADC_CHANNEL_5` for PA0)      | `ADC_CHANNEL_5` |
| `SamplingTime` | ADC sampling cycles (speed vs. noise trade-off) | `6CYCLES_5`     |

---

## 📄 License

This project is released under the **MIT License**. See [LICENSE](LICENSE) for full details.
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
