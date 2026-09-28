# BCA182 FreeRTOS Multisensor Room Monitoring System

A real-time room monitoring system for the STM32F103C8T6 Blue Pill, built using STM32Cube HAL and native FreeRTOS. The system monitors temperature, humidity, ambient light, and motion while providing OLED-based measurement display, rotary encoder navigation, temperature alarm functionality, and ACTIVE/INACTIVE system states.

The project is developed and tested using PlatformIO and the Wokwi simulation environment.

---

## Project Overview

The BCA182 FreeRTOS Multisensor Room Monitoring System is a multitasking embedded application designed to demonstrate the use of a real-time operating system in an environmental monitoring system.

The system uses an STM32F103C8T6 Blue Pill as the main microcontroller. Several sensors and user-interface devices are connected to the microcontroller to provide real-time monitoring and control.

The system monitors four primary room conditions:

- Temperature
- Humidity
- Ambient light
- Motion

Temperature and humidity are measured using a DHT22 sensor. Ambient light is measured using an LDR/photoresistor connected to an analog input. Motion is detected using a PIR sensor.

The user can select which measurement is shown on the SSD1306 OLED display using a KY-040 rotary encoder. The available display modes are:

1. Temperature
2. Humidity
3. Light
4. Motion

The system also contains a temperature alarm mechanism. Temperatures below 18.0 °C are classified as LOW temperature, while temperatures above 30.0 °C are classified as HIGH temperature. Temperatures from 18.0 °C through 30.0 °C are considered NORMAL.

A PWM-controlled buzzer provides the temperature alarm output.

The system also implements an ACTIVE/INACTIVE state machine. When the system remains without detected motion for 15 seconds, it transitions from ACTIVE to INACTIVE. Motion detected by the PIR sensor causes the system to return to ACTIVE.

The firmware is organized into multiple FreeRTOS tasks instead of placing all application logic inside `main.cpp`. Hardware-independent decision logic is separated into dedicated `*_logic.cpp` modules so that it can be tested independently on a PC.

The project does not use the Arduino framework. The firmware uses STM32Cube HAL and native FreeRTOS APIs.

<img width="781" height="332" alt="image" src="https://github.com/user-attachments/assets/022042d5-269b-4b6e-8754-21b117135704" />

*Figure 1. Overall BCA182 FreeRTOS multisensor room monitoring system.*

---

## Features

The system provides the following features:

- DHT22 temperature monitoring
- DHT22 humidity monitoring
- LDR-based ambient light measurement
- PIR motion detection
- SSD1306 128x64 OLED display
- Rotary encoder navigation
- Temperature alarm detection
- PWM-driven buzzer
- ACTIVE/INACTIVE state machine
- 15-second inactivity timeout
- FreeRTOS multitasking
- Explicit task priorities
- Periodic scheduling using `vTaskDelayUntil()`
- FreeRTOS queues
- FreeRTOS queue set
- FreeRTOS mutex
- FreeRTOS event group
- Inter-task communication
- Hardware-independent application logic
- Native Unity unit testing
- Static analysis using cppcheck
- Wokwi simulation
- Modular source-code organization

---

## Learning Objectives

This project demonstrates the practical use of real-time operating system concepts in an embedded application.

The main learning objectives are:

- Creating multiple FreeRTOS tasks
- Assigning explicit priorities to tasks
- Understanding FreeRTOS task scheduling
- Using blocking delays instead of uncontrolled busy loops
- Using `vTaskDelayUntil()` for periodic execution
- Using queues for inter-task communication
- Using a queue set to wait for multiple communication sources
- Using a mutex to protect a shared serial resource
- Using an event group for shared system status
- Implementing a state machine using FreeRTOS event information
- Separating hardware-independent logic from hardware-dependent implementation
- Performing unit testing on embedded decision logic
- Performing static code analysis
- Developing and testing an STM32 embedded application in Wokwi
- Using Git and GitHub for source-code management

---

## System Architecture

The system consists of an STM32F103C8T6 Blue Pill running FreeRTOS with multiple input devices and output devices.

The main inputs are:

- DHT22 temperature and humidity sensor
- LDR/photoresistor
- PIR motion sensor
- KY-040 rotary encoder

The main outputs are:

- SSD1306 OLED
- PWM buzzer
- On-board LED
- Serial monitor

The software architecture is divided into hardware-dependent modules, FreeRTOS tasks, RTOS communication objects, and hardware-independent logic.

