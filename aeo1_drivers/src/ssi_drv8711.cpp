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
#include "ssi_drv8711.h"
#include "alpha7segment.h"
//--------------------------------
extern "C" void OnGPIOBInterrupt(void) {
}
//--------------------------------
extern "C" void OnGPIOEInterrupt(void) {
}
//--------------------------------
namespace aeo1 {
//--------------------------------
ssi_drv8711::ssi_drv8711() :
		ssi_peripheral(ssi_peripheral::SSI2, 1000000, false) {
	memset(m_nRegister, 0, sizeof(m_nRegister));
}
//--------------------------------
ssi_drv8711::~ssi_drv8711() {
}
//--------------------------------
void ssi_drv8711::Initialize() {
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
	IntEnable(INT_GPIOB);
	// PE0 Input: nFault
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
	GPIO_PIN_TYPE_STD_WPU);
	GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0);
	IntEnable(INT_GPIOE);

	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5); // nSleep = 1

	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5); // Reset = 1
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0); // Reset = 0

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
void ssi_drv8711::Diag() {
	ssi_peripheral::Diag();
	UARTprintf("ssi_drv8711: r_cnt=%d\n", m_nRxCount);
	for (int nRegister = 0; NumberOfRegisters > nRegister; nRegister++) {
		UARTprintf("\t%d=0x%04X\n", nRegister, m_nRegister[nRegister]);
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
