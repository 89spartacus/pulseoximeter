/*
*    FILE: demo01.pde
*  AUTHOR: Rob Tillaart
*    DATE: 2011 05 16
*
* PURPOSE: prototype TSL235R monitoring
*
* Digital Pin layout ARDUINO
* =============================
*  2     IRQ 0    - to TSL235R
*
* PIN 1 - GND
* PIN 2 - VDD - 5V
* PIN 3 - SIGNAL
*
Tut  für geringe Intensitäten. Ein direct hit von einem Laserpointer bringt das ganze zum Stocken.....

*/


#define INTEGRATION_TIME 10	//Integrationszeit in Millisekunden
#define LESELAENGE 15
#define NAME "TSL235R Readout"
#define BAUDRATE 115200

volatile unsigned long cnt = 0;
unsigned long oldcnt = 0;
unsigned long t = 0;
unsigned long last;
int _now;
int _oldnow;
unsigned long Dose = 0;		//uJ/cm^2
unsigned long  Intensity = 0;	//uW/cm^2; btw. Bestrahlungsstärke
unsigned long Timer = 0;

//void irq1()
//{
//	cnt++;
//}

///////////////////////////////////////////////////////////////////
//
// SETUP
//
void setup()
{
	Serial.begin(BAUDRATE);
	Serial.println("START");
	pinMode(2, INPUT_PULLUP);
	//attachInterrupt(0, irq1, RISING);
	EICRA = (1 << ISC00) | (1 << ISC01);	//Interrupt0 on rising edge, PIN2 an den Arduinos, matching ISR is called ISR(INT0_vect)
	EIMSK = (1 << INT0);
	_now = 0;
	_oldnow = 0;
}

///////////////////////////////////////////////////////////////////
//
// MAIN LOOP
//
void loop()
{


	if (Serial.available())
	{
		if (Serial.read() == 's')
		{
			int readLength;
			char rBuffer[LESELAENGE];
			readLength = Serial.readBytesUntil('e', rBuffer, LESELAENGE);
			char commandChar = rBuffer[0];
			switch (commandChar)
			{
			case '*':
				Serial.println(NAME);
				break;

				//Return Dose and reset
			case '?':
				Serial.println(Dose);
				break;
				//return Intensity
			case '!':
				Serial.println(Intensity);
				break;
			case 'x':
				Serial.println('k');
				Dose = 0;
				break;

			default:
				Serial.println(
					"Hallo!\n"
					"Ich bin ein TSL235R Ausleser\n"
					"Befehle:\n"
					"Query Dose: '?'\n"
					"Query Intensity: '!'\n"
					"Reset Dose: 'x'\n"


					);
			}

		}

	}

	//Die Intensität wird über eine Millisekunde gemittel(Frequenzbestimmung. Daher ergibt sich eine Mindestfrequenz am TSL235Ausgang von 1kHz. Alles darunter ist Mist...
	_now = millis();
	if (_now - _oldnow > INTEGRATION_TIME)
	{
		_oldnow = _now;
		unsigned long int frequency = cnt / INTEGRATION_TIME;// mW/cm^2 *1000;
		cnt = 0;
		Intensity = frequency * 1.584;	//Correction for Zero-Point-Offset(estimated from datasheet and taken from Original Sketch), (10^0.2), logscale adapted
		Intensity = Intensity / 0.55;	//Correction for Lambda=405nm Responsitivity
		Intensity = Intensity * 1.5;	//Corection Factor against Newport Measurement
		Dose = Dose + (Intensity * INTEGRATION_TIME/1000);
	}


}
// END OF FILE


ISR(INT0_vect){
	cnt++;
}





/*Original Sketch
//*    FILE: demo01.pde
//*  AUTHOR: Rob Tillaart
//*    DATE: 2011 05 16
//*
//* PURPOSE: prototype TSL235R monitoring
//*
//* Digital Pin layout ARDUINO
//* =============================
//*  2     IRQ 0    - to TSL235R
//*
//* PIN 1 - GND
//* PIN 2 - VDD - 5V
//* PIN 3 - SIGNAL
//*
//*/
//
//volatile unsigned long cnt = 0;
//unsigned long oldcnt = 0;
//unsigned long t = 0;
//unsigned long last;
//
//void irq1()
//{
//	cnt++;
//}
//
/////////////////////////////////////////////////////////////////////
////
//// SETUP
////
//void setup()
//{
//	Serial.begin(115200);
//	Serial.println("START");
//	pinMode(2, INPUT);
//	digitalWrite(2, HIGH);
//	attachInterrupt(0, irq1, RISING);
//}
//
/////////////////////////////////////////////////////////////////////
////
//// MAIN LOOP
////
//void loop()
//{
//	if (millis() - last >= 1000)
//	{
//		last = millis();
//		t = cnt;
//		unsigned long hz = t - oldcnt;
//		Serial.print("FREQ: ");
//		Serial.print(hz);
//		Serial.print("\t = ");
//		Serial.print((hz + 50) / 100);  // +50 == rounding last digit
//		Serial.println(" mW/m2");
//		oldcnt = t;
//	}
//}
//// END OF FILE