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
#include "ssi_drv8711.h"
#include "alpha7segment.h"
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
}
//--------------------------------
void ssi_drv8711::Read() {
	for (int nIndex = 0; BufferSize > nIndex; nIndex++) {
		m_nDataTx[nIndex] = 0x8000 | (nIndex << 12);
	}
	LoadTxFIFO();
}
//--------------------------------
void ssi_drv8711::OnRx() {
}
//--------------------------------
void ssi_drv8711::Diag() {
	ssi_peripheral::Diag();
	UARTprintf("ssi_drv8711: r_cnt=%d\n", m_nRxCount );
	for (int nIndex = 0; BufferSize > nIndex; nIndex++) {
		UARTprintf("\t%d=0x%04X\n", nIndex, m_nDataRx[nIndex]);
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
