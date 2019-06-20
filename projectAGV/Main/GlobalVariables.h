#pragma once
#include "Vector.h"


/*
	TODO-list
	- bocht maken
	- positie bepaling maar 1 coordinaat punt
*/


/*
LOGLEVEL 0: Geen logs
LOGLEVEL 1: Wel logs
*/
#define LOGLEVEL 0
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

enum class StuurRichting {
	STADY,
	LEFT,
	RIGHT
};

enum NavigationState {
	PATH_CALCULATION,
	DRIVE_X,
	DRIVE_Y,
	BOCHT,
	SIGNALEREN
};

enum class Scherm {
	DEBUG,
	HOME,
	MANUAL
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

static NavigationState navState;
static Scherm schermMode;

//static int16_t bijstuurWaarde;
static bool bijBocht = false;
static bool bochtGemaakt;

static bool boomGedetecteerd = false;

static Orientation orientation;
static Vector pos;
static Vector target;
static uint8_t pathNumber;

volatile boolean NoodstopActive;

/*
Globale sensoren
*/
static ObstakelDetectie* US_front;
static ObstakelDetectie* US_rear;

static double AGV_Angle_RAD = -PI / 2;

String printDirection(Direction dir) {
	switch (dir) {
	case Direction::BACKWARDS:
		return "Backwards";
	case Direction::STOP:
		return "Stop";
	case Direction::FORWARDS:
		return "Forwards";
	case Direction::LEFT:
		return "Left";
	case Direction::RIGHT:
		return "Right";
	}
}

void printAandrijving(AandrijfMode a) {
	switch (a) {
	case Stop:
		Serial.print("Stop");
		break;
	case Vooruit:
		Serial.print("Vooruit");
		break;
	case Achteruit:
		Serial.print("Achteruit");
		break;
	case Linksom:
		Serial.print("Linksom");
		break;
	case Rechtsom:
		Serial.print("Rechtsom");
		break;
	}
}

void printNavstate(NavigationState n) {
	switch (n) {
	case PATH_CALCULATION:
		Serial.print("PC");
		break;
	case DRIVE_X:
		Serial.print("DRIVE_X");
		break;
	case DRIVE_Y:
		Serial.print("DRIVE_Y");
		break;
	case BOCHT:
		Serial.print("BOCHT");
		break;
	case SIGNALEREN:
		Serial.print("SIGNALEREN");
		break;
	}
}