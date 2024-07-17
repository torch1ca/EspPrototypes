#define STA_MODE
#include "easy_esp_now.h"

#define IN_PIN 34

bool isHit = false;
int hitThresh = 50;
unsigned long hitCooldownTimer = 0;
unsigned long hitCooldownThresh = 200;
uint8_t data = 0;


static void send_cb(const uint8_t *mac_address, esp_now_send_status_t status){
  send_cb_received = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(IN_PIN, INPUT);
  esp_now_setup();
}

void loop() {
  float analogIn = analogRead(IN_PIN);

  if (analogIn > hitThresh && !isHit && (millis() - hitCooldownTimer) > hitCooldownThresh) {
    Serial.println("!!! HIT !!!");
    isHit == true;
    hitCooldownTimer = millis();
    data = 1;
  }
  else if (analogIn < hitThresh) {
    isHit = false; //currently does nothing because when you touch the piezo, it moves up and down and activates this before you release with your hand
  }

  Serial.println(analogIn);
  
  if (data > 0 && send_cb_received){
    uint8_t *buf = (uint8_t *)malloc(2);
    *buf = 255;
    *(buf + 1) = data;
    esp_now_send(APMac, buf, 2);
    free(buf);
    Serial.print("Data sent: ");
    Serial.println(data);
    data = 0;
  }

  delay(2);
}