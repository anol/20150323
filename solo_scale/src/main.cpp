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
#include "driverlib/qei.h"
//--------------------------------
#include "qei_sensor.h"
#include "ssi_display.h"
#include "cmdline.h"
#include "uartstdio.h"
#include "primary_activity.h"
//--------------------------------
#define APP_SYSTICKS_PER_SEC 32
#define APP_INPUT_BUF_SIZE 128
//--------------------------------
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
//--------------------------------
static char g_zInput[APP_INPUT_BUF_SIZE];
//--------------------------------
primary_activity g_oPrimaryActivity;
//--------------------------------
extern "C" void SysTickIntHandler(void) {
	g_oPrimaryActivity.OnTick();
}

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
	g_oPrimaryActivity.Initialize();
	SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
	SysTickEnable();
	SysTickIntEnable();
	IntMasterEnable();
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
static void MainLoop() {
	while (1) {
		if (-1 != UARTPeek('\r')) {
			UARTgets(g_zInput, sizeof(g_zInput));
			OnCommand(g_zInput);
		} else {
			SysCtlDelay(SysCtlClockGet() / (1000 / 3));
		}
	}
}
//--------------------------------
static void PrintProgramInfo() {
	UARTprintf("\n\n");
	UARTprintf(STRINGIZE(ProjName) " " __DATE__ " " __TIME__);
	UARTprintf("\n\n>");
}
//--------------------------------
int main(void) {
	Initialize();
	PrintProgramInfo();
	MainLoop();
	return 0;
}
//--------------------------------
