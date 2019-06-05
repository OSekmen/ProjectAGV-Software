VL53L0X sensorRand_L;
VL53L0X sensorRand_R;
VL53L0X sensorBoom_L;
VL53L0X sensorBoom_R;

VL53L0X sensorArray[] = { sensorRand_L, sensorRand_R, sensorBoom_L, sensorBoom_R };
const int sensorAddresses[] = { 0x30, 0x31, 0x32, 0x33 };

enum sensorName {
	Rand_L = 0,
	Rand_R = 1,
	Boom_L = 2,
	Boom_R = 3
} SensorName;


void resetToFsensor(uint8_t sensorNumber) {
	const  uint16_t timeOut = 10;
	static uint16_t prev_time[sizeof sensorArray];

	uint16_t curr_time = millis();
	switch (resetProcress[sensorNumber]) {
	case 0:
		digitalWrite(sensorResetArray[sensorNumber], false);
		prev_time[sensorNumber] = curr_time;
		resetProcress[sensorNumber] = 1;
		break;

	case 1:
		if (curr_time >= (prev_time[sensorNumber] + timeOut)) {
			digitalWrite(sensorResetArray[sensorNumber], true);
			prev_time[sensorNumber] = curr_time;
			resetProcress[sensorNumber] = 2;
		}
		break;

	case 2:
		if (curr_time >= (prev_time[sensorNumber] + timeOut)) {
			digitalWrite(sensorResetArray[sensorNumber], true);
			prev_time[sensorNumber] = curr_time;

			sensorArray[sensorNumber].init();
			sensorArray[sensorNumber].setAddress(sensorAddresses[sensorNumber]);
			sensorArray[sensorNumber].setTimeout(500);

			resetProcress[sensorNumber] = 0;
		}
		break;
	}
}

void TreeProssing(uint8_t mode) {
	switch (mode) {
	case 0:	//geen scan
		break;

	case 1:	//links van AGV
		if (ScanTree(Boom_L)) {
			// doe iets!
		}
		break;

	case 2:	//rechts van AGV
		if (ScanTree(Boom_R)) {
			// doe iets!
		}
		break;
	}
}

bool ScanTree(uint8_t sensorNumber) {
	long distance = sensorArray[sensorNumber].readRangeSingleMillimeters();

	if (distance >= 80 && distance <= 125) {	//200/2 + 60/2 - (170/2 + 20) = afstand van sensor tot boom 
		return true;
	}
	else {
		return false;
	}
}

