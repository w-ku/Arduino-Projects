/*    
    Author:
    Wojciech Kuprianowicz

    License (sort of):
    The source code can be used freely by anyone and for any purpose. I do not take any responsibility for any damage caused through use of this software.

    Application name:
    Christmas Music Box
    
    Version:
    1.0 (2013-01-04)

    Description:
    A simple Arduino program that mimicks a Christmas card / music box with switchable melody and light sequences.

    Used equipment:
    - Arduino UNO R3
    - TinkerKit
    - 3 TinkerKit LED modules
    - 2 TinkerKit Touch Sensor modules (instead of Buttons)
    - 8-ohm speaker (connected to digital pin 8 with 100-ohm resistor)
    
    TO-DO List:
    - Either fix tempo arrays or implement a better tempo setting method, because the present tempo is unsatisfactory
    - Add the possibility of inserting pauses between notes
    - Add more melodies and light sequences
*/


#include "TinkerKit.h"
#include "pitches.h"
 
 
TKTouchSensor melodySwitch(I0), ledsSwitch(I1);
boolean melodySwitchPressed, ledsSwitchPressed;
unsigned long melodySwitchCooldown, ledsSwitchCooldown; 

TKLed leds[] = { O0, O1, O2 };
int ledsNumber = 3;
 
int speaker = 8;


unsigned long time;

const int *notes, *notesDuration;
unsigned long nextMelodyEventTime;
int currentMelody, melodiesNumber = 5;
boolean soundIsBeingPlayed;
int currentNote, notesNumber;
  
  
const int closeEncNotes[] = { NOTE_G4, NOTE_A4, NOTE_F4, NOTE_F3, NOTE_C4 };
const int closeEncTempo[] = { 500, 500, 500, 500, 1000 };


const int auldLangSyneNotes[] = { NOTE_G3, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_D4,
                                  NOTE_C4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_E4, NOTE_C4,
                                  NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_A3, NOTE_A3, NOTE_G3, NOTE_C4 };
const int auldLangSyneTempo[] = { 500, 450, 250, 450, 450, 500, 250, 450, 250, 250,
                                  450, 250, 450, 500, 600, 500, 450, 250, 450, 500,
                                  450, 250, 450, 350, 300, 450, 350, 450, 400, 600 };

const int somwhInMyMemNotes[] = { NOTE_G4, NOTE_E4, NOTE_G4, NOTE_E4, NOTE_C5, NOTE_G4, NOTE_D4, NOTE_A4, NOTE_G4, NOTE_C4, NOTE_F4, NOTE_E4, NOTE_D4, };
const int somwhInMyMemTempo[] = { 500, 500, 500, 500, 1000, 800, 500, 500, 500, 400, 350, 700, 800 };

const int oHolyNightNotes[] = { NOTE_C5, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_B4, NOTE_D5, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_C5,
                                NOTE_C5, NOTE_C5, NOTE_E5, NOTE_G4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_G4, NOTE_A4,
                                NOTE_G4, NOTE_G4}; 
const int oHolyNightTempo[] = { 800, 500, 500, 800, 400, 800, 300, 400, 400, 700,
                                800, 500, 600, 600, 500, 500, 500, 500, 500, 800,
                                500, 1000 }; 




unsigned long nextLedsEventTime;
int currentLedsSetting, ledsSettingsNumber = 5;



void switchMelody(int no) {
    melodySwitchPressed = false;
    currentMelody = no;
    currentNote = 0;
    
    switch(currentMelody) {
        
        case 0:
            notesNumber = 0;
            break;
            
        case 1:
            notesNumber = 5;
            notes = closeEncNotes;
            notesDuration = closeEncTempo;
            break;
            
        case 2:
            notesNumber = 30;
            notes = auldLangSyneNotes;
            notesDuration = auldLangSyneTempo;
            break;
      
        case 3:
            notesNumber = 13;
            notes = somwhInMyMemNotes;
            notesDuration = somwhInMyMemTempo;
            break;
      
        case 4:
            notesNumber = 22;
            notes = oHolyNightNotes;
            notesDuration = oHolyNightTempo;
            break;      
    }
    nextMelodyEventTime = millis() + 1000;
}



// ledsMultiTaskVariables (lmtv)
int lmtv1, lmtv2;

void switchLedsSetting(int no) {
  
    ledsSwitchPressed = false;
    currentLedsSetting = no;
  
    switch(currentLedsSetting) {
      
        case 0:
            for(int i=0; i<ledsNumber; i++) leds[i].off();
            nextLedsEventTime = millis() + 1000;
            break;
     
        case 1:
            leds[0].off();
            for(int i=1; i<ledsNumber; i++) leds[i].on();
            nextLedsEventTime = millis() + 1000;
            break;
            
        case 2:
            lmtv1 = 0;  // 0 - even leds are on; 1 - odd leds are on
            for(int i=0; i<ledsNumber; i++)
                if(!(i & 1)) leds[i].on();
                else leds[i].off();
            nextLedsEventTime = millis() + 1000;
            break;
            
        case 3:
            for(int i=0; i<ledsNumber; i++)
                if(random(0, 2)) leds[i].on();
                else leds[i].off();
            nextLedsEventTime = millis() + 1000;
            break;
            
        case 4:
            for(int i=0; i<ledsNumber; i++) leds[i].on();
            nextLedsEventTime = millis() + 1000;
            break;
    }
}
 
  
  
void speakerAndLedsTest() {
  
    for(int i=0; i<ledsNumber; i++) leds[i].on();
    
    for(int i=0; i<5; i++) {
        tone(speaker, closeEncNotes[i], closeEncTempo[i]);
        delay((int)(closeEncTempo[i] * 1.3));
        noTone(speaker);
    }
    
    for(int i=0; i<ledsNumber; i++) leds[i].off();
}
  
  
  
void setup() {
    switchMelody(4);
    switchLedsSetting(1);
    melodySwitchCooldown = 0;
    ledsSwitchCooldown = 0;
    speakerAndLedsTest();
}



void loop() {
    
    time = millis();
    
    if(melodySwitch.get() == HIGH && time >= melodySwitchCooldown) {
        Serial.println("melodySwitch");
        melodySwitchCooldown = millis() + 1000;
        melodySwitchPressed = true;
    }
    
    if(ledsSwitch.get() == HIGH && time >= ledsSwitchCooldown) {
        Serial.println("ledSwitch");
        ledsSwitchCooldown = millis() + 1000;
        ledsSwitchPressed = true;
    }


    if(time >= nextLedsEventTime && !soundIsBeingPlayed) {
      
        if(ledsSwitchPressed) switchLedsSetting((currentLedsSetting + 1 ) % ledsSettingsNumber);
        
        switch(currentLedsSetting) {
        
            case 0:
                nextLedsEventTime = millis() + 1000;
                break;
          
            case 1:
                for(int i=0; i<ledsNumber; i++) 
                    if(leds[i].state() == LOW) {
                        leds[i].on();
                        leds[(i + 1) % ledsNumber].off();
                        break;
                    }
                 nextLedsEventTime = millis() + 1000;
                 break;
                 
            case 2:      
                for(int i=0; i<ledsNumber; i++)
                    if((i & 1) == lmtv1) leds[i].on();
                    else leds[i].off();    
                lmtv1 ^= 1;
                nextLedsEventTime = millis() + 1000;
                break;
                
            case 3:
                for(int i=0; i<ledsNumber; i++)
                    if(random(0, 2)) leds[i].on();
                    else leds[i].off();
                nextLedsEventTime = millis() + 300;
                break;
                
            case 4:
                nextLedsEventTime = millis() + 1000;
                break;    
        }        
    }    
    
    if(time >= nextMelodyEventTime) {
      
        if(notesNumber == 0) {
            if(melodySwitchPressed) switchMelody(1);
            else nextMelodyEventTime = millis() + 1000;
        }
        else {
            if(soundIsBeingPlayed) {
                noTone(speaker);
                soundIsBeingPlayed = false;
                
                if(melodySwitchPressed) switchMelody((currentMelody + 1) % melodiesNumber);
                else {
                    nextMelodyEventTime = millis() + notesDuration[currentNote] * 0.30;
                    currentNote = (currentNote + 1) % notesNumber;
                }
            }
            else {
                if(melodySwitchPressed) switchMelody((currentMelody + 1) % melodiesNumber);
                else {
                    tone(speaker, notes[currentNote], notesDuration[currentNote]);
                    soundIsBeingPlayed = true;
                    nextMelodyEventTime = millis() + notesDuration[currentNote];        
                }
            }
        }      
    }
}

