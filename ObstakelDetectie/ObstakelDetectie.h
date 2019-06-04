#pragma once
#include <Servo.h>
#include <Ultrasonic.h>
#include <HardwareSerial.h>
#include "States.h"

/*
LOGLEVEL 0: Geen logs
LOGLEVEL 1: State en afstand
LOGLEVEL 2: Gemiddelde
*/
#define LOGLEVEL 1

/*
Dummy variabelen moeten van andere software delen komen
*/
int8_t dummyDirection = 1; // -1 achteruit, 0 stilstand, 1 vooruit
int8_t dummyMode = 1; // 0 normaal, 1 volgmode
bool dummyBocht = false;
// ---------------------------------------------------------------------
const float stopDistance = 5;
const float followDistanceMargin = 1;
const uint32_t updateTime_ms = 1;
uint64_t US_millis;
State state;

#define NUM_MEASUREMENTS 10
#define NUM_SCANPOINTS 4
#define SCAN_FIELD_DEGREE 90
#define MILLIS_PER_DEGREE (1.0 + 2.0 / 3.0)
#define SCAN_MILLIS_MARGIN 10

float scanPoints[NUM_SCANPOINTS];

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

#if LOGLEVEL >= 2
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

#if LOGLEVEL >= 1
void printState() {
	switch (state) {
	case State::FORWARD:
		Serial.print("FORWARD");
		break;
	case State::BACKWARD:
		Serial.print("BACKWARD");
		break;
	case State::LEFT:
		Serial.print("LEFT");
		break;
	case State::RIGHT:
		Serial.print("RIGHT");
		break;
	case State::STOP:
		Serial.print("STOP");
		break;
	case State::SCANNING:
		Serial.print("SCANNING");
		break;
	}
}
#endif

void setupObstakelDetectie() {
	front = ObstakelDetectie(6, 7, updateTime_ms, 3);
	back = ObstakelDetectie(0, 0, updateTime_ms);

	US_millis = millis();
	front.servo.write(90);

	pinMode(32, INPUT_PULLUP);
}

uint32_t nextScanMillis;
uint16_t scanIndex;
uint16_t scanMeasureIndex;

void updateObstakelDetectie() {
	dummyBocht = !digitalRead(32);
	if (millis() - US_millis > updateTime_ms) {
		front.read();
		back.read();

		US_millis = millis();
		if (dummyMode == 0) { // normale mode
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

		else if (dummyMode == 1) { // volg mode
			float distance = front.distance();
			switch (state) {
			case State::FORWARD:
				// De AGV komt binnen volg afstand en moet daarvoor stoppen
				if (distance <= stopDistance + followDistanceMargin) {
					state = State::STOP;
				}

				// AGV komt aan bij een bocht, hier moet hij eerst stoppen voordat hij kan gaan scannen
				else if (dummyBocht) {
					state = State::STOP;
				}
				break;
			case State::BACKWARD:
				// De AGV komt binnen volg afstand en moet daarvoor stoppen
				if (distance >= stopDistance - followDistanceMargin) {
					state = State::STOP;
				}

				// De AGV kan niet verder achteruit rijden
				else if (back.distance() < stopDistance && dummyDirection == -1) {
					state = State::STOP;
				}
				break;
			case State::LEFT:
				break;
			case State::RIGHT:
				break;
			case State::STOP:
				// De AGV staat bij een bocht en de volgpersoon is naar links of rechts gegaan, nu moet de AGV gaan scannen
				if (dummyBocht && distance > 10) {
					state = State::SCANNING;
					for (uint8_t i = 0; i < NUM_SCANPOINTS; i++) {
						scanPoints[i] = 0;
					}
					scanMeasureIndex = 0;
					scanIndex = 0;
					float targetAngle = scanIndex * SCAN_FIELD_DEGREE / (NUM_SCANPOINTS - 1) - SCAN_FIELD_DEGREE / 2.0 + 90;
					nextScanMillis = millis() + abs(front.servo.read() - targetAngle) * MILLIS_PER_DEGREE + SCAN_MILLIS_MARGIN;
					front.servo.write(targetAngle);
				}

				// De AGV is te ver weg verwijderd van de volgpersoon en moet hierdoor vooruit
				else if (distance > stopDistance + followDistanceMargin && !dummyBocht) {
					state = State::FORWARD;
				}

				// De AGV komt te dichtbij de volgpersoon en moet hierdoor achteruit
				if (distance < stopDistance - followDistanceMargin) {
					state = State::BACKWARD;
				}
				break;
			case State::SCANNING:
				//TODO 2 soorten bochten (T- bocht van onder en T- bocht van boven)
				if (millis() >= nextScanMillis) {
					scanPoints[scanIndex] += distance / NUM_MEASUREMENTS;
					scanMeasureIndex++;

					if (scanMeasureIndex >= NUM_MEASUREMENTS) {
						scanMeasureIndex = 0;
						scanIndex++;

						if (scanIndex >= NUM_SCANPOINTS) {
							// DO THE MAGIC
							state = State::LEFT;
							front.servo.write(90);
							break;
						}
						
						float targetAngle = scanIndex * SCAN_FIELD_DEGREE / (NUM_SCANPOINTS - 1) - SCAN_FIELD_DEGREE / 2.0 + 90;
						nextScanMillis = millis() + abs(front.servo.read() - targetAngle) * MILLIS_PER_DEGREE + SCAN_MILLIS_MARGIN;
						front.servo.write(targetAngle);
					}
				}
				break;
			}
		}

#if LOGLEVEL >= 1
		Serial.print("State: ");
		printState();
		Serial.print(", Afstand: ");
		Serial.println(front.distance());
		for (uint8_t i = 0; i < NUM_SCANPOINTS; i++) {
			Serial.print(" ");
			Serial.print(scanPoints[i]);
		}
		Serial.println();
#endif
	}
}