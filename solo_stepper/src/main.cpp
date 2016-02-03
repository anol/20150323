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
#define APP_SYSTICKS_PER_SEC 50
#define APP_INPUT_BUF_SIZE 128
//--------------------------------
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
//--------------------------------
aeo1::drv8711 g_oDrv8711;
static char g_zInput[APP_INPUT_BUF_SIZE];
//--------------------------------
enum MenuMode {
	MenuMode_Menu, MenuMode_Set, MenuMode_Feed, MenuMode_Move
};
static MenuMode g_nMenuMode = MenuMode_Menu;
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
	} else {
		UARTprintf("Use: move <um>\n");
	}
	return (0);
}
//--------------------------------
int CMD_stop(int argc, char **argv) {
	if (argc == 2) {
		g_oDrv8711.Stop(false);
	} else {
		g_oDrv8711.Stop(true);
	}
	return (0);
}
//--------------------------------
int CMD_diag(int argc, char **argv) {
	g_oDrv8711.Diag();
	return (0);
}
//--------------------------------
int CMD_sleep(int argc, char **argv) {
	static bool bSleep = false;
	bSleep = !bSleep;
	g_oDrv8711.Sleep(bSleep);
	return (0);
}
//--------------------------------
int CMD_reset(int argc, char **argv) {
	g_oDrv8711.Reset();
	return (0);
}
//--------------------------------
int CMD_noerr(int argc, char **argv) {
	g_oDrv8711.ClearFaults();
	return (0);
}
//--------------------------------
int CMD_step(int argc, char **argv) {
	if (argc == 2) {
		int nValue = ustrtoul(argv[1], 0, 10);
		if (0 > nValue) {
			nValue = -nValue;
			g_oDrv8711.Step(nValue, false);
		} else {
			g_oDrv8711.Step(nValue, true);
		}
	}
	return (0);
}
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
tCmdLineEntry g_psCmdTable[] = {

{ "idle", CMD_idle, " : Idle (free) the motor" },

{ "halt", CMD_halt, " : Halt (breake) the motor" },

{ "feed", CMD_feed, " : Feed by speed (micrometers per second) - not implemented yet" },

{ "move", CMD_move, " : Move by lenght (micrometers)" },

{ "stop", CMD_stop, " : Stop feed or move" },

{ "diag", CMD_diag, " : Show diagnostic information" },

{ "sleep", CMD_sleep, " : Toggle DRV8711 Sleep" },

{ "noerr", CMD_noerr, " : Clear DRV8711 faults" },

{ "reset", CMD_reset, " : Reset DRV8711" },

{ "step", CMD_step, " : Single step (RSTEP)" },

{ "help", CMD_help, " : Display list of commands" },

{ 0, 0, 0 } };
//--------------------------------
int MNU_set(int argc, char **argv) {
	(void) argc;
	(void) argv;
	g_nMenuMode = MenuMode_Set;
	g_oRotaryDialer.Set(g_oLinearScale.Get() * 2);
	return (0);
}
//--------------------------------
int MNU_feed(int argc, char **argv) {
	(void) argc;
	(void) argv;
	g_nMenuMode = MenuMode_Feed;
	g_oRotaryDialer.Zero();
	return (0);
}
//--------------------------------
int MNU_move(int argc, char **argv) {
	(void) argc;
	(void) argv;
	g_nMenuMode = MenuMode_Move;
	g_oRotaryDialer.Zero();
	return (0);
}
//--------------------------------
bool OnFeed(int nEvent) {
	static int32_t nOldPosition = 0;
	int32_t nNewPosition = g_oRotaryDialer.Get() / 2;
	int32_t nFeed = nNewPosition - nOldPosition;
	nOldPosition = nNewPosition;
	if (nEvent) {
		g_oDialerDisplay.Set(nNewPosition, 2);
		if (0 > nFeed) {
			nFeed = -nFeed;
			g_oDrv8711.Step(nFeed * 16, false);
		} else {
			g_oDrv8711.Step(nFeed * 16, true);
		}
		return false;
	} else {
		nOldPosition = 0;
		g_oRotaryDialer.Set(0);
		g_nMenuMode = MenuMode_Menu;
		return true;
	}
}
//--------------------------------
bool OnMove(int nEvent) {
	static int32_t nOldPosition = 0;
	static bool bCoarseTune = true;
	bool bFinished = false;
	int32_t nNewPosition = g_oRotaryDialer.Get();
	if (bCoarseTune) {
		nNewPosition *= 100;
	}
	if (nEvent) {
		g_oDialerDisplay.Set(nNewPosition, 2);
	} else {
		if (bCoarseTune) {
			bCoarseTune = false;
			g_oRotaryDialer.Set(nNewPosition);
		} else {
			int32_t nFeed = nNewPosition - nOldPosition;
			g_oDrv8711.Move(nFeed * 10);
			bCoarseTune = true;
			nOldPosition = 0;
			g_oRotaryDialer.Set(0);
			g_nMenuMode = MenuMode_Menu;
			bFinished = true;
		}
	}
	return bFinished;
}
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
}
//--------------------------------
void Initialize() {
	FPUEnable();
	FPUStackingEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
	SYSCTL_OSC_MAIN);
	ConfigureUART();
	//
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
		UARTprintf("\n>");
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
