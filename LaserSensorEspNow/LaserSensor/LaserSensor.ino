#include "Adafruit_VL53L0X.h"
#define STA_MODE
#include "easy_esp_now.h"

//calculates the input data
Adafruit_VL53L0X laserSensor = Adafruit_VL53L0X();

//data collection with low-pass filter
const float distFilterWeight = 0.5f;
uint laserDist = 8191;
uint prevLaserDist = 8191;

//colour interpolation for DMX script
float distFactor = 0.0f;                //ranges 0-1
const float maxDist = 460;               //optional for cutting off the reading beyond a certain distance
const float maxDistPadding = 100;                //optional for allowing extra areas of max colour activation

//transition between off and on when laser sensor is able to detect something or not
float transitionFactor = 0.0f;          //ranges from 0-1
const float transitionSpeed = 0.25f;    //lerps transitionFactor to isWithinRange value (0-1)
bool prevInRange = false;

//////////////////////////////////////////////FUNCTIONS///////////////////////////////////////////////////////
static void send_cb(const uint8_t *mac_address, esp_now_send_status_t status) {
  send_cb_received = true;
}

void setup() {
  Serial.begin(115200);
  esp_now_setup();

  //initialise laser sensor
  if (!laserSensor.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  laserSensor.startRangeContinuous();
}

void loop() {
  if (laserSensor.isRangeComplete()) {
    uint actualDist = laserSensor.readRange();

    //transition between on and off when object is in/out of range (defined by maxDist, otherwise max dist hovers around 8191)
    bool isInRange = actualDist < maxDist;
    transitionFactor = transitionFactor + ((float)isInRange - transitionFactor) * transitionSpeed;
    transitionFactor = max(0.0f, min(1.0f, transitionFactor));
    
    if (isInRange){
      //reset the laserDist when begins reading again so it doesn't interpolate old data
      if(!prevInRange && transitionFactor > 0.08f){
        laserDist = actualDist;
        prevLaserDist = actualDist;
      }
      //interpolate data to give smoothing effect using a low-pass filter
      else {
        uint estimatedDist = distFilterWeight * distFilterWeight * prevLaserDist + distFilterWeight * (1.0f - distFilterWeight) * laserDist + (1.0f - distFilterWeight) * actualDist;
        prevLaserDist = laserDist;
        laserDist = estimatedDist;
      }

      //calculate distFactor for colour lerping in the DMX script
      distFactor = laserDist - maxDistPadding;
      distFactor = distFactor / (maxDist - maxDistPadding * 2);
      distFactor = max(0.0f, min(1.0f, distFactor));
    }
    prevInRange = isInRange;

    //debug
    Serial.print("Dist (mm): ");
    Serial.print(laserDist);
    Serial.print(" | Dist Factor: ");
    Serial.print(distFactor);
    Serial.print(" | Transition: ");
    Serial.print(transitionFactor);
    Serial.println();

    //send espNOW data
    if (send_cb_received){
      uint8_t *buf = (uint8_t *)malloc(8);
      memcpy(buf, &distFactor, 4);
      memcpy(buf + 4, &transitionFactor, 4);
      esp_now_send(APMac, buf, 8);
      free(buf);
    }
  }
}



