/*
 * main.c
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inc/hw_memmap.h>
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "drivers/rgb.h"
//#include "drivers/buttons.h"
//--------------------------------
// Stellaris lm4f120h5qr is Tiva tm4c1233h6pm
#include "inc/tm4c1233h6pm.h"
//--------------------------------
#include "ain7.h"
//--------------------------------
#define NewLine	"\r\n"
#define APP_PI 3.1415926535897932384626433832795f
#define APP_INTENSITY_DEFAULT 0.2f
#define APP_COLOR_DEFAULT 960
//--------------------------------
enum FixedAttributes {
	TempSenseRa = 6800,
	VoltageReference = 3300,
	SamplesPerSecond = 10,
	TimerPeripheral = SYSCTL_PERIPH_TIMER1,
	TimerBase = TIMER1_BASE,

	// Console
	ConsoleBase = UART0_BASE,
	ConsoleGPIOBase = GPIO_PORTA_BASE,
	ConsoleBitrate = 115200,
	ConsoleInterrupt = INT_UART0,

};
//--------------------------------
struct tColorEntry {
	uint32_t ui32Value;
	uint32_t ui32Colors[3]; // [R, G, B] range is 0 to 0xFFFF per color.
	float fIntensity;
};
//--------------------------------
static tColorEntry oColorTable[] = {

{ 500, 20000, 0, 0, 0.3 },

{ 900, 0, 20000, 0, 0.2 },

{ 1000, 20000, 20000, 20000, 0.1 },

{ 1400, 0, 0, 20000, 0.2 },

{ 4096, 20000, 0, 0, 0.3 },

{ 0, 0, 0, 0, 0.0 }

};
//--------------------------------
static void SetColor(uint32_t nValue, bool bForceUpdate) {
	static uint32_t nPrevious = 0;
	tColorEntry* pColorEntry;
	if ((nValue != nPrevious) || bForceUpdate) {
		for (pColorEntry = oColorTable;
				pColorEntry && (pColorEntry->ui32Value)
						&& (nValue > pColorEntry->ui32Value); pColorEntry++) {
		}
		if (pColorEntry && pColorEntry->ui32Value) {
			RGBColorSet(pColorEntry->ui32Colors);
			RGBIntensitySet(pColorEntry->fIntensity);
		}
	}
}
//--------------------------------
static char m_zString[128] = "";
//--------------------------------
extern "C" void OnConsoleInterrupt(void) {
	int32_t i32Char = 0;
	uint8_t ui8Char;
	uint32_t ulStatus = MAP_UARTIntStatus(ConsoleBase, true);
	MAP_UARTIntClear(ConsoleBase, ulStatus);
	if (ulStatus & (UART_INT_RX | UART_INT_RT)) {
		if (MAP_UARTCharsAvail(ConsoleBase)) {
			i32Char = MAP_UARTCharGetNonBlocking(UART0_BASE);
			if (!(i32Char & ~0xFF)) {
				ui8Char = (uint8_t) (i32Char & 0xFF);
				MAP_UARTCharPut(ConsoleBase, ui8Char);
			}
		}
	}
}
//--------------------------------
static void SetupConsole() {
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
	MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
	MAP_GPIOPinTypeUART(ConsoleGPIOBase, GPIO_PIN_0 | GPIO_PIN_1);
	MAP_UARTConfigSetExpClk(ConsoleBase, SysCtlClockGet(), ConsoleBitrate,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	MAP_IntEnable(ConsoleInterrupt);
	MAP_UARTIntEnable(ConsoleBase, UART_INT_RX | UART_INT_RT);
}
//--------------------------------
static void Send(const char* zString) {
	int nCount = strlen(zString);
	while (0 < nCount--) {
		MAP_UARTCharPut(ConsoleBase, *zString++);
	}
}
//--------------------------------
static void SetupColors() {
	RGBInit(0);
	RGBIntensitySet(APP_INTENSITY_DEFAULT);
	SetColor(APP_COLOR_DEFAULT, true);
	RGBEnable();
}
//--------------------------------
static void PrintProgramInfo() {
	Send(NewLine NewLine);
	Send("test_adc " __DATE__ " " __TIME__ " " NewLine);
	Send(NewLine NewLine);
}
//--------------------------------
int MainLoop() {
	ain7 oAin7;
	uint32_t nNewValue;
	uint32_t nOldValue = 0;
	oAin7.Initialize();
	oAin7.Trigger();
	while (1) {
		if (oAin7.IsUpdated()) {
			nNewValue = oAin7.GetValue();
			if (((nOldValue - 1) > nNewValue)
					|| ((nOldValue + 1) < nNewValue)) {
				nOldValue = nNewValue;
				sprintf(m_zString, "\r% 6d", nNewValue);
				Send(m_zString);
				SetColor(nNewValue, false);
			}
			oAin7.Trigger();
		} else if (int nError = oAin7.GetErrorCounter()) {
			sprintf(m_zString, "Errors:% 6d" NewLine, nError);
			Send(m_zString);
		}
	}
}
//--------------------------------
int main(void) {
	SetupConsole();
	SetupColors();
	PrintProgramInfo();
	MainLoop();
}
//--------------------------------
