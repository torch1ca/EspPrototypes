#define CLK 35
#define DT 34
volatile int prevRotState;
volatile int rotCounter = 0;
int maxRot = 20;
int minRot = 0;

void EncoderValue() {
  int currentRotState = digitalRead(CLK);
  if (currentRotState != prevRotState) {
    if (digitalRead(DT) == currentRotState) {
      rotCounter = max(rotCounter - 1, minRot);
    } else {
      rotCounter = min(rotCounter + 1, maxRot);
    }
  }
  prevRotState = currentRotState;
}

void setup() {
  Serial.begin(115200);

  //Initialise the Rotary Encoder
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  prevRotState = digitalRead(CLK);
  attachInterrupt(digitalPinToInterrupt(CLK), EncoderValue, CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(rotCounter);
}
