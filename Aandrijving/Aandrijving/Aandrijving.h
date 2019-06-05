#define RAMP 2
#define SPEEDMAX 200
#define SPEEDMIN 1000

//Pinnummers aan naam gekoppelt
int stepL = A2;
int dirL = A3;
int stepR = A0;
int dirR = A1;

int schakelaar = 2; //Tijdelijke schakelaar die het detecteren van bomen simuleert.
int pot = A7; //Tijdelijke pot meter om de rand richting te simuleren.

//Globale variabele
int stepLState = 0; //Waarde van de puls links, moet nog LOW worden maar voor het testen is het 0.
int stepRState = 4; //Waarde van de puls rechts moet nog veranderd worden naar LOW, is nu een 4 om links en rechts onder elkaar te plotten.
unsigned long previousLMillis = 0; //Slaat het aantal millis op om de tijd van de nieuwe puls te bereken voor links.
unsigned long previousRMillis = 0; //Slaat het aantal millis op om de tijd van de nieuwe puls te bereken voor rechts.
unsigned int speedLMax = 200; //Max snelheid linker motor.
unsigned int speedRMax = 200; //Max snelheid rechter motor.
int bijsturen; //Waarde van -100 t/m 100, -100 betekent hard veel naar links, 100 betekent veel naar rechts.
int boom = 0; //Wanneer 0 betekent geen boom je kan rijden, wanneer 1 betekent boom wacht tot deze weer 0 is.

void aandrijving() {
	bijstuurwaarde();
	bijstuurSnelheid();

	int speedL; //Begin snelheid van motoren wanneer het programma start, hoe hoger de waarde hoe langzamer de AGV links start.
	int speedR; //Begin snelheid van motoren wanneer het programma start, hoe hoger de waarde hoe langzamer de AGV rechts start.
	unsigned long currentLMillis = millis();
	unsigned long currentRMillis = millis();
	boom = digitalRead(schakelaar);

	if (bijsturen == 0) { //Wanneer de AGV gewoon rechtdoor moet rijden.
		if (speedL < speedR) { //Als links sneller gaat dan rechts, maak links dan gelijk aan rechts.
			speedL = speedR;
		}
		else { //Als rechts sneller gaat dan links, maak rechts dan gelijk aan links.
			speedR = speedL;
		}
	}

	if (speedL > speedLMax) {
		speedL = speedL - RAMP; //Wanneer de maximale snelheid links nog niet bereikt is zal de motor versnellen, de versnelling verandert door de RAMP.
	}
	if (speedL < speedLMax) {
		speedL = speedL + RAMP; //Wanneer de maximale snelheid links overschreiden is zal de motor vertragen, de vertraging verandert door de RAMP.
	}

	Serial.print("StepL: ");
	Serial.print(stepLState);
	if (currentLMillis - previousLMillis >= speedL) {
		previousLMillis = currentLMillis;
		if ((boom == 1) || (bijsturen == 100)) {
			speedL = 1000;
			stepLState = 0;
		}
		else {
			if (stepLState == 0) {
				stepLState = 1;
			}
			else {
				stepLState = 0;
			}
		}
		digitalWrite(stepL, stepLState);
	}

	Serial.print("   ");

	if (speedR > speedRMax) {
		speedR = speedR - RAMP; //Wanneer de maximale snelheid rechts nog niet bereikt is zal de motor versnellen, de versnelling verandert door de RAMP.
	}
	if (speedR < speedRMax) {
		speedR = speedR + RAMP; //Wanneer de maximale snelheid rechts overschreiden is zal de motor vertragen, de vertraging verandert door de RAMP.
	}

	Serial.print("StepR: ");
	Serial.println(stepRState);
	if (currentRMillis - previousRMillis >= speedR) {
		previousRMillis = currentRMillis;
		if ((boom == 1) || (bijsturen == -100)) {
			stepRState = 4;
		}
		else {
			if (stepRState == 4) {
				stepRState = 5;
			}
			else {
				stepRState = 4;
			}
		}
		digitalWrite(stepR, stepRState);
	}
}

void bijstuurSnelheid() {
	if (bijsturen != 0) {
		if (bijsturen > 0) { //SPEEDMIN is de langzaamste waarde voordat de AGV stilstaat, dit bepaald ook hoe groot de ramp up is.
			speedLMax = (((SPEEDMIN - SPEEDMAX) / 100) * bijsturen + SPEEDMAX);
		}
		if (bijsturen < 0) {
			speedRMax = (-((SPEEDMIN - SPEEDMAX) / 100) * bijsturen + SPEEDMAX);
		}
	}
	else {
		speedLMax = SPEEDMAX; //Wanneer er niet bijgestuurd hoeft te worden mag links maximale snelheid.
		speedRMax = SPEEDMAX; //Wanneer er niet bijgestuurd hoeft te worden mag rechts maximale snelheid.
	}
	//Serial.print("   ");
	//Serial.print("speedLMax: ");
	//Serial.print(speedLMax);
	//Serial.print("   ");
	//Serial.print("speedRMax: ");
	//Serial.print(speedRMax);
	//Serial.print("   ");
}

void bijstuurwaarde() { //Deze tijdelijke functie vertaald de waarde van de pot in een percentage sturen.
	int potWaarde;
	potWaarde = analogRead(pot);
	//Serial.print("potWaarde: ");
	//Serial.print(potWaarde);
	//Serial.print("   ");
	if (potWaarde < 400) { //Wanneer de pot waarde minder dan 400 is wordt het een waarde van -1 tot -100.
		bijsturen = ((potWaarde / 4) - 100);
	}
	if ((potWaarde >= 400) && (potWaarde < 623)) { //Wanneer de pot waarde tussen de 400 en 622 ligt wordt het een waarde van 0.
		bijsturen = 0;
	}
	if (potWaarde >= 623) {
		bijsturen = ((potWaarde - 623) / 4); //Wanneer de pot waarde meer dan 622 is wordt het een waarde van 1 tot 100.
	}
	//Serial.print("bijsturen: ");
	//Serial.print(bijsturen);
	//Serial.print("   ");
}


void setupAandrijving() {
	//Inputs en outputs declareren
	pinMode(stepL, OUTPUT);
	pinMode(dirL, OUTPUT);
	pinMode(stepR, OUTPUT);
	pinMode(dirR, OUTPUT);
	pinMode(pot, INPUT);

	Serial.begin(9600);
}

void loopAandrijving() {
	aandrijving();
}