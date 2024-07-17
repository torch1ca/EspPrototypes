#include "Arduino.h"
#include "esp_dmx.h"
#define AP_MODE
#include "easy_esp_now.h"

//ESP NOW Data
struct Accel {
  int16_t x = 0;
  int16_t y = 0;
};
Accel accel = Accel();
bool espNowReceived = false;

//DMX Lights
#define DMX_COUNT 4
int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;
dmx_port_t dmxPort = 1;
#define DMX_PCT_SIZE 29
byte dmxData[DMX_PCT_SIZE] = {0};

//DMX Animations
float flashDemoColours[3][3] = {
  {255, 0, 215},
  {0, 255, 225},
  {255, 255, 255}
};

void recv_cb(const uint8_t *recv_info, const uint8_t *data, int dataLength){
  memcpy(&accel.x, data, 2);
  memcpy(&accel.y, data + 2, 2);
  // Serial.print("x = "); Serial.print(accel.x);
  // Serial.print(" | y = "); Serial.print(accel.y);
  // Serial.println();
}

void setDmxData() {
  for (int light = 0; light < DMX_COUNT; light++) {
    int col[3] = {0};
    
    int invert = (light == 1 || light == 3) ? 1 : -1;
    int colID = (int)floor(light*0.5f);
    float accelVal = colID == 0 ? accel.x : accel.y;
    // if (accelVal > 10000) accelVal = 10000;
    // if (accelVal < -10000) accelVal = -10000;


    col[0] = flashDemoColours[colID][0] * accelVal / 16000;
    col[0] = min(255, max(0, col[0] * invert));
    col[1] = flashDemoColours[colID][1] * accelVal / 16000;
    col[1] = min(255, max(0, col[1] * invert));
    col[2] = flashDemoColours[colID][2] * accelVal / 16000;
    col[2] = min(255, max(0, col[2] * invert));
    
    Serial.print(colID);
    Serial.println();

    for (int i = 0; i < 4; i++) {
      dmxData[light * 7 + 2 + i] = col[i];
    }
  }
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
