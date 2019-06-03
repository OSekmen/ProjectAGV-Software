#pragma once
/*
In de normale modus betekent FORWARD en BACKWARD dat de AGV kan rijden in de gewenste rij richting.
STOP betekent dat er iets voor de rij richting staat en dat de AGV niet mag rijden.

In de volgmodus betekent FORWARD dat de AGV vooruit moet en BACKWARD dat de AGV achteruit moet. 
LEFT en RIGHT betekent dat de AGV een bocht moet maken.
Bij STOP en SCANNING moet de AGV stil staan.

NUM_STATES is het aantal states.
*/
enum class State {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	STOP,
	SCANNING,
	NUM_STATES
};