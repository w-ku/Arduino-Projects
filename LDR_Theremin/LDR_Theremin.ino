/*
    Author:
    Wojciech Kuprianowicz

    Application name:
    LDR Theremin

    Current Version:
    1.0 (2013-01-23)

    Description:
    Sketch for a simple Theremin-like instrument. Pitch is controlled by an LDR module, whereas volume is controlled by
    a potentiometer. Features include: settable min and max LDR values, fluent base frequency changing, recording and
    playing short sound sequences.

    Used equipment:
    - Arduino UNO R3
    - TinkerKit
    - TinkerKit LDR module (for pitch control)
    - TinkerKit Rotary Potentiometer module (for base frequency change)
    - TinkerKit Linear Potentiometer module (for volume control, connected to pin 8, ground and directly to the speaker)
    - 2 TinkerKit Touch Sensor modules (option selection and confirmation buttons)
    - 2 TinkerKit LED modules (to show which option is currently selected)
    - 8-ohm speaker (connected to ground and linear potentiometer's output pin)
*/

#include <TinkerKit.h>
#include "pitches.h"

#define DEBUG


int speaker = 8;

TKLightSensor ldr(I0);
int ldrMin, ldrMax;

TKPotentiometer pot(I1);
#define POT_MIN 2
#define POT_MAX 1019

TKTouchSensor optionButton(I2), enterButton(I3);
TKLed opLed0(O0), opLed1(O1);
int currentOption;  // 0 = set ldrMin ;  1 = set ldrMax ;  2 = play the recording ;  3 = record
boolean currentlyPlaying, currentlyRecording;

#define MAX_RECORDING_SIZE 500
int recording[MAX_RECORDING_SIZE];
int recPtr, recordingSize;

#define FREQ_MIN 31
#define FREQ_MAX 4978
#define RANGE_IN_OCTAVES 4
int freqBase, freqRange;


void changeFreqBase() {
    int potValue = pot.get();
    freqBase = map(potValue, POT_MIN, POT_MAX, FREQ_MIN, FREQ_MAX >> RANGE_IN_OCTAVES);
    freqRange = (freqBase << RANGE_IN_OCTAVES) - freqBase;
}


int setLdr() {
    int sum = 0;
    for(int i=0; i<5; i++) {
        sum += ldr.get();
        delay(10);
    }
    #ifdef DEBUG
        Serial.print("New ldr value = ");
        Serial.println(sum / 5);
    #endif
    return (sum / 5);
}


void switchOption() {
    currentOption = (currentOption + 1) % 4;
    opLed0.set((currentOption & 1 ? HIGH : LOW));
    opLed1.set((currentOption > 1 ? HIGH : LOW));
    #ifdef DEBUG
        Serial.print("Selected option = ");
        Serial.println(currentOption);
    #endif
}


int getCurrentFreq() {
    int ldrVal = ldr.get();
    if(ldrMin <= ldrMax) {
        if(ldrVal < ldrMin) ldrVal = ldrMin;
        if(ldrVal > ldrMax) ldrVal = ldrMax;
    }
    else {
        if(ldrVal < ldrMax) ldrVal = ldrMax;
        if(ldrVal > ldrMin) ldrVal = ldrMin;
    }
    return map(ldrVal, ldrMin, ldrMax, freqBase, freqBase + freqRange);
}


#ifdef DEBUG
    void westminsterQuarters() {
        int westm[] = { NOTE_E4, NOTE_GS4, NOTE_FS4, NOTE_B3,
                        NOTE_E4, NOTE_FS4, NOTE_GS4, NOTE_E4,
                        NOTE_GS4, NOTE_E4, NOTE_FS4, NOTE_B3,
                        NOTE_B3, NOTE_FS4, NOTE_GS4, NOTE_E4 };
                        
        for(int i=0; i<16; i++) {
            tone(speaker, westm[i], 800);
            delay(400);
            if(i % 4 == 3) delay(500);
            noTone(speaker);
        }
    }
#endif


void setup() {
    #ifdef DEBUG
        Serial.begin(9600);
        westminsterQuarters();
    #endif
    
    freqBase = 31;
    freqRange = (freqBase << RANGE_IN_OCTAVES) - freqBase;
    ldrMin = 0;
    ldrMax = 1023;
    currentOption = 0;
    opLed0.off();
    opLed1.off();
    currentlyPlaying = false;
    currentlyRecording = false;
    recPtr = 0;
    recordingSize = 0;
}


void loop() {
  
    if(optionButton.get() == HIGH && !(currentlyPlaying || currentlyRecording)) {
        noTone(speaker);
        switchOption();
        delay(500);
    }
    
    if(enterButton.get() == HIGH) {
        noTone(speaker);
        switch(currentOption) {
            case 0:
                ldrMin = setLdr();
                break;
            case 1:
                ldrMax = setLdr();
                break;
            case 2:
                recPtr = 0;
                currentlyPlaying ^= 1;
                break;
            case 3:
                if(currentlyRecording) recordingSize = recPtr;
                recPtr = 0;
                currentlyRecording ^= 1;
                break;
        }
        delay(500);
    }
    
    if(currentlyPlaying) {
        noTone(speaker);
        if(recPtr == recordingSize) {
            recPtr = 0;
            currentlyPlaying = false;
            delay(500);
        }
        else {
            tone(speaker, recording[recPtr]);
            recPtr++;
            delay(50);
        }
        return;
    }
    
    changeFreqBase();
    int curFreq = getCurrentFreq();
    noTone(speaker);
    tone(speaker, curFreq);
    #ifdef DEBUG
        Serial.print("BASE_FREQ = ");
        Serial.print(freqBase);
        Serial.print("; FREQ_RANGE = ");
        Serial.print(freqRange);
        Serial.print("; FREQ = ");
        Serial.println(curFreq);
    #endif
    
    if(currentlyRecording) {
        if(recPtr == MAX_RECORDING_SIZE) {
            recordingSize = recPtr;
            noTone(speaker);
            for(int i=0; i<3; i++) {
                opLed0.off(); opLed1.off(); delay(300);
                opLed0.on(); opLed1.on(); delay(300);
            }
            currentlyRecording = false;
        }
        else recording[recPtr++] = curFreq;       
    }
    
    delay(50);
}

