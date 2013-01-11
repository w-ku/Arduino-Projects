Author: Wojciech Kuprianowicz, basing on Brett Hagman's Tone Library    

License (sort of): The source code can be used freely by anyone and for any purpose. I do not take any responsibility for any damage caused through use of this software.

Application name: DTMF Emulator

Current Version: 1.0 (2013-01-11)

Description: A short Arduino sketch emulating DTMF tones. Uses Arduino Tone Library by Brett Hagman (http://code.google.com/p/rogue-code/wiki/ToneLibraryDocumentation). Source code based on DTMFTest.pde (http://code.google.com/p/arduino-tone/source/browse/trunk/examples/DTMFTest/DTMFTest.pde).

Used equipment:
  - Arduino UNO R3
  - 8-ohm speaker (connected to digital pins FREQ1_PIN and FREQ2_PIN with 100-ohm resistors)

Issues:
  - possible compilation errors of the Tone Library. Change <wiring.h> to <Arduino.h> in the "Tone.cpp" source file (solution found on http://arduino.cc/forum/index.php/topic,87398.0.html).
