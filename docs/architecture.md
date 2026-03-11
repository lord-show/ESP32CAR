# ESPCar Architecture

## Overview

ESPCar — это система управления RC-автомобилем с использованием игрового руля.
Архитектура разделена на две основные части:

* **PC Control Station** — обработка пользовательского ввода и отправка команд.
* **ESP32 Vehicle Controller** — управление исполнительными механизмами автомобиля.

Связь осуществляется по **TCP через WiFi**.

---

# System Architecture

```mermaid
flowchart LR

Wheel[Steering Wheel & Pedals]
PC[PC Control Station]
WiFi[WiFi Network]
ESP[ESP32 Vehicle Controller]

Servo[Steering Servo]
Motor[Drive Motor]

Wheel --> PC
PC --> WiFi
WiFi --> ESP
ESP --> Servo
ESP --> Motor
```

---

# High-Level Software Architecture

```mermaid
flowchart LR

subgraph PC["PC Control Station"]
Input[Wheel Input]
Mapper[Control Mapper]
Protocol[Packet Encoder]
Network[TCP Client]
GUI[Dashboard GUI]
end

subgraph Vehicle["ESP32 Vehicle Controller"]
Server[TCP Server]
Parser[Packet Parser]
Controller[Vehicle Controller]
ServoDriver[Servo Driver]
MotorDriver[Motor Driver]
Safety[Watchdog Safety]
end

Input --> Mapper
Mapper --> Protocol
Protocol --> Network
Network --> Server
Server --> Parser
Parser --> Controller

Controller --> ServoDriver
Controller --> MotorDriver

Controller --> Safety
```

---

# PC Application Architecture

```mermaid
flowchart TD

main[main.py]

subgraph Input
wheel[wheel_input.py]
end

subgraph Control
mapper[mapper.py]
end

subgraph Protocol
packet[control_packet.py]
end

subgraph Network
tcp[tcp_client.py]
end

subgraph GUI
dashboard[dashboard.py]
end

wheel --> mapper
mapper --> packet
packet --> tcp

wheel --> dashboard

main --> wheel
main --> mapper
main --> packet
main --> tcp
main --> dashboard
```

---

# PC Runtime Data Flow

```mermaid
sequenceDiagram

participant Wheel
participant Input
participant Mapper
participant Protocol
participant Network
participant ESP32

Wheel->>Input: axis values
Input->>Mapper: wheel state
Mapper->>Protocol: control command
Protocol->>Network: encoded packet
Network->>ESP32: TCP packet
```

Packet format (v1):

```
steer,throttle,brake\n
```

Example:

```
0.120,0.850,0.000
```

---

# ESP32 Firmware Architecture

```mermaid
flowchart TD

main[main.cpp]

subgraph Network
tcp_server[tcp_server]
end

subgraph Protocol
parser[control_parser]
end

subgraph Control
vehicle[vehicle_controller]
end

subgraph Drivers
servo[steering_servo]
motor[motor_driver]
end

subgraph Safety
watchdog[connection_watchdog]
end

tcp_server --> parser
parser --> vehicle

vehicle --> servo
vehicle --> motor
vehicle --> watchdog

main --> tcp_server
main --> vehicle
```

---

# ESP32 Control Loop

```mermaid
flowchart LR

Packet[TCP Packet Received]
Parse[Parse Control Packet]

Steer[Update Steering Servo]
Throttle[Update Motor PWM]

Packet --> Parse
Parse --> Steer
Parse --> Throttle
```

---

# Safety Mechanism

ESP32 реализует механизм безопасности:

* если **пакеты управления не приходят более 200 ms**
* система автоматически:

```
motor = stop
servo = center
```

```mermaid
flowchart TD

Packets[Control Packets]
Timer[Watchdog Timer]
SafeState[Safe Stop]

Packets --> Timer

Timer -->|timeout| SafeState
```

---

# Future Extensions

Архитектура проектируется с учётом расширения системы.

Планируемые модули:

* Video Streaming
* Telemetry
* Autonomous Driving
* Recording and Replay
* Force Feedback

```mermaid
flowchart LR

PC[PC Control Station]

Video[Video Stream]
Telemetry[Telemetry Channel]
Autopilot[Autonomous Driving]

PC --> Video
PC --> Telemetry
PC --> Autopilot
```

---

# Repository Structure

```mermaid
flowchart TD

Repo[ESPCar Repository]

Repo --> PC[pc/]
Repo --> Firmware[firmware/]
Repo --> Docs[docs/]
Repo --> Tools[tools/]

PC --> Input[input]
PC --> Network[network]
PC --> Protocol[protocol]
PC --> GUI[gui]

Firmware --> Drivers[drivers]
Firmware --> Control[control]
Firmware --> NetworkF[network]
Firmware --> Safety[safety]
```

---

# Control Frequency

| Component     | Frequency |
| ------------- | --------- |
| Wheel Input   | ~100 Hz   |
| Network Send  | ~50 Hz    |
| GUI Update    | ~30 FPS   |
| Servo Control | ~50 Hz    |

---

# Design Goals

* Простая отладка
* Чёткое разделение модулей
* Расширяемость
* Безопасность управления
* Минимальная задержка управления

---
