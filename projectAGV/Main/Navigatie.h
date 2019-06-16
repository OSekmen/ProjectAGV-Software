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
	DECICION_MAKING,
	DRIVE_X,
	DRIVE_Y,
	BOCHT
};

NavigationState navState;
Vector marge(1, 1);

void setupNavigatie() {
	orientation = Orientation::POSITIVE_Y;
	pos.x = readToF_cm(ToFSensors[Rand_L]) + 2;
	pos.y = US_rear->distance() + 14.3;
	target = Vector(paths_vertical_x[0], paths_horizontal_y[1]);
	navState = DECICION_MAKING;
}

void loopNavigatie() {
	switch (navState) {
#pragma region DECICION_MAKING
	case DECICION_MAKING:
		uint8_t i = 0;
		while (!treePathScanned[i]) i++;
		float x, y;
		
		// Bepalen of de AGV links of rechts over de y-as gaat rijden
		if (pos.x <= 100.0) {
			x = paths_vertical_x[0];
		}
		else {
			x = paths_vertical_x[1];
		}
		y = paths_horizontal_y[i + 1];
		target = Vector(x, y);
		Vector verschil = target - pos;

		// Bepalen in welke richting de AGV eerst moet rijden
		switch (orientation) {
		/*
		We staan in een horizontaal pad, dus eerst horizontaal rijden
		*/
		case Orientation::POSITIVE_X:
		case Orientation::NEGATIVE_X:
			int8_t d = 0;
			if (verschil.x != 0) {
				d = -verschil.x / abs(verschil.x);
				if (orientation == Orientation::POSITIVE_X) d *= -1;
			}
			direction = (Direction)d;
			navState = DRIVE_X;
			break;
		/*
		We staan in een verticaal pad, dus eerst verticaal rijden
		*/
		case Orientation::POSITIVE_Y:
		case Orientation::NEGATIVE_Y:
			// TODO Zelfde als bij x maar nu met y
			break;
		}

		break;
#pragma endregion
#pragma region DRIVE_X
	case DRIVE_X:
		// TODO iets naar de aandrijving waarbij de direction(globaal!) wordt mee gegeven

		// TODO scannen voor bomen

		// TODO y-coordinaat correctie

		// TODO appart bepalen of de AGV bij een bocht is

		// TODO pad afvinken

		// De AGV komt binnen x marge van de target, de AGV is nu bij een bocht
		if (abs(verschil.x) < marge.x) {


			// TODO bepaal bocht richting
			//navState = BOCHT;
		}
		break;
#pragma endregion
#pragma region DRIVE_Y
	case DRIVE_Y:

		break;
#pragma endregion
#pragma region BOCHT
	case BOCHT:
		/* TODO 
		iets in aandrijving in de vorm 
		bool funcName(richting)
		waarbij return true betekent dat de bocht is gemaakt en return false dat de AGV nog bezig is met de bocht
		*/
		break;
#pragma endregion
	}
}