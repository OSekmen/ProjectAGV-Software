#pragma once
#include <Servo.h>
#include <Ultrasonic.h>
#include <HardwareSerial.h>
#include "States.h"

#define DEBUG
int8_t dummyDirection = 1;
int8_t dummyModus = 0;

#define NUM_MEASUREMENTS 15

class ObstakelDetectie {
private:
	Ultrasonic us;
	uint16_t measurements[NUM_MEASUREMENTS];
	uint8_t index;

public:
	bool servoAttached;
	Servo servo;

public:
	ObstakelDetectie() : us(Ultrasonic(0, 0)) {}

	/*
	Obstakel Detectie constructor

	@param trig, trigger pin.
	@param echo, echo pin.
	@param timeOut, time out voor ultrasone sensor
	@param servoPin, servo pin
	*/
	ObstakelDetectie(uint8_t trig, uint8_t echo, unsigned long timeOut = 20, uint8_t servoPin = 0) 
		: us(Ultrasonic(trig, echo, timeOut * 1000)) {
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

float stopDistance = 4;
const uint32_t updateTime_ms = 1000;
uint64_t US_millis;
State state;

void startVolgModus() {

}

void setupObstakelDetectie() {
	front = ObstakelDetectie(6, 7, updateTime_ms, 3);
	back = ObstakelDetectie(0, 0, updateTime_ms);

	US_millis = millis();
	front.servo.write(90);
}

void updateObstakelDetectie() {
	if (millis() - US_millis > updateTime_ms) {
		front.read();
		back.read();

		US_millis = millis();
		if (dummyModus == 0) { // normale modus
			front.servo.write(90);
			switch (dummyDirection) {
			case -1:
				if (back.distance() <= stopDistance) {
					state = State::STOP;
				}
				else {
					state = State::BACKWARD;
				}
				break;
			case 0:
				state = State::STOP;
				break;
			case 1:
				if (front.distance() <= stopDistance) {
					state = State::STOP;
				}
				else {
					state = State::FORWARD;
				}
				break;
			}
		}

		else if (dummyModus == 1) { // volg modus

		}

#ifdef DEBUG
		Serial.println(front.distance());
#endif
	}
}