/**********************************************************
 * bioZAP_func.cpp
 * 		multiZAP and free-PEMF devices library.
 * 		BIOzap script therapy language.
 * 		See https://biotronika.pl
 *
 *  Created on: 2017-01-01
 *      Author: elektros230 & hehaka
 **********************************************************/

#include "Arduino.h"
#include <bioZAP_func.h>

#ifdef MULTIZAP
void wipersON(){
  ds1803.set_wiper0(wiper0);
  ds1803.set_wiper1(wiper1);
}

void wipersOFF(){
  ds1803.set_wiper0(0);
  ds1803.set_wiper1(0);
  ad9850.powerDown();
}
#endif

#ifdef FREE_PEMF

#endif


String formatLine(int adr, String line){
  String printLine;
  printLine.reserve(22);
  printLine = "000"+String(adr,DEC);
  printLine = printLine.substring(printLine.length()-3,  printLine.length());
  printLine+=": "+line; //end marker for appending program
  return printLine;
}

void executeCmd(String cmdLine, boolean directMode){
  // Main interpreter function
  //digitalWrite(powerPin, HIGH);
  getParams(cmdLine);


    if ( param[0]=="mem" ) {
// Upload therapy to EEPROM
    	mem();


    } else if ( param[0]=="ls" ) {
//List therapy
    	ls();

    } else if (param[0].charAt(0)=='#') {
// Comment
    	;


    } else if (param[0].charAt(0)==':') {
// Label (ignore)
//TODO: for hehaka
    	;


    } else if (param[0]==""){
// Emptyline
    	;


    } else if (param[0]=="rm"){
// Remove, clear script therapy from memory
    	rm();


    } else if (param[0]=="print"){
// Print command
		#ifdef MULTIZAP
    	//TODO: message(param[1]);
		#endif

      if (cmdLine.length()>6) {   //TODO: I don't remember why 6 - to check (elektros)
    	  Serial.println(cmdLine.substring(6,cmdLine.length()-1));
      } else {
    	  Serial.println();
      }


    } else if (param[0]=="bat"){
// Print battery voltage
        Serial.println(bat());


    } else if (param[0]=="cbat"){
// Calibrate battery voltage
    	cbat();


    } else if (param[0]=="hr"){
// Print heart rate
        Serial.println(hr);


    } else if (param[0]=="beep"){
// Beep [time_ms]
        beep(param[1].toInt());
        Serial.println("OK");


    } else if (param[0]=="off"){
// Turn off
    	off();


    } else if (param[0]=="chp"){
// Change output signal polarity
    	Serial.println("Error: multiZAP doesn't support");


    } else if (param[0]=="wait"){
// Wait millis
    	wait(param[1].toInt());
      	Serial.println("OK");


    } else if (param[0]=="freq" || param[0]=="rec" || param[0]=="sin"){
// Generate rectangle signal - rec [freq] [time_sec]
    	//TODO: Different result functions - to divide
    	freq(param[1].toInt(), param[2].toInt());
      	Serial.println("OK");


    } else if (param[0]=="scan"){
// Scan from lastFreq  - scan [freq to] [time_ms]
    	scan(param[1].toInt(), param[2].toInt());
    	Serial.println("OK");


    } else if (param[0]=="exe"){
// Execute eeprom program only in direc mode
    	if ( directMode) {
    		exe();
    	} else {
    		Serial.println("Error: can't execute program from eeprom program!");
    	}


    }  else {
//Unknown command

    	Serial.println("Unknown command: "+param[0]);

    }


}

///////////////////////////// bioZAP functions ///////////////////////////////

#ifdef MULTIZAP
void pbar(uint8_t percent, uint32_t period){
// Scaling progress bar on lcd, and show remaining time
//TODO: elektros
	;
}

void print(String *str){
// Shows script therapy message on lcd display
// TODO: elektros
	;
}
#endif

void cbat(){
// Calibrate battery voltage

	//Correction factor
	byte i = 100 * param[1].toInt()/(int(analogRead(batPin)*BATTERY_VOLTAGE_RATIO));

	EEPROM.put(EEPROM_BATTERY_CALIBRATION_ADDRESS, i);
	Serial.println("OK");
}

