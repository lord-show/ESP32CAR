#pragma once

// ===== Pin Definitions =====
#define SERVO_PIN       27
#define MOTOR_PIN_A     25
#define MOTOR_PIN_B     26

// ===== Servo Settings =====
#define SERVO_MIN_PULSE 700
#define SERVO_MAX_PULSE 2245
#define SERVO_CENTER    90

// ===== Communication =====
#define TCP_PORT        3333
#define SERIAL_BAUD     115200

// ===== Safety =====
#define FAILSAFE_TIMEOUT 500  // milliseconds