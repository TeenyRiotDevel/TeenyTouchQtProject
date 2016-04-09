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
    samples = 100;

    pinMode(PB0, OUTPUT);

}


void loop()
{

    //value = readTouch(2,500);
    //value = analogTouchRead(2, 10);
   // value = TeenyTouchDusjagr.touch(PB4,PB2,samples);

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
    default:
        break;
    }

    velocityValue = value-prevValue;


//    for (uint8_t i = 0; i<100; i++)
//    {
//        pinMode(PB0,OUTPUT);
//        digitalWrite(PB0, LOW);
//        digitalWrite(PB2, HIGH);
//        delay(1);
//        digitalWrite(PB2, LOW);
//        pinMode(PB0,INPUT);

//        value = TeenyTouchDusjagr.sense(PB2,PB4, 100);
//        //velocityValue = 10+(value-prevValue);

//        //digitalWrite(PB2, HIGH);
//        //digitalWrite(PB2, LOW);

//        delay(7);

//        sendHiresMidi(value);
//        TeenyMidi.update();

//    }

//    for (uint8_t i = 0; i<100; i++)
//    {
//        pinMode(PB0,OUTPUT);
//        digitalWrite(PB0, LOW);
//        digitalWrite(PB2, LOW);
//        delay(1);
//        digitalWrite(PB2, LOW);
//        pinMode(PB0,INPUT);

//        value = TeenyTouchDusjagr.sense(PB2,PB4, 100);
//        //velocityValue = 10+(value-prevValue);

//        //digitalWrite(PB2, HIGH);
//        //digitalWrite(PB2, LOW);

//        delay(7);

//        sendHiresMidi(value);
//        TeenyMidi.update();

//    }

#ifdef USE_MIDI


//   i++;
//   if (i> 1023) i = -1023;


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
          // sense_mode = midimsg.value;
       }

   }


   analogWrite(PB0, (velocityValue)*2);

    TeenyMidi.sendCCHires(value, 1);
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

    //TeenyMidi.delay(25); // give some time for sending, otherwhise the MIDI queue could fill up
#else
    TeenySerial.println(velocityValue); //wrap your strings in F() to save ram!
    TeenySerial.delay(1);
#endif

    prevValue = value;

}
