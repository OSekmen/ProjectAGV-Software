/*
 Name:		Main.ino
 Created:	5/21/2019 1:37:04 PM
 Author:	Pim Stoopman
*/
#include <TimerOne.h>

#include <VL53L0X.h>
#include <Wire.h>

#include <Servo.h>
#include <Ultrasonic.h>
#include "ObstakelDetectie.h"

VL53L0X sensorL;
VL53L0X sensorR;

VL53L0X sensorArray[] = { sensorL, sensorR };

int stepL = 2;
int dirL = 5;
int stepR = 4;
int dirR = 7;

int ledState = LOW;
unsigned long previousMillis = 0;

unsigned int clck = 0;
unsigned int clckL = 0;
unsigned int clckR = 0;

int distanceL;
int distanceR;
int sensorLreset = 13;
int sensorRreset = 12;
int pwm;

void setup() {
	Serial.begin(9600);
	setupObstakelDetectie();

	Wire.begin();
	pinMode(stepL, OUTPUT);
	pinMode(dirL, OUTPUT);
	pinMode(stepR, OUTPUT);
	pinMode(dirR, OUTPUT);
	pinMode(sensorLreset, OUTPUT);
	pinMode(sensorRreset, OUTPUT);
	pinMode(8, OUTPUT);
	digitalWrite(8, LOW);

	//reset all sensors by pulling shutdownpins low and high (HIGH = active)
	digitalWrite(sensorLreset, LOW);
	digitalWrite(sensorRreset, LOW);
	delay(10);
	digitalWrite(sensorLreset, HIGH);
	digitalWrite(sensorRreset, HIGH);
	delay(10);


	//keep Left sensor on by pulling Right LOW, assign new adress
	digitalWrite(sensorRreset, LOW);
	sensorArray[0].init();
	sensorArray[0].setAddress(0x30);
	sensorArray[0].setTimeout(500);

	//repeat on Right Sensor
	digitalWrite(sensorRreset, HIGH);
	sensorArray[1].init();
	sensorArray[1].setAddress(0x31);
	sensorArray[1].setTimeout(0);

	Serial.begin(9600);

	Timer1.initialize(500); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second) 
	Timer1.attachInterrupt(timerIsr); // attach the service routine here
}

void timerIsr() {
	// Toggle LED  static unsigned long clck=0;

	clckL += 1;
	clckR += 1;
	clck += 1;

	if (clckL >= pwm) {
		digitalWrite(stepL, HIGH);
		digitalWrite(stepL, LOW);
		clckL = 0;
	}
	if (clckR >= 1) {
		digitalWrite(stepR, HIGH);
		digitalWrite(stepR, LOW);
		clckR = 0;
	}
}


void loop() {
	if (clck == 200) {
		pwm = map(sensorArray[0].readRangeSingleMillimeters(), 30, 150, 1, 15);
		clck = 0;
	}
	// the loop function runs over and over again until power down or reset
	updateObstakelDetectie();
}


