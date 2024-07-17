#define IN_PIN 34

int hitDurTimer = 0;
int hitDurThresh = 80;
unsigned long hitCooldownTimer = 0;
unsigned long hitCooldownThresh = 300;

void setup() {
  Serial.begin(250000);
  pinMode(IN_PIN, INPUT);
}

void loop() {
  float analogIn = analogRead(IN_PIN);

  if (analogIn == 0.0f){
    hitDurTimer++;
  }
  else {
    if ((hitDurTimer > hitDurThresh) && ((millis() - hitCooldownTimer) > hitCooldownThresh)) {
      Serial.print(hitDurTimer);
      Serial.println(" - HIT!!");
      hitCooldownTimer = millis();
    }
    hitDurTimer = 0;
  }

  //Serial.println(analogIn);
}
