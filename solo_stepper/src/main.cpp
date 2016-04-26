/*
 * main.c
 */
//--------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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
#include "esp8266.h"
//--------------------------------
#define APP_SYSTICKS_PER_SEC 50
#define APP_INPUT_BUF_SIZE 128
//--------------------------------
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
//--------------------------------
aeo1::drv8711 g_oDrv8711;
aeo1::esp8266 g_oEsp8266;
static char g_zInput[APP_INPUT_BUF_SIZE];
//--------------------------------
enum MenuMode {
	MenuMode_Menu, MenuMode_Set, MenuMode_Feed, MenuMode_Move
};
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
int CMD_get(int argc, char **argv) {
	char zValue[32] = "";
	if (argc == 2) {
		switch (g_oDrv8711.Get(argv[1], zValue, 32)) {
		case aeo1::drv8711::Success:
			UARTprintf("%s = %s\n", argv[1], zValue);
			break;
		case aeo1::drv8711::No_Such_Attribute_Name:
			UARTprintf("No such attribute name: '%s'\n", argv[1]);
			break;
		default:
			UARTprintf("Failed!\n");
			break;
		}
	} else {
		UARTprintf("Use: get <name>\n");
	}
	return (0);
}
//--------------------------------
int CMD_set(int argc, char **argv) {
	char zValue[32] = "";
	if (argc == 3) {
		switch (g_oDrv8711.Set(argv[1], argv[2])) {
		case aeo1::drv8711::Success:
			g_oDrv8711.Get(argv[1], zValue, 32);
			UARTprintf("%s = %s\n", argv[1], zValue);
			break;
		case aeo1::drv8711::No_Such_Attribute_Name:
			UARTprintf("No such attribute name: '%s'\n", argv[1]);
			break;
		case aeo1::drv8711::Illegal_Attribute_Value:
			UARTprintf("Illegal attribute value: '%s'\n", argv[2]);
			break;
		default:
			UARTprintf("Failed!\n");
			break;
		}
	} else {
		UARTprintf("Use: set <name> <value>\n");
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
	if (argc == 2) {
		if ('w' == *argv[1]) {
			g_oEsp8266.Diag();
		} else if ('x' == *argv[1]) {
			g_oDrv8711.DiagExtra();
		} else {
			g_oDrv8711.Diag();
			g_oDrv8711.DiagExtra();
		}
	} else {
		g_oDrv8711.Diag();
		g_oDrv8711.DiagExtra();
		g_oEsp8266.Diag();
	}
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
int CMD_mreset(int argc, char **argv) {
	g_oDrv8711.Reset();
	return (0);
}
//--------------------------------
int CMD_wreset(int argc, char **argv) {
	g_oEsp8266.Reset();
	g_oEsp8266.Setup();
	return (0);
}
//--------------------------------
int CMD_reset(int argc, char **argv) {
	return (0);
}
//--------------------------------
int CMD_noerr(int argc, char **argv) {
	g_oDrv8711.ClearFaults();
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

{ "set", CMD_set, " : Set attribute value" },

{ "get", CMD_get, " : Get attribute value" },

{ "move", CMD_move, " : Move # steps" },

{ "stop", CMD_stop, " : Stop feed or move" },

{ "diag", CMD_diag,

" : Show diagnostic information, diag [w|m|x] (w=wifi, m=motor, x=extra)" },

{ "sleep", CMD_sleep, " : Toggle DRV8711 Sleep" },

{ "noerr", CMD_noerr, " : Clear DRV8711 faults" },

{ "mreset", CMD_mreset, " : Reset DRV8711" },

{ "wreset", CMD_wreset, " : Reset ESP8266" },

{ "reset", CMD_reset, " : Reset controller" },

{ "help", CMD_help, " : Display list of commands" },

{ 0, 0, 0 } };
//--------------------------------
static void SetupDebugUart() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
	UARTStdioConfig(0, 115200, 16000000);
}
//--------------------------------
static void SetupDebugLeds() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
	GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
	GPIO_PIN_2);
}
//--------------------------------
extern "C" void SysTickIntHandler(void) {
}
//--------------------------------
static void SetupSys() {
	FPUEnable();
	FPUStackingEnable();
	SysCtlClockSet(
	SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
	SysTickEnable();
	SysTickIntEnable();
	IntMasterEnable();
}
//--------------------------------
static void Initialize() {
	SetupSys();
	SetupDebugUart();
	SetupDebugLeds();
	UARTprintf("\n\n");
	UARTprintf(STRINGIZE(ProjName) " " __DATE__ " " __TIME__);
	UARTprintf("\n\n");
	if (g_oEsp8266.Initialize()) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
		GPIO_PIN_3);
	} else {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
		GPIO_PIN_1);
	}
	g_oDrv8711.Initialize();
}
//--------------------------------
static char* RemoteCommand(char* zCmdLine) {
	char* zCommand = 0;
	if (zCmdLine && *zCmdLine) {
		zCommand = strstr(zCmdLine, "+IPD,");
		if (zCommand) {
			zCommand = strchr(zCommand, ':');
			if (zCommand && *zCommand) {
				int32_t nStatus;
				zCommand++;
				UARTprintf("%s\r\n", zCommand);
				if (*zCommand) {
					g_oEsp8266.Write(zCommand);
					nStatus = CmdLineProcess(zCommand);
					if (nStatus == CMDLINE_BAD_CMD) {
						g_oEsp8266.Write("ERROR: Bad command");
					} else if (nStatus == CMDLINE_TOO_MANY_ARGS) {
						g_oEsp8266.Write("ERROR: Bad arguments");
					} else {
						g_oEsp8266.Write("OK");
					}
				}
			} else {
				g_oEsp8266.Write("ERROR");
				UARTprintf(zCmdLine);
			}
		} else {
			UARTprintf(zCmdLine);
		}
	}
	return zCommand;
}
//--------------------------------
static void OnCommand(char* zCmdLine) {
	int32_t i32CommandStatus = CmdLineProcess(zCmdLine);
	if (i32CommandStatus == CMDLINE_BAD_CMD) {
		UARTprintf("Bad command!\n");
	} else if (i32CommandStatus == CMDLINE_TOO_MANY_ARGS) {
		UARTprintf("Too many arguments for command processor!\n");
	}
	UARTprintf("\n>");
}
//--------------------------------
void MainLoop() {
	UARTprintf("\n>");
	while (1) {
		if (-1 != UARTPeek('\r')) {
			UARTgets(g_zInput, sizeof(g_zInput));
			OnCommand(g_zInput);
		} else if (g_oEsp8266.RxEndOfLine()) {
			g_oEsp8266.ReadLine(g_zInput, sizeof(g_zInput));
			char* zCmdLin = RemoteCommand(g_zInput);
			if (zCmdLin) {
				OnCommand(zCmdLin);
			}
		} else {
			SysCtlDelay(SysCtlClockGet() / (1000 / 3));
		}
	}
}
//--------------------------------
int main(void) {
	Initialize();
	MainLoop();
	return 0;
}
//--------------------------------
