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
#include "utils/uartstdio.h"
//--------------------------------
#include "aeo1_drivers/qei_sensor.h"
aeo1::qei_sensor g_oQei0(aeo1::qei_sensor::QEI0);
aeo1::qei_sensor g_oQei1(aeo1::qei_sensor::QEI1);
//--------------------------------
#define BIT_RATE	115200
void ConfigureUART(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
	UARTStdioConfig(0, BIT_RATE, 16000000);
}
//--------------------------------
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)
static void PrintProgramInfo() {
	UARTprintf("\n\n");
	UARTprintf(STRINGIZE(ProjName) " " __DATE__ " " __TIME__);
	UARTprintf("\n\n");
}
//--------------------------------
#define APP_SYSTICKS_PER_SEC 1000
static unsigned int g_nTickCounter = 0;
extern "C" void SysTickIntHandler(void) {
	g_nTickCounter++;
}
//--------------------------------
void Initialize() {
	ROM_FPUEnable();
	ROM_FPUStackingEnable();
	ROM_SysCtlClockSet(
	SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	ConfigureUART();
	g_oQei0.Initialize();
	g_oQei1.Initialize();
	SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
	SysTickEnable();
	SysTickIntEnable();
	IntMasterEnable();
}
//--------------------------------
void MainLoop() {
	unsigned int nTickCounter = 0;
	while (1) {
		if (nTickCounter != g_nTickCounter) {
			nTickCounter = g_nTickCounter;
			if (!(nTickCounter % APP_SYSTICKS_PER_SEC)) {
				UARTprintf("0=%d, 1=%d\n", g_oQei0.Get(), g_oQei1.Get());
			}
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
