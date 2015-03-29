/*
 * main.c
 */
//--------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_hibernate.h"
#include "inc/hw_gpio.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/qei.h"
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
//--------------------------------
#include "ssi_display.h"
#include "qei_sensor.h"
#include "drv8711.h"
//--------------------------------
#define APP_SYSTICKS_PER_SEC 32
#define APP_INPUT_BUF_SIZE 128
//--------------------------------
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
//--------------------------------
aeo1::ssi_display g_oDialerDisplay(aeo1::ssi_display::SSI1);
aeo1::ssi_display g_oScaleDisplay(aeo1::ssi_display::SSI3);
aeo1::qei_sensor g_oRotaryDialer(aeo1::qei_sensor::QEI0,
		aeo1::qei_sensor::SwapPins);
aeo1::qei_sensor g_oLinearScale(aeo1::qei_sensor::QEI1);
aeo1::drv8711 g_oDrv8711;
static char g_zInput[APP_INPUT_BUF_SIZE];
//--------------------------------
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
int CMD_idle(int argc, char **argv) {
	g_oDrv8711.Idle();
	return (0);
}
//--------------------------------
int CMD_halt(int argc, char **argv) {
	g_oDrv8711.Halt();
	return (0);
}
//--------------------------------
int CMD_feed(int argc, char **argv) {
	if (argc == 2) {
		uint32_t nValue = ustrtoul(argv[1], 0, 10);
		g_oDrv8711.Feed(nValue);
	}
	return (0);
}
//--------------------------------
int CMD_move(int argc, char **argv) {
	if (argc == 2) {
		uint32_t nValue = ustrtoul(argv[1], 0, 10);
		g_oDrv8711.Move(nValue);
	}
	return (0);
}
//--------------------------------
int CMD_stop(int argc, char **argv) {
	g_oDrv8711.Stop();
	return (0);
}
//--------------------------------
int CMD_diag(int argc, char **argv) {
	g_oDialerDisplay.Diag();
	g_oScaleDisplay.Diag();
	g_oRotaryDialer.Diag();
	g_oLinearScale.Diag();
	g_oDrv8711.Diag();
	return (0);
}
//--------------------------------
tCmdLineEntry g_psCmdTable[] = {

{ "help", CMD_help, " : Display list of commands" },

{ "idle", CMD_idle, " : Idle (free) the motor" },

{ "halt", CMD_halt, " : Halt (breake) the motor" },

{ "feed", CMD_feed, " : Feed by speed (micrometers per second)" },

{ "move", CMD_move, " : Move by lenght (micrometers)" },

{ "stop", CMD_stop, " : Stop feed or move" },

{ "diag", CMD_diag, " : Show diagnostic information" },

{ 0, 0, 0 } };
//--------------------------------
void ConfigureUART(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
	UARTStdioConfig(0, 115200, 16000000);
}
//--------------------------------
extern "C" void SysTickIntHandler(void) {
	static int32_t nOldDialer = 0;
	static int32_t nOldScale = 0;
	int32_t nNewDialer = g_oRotaryDialer.Get() / 2;
	int32_t nNewScale = g_oLinearScale.Get();
	if (nNewDialer > nOldDialer) {
		g_oDialerDisplay.Set("UP");
	} else if (nNewDialer < nOldDialer) {
		g_oDialerDisplay.Set("DOWN");
	}
	nOldDialer = nNewDialer;
	if (nOldScale != nNewScale) {
		g_oScaleDisplay.Set(nNewScale, 2);
		nOldScale = nNewScale;
	}
}
//--------------------------------
void Initialize() {
	ROM_FPUEnable();
	ROM_FPUStackingEnable();
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
	SYSCTL_OSC_MAIN);
	ConfigureUART();
	//
	g_oDialerDisplay.Initialize();
	g_oDialerDisplay.Set("--------");
	g_oScaleDisplay.Initialize();
	g_oScaleDisplay.Set(0, 2);
	g_oRotaryDialer.Initialize();
	g_oLinearScale.Initialize();
	g_oDrv8711.Initialize();
	//
	SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
	SysTickEnable();
	SysTickIntEnable();
	IntMasterEnable();
}
//--------------------------------
static void PrintProgramInfo() {
	UARTprintf("\n\n");
	UARTprintf(STRINGIZE(ProjName) " " __DATE__ " " __TIME__);
	UARTprintf("\n\n");
}
//--------------------------------
void MainLoop() {
	int32_t i32CommandStatus;
	while (1) {
		UARTprintf("\n>");
		while (UARTPeek('\r') == -1) {
			// SysCtlSleep();
			SysCtlDelay(SysCtlClockGet() / (1000 / 3));
		}
		UARTgets(g_zInput, sizeof(g_zInput));
		i32CommandStatus = CmdLineProcess(g_zInput);
		if (i32CommandStatus == CMDLINE_BAD_CMD) {
			UARTprintf("Bad command!\n");
		} else if (i32CommandStatus == CMDLINE_TOO_MANY_ARGS) {
			UARTprintf("Too many arguments for command processor!\n");
		}
	}
}
//--------------------------------
int main(void) {
	Initialize();
	PrintProgramInfo();
	MainLoop();
	return 0;
}
//--------------------------------
