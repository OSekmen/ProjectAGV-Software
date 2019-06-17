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

void printDirection() {
	switch (direction) {
	case Direction::BACKWARDS:
		Serial.print("BACKWARDS");
		break;
	case Direction::STOP:
		Serial.print("BACKWARDS");
		break;
	case Direction::FORWARDS:
		Serial.print("BACKWARDS");
		break;
	case Direction::LEFT:
		Serial.print("BACKWARDS");
		break;
	case Direction::RIGHT:
		Serial.print("BACKWARDS");
		break;
	}
}

void setup() {
	Serial.begin(9600);

	//setupObstakelDetectie();
	setup_ToF_Detectie();
	setupNavigatie();
	setupAandrijving();
}


void loop() {
	//loopObstakelDetectie();
	loop_ToF_Detectie();
	loopNavigatie();
	loopAandrijving();

#if LOGLEVEL > 0
	Serial.print("pos: ");
	Serial.print(pos.x);
	Serial.print(", ");
	Serial.print(pos.y);
	Serial.print(", target: ");
	Serial.print(target.x);
	Serial.print(", ");
	Serial.println(target.y);
#endif
}