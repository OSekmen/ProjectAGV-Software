#pragma once
#include "States.h"
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

enum class Mode {
	NORMAL,
	FOLLOW
};

enum class Direction {
	BACKWARDS = -1,
	STOP = 0,
	FORWARDS = 1
};

enum class Orientation {
	POSITIVE_X,
	NEGATIVE_X,
	POSITIVE_Y,
	NEGATIVE_Y
};

class ObstakelDetectie;
//---------------------------------

/*
Alle variabelen hierin zijn gedeeld tussen de modules
en kunnen ook door verschillende modules worden gelezen en aangepast.
*/

static Mode mode = Mode::NORMAL;
static State obstakelState;
static Direction direction;

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

static double AGV_Angle_RAD = PI / 2;
