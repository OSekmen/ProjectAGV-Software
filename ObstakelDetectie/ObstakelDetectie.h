#pragma once
#include <Servo.h>
#include <Ultrasonic.h>
#include <HardwareSerial.h>
#include "States.h"

//#define DEBUG
uint8_t dummyDirection = 1;

#define NUM_MEASUREMENTS 15

class ObstakelDetectie {
private:
	Ultrasonic us;
	uint16_t measurements[NUM_MEASUREMENTS];
	uint8_t index;
	Servo servo;

public:
	bool servoAttached;

public:
	ObstakelDetectie() : us(Ultrasonic(0, 0)) {}

	/*
	Obstakel Detectie constructor

	@param trig, trigger pin.
	@param echo, echo pin.
	@param timeOut, time out voor ultrasone sensor
	@param servoPin, servo pin
	*/
	ObstakelDetectie(uint8_t trig, uint8_t echo, unsigned long timeOut = 20000UL, uint8_t servoPin = 0) 
		: us(Ultrasonic(trig, echo, timeOut)) {
		index = 0;
		uint16_t m = us.read();
		for (uint8_t i = 0; i < NUM_MEASUREMENTS; i++) {
			measurements[i] = m;
		}

		servoAttached = servoPin ? true : false;
		if (servoAttached) {
			servo.attach(servoPin);
		}
	}

	/*
	Leest de Ultrasone sensor uit en zet het in een array.

	@return Laatste individuele meting.
	*/
	unsigned int read() {
		if (index >= NUM_MEASUREMENTS) index = 0;
		measurements[index] = us.read();
		return measurements[index++];
	}

	/*
	Geeft de afstand uit een NUM_MEASUREMENTS aantal metingen.

	@return Gemiddelde meting.
	*/
	float distance() {
		float v = 0;
		for (uint8_t i = 0; i < NUM_MEASUREMENTS; i++) {
			v += measurements[i];

#ifdef DEBUG
			Serial.print(measurements[i]);
			if (i == NUM_MEASUREMENTS - 1) Serial.println();
			else Serial.print(" ");
#endif
		}
		v /= NUM_MEASUREMENTS;
		return v;
	}
};

// Obstakel module voorkant
ObstakelDetectie front;

// Obstakel module achterkant
ObstakelDetectie back;


void setupObstakelDetectie() {
	front = ObstakelDetectie(6, 7, 20000UL, 0);
	back = ObstakelDetectie(2, 2);
}

void updateObstakelDetectie() {
#ifdef DEBUG
	Serial.println(front.distance());
#endif

	front.read();
	back.read();
}