#define IN_PIN 34

bool isHit = false;
int hitThresh = 1200;
unsigned long hitCooldownTimer = 0;
unsigned long hitCooldownThresh = 230;

void setup() {
  Serial.begin(9600);
  //pinMode(IN_PIN, INPUT);
}

void loop() {
  float analogIn = analogRead(IN_PIN);

  if (analogIn > hitThresh && !isHit && (millis() - hitCooldownTimer) > hitCooldownThresh) {
    Serial.println("!!! HIT !!!");
    isHit == true;
    hitCooldownTimer = millis();
  }
  else if (analogIn < hitThresh) {
    isHit = false; //currently does nothing because when you touch the piezo, it moves up and down and activates this before you release with your hand
  }

  //Serial.println(analogIn);
  delay(5);
}
