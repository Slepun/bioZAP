/*
 * multiZAP_def.h
 *
 *  Created on: 26 dec 2017
 *      Author: elektros230
 */

#ifndef MULTIZAP_DEF_H_
#define MULTIZAP_DEF_H_

#define HRDW_VER "multiZAP NANO3"
#define SOFT_VER "2017-12-31"


#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <DS1803.h>
#include <AD9850.h>

#ifndef INPUTS_DEF_
#define INPUTS_DEF_
#define INPUT_SIGN '>'			//Serial and LCD input sign
#define INPUT_SIGN_KEYPAD '*'	//Serial sign if command were inserted from keypad
#define INPUT_BACK_KEYPAD '<'	//Inputed back data from keypad
#endif


#define ENCODER_PROTOTYPE //TODO: To remove - only prototype device

#ifdef ENCODER_PROTOTYPE
//#include <Encoder_Polling.h>
#define encoderPin_A 3
#define encoderPin_B 2
#endif


#define DS1803_I2C_ADDRESS 0x28

#define AD9850__CLK A1
#define AD9850__FQUP A2
#define AD9850__DATA A3

#define powerPin 4
#define buzerPin 13

#define outVoltagePin PIN_A6

#define EEPROM_WIPER0_ADDRESS 1022
#define EEPROM_WIPER1_ADDRESS 1023

#define REF_VOLTAGE 15.6383   //This voltage equals 1023 on analog input
#define ONE_GRADE 0.015287    //REF_VOLTAGE/1023 --One bit //0.015635


AD9850 ad9850(AD9850__CLK, AD9850__FQUP, AD9850__DATA);
//AD9850 AD9850(AD9850__CLK, AD9850__FQUP, AD9850__DATA, 30); //for AD9851

DS1803 ds1803(DS1803_I2C_ADDRESS);


LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display


const byte ROWS = 4; //rows
const byte COLS = 4; //columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {12, 11, 10, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6, 5}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char key;

int col =0;
int pos = 0;
int val;
long Freq = 1000000; //100kHz

//boolean startNormaly = false;

//              1kHz    100kHz    500kHz    900kHz    10Hz
long freqs[] = {100000, 10000000, 50000000, 90000000, 1000};
byte currentFreq = 0;

// Function prototypes
long inputVal (String dialogText,long defaultVal = -1);


#endif /* MULTIZAP_DEF_H_ */
