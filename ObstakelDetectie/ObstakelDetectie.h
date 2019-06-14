#pragma once
#include <Servo.h>
#include <Ultrasonic.h>
#include <HardwareSerial.h>
#include "States.h"
#include "Pins.h"
#include "GlobalVariables.h"

/*
Dummy variabelen moeten van andere software delen komen
*/
//int8_t dummyDirection = 1; // -1 achteruit, 0 stilstand, 1 vooruit
//int8_t dummyMode = 1; // 0 normaal, 1 volgmode
bool dummyBocht = false;
// ---------------------------------------------------------------------
const float stopDistance = 5;
const float followDistanceMargin = 1;
const uint32_t updateTime_ms = 1;
uint64_t US_millis;

#define NUM_SCANPOINTS 4
#define SCAN_FIELD_DEGREE 90
#define MILLIS_PER_DEGREE (1.0 + 2.0 / 3.0)
#define SCAN_MILLIS_MARGIN 10

float scanPoints[NUM_SCANPOINTS];

#if LOGLEVEL >= 1
void printState() {
	switch (obstakelState) {
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
	front = ObstakelDetectie(FRONT_TRIGGER, FRONT_ECHO, updateTime_ms, SERVO);
	back = ObstakelDetectie(BACK_TRIGGER, BACK_ECHO, updateTime_ms);

	US_millis = millis();
	front.servo.write(90);

	pinMode(32, INPUT_PULLUP);
}

uint32_t nextScanMillis;
uint16_t scanIndex;
uint16_t scanMeasureIndex;

void loopObstakelDetectie() {
	dummyBocht = !digitalRead(32);
	if (millis() - US_millis > updateTime_ms) {
		front.read();
		back.read();

		US_millis = millis();
		if (mode == Mode::NORMAAL) { // normale mode
			front.servo.write(90);
			switch (direction) {
			case Direction::BACKWARD:
				if (back.distance() <= stopDistance) {
					obstakelState = State::STOP;
				}
				else {
					obstakelState = State::BACKWARD;
				}
				break;
			case Direction::STOP:
				obstakelState = State::STOP;
				break;
			case Direction::FORWARD:
				if (front.distance() <= stopDistance) {
					obstakelState = State::STOP;
				}
				else {
					obstakelState = State::FORWARD;
				}
				break;
			}
		}

		else if (mode == Mode::VOLGEN) { // volg mode
			float distance = front.distance();
			switch (obstakelState) {
			case State::FORWARD:
				// De AGV komt binnen volg afstand en moet daarvoor stoppen
				if (distance <= stopDistance + followDistanceMargin) {
					obstakelState = State::STOP;
				}

				// AGV komt aan bij een bocht, hier moet hij eerst stoppen voordat hij kan gaan scannen
				else if (dummyBocht) {
					obstakelState = State::STOP;
				}
				break;
			case State::BACKWARD:
				// De AGV komt binnen volg afstand en moet daarvoor stoppen
				if (distance >= stopDistance - followDistanceMargin) {
					obstakelState = State::STOP;
				}

				// De AGV kan niet verder achteruit rijden
				else if (back.distance() < stopDistance && direction == Direction::BACKWARD) {
					obstakelState = State::STOP;
				}
				break;
			case State::LEFT:
				break;
			case State::RIGHT:
				break;
			case State::STOP:
				// De AGV staat bij een bocht en de volgpersoon is naar links of rechts gegaan, nu moet de AGV gaan scannen
				if (dummyBocht && distance > 10) {
					obstakelState = State::SCANNING;
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
					obstakelState = State::FORWARD;
				}

				// De AGV komt te dichtbij de volgpersoon en moet hierdoor achteruit
				if (distance < stopDistance - followDistanceMargin) {
					obstakelState = State::BACKWARD;
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
							obstakelState = State::LEFT;
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