#pragma once
/*
In de normale mode betekent FORWARDS en BACKWARDS dat de AGV kan rijden in de gewenste rij richting.
STOP betekent dat er iets voor de rij richting staat en dat de AGV niet mag rijden.

In de volgmode betekent FORWARDS dat de AGV vooruit moet en BACKWARDS dat de AGV achteruit moet. 
LEFT en RIGHT betekent dat de AGV een bocht moet maken.
Bij STOP en SCANNING moet de AGV stil staan.

NUM_STATES is het aantal states.
*/
enum class State {
	FORWARDS,
	BACKWARDS,
	LEFT,
	RIGHT,
	STOP,
	SCANNING,
	NUM_STATES
};