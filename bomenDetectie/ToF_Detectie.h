#pragma once
#include "Pins.h"

VL53L0X sensorRand_L;
VL53L0X sensorRand_R;
VL53L0X sensorBoom_L;
VL53L0X sensorBoom_R;

byte ScaneMode; // welke kant moet er gescand worden
boolean StopCommand; // commando om de AGV te stoppen 

VL53L0X sensorArray[] = { sensorRand_L, sensorRand_R, sensorBoom_L, sensorBoom_R };
const int sensorAddresses[] = { 0x30, 0x31, 0x32, 0x33 };
const int sensorPinResetArray[] = { RESET_RAND_L, RESET_RAND_R, RESET_BOOM_L, RESET_BOOM_R };

enum sensorName {
	Rand_L = 0,
	Rand_R = 1,
	Boom_L = 2,
	Boom_R = 3
} SensorName;

long int Distance; // afstand tot hek
int pwm;

void resetToFsensor(uint8_t sensorNumber) {
	const  uint16_t timeOut = 10;
	static uint16_t prev_time[sizeof sensorArray];

	static uint8_t resetProcress[sizeof sensorArray];

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
			sensorArray[sensorNumber].init();
			sensorArray[sensorNumber].setAddress(sensorAddresses[sensorNumber]);
			sensorArray[sensorNumber].setTimeout(500);

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

	sensorArray[sensorNumber].init();
	sensorArray[sensorNumber].setAddress(sensorAddresses[sensorNumber]);
	sensorArray[sensorNumber].setTimeout(500);
}

bool ScanTree(uint8_t sensorNumber) {
	static bool reset[sizeof sensorArray];

	long distance = sensorArray[sensorNumber].readRangeSingleMillimeters();

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

void TreeProssing(uint8_t mode, boolean * command) {
	switch (mode) {
	case 0:	//geen scan
		break;

	case 1:	//links van AGV
		if (ScanTree(Boom_L)) {
			// doe iets!
			*command = true;
		}
		else
			*command = false;
		break;

	case 2:	//rechts van AGV
		if (ScanTree(Boom_R)) {
			// doe iets!
			*command = true;
		}
		else
			*command = false;
		break;
	}
}

#pragma region Rand Detectie
int leesWaarde(VL53L0X sensor) {
	int waarde;
	waarde = sensor.readRangeSingleMillimeters();
	return waarde;
}

int stuurRichting(int long distanceL, int long distanceR) {
	Distance = ((abs(75 - distanceL) + abs(75 - distanceR)) / 2);

	if (distanceL < 70 && distanceR > 80) {
		pwm = map(Distance, 0, 75, 0, 100);
		Serial.println(pwm);
		return pwm;
	}
	else if (distanceL > 80 && distanceR < 70) {
		pwm = map(Distance, 0, 75, 0, -100);
		Serial.println(pwm);
		return pwm;
	}
	else if (distanceL > 70 && distanceL < 80) {
		pwm = 0;
		return pwm;
	}
}
#pragma endregion



void setup_ToF_Detectie() {
	for (int i = 0; i <= 3; i++) { setupResetToFsensor(i); }
}

void loop_ToF_Detectie() {
	TreeProssing(ScaneMode, &StopCommand);

	int16_t distanceL = leesWaarde(sensorArray[Rand_L]);
	int16_t distanceR = leesWaarde(sensorArray[Rand_R]);
	// put your main code here, to run repeatedly:
	

	stuurRichting(distanceL, distanceR);
}
