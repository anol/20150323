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
#include "alpha7segment.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
ssi_display::ssi_display(ssi_peripheral::device_id nDevice) :
		ssi_peripheral(nDevice) {
}
//--------------------------------
ssi_display::~ssi_display() {
}
//--------------------------------
void ssi_display::Initialize() {
	ssi_peripheral::Initialize();
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
	if (IsEmpty()) {
		LoadTxFIFO();
	}
}
//--------------------------------
void ssi_display::Set(const char* zString) {
	for (int nGpc = BufferSize; 0 < nGpc; nGpc--) {
		int nSymbolNumber = nGpc-1;
		uint8_t nCode;
		if (zString && *zString) {
			char cSymbol = *zString++;
			nCode = Alpha7Segment(cSymbol);
			if ('.' == *zString) {
				zString++;
				nCode |= alpha7segment_dot;
			}
		} else {
			nCode = alpha7segment_space;
		}
		nCode = ~nCode;
		m_nDataTx[nSymbolNumber] = (nCode << 8) | (1 << nSymbolNumber);
	}
	if (IsEmpty()) {
		LoadTxFIFO();
	}
}
//--------------------------------
void ssi_display::Diag() {
	UARTprintf("ssi_display\n");
	ssi_peripheral::Diag();
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
