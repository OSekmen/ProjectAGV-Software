#pragma once
#include "States.h"
/*
Alle variabelen hierin zijn gedeeld tussen de modules
en kunnen ook door verschillende modules worden gelezen en aangepast.
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

//---------------------------------

static Mode mode = Mode::NORMAAL;
static State obstakelState;
static Direction direction;

static int16_t bijstuurWaarde;