void rm(){
// Remove, clear script therapy from memory

	for(int i=0; i<PROGRAM_SIZE; i++){
		EEPROM.put(i, 255);
		if (!(i % 128)) Serial.print(".");
	}
	Serial.println("OK");
}

void ls(){
//List script therapy
	int adr=0;
	int endLine;
	String line;

	if (param[1]=="-n") {
		Serial.println("Adr  Command");

		while ((endLine = readEepromLine(adr,line)) && (adr<PROGRAM_SIZE) ){
		  Serial.print(formatLine(adr,line));
		  adr = adr + endLine;
		}

		//End marker (@) informs an user where to start appending of program
		if (adr<PROGRAM_SIZE) {
			Serial.println(formatLine(adr,"@"));
		}

	} else {

		for(int i=0; i<PROGRAM_SIZE; i++){
			char eeChar=(char)EEPROM.read(i);

			if ((eeChar=='@') || (eeChar==char(255))) {
				break;
			}

			Serial.print(eeChar);
		}
	}

}

int mem(){
// Upload therapy to EEPROM

	if (param[1]=="\0") {
		eepromUpload();

	} else if (param[1]=="@") {
		//Find script end
		int endAdr=0;
		for (int i=0; i<PROGRAM_SIZE; i++){
		  if ((byte)EEPROM.read(i)==255 || (char)EEPROM.read(i)=='@'){
			endAdr=i;

			break;
		  }
		}
		Serial.println(formatLine(endAdr,"appending from..."));
		eepromUpload(endAdr);

	} else if (param[1].toInt()>0 && param[1].toInt()<PROGRAM_SIZE) {
		eepromUpload(param[1].toInt());
	} else {
		Serial.print("Error: unknown parameter ");
		Serial.println(param[1]);
		return -1;
	}

	return 0;
}

void exe(){
//Execute program

	int adr=0;
	String line;

	while (int endLine = readEepromLine(adr,line)){


		Serial.print("executing: ");
		Serial.print(line);


		if (line.startsWith("repeat")) {
			adr=0;
			line="";
		}

		executeCmd(line);
		adr = adr + endLine;
	}


  	Serial.println("Script done.");
  	Serial.println("OK");

}

void scan(unsigned long Freq, unsigned long period){
	// Scan from lastFreq to Freq used SCAN_STEPS by period

	long scanSteps=SCAN_STEPS;
	long stepPeriod = period /scanSteps;

	if (stepPeriod < 1) {
		scanSteps = period;
		stepPeriod=1;
	}

 	long startFreq = lastFreq;
 	long stepFreq = long( constrain(Freq, MIN_FREQ_OUT, MAX_FREQ_OUT) - lastFreq ) / scanSteps;

 /*
 	Serial.println(Freq);
 	Serial.println(lastFreq);
 	Serial.println(long(Freq-lastFreq));
 	Serial.println(startFreq);
 	Serial.println(stepPeriod);
 	Serial.println(scanSteps);
 	Serial.println(stepFreq);
*/

 	for (int i=0; i<scanSteps; i++) {
 		freq(startFreq+(i*stepFreq), stepPeriod);
 	}
}

#ifdef MULTIZAP
void freq(unsigned int Freq, unsigned long period) {
//Rectangle signal generate, Freq=783 for 7.83Hz, period in seconds

  lastFreq =constrain( Freq, MIN_FREQ_OUT, MAX_FREQ_OUT) ; //For scan() function propose

  unsigned long interval = 50000/constrain(Freq, MIN_FREQ_OUT, MAX_FREQ_OUT);
  unsigned long timeUp = millis() + (period*1000);
  unsigned long serialStartPeriod = millis();
  unsigned long startInterval = millis();
  unsigned long pausePressed;

  boolean pause = false;

  //start
  wipersON();
  ad9850.set_frequency(Freq);

  while(millis()< timeUp) {
      //time loop

      if ((millis() - startInterval) >= interval) {

        //Save start time interval
        startInterval = millis();

      }

      checkBattLevel(); //If too low then off

      //TODO Serial break command - mark @
      //TODO Keyboard break

      if (pause) {
        //Pause - button pressed

          pausePressed = millis();
          beep(200);
          wipersOFF();

          while (pause){
            //wait pauseTimeOut or button pressed
            if (millis()> pausePressed + pauseTimeOut) {
               beep(500);
               off();
            }
          }
          beep(200);

          //Correct working time
          timeUp += millis()-pausePressed;
          startInterval += millis()-pausePressed;

          //Continue
          wipersON();
          ad9850.set_frequency(Freq);
      }

      //count each second
      if (millis()-serialStartPeriod >= 1000) { //one second
        Serial.print('.');
        serialStartPeriod = millis();
      }
  }
  wipersOFF();

}
#endif


int rec(){

	//-1 not supported
	return -1;
}

int sin(){

	//0 - That's it (only sin)
	return 0;
}

void off() {
  // Power off function

	// Turn coil off by making digital out to low
	ad9850.powerDown();

	// Turn electrodes off by setting both pots to 0
	ds1803.set_wiper0(0);
	ds1803.set_wiper1(0);

	// Turn power off if not USB power
	digitalWrite(powerPin, LOW);


}

int bat() {
  // Get battery voltage function

  return (  analogRead(batPin) *
            BATTERY_VOLTAGE_RATIO *
            (byte)EEPROM.read(EEPROM_BATTERY_CALIBRATION_ADDRESS) /
            100
          );
}

void wait( unsigned long period) {
  // wait [period_ms]

  unsigned long serialStartPeriod = millis();
  unsigned long startInterval = millis();

  while(millis()-startInterval <= period){
    //time loop

	//TODO serial break command - mark @

    //count each second
    if (millis()-serialStartPeriod >= 1000) {
      Serial.print('.');
      serialStartPeriod = millis();
    }
  }

}

void beep( unsigned int period ) {
  // beep [period_ms]

  unsigned long serialStartPeriod = millis();
  unsigned long startInterval = millis();

  digitalWrite(buzerPin, HIGH);
  while(millis()-startInterval <= period){
    //time loop

	//TODO serial break command - mark @

    //count each second
    if (millis()-serialStartPeriod >= 1000) { //one second
      Serial.print('.');
      serialStartPeriod = millis();
    }
  }

  digitalWrite(buzerPin, LOW);
}

/***************** bioZAP functions end *************************/

int readEepromLine(int fromAddress, String &lineString){
  //Read one line from EEPROM memory
  int i = 0;
  lineString="";
  do {
    char eeChar=(char)EEPROM.read(fromAddress+i);
    if ((eeChar==char(255)) ||(eeChar==char('@'))) {
      if (i>0) {
        eeChar='\n';
      } else {
        i=0;
        break;
      }
    }
    lineString+=eeChar;
    i++;
    if (eeChar=='\n') break;
  } while (1);

  return i;
}

void getParams(String &inputString){
  for (int i=0; i<MAX_CMD_PARAMS; i++) param[i]="";

  int from =0;
  int to =0;
  for (int i=0; i<MAX_CMD_PARAMS; i++){
    to = inputString.indexOf(' ',from); //Wykryj spacje

    if (to==-1) {
      to = inputString.indexOf('\n',from); //Wykryj NL #10
      if (to>0) param[i] = inputString.substring(from,to);
      param[i].trim();
      break;
    }

    if (to>0) param[i] = inputString.substring(from,to);
    param[i].trim();
    from = to+1;
  }
}



void checkBattLevel() {
  //Check battery level

  if ( analogRead(batPin) < minBatteryLevel) {
    //Emergency turn off


	  Serial.println();
	  Serial.print("Error: battery too low: ");
	  Serial.println(bat());



    for (int x=0; x<10; x++){
      digitalWrite(buzerPin, HIGH);   // Turn buzzer on
      delay(100);
      digitalWrite(buzerPin, LOW);    // Turn buzzer off
      delay(200);
    }

    beep(500);
    off();
  }

}

