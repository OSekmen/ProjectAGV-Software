#pragma once
#include "Pins.h"
#include "GlobalVariables.h"
#include "ToF_Detectie.h"
#include "ObstakelDetectie.h"

/*
Positie van paden ligt vast
*/
const float paths_horizontal_y[] = { 10.25, 36.75, 63.25, 89.75 };
const float paths_vertical_x[] = { 15.0, 185.0 };
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
Orientation orientation_naBocht;

NavigationState navState;
Vector stopVoorBocht(0, 0);
Vector marge(0.1, 0.1);

/*Vector queue[6] = {
	{paths_vertical_x[0], paths_horizontal_y[1]},
	{paths_vertical_x[1], paths_horizontal_y[1]},
	{paths_vertical_x[1], paths_horizontal_y[2]},
	{paths_vertical_x[0], paths_horizontal_y[2]},
	{paths_vertical_x[0], paths_horizontal_y[3]},
	{paths_vertical_x[1], paths_horizontal_y[3]}
};
uint8_t queue_i = 0;*/

Vector* queue;
uint8_t queueSize = 0;
uint8_t queueIndex = 0;

void addToQueue(Vector v) {
	queueSize++;
	Vector* temp = new Vector[queueSize];
	for (uint8_t i = 0; i < queueSize - 1; i++) {
		temp[i] = queue[i];
	}
	temp[queueSize - 1] = v;
	delete[] queue;
	queue = temp;
}

uint8_t xi;
/*uint8_t next_xi() {
	/*if (xi == 0) {
		return xi = 1;
	}
	else if (xi == 1) {
		return xi = 0;
	}*
	return !xi;
}*/

void createPath(Info info) {
	uint8_t treePathID = 0;
	while (treePathScanned[treePathID]) treePathID++;

	if (pos.x < 100) {
		xi = 0;
	}
	else {
		xi = 1;
	}

	Vector _target = { paths_vertical_x[xi], paths_horizontal_y[treePathID + 1] };

	// WARNING pos is niet exact
	Vector qPos = pos;

	// Bepalen welke richting eerst
	switch (orientation) {
	case Orientation::POSITIVE_X:
	case Orientation::NEGATIVE_X:
		qPos.x = _target.x;
		addToQueue(qPos);
	case Orientation::POSITIVE_Y:
	case Orientation::NEGATIVE_Y:
		qPos.y = _target.y;
		addToQueue(qPos);
		break;
	}

	uint8_t h3Counter = 0;
	while (true) {
		// x
		qPos.x = paths_vertical_x[!xi];
		addToQueue(qPos);

		// De AGV is boven en klaar met alle paden, eide van de queue
		if (qPos.y == paths_horizontal_y[3]) {
			h3Counter++;
			if (h3Counter >= 1) {
				break;
			}
		}

		// y
		qPos.y = paths_horizontal_y[++treePathID + 1];
		addToQueue(qPos);
	}
}

Vector getNextCoord() {
	return queue[queueIndex + 1];
}

Vector getCoord() {
	return queue[queueIndex];
}

/*bool onTarget(Vector marge) {
	Vector verschil = target - pos;
	if (abs(verschil.x) < marge.x && abs(verschil.y) < marge.y) return true;
	return false;
}*/

/*
bool bijBocht(Vector& b) {
	Vector t;
	if (direction == Direction::FORWARDS) {
		switch (orientation) {
		case Orientation::POSITIVE_X:
			t = { paths_vertical_x[1], pos.y };
			Vector v = t - pos;
			break;
		case Orientation::NEGATIVE_X:
			t = { paths_vertical_x[0], pos.y };
			break;
		case Orientation::POSITIVE_Y:
			t = {pos.x, pa}
			break;
		case Orientation::NEGATIVE_Y:
			break;
		}
	}

	return false;
}*/

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
	pos.x = paths_vertical_x[0];
	pos.y = 0; // TODO meten
	//target = Vector(paths_vertical_x[0], paths_horizontal_y[1]);
	pathNumber = 1;

	createPath(info);

	for (uint8_t p = 0; p < queueSize; p++) {
		Serial.print("(");
		Serial.print(queue[p].x);
		Serial.print(", ");
		Serial.print(queue[p].y);
		Serial.println(")");
	}

	//TODO remove
	queue[0].y = 40000000;

	target = queue[queueIndex];
	navState = PATH_CALCULATION;
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
			if (abs(verschil.x) < marge.x + stopVoorBocht.x) {
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

					if (direction == Direction::FORWARDS) {
						orientation_naBocht = Orientation::POSITIVE_Y;
					}
					else if (direction == Direction::BACKWARDS) {
						orientation_naBocht = Orientation::NEGATIVE_Y;
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

					if (direction == Direction::FORWARDS) {
						orientation_naBocht = Orientation::NEGATIVE_Y;
					}
					else if (direction == Direction::BACKWARDS) {
						orientation_naBocht = Orientation::POSITIVE_Y;
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
			if (abs(verschil.y) < marge.y + stopVoorBocht.y) {
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

					if (direction == Direction::FORWARDS) {
						orientation_naBocht = Orientation::POSITIVE_X;
					}
					else if (direction == Direction::BACKWARDS) {
						orientation_naBocht = Orientation::NEGATIVE_X;
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

					if (direction == Direction::FORWARDS) {
						orientation_naBocht = Orientation::NEGATIVE_X;
					}
					else if (direction == Direction::BACKWARDS) {
						orientation_naBocht = Orientation::POSITIVE_X;
					}
				}
				navState = BOCHT;
			}

			break;
#pragma endregion
#pragma region BOCHT
		case BOCHT:

			aandrijvingMode = Direction_To_AandrijfMode(bocht);

			if (bochtGemaakt == true) {
				//orientation = bocht; // TODO fixen

				switch (orientation) {
				case Orientation::POSITIVE_X:
				case Orientation::NEGATIVE_X:
					if (pos.y == paths_horizontal_y[0]) pathNumber = 0;
					else if (pos.y == paths_horizontal_y[1]) pathNumber = 1;
					else if (pos.y == paths_horizontal_y[2]) pathNumber = 2;
					else if (pos.y == paths_horizontal_y[3]) pathNumber = 3;
					break;
				case Orientation::POSITIVE_Y:
				case Orientation::NEGATIVE_Y:
					if (pos.x < 100) {
						pathNumber = 4;
					}
					else pathNumber = 5;
				}

				orientation = orientation_naBocht;
				navState = PATH_CALCULATION;
				queueIndex++;

				bochtGemaakt = false;
			}

			break;
#pragma endregion
#pragma region SIGNALEREN
		case SIGNALEREN:
			if (millis() >= signalEndMillis) {
				navState = prevNavState;
			}
			break;
#pragma endregion
		}
	}
#pragma endregion

#pragma region FOLLOW
	else if (mode == Mode::FOLLOW) {
		float distanceFront = US_front->distance();
		float distanceRear = US_rear->distance();

		// Vooruit
		if (distanceFront > stopDistance + followMarge) {
			aandrijvingMode = Vooruit;
			// if (bijbocht) scannen na afstands inval
		}
		// Achteruit zolang dat kan
		else if (distanceFront < stopDistance - followMarge &&
			distanceRear > stopDistance + followMarge) {
			aandrijvingMode = Achteruit;
		}
		else {
			aandrijvingMode = Stop;
		}
		
	}
#pragma endregion
}