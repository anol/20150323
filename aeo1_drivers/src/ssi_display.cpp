/*
 * ssi_display.cpp
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
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
//--------------------------------
extern "C" void OnSSI1Interrupt(void) {
	if (aeo1::ssi_display::m_pTheSSI1) {
		aeo1::ssi_display::m_pTheSSI1->OnInterrupt();
	}
}
//--------------------------------
extern "C" void OnSSI3Interrupt(void) {
	if (aeo1::ssi_display::m_pTheSSI3) {
		aeo1::ssi_display::m_pTheSSI3->OnInterrupt();
	}
}
//--------------------------------
namespace aeo1 {
//--------------------------------
 ssi_display_specification Display_SSI1

 = { SYSCTL_PERIPH_GPIOF, // =	GPIOPeripheral
		GPIO_PORTF_BASE, // = GPIOBase
		GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, // = GPIOPins
		SYSCTL_PERIPH_SSI1, // = SSIPeripheral
		SSI1_BASE, // = SSIBase
		GPIO_PF2_SSI1CLK, // = SSIPinClk
		GPIO_PF3_SSI1FSS, // = SSIPinFss
		GPIO_PF0_SSI1RX, // = SSIPinRx
		GPIO_PF1_SSI1TX, // = SSIPinTx
		0x40009004, //SSI1_CR1_R, // = SSI_CR1_R
		INT_SSI1 // =	Interrupt
		};

//--------------------------------
 ssi_display_specification Display_SSI3

 = { SYSCTL_PERIPH_GPIOD, // = GPIOPeripheral
		GPIO_PORTD_BASE, // = GPIOBase
		GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, // = GPIOPins
		SYSCTL_PERIPH_SSI3, // = SSIPeripheral
		SSI3_BASE, // = SSIBase
		GPIO_PD0_SSI3CLK, // = SSIPinClk
		GPIO_PD1_SSI3FSS, // = SSIPinFss
		GPIO_PD2_SSI3RX, // = SSIPinRx
		GPIO_PD3_SSI3TX, // = SSIPinTx
		0x4000B004, //SSI3_CR1_R, // = SSI_CR1_R
		INT_SSI3 // = Interrupt
		};

//--------------------------------
ssi_display* ssi_display::m_pTheSSI1 = 0;
ssi_display* ssi_display::m_pTheSSI3 = 0;
uint32_t ssi_display::m_nCounter = 0;
uint32_t ssi_display::m_nEotCounter = 0;
uint32_t ssi_display::m_nOverflowCounter = 0;
//--------------------------------
ssi_display::ssi_display(device_id nDevice) :
		m_nDevice(nDevice), m_rSpecification(
				(ssi_display::SSI1 == nDevice) ? Display_SSI1 : Display_SSI3), m_bEmpty(
				false) {
	Set(0);
}
//--------------------------------
ssi_display::~ssi_display() {
	Terminate();
}
//--------------------------------
void ssi_display::Initialize() {
	MAP_SysCtlPeripheralEnable(m_rSpecification.m_nSSIPeripheral);
	MAP_SysCtlPeripheralEnable(m_rSpecification.m_nGPIOPeripheral);
	MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinClk);
	MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinFss);
	MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinRx);
	MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinTx);
	MAP_GPIOPinTypeSSI(m_rSpecification.m_nGPIOBase,
			m_rSpecification.m_nGPIOPins);
	// Configure and enable the SSI port for TI master mode.  Use SSI3, system
	// clock supply, master mode, SSI frequency, and 16-bit data.
	SSIConfigSetExpClk(m_rSpecification.m_nSSIBase, SysCtlClockGet(),
	SSI_FRF_MOTO_MODE_2,
	SSI_MODE_MASTER, 10000, 16);
	// Enable the SSI module.
	MAP_SSIEnable(m_rSpecification.m_nSSIBase);
	// Read any residual data from the SSI port.  This makes sure the receive
	// FIFOs are empty, so we don't read any unwanted junk.  This is done here
	// because the TI SSI mode is full-duplex, which allows you to send and
	// receive at the same time.  The SSIDataGetNonBlocking function returns
	// "true" when data was returned, and "false" when no data was returned.
	// The "non-blocking" function checks if there is any data in the receive
	// FIFO and does not "hang" if there isn't.
	while (MAP_SSIDataGetNonBlocking(m_rSpecification.m_nSSIBase, &m_nDataRx[0])) {
	}
	m_bEmpty = true;
	// Enable the SSI interrupt
	switch (m_nDevice) {
	case ssi_display::SSI0:
		break;
	case ssi_display::SSI1:
		m_pTheSSI1 = this;
		break;
	case ssi_display::SSI2:
		break;
	case ssi_display::SSI3:
		m_pTheSSI3 = this;
	default:
		break;
	}
	SSIIntDisable(m_rSpecification.m_nSSIBase,
	SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR);
	SSIIntClear(m_rSpecification.m_nSSIBase,
	SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR);
	(*((volatile uint32_t *) m_rSpecification.m_nSSI_CR1_R)) |= SSI_CR1_EOT; /* switch tx interrupt to eot int */
	SSIIntEnable(m_rSpecification.m_nSSIBase, SSI_TXFF); /* SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR  */
	MAP_IntEnable(m_rSpecification.m_nInterrupt);
}
//--------------------------------
void ssi_display::Terminate() {
	switch (m_nDevice) {
	case ssi_display::SSI0:
		break;
	case ssi_display::SSI1:
		m_pTheSSI1 = 0;
		break;
	case ssi_display::SSI2:
		break;
	case ssi_display::SSI3:
		m_pTheSSI3 = 0;
	default:
		break;
	}
	SSIIntDisable(m_rSpecification.m_nSSIBase, SSI_TXFF); /* SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR  */
	MAP_IntDisable(m_rSpecification.m_nInterrupt);
	MAP_SSIDisable(m_rSpecification.m_nSSIBase);
}
//--------------------------------
void ssi_display::Diag() {
	UARTprintf("SSI: %d/%d/%d\n", m_nEotCounter, m_nCounter,
			m_nOverflowCounter);
}
//--------------------------------
void ssi_display::OnInterrupt() {
	uint32_t nIntStatus = SSIIntStatus(m_rSpecification.m_nSSIBase, false);
	SSIIntClear(m_rSpecification.m_nSSIBase, nIntStatus);
//	if (SSI_TXFF & nIntStatus) {
	LoadFIFO();
	m_nEotCounter++;
//	}
	m_nCounter++;
}
//--------------------------------
void ssi_display::LoadFIFO() {
	int32_t nResult = 1;
	for (int nIndex = 0; nResult && (BufferSize > nIndex); nIndex++) {
		nResult = SSIDataPutNonBlocking(m_rSpecification.m_nSSIBase,
				m_nDataTx[nIndex]);
	}
	if (!nResult) {
		m_nOverflowCounter++;
	}
	m_bEmpty = false;
}
//--------------------------------
uint32_t ssi_display::Digit2Segments(uint32_t nDigitNumber,
		uint32_t nDigitValue, bool bbDecimalPoint) {
	const uint8_t Symbols[18] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D,
			0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
	if (0xF > nDigitValue) {
		uint8_t nSegmentValue = ~(Symbols[nDigitValue]
				| (bbDecimalPoint ? 0x80 : 0x0));
		return (nSegmentValue << 8) | (1 << nDigitNumber);
	} else if (0x10 == nDigitValue) {
		return (0xBF << 8) | (1 << nDigitNumber);
	} else {
		return (0xFF << 8) | (1 << nDigitNumber);
	}
}
//--------------------------------
void ssi_display::Set(int32_t nValue, int nDecimals) {
	bool bNegative = (0 > nValue);
	if (bNegative) {
		nValue = -nValue;
	}
	for (int nDigitNumber = 0; BufferSize > nDigitNumber; nDigitNumber++) {
		int nDigitValue = nValue % 10;
		bool bDecimalPoint = (0 == nDecimals);
		if (nValue || nDigitValue || (0 <= nDecimals)) {
			m_nDataTx[nDigitNumber] = Digit2Segments(nDigitNumber, nDigitValue,
					bDecimalPoint);
		} else if (bNegative) {
			bNegative = false;
			m_nDataTx[nDigitNumber] = Digit2Segments(nDigitNumber, 0x10,
					bDecimalPoint);
		} else {
			m_nDataTx[nDigitNumber] = Digit2Segments(nDigitNumber, 0x11,
					bDecimalPoint);
		}
		nValue /= 10;
		nDecimals--;
	}
	if (m_bEmpty) {
		LoadFIFO();
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
