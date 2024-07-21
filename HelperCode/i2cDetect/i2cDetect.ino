#include <Wire.h>
#include <i2cdetect.h>

void setup(){
  Serial.begin(9600);
  Wire.begin();
}

void loop(){
 i2cdetect();
 delay(1000);
}