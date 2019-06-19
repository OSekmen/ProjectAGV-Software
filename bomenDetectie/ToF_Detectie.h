#pragma once
#include "Pins.h"
#include "GlobalVariables.h"

#define NUM_TOF_MEASUREMENTS 1

VL53L0X sensorRand_R;
VL53L0X sensorRand_L;
VL53L0X sensorBoom_R;
VL53L0X sensorBoom_L;

byte TreeScanMode; // welke kant moet er gescand worden
boolean StopCommand; // commando om de AGV te stoppen 

VL53L0X* ToFSensors[] = { &sensorRand_L, &sensorRand_R, &sensorBoom_L, &sensorBoom_R };
const int sensorAddresses[] = { 0x30, 0x31, 0x32, 0x33 };
const int sensorPinResetArray[] = { RESET_RAND_R, RESET_RAND_L, RESET_BOOM_R, RESET_BOOM_L };

enum sensorName {
	Rand_R = 0,
	Rand_L = 1,
	Boom_R = 2,
	Boom_L = 3
} SensorName;

int readToF_mm(VL53L0X* sensor) {
	return sensor->readRangeSingleMillimeters();
}

float readToF_cm(VL53L0X* sensor) {
	return (float)sensor->readRangeSingleMillimeters() / 10.0;
}

class ToF {
private:
	float measurements[NUM_TOF_MEASUREMENTS];
	uint8_t index;
	sensorName sensor;

public:
	ToF() {}
	ToF(sensorName _sensor) {
		sensor = _sensor;
		index = 0;

		uint16_t m = readToF_mm(ToFSensors[sensor]);
		for (uint8_t i = 0; i < NUM_TOF_MEASUREMENTS; i++) {
			measurements[i] = m;
		}
	};

	float read() {
		if (index >= NUM_TOF_MEASUREMENTS) index = 0;
		measurements[index] = readToF_mm(ToFSensors[sensor]);
		return measurements[index++];
	}

	float distance() {
		float v = 0;
		for (uint8_t i = 0; i < NUM_TOF_MEASUREMENTS; i++) {
			v += measurements[i];
		}
		v /= NUM_TOF_MEASUREMENTS;
		return v;
	}
};

ToF randR;
ToF randL;


void resetToFsensor(uint8_t sensorNumber) {
	const  uint16_t timeOut = 10;
	static uint16_t prev_time[sizeof ToFSensors];

	static uint8_t resetProcress[sizeof ToFSensors];

	uint16_t curr_time = millis();
	switch (resetProcress[sensorNumber]) {
	case 0:
		digitalWrite(LATCHPIN, LOW);
		IC_REG = IC_REG & ~sensorPinResetArray[sensorNumber];
		shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, IC_REG);
		digitalWrite(LATCHPIN, HIGH);

		prev_time[sensorNumber] = curr_time;
		resetProcress[sensorNumber] = 1;
		break;

	case 1:
		if (curr_time >= (prev_time[sensorNumber] + timeOut)) {
			digitalWrite(LATCHPIN, LOW);
			IC_REG = IC_REG | sensorPinResetArray[sensorNumber];
			shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, IC_REG);
			digitalWrite(LATCHPIN, HIGH);


			//digitalWrite(sensorPinResetArray[sensorNumber], HIGH);

			prev_time[sensorNumber] = curr_time;
			resetProcress[sensorNumber] = 2;
		}
		break;

	case 2:
		if (curr_time >= (prev_time[sensorNumber] + timeOut)) {
			/*digitalWrite(sensorPinResetArray[sensorNumber], HIGH);
			prev_time[sensorNumber] = curr_time;
*/
			ToFSensors[sensorNumber]->init();
			ToFSensors[sensorNumber]->setAddress(sensorAddresses[sensorNumber]);
			ToFSensors[sensorNumber]->setTimeout(500);

			resetProcress[sensorNumber] = 0;
		}
		break;
	}
}

void setupResetToFsensor(uint8_t sensorNumber) {
	//digitalWrite(sensorPinResetArray[sensorNumber], LOW); //output
	digitalWrite(LATCHPIN, LOW);
	IC_REG = IC_REG & ~sensorPinResetArray[sensorNumber];
	shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, IC_REG);
	digitalWrite(LATCHPIN, HIGH);
	delay(10);
	//digitalWrite(sensorPinResetArray[sensorNumber], HIGH);
	digitalWrite(LATCHPIN, LOW);
	IC_REG = IC_REG | sensorPinResetArray[sensorNumber];
	shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, IC_REG);
	digitalWrite(LATCHPIN, HIGH);
	delay(10);

	ToFSensors[sensorNumber]->init();
	ToFSensors[sensorNumber]->setAddress(sensorAddresses[sensorNumber]);
	ToFSensors[sensorNumber]->setTimeout(500);
}



bool ScanTree(uint8_t sensorNumber) {
	static bool reset[sizeof ToFSensors];

	long distance = readToF_mm(ToFSensors[sensorNumber]);

	if (distance >= 80 && distance <= 125) {	//200/2 + 60/2 - (170/2 + 20) = afstand van sensor tot boom 
		if (!reset[sensorNumber]) {
			reset[sensorNumber] = true;
			return true;
		}
		else
			return false;
	}
	else {
		reset[sensorNumber] = false;
		return false;
	}
}

