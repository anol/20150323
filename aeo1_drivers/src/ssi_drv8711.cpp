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
ssi_drv8711::ssi_drv8711(ssi_peripheral::device_id nDevice) :
		ssi_peripheral(nDevice) {
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

	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0);

//	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);

}
//--------------------------------
void ssi_drv8711::Read() {
	for (int nIndex = 0; BufferSize > nIndex; nIndex++) {
		m_nDataTx[nIndex] = 0x8000 | (nIndex << 12);
	}
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
	LoadTxFIFO();
}
//--------------------------------
void ssi_drv8711::OnRx() {
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
}
//--------------------------------
void ssi_drv8711::Diag() {
	ssi_peripheral::Diag();
	UARTprintf("ssi_drv8711: r_cnt=%d\n", m_nRxCount);
	for (int nIndex = 0; BufferSize > nIndex; nIndex++) {
		UARTprintf("\t%d=0x%04X\n", nIndex, m_nDataRx[nIndex]);
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
