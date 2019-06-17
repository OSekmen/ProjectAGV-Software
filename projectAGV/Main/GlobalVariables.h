#pragma once
#include "Vector.h"

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

class ObstakelDetectie;
//---------------------------------

/*
Alle variabelen hierin zijn gedeeld tussen de modules
en kunnen ook door verschillende modules worden gelezen en aangepast.
*/

static Mode mode = Mode::NORMAL;
static Direction obstakelState;
static Direction direction; // wordt alleen BACKWARDS, STOP of FORWARDS

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