void TreeProssing(uint8_t mode) {
	switch (mode) {
	case 0:	//geen scan
		break;

	case 1:	//links van AGV
		if (ScanTree(Boom_L)) {
			// doe iets!
			//*command = true;
			Serial.println("boom links");
			boomGedetecteerd = true;
		}
		else
			//*command = false;
			boomGedetecteerd = false;
		break;

	case 2:	//rechts van AGV
		if (ScanTree(Boom_R)) {
			// doe iets!
			//*command = true;
			boomGedetecteerd = true;
		}
		else
			//*command = false;
			boomGedetecteerd = false;
		break;
	}
}

#pragma region Rand Detectie
void scanOneSide(double& bijstuurWaarde, double& distance, StuurRichting& turnSide, boolean side, uint16_t space)
{
	float distanceScan;
	if (!side) // links scannen
	{
		//distanceScan = (cos(AGV_Angle_RAD) * readToF_mm(ToFSensors[Rand_L])) - 40;
		//distanceScan = readToF_mm(ToFSensors[Rand_L]) - 40;
		distanceScan = randL.distance() - 40;
	}

	else if (side) // rechts scannen
	{
		//distanceScan = (cos(AGV_Angle_RAD) * readToF_mm(ToFSensors[Rand_R])) - 40;
		//distanceScan = readToF_mm(ToFSensors[Rand_R]) - 40;
		distanceScan = randR.distance() - 40;
	}


	distance = space;

	if (distanceScan < distance)
	{
		bijstuurWaarde = distance - distanceScan;

		if (!side)
		{
			turnSide = StuurRichting::RIGHT;
		}
		else if (side)
		{
			turnSide = StuurRichting::LEFT;
		}

	}

	else if (distanceScan > distance)
	{
		bijstuurWaarde = distance - distanceScan;

		if (!side)
		{
			turnSide = StuurRichting::LEFT;
		}
		else if (side)
		{
			turnSide = StuurRichting::RIGHT;
		}
	}

	else // if (distanceScan == distance)
	{
		bijstuurWaarde = 0;
		turnSide = StuurRichting::STADY;
	}
}

void scanTwoSides(double& bijstuurWaarde, double& distance, StuurRichting& turnSide)
{
	//int16_t distanceL = (cos(AGV_Angle_RAD) * readToF_mm(ToFSensors[Rand_L])) - 40;
	//int16_t distanceR = (cos(AGV_Angle_RAD) * readToF_mm(ToFSensors[Rand_R])) - 40;

	//int16_t distanceL = (readToF_mm(ToFSensors[Rand_L])) - 40;
	//int16_t distanceR = (readToF_mm(ToFSensors[Rand_R])) - 40;

	float distanceL = randL.distance() - 40;
	float distanceR = randR.distance() - 40;

	distance = (distanceL + distanceR) / 2 ;

	if (distanceL < distance && distanceR > distance)
	{
		bijstuurWaarde = distance - distanceL;
		turnSide = StuurRichting::RIGHT;
	}

	else if (distanceL > distance && distanceR < distance)
	{
		bijstuurWaarde = distance - distanceR;
		turnSide = StuurRichting::LEFT;
	}

	else //if (distanceL == distance)
	{
		bijstuurWaarde = 0;
		turnSide = StuurRichting::STADY;
	}
}

void stuurRichting(double& bijstuurWaarde, double& distance, StuurRichting& turnSide)
{
	enum class Side
	{
		left,
		right
	};
	const uint16_t path_0_3 = 100 - 85 + 22;
	const uint16_t path_4_5 = 150 - 85 + 22;

	switch (pathNumber)
	{
	case 0:
		switch (orientation)
		{
		case Orientation::POSITIVE_X:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::left, path_0_3);
			break;
		case Orientation::NEGATIVE_X:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::right, path_0_3);
			break;
		default:
			break;
		}
		break;
	case 1:
	case 2:
		scanTwoSides(bijstuurWaarde, distance, turnSide);
		break;
	case 3:
		switch (orientation)
		{
		case Orientation::POSITIVE_X:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::right, path_0_3);
			break;
		case Orientation::NEGATIVE_X:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::left, path_0_3);
			break;
		default:
			break;
		}
		break;
	case 4:
		switch (orientation)
		{
		case Orientation::POSITIVE_Y:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::right, path_4_5);
			break;
		case Orientation::NEGATIVE_Y:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::left, path_4_5);
			break;
		default:
			break;
		}
		break;
	case 5:
		switch (orientation)
		{
		case Orientation::POSITIVE_Y:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::left, path_4_5);
			break;
		case Orientation::NEGATIVE_Y:
			scanOneSide(bijstuurWaarde, distance, turnSide, (boolean)Side::right, path_4_5);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	/*Serial.print("Links: ");
	Serial.print(_distanceL);
	Serial.print("  Rechts:  ");
	Serial.print(_distanceR);
	Serial.print("  Afstand:  ");
	Serial.print(Distance);
	Serial.print("  De waarde is ");
	Serial.println(bijstuurWaarde);*/
}
#pragma endregion



void setup_ToF_Detectie() {
	Wire.begin();
	pinMode(LATCHPIN, OUTPUT);
	pinMode(CLOCKPIN, OUTPUT);
	pinMode(DATAPIN, OUTPUT);


	for (int i = 0; i <= 3; i++) { setupResetToFsensor(i); }

	delay(10);
	randR = ToF(Rand_R);
	randL = ToF(Rand_L);
}

void loop_ToF_Detectie() {
	//TreeScanMode = 1;
	TreeProssing(TreeScanMode);
	randR.read();
	randL.read();

	/*Serial.print(randR.distance());
	Serial.print("  ");
	Serial.println(randL.distance());*/

	//stuurRichting(distanceL, distanceR);
}
