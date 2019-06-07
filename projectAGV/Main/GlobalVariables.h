#pragma once
#include "States.h"

/*
Global enums
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