### High-Level Architecture

<img width="256" height="230" alt="image" src="https://github.com/user-attachments/assets/d6ae96d2-0c9e-43a7-8dcd-476ed330b939" />

*Figure 2. High-level FreeRTOS system architecture showing the application tasks, sensor inputs, FreeRTOS inter-process communication, and system outputs.*

### Software Layers

| Layer | Contents |
|---|---|
| Application Tasks | `SensorTask`, `DisplayTask`, `InputTask`, `MotionTask`, `AlarmTask`, and `StateTask` |
| Hardware-Independent Logic | Temperature evaluation, light conversion, encoder navigation, and state evaluation |
| Platform | STM32Cube HAL, STM32F103C8T6 hardware configuration, and FreeRTOS |

The task modules are separated from the decision logic where possible. For example, temperature alarm decisions are handled by `alarm_logic.cpp`, while hardware-specific buzzer control remains in `alarm.cpp`.

This organization allows the logic to be compiled and tested in the native PlatformIO environment without requiring the STM32 hardware.

---

## FreeRTOS Architecture

The application uses six main FreeRTOS tasks.

| Task | Priority | Stack | Responsibility |
|---|---:|---:|---|
| `SensorTask` | 2 | 256 | DHT22 and LDR acquisition |
| `DisplayTask` | 1 | 256 | OLED display management |
| `InputTask` | 3 | 128 | Rotary encoder input |
| `MotionTask` | 3 | 128 | PIR motion monitoring |
| `AlarmTask` | 2 | 128 | Temperature alarm and buzzer |
| `StateTask` | 2 | 128 | ACTIVE/INACTIVE state management |

The tasks are created during application startup in `app_main()` and the FreeRTOS scheduler is started after all required tasks and RTOS objects have been created.

The task priorities are explicitly specified during `xTaskCreate()`.

### Task Priorities

`InputTask` and `MotionTask` use priority 3 because they process user input and motion events.

`SensorTask`, `AlarmTask`, and `StateTask` use priority 2 because their processing is important but operates on slower environmental and state-management timescales.

`DisplayTask` uses priority 1 because OLED rendering is less time-critical than sensor acquisition, motion detection, and user input.

### Periodic Scheduling

Periodic work uses FreeRTOS blocking mechanisms.

At least one periodic task uses:

```cpp
vTaskDelayUntil()
```

This allows periodic tasks to maintain a consistent execution schedule while blocking between executions.

---

## Task Design

The application is divided into six FreeRTOS tasks. Each task has a specific responsibility and an explicitly assigned priority.

| Task | Priority | Stack | Responsibility |
|---|---:|---:|---|
| `SensorTask` | 2 | 256 | DHT22 and LDR acquisition |
| `DisplayTask` | 1 | 256 | OLED display management |
| `InputTask` | 3 | 128 | Rotary encoder input |
| `MotionTask` | 3 | 128 | PIR motion monitoring |
| `AlarmTask` | 2 | 128 | Temperature alarm and buzzer |
| `StateTask` | 2 | 128 | ACTIVE/INACTIVE state management |

### SensorTask

`SensorTask` periodically reads the DHT22 temperature and humidity values and the LDR light level.

The collected measurements are packaged as sensor data and communicated to the other tasks using FreeRTOS communication objects.

### DisplayTask

`DisplayTask` is responsible for the SSD1306 OLED display.

The OLED is managed by this task so that display operations remain centralized. The task receives sensor information and display-mode changes and updates the selected measurement.

The available display modes are:

1. Temperature
2. Humidity
3. Light
4. Motion

### InputTask

`InputTask` monitors the KY-040 rotary encoder.

Clockwise rotation moves to the next display mode, while counterclockwise rotation moves to the previous display mode. Navigation wraps around at both ends of the list.

### MotionTask

`MotionTask` monitors the PIR motion sensor and reports the current motion condition to the rest of the system.

Motion information is also used by `StateTask` to determine whether the system should remain ACTIVE or return to ACTIVE after becoming INACTIVE.

### AlarmTask

`AlarmTask` evaluates the latest temperature reading.

The temperature decision logic uses the following limits:

| Temperature | State |
|---|---|
| Below 18.0 °C | LOW |
| 18.0 °C to 30.0 °C | NORMAL |
| Above 30.0 °C | HIGH |

