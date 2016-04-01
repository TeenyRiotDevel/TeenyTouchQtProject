/*
Library zur Verwendung der Analogeingänge für Touchsensoren
Diese Librarie basiert auf dem ATMEL QTouch-Prinzip und dem Basis Code von
J.Geisler -> https://github.com/jgeisler0303/QTouchADCArduino

Hardware:
Kupferfläche oder sonstige leitfähige Fläche über 1kOhm Widerstand an AnalogIn 

Anian Bühler 01.09.2014
*/
#include "Arduino.h"


#ifndef _LGIQTOUCH_
#define _LGIQTOUCH_

#define ADMUX_MASK 0b00001111 // mask the mux bits in the ADMUX register
#define MUX_GND 0b00001111 // mux value for connecting the ADC unit internally to GND
#define CHARGE_DELAY 5 // time it takes for the capacitor to get charged/discharged in microseconds
#define TRANSFER_DELAY 5 // time it takes for the capacitors to exchange charge

	 // IF Attiny 8Pin is used
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)         
	#define ADC_PORT  PORTB     //USE PORT B
	#define ADC_DIR   DDRB	      //
	#define ADMUX_SETUP 0b10010000// Set Vref to 2,56V with ext Cap
	#define MUX_REF_VCC 0b10010000 // value to set the ADC reference to Vcc
	#define ADC_SETUP   0b10000100//0b11000011  // Set ADC pre-scaler to 8
#else // IF Standard Arduino Boards are used
	#define ADC_PORT  PORTC       //USE PORT C
	#define ADC_DIR   DDRC	      // 
	#define ADMUX_SETUP 0b01000000// Set Vref to AVcc with ext Cap 
	#define MUX_REF_VCC 0b01000000 // value to set the ADC reference to Vcc
	#define ADC_SETUP 0b11000100  // Set ADC pre-scaler to 16
#endif;

class TeenyTouch
{
	private:
		uint8_t _TouchPin,		
				_PartnerPin;	
				
	const uint8_t _tiny8AnalogToMux[10] = {
				0, //D0
				0, //D1
				1, //D2 Ain1
				3, //D3 Ain3
				2, //D4 Ain2
				0, //RESET D5 Ain0
				0, //A0->Ain0(Reset)
				1, //A1->Ain1 
				2, //A2->Ain2
				3, //A3->Ain3
			};			
	

	public:


	
	//Konstruktoren
	//******************************************************************
    TeenyTouch();
	
	//setup
	//******************************************************************
    void begin(uint8_t TouchPin1, uint8_t PartnerPin);
	
	//getter
	//******************************************************************
	int getRawValue(uint8_t iterMeasure);
	uint16_t touch_probe(uint8_t pin, uint8_t partner, bool dir);
};


class TeenyTouchButton
{
private:
	
    TeenyTouch 	_QTouch;
	uint8_t _hysteresis;
	
	int		_Offset;


	

public:
	//Konstruktoren
	//******************************************************************
    TeenyTouchButton(uint8_t TouchPin1, uint8_t PartnerPin);
	
	//setup
	//******************************************************************
	void init();
	
	//setter
	//******************************************************************
	void setHysteresis(uint8_t hysteresis);
	void setOffset(uint8_t Offset);
	
	//getter
	//******************************************************************
	uint8_t getHysteresis();
	int getOffset();
	bool isTouched();
	int getRawTouch();	
};


class TeenyTouchSlider
{
private:
	
    TeenyTouch 	_QTouch1, _QTouch2, _QTouch3;
	uint8_t _hysteresis, _threshold,
			_maxVal1, _maxVal2, _maxVal3,
			_lastSliderPos;
	
	int		_Offset1, _Offset2, _Offset3;


	

public:
	//Konstruktoren
	//******************************************************************
    TeenyTouchSlider(uint8_t TouchPin1, uint8_t TouchPin2, uint8_t TouchPin3);
	
	//setup
	//******************************************************************
	void init();
	
	//setter
	//******************************************************************
	void setHysteresis(uint8_t hysteresis);
	void setOffset(uint8_t Offset1, uint8_t Offset2, uint8_t Offset3);
	void setThreshold(uint8_t threshold);
	
	//getter
	//******************************************************************
	uint8_t getHysteresis();
	uint8_t getThreshold();
	int getOffset(uint8_t num);
	int getRawTouch(uint8_t field);	
	uint8_t getTouchPosition();
};

#endif;
