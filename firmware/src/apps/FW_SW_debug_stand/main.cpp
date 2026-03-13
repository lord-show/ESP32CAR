#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <FS_MX1508.h>
#include "secrets.h"

// TODO: Добавить из config.h

// ===== WiFi =====
WiFiServer server(3333);
WiFiClient client;

// ===== Servo =====
Servo steeringServo;
const int SERVO_PIN = 27;

// ===== Motor =====
#define MOTOR_PIN_A 25
#define MOTOR_PIN_B 26
MX1508 driveMotor(MOTOR_PIN_A, MOTOR_PIN_B);

// ===== Control values =====
float steer = 0;
float throttle = 0;
float brake = 0;

// ===== FailSafe =====
unsigned long lastPacketTime = 0;
const int FAILSAFE_TIMEOUT = 1000;
bool failsafeActive = false;

// ===== Packet structure (10 bytes total) =====
#pragma pack(push, 1)
struct ControlPacket {
    uint8_t header;      // 0xAA
    uint8_t type;        // packet type
    int16_t steer;       // -1000 to 1000
    int16_t throttle;    // 0 to 1000
    int16_t brake;       // 0 to 1000
    uint8_t buttons;     // bitmask for buttons
    uint8_t checksum;    // sum of all previous bytes % 256
};
#pragma pack(pop)

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== ESPCar Debug Stand ===");

    // WiFi connect
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Connecting WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.println("TCP server started");

    // Servo init
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    steeringServo.setPeriodHertz(50);
    steeringServo.attach(SERVO_PIN, 544, 2400);
    steeringServo.write(90);

    Serial.println("Servo ready");
    Serial.println("Motor ready");
    Serial.print("Packet size: ");
    Serial.println(sizeof(ControlPacket));
}

void loop() {
    // === Client connect ===
    if (!client || !client.connected()) {
        client = server.available();

        if (client) {
            Serial.println("Controller connected");
            lastPacketTime = millis();
        }

        return;
    }

    // === Receive and parse binary packet ===
    if (client.available() >= sizeof(ControlPacket)) {
        ControlPacket packet;
        
        // Read the entire packet
        client.read((uint8_t*)&packet, sizeof(packet));
        
        // Calculate checksum (sum of all bytes except the last one)
        uint8_t calcChecksum = 0;
        uint8_t* packetBytes = (uint8_t*)&packet;
        
        for (int i = 0; i < sizeof(ControlPacket) - 1; i++) {
            calcChecksum += packetBytes[i];
        }
        calcChecksum = calcChecksum % 256;
        
        // Verify checksum
        if (calcChecksum != packet.checksum) {
            Serial.println("Checksum error");
            return;
        }
        
        // Verify header
        if (packet.header != 0xAA) {
            Serial.print("Header error: 0x");
            Serial.println(packet.header, HEX);
            return;
        }
        
        // === Decode values ===
        steer = packet.steer / 1000.0f;           // Convert to -1.0 .. 1.0
        throttle = packet.throttle / 1000.0f;      // Convert to 0.0 .. 1.0
        brake = packet.brake / 1000.0f;            // Convert to 0.0 .. 1.0
        
        lastPacketTime = millis();
        failsafeActive = false;
        
        // === Debug output ===
        Serial.print("STEER:");
        Serial.print(steer, 3);
        Serial.print(" THR:");
        Serial.print(throttle, 3);
        Serial.print(" BRK:");
        Serial.print(brake, 3);
        Serial.print(" BTNS:0x");
        Serial.print(packet.buttons, HEX);
        Serial.print(" TYPE:");
        Serial.println(packet.type);
        
        // Optional: raw values debug
        // Serial.print("Raw steer: ");
        // Serial.println(packet.steer);
        
        // ===== Steering control =====
        int servoPos = map(packet.steer, -1000, 1000, 0, 180);
        steeringServo.write(servoPos);
        
        // ===== Motor control =====
        int motorSpeed = 0;
        
        if (packet.throttle > 0) {
            // Forward: throttle range 0-1000 -> 0-200
            motorSpeed = packet.throttle / 5;  // 1000/5 = 200
        }
        
        if (packet.brake > 0) {
            // Reverse/Brake: brake range 0-1000 -> -200-0
            motorSpeed = -(packet.brake / 5);  // -1000/5 = -200
        }
        
        driveMotor.motorGo(motorSpeed);
        
        // Optional: motor debug
        // Serial.print(" Motor PWM:");
        // Serial.println(motorSpeed);
    }

    // === FAILSAFE ===
    if (millis() - lastPacketTime > FAILSAFE_TIMEOUT) {
        if (!failsafeActive) {
            Serial.println("FAILSAFE ACTIVATED - No signal");
            
            // steeringServo.write(90);        // Center steering
            driveMotor.motorStop();          // Stop motor
            
            failsafeActive = true;
        }
    }
}