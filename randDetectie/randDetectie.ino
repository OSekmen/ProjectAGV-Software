#include <VL53L0X.h>
#include <Wire.h>

VL53L0X sensorL;
VL53L0X sensorR;
long int distanceL;
long int distanceR;
long int distance;
long int left;
long int right;
int sensorLreset = 8;
int sensorRreset = 7;
int pwm;

void setup() {
  // put your setup code here, to run once:
  
  Wire.begin();

  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  //reset all sensors by pulling shutdownpins low and high (HIGH = active)
  digitalWrite(sensorLreset, LOW);
  digitalWrite(sensorRreset, LOW);
  delay(10);
  digitalWrite(sensorLreset, HIGH);
  digitalWrite(sensorRreset, HIGH);
  delay(10);

  //keep Left sensor on by pulling Right LOW, assign new adress
  digitalWrite(sensorRreset, LOW);
  sensorL.init();
  sensorL.setAddress(0x30);
  sensorL.setTimeout(500);

  hallo

  //repeat on Right Sensor
  digitalWrite(sensorRreset, HIGH);
  sensorR.init();
  sensorR.setAddress(0x31);
  sensorR.setTimeout(0);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  distanceL = sensorL.readRangeSingleMillimeters();
  distanceR = sensorR.readRangeSingleMillimeters();
  distance = ((abs(75-distanceL)+abs(75-distanceR))/2);
  pwm = map(distance,20,75,0,255);
  if (distanceL < 70 && distanceR > 80) {
    analogWrite(10, pwm);
    digitalWrite(9, LOW);
  } else if (distanceR < 70 && distanceL > 80) {
    analogWrite(9, pwm);
    digitalWrite(10, LOW);
  }
  else if (distanceL > 70 && distanceL < 80) {
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
  }

    Serial.print("distanceL: ");
    Serial.print(distanceL);
    Serial.print("  distanceR: ");
    Serial.println(distanceR);
    Serial.print("  distance: ");
    Serial.println(distance);
    /*
    Serial.print("75 -distance: ");
    Serial.print(75 - distance);
    Serial.print("  PWM: ");
    Serial.println(pwm);
*/
}
