/*
Library zur Verwendung der Analogeingänge für Touchsensoren
Diese Librarie basiert auf dem ATMEL QTouch-Prinzip und dem Basis Code von
J.Geisler -> https://github.com/jgeisler0303/QTouchADCArduino

Hardware:
Kupferfläche oder sonstige leitfähige Fläche über 1-10kOhm Widerstand an AnalogIn 
Attiny 25/45/85 AIN1-3

Anian Bühler 09.01.2015
AB	27.05.2015
*/


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

#include "TeenyTouch.h"

 
//QTouch
//******************************************************************
//******************************************************************

//Konstruktor
//******************************************************************
TeenyTouch :: TeenyTouch(){}

//setup
//******************************************************************
void TeenyTouch :: begin(uint8_t TouchPin, uint8_t PartnerPin) { //Abfrage bei mehereren Objekten über ifndef _TOUCHINIT_


	
	this -> _TouchPin   = TouchPin;
	this -> _PartnerPin = PartnerPin;
	
	#ifndef _TOUCHINIT_
	#define _TOUCHINIT_
	    // prepare the ADC unit for one-shot measurements
		// see the atmega328 datasheet for explanations of the registers and values
		ADMUX = ADMUX_SETUP; // Vcc as voltage reference (bits76), right adjustment (bit5), use ADC0 as input (bits3210)
        ADCSRA = 0b11000100; // enable ADC (bit7), initialize ADC (bit6), no autotrigger (bit5), don't clear int-flag (bit4), no interrupt (bit3), clock div by 16@16Mhz=1MHz (bit210) ADC should run at 50kHz to 200kHz, 1MHz gives decreased resolution
        ADCSRB = 0b00000000; // autotrigger source free running (bits210) doesn't apply
		while(ADCSRA & (1<<ADSC)){} // wait for first conversion to complete 
  #endif;
}

//getter
//******************************************************************
int TeenyTouch :: getRawValue(uint8_t iterMeasure){
	int adc1 = 0;
	int adc2 = 0;
	
	for(int i = 1; i <= iterMeasure; i++){
		adc1 += touch_probe(this-> _TouchPin, this-> _PartnerPin, true);
		adc2 += touch_probe(this-> _TouchPin, this-> _PartnerPin, false);
	}

	adc1 /= iterMeasure;
	adc2 /= iterMeasure;
	
	return (adc2-adc1);
}

uint16_t TeenyTouch :: touch_probe(uint8_t pin, uint8_t partner, bool dir) {
      uint8_t MUXPin;
	  uint8_t MUXPartner;
	  
	  	 // IF Attiny 8Pin is used --> ADC1 (PB2), ADC2(PB4), ADC3 (PB3)
	#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)			
		MUXPin     = _tiny8AnalogToMux[pin];
		MUXPartner = _tiny8AnalogToMux[partner];
		pin = (pin < 6 ? pin : pin - 6);//analogPinToChannel(pin);
		partner = (partner < 6 ? partner : partner - 6);

	#else // IF Standard Arduino Boards are used
		//if const A0-A5 is used
		pin = (pin < 14 ? pin : pin - 14);//analogPinToChannel(pin);
		partner = (partner < 14 ? partner : partner - 14);
		MUXPin = pin;
		MUXPartner = partner;
	#endif;

	  uint8_t mask= _BV(pin) | _BV(partner);
	  
	  ADC_DIR|= mask; // config pins as push-pull output
	  if(dir)
		ADC_PORT= (ADC_PORT & ~_BV(pin)) | _BV(partner); // set partner high to charge the s&h cap and pin low to discharge touch probe cap
	  else
		ADC_PORT= (ADC_PORT & ~_BV(partner)) | _BV(pin); // set pin high to charge the touch probe and pin low to discharge s&h cap cap
	  // charge/discharge s&h cap by connecting it to partner
	  ADMUX = MUX_REF_VCC | MUXPartner; // select partner as input to the ADC unit
	  delayMicroseconds(CHARGE_DELAY); // wait for the touch probe and the s&h cap to be fully charged/dsicharged
	  ADC_DIR&= ~mask; // config pins as input
	  ADC_PORT&= ~mask; // disable the internal pullup to make the ports high Z
	  // connect touch probe cap to s&h cap to transfer the charge
	  ADMUX= MUX_REF_VCC | MUXPin; // select pin as ADC input
	  delayMicroseconds(TRANSFER_DELAY); // wait for charge to be transfered
	  // measure
	  ADCSRA|= (1<<ADSC); // start measurement
	  while(ADCSRA & (1<<ADSC)){ 
	  } // wait for conversion to complete

	  return ADC; // return conversion result
}


//QTouch BUTTON
//******************************************************************
//******************************************************************


//Konstruktor
//******************************************************************
TeenyTouchButton :: TeenyTouchButton(uint8_t TouchPin, uint8_t PartnerPin):_QTouch(){

    this->_QTouch.begin(TouchPin, PartnerPin);

	this-> _Offset		= 0;
	this-> _hysteresis = 30;
	
}


//setup
//******************************************************************
void TeenyTouchButton :: init(){ //Abfrage bei mehereren Objekten über ifndef _TOUCHINIT_
  this-> _Offset = this->_QTouch.getRawValue(10);
}


//setter
//******************************************************************
void TeenyTouchButton :: setHysteresis(uint8_t hysteresis){
	this-> _hysteresis = hysteresis;
	}

