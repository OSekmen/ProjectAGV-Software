/*
 Name:		Main.ino
 Created:	5/21/2019 1:37:04 PM
 Author:	Pim Stoopman
*/

#include <Ultrasonic.h>
#include "ObstakelDetectie.h"


// the setup function runs once when you press reset or power the board
void setup() 
{
	setupObstakelDetectie();
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	updateObstakelDetectie();
}
