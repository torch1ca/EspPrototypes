#include "Arduino.h"
#include "esp_dmx.h"
#define AP_MODE
#include "easy_esp_now.h"
#include "hsl2rgb.h"

//ESP NOW Data
float distFactor = 0.0f;
float transitionFactor = 0.0f;

//DMX Lights
#define DMX_COUNT 4
int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;
dmx_port_t dmxPort = 1;
#define DMX_PCT_SIZE 29
byte dmxData[DMX_PCT_SIZE] = {0};

//DMX Animations
float palette[3][3] = {
  {255, 0, 215}, //b=215
  {0, 225, 225},
  {255, 255, 255}
};

void recv_cb(const uint8_t *recv_info, const uint8_t *data, int dataLength){
  memcpy(&distFactor, data, 4);
  memcpy(&transitionFactor, data + 4, 4);
  // Serial.print(distFactor);
  // Serial.print(" | ");
  // Serial.println(transitionFactor);
}

int lerp(int a, int b, float t){
  return a + (b - a) * t;
}

void setDmxData() {
  int col[3] = {0};
  // int mag[3] = {palette[0][0], palette[0][1], palette[0][2]};
  // int cyan[3] = {palette[1][0], palette[1][1], palette[1][2]};

  // rgb2hsl(col);
  // rgb2hsl(mag);
  // rgb2hsl(cyan);

  col[0] = lerp(palette[0][0], palette[1][0], distFactor);
  col[1] = lerp(palette[0][1], palette[1][1], distFactor);
  col[2] = lerp(palette[0][2], palette[1][2], distFactor);

  col[0] = lerp(50, col[0], transitionFactor);
  col[1] = lerp(50, col[1], transitionFactor);
  col[2] = lerp(50, col[2], transitionFactor);

  // hsl2rgb(col);

  for (int light = 0; light < DMX_COUNT; light++) {
    for (int i = 0; i < 3; i++) {
      dmxData[light * 7 + 2 + i] = col[i];
    }
  }

  // rgb2hsl(col);
  // Serial.print("h="); Serial.print(col[0]);
  // Serial.print(" | s="); Serial.print(col[1]);
  // Serial.print(" | l="); Serial.print(col[2]);
  // Serial.println();
}

void setup() {
  Serial.begin(250000);
  esp_now_setup();
  
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_personality_t personalities[] = {};
  int personality_count = 0;
  dmx_driver_install(dmxPort, &config, personalities, personality_count);
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);


  dmxData[1] = 255;
  dmxData[8] = 255;
  dmxData[15] = 255;
  dmxData[22] = 255;
}

void loop() {
  setDmxData();

  dmx_wait_sent(dmxPort, DMX_PCT_SIZE);
  dmx_write(dmxPort, dmxData, DMX_PCT_SIZE);
  dmx_send_num(dmxPort, DMX_PCT_SIZE);
}