void TeenyTouchButton :: setOffset(uint8_t Offset){
	this-> _Offset = Offset;
	}

	
//getter
//******************************************************************
uint8_t TeenyTouchButton :: getHysteresis(){
	return this-> _hysteresis;
	}

int TeenyTouchButton :: getOffset(){
	return this-> _Offset;
	}

bool TeenyTouchButton :: isTouched(){
	return ((this->_QTouch.getRawValue(4) - this-> _Offset) >  this-> _hysteresis);
}

int TeenyTouchButton :: getRawTouch(){
	return this->_QTouch.getRawValue(4);
}


//QTouch SLIDER
//******************************************************************
//******************************************************************

//Konstruktor
//******************************************************************
TeenyTouchSlider :: TeenyTouchSlider(uint8_t TouchPin1, uint8_t TouchPin2, uint8_t TouchPin3):_QTouch1(), _QTouch2(), _QTouch3(){

    this->_QTouch1.begin(TouchPin1, TouchPin3);
    this->_QTouch2.begin(TouchPin2, TouchPin1);
    this->_QTouch3.begin(TouchPin3, TouchPin2);

	this-> _Offset1		= 0;
	this-> _Offset2		= 0;
	this-> _Offset3		= 0;
	this-> _hysteresis  = 30;
	this-> _threshold   = 10;
	this-> _maxVal1     = 0;
	this-> _maxVal2     = 0;
	this-> _maxVal3     = 0;
    this-> _lastSliderPos = 0;
}


//setup
//******************************************************************
void TeenyTouchSlider :: init(){ //Abfrage bei mehereren Objekten über ifndef _TOUCHINIT_
	this-> _Offset1		=  this->_QTouch1.getRawValue(10);
	this-> _Offset2		=  this->_QTouch2.getRawValue(10);
	this-> _Offset3		=  this->_QTouch3.getRawValue(10);
}


//setter
//******************************************************************
void TeenyTouchSlider :: setHysteresis(uint8_t hysteresis){
	this-> _hysteresis = hysteresis;
	}

void TeenyTouchSlider :: setOffset(uint8_t Offset1, uint8_t Offset2, uint8_t Offset3){
	this-> _Offset1 = Offset1;
	this-> _Offset2 = Offset2;
	this-> _Offset3 = Offset3;
	}
	
    void TeenyTouchSlider :: setThreshold(uint8_t threshold){
	this-> _threshold = threshold;
	}

	
//getter
//******************************************************************
uint8_t TeenyTouchSlider :: getHysteresis(){
	return this-> _hysteresis;
	}
	
uint8_t TeenyTouchSlider :: getThreshold(){
	return this-> _threshold;
}

int TeenyTouchSlider :: getOffset(uint8_t num){
	switch(num){
		case 1:
			return this-> _Offset1;
			break;
		case 2:
			return this-> _Offset2;
			break;
		case 3:
			return this-> _Offset3;
			break;
	}
}



int TeenyTouchSlider :: getRawTouch(uint8_t field){
	switch(field){
		case 1:
			return this->_QTouch1.getRawValue(4);
			break;
		case 2:
			return this->_QTouch2.getRawValue(4);
			break;
		case 3:
			return this->_QTouch3.getRawValue(4);
			break;
	}
}

uint8_t TeenyTouchSlider :: getTouchPosition(){
	bool _mode = true;
	
	int raw1, raw2, raw3;
	int map1, map2, map3;
	uint8_t sliderPosition = 0;
	
	raw1 = getRawTouch(1)- getOffset(1);
    raw2 = getRawTouch(2)- getOffset(2);
    raw3 = getRawTouch(3)- getOffset(3);
	
	this-> _maxVal1 = max(this-> _maxVal1, raw1);
    this-> _maxVal2 = max(this-> _maxVal2, raw2);
    this-> _maxVal3 = max(this-> _maxVal3, raw3);
	
	map1 = constrain(map( raw1, this-> _threshold, this-> _maxVal1, 0, 100), 0, 100);
    map2 = constrain(map( raw2, this-> _threshold, this-> _maxVal2, 0, 100), 0, 100);
    map3 = constrain(map( raw3, this-> _threshold, this-> _maxVal3, 0, 100), 0, 100);
	
	//not touched
    if( map1 <= 0 && map2 <= 0 && map2 <= 0){ 
		if(_mode) 
			sliderPosition = _lastSliderPos;
		else
			sliderPosition = 0;
    }
	//upper half is touched
    else if( (map1 - map3) > 0 ){
		if( map2 > 0){
			sliderPosition = map(map2 - map1, -100, 100, 0, 50);
			_lastSliderPos = sliderPosition;
		}
		else{
			sliderPosition = 0;
			_lastSliderPos = sliderPosition;
		}
    }
	//middle is touched
	else if( map2 > 0 && map1 == 0 && map3 == 0 ){
			sliderPosition = 50;
			_lastSliderPos = sliderPosition;
	}
	//lower half is touched
    else if((map1 - map3) < 0){
		if( map2 > 0){
			sliderPosition = map(map3 - map2, -100, 100, 50, 100); 
			_lastSliderPos = sliderPosition;
		}
		else{
			sliderPosition = 100;
			_lastSliderPos = sliderPosition;
		}
    }

	return sliderPosition;
}
