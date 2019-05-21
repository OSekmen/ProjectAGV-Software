#pragma once
#include <Ultrasonic.h>

#define num_measurements 10

class ObstakelDetectie {
private:
	Ultrasonic us;
	uint16_t measurements[num_measurements];
	uint8_t index;

public:
	ObstakelDetectie() : us(Ultrasonic(0, 0)) {}

	ObstakelDetectie(uint8_t trig, uint8_t echo) : us(Ultrasonic(trig, echo)) {
		index = 0;
		uint16_t m = us.read();
		for (uint8_t i = 0; i < num_measurements; i++) {
			measurements[i] = m;
		}
	}

	void read() {
		measurements[index] = us.read();
	}

	float value() {
		float v = 0;
		for (uint8_t i = 0; i < num_measurements; i++) {
			v += measurements[i];
		}
		v /= num_measurements;
		return v;
	}
};

ObstakelDetectie front;
ObstakelDetectie back;

void setupObstakelDetectie() {
	front = ObstakelDetectie(1, 1);
	back = ObstakelDetectie(2, 2);
}

void updateObstakelDetectie() {
	front.read();
	back.read();
}