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
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
//--------------------------------
#include "ssi_display.h"
#include "qei_sensor.h"
//--------------------------------
#define APP_SYSTICKS_PER_SEC 32
#define APP_INPUT_BUF_SIZE 128
//--------------------------------
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
//--------------------------------
aeo1::ssi_display g_oDisplay1(aeo1::ssi_display::SSI1);
aeo1::ssi_display g_oDisplay2(aeo1::ssi_display::SSI3);
aeo1::qei_sensor g_oRotaryDialer(aeo1::qei_sensor::QEI0,
		aeo1::qei_sensor::SwapPins);
aeo1::qei_sensor g_oLinearScale(aeo1::qei_sensor::QEI1);
static char g_zInput[APP_INPUT_BUF_SIZE];
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
		g_oDisplay1.Set("UP");
	} else if (nNewDialer < nOldDialer) {
		g_oDisplay1.Set("DOWN");
	}
	nOldDialer = nNewDialer;
	if (nOldScale != nNewScale) {
		g_oDisplay2.Set(nNewScale, 2);
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
	g_oDisplay1.Initialize();
	g_oDisplay1.Set("--------");
	g_oDisplay2.Initialize();
	g_oDisplay2.Set(0, 2);
	g_oRotaryDialer.Initialize();
	g_oLinearScale.Initialize();
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
