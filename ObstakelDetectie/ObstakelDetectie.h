#pragma once
#include <Servo.h>
#include <Ultrasonic.h>
#include <HardwareSerial.h>
#include "Pins.h"
#include "GlobalVariables.h"

/*
Dummy variabelen moeten van andere software delen komen
*/
//int8_t dummyDirection = 1; // -1 rearuit, 0 stilstand, 1 frontuit
//int8_t dummyMode = 1; // 0 normaal, 1 volgmode
//bool bijBocht = false;
// ---------------------------------------------------------------------
const float stopDistance = 5;
const float followMarge = 1;
const uint32_t updateTime_ms = 1;
uint64_t US_millis;

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

	@return Gemiddelde meting in cm.
	*/
	float distance() {
		float v = 0;
		for (uint8_t i = 0; i < NUM_MEASUREMENTS; i++) {
			v += measurements[i];
		}
		v /= NUM_MEASUREMENTS;
		return v;
	}
};

/*
bool scanO(float dist) {
	//TODO 2 soorten bochten (T- bocht van onder en T- bocht van boven)
	if (millis() >= nextScanMillis) {
		scanPoints[scanIndex] += dist / NUM_MEASUREMENTS;
		scanMeasureIndex++;

		if (scanMeasureIndex >= NUM_MEASUREMENTS) {
			scanMeasureIndex = 0;
			scanIndex++;

			if (scanIndex >= NUM_SCANPOINTS) {
				// DO THE MAGIC
				obstakelState = Direction::LEFT;
				US_front->servo.write(90);
				break;
			}

			float targetAngle = scanIndex * SCAN_FIELD_DEGREE / (NUM_SCANPOINTS - 1) - SCAN_FIELD_DEGREE / 2.0 + 90;
			nextScanMillis = millis() + abs(US_front->servo.read() - targetAngle) * MILLIS_PER_DEGREE + SCAN_MILLIS_MARGIN;
			US_front->servo.write(targetAngle);
		}
	}
	return false;
}

bool scan(float d) {
	
	return false;
}*/

void setupObstakelDetectie() {
	US_front = new ObstakelDetectie(FRONT_TRIGGER, FRONT_ECHO, updateTime_ms, SERVO);
	US_rear = new ObstakelDetectie(REAR_TRIGGER, REAR_ECHO, updateTime_ms);

	US_millis = millis();
	US_front->servo.write(90);
}

uint32_t nextScanMillis;
uint16_t scanIndex;
uint16_t scanMeasureIndex;

void loopObstakelDetectie() {
	if (millis() - US_millis > updateTime_ms) {
		US_front->read();
		US_rear->read();

		US_millis = millis();
		if (mode == Mode::NORMAL) { // normale mode
			US_front->servo.write(90);
			switch (direction) {
			case Direction::BACKWARDS:
				if (US_rear->distance() <= stopDistance) {
					obstakelState = Direction::STOP;
				}
				else {
					obstakelState = Direction::BACKWARDS;
				}
				break;
			case Direction::STOP:
				obstakelState = Direction::STOP;
				break;
			case Direction::FORWARDS:
				if (US_front->distance() <= stopDistance) {
					obstakelState = Direction::STOP;
				}
				else {
					obstakelState = Direction::FORWARDS;
				}
				break;
			}
		}

		else if (false) { // volg mode
			float distance = US_front->distance();
			switch (obstakelState) {
			case Direction::FORWARDS:
				// De AGV komt binnen volg afstand en moet daarvoor stoppen
				if (distance <= stopDistance + followMarge) {
					obstakelState = Direction::STOP;
				}

				else if (bijBocht) {
					obstakelState = Direction::STOP;
				}
				break;
			case Direction::BACKWARDS:
				if (distance >= stopDistance - followMarge) {
					obstakelState = Direction::STOP;
				}

				// De AGV kan niet verder vooruit rijden
				else if (US_rear->distance() < stopDistance && direction == Direction::BACKWARDS) {
					obstakelState = Direction::STOP;
				}
				break;
			case Direction::LEFT:
				break;
			case Direction::RIGHT:
				break;
			case Direction::STOP:
				// De AGV staat bij een bocht en de volgpersoon is naar links of rechts gegaan, nu moet de AGV gaan scannen
				if (bijBocht && distance > 10) {
					//obstakelState = State::SCANNING;
					for (uint8_t i = 0; i < NUM_SCANPOINTS; i++) {
						scanPoints[i] = 0;
					}
					scanMeasureIndex = 0;
					scanIndex = 0;
					float targetAngle = scanIndex * SCAN_FIELD_DEGREE / (NUM_SCANPOINTS - 1) - SCAN_FIELD_DEGREE / 2.0 + 90;
					nextScanMillis = millis() + abs(US_front->servo.read() - targetAngle) * MILLIS_PER_DEGREE + SCAN_MILLIS_MARGIN;
					US_front->servo.write(targetAngle);
				}

				// De AGV is te ver weg verwijderd van de volgpersoon en moet hierdoor vooruit
				else if (distance > stopDistance + followMarge && !bijBocht) {
					obstakelState = Direction::FORWARDS;
				}

				// De AGV komt te dichtbij de volgpersoon en moet hierdoor achteruit
				if (distance < stopDistance - followMarge) {
					obstakelState = Direction::BACKWARDS;
				}
				break;
			/*case State::SCANNING:
				//TODO 2 soorten bochten (T- bocht van onder en T- bocht van boven)
				if (millis() >= nextScanMillis) {
					scanPoints[scanIndex] += distance / NUM_MEASUREMENTS;
					scanMeasureIndex++;

					if (scanMeasureIndex >= NUM_MEASUREMENTS) {
						scanMeasureIndex = 0;
						scanIndex++;

						if (scanIndex >= NUM_SCANPOINTS) {
							// DO THE MAGIC
							obstakelState = Direction::LEFT;
							US_front->servo.write(90);
							break;
						}
						
						float targetAngle = scanIndex * SCAN_FIELD_DEGREE / (NUM_SCANPOINTS - 1) - SCAN_FIELD_DEGREE / 2.0 + 90;
						nextScanMillis = millis() + abs(US_front->servo.read() - targetAngle) * MILLIS_PER_DEGREE + SCAN_MILLIS_MARGIN;
						US_front->servo.write(targetAngle);
					}
				}
				break;*/
			}
		}
	}
}