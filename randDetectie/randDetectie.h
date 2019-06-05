#include <VL53L0X.h>
#include <Wire.h>

VL53L0X sensorL;
VL53L0X sensorR;


VL53L0X sensorArray[] = { sensorL, sensorR };


long int distance;
int sensorLreset = 8;
int sensorRreset = 7;
int pwm;

int stuurRichting(int long distanceL, int long distanceR) {
	distance = ((abs(75 - distanceL) + abs(75 - distanceR)) / 2);

	if (distanceL < 70 && distanceR > 80) {
		pwm = map(distance, 0, 75, 0, 100);
		Serial.println(pwm);
		return pwm;
	}
	else if (distanceL > 80 && distanceR < 70) {
		pwm = map(distance, 0, 75, 0, -100);
		Serial.println(pwm);
		return pwm;
	}
	else if (distanceL > 70 && distanceL < 80) {
		pwm = 0;
		return pwm;
	}
}
int leesWaarde(VL53L0X sensor) {
	int waarde;
	waarde = sensor.readRangeSingleMillimeters();
	return waarde;
}

void setupRandDetectie() {
	// put your setup code here, to run once:

	Wire.begin();

	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);

	//reset all sensors by pulling shutdownpins low and high (HIGH = active)
	digitalWrite(sensorLreset, LOW);
	digitalWrite(sensorRreset, LOW);
	delay(10);
	digitalWrite(sensorLreset, HIGH);
	digitalWrite(sensorRreset, HIGH);
	delay(10);

	//keep Left sensor on by pulling Right LOW, assign new adress
	digitalWrite(sensorRreset, LOW);
	sensorArray[0].init();
	sensorArray[0].setAddress(0x30);
	sensorArray[0].setTimeout(500);

	//repeat on Right Sensor
	digitalWrite(sensorRreset, HIGH);
	sensorArray[1].init();
	sensorArray[1].setAddress(0x31);
	sensorArray[1].setTimeout(0);

	Serial.begin(9600);
}

void loopRandDetectie() {
	long int distanceL;
	long int distanceR;
	// put your main code here, to run repeatedly:
	distanceL = leesWaarde(sensorArray[0]);
	distanceR = leesWaarde(sensorArray[1]);

	stuurRichting(distanceL, distanceR);
}