When the temperature is outside the normal range, the buzzer is activated using PWM.

### StateTask

`StateTask` manages the system's ACTIVE/INACTIVE state.

The system starts in ACTIVE mode. If no motion is detected for 15 seconds, the system transitions to INACTIVE. Motion detection causes the system to return to ACTIVE.

---

## Inter-Task Communication

FreeRTOS communication mechanisms are used to exchange information between tasks and to protect shared resources.

The main mechanisms used by the application are:

| Mechanism | Purpose |
|---|---|
| Queue | Transfers sensor data and display-mode information between tasks |
| Queue Set | Allows `DisplayTask` to wait for multiple communication sources |
| Event Group | Stores shared system status such as ACTIVE, motion, and alarm conditions |
| Mutex | Protects shared serial logging |

### Sensor Data

Sensor measurements are grouped into a common structure:

```cpp
struct SensorData {
    float temperature;
    float humidity;
    int lightLevel;
    bool motionDetected;
};
```

This structure allows related measurements to be transferred between tasks as one data item.

### Queue

Queues are used for communication between producer and consumer tasks.

Sensor information is passed to the tasks that require the latest measurements, while the rotary encoder produces display-mode information for `DisplayTask`.

### Queue Set

A queue set is used by `DisplayTask` so that it can wait for communication from multiple queue sources.

This allows the display task to respond to either new sensor information or a new display mode selected by the rotary encoder.

### Event Group

An event group is used for system-wide status information.

The application uses event flags for conditions including:

- ACTIVE state
- Motion detection
- Temperature alarm

This allows multiple tasks to observe shared system conditions.

### Mutex

A mutex protects the shared serial logging resource.

Multiple tasks can generate diagnostic messages. The mutex prevents two tasks from writing to the serial output at the same time, avoiding interleaved diagnostic messages.

---

## State Machine

The system uses two primary operating states:

```text
             No motion for 15 seconds
        ┌──────────────────────────────┐
        │                              ▼
   ┌──────────┐                   ┌──────────┐
   │  ACTIVE  │                   │ INACTIVE │
   └──────────┘                   └──────────┘
        ▲                              │
        │                              │
        └──────── Motion detected ─────┘
```

The transition behavior is:

| Current State | Condition | Next State |
|---|---|---|
| ACTIVE | Motion detected | ACTIVE |
| ACTIVE | No motion for 15 seconds | INACTIVE |
| INACTIVE | No motion | INACTIVE |
| INACTIVE | Motion detected | ACTIVE |

### ACTIVE

In ACTIVE mode:

- Sensor monitoring continues.
- The OLED display can show the selected measurement.
- Rotary encoder navigation is active.
- Temperature alarm processing is active.
- PIR motion monitoring continues.

### INACTIVE

In INACTIVE mode:

- The system has reached the 15-second inactivity timeout.
- Unnecessary display activity can be reduced.
- Motion detection remains active.
- Motion detection restores the system to ACTIVE.

The state transition logic is separated into hardware-independent logic so that it can be tested independently.

---

## Hardware / Simulated Components

The project uses the STM32F103C8T6 Blue Pill as the main microcontroller and Wokwi for simulation.

| Component | Purpose |
|---|---|
| STM32F103C8T6 Blue Pill | Main microcontroller |
| DHT22 | Temperature and humidity measurement |
| LDR / Photoresistor | Ambient light measurement |
| PIR Motion Sensor | Motion detection |
| KY-040 Rotary Encoder | Display navigation |
| SSD1306 128x64 OLED | Measurement display |
| Buzzer | Temperature alarm |
| On-board LED | System indication |
| Serial Monitor | Debug and diagnostic output |

---

## Pin Configuration

The Wokwi circuit uses the following connections:

| STM32 Pin | Component | Function |
|---|---|---|
| PA0 | DHT22 | DHT22 data |
| PA1 | LDR | Analog light input |
| PA2 | PIR | Motion input |
| PA3 | Rotary Encoder | CLK |
| PA4 | Rotary Encoder | DT |
| PA5 | Rotary Encoder | Switch |
| PA8 | Buzzer | PWM output |
| PA9 | Serial Monitor | USART1 TX |
| PA10 | Serial Monitor | USART1 RX |
| PB6 | SSD1306 | I2C SCL |
| PB7 | SSD1306 | I2C SDA |
| PC13 | On-board LED | System indication |

