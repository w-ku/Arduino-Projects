/*
	Author:
	Wojciech Kuprianowicz, basing on Brett Hagman's Tone Library    

	License (sort of):
	The source code can be used freely by anyone and for any purpose. I do not take any responsibility for any damage caused through use of this software.

	Application name:
	DTMF Emulator

	Current Version:
	1.0 (2013-01-11)

	Description:
	A short Arduino sketch emulating DTMF tones. Uses Arduino Tone Library by Brett Hagman (http://code.google.com/p/rogue-code/wiki/ToneLibraryDocumentation). Source code based on DTMFTest.pde (http://code.google.com/p/arduino-tone/source/browse/trunk/examples/DTMFTest/DTMFTest.pde).

	Used equipment:
	- Arduino UNO R3
	- 8-ohm speaker (connected to digital pins FREQ1_PIN and FREQ2_PIN with 100-ohm resistors)

	Issues:
	- possible compilation errors of the Tone Library. Change <wiring.h> to <Arduino.h> in the "Tone.cpp" source file (solution found on http://arduino.cc/forum/index.php/topic,87398.0.html).
*/


#include <Tone.h>

#define FREQ1_PIN 8
#define FREQ2_PIN 10

#define DELAY_TIME 800
#define TONE_DURATION 600

/*
DTMF Table:
    Hz\Hz  1209  1336  1477  1633
    697    1     2     3     A
    770    4     5     6     B
    852    7     8     9     C
    941    *     0     #     D
*/
const int DTMF[][2] = { /*0*/{941, 1336}, /*1*/{697, 1209}, /*2*/{697, 1336}, /*3*/{697, 1477}, /*4*/{770, 1209},
                        /*5*/{770, 1336}, /*6*/{770, 1477}, /*7*/{852, 1209}, /*8*/{852, 1336}, /*9*/{852, 1447}, 
                        /*A*/{697, 1633}, /*B*/{770, 1633}, /*C*/{852, 1633}, /*D*/{941, 1633},
                        /***/{941, 1209}, /*#*/{941, 1447} };
                        
int inputKey;
Tone freq1, freq2;


int getTableIndex(int key) {
    if(key >= '0' && key <= '9') return (key - '0');
    if(key >= 'A' && key <= 'D') return (10 + key - 'A');
    if(key == '*') return 14;
    if(key == '#') return 15;
    return -1;  // invalid key
}


void setup() {
    Serial.begin(9600);
    freq1.begin(FREQ1_PIN);
    freq2.begin(FREQ2_PIN);
}


void loop() {
  
  if(Serial.available()) {
      inputKey = Serial.read();
      inputKey = getTableIndex(inputKey);
      
      if(inputKey == -1) {
          Serial.println("Invalid key given.");
          return;
      }
      Serial.println(inputKey);
      freq1.play(DTMF[inputKey][0], TONE_DURATION);
      freq2.play(DTMF[inputKey][1], TONE_DURATION);
      delay(DELAY_TIME);
  }
}

