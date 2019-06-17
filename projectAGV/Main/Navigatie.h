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
	BOCHT,
	SIGNALEREN
};

struct Info {
	Vector pos;
	Direction direction;
	Orientation orientation;
};

Info info = { Vector(paths_vertical_x[0], 0), Direction::STOP, Orientation::POSITIVE_Y };

NavigationState prevNavState;
uint32_t signalEndMillis;

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

void createPath(Info info) {
	uint8_t treePathID = 0;
	while (!treePathScanned[treePathID]) treePathID++;
	Vector _target = info.pos;
}

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

AandrijfMode Direction_To_AandrijfMode(Direction dir) {
	switch (dir) {
	case Direction::BACKWARDS:
		return Achteruit;
	case Direction::STOP:
		return Stop;
	case Direction::FORWARDS:
		return Vooruit;
	case Direction::LEFT:
		return Linksom;
	case Direction::RIGHT:
		return Rechtsom;
	}
	return Stop;
}

void setupNavigatie() {
	orientation = Orientation::POSITIVE_Y;
	//pos.x = readToF_cm(ToFSensors[Rand_L]) + 2;
	//pos.y = US_rear->distance() + 14.3;
	pos.x = 15;
	pos.y = 0;
	target = Vector(paths_vertical_x[0], paths_horizontal_y[1]);
	navState = PATH_CALCULATION;

	// TODO eventueel de queue berekenen
}

void loopNavigatie() {
	Vector verschil = target - pos;
#pragma region NORMAL
	if (mode == Mode::NORMAL) {
		switch (navState) {
#pragma region PATH_CALCULATION
		case PATH_CALCULATION:

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
			/// TODO iets naar de aandrijving waarbij de direction(globaal!) wordt mee gegeven
			aandrijvingMode = Direction_To_AandrijfMode(direction);
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
			/// TODO iets naar de aandrijving waarbij de direction(globaal!) wordt mee gegeven
			aandrijvingMode = Direction_To_AandrijfMode(direction);

			// TODO scannen voor obstakels

			/// TODO x-coordinaat correctie
			pos.x = target.x;

			/// TODO bepalen of de AGV bij een bocht is
			if (abs(verschil.y) < marge.y) {
				// bocht richting bepalen
				Vector v = getNextCoord() - pos;

				// Target is rechts
				if (v.x > 0) {
					if (orientation == Orientation::POSITIVE_Y) {
						bocht = Direction::RIGHT;
					}
					else if (orientation == Orientation::NEGATIVE_Y) {
						bocht = Direction::LEFT;
					}
				}
				// Target is links
				else if (v.x < 0) {
					if (orientation == Orientation::POSITIVE_Y) {
						bocht = Direction::LEFT;
					}
					else if (orientation == Orientation::NEGATIVE_Y) {
						bocht = Direction::RIGHT;
					}
				}
				navState = BOCHT;
			}

			break;
#pragma endregion
#pragma region BOCHT
		case BOCHT:

			aandrijvingMode = Direction_To_AandrijfMode(bocht);

			/*if (bochtGemaakt == true) {
				orientation = bocht;
				navState = PATH_CALCULATION;
				queue_i++;
			}*/

			break;
#pragma endregion
#pragma region SIGNALEREN
		case SIGNALEREN:
			if (millis() > signalEndMillis) {
				navState = prevNavState;
			}
			break;
#pragma endregion
		}
	}
#pragma endregion

#pragma region FOLLOW
	// Magic
	else if (mode == Mode::FOLLOW) {
		// De AGV komt binnen volg afstand en moet daarvoor stoppen
		
	}
#pragma endregion
}