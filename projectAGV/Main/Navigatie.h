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

void setTarget(float x, float y) {
	target.x = x;
	target.y = y;
}


void setupNavigatie() {
	orientation = Orientation::POSITIVE_Y;
	pos.x = readToF(ToFSensors[Rand_L]) + 2;
	pos.y = US_achter->distance() + 14.3;
}

void loopNavigatie() {

}