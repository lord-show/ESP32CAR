#include <Arduino.h>
#include <ESP32Servo.h>
#include <FS_MX1508.h>
#include "config.h"

// ===== Servo =====
Servo steeringServo;
// const int SERVO_PIN = 27;

// ===== Motor =====
// #define MOTOR_PIN_A 25
// #define MOTOR_PIN_B 26
MX1508 driveMotor(MOTOR_PIN_A, MOTOR_PIN_B);

// ===== Current state =====
int currentServoPos = SERVO_CENTER;
int currentMotorSpeed = 0;

// ===== Command buffer =====
String inputBuffer = "";

void printHelp() {
    Serial.println();
    Serial.println("=== Available Commands ===");
    Serial.println();
    Serial.println("SERVO CONTROL:");
    Serial.println("  servo <0-180>    - Set servo position (or 's')");
    Serial.println("  servo            - Show current servo position");
    Serial.println("  center           - Center servo at 90° (or 'c')");
    Serial.println("  sweep            - Run servo sweep test");
    Serial.println();
    Serial.println("MOTOR CONTROL:");
    Serial.println("  motor <-200-200> - Set motor speed (or 'm')");
    Serial.println("  motor            - Show current motor speed");
    Serial.println("  stop             - Stop motor");
    Serial.println("  spin             - Run basic motor test");
    Serial.println("  ramp             - Run gradual motor test");
    Serial.println();
    Serial.println("GENERAL:");
    Serial.println("  default          - Reset to default state (servo 90°, motor 0)");
    Serial.println("  status           - Show current status (or 'st')");
    Serial.println("  help             - Show this help (or 'h' or '?')");
    Serial.println();
}


void processCommand(String cmd) {
    cmd.trim();
    
    if (cmd.length() == 0) return;
    
    // Convert to lowercase for case-insensitive comparison
    String lowerCmd = cmd;
    lowerCmd.toLowerCase();
    
    // Parse command and parameter
    int spaceIndex = cmd.indexOf(' ');
    String command = (spaceIndex == -1) ? cmd : cmd.substring(0, spaceIndex);
    String param = (spaceIndex == -1) ? "" : cmd.substring(spaceIndex + 1);
    
    command.toLowerCase();
    
    // === HELP ===
    if (command == "help" || command == "h" || command == "?") {
        printHelp();
    }
    
    // === SERVO ===
    else if (command == "servo" || command == "s") {
        if (param.length() > 0) {
            int pos = param.toInt();
            if (pos >= 0 && pos <= 180) {
                steeringServo.write(pos);
                currentServoPos = pos;
                Serial.print("✓ Servo set to ");
                Serial.print(pos);
                Serial.println("°");
            } else {
                Serial.println("❌ Servo position must be 0-180");
            }
        } else {
            Serial.print("Current servo position: ");
            Serial.print(currentServoPos);
            Serial.println("°");
        }
    }
    
    // === MOTOR ===
    else if (command == "motor" || command == "m") {
        if (param.length() > 0) {
            int speed = param.toInt();
            if (speed >= -200 && speed <= 200) {
                driveMotor.motorGo(speed);
                currentMotorSpeed = speed;
                Serial.print("✓ Motor speed set to ");
                Serial.println(speed);
                
                if (speed > 0) {
                    Serial.println("  Direction: FORWARD");
                } else if (speed < 0) {
                    Serial.println("  Direction: BACKWARD");
                } else {
                    Serial.println("  Direction: STOP");
                }
            } else {
                Serial.println("❌ Motor speed must be -200 to 200");
                Serial.println("  Positive = forward, Negative = backward");
            }
        } else {
            Serial.print("Current motor speed: ");
            Serial.println(currentMotorSpeed);
        }
    }
    
    // === DEFAULT / RESET ===
    else if (command == "default" || command == "d" || command == "reset" || command == "home") {
        steeringServo.write(90);
        currentServoPos = 90;
        
        driveMotor.motorStop();
        currentMotorSpeed = 0;
        
        Serial.println("✓ Reset to default state:");
        Serial.println("  Servo: 90°");
        Serial.println("  Motor: STOP (0)");
    }
    
    // === SWEEP (servo test) ===
    else if (command == "sweep") {
        Serial.println("🔄 Running servo sweep test...");
        for (int pos = 0; pos <= 180; pos += 10) {
            steeringServo.write(pos);
            Serial.print(".");
            delay(200);
        }
        for (int pos = 180; pos >= 0; pos -= 10) {
            steeringServo.write(pos);
            Serial.print(".");
            delay(200);
        }
        steeringServo.write(90);
        currentServoPos = 90;
        Serial.println(" Done!");
        Serial.println("✓ Servo returned to 90°");
    }
    
    // === SPIN (motor test) ===
    else if (command == "spin") {
        Serial.println("🔄 Running motor spin test...");
        
        Serial.println("  Forward at 100");
        driveMotor.motorGo(100);
        currentMotorSpeed = 100;
        delay(2000);
        
        Serial.println("  Stop");
        driveMotor.motorGo(0);
        currentMotorSpeed = 0;
        delay(1000);
        
        Serial.println("  Backward at -100");
        driveMotor.motorGo(-100);
        currentMotorSpeed = -100;
        delay(2000);
        
        Serial.println("  Stop");
        driveMotor.motorStop();
        currentMotorSpeed = 0;
        
        Serial.println("✓ Motor test complete");
    }
    
    // === RAMP (gradual motor test) ===
    else if (command == "ramp") {
        Serial.println("📈 Running motor ramp test...");
        
        // Ramp up forward
        for (int speed = 0; speed <= 200; speed += 20) {
            driveMotor.motorGo(speed);
            currentMotorSpeed = speed;
            Serial.print("  Speed: ");
            Serial.println(speed);
            delay(500);
        }
        
        // Ramp down forward
        for (int speed = 180; speed >= 0; speed -= 20) {
            driveMotor.motorGo(speed);
            currentMotorSpeed = speed;
            Serial.print("  Speed: ");
            Serial.println(speed);
            delay(500);
        }
        
        // Ramp up backward
        for (int speed = 0; speed >= -200; speed -= 20) {
            driveMotor.motorGo(speed);
            currentMotorSpeed = speed;
            Serial.print("  Speed: ");
            Serial.println(speed);
            delay(500);
        }
        
        // Ramp down backward
        for (int speed = -180; speed <= 0; speed += 20) {
            driveMotor.motorGo(speed);
            currentMotorSpeed = speed;
            Serial.print("  Speed: ");
            Serial.println(speed);
            delay(500);
        }
        
        driveMotor.motorStop();
        currentMotorSpeed = 0;
        Serial.println("✓ Ramp test complete");
    }
    
    // === CENTER ===
    else if (command == "center" || command == "c") {
        steeringServo.write(90);
        currentServoPos = 90;
        Serial.println("✓ Servo centered at 90°");
    }
    
    // === STOP ===
    else if (command == "stop") {
        driveMotor.motorStop();
        currentMotorSpeed = 0;
        Serial.println("✓ Motor stopped");
    }
    
    // === STATUS ===
    else if (command == "status" || command == "st") {
        Serial.println("=== Current Status ===");
        Serial.print("Servo position: ");
        Serial.print(currentServoPos);
        Serial.println("°");
        
        Serial.print("Motor speed: ");
        Serial.print(currentMotorSpeed);
        
        if (currentMotorSpeed > 0) {
            Serial.println(" (FORWARD)");
        } else if (currentMotorSpeed < 0) {
            Serial.println(" (BACKWARD)");
        } else {
            Serial.println(" (STOP)");
        }
    }
    
    // === UNKNOWN COMMAND ===
    else {
        Serial.print("❌ Unknown command: '");
        Serial.print(cmd);
        Serial.println("'");
        Serial.println("Type 'help' for available commands");
    }
}


void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);
    
    Serial.println("\n=================================");
    Serial.println("ESPCar Debug Console");
    Serial.println("=================================");
    Serial.println();
    
    // Servo init
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    steeringServo.setPeriodHertz(50);
    steeringServo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
    steeringServo.write(SERVO_CENTER);
    
    Serial.println("✓ Servo initialized at 90°");
    Serial.println("✓ Motor initialized");
    Serial.println();
    
    printHelp();
    Serial.println();
    Serial.print("> ");
}

void loop() {
    // Read serial input
    while (Serial.available()) {
        char c = Serial.read();
        
        if (c == '\n') {
            // Process command
            processCommand(inputBuffer);
            inputBuffer = "";
            Serial.print("> ");
        } else {
            inputBuffer += c;
        }
    }
}