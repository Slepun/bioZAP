/**********************************************************
 * bioZAP_func.h
 * 		multiZAP and free-PEMF devices library.
 * 		BIOzap script therapy language.
 * 		See https://biotronika.pl
 *
 *  Created on: 2017-01-01
 *      Author: elektros230 & hehaka
 **********************************************************/

#ifndef BIOZAP_FUNC_H_
#define BIOZAP_FUNC_H_

//Target device definition
//TODO: Automate switch based on project include definition file
#define MULTIZAP
//#define FREE_PEMF

#include <Arduino.h>

#ifdef MULTIZAP
#include "multiZAP_def.h"
//#include <multiZAP_def.h>
//#include <multiZAP_lcd.h>
//#include <DS1803.h>
//#include <AD9850.h>
#endif



//BIOzap
#define WELCOME_SCR "Free BIOzap interpreter welcome! See https://biotronika.pl"
#define PROGRAM_SIZE 1000     	// Maximum program size
#define PROGRAM_BUFFER 128    	// SRAM buffer size, used for script loading  TODO lack of memory
#define MAX_CMD_PARAMS 3      	// Count of command params
#define LCD_SCREEN_LINE 1     	// LCD user line number, -1 = no lcd, 0 = first, 1= second
#define MIN_FREQ_OUT 1        	//  0.01 Hz

#ifdef FREE_PEMF
#define MAX_FREQ_OUT 5000     	// 50HzHz
#endif

#ifdef MULTIZAP
#define MAX_FREQ_OUT 90000000 	// 900kHz
#endif

#define SCAN_STEPS 100        	// For scan function purpose - default steps
#define XON 17  //0x11			//TODO: to remove
#define XOFF 19 //0x13          //TODO: to remove
#define MAX_LABELS 8          	// Number of labels in code

#ifndef INPUTS_DEF_
#define INPUTS_DEF_
#define INPUT_SIGN '>'			//Serial and LCD input sign
#define INPUT_SIGN_KEYPAD '*'	//Serial sign if command were inserted from keypad
#define INPUT_BACK_KEYPAD '<'	//Inputed back data from keypad
#endif

//Battery staff
#define batPin PIN_A7                           // Analog-in battery level
#define BATTERY_VOLTAGE_RATIO 0.153             // Include divider 10k/4,7k resistors. 5V*(10k+4,7k)/4,7k = 0,0153 (x10)
#define EEPROM_BATTERY_CALIBRATION_ADDRESS 1023 // Memory address of battery correction factor - 100 means RATIO x 1,00
#define MIN_BATTERY_LEVEL 90                    // 90 means 9.0 V  (x10), less then that turn off
#define USB_POWER_SUPPLY_LEVEL 65               // Maximum USB voltage level means 6.5V

// During charging battery minimum increasing voltage after x milliseconds.
const unsigned long checkDeltaBatteryIncreasingVoltageTime = 600000UL;

//BIOzap
String inputString = "";                // a string to hold incoming serial data
String param[MAX_CMD_PARAMS];           // param[0] = cmd name
boolean stringComplete = false;         // whether the string is complete
boolean memComplete = false;
unsigned long lastFreq = MIN_FREQ_OUT;  // Uses scan function
int minBatteryLevel = 0;
boolean Xoff = false;					//TODO: XON/XOFF to remove

#ifdef FREE_PEMF
byte pwm = 50;                          // TODO: Pulse width modulation is rectangle signal fulfillment : 0-100%, default 50
boolean outputDir = false;
byte coilState = LOW;
#endif

#ifdef MULTIZAP
byte wiper0 = 0;
byte wiper1 = 0;
#endif

//TODO: hehaka
//Labels & jumps
String labelName[MAX_LABELS];           // TODO: Labels names e.g. :MY_LABEL
unsigned int labelPointer[MAX_LABELS];  // TODO: Next line of label
unsigned int labelLoops[MAX_LABELS];    // TODO: Number of left jump loops
byte labelsPointer = 0;                 // TODO: Pointer of end label table


//Serial buffer
char memBuffer[PROGRAM_BUFFER];

const unsigned long pauseTimeOut = 600000UL;    // 600000 Time of waiting in pause state as turn power off. (60000 = 1 min.)
const unsigned int btnTimeOut = 5000UL;         // Choose therapy program time out. Counted form released button.


unsigned long pauseTime =0;

volatile boolean pause = false; // true = pause on
unsigned long pressTime = 0;    // Time of pressing the button
unsigned long startInterval;    // For unused timeout off.
int programNo = 1;              // TODO: to reconstruct in free-PEMF deprecated: 0 = PC connection, 1= first program etc.
								// New: 0 = default program in memory, 1-9 or 1-3 = standard programs , -1 = PC
byte hr = 0;                    // User pulse from hrmPin

//function prototypes
int readEepromLine(int fromAddress, String &lineString);
void getParams(String &inputString);
void executeCmd(String cmdLine, boolean directMode = false);
void eepromUpload(int adr = 0);
boolean readSerial2Buffer(int &endBuffer);
void rechargeBattery();
int initBatteryLevel();
void checkBattLevel();


void off();
void beep( unsigned int period);
void freq(unsigned int Freq, unsigned long period);
void rec(unsigned int Freq, unsigned long period);  //deprecated
 int rec();
 int sin();
 int bat();
void wait( unsigned long period);
void exe();
void scan(unsigned long Freq, unsigned long period);
 int mem();
void ls();
void rm();
void cbat();


#ifdef MULTIZAP
void pbar(uint8_t percent, uint32_t period); 		//TODO
void print(String *str); 							//TODO
void wipersON();
void wipersOFF();
#endif




#endif /* BIOZAP_FUNC_H_ */