The OLED communicates through I2C using PB6 and PB7.

---

## Repository Structure

The repository is organized into separate source, header, testing, and configuration files.

```text
.
├── .vscode/
├── include/
│   ├── alarm.h
│   ├── display.h
│   ├── input.h
│   ├── motion.h
│   ├── rtos_objects.h
│   ├── sensors.h
│   ├── serial_log.h
│   ├── system_state.h
│   └── FreeRTOSConfig.h
│
├── lib/
│   └── freertos_port_patch/
│
├── scripts/
│   └── native_toolchain.py
│
├── src/
│   ├── main.cpp
│   ├── alarm.cpp
│   ├── alarm_logic.cpp
│   ├── dht22.cpp
│   ├── dht22_logic.cpp
│   ├── display.cpp
│   ├── display_logic.cpp
│   ├── input.cpp
│   ├── input_logic.cpp
│   ├── motion.cpp
│   ├── motion_logic.cpp
│   ├── rtos_objects.cpp
│   ├── sensors.cpp
│   ├── sensors_logic.cpp
│   ├── serial_log.cpp
│   └── system_state.cpp
│
├── test/
│   ├── test_alarm/
│   ├── test_light_level/
│   ├── test_navigation/
│   └── test_state/
│
├── .gitignore
├── diagram.json
├── platformio.ini
└── wokwi.toml
```

The project separates hardware-specific code from hardware-independent application logic.

The `*_logic.cpp` files contain decision-making functions that can be tested independently using the native PlatformIO test environment.

---

## Getting Started

### Prerequisites

The project requires:

- Visual Studio Code
- PlatformIO
- Wokwi
- Git

The project uses PlatformIO with the STM32Cube framework and native FreeRTOS.

### Clone the Repository

```bash
git clone https://github.com/karlvincentremo/freertos-multisensor.git
cd freertos-multisensor
```

Open the project folder in Visual Studio Code.

---

## Building the Project

The STM32 firmware can be built using:

```bash
pio run -e bluepill_f103c8
```

The generated firmware files are placed under:

```text
.pio/build/bluepill_f103c8/
```

The final firmware build completed successfully with the following approximate resource usage:

- RAM: 58.8%
- Flash: 31.3%

---

## Running the Wokwi Simulation

The Wokwi circuit is defined in:

```text
diagram.json
```

The Wokwi configuration is defined in:

```text
wokwi.toml
```

The firmware ELF file used by the simulator is:

```text
.pio/build/bluepill_f103c8/firmware.elf
```

After building the project, the Wokwi simulation can be started from Visual Studio Code.

The serial monitor provides startup and diagnostic information.

Example startup output:

```text
BCA182 FreeRTOS Multisensor
System starting...
MotionTask started
InputTask started
SensorTask started
AlarmTask started
StateTask started
DisplayTask started
DISPLAY: OLED initialised
```

---

## Unit Testing

Hardware-independent application logic is tested using PlatformIO's native test environment.

Run the tests using:

```bash
pio test -e native
```

The project contains four test groups:

| Test Group | Number of Tests |
|---|---:|
| Temperature alarm | 5 |
| Light level conversion | 4 |
| Display navigation | 6 |
| System state | 5 |
| **Total** | **20** |

### Temperature Alarm

The tests cover:

- Below 18.0 °C
- Exactly 18.0 °C
- Normal temperature
- Exactly 30.0 °C
- Above 30.0 °C

### Light Level

The tests cover:

- Full-scale ADC value
- Zero ADC value
- Mid-scale ADC value
- Values above the expected ADC range

### Display Navigation

The tests cover:

- Forward navigation
- Reverse navigation
- Forward wraparound
- Reverse wraparound
- Inverse navigation
- Complete navigation cycle

### System State

The tests cover:

- ACTIVE without timeout
- ACTIVE with timeout
- INACTIVE without motion
- INACTIVE with motion
- Motion overriding timeout

### Test Result

The final native test execution completed successfully:

```text
20 test cases: 20 succeeded
```

This exceeds the laboratory minimum requirement of 13 meaningful unit tests.

---

## Static Code Analysis

Static analysis was performed using PlatformIO:

```bash
pio check
```

The project uses cppcheck.

The final analysis reported:

```text
Component     HIGH    MEDIUM    LOW
-----------  ------  --------  ------
src            0        0       41
Total          0        0       41

Environment      Tool      Status
bluepill_f103c8  cppcheck  PASSED
```

