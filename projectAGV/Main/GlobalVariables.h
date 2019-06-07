#pragma once
#include "States.h"

/*
LOGLEVEL 0: Geen logs
LOGLEVEL 1: State en afstand
LOGLEVEL 2: Gemiddelde
*/
#define LOGLEVEL 1


/*
Global classes
*/

enum class Mode {
	NORMAAL,
	VOLGEN
};

enum class Direction {
	FORWARD,
	BACKWARD,
	STOP
};

enum class Orientation {
	POSITIVE_X,
	POSITIVE_Y,
	NEGATIVE_X,
	NEGATIVE_Y
};

#define NUM_MEASUREMENTS 10

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

//---------------------------------

/*
Alle variabelen hierin zijn gedeeld tussen de modules
en kunnen ook door verschillende modules worden gelezen en aangepast.
*/

static Mode mode = Mode::NORMAAL;
static State obstakelState;
static Direction direction;

static int16_t bijstuurWaarde;

static Orientation orientation;
static float posX;
static float posY;

/*
Globale sensoren
*/
static ObstakelDetectie front;
static ObstakelDetectie back;
