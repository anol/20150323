/*
 * ssi_drv8711.cpp
 *
 *  Created on: 3. mars 2015
 *      Author: Anders Emil Olsen
 *
 *	BOOST_DRV8711 Pinout
 *      PA2 Output: Chip Select
 *      PA3 Output: Bin1
 *      PA4 Output: Bin2
 *      PA5 Output: Reset
 *      PA6 M1PWM2: Step (Ain1)
 *      PA7 Output: Direction (Ain2)
 *      PB2 Input:	nStall
 *      PB4 SSI2CL: SPI Clock
 *      PB5 AIN11:	Potmeter
 *      PB6 SSI2RX: SDO (DRV8711 Output)
 *      PB7 SSI2TX: SDI (DRV8711 Input)
 *      PE0 Input:  nFault
 *      PE5 Output: nSleep
 *
 *
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "inc/tm4c123gh6pm.h"
//--------------------------------
#include "ssi_display.h"
#include "ssi_drv8711.h"
#include "alpha7segment.h"
//--------------------------------
extern aeo1::ssi_display g_oScaleDisplay;
//--------------------------------
static aeo1::ssi_drv8711* g_pTheDRV8711 = 0;
//--------------------------------
extern "C" void OnGPIOBInterrupt(void) {
	GPIOIntDisable(GPIO_PORTB_BASE, GPIO_PIN_2);
	if (g_pTheDRV8711) {
		g_pTheDRV8711->OnGpioInterrupt(aeo1::ssi_drv8711::StallEvent);
	}
}
//--------------------------------
extern "C" void OnGPIOEInterrupt(void) {
	GPIOIntDisable(GPIO_PORTE_BASE, GPIO_PIN_0);
	if (g_pTheDRV8711) {
		g_pTheDRV8711->OnGpioInterrupt(aeo1::ssi_drv8711::FaultEvent);
	}
}
//--------------------------------
namespace aeo1 {
//--------------------------------
enum {
	ssi_drv8711_bitrate = 1000000
};
//--------------------------------
ssi_drv8711::ssi_drv8711() :
		ssi_peripheral(ssi_peripheral::SSI2, ssi_drv8711_bitrate, false) {
	memset(m_nRegister, 0, sizeof(m_nRegister));
}
//--------------------------------
ssi_drv8711::~ssi_drv8711() {
}
//--------------------------------
void ssi_drv8711::Initialize() {
	g_pTheDRV8711 = this;
	ssi_peripheral::Initialize();
	// PA2 Output: Chip Select = 0
	// PA3 Output: Bin1 = 0
	// PA4 Output: Bin2 = 0
	// PA5 Output: Reset = 0
	// PA7 Output: Direction = 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
	GPIOPadConfigSet(GPIO_PORTA_BASE,
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7,
	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	GPIOPinWrite(GPIO_PORTA_BASE,
	GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7, 0);
	//PE5 Output: nSleep = 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA,
	GPIO_PIN_TYPE_STD);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0);
	// PB2 Input: nStall
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_FALLING_EDGE);
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA,
	GPIO_PIN_TYPE_STD_WPU);
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_2);
	// PE0 Input: nFault
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
	GPIO_PIN_TYPE_STD_WPU);
	GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0);
	//
	Sleep(false);
	Reset();
	//
	IntEnable(INT_GPIOB);
	IntEnable(INT_GPIOE);
}
//--------------------------------
void ssi_drv8711::Sleep(bool bSleep) {
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, bSleep ? 0 : GPIO_PIN_5);
}
//--------------------------------
void ssi_drv8711::Reset() {
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);
	OnGpioInterrupt(NoStall);
	OnGpioInterrupt(NoFault);
}
//--------------------------------
void ssi_drv8711::OnGpioInterrupt(Event nEvent) {
	switch (nEvent) {
	case NoStall:
		g_oScaleDisplay.Set(".");
		GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_2);
		break;
	case StallEvent:
		g_oScaleDisplay.Set("STALL");
		break;
	case NoFault:
		g_oScaleDisplay.Set(".");
		GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0);
		break;
	case FaultEvent:
		g_oScaleDisplay.Set("FAULT");
		break;
	default:
		break;
	}
}
//--------------------------------
uint32_t ssi_drv8711::Read(uint32_t nRegister) {
	uint32_t nValue = (uint32_t) -1;
	if (NumberOfRegisters > nRegister) {
		uint32_t nRead = 0x8000 | (nRegister << 12);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2); // Chip Select = 1
		Put(nRead);
		nValue = Get();
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0); // Chip Select = 0
		m_nRegister[nRegister] = nValue;
	}
	return nValue;
}
//--------------------------------
uint32_t ssi_drv8711::Write(uint32_t nRegister, uint32_t nValue) {
	uint32_t nEcho = 0;
	if (NumberOfRegisters > nRegister) {
		uint32_t nRead = (nRegister << 12) | (0xFFF & nValue);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2); // Chip Select = 1
		Put(nRead);
		nEcho = Get();
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0); // Chip Select = 0
	}
	return nEcho;
}
//--------------------------------
void ssi_drv8711::PrintStatus(uint32_t nStatus) {
	struct StatusEntry {
		uint32_t nMask;
		const char* zText;
	};
	const StatusEntry StatusTable[] = { { 0x001, "OverTemp" },
			{ 0x002, "A-amps" }, { 0x004, "B-amps" }, { 0x008, "A-fault" }, {
					0x010, "B-fault" }, { 0x020, "Low-V" }, { 0x040, "Stall" },
			{ 0x080, "Latched" }, { 0, "" } };
	const StatusEntry* pEntry = &StatusTable[0];
	while (pEntry) {
		uint32_t nMask = pEntry->nMask;
		if (!nMask) {
			pEntry = 0;
		} else {
			if (nMask & nStatus) {
				UARTprintf("%s ", pEntry->zText);
			}
			pEntry++;
		}
	}
	UARTprintf("\n");
}
//--------------------------------
void ssi_drv8711::Diag() {
	UARTprintf("\nssi_drv8711: ");
	ssi_peripheral::Diag();
	UARTprintf("  Control=%03X, ", m_nRegister[0]);
	UARTprintf("Torque=%03X, ", m_nRegister[1]);
	UARTprintf("Off=%03X, ", m_nRegister[2]);
	UARTprintf("Blank=%03X,\n  ", m_nRegister[3]);
	UARTprintf("Decay=%03X, ", m_nRegister[4]);
	UARTprintf("Stall=%03X, ", m_nRegister[5]);
	UARTprintf("Drive=%03X, ", m_nRegister[6]);
	UARTprintf("Status=%03X\n", m_nRegister[7]);
	if (m_nRegister[7]) {
		UARTprintf("  -> ");
		PrintStatus(m_nRegister[7]);
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
