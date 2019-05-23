//Pinnummers aan naam gekoppelt
int stepL = A2;
int dirL = A3;
int stepR = A0;
int dirR = A1;

int stepLState = LOW;
int stepRState = LOW;
unsigned long previousLMillis = 0;
unsigned long previousRMillis = 0;
long intervalL = 1000;
long intervalR = 500;

void setup() {
  //Pins als output declareren
  pinMode(stepL, OUTPUT);
  pinMode(dirL, OUTPUT);
  pinMode(stepR, OUTPUT);
  pinMode(dirR, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  unsigned long currentLMillis = millis();
  Serial.print("StepL: ");
  Serial.print(stepLState);
  if (currentLMillis - previousLMillis >= intervalL) {
    previousLMillis = currentLMillis;
    if (stepLState == LOW) {
      stepLState = HIGH;
    } else {
      stepLState = LOW;
    }
    digitalWrite(stepL, stepLState);
  }

  Serial.print("   ");

  unsigned long currentRMillis = millis();
  Serial.print("StepR: ");
  Serial.println(stepRState);
  if (currentRMillis - previousRMillis >= intervalR) {
    previousRMillis = currentRMillis;
    if (stepRState == LOW) {
      stepRState = HIGH;
    } else {
      stepRState = LOW;
    }
    digitalWrite(stepR, stepRState);
  }
}
