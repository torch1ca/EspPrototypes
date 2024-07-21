#ifndef EASY_ESP_NOW
#define EASY_ESP_NOW

#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"

#define CHANNEL 11
uint8_t APMac[6] = { 0xD0, 0xEF, 0x76, 0x33, 0x78, 0x65  };

#ifdef STA_MODE
bool send_cb_received = true;

static void send_cb(const uint8_t *mac_address, esp_now_send_status_t status);
#else
static void recv_cb(const uint8_t *recv_info, const uint8_t *data, int len);
#endif

void wifi_init(){
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  #ifdef STA_MODE
  wifi_mode_t wifi_mode = WIFI_MODE_STA;
  #else
  wifi_mode_t wifi_mode = WIFI_MODE_AP;
  #endif
  ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_mode));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));
}

void esp_now_setup(){
  //initialise wifi
  wifi_init();

  //initialise ESPNOW and register cb functions
  ESP_ERROR_CHECK(esp_now_init());
  #ifdef STA_MODE
    ESP_ERROR_CHECK(esp_now_register_send_cb(send_cb));
  #else
    ESP_ERROR_CHECK(esp_now_register_recv_cb(recv_cb));
  #endif

  //add peer to peer list
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

#endif