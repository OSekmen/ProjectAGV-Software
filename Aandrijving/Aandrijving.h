#pragma once
#include "Pins.h"
#include "GlobalVariables.h"
#include "ToF_Detectie.h"

#define StepperLinks  0
#define StepperRechts 1

const uint16_t diameterWiel  = 75;
const uint16_t microStepping = 8;
uint32_t MaxFRQ = 0;
uint32_t MaxBijstuurFRQ;

const uint16_t WielBase = 126;
double LengteStap;
double AngleConst_RAD; /// Niet aankomen deze mag alleen gelezen worden en wordt een keer gezet!!!!

boolean StepperDirection[2] = { false, false };
boolean StepperStopBit; /// Deze variable wordt gebruikt voor het stoppen van de AGV
boolean SteppersTurningBit;

int32_t stepsToPass[2];
uint32_t stepsConter[2];


void StepperHandler()
{
	static uint32_t _stepsCounter[2]; // telt de counter stappen die voorbij zijn zonder de pulse pin te veranderen
	static boolean  _stepperPulse[2]; // pulse boolean
	static boolean  _stepperChange[2];
	static int8_t	_direction = (int)direction;

	_direction = (int)direction;

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
				_stepperChange[StepperLinks] = true;

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
				_stepperChange[StepperRechts] = true;

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



		if (!_stepperChange[StepperLinks] && !_stepperChange[StepperRechts])
		{
			/// er word geen stap gezet, dus de positie hoeft niet veranderd te worden.
		}
		else if (!_stepperChange[StepperLinks] && _stepperChange[StepperRechts])
		{
			AGV_Angle_RAD += AngleConst_RAD * _direction;

			pos.x = pos.x + (cos(AGV_Angle_RAD) * LengteStap) * _direction;
			pos.y = pos.y + (sin(AGV_Angle_RAD) * LengteStap) * _direction;

			_stepperChange[StepperLinks] = false;
			_stepperChange[StepperRechts] = false;
		}
		else if (_stepperChange[StepperLinks] && !_stepperChange[StepperRechts])
		{
			AGV_Angle_RAD -= AngleConst_RAD * _direction;

			pos.x = pos.x + (cos(AGV_Angle_RAD) * LengteStap) * _direction;
			pos.y = pos.y + (sin(AGV_Angle_RAD) * LengteStap) * _direction;

			_stepperChange[StepperLinks] = false;
			_stepperChange[StepperRechts] = false;
		}
		else if (_stepperChange[StepperLinks] && _stepperChange[StepperRechts])
		{
			pos.x = pos.x + (cos(AGV_Angle_RAD) * LengteStap) * _direction;
			pos.y = pos.y + (sin(AGV_Angle_RAD) * LengteStap) * _direction;

			_stepperChange[StepperLinks] = false;
			_stepperChange[StepperRechts] = false;
		}
		else
		{ }
		if (AGV_Angle_RAD >= 2 * PI)
		{
			AGV_Angle_RAD -= (2 * PI);
		}
		if (AGV_Angle_RAD <= -2 * PI)
		{
			AGV_Angle_RAD += (2 * PI);
		}
		//Serial.print("Angle: ");
		//Serial.println(AGV_Angle_RAD);
	}

	else if (StepperStopBit) /// wanneer de AGV stil moet staan
	{
		// Het tellen van de klok heeft nu geen zin omdat hij niet hoeft te rijden, daarom staan ze op null
		_stepsCounter[StepperLinks] = 0;
		_stepsCounter[StepperRechts] = 0;

		//De pulse zijn, wanneer de AGV niet meer reid, laag.
		//Zodat wanneer de AGV weer moet rijden de pulsen gelijk hoog gemaakt kunnen worden. 
		_stepperPulse[StepperLinks] = false;
		_stepperPulse[StepperRechts] = false;

		_stepperChange[StepperLinks] = false;
		_stepperChange[StepperRechts] = false;
	}

	digitalWrite(DIRECTION_L, StepperDirection[StepperLinks]);
	digitalWrite(DIRECTION_R, StepperDirection[StepperRechts]);

	digitalWrite(PULSE_L, _stepperPulse[StepperLinks]);
	digitalWrite(PULSE_R, _stepperPulse[StepperRechts]);
}



#pragma region Setup Aandrijving

void StepOriantationCalculation(double distanceTravel, uint16_t stepResolution)
{
	double _travel_mm = distanceTravel / stepResolution;

	double _angleStep = (_travel_mm * 360.0) / (2.0 * PI * WielBase);

	LengteStap = (2 * (sin(DEG_TO_RAD *(_angleStep / 2)) * (WielBase / 2))) * 0.1;

	AngleConst_RAD = DEG_TO_RAD * (((180.0 - _angleStep) / 2.0) - 90);

	//Serial.println(AngleConst_RAD, 5);
}

uint32_t ClockSpeedCalculations()
{
	double _distanceTravel = PI * diameterWiel; // bereken de omtrek 
	uint16_t _stepResolution = 200 * microStepping; // bereken de aantal stappen per omwenteling

	StepOriantationCalculation(_distanceTravel, _stepResolution);

	double _travelSteps = _stepResolution / _distanceTravel;
	double _stepsPer2000 = _travelSteps * 2000;

	MaxFRQ = (_stepsPer2000 * 2.0) / 15.0; // keer 2, omdat 50% aan & 50% uit

	return ((1.0 / (double)MaxFRQ) * 1000000); // zet het om tot microseconden
}
#pragma endregion


void setupAandrijving()
{
	pinMode(DIRECTION_L, OUTPUT);
	pinMode(PULSE_L, OUTPUT);
	pinMode(DIRECTION_R, OUTPUT);
	pinMode(PULSE_R, OUTPUT);
	Timer1.initialize(ClockSpeedCalculations());
	Timer1.attachInterrupt(StepperHandler); // attach the service routine here

	MaxBijstuurFRQ = (MaxFRQ / 500);

	
}


#pragma region Loop Aandrijving

void loopAandrijving()
{
	static boolean firstHandel[5];
	static AandrijfMode _safeMode_by_nood;
	static boolean _noodsetCheck;
	static boolean _noodresetCheck;
	
	double _bijstuurWaarde;
	double _distance;
	StuurRichting _stuurRichting;

	boolean NoodstopStatus = digitalRead(NOODSTOP);

	if (NoodstopStatus)
	{
		if (!_noodsetCheck)
		{
			_safeMode_by_nood = aandrijvingMode;

			_noodsetCheck = true;
			_noodresetCheck = false;
			NoodstopActive = true;

			aandrijvingMode = Stop;
		}

		NoodstopActive = true;
	}
	else if (!NoodstopStatus && !_noodresetCheck)
	{
		aandrijvingMode = _safeMode_by_nood;
		_noodsetCheck = false;
		_noodresetCheck = true;
	}
	else {
		NoodstopActive = false;
	}
	
	Serial.print("Noodstop: ");
	Serial.println(NoodstopStatus);
	switch (aandrijvingMode)
	{
	case Stop:
		StepperStopBit = true;
		SteppersTurningBit = false;
		//StepperMode = Stop;
		break;

	case Vooruit:
		
		if (!firstHandel[Vooruit])
		{
			AGV_Angle_RAD += PI;

			firstHandel[Vooruit] = true;
			firstHandel[Achteruit] = false;

			SteppersTurningBit = false;
		}

		StepperStopBit = false;
		stuurRichting(_bijstuurWaarde, _distance, _stuurRichting);
		switch (_stuurRichting)
		{
		case StuurRichting::STADY:
			stepsToPass[StepperLinks] = 0;
			stepsToPass[StepperRechts] = 0;
			break;
		case StuurRichting::LEFT:
			stepsToPass[StepperLinks] = map_double((_bijstuurWaarde), _distance, 0, MaxBijstuurFRQ, 0);
			stepsToPass[StepperRechts] = 0;
			break;
		case StuurRichting::RIGHT:
			stepsToPass[StepperLinks] = 0;
			stepsToPass[StepperRechts] = map_double((_bijstuurWaarde), _distance, 0, MaxBijstuurFRQ, 0);
			break;
		default:
			break;
		}
		StepperDirection[StepperLinks] = 1;
		StepperDirection[StepperRechts] = 1;
		break;

	case Achteruit:
		if (!firstHandel[Achteruit])
		{
			AGV_Angle_RAD -= PI;

			firstHandel[Achteruit] = true;
			firstHandel[Vooruit] = false;

			SteppersTurningBit = false;
		}
		StepperStopBit = false;

		stuurRichting(_bijstuurWaarde, _distance, _stuurRichting);

		switch (_stuurRichting)
		{
		case StuurRichting::STADY:
			stepsToPass[StepperLinks] = 0;
			stepsToPass[StepperRechts] = 0;
			break;
		case StuurRichting::LEFT:
			stepsToPass[StepperLinks] = map_double(pow(_bijstuurWaarde, 2), _distance, 0, 0, MaxBijstuurFRQ);
			stepsToPass[StepperRechts] = 0;
			break;
		case StuurRichting::RIGHT:
			stepsToPass[StepperLinks] = 0;
			stepsToPass[StepperRechts] = map_double(pow(_bijstuurWaarde, 2), _distance, 0, 0, MaxBijstuurFRQ);
			break;
		default:
			break;
		}
		StepperDirection[StepperLinks] = 0;
		StepperDirection[StepperRechts] = 0;
		break;

	case Linksom:
		//StepperMode = Linksom;
		StepperStopBit = false;
		SteppersTurningBit = true;
		break;

	case Rechtsom:
		//StepperMode = Rechtsom;
		StepperStopBit = false;
		SteppersTurningBit = true;
		break;

	default:
		break;
	}
}

#pragma endregion
