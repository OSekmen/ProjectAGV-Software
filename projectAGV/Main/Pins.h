#pragma once
/*
In dit bestand staan welke pins gebruikt moeten worden
*/

#pragma region Obstakel Detectie
#define FRONT_TRIGGER 0
#define REAR_TRIGGER 0
#define FRONT_ECHO 0
#define REAR_ECHO 0
#define SERVO 0
#pragma endregion

#pragma region ToF Detectie
#define RESET_RAND_L (1<<1)
#define RESET_RAND_R (1<<2)
#define RESET_BOOM_L (1<<3)
#define RESET_BOOM_R (1<<0)
#pragma endregion

#pragma region Aandrijving
#define DIRECTION_L A2
#define PULSE_L		A3
#define DIRECTION_R A0
#define PULSE_R		A1
#pragma endregion

#pragma region IC Control
#define LATCHPIN 14
#define CLOCKPIN 16
#define DATAPIN	 15
static uint8_t IC_REG = 0;
#pragma endregion

#pragma region xD
#define NOODSTOP 8
#pragma endregion
