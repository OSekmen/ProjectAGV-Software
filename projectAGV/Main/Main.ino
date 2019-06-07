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
#include "ObstakelDetectie.h"
#include "Navigatie.h"

void setup() {
	Serial.begin(9600);

	//setupAandrijving();
	setupObstakelDetectie();
	setup_ToF_Detectie();
	setupNavigatie();
}


void loop() {

	/*
	Hier komt de navigatie switch
	*/


	//loopAandrijving();
	loopObstakelDetectie();
	loop_ToF_Detectie();
	//loopNavigatie();
}