#pragma once
#include "GlobalVariables.h"

String text; //var String for raw input
String command; //var String for editted input
int input; //var int for raw input

uint32_t millisnow;
uint32_t previousmillis;

int cntFF;

void inputCode() {
	while (Serial1.available() > 0) { //while nextion --> arduino communication
		//static int cntFF;
		input = Serial1.read();
		//Serial.print(input);
		switch (input) {
		case 0: //ignore error message
			break;
		case 26://ignore error message
			break;
		case 112:
			if (text.length() < 2) {//nextion Startcode
				text = "";
			}
			else {
				text += char(input); // normal letter 'p'
			}
			break;
		case 255: //nextion Stopcode
			cntFF++;
			break;
		default:
			text += char(input); // ASCII hex to char
			break;
		}
		//Serial.println(cntFF);
		if (cntFF == 3) { //3 Stop codes = end of transmission
			//Serial.println(text);
			switch (text[0]) {
			case 'D':
				schermMode = Scherm::DEBUG;
				break;
			case 'H':
				schermMode = Scherm::HOME;
				break;
			case 'M':
				schermMode = Scherm::MANUAL;
				break;
			case 'V':
				mode = Mode::FOLLOW;
				break;
			case 'A':
				mode == Mode::NORMAL;
				break;

			/*case 'M':
				magnet = !magnet;
				digitalWrite(33, magnet);
				digitalWrite(magneetactief, !magnet);
				break;
			case 'Z':
				if (text.length() > 1) {
					command = text;
					setPositionZ(); //processing nextion--> arduino to target coordinates.
				}
				break;*/
			default:
				if (text.length() > 2) {
					command = text;
					//setPositionXY(); //processing nextion--> arduino to target coordinates.
				}
				break;
			}

			text = "";
			cntFF = 0;
		}
	}
}

/*
void writeToNextion() {
	//write current sensor distance to nextion screen as TEXT
	nexWriteTxt("Aflees", "t8", String(positie.x));
	nexWriteTxt("Aflees", "t9", String(positie.y));
	nexWriteTxt("Aflees", "t10", String(positie.z));

	//write target distance to nextion screen as TEXT
	nexWriteTxt("Aflees", "t3", String(target.x));
	nexWriteTxt("Aflees", "t4", String(target.y));
	nexWriteTxt("Aflees", "t5", String(target.z));


	//write current sensor distance to nextion screen as SLIDER values
	//X-axis
	if (positie.x < target.x - 15) {
		nexWriteSlid("Aflees", "j0", positie.x, 0, target.x);
		nexWriteSlid("Aflees", "j1", 0, 0, 100);
	}
	else if (positie.x > target.x + 5) {
		nexWriteSlid("Aflees", "j1", positie.x, target.x, 1000);
		nexWriteSlid("Aflees", "j0", 100, 0, 100);
	}

	//Y-axis
	if (positie.y < target.y - 5) {
		nexWriteSlid("Aflees", "j2", positie.y, 0, target.y);
		nexWriteSlid("Aflees", "j3", 0, 0, 100);
	}
	else if (positie.y > target.y + 5) {
		nexWriteSlid("Aflees", "j3", positie.y, target.y, 500);
		nexWriteSlid("Aflees", "j2", 100, 0, 100);
	}
	//Z-axis
	if (positie.z < target.z - 5) {
		nexWriteSlid("Aflees", "j4", positie.z, 0, target.z);
		nexWriteSlid("Aflees", "j5", 0, 0, 100);
	}
	else if (positie.z > target.z + 5) {
		nexWriteSlid("Aflees", "j5", positie.z, target.z, 500);
		nexWriteSlid("Aflees", "j4", 100, 0, 100);
	}
	previousmillis = millisnow;
}*/

void nexWriteVal(String scherm, String element, int msg) {
	String cmd;
	cmd += scherm;
	cmd += ".";
	cmd += element;
	cmd += ".val=";
	cmd += msg;
	Serial1.print(cmd);
	Serial1.write(0xff);
	Serial1.write(0xff);
	Serial1.write(0xff);
}

void nexWriteSlid(String screen, String element, int msg, int mini1, int maxi1) {
	int mapValue = map(msg, mini1, maxi1, 0, 100);
	String cmd;
	cmd += screen;
	cmd += ".";
	cmd += element;
	cmd += ".val=";
	cmd += mapValue;
	Serial1.print(cmd);
	Serial1.write(0xff);
	Serial1.write(0xff);
	Serial1.write(0xff);
}

String nexWriteTxt(String screen, String element, String msg) {
	String cmd;
	cmd += screen;
	cmd += ".";
	cmd += element;
	cmd += ".txt=\"";
	cmd += msg;
	cmd += "\"";
	Serial1.print(cmd);
	Serial1.write(0xff);
	Serial1.write(0xff);
	Serial1.write(0xff);
	return cmd;
}

void setupCommunicatie() {
	schermMode = Scherm::HOME;
	Serial1.begin(9600);
	previousmillis = millis();
}

void loopCommunicatie() {
	/*millisnow = millis();

	inputCode();

	if (millisnow - previousmillis > 0) {
		if (!Serial1.available()) {
			Serial.println("magic");
			//writeToNextion();
			String cmd = nexWriteTxt("Debug", "t2", "12");

			Serial.print(cmd);
			Serial.write(0xff);
			Serial.write(0xff);
			Serial.write(0xff);
		}

		previousmillis = millis();
	}*/
	inputCode();
	if (millis() - previousmillis > 250 && schermMode == Scherm::DEBUG) {
		nexWriteTxt("Debug", "t6", String(readToF_mm(ToFSensors[Rand_L])));
		nexWriteTxt("Debug", "t7", String(readToF_mm(ToFSensors[Rand_R])));
		nexWriteTxt("Debug", "t8", String(0));
		nexWriteTxt("Debug", "t9", String(0));
		nexWriteTxt("Debug", "t10", String(pos.x));
		nexWriteTxt("Debug", "t11", String(pos.y));
		nexWriteTxt("Debug", "t13", String(AGV_Angle_RAD));
		nexWriteTxt("Debug", "t14", String(printDirection(direction)));
		previousmillis = millis();
	}
}
