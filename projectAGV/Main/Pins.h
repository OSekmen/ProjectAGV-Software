#pragma once
/*
In dit bestand staan welke pins gebruikt moeten worden
*/

#pragma region Obstakel Detectie

#define FRONT_TRIGGER 0
#define BACK_TRIGGER 0
#define FRONT_ECHO 0
#define BACK_ECHO 0

#define SERVO 0

#pragma endregion

#pragma region ToF Detectie

#define RESET_RAND_L (1<<1)
#define RESET_RAND_R (1<<2)
#define RESET_BOOM_L (1<<3)
#define RESET_BOOM_R (1<<4)

#pragma endregion

#pragma region Aandrijving



#pragma endregion

#pragma region IC control
#define LATCHPIN 0
#define CLOCKPIN 0
#define DATAPIN	 0
static uint8_t IC_REG = 0;
#pragma endregion