void rechargeBattery() {
//Charger is plugged

	// turn power ON because of # key stop issue
	//digitalWrite(powerPin, HIGH);

	lcd.init();
	lcd.print("Battery charging");
	//digitalWrite(powerPin, LOW); // turn power relay off


	unsigned long startInterval = millis();
	int startBatLevel = analogRead(batPin);

	//Turn backlight off
	delay(5000);
	lcd.noBacklight();
	digitalWrite(powerPin, LOW); // turn power relay off

	do {
		if ( millis() - startInterval > checkDeltaBatteryIncreasingVoltageTime) {
		  if (analogRead(batPin)-startBatLevel <= 0) { //no increasing voltage
			  //Battery charged

			  digitalWrite(powerPin, HIGH);

			  lcd.init();
			  lcd.print("Battery charged");

			  beep(200);

			  delay(5000);
			  lcd.noBacklight();

			  digitalWrite(powerPin, LOW);

			  // ... and charging further.
			  while (1);
		  }

		  //Start new charging period with new values
		  startInterval = millis();
		  startBatLevel = analogRead(batPin);
		}
	}  while (1); //forever loop
}

int initBatteryLevel(){

	  //Auto-correction voltage - for new device - see bat & cbat commands
	  if ( (byte)EEPROM.read(EEPROM_BATTERY_CALIBRATION_ADDRESS) > 130 ||
	       (byte)EEPROM.read(EEPROM_BATTERY_CALIBRATION_ADDRESS) < 70 ) {
	    EEPROM.put(EEPROM_BATTERY_CALIBRATION_ADDRESS,100); // 100 =  x 1.00
	  }

	  //Define minimum battery level uses in working for performance purpose.
	  return 	/*0-1023*/ 100 *
	                       MIN_BATTERY_LEVEL /
	                       BATTERY_VOLTAGE_RATIO /
	                       (byte)EEPROM.read(EEPROM_BATTERY_CALIBRATION_ADDRESS);
}

void serialEvent() {
 //if (!eepromLoad) {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar!='\r'){
      inputString += inChar;
    }

    Serial.print(inChar); //echo

    // if the incoming character is a newline, set a flag
    if (inChar == '\n') {
      stringComplete = true;
    }

    if (inChar == '@') {
      memComplete = true;
    }
  }
 //}
}

void eepromUpload(int adr) {
  unsigned int i = 0;
  boolean flagCompleted;
  boolean Xoff = false;
  int endBuffer;
  //eepromLoad = true;

  do {
    //Serial.print(char(XON));
    Xoff = readSerial2Buffer(endBuffer);
    int b =0; // buffer pointer
    flagCompleted = false;
    while (!flagCompleted){

      flagCompleted = !(i+adr<PROGRAM_SIZE) || (memBuffer[b]=='@') || !(b < endBuffer);
      if (memBuffer[b]==';') memBuffer[b]='\n';   //Semicollon as end line LF (#10) for windows script
      if (memBuffer[b]=='\r') memBuffer[b] = ' '; //#13 -> space, No continue because of changing script length
      EEPROM.write(i+adr, memBuffer[b]);
      //Serial.print(memBuffer[b]);
      i++; b++;
    }
    //End of shorter program then PROGRAM_SIZE size

  } while (Xoff);
  if (i+adr<PROGRAM_SIZE) EEPROM.write(i+adr, 255);
  //eepromLoad=false;
}

boolean readSerial2Buffer(int &endBuffer) {
    int i = 0; //buffer indicator
    char c;

    boolean Xoff = false;
    int highBufferLevel = 0.7 * PROGRAM_BUFFER;

    Serial.write(XON);
    //Serial.print("\nXON\n");

    while(true) {
      if (Xoff) {
        //after send Xoff

          if (Serial.available()){
            c = Serial.read();
            memBuffer[i] = c;
            //Serial.print(c);
            endBuffer = i;
            i++;

          } else {
            break;
          };
           //if (i>= PROGRAM_BUFFER) break;


      } else {
        //before send Xoff

          Xoff = (i > highBufferLevel);

          while(!Serial.available());

          c = Serial.read();

          memBuffer[i] = c;
          //Serial.print(c);
          endBuffer = i;

          if (c == '@' ) {
            break;
          }

          i++;
          if (Xoff) {
            for (int j=0; j<64; j++)
            Serial.write(XOFF);
            //Serial._rx_complete_irq();
            //Serial._tx_udr_empty_irq();
            //Serial.print("\nXOFF\n");
          }
      }



    }
  return Xoff;

}
