#pragma once
#include "Pins.h"
#include "GlobalVariables.h"
#include "ToF_Detectie.h"
#include "ObstakelDetectie.h"

/*
Positie van paden ligt vast
*/
float paths_horizontal_y[] = { 10.25, 36.75, 63.25, 89.75 };
float paths_vertical_x[] = { 15.0, 185.0 };
bool treePathScanned[] = { false, false, false };

enum NavigationState {
	PATH_CALCULATION,
	DRIVE_X,
	DRIVE_Y,
	BOCHT
};

Direction bocht;

NavigationState navState;
Vector marge(1, 1);

Vector queue[6] = {
	{paths_vertical_x[0], paths_horizontal_y[1]},
	{paths_vertical_x[1], paths_horizontal_y[1]},
	{paths_vertical_x[1], paths_horizontal_y[2]},
	{paths_vertical_x[0], paths_horizontal_y[2]},
	{paths_vertical_x[0], paths_horizontal_y[3]},
	{paths_vertical_x[1], paths_horizontal_y[3]}
};
uint8_t queue_i = 0;

Vector getNextCoord() {
	return queue[queue_i + 1];
}

Vector getCoord() {
	return queue[queue_i];
}

bool onTarget(Vector marge) {
	Vector verschil = target - pos;
	if (abs(verschil.x) < marge.x && abs(verschil.y) < marge.y) return true;
	return false;
}

void setupNavigatie() {
	orientation = Orientation::POSITIVE_Y;
	pos.x = readToF_cm(ToFSensors[Rand_L]) + 2;
	pos.y = US_rear->distance() + 14.3;
	target = Vector(paths_vertical_x[0], paths_horizontal_y[1]);
	navState = PATH_CALCULATION;

	// TODO eventueel de queue berekenen
}

void loopNavigatie() {
	Vector verschil = target - pos;

	switch (navState) {
#pragma region PATH_CALCULATION
	case PATH_CALCULATION:
#pragma region DRIVE_X
	case DRIVE_X:
		// TODO iets naar de aandrijving waarbij de direction(globaal!) wordt mee gegeven

		// TODO scannen voor bomen

		// TODO scannen voor obstakels

		/// TODO y-coordinaat correctie
		pos.y = target.y;

		// TODO pad afvinken, stoppen met scannen voor bomen

		// De AGV komt binnen x marge van de target, dit is ook een bocht
		if (abs(verschil.x) < marge.x) {
			// bocht richting bepalen
			Vector v = getNextCoord() - pos;

			// Target is hoger
			if (v.y > 0) {
				if (orientation == Orientation::POSITIVE_X) {
					bocht = Direction::LEFT;
				}
				else if (orientation == Orientation::NEGATIVE_X) {
					bocht = Direction::RIGHT;
				}
			}
			// Target is lager
			else if (v.y < 0) {
				if (orientation == Orientation::POSITIVE_X) {
					bocht = Direction::RIGHT;
				}
				else if (orientation == Orientation::NEGATIVE_X) {
					bocht = Direction::LEFT;
				}
			}
			
			navState = BOCHT;
		}
		break;
#pragma endregion
#pragma region DRIVE_Y
	case DRIVE_Y:
		// TODO iets naar de aandrijving waarbij de direction(globaal!) wordt mee gegeven

		// TODO scannen voor obstakels

		/// TODO x-coordinaat correctie
		pos.x = target.x;

		/// TODO bepalen of de AGV bij een bocht is
		if (abs(verschil.y) < marge.y) {
			// bocht richting bepalen
			Vector v = getNextCoord() - pos;

			if (v.x > 0) {
				bocht = Orientation::POSITIVE_X;
			}
			else if (v.x < 0) {
				bocht = Orientation::NEGATIVE_X;
			}
			navState = BOCHT;
		}

		break;
#pragma endregion
#pragma region BOCHT
	case BOCHT:
		/* TODO 
		iets in aandrijving in de vorm 
		bool funcName(richting)
		waarbij return true betekent dat de bocht is gemaakt en return false dat de AGV nog bezig is met de bocht
		*/

		/*if (funcName == true) {
			orientation = bocht;
			navState = PATH_CALCULATION;
			queue_i++;
		}*/

		break;
#pragma endregion
	}
}