#include <Arduino.h>

#include "modrob.h"
#include "multi_uart.h"

//Wifi UDP
#include "WiFi.h"
#include <WiFiUdp.h>

// --- Wifi parameters ---
const char* wifi_ssid = "ModRob";
const char* wifi_pass = "test1234";
const uint16_t wifi_port = 9999;
WiFiUDP udp;
uint8_t udp_buffer_rx[255];
uint8_t udp_buffer_tx[255];

// === Module parameters ===
ModRob module;
uint8_t module_attributes[] = {0,2, 16,2, 16,(uint8_t)-2, 0,(uint8_t)-2};
// === ports configuations ===
//TX pin for all ports
#define TX_PIN    33
//port 0
#define RX_PIN_0  32
uint8_t port0_location[] = {0,0,1,0};
//port 1
#define RX_PIN_1  17
uint8_t port1_location[] = {16,0,(uint8_t)-1,0};


void setup() {
    //Debug
    Serial.begin(115200);

    //Wifi setup
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setMinSecurity(WIFI_AUTH_OPEN);
    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.println(WiFi.localIP()[3]);
    udp.begin(wifi_port);

    //module setup
    module.setup(WiFi.localIP()[3], sizeof(port0_location));
    module.set_port_tx(TX_PIN);
    module.add_port_rx(RX_PIN_0, port0_location);
    module.add_port_rx(RX_PIN_1, port1_location);
    module.set_module_attributes(module_attributes, sizeof(module_attributes));
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
      //Serial.println("packet received");
      int len = udp.read(udp_buffer_rx, 0xFF);

      uint16_t tx_len = module.process_udp(udp_buffer_rx, len, udp_buffer_tx);
      Serial.println(udp.remoteIP());
      Serial.println(udp.remotePort());
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write(udp_buffer_tx, tx_len);
      udp.endPacket();
  }
}