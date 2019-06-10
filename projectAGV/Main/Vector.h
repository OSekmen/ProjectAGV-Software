#pragma once
class Vector {
public:
	float x;
	float y;
	Vector() {}
	Vector(float _x, float _y) {
		x = _x;
		y = _y;
	}

	bool operator == (Vector& other) {
		if (x == other.x && y == other.y) { return true; }
		return 0;
	}

	Vector operator + (Vector& other) {
		return Vector(x + other.x, y + other.y);
	}

	Vector operator - (Vector& other) {
		return Vector(x - other.x, y - other.y);
	}

	Vector operator * (Vector& other) {
		return Vector(x * other.x, y * other.y);
	}

	Vector operator / (Vector& other) {
		return Vector(x / other.x, y / other.y);
	}

	float magnitude() {
		return sqrtf(pow(x, 2) + pow(y, 2));
	}
};