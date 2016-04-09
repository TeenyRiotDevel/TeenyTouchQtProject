#define USE_MIDI

#ifdef USE_MIDI
#include <TeenyMidi.h>
#else
#include <TeenySerial.h>
#endif

//#include <TeenyTouch.h>

#include "TeenyTouchDusjagr.h"
#include "AnalogTouch.h"

MIDIMessage midimsg;
//TeenyTouch test;

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

int i = -1023;
int value = 0;
int prevValue = 0;
int velocityValue = 0;

uint8_t samples;
uint8_t midi_delay;
uint8_t sense_mode;
uint8_t send_mode;

uint16_t timer = 0;

uint8_t note_off = 0;



int muliplexAnalogRead (const uint8_t _pin_index)
{
    // set switch to output (not sure why, but must be set everytime..)
    pinMode(PB2, OUTPUT);
    pinMode(PB1, OUTPUT);
    pinMode(PB0, OUTPUT);

    // set multiplexer, select channel
    digitalWrite(PB2, (_pin_index & 0x01));
    digitalWrite(PB1, ((_pin_index>>1) & 0x01));
    digitalWrite(PB0, ((_pin_index>>2) & 0x01));

    // read value
    return analogRead(2);
}

void delay(int * milli)
{
    TeenyMidi.delay(*milli);
}

void setup()
{

#ifdef USE_MIDI
    TeenyMidi.init();
#else
    TeenySerial.begin();
#endif

    TeenyTouchDusjagr.begin();

    TeenyTouchDusjagr.setAdcSpeed(3);
    TeenyTouchDusjagr.setDelayCb(&delay);

    TeenyTouchDusjagr.delay = 35;
    midi_delay = 5;
    sense_mode = 1;
    send_mode = 0;
    samples = 100;

    pinMode(PB0, OUTPUT);

}


void loop()
{

    switch (sense_mode) {
    case 1:
        value = TeenyTouchDusjagr.sense(PB4,PB2, samples);
        break;
    case 2:
        value = TeenyTouchDusjagr.touchPin(PB4,samples);
        break;
    case 3:
        value = TeenyTouchDusjagr.sense(PB4,PB3, samples);
        break;
    case 4:
        value = analogRead(3);
        break;
    case 5:
        i++;
        if (i> 1023) i = -1023;
        value = i;
        break;
    }

    velocityValue = value-prevValue;
    prevValue = value;

#ifdef USE_MIDI

   if (TeenyMidi.read(&midimsg)) {                                    // need to put the ampersand "&" before "message"

       if (midimsg.key == 1) // touch sample
       {
           samples = midimsg.value;
           //TeenyMidi.send(midimsg.command,midimsg.key,midimsg.value);
       }
       if (midimsg.key == 2) //touch ADCSRA
       {
           TeenyTouchDusjagr.setAdcSpeed(midimsg.value);
       }

       if (midimsg.key == 3) //touch delay
       {
           TeenyTouchDusjagr.delay = midimsg.value;
       }

       if (midimsg.key == 4) //midi delay
       {
           TeenyTouchDusjagr.delay = midimsg.value;
       }

       if (midimsg.key == 5) //midi delay
       {
           sense_mode = midimsg.value;
       }

       if (midimsg.key == 6) //midi delay
       {
           analogWrite(PB0, midimsg.value);
       }

       if (midimsg.key == 7) //midi delay
       {
           send_mode = midimsg.value;
       }

   }


   if (send_mode == 1)
   {
        TeenyMidi.sendCCHires(velocityValue, 1);
    }else{
        TeenyMidi.sendCCHires(value, 1);
   }

    TeenyMidi.delay(midi_delay); // give some time for sending, otherwhise the MIDI queue could fill up


//    if (velocityValue > 50)
//        {
//            if (note_off == 1)
//                {
//                    TeenyMidi.send(MIDI_NOTEON,67, velocityValue/2 );
//                    note_off = 0;
//                }
//        }
//    else
//        {
//            if (note_off == 0)
//                {
//                    TeenyMidi.send(MIDI_NOTEOFF,67,127);
//                    note_off = 1;
//                }
//        }

#else
    TeenySerial.println(velocityValue); //wrap your strings in F() to save ram!
    TeenySerial.delay(1);
#endif


}
