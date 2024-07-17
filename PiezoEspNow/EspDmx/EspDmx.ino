#include "Arduino.h"
#include "esp_dmx.h"
#define AP_MODE
#include "easy_esp_now.h"
#include "c:\Users\avery\Documents\Projects\Sword in Stone\Arduino\sword_animations.h"

//ESP NOW Data
uint8_t espNowData[4] = {};
bool espNowReceived = false;

//DMX Lights
#define DMX_COUNT 4
int transmitPin = 17;
int receivePin = 16;
int enablePin = 21;
dmx_port_t dmxPort = 1;
#define DMX_PCT_SIZE 29
byte dmxData[DMX_PCT_SIZE] = {};

//DMX Animations
float flashDemoColours[3][3] = {
  {255, 0, 215},
  {0, 255, 225},
  {255, 255, 255}
};
double flashTimeSave[DMX_COUNT];
float *pFlashCol[DMX_COUNT];



void recv_cb(const uint8_t *recv_info, const uint8_t *data, int dataLength){
  memcpy(espNowData, data, dataLength);
  espNowReceived = true;
}

void setDmxData() {
  for (int light = 0; light < DMX_COUNT; light++) {
    int col[3] = {0, 0, 0};
    int *pCol = col;//ManageLightAnims(light);
    if (pFlashCol[light] != NULL){
      if (SnapFlash(pCol, pFlashCol[light], flashTimeSave[light]) >= 1.0f){
        pFlashCol[light] = NULL;
        // Serial.println("\nEND");
      }
      // Serial.print(col[1]);
      // Serial.print(" ");
    }
    for (int i = 0; i < 4; i++) {
      dmxData[light * 7 + 2 + i] = col[i];
    }
  }
}

void setup() {
  // Serial.begin(250000);
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
  prevSeconds = seconds;
  seconds = millis() * 0.001;

  if (espNowReceived){
    espNowReceived = false;

    if (espNowData[0] == 255){
      for (int i=0; i<DMX_COUNT; i++){
        int light = i;
        if ((espNowData[1] & (1<<light)) > 0){
          //if (light % 2 == 1) light--;
          pFlashCol[light] = flashDemoColours[(int)(light/* * 0.5*/)];//flashColors[random(0,FLASH_COL_COUNT)];
          //pFlashCol[light+1] = flashDemoColours[(int)(light * 0.5)];//flashColors[random(0,FLASH_COL_COUNT)];
          flashTimeSave[light] = seconds;
          //flashTimeSave[light+1] = seconds;

          // Serial.print("\nRecieved ");
          // Serial.print(espNowData[1] & (1<<light));
          // Serial.print(" from light ");
          // Serial.print(light);
          // Serial.print(". Colour: [");
          // Serial.print(*pFlashCol[light]); Serial.print(", ");
          // Serial.print(*(pFlashCol[light]+1)); Serial.print(", ");
          // Serial.print(*(pFlashCol[light]+2)); Serial.print("]\n");
        }
      }
    }
    //bytes came from sword
    else{
      counter = espNowData[0];
    }
  }
  setDmxData();
  dmx_wait_sent(dmxPort, DMX_PCT_SIZE);
  dmx_write(dmxPort, dmxData, DMX_PCT_SIZE);
  dmx_send_num(dmxPort, DMX_PCT_SIZE);
}
