#pragma once

#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"

#define CHANNEL 11
uint8_t macAddresses[4][6] = {};


bool send_cb_received = true;
static void send_cb(const uint8_t *mac_address, esp_now_send_status_t status){}
bool recv_cb_received = true;
static void recv_cb(const uint8_t *recv_info, const uint8_t *data, int len){}

void espNowInit(){
  //initialise wifi
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));

  //initialise ESPNOW and register cb functions
  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_send_cb(send_cb));
  ESP_ERROR_CHECK(esp_now_register_recv_cb(recv_cb));
}

void peerInit(){
  esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));
  memset(peer, 0, sizeof(esp_now_peer_info_t));
  peer->channel = CHANNEL;
  peer->encrypt = false;
  #ifdef STA_MODE
  memcpy(peer->peer_addr, APMac, 6);
  ESP_ERROR_CHECK(esp_now_add_peer(peer));
  #endif
  free(peer);
}
