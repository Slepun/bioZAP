**********************************************************
 * bioZAP_commands.h
 * 		multiZAP and free-PEMF devices library.
 * 		BIOzap script therapy language.
 * 		See https://biotronika.pl
 *
 *  Created on: 2018-01-30
 *      Author: Slepun
 **********************************************************/

#ifndef BIOZAP_COMMANDS_H_
#define BIOZAP_COMMANDS_H_

 //-----------Types-----------
typedef enum bioZapCmdsE{
 	CMD_UNK = 0,
 	CMD_MEM = 1,
 	CMD_LS = 2,
 	CMD_HASH = 3,
 	CMD_EMPTY = 4,
 	CMD_RM = 5,
 	CMD_PRINT = 6,
 	CMD_BAT = 7,
 	CMD_CBAT = 8,
 	CMD_HR = 9,
 	CMD_BEEP = 10,
 	CMD_OFF = 11,
 	CMD_CHP = 12,
 	CMD_WAIT = 13,
 	CMD_FREQ = 14,
 	CMD_REC = 15,
 	CMD_SIN = 16,
 	CMD_SCAN = 17,
 	CMD_EXE = 18,
	CMD_REPEAT = 19,
 	CMD_LAST_ENUM
} eBioZapCmds;

//-----------Global variables-----------
String aBioZapCmds[CMD_LAST_ENUM] = {
	""			//0 unknown command
	"mem",		//1
	"ls",		//2
	"#",		//3
	"",			//4
	"rm",		//5
	"print",	//6
	"bat",		//7
	"cbat",		//8
	"hr",		//9
	"beep",		//10
	"off",		//11
	"chp",		//12
	"wait",		//13
	"freq",		//14
	"rec",		//15
	"sin",		//16
	"scan",		//17
	"exe",		//18
	"repeat"	//19
 };

//-----------Functions declarationss-----------
eBioZapCmds cmdToValue(String *cmdName);
String valueTocmd(eBioZapCmds value);

#endif /* BIOZAP_COMMANDS_H_ */
