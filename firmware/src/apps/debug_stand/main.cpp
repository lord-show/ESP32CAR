// Only wifi and TCP server code, no car control logic here. This is a simple debug receiver that can be used to test the communication between the PC client and the ESP32. It can also be used to visualize the control inputs in the Arduino Plotter.

#include <Arduino.h>
#include <WiFi.h>

#include "secrets.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

WiFiServer server(3333);

const uint8_t HEADER = 0xAA;
const uint8_t PACKET_TYPE_CONTROL = 0x01;

bool plotterMode = false;   // true → Arduino Plotter


void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== ESPCar Debug Receiver ===");

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

  Serial.println("TCP server started");
  Serial.println("Waiting for PC client...");
}


void parsePacket(uint8_t *packet) {

  if (packet[0] != HEADER) {
    Serial.println("Invalid header");
    return;
  }

  uint8_t type = packet[1];

  if (type != PACKET_TYPE_CONTROL) {
    Serial.println("Unknown packet type");
    return;
  }

  uint8_t checksum = 0;

  for (int i = 0; i < 9; i++) {
    checksum += packet[i];
  }

  checksum = checksum % 256;

  if (checksum != packet[9]) {
    Serial.println("Checksum error");
    return;
  }

  int16_t steer =
    (int16_t)(packet[2] | (packet[3] << 8));

  int16_t throttle =
    (int16_t)(packet[4] | (packet[5] << 8));

  int16_t brake =
    (int16_t)(packet[6] | (packet[7] << 8));

  uint8_t buttons = packet[8];


  if (plotterMode) {

    Serial.print(steer);
    Serial.print(",");
    Serial.print(throttle);
    Serial.print(",");
    Serial.println(brake);

  } else {

    Serial.print("Steer: ");
    Serial.print(steer);

    Serial.print("  Throttle: ");
    Serial.print(throttle);

    Serial.print("  Brake: ");
    Serial.print(brake);

    Serial.print("  Buttons: ");
    Serial.println(buttons, BIN);
  }
}



void loop() {

  WiFiClient client = server.available();

  if (!client) {
    delay(10);
    return;
  }

  Serial.println("Client connected");
  Serial.print("Client IP: ");
  Serial.println(client.remoteIP());

  while (client.connected()) {

    if (client.available() >= 10) {

      uint8_t packet[10];

      client.read(packet, 10);

      parsePacket(packet);
    }
  }

  Serial.println("Client disconnected");
  client.stop();
}



