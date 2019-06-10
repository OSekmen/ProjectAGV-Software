#pragma once
#include "Pins.h"
#include "GlobalVariables.h"

enum stepperMode
{
    Stop,
    Vooruit,
    Achteruit,
    Linksom,
    Rechtsom
}StepperMode;

#define StepperLinks  0
#define StepperRechts 1

#define PI 3.1415926535897932384626433832795


const uint16_t diameterWiel  = 75;
const uint16_t microStepping = 8;
uint32_t MaxFRQ = 0;


boolean StepperDirection[2] = { false, false };
boolean StepperStopBit; /// Deze variable wordt gebruikt voor het stoppen van de AGV
boolean SteppersTurningBit;

int32_t stepsToPass[2];
uint32_t stepsConter[2];



/*----------------------------------------------------------------------------------------*/



#pragma region Setup Aandrijving
void SetupAandrijving()
{
    Timer1.initialize(ClockSpeedCalculations()); 
	Timer1.attachInterrupt(StepperHandler); // attach the service routine here
}

uint32_t ClockSpeedCalculations()
{
    double _distanceTravel = PI * diameterWiel; // bereken de omtrek 
    uint16_t _stepResolution = 200 * microStepping; // bereken de aantal stappen per omwenteling

	double _travelSteps = _stepResolution / _distanceTravel;
	double _stepsPer2000 = _travelSteps * 2000;

    MaxFRQ = (_stepsPer2000 * 2.0) / 10.0; // keer 2, omdat 50% aan & 50% uit

    return ((1.0 / (double)MaxFRQ) * 1000000); // zet het om tot microseconden
}
#pragma endregion




/*----------------------------------------------------------------------------------------*/


#pragma region Loop Aandrijving

void LoopAandrijving(uint8_t mode)
{
    switch (mode)
    {
    case Stop:
        StepperStopBit = true;
		SteppersTurningBit = false;
        StepperMode = Stop;
        break;

    case Vooruit:
        StepperMode = Vooruit;
        StepperStopBit = false;
		SteppersTurningBit = false;

        if (bijstuurWaarde < 0)
        {
            stepsToPass[StepperLinks]  = map(abs(bijstuurWaarde), 0, 100, 0, MaxFRQ + 1);
            stepsToPass[StepperRechts] = 0;
        }
        else if (bijstuurWaarde > 0)
        {
            stepsToPass[StepperLinks]  = 0;
            stepsToPass[StepperRechts] = map(abs(bijstuurWaarde), 0, 100, 0, MaxFRQ + 1);
        }
        else if (bijstuurWaarde == 0)
        {
            stepsToPass[StepperLinks]  = 0;
            stepsToPass[StepperRechts] = 0;
        }
        StepperDirection[StepperLinks]  = 1;
        StepperDirection[StepperRechts] = 1;
        break;

    case Achteruit:
        StepperMode = Achteruit;
        StepperStopBit = false;
		SteppersTurningBit = false;

        if (bijstuurWaarde < 0)
        {
            stepsToPass[StepperLinks]  = map(abs(bijstuurWaarde), 0, 100, MaxFRQ + 1, 0);
            stepsToPass[StepperRechts] = 0;
        }
        else if (bijstuurWaarde > 0)
        {
            stepsToPass[StepperLinks]  = 0;
            stepsToPass[StepperRechts] = map(abs(bijstuurWaarde), 0, 100, MaxFRQ + 1, 0);
        }
        else if (bijstuurWaarde == 0)
        {
            stepsToPass[StepperLinks]  = 0;
            stepsToPass[StepperRechts] = 0;
        }
        StepperDirection[StepperLinks]  = 0;
        StepperDirection[StepperRechts] = 0;
        break;

    case Linksom:
        StepperMode = Linksom;
        StepperStopBit = false;
		SteppersTurningBit = true;
        break;

    case Rechtsom:
        StepperMode = Rechtsom;
        StepperStopBit = false;
		SteppersTurningBit = true;
        break;
    
    default:
        break;
    }
}


void PositionDetermination()
{
	static double orientatiePunten = 0;

	

	switch (orientation)
	{
	case Orientation::POSITIVE_X:
		break;
	case Orientation::POSITIVE_Y:
		break;
	case Orientation::NEGATIVE_X:
		break;
	case Orientation::NEGATIVE_Y:
		break;
	default:
		break;
	}



    switch (StepperMode)
    {
    case Vooruit:
		if (stepsConter[StepperLinks] == stepsConter[StepperRechts])
		{
			orientatiePunten = stepsConter[StepperLinks] * ((200.0 * microStepping) / (PI * (double)diameterWiel));
		}
		if (stepsConter[StepperLinks] > stepsConter[StepperRechts])
		{
			orientatiePunten = stepsConter[StepperRechts] * ((200.0 * microStepping) / (PI * (double)diameterWiel));

		}

		orientatiePunten = stepsConter[StepperLinks] * ((200.0 * microStepping) / (PI * (double)diameterWiel));
        break;

    case Achteruit:
        
        break;

    
    default:
        break;
    }
}


#pragma endregion






/*----------------------------------------------------------------------------------------*/



void StepperHandler()
{
    static uint32_t _stepsCounter[2]; // telt de counter stappen die voorbij zijn zonder de pulse pin te veranderen
    static boolean  _stepperPulse[2]; // pulse boolean

    if (!StepperStopBit) /// wanneer de AGV beweegt 
    {
        _stepsCounter[StepperLinks]++;
        _stepsCounter[StepperRechts]++;

        if (_stepsCounter[StepperLinks] > stepsToPass[StepperLinks]) /// Berekend alles van de linker stepper.
        {
            _stepperPulse[StepperLinks] = !_stepperPulse[StepperLinks];
            _stepsCounter[StepperLinks] = 0;

            if (_stepperPulse[StepperLinks])
            {
				if (!SteppersTurningBit)
				{
					if (StepperDirection[StepperLinks])
					{
						stepsConter[StepperLinks]++;
					}
					else if (!StepperDirection[StepperLinks])
					{
						stepsConter[StepperLinks]--;
					}
				}
            }
        }

        if (_stepsCounter[StepperRechts] > stepsToPass[StepperRechts]) /// Berekend alles van de rechter stepper.
        {
            _stepperPulse[StepperRechts] = !_stepperPulse[StepperRechts];
            _stepsCounter[StepperRechts] = 0;

            if (_stepperPulse[StepperRechts])
            {
				if (!SteppersTurningBit)
				{
					if (StepperDirection[StepperRechts])
					{
						stepsConter[StepperRechts]++;
					}
					else if (!StepperDirection[StepperRechts])
					{
						stepsConter[StepperRechts]--;
					}
				}
            }
        }
    }
	

    else if (StepperStopBit) /// wanneer de AGV stil moet staan
    {
        // Het tellen van de klok heeft nu geen zin omdat hij niet hoeft te rijden, daarom staan ze op null
        _stepsCounter[StepperLinks]  = 0; 
        _stepsCounter[StepperRechts] = 0;

        //De pulse zijn, wanneer de AGV niet meer reid, laag.
        //Zodat wanneer de AGV weer moet rijden de pulsen gelijk hoog gemaakt kunnen worden. 
        _stepperPulse[StepperLinks]  = false;
        _stepperPulse[StepperRechts] = false;
    }
    
	digitalWrite(DIRECTION_L, StepperDirection[StepperLinks]);
	digitalWrite(DIRECTION_R, StepperDirection[StepperRechts]);

    digitalWrite(PULSE_L, _stepperPulse[StepperLinks]);
    digitalWrite(PULSE_R, _stepperPulse[StepperRechts]);
}

