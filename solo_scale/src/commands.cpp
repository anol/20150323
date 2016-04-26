//*****************************************************************************
//
// rgb_commands.c - Command line functionality implementation
//
// Copyright (c) 2012-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/qei.h"
//--------------------------------
#include "commands.h"
#include "qei_sensor.h"
#include "ssi_display.h"
#include "cmdline.h"
#include "uartstdio.h"
#include "ustdlib.h"
//--------------------------------
extern aeo1::ssi_display g_oDisplayX;
extern aeo1::ssi_display g_oDisplayY;
extern aeo1::qei_sensor g_oScaleX;
extern aeo1::qei_sensor g_oScaleY;
//*****************************************************************************
//
// Table of valid command strings, callback functions and help messages.  This
// is used by the cmdline module.
//
//*****************************************************************************
tCmdLineEntry g_psCmdTable[] = {

{ "help", CMD_help, " : Display list of commands" },

{ "setx", CMD_setx, " : Set the X value" },

{ "sety", CMD_sety, " : Set the Y value" },

{ "diag", CMD_diag, " : Show diagnostic information" },

{ "dummy", CMD_dummy, " : Dummy command" },

{ 0, 0, 0 } };

//*****************************************************************************
//
// Command: help
//
// Print the help strings for all commands.
//
//*****************************************************************************
int CMD_help(int argc, char **argv) {
	int32_t i32Index;
	(void) argc;
	(void) argv;
	i32Index = 0;
	UARTprintf("\nAvailable Commands\n------------------\n\n");
	while (g_psCmdTable[i32Index].pcCmd) {
		UARTprintf("%17s %s\n", g_psCmdTable[i32Index].pcCmd,
				g_psCmdTable[i32Index].pcHelp);
		i32Index++;
	}
	UARTprintf("\n");
	return (0);
}
//--------------------------------
int CMD_dummy(int argc, char **argv) {
	// Keep the compiler happy.
	(void) argc;
	(void) argv;
	return (0);
}
//--------------------------------
int CMD_setx(int argc, char **argv) {
	if (argc == 2) {
		uint32_t nValue = ustrtoul(argv[1], 0, 10);
		g_oDisplayX.Set(nValue, 2);
		g_oScaleX.Set(nValue);
	}
	return (0);
}
//--------------------------------
int CMD_sety(int argc, char **argv) {
	if (argc == 2) {
		uint32_t nValue = ustrtoul(argv[1], 0, 10);
		g_oDisplayY.Set(nValue, 2);
		g_oScaleY.Set(nValue);
	}
	return (0);
}
//--------------------------------
int CMD_diag(int argc, char **argv) {
	g_oDisplayX.Diag();
	g_oDisplayY.Diag();
	g_oScaleX.Diag();
	g_oScaleY.Diag();
	return (0);
}
//--------------------------------
