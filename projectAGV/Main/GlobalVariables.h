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
	NORMAL,
	FOLLOW
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

static Orientation orientation;
static float posX;
static float posY;

/*
Globale sensoren
*/
static ObstakelDetectie* front;
static ObstakelDetectie* back;
