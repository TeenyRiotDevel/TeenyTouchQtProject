/*
  QTouchSense.h - Library for Capacitive touch sensors using only one ADC PIN and a Reference
  modified from https://github.com/jgeisler0303/QTouchADCArduino
  Released into the public domain.
*/

#ifndef QTouchADCTiny_h
#define QTouchADCTiny_h

#include <Arduino.h>
//#include "TeenyMidi.h"

class TeenyTouchDusjagrClass
{
  public:
    TeenyTouchDusjagrClass();
    void begin();
    uint16_t sense(byte adcPin, byte refPin, uint8_t samples);
    uint16_t touch(uint8_t pin, uint8_t partner, uint8_t samples);
    uint16_t touch_probe(uint8_t pin, uint8_t partner, bool dir);
    uint16_t touchPin(uint8_t adcPin, uint8_t samples);
    void setAdcSpeed(uint8_t mode);
    void setDelayCb(void (*cb_func)(unsigned int *));
    void delayUs(unsigned int milli);
    uint8_t delay;
private:
    void (*delay_cb)(unsigned int* text);
};

extern TeenyTouchDusjagrClass TeenyTouchDusjagr;

#endif

