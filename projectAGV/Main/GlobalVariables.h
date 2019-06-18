#pragma once
#include "Vector.h"

/*
LOGLEVEL 0: Geen logs
LOGLEVEL 1: State en afstand
LOGLEVEL 2: Gemiddelde
*/
#define LOGLEVEL 1
#define PI 3.1415926535897932384626433832795

/*
Global classes
*/

static double map_double(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

enum class Mode {
	NORMAL,
	FOLLOW
};

enum class Direction {
	BACKWARDS = -1,
	STOP,
	FORWARDS,
	LEFT,
	RIGHT
};

enum class Orientation {
	POSITIVE_X,
	NEGATIVE_X,
	POSITIVE_Y,
	NEGATIVE_Y
};

enum AandrijfMode {
	Stop,
	Vooruit,
	Achteruit,
	Linksom,
	Rechtsom
};

class ObstakelDetectie;
//---------------------------------

/*
Alle variabelen hierin zijn gedeeld tussen de modules
en kunnen ook door verschillende modules worden gelezen en aangepast.
*/

static Mode mode = Mode::NORMAL;
static Direction obstakelState;
static Direction direction; // wordt alleen BACKWARDS, STOP of FORWARDS
static AandrijfMode aandrijvingMode;

static int16_t bijstuurWaarde;
static bool bijBocht = false;

static Orientation orientation;
static Vector pos;
static Vector target;

/*
Globale sensoren
*/
static ObstakelDetectie* US_front;
static ObstakelDetectie* US_rear;

static double AGV_Angle_RAD = -PI / 2;

void printDirection(Direction dir) {
	switch (dir) {
	case Direction::BACKWARDS:
		Serial.print("BACKWARDS");
		break;
	case Direction::STOP:
		Serial.print("STOP");
		break;
	case Direction::FORWARDS:
		Serial.print("FORWARDS");
		break;
	case Direction::LEFT:
		Serial.print("LEFT");
		break;
	case Direction::RIGHT:
		Serial.print("RIGHT");
		break;
	}
}