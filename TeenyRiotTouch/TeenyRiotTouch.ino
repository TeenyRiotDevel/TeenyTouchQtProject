#define USE_MIDI

#ifdef USE_MIDI
#include <TeenyMidi.h>
#else
#include <TeenySerial.h>
#endif

#include <TeenyTouch.h>

#include "TeenyTouchDusjagr.h"

TeenyTouch test;

  //TeenyTouchDusjagr test2;
// ATMEL ATTINY85
//
//                   +-\/-+
//             PB5  1|    |8  VCC
//        ADC3/PB3  2|    |7  PB2/ADC1
//        ADC2/PB4  3|    |6  PB1*
//             GND  4|    |5  PB0*
//                   +----+
//
// * indicates PWM port
//

int prevValue = 0;
int velocityValue = 0;

void setup()
{
  //test.begin(PB4,PB2);
  #ifdef USE_MIDI
    TeenyMidi.init();
  #else
    TeenySerial.begin();
  #endif
  TeenyTouchDusjagr.begin();
}

#ifdef USE_MIDI
void sendCC(uint8_t ch, uint8_t value)
{
    TeenyMidi.send(MIDI_CONTROLCHANGE, ch, value);
}

void sendHiresMidi(uint16_t value)
{
    uint8_t valH = value >> 3;
    uint8_t valL = value & 0b0111;
    TeenyMidi.send(MIDI_CONTROLCHANGE, 1, valH);
    TeenyMidi.send(MIDI_CONTROLCHANGE, 2, valL);
}
#endif

void loop()
{
  int value = TeenyTouchDusjagr.sense(PB4,PB2, 100);
  velocityValue = value-prevValue;

  #ifdef USE_MIDI

  sendHiresMidi(value);
  TeenyMidi.delay(1); // give some time for sending, otherwhise the MIDI queue could fill up

  #else
    TeenySerial.println(velocityValue); //wrap your strings in F() to save ram!
    TeenySerial.delay(1);
  #endif

  prevValue = value;

}
