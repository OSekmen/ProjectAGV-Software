#pragma once
#include "Pins.h"
#include "GlobalVariables.h"
#include "ToF_Detectie.h"
#include "ObstakelDetectie.h"

/*#define DIM_X 31.2 // Overal hetzelfde
#define DIM_Y_0 22.6
#define DIM_Y_2 20.5

#define X_COL_0 15.6
#define X_COL_1 184.4
#define Y_ROW_0 11.1
#define Y_ROW_1 38.6
#define Y_ROW_2 61.4
#define Y_ROW_3 88.4

enum BochtType {
	L,
	T
};

struct Bocht {
	float x;
	float y;
	float sizeY;
	BochtType type;
};

#define NUM_BOCHTEN 8
Bocht bochten[NUM_BOCHTEN] = {
	{X_COL_0, Y_ROW_0, DIM_Y_0, L},
	{X_COL_1, Y_ROW_0, DIM_Y_0, L},
	{X_COL_0, Y_ROW_1, DIM_Y_2, T},
	{X_COL_1, Y_ROW_1, DIM_Y_2, T},
	{X_COL_0, Y_ROW_2, DIM_Y_2, T},
	{X_COL_1, Y_ROW_2, DIM_Y_2, T},
	{X_COL_0, Y_ROW_3, DIM_Y_0, L},
	{X_COL_1, Y_ROW_3, DIM_Y_0, L}
};*/

/*
Positie van paden ligt vast
*/
float paths_horizontal_y[] = { 10.25, 36.75, 63.25, 89.75 };
float paths_vertical_x[] = { 15.0, 185.0 };
bool treePathScanned[] = { false, false, false };

/*
float margin = 10;
template<class T, class R>
bool equals(T a, R b) {
	if (a - abs(b) <= margin) return true;
	return false;
}*/

enum NavigationState {
	DECICION_MAKING,
	DRIVE_X,
	DRIVE_Y

};

NavigationState navState;


void setupNavigatie() {
	orientation = Orientation::POSITIVE_Y;
	pos.x = readToF(ToFSensors[Rand_L]) + 2;
	pos.y = US_achter->distance() + 14.3;
	target = Vector(paths_vertical_x[0], paths_horizontal_y[1]);
	navState = DECICION_MAKING;
}

void loopNavigatie() {
	switch (navState) {
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
				direction = Direction::BACKWARD;
			}
			else {
				direction = Direction::FORWARD;
			}
			navState = DRIVE_X;
			break;


		/*
		We staan in een verticaal pad, dus eerst verticaal rijden
		*/
		case Orientation::POSITIVE_Y:
		case Orientation::NEGATIVE_Y:
			if (target.y < pos.y) {
				direction = Direction::BACKWARD;
			}
			else {
				direction = Direction::FORWARD;
			}
			navState = DRIVE_Y;
			break;
		}

		break;
	case DRIVE_X:
		if (direction == Direction::FORWARD) {
			if (pos.x < target.x) {
				// Nog niet  op target.x dus doorgaan
				break;
			}
		}
		if (direction == Direction::BACKWARD) {
			if (pos.x > target.x) {
				// Nog niet  op target.x dus doorgaan
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

	case DRIVE_Y:

		if (direction == Direction::FORWARD) {
			if (pos.y < target.y) {
				// Nog niet  op target.y dus doorgaan
				break;
			}
		}
		if (direction == Direction::BACKWARD) {
			if (pos.y > target.y) {
				// Nog niet  op target.y dus doorgaan
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
	}
}