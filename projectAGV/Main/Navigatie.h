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

Orientation bocht;

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
#pragma region oude shit
//		uint8_t i = 0;
//		while (!treePathScanned[i]) i++;
//		float x, y;
//		
//		// Bepalen of de AGV links of rechts over de y-as gaat rijden
//		if (pos.x <= 100.0) {
//			x = paths_vertical_x[0];
//		}
//		else {
//			x = paths_vertical_x[1];
//		}
//		y = paths_horizontal_y[i + 1];
//		target = Vector(x, y);
//		verschil = target - pos;
//
//		// Bepalen in welke richting de AGV eerst moet rijden
//		switch (orientation) {
//		/*
//		We staan in een horizontaal pad, dus eerst horizontaal rijden
//		*/
//		case Orientation::POSITIVE_X:
//		case Orientation::NEGATIVE_X:
//			int8_t d = 0;
//			if (verschil.x != 0) {
//				d = -verschil.x / abs(verschil.x);
//				if (orientation == Orientation::POSITIVE_X) d *= -1;
//			}
//			direction = (Direction)d;
//			navState = DRIVE_X;
//
//			// TODO bepalen welke kant bomen moet scannen
//
//			break;
//		/*
//		We staan in een verticaal pad, dus eerst verticaal rijden
//		*/
//		case Orientation::POSITIVE_Y:
//		case Orientation::NEGATIVE_Y:
//			d = 0;
//			if (verschil.y != 0) {
//				d = -verschil.y / abs(verschil.y);
//				if (orientation == Orientation::POSITIVE_Y) d *= -1;
//			}
//			direction = (Direction)d;
//			navState = DRIVE_Y;
//			break;
//		}
//#pragma endregion
		target = getCoord();
		verschil = target - pos;

		// x coordinaat is goed
		if (abs(verschil.x) < marge.x) {
			navState = DRIVE_Y;
			int8_t d = 0;
			if (verschil.y != 0) {
				d = -verschil.y / abs(verschil.y);
				if (orientation == Orientation::POSITIVE_Y) d *= -1;
			}
			direction = (Direction)d;
		}
		// y coordinaat is goed
		if (abs(verschil.y) < marge.y) {
			navState = DRIVE_X;
			int8_t d = 0;
			if (verschil.x != 0) {
				d = -verschil.x / abs(verschil.x);
				if (orientation == Orientation::POSITIVE_X) d *= -1;
			}
			direction = (Direction)d;
		}
		break;
#pragma endregion
#pragma region DRIVE_X
	case DRIVE_X:
		// TODO iets naar de aandrijving waarbij de direction(globaal!) wordt mee gegeven

		// TODO scannen voor bomen

		// TODO scannen voor obstakels

		// TODO y-coordinaat correctie

		/// TODO bepalen of de AGV bij een bocht is (als hij op het target coordinaat is)

		/// TODO pad afvinken, stoppen met scannen voor bomen

		// De AGV komt binnen x marge van de target, dit is ook een bocht
		if (abs(verschil.x) < marge.x) {
			// bocht richting bepalen
			Vector v = getNextCoord() - pos;

			if (v.y > 0) {
				bocht = Orientation::POSITIVE_Y;
			}
			else if (v.y < 0) {
				bocht = Orientation::NEGATIVE_Y;
			}
			
			/*uint8_t i = 0;
			while (!treePathScanned[i]) i++;
			treePathScanned[i] = true;*/
			navState = BOCHT;
		}
		break;
#pragma endregion
#pragma region DRIVE_Y
	case DRIVE_Y:
		// TODO iets naar de aandrijving waarbij de direction(globaal!) wordt mee gegeven

		// TODO scannen voor obstakels

		// TODO x-coordinaat correctie

		// TODO bepalen of de AGV bij een bocht is
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

		if (funcName == true) {
			orientation = bocht;
			navState = PATH_CALCULATION;
			queue_i++;
		}

		break;
#pragma endregion
	}
}