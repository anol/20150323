/*
 * display.cpp
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
#include "display.h"
//--------------------------------
extern "C" void OnSSI3Interrupt(void) {
	if (aeo1::display::m_pTheOneAndOnly) {
		aeo1::display::m_pTheOneAndOnly->OnInterrupt();
	}
}
//--------------------------------
namespace aeo1 {
//--------------------------------
display* display::m_pTheOneAndOnly = 0;
uint32_t display::m_nCounter = 0;
uint32_t display::m_nEotCounter = 0;
uint32_t display::m_nOverflowCounter = 0;
//--------------------------------
display::display() :
		m_bEmpty(false) {
	Set(0);
}
//--------------------------------
display::~display() {
	if (this == m_pTheOneAndOnly) {
		m_pTheOneAndOnly = 0;
	}
	Terminate();
}
//--------------------------------
void display::Initialize() {
	m_pTheOneAndOnly = this;
	MAP_SysCtlPeripheralEnable(SSIPeripheral);
	MAP_SysCtlPeripheralEnable(GPIOPeripheral);
	MAP_GPIOPinConfigure(SSIPinClk);
	MAP_GPIOPinConfigure(SSIPinFss);
	MAP_GPIOPinConfigure(SSIPinRx);
	MAP_GPIOPinConfigure(SSIPinTx);
	MAP_GPIOPinTypeSSI(GPIOBase, GPIOPins);
	// Configure and enable the SSI port for TI master mode.  Use SSI3, system
	// clock supply, master mode, SSI frequency, and 16-bit data.
	SSIConfigSetExpClk(SSIBase, SysCtlClockGet(),

	//SSI_FRF_MOTO_MODE_0,
	//SSI_FRF_MOTO_MODE_1,
			SSI_FRF_MOTO_MODE_2,
			//SSI_FRF_MOTO_MODE_3,
			//SSI_FRF_TI,

			SSI_MODE_MASTER, 10000, 16);
	// Enable the SSI module.
	MAP_SSIEnable(SSIBase);
	// Read any residual data from the SSI port.  This makes sure the receive
	// FIFOs are empty, so we don't read any unwanted junk.  This is done here
	// because the TI SSI mode is full-duplex, which allows you to send and
	// receive at the same time.  The SSIDataGetNonBlocking function returns
	// "true" when data was returned, and "false" when no data was returned.
	// The "non-blocking" function checks if there is any data in the receive
	// FIFO and does not "hang" if there isn't.
	while (MAP_SSIDataGetNonBlocking(SSIBase, &m_nDataRx[0])) {
	}
	m_bEmpty = true;
	// Enable the SSI interrupt
	SSIIntDisable(SSIBase, SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR);
	SSIIntClear(SSIBase, SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR);
	SSI3_CR1_R |= SSI_CR1_EOT; /* switch tx interrupt to eot int */
	SSIIntEnable(SSIBase, SSI_TXFF); /* SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR  */
	MAP_IntEnable(Interrupt);
}
//--------------------------------
void display::Terminate() {
	MAP_SSIDisable(SSIBase);
}
//--------------------------------
void display::Diag() {
	UARTprintf("SSI: %d/%d/%d\n", m_nEotCounter, m_nCounter,
			m_nOverflowCounter);
}
//--------------------------------
void display::OnInterrupt() {
	uint32_t nIntStatus = SSIIntStatus(SSIBase, false);
	SSIIntClear(SSIBase, nIntStatus);
//	if (SSI_TXFF & nIntStatus) {
	LoadFIFO();
	m_nEotCounter++;
//	}
	m_nCounter++;
}
//--------------------------------
void display::LoadFIFO() {
	int32_t nResult = 1;
	for (int nIndex = 0; nResult && (BufferSize > nIndex); nIndex++) {
		nResult = SSIDataPutNonBlocking(SSIBase, m_nDataTx[nIndex]);
	}
	if (!nResult) {
		m_nOverflowCounter++;
	}
	m_bEmpty = false;
}
//--------------------------------
uint32_t display::Digit2Segments(uint32_t nDigitNumber, uint32_t nDigitValue,
		bool bbDecimalPoint) {
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
void display::Set(int32_t nValue, int nDecimals) {
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
//--------------------------------nDigit
} /* namespace aeo1 */
//--------------------------------
