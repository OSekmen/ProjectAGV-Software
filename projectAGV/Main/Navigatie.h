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
		if (pos.x <= 100.0) {
			x = paths_vertical_x[0];
		}
		else {
			x = paths_vertical_x[1];
		}
		y = paths_horizontal_y[i + 1];
		target = Vector(x, y);

		switch (orientation) {
		/*
		We staan in een horizontaal pad, dus eerst horizontaal rijden
		*/
		case Orientation::POSITIVE_X:
		case Orientation::NEGATIVE_X:
			if (target.x < pos.x) {
				direction = Direction::BACKWARDS;
			}
			else {
				direction = Direction::FORWARDS;
			}
			navState = DRIVE_X;
			break;


		/*
		We staan in een verticaal pad, dus eerst verticaal rijden
		*/
		case Orientation::POSITIVE_Y:
		case Orientation::NEGATIVE_Y:
			if (target.y < pos.y) {
				direction = Direction::BACKWARDS;
			}
			else {
				direction = Direction::FORWARDS;
			}
			navState = DRIVE_Y;
			break;
		}

		break;
#pragma endregion
#pragma region DRIVE_X
	case DRIVE_X:
		// TODO dit is ook orientatie afhankelijk
		if (direction == Direction::FORWARDS) {
			if (pos.x < target.x) {
				// Nog niet op target.x dus doorgaan
				break;
			}
		}
		if (direction == Direction::BACKWARDS) {
			if (pos.x > target.x) {
				// Nog niet op target.x dus doorgaan
				break;
			}
		}

		//TODO margin
		if (pos == target) {
			// Bochten maken
		}
		else {
			navState = DRIVE_Y;
		}
		break;
#pragma endregion
#pragma region DRIVE_Y
	case DRIVE_Y:

		if (direction == Direction::FORWARDS) {
			if (pos.y < target.y) {
				// Nog niet op target.y dus doorgaan
				break;
			}
		}
		if (direction == Direction::BACKWARDS) {
			if (pos.y > target.y) {
				// Nog niet op target.y dus doorgaan
				break;
			}
		}

		//TODO margin
		if (pos == target) {
			// Bochten maken
		}
		else {
			navState = DRIVE_X;
		}
		break;
#pragma endregion
	}
}