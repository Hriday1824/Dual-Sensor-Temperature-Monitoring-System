# Dual-Sensor Temperature Monitoring System

## Overview

A real-time dual-sensor temperature monitoring system developed using the **STM32F439ZI** microcontroller. The system simultaneously samples two analog temperature sensors through **ADC1**, calculates their individual temperatures and average temperature, and displays the result on a **16×2 I²C LCD** with a graphical bar indicator.

The measurement update rate can be selected between **100 ms, 500 ms and 1000 ms** using an external interrupt.

## Features

* Dual analog temperature sensor monitoring
* 12-bit ADC acquisition using **ADC1**
* Simultaneous two-channel scanning
* Temperature conversion from sensor voltage
* Average temperature calculation
* 16×2 LCD display through **I²C**
* Graphical temperature bar on LCD
* Selectable update intervals:

  * 100 ms
  * 500 ms
  * 1000 ms
* Timer-interrupt-driven sampling control
* External-interrupt-based interval selection

## Hardware

| Component          | Configuration       |
| ------------------ | ------------------- |
| Microcontroller    | STM32F439ZI         |
| ADC                | ADC1, 12-bit        |
| Sensor 1           | ADC Channel 0 — PA0 |
| Sensor 2           | ADC Channel 1 — PA1 |
| Display            | 16×2 LCD            |
| LCD Interface      | I²C1                |
| I²C Speed          | 100 kHz             |
| Timer              | TIM2                |
| External Interrupt | PC13                |

## System Architecture

```text
             ┌──────────────────────┐
             │ Temperature Sensor 1 │
             └──────────┬───────────┘
                        │ PA0
                        ▼
                 ┌─────────────┐
                 │             │
                 │   ADC1      │
                 │  12-bit     │
                 │             │
                 └──────┬──────┘
                        │
                        │
                 ┌──────▼──────┐
                 │ Temperature │
                 │ Calculation │
                 └──────┬──────┘
                        │
              ┌─────────┴─────────┐
              │                   │
              ▼                   ▼
       Individual Temps       Average Temp
                                  │
                                  ▼
                         ┌────────────────┐
                         │ 16×2 I²C LCD   │
                         │ Avg: XX.X °C    │
                         │ |||||||         │
                         └────────────────┘

             ┌──────────────────────┐
             │ Temperature Sensor 2 │
             └──────────┬───────────┘
                        │ PA1
                        └──────────► ADC1
```

## Working Principle

### 1. ADC Acquisition

ADC1 is configured in **12-bit resolution** with two conversions in scan mode:

* **Channel 0 / Rank 1:** Sensor 1 — PA0
* **Channel 1 / Rank 2:** Sensor 2 — PA1

The ADC values range from **0 to 4095** for a 0–3.3 V input.

### 2. Voltage Conversion

The ADC readings are converted to voltage using:

```text
Voltage = ADC_Value × 3.3 / 4095
```

### 3. Temperature Conversion

The sensor output is converted to temperature using the implemented sensor relationship:

```text
Temperature (°C) = Voltage / 0.01
```

The two sensor temperatures are then averaged:

```text
Average Temperature = (Temperature 1 + Temperature 2) / 2
```

### 4. LCD Display

The average temperature is displayed on the first LCD line:

```text
Avg: XX.X C
```

The second line displays a 16-character bar graph whose length corresponds to the measured average temperature.

## Sampling Rate Control

TIM2 generates periodic interrupts used to control the temperature update interval.

Three selectable update intervals are stored in the firmware:

```c
uint16_t intervals[] = {100, 500, 1000};
```

An external interrupt on **PC13** cycles through the available intervals:

```text
100 ms → 500 ms → 1000 ms → 100 ms → ...
```

The timer interrupt increments a counter and sets an update flag whenever the selected interval is reached.

The main loop then performs the ADC acquisition and LCD update when the flag is set.

## Firmware Flow

```text
System Initialization
        │
        ▼
Initialize GPIO
        │
        ▼
Initialize ADC1
        │
        ▼
Initialize I²C1
        │
        ▼
Initialize TIM2
        │
        ▼
Initialize LCD
        │
        ▼
Start TIM2 Interrupt
        │
        ▼
   Main Loop
        │
        ▼
  update_flag = 1?
      /     \
    No       Yes
    │         │
    │         ▼
    │    Start ADC
    │         │
    │         ▼
    │    Read Sensor 1
    │         │
    │         ▼
    │    Read Sensor 2
    │         │
    │         ▼
    │ Calculate Temperatures
    │         │
    │         ▼
    │ Calculate Average
    │         │
    │         ▼
    │ Update LCD + Bar Graph
    │         │
    └─────────┘
```

## Interrupts

### TIM2 Interrupt

TIM2 periodically increments a counter. When the counter reaches the selected update interval, the firmware sets:

```c
update_flag = 1;
```

The ADC processing is then performed in the main loop rather than inside the interrupt callback.

### External Interrupt

The **PC13** external interrupt changes the selected sampling interval:

```text
100 ms → 500 ms → 1000 ms
```

This allows the measurement refresh rate to be changed during operation.

## Software

* **STM32CubeIDE**
* **STM32 HAL**
* C
* LCD16x2 driver

## Peripheral Configuration

### ADC1

* Resolution: 12-bit
* Scan conversion: Enabled
* Number of conversions: 2
* Sampling time: 144 cycles
* Channels:

  * ADC1 Channel 0 → PA0
  * ADC1 Channel 1 → PA1

### I²C1

* Clock speed: 100 kHz
* 7-bit addressing
* Analog filter: Enabled
* Digital filter: 0

### TIM2

* Prescaler: 15999
* Period: 1
* Internal clock source
* Interrupt enabled

### GPIO

* PA0 → Temperature Sensor 1
* PA1 → Temperature Sensor 2
* PC13 → External interrupt for interval selection

## Project Structure

```text
Dual-Sensor-Temperature-Monitoring/
│
├── Core/
│   ├── Inc/
│   │   └── main.h
│   │
│   └── Src/
│       └── main.c
│
├── Drivers/
│   └── lcd16x2/
│       ├── lcd16x2.c
│       └── lcd16x2.h
│
└── README.md
```

## Key Implementation

The system uses a flag-based architecture so that the **timer interrupt only schedules the measurement**, while ADC acquisition, temperature calculation and LCD updating are handled in the main loop.

This avoids performing the complete measurement and display operation inside an interrupt service routine.

## Results

The implemented system provides:

* Real-time acquisition from two temperature sensors
* Individual sensor temperature calculation
* Average temperature computation
* LCD-based numerical display
* LCD bar-graph visualization
* Runtime selection of 100 ms, 500 ms and 1000 ms update intervals

## Future Improvements

Possible extensions include:

* Temperature threshold detection
* Automatic fan control using PWM
* Sensor fault detection
* UART-based data logging
* Temperature history and trend analysis
* Wireless monitoring through Bluetooth or Wi-Fi
* Calibration and sensor-offset compensation