No HIGH or MEDIUM severity findings were reported.

The remaining LOW findings were primarily related to unused functions and C-style casts.

---

## Functional Verification

The system was verified using the Wokwi simulation environment.

| Test | Stimulus | Expected Result |
|---|---|---|
| FT-01 | Change temperature | Temperature display updates |
| FT-02 | Change humidity | Humidity display updates |
| FT-03 | Change light input | Light value changes |
| FT-04 | Rotate encoder clockwise | Next display mode is selected |
| FT-05 | Rotate encoder counterclockwise | Previous display mode is selected |
| FT-06 | Set temperature above 30 °C | Temperature alarm activates |
| FT-07 | Set temperature below 18 °C | Low-temperature alarm activates |
| FT-08 | Trigger PIR | Motion is detected |
| FT-09 | Leave system without motion for 15 seconds | System becomes INACTIVE |
| FT-10 | Trigger PIR while INACTIVE | System returns to ACTIVE |

---

## Engineering Decisions

### Native FreeRTOS

The project uses native FreeRTOS APIs with STM32Cube HAL rather than the Arduino framework.

### Modular Software Design

The application is separated into multiple modules rather than placing the entire application inside `main.cpp`.

The main application startup sequence is responsible for:

1. MCU initialization
2. Hardware initialization
3. RTOS object creation
4. Task creation
5. Starting the FreeRTOS scheduler

### Hardware-Independent Logic

Decision-making logic is separated from hardware-specific code.

Examples include:

- Temperature alarm evaluation
- Light-level conversion
- Display navigation
- System state evaluation

This structure makes the logic suitable for native unit testing.

### Explicit Task Priorities

Task priorities are assigned according to responsiveness requirements.

`InputTask` and `MotionTask` use priority 3.

`SensorTask`, `AlarmTask`, and `StateTask` use priority 2.

`DisplayTask` uses priority 1.

### Shared Serial Resource

Serial logging is shared by multiple tasks.

A mutex is used to ensure that diagnostic output from different tasks does not overlap.

---

## Limitations

The project has the following limitations:

- The system was primarily tested using Wokwi simulation.
- Sensor readings are simulated rather than obtained from physical sensors.
- The system has not been fully validated on physical STM32 hardware.
- The OLED interface displays one selected measurement at a time.
- The project does not currently include remote monitoring or wireless connectivity.
- The project does not currently include long-term sensor data logging.

---

## Future Improvements

Possible future improvements include:

- Testing with physical STM32 hardware and sensors
- Improving the OLED graphical interface
- Adding temperature and humidity graphs
- Adding configurable alarm thresholds
- Adding data logging
- Adding wireless monitoring
- Adding RTC-based timestamps
- Adding additional environmental sensors
- Expanding automated functional testing
- Improving power management

---

## References and Acknowledgments

### References

- FreeRTOS documentation
- STM32F103C8T6 documentation
- STM32Cube HAL documentation
- PlatformIO documentation
- Wokwi documentation
- SSD1306 OLED documentation
- DHT22 sensor documentation

### Laboratory Reference

This project was developed as part of the BCA182 Real-Time Operating Systems laboratory activity.

Prepared under the laboratory activity provided by:

**Asst. Prof. Paul Rodolf P. Castor, M.Sc.**

### Development Tools

- Visual Studio Code
- PlatformIO
- STM32Cube HAL
- FreeRTOS
- Wokwi
- Git
- GitHub
- Unity
- cppcheck

---

## Project Status

The project currently includes:

- STM32F103C8T6 firmware
- Native FreeRTOS multitasking
- DHT22 temperature and humidity monitoring
- LDR ambient light monitoring
- PIR motion detection
- Rotary encoder navigation
- SSD1306 OLED display
- PWM temperature alarm
- ACTIVE/INACTIVE state machine
- FreeRTOS queues
- FreeRTOS queue set
- FreeRTOS mutex
- FreeRTOS event group
- Native unit testing
- Static code analysis
- Wokwi simulation

### Verification Summary

| Verification | Result |
|---|---|
| STM32 firmware build | PASS |
| Native unit tests | 20/20 PASS |
| Static analysis | PASS |
| HIGH cppcheck findings | 0 |
| MEDIUM cppcheck findings | 0 |

---

## License

This project was developed for academic laboratory work under the BCA182 Real-Time Operating Systems course.
