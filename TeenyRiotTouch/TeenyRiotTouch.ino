#include "ArduinoMain.h"

//#include <TeenyMouse.h>
#include <TeenySerial.h>

#include "TeenyTouchDusjagr.h"
#include "TeenyTouch.h"

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

void setup()
{
    test.begin(PB4,PB2);
    //TeenyTouchDusjagr.begin();
    //TeenyTouchDusjagr.begin();
    TeenySerial.begin();
    // TeenyMouse.begin(); //start or reenumerate USB - BREAKING CHANGE from old versions that didn't require this
}

void loop()
{
    TeenySerial.println(test.getRawValue(10));
    //TeenySerial.println(TeenyTouchDusjagr.sense(PB4,PB2, 10));
    TeenySerial.delay(1);
}
