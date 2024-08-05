#pragma once

#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"

#define CHANNEL 11
uint8_t controllerMAC[6] = {};
enum SensorMode {CENTRE, X, Y, Z, CONTROLLER} sensorMode = CENTRE; //Set for each board

void wifiInit(){
  //initialise wifi
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));
  uint8_t mac[6] = {};
  ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_AP, mac));
  Serial.printf();
}
