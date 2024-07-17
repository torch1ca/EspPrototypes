// (c) Michael Schoeffler 2017, http://www.mschoeffler.de
#include "Wire.h"
#define STA_MODE
#include "easy_esp_now.h"

const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data

struct Accel {
  int16_t x = 0;
  int16_t y = 0;

  void populateBuffer(uint8_t *pBuf){    
    memcpy(pBuf, &x, 2);
    memcpy(pBuf + 2, &y, 2);
  }
};

static void send_cb(const uint8_t *mac_address, esp_now_send_status_t status){
  send_cb_received = true;
}

char tmp_str[7]; // temporary variable used in convert function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup() {
  Serial.begin(115200);

  esp_now_setup();
  esp_now_peer_info_t peer;
  ESP_ERROR_CHECK(esp_now_get_peer(APMac, &peer));
  Serial.println(peer.channel);

  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  
  Accel accel;
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accel.x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accel.y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  
  // // print out data
  // Serial.print("aX = "); Serial.print(convert_int16_to_str(accel.x));
  // Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accel.y));
  // Serial.println();
  
  if (send_cb_received){
    send_cb_received = false;
    uint8_t *pBuf = (uint8_t *) malloc(4);
    memcpy(pBuf, &accel.x, 2);
    memcpy(pBuf + 2, &accel.y, 2);
    esp_now_send(APMac, pBuf, sizeof(int));
    free(pBuf);

    Serial.print("aX = "); Serial.print(convert_int16_to_str(accel.x));
    Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accel.y));
    Serial.println();
  }
}