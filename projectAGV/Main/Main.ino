/*
 Name:		Main.ino
 Created:	5/21/2019 1:37:04 PM
 Author:	Pim Stoopman
*/
#include <TimerOne.h>

#include <VL53L0X.h>
#include <Wire.h>

#include <Servo.h>

#include "Aandrijving.h"
#include "ToF_Detectie.h"
#include "ObstakelDetectie.h"
#include "Navigatie.h"
#include "Communicatie.h"

void setup() {
	Serial.begin(9600);

	pinMode(NOODSTOP, INPUT);

	setupObstakelDetectie();
	setup_ToF_Detectie();
	setupNavigatie();
	setupAandrijving();
	setupCommunicatie();
}

uint32_t nextMillis;

void loop() {
	loopObstakelDetectie();
	loop_ToF_Detectie();
	loopNavigatie();
	loopAandrijving();
	loopCommunicatie();

#if LOGLEVEL > 0
	if (millis() >= nextMillis) {
		Serial.print("pos: ");
		Serial.print(pos.x);
		Serial.print(", ");
		Serial.print(pos.y);
		Serial.print(", Angle: ");
		Serial.print(AGV_Angle_RAD);
		Serial.print(", target: ");
		Serial.print(target.x);
		Serial.print(", ");
		Serial.print(target.y);
		Serial.print(", Direction: ");
		Serial.print(printDirection(direction));
		Serial.print(", nav: ");
		printNavstate(navState);
		Serial.print(", aandrijving: ");
		printAandrijving(aandrijvingMode);
		Serial.print(", US_front: ");
		Serial.print(US_front->distance());
		Serial.print(", US_rear: ");
		Serial.print(US_rear->distance());
		Serial.println();
		nextMillis = millis() + 50;
	}
#endif
}
