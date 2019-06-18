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

#include "Aandrijving.h"
#include "ToF_Detectie.h"
//#include "ObstakelDetectie.h"
#include "Navigatie.h"

void setup() {
	Serial.begin(9600);

	pinMode(NOODSTOP, INPUT);

	//setupObstakelDetectie();
	setup_ToF_Detectie();
	setupNavigatie();
	setupAandrijving();
}

uint32_t nextMillis;

void loop() {
	//loopObstakelDetectie();
	loop_ToF_Detectie();
	loopNavigatie();
	loopAandrijving();
}
//#if LOGLEVEL > 0
//	if (millis() >= nextMillis) {
//		Serial.print("pos: ");
//		Serial.print(pos.x);
//		Serial.print(", ");
//		Serial.print(pos.y);
//		Serial.print(", target: ");
//		Serial.print(target.x);
//		Serial.print(", ");
//		Serial.print(target.y);
//		Serial.print(", Direction: ");
//		printDirection(direction);
//		Serial.println();
//
//		nextMillis = millis() + 500;
//	}
//	
//#endif
