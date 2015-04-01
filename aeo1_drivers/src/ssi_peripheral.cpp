/*
 * ssi_peripheral.cpp
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "ssi_peripheral.h"
//--------------------------------
static aeo1::ssi_peripheral* g_pTheSSI0 = 0;
static aeo1::ssi_peripheral* g_pTheSSI1 = 0;
static aeo1::ssi_peripheral* g_pTheSSI2 = 0;
static aeo1::ssi_peripheral* g_pTheSSI3 = 0;
//--------------------------------
extern "C" void OnSSI0Interrupt(void) {
	if (g_pTheSSI0) {
		g_pTheSSI0->OnInterrupt();
	}
}
//--------------------------------
extern "C" void OnSSI1Interrupt(void) {
	if (g_pTheSSI1) {
		g_pTheSSI1->OnInterrupt();
	}
}
//--------------------------------
extern "C" void OnSSI2Interrupt(void) {
	if (g_pTheSSI2) {
		g_pTheSSI2->OnInterrupt();
	}
}
//--------------------------------
extern "C" void OnSSI3Interrupt(void) {
	if (g_pTheSSI3) {
		g_pTheSSI3->OnInterrupt();
	}
}
//--------------------------------
namespace aeo1 {
//--------------------------------
ssi_specification SSI0_Specification

= { SYSCTL_PERIPH_GPIOA, // =	GPIOPeripheral
		GPIO_PORTA_BASE, // = GPIOBase
		GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, // = GPIOPins
		SYSCTL_PERIPH_SSI0, // = SSIPeripheral
		SSI0_BASE, // = SSIBase
		GPIO_PA2_SSI0CLK, // = SSIPinClk
		GPIO_PA3_SSI0FSS, // = SSIPinFss
		GPIO_PA4_SSI0RX, // = SSIPinRx
		GPIO_PA5_SSI0TX, // = SSIPinTx
		0x40008004, //SSI0_CR1_R, // = SSI_CR0_R
		INT_SSI0 // =	Interrupt
		};
//--------------------------------
ssi_specification SSI1_Specification

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
ssi_specification SSI2_Specification

= { SYSCTL_PERIPH_GPIOB, // =	GPIOPeripheral
		GPIO_PORTB_BASE, // = GPIOBase
		GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, // = GPIOPins
		SYSCTL_PERIPH_SSI2, // = SSIPeripheral
		SSI2_BASE, // = SSIBase
		GPIO_PB4_SSI2CLK, // = SSIPinClk
		GPIO_PB5_SSI2FSS, // = SSIPinFss
		GPIO_PB6_SSI2RX, // = SSIPinRx
		GPIO_PB7_SSI2TX, // = SSIPinTx
		0x4000A004, //SSI2_CR1_R, // = SSI_CR1_R
		INT_SSI2 // =	Interrupt
		};

//--------------------------------
ssi_specification SSI3_Specification

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
ssi_peripheral::ssi_peripheral(device_id nDevice) :
		// Use the Right SSI Peripheral Setup
		m_rSpecification(
				(ssi_peripheral::SSI0 == nDevice) ?
						SSI0_Specification :
						((ssi_peripheral::SSI1 == nDevice) ?
								SSI1_Specification :
								((ssi_peripheral::SSI2 == nDevice) ?
										SSI2_Specification : SSI3_Specification))),
		// Other members
		m_nDevice(nDevice), m_nSRTFE(0), m_nTXEOT(0), m_nDMATX(0), m_nDMARX(0), m_nTXFF(
				0), m_nRXFF(0), m_nRXTO(0), m_nRXOR(0), m_bEmpty(false), m_nRxCount(
				0) {
	memset(m_nDataTx, 0, sizeof(m_nDataTx));
	memset(m_nDataRx, 0, sizeof(m_nDataRx));
}
//--------------------------------
ssi_peripheral::~ssi_peripheral() {
	Terminate();
}
//--------------------------------
void ssi_peripheral::Initialize() {
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
	case ssi_peripheral::SSI0:
		g_pTheSSI0 = this;
		break;
	case ssi_peripheral::SSI1:
		g_pTheSSI1 = this;
		break;
	case ssi_peripheral::SSI2:
		g_pTheSSI2 = this;
		break;
	case ssi_peripheral::SSI3:
		g_pTheSSI3 = this;
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
void ssi_peripheral::Terminate() {
	switch (m_nDevice) {
	case ssi_peripheral::SSI0:
		g_pTheSSI0 = 0;
		break;
	case ssi_peripheral::SSI1:
		g_pTheSSI1 = 0;
		break;
	case ssi_peripheral::SSI2:
		g_pTheSSI2 = 0;
		break;
	case ssi_peripheral::SSI3:
		g_pTheSSI3 = 0;
	default:
		break;
	}
	SSIIntDisable(m_rSpecification.m_nSSIBase, SSI_TXFF); /* SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR  */
	MAP_IntDisable(m_rSpecification.m_nInterrupt);
	MAP_SSIDisable(m_rSpecification.m_nSSIBase);
}
//--------------------------------
void ssi_peripheral::OnInterrupt() {
	uint32_t nIntStatus = SSIIntStatus(m_rSpecification.m_nSSIBase, false);
	uint32_t nSsiStatus = (HWREG(m_rSpecification.m_nSSIBase + SSI_O_SR));
	SSIIntClear(m_rSpecification.m_nSSIBase, nIntStatus);
	if (SSI_SR_TFE & nSsiStatus) { // SSI Transmit FIFO Empty (status)
		m_nSRTFE++;
		OnTx();
	}
	if ( SSI_TXEOT & nIntStatus) { // Transmit FIFO is empty (interrupt)
		m_nTXEOT++;
	}
	if ( SSI_DMATX & nIntStatus) { // DMA Transmit complete
		m_nDMATX++;
	}
	if ( SSI_DMARX & nIntStatus) { // DMA Receive complete
		m_nTXFF++;
	}
	if ( SSI_TXFF & nIntStatus) { // TX FIFO half full or less
		m_nTXFF++;
	}
	if ( SSI_RXFF & nIntStatus) { // RX FIFO half full or more
		m_nRXFF++;
		UnloadRxFIFO();
	}
	if ( SSI_RXTO & nIntStatus) {  // RX timeout
		m_nRXTO++;
	}
	if ( SSI_RXOR & nIntStatus) {  // RX overrun
		m_nRXOR++;
	}
}
//--------------------------------
void ssi_peripheral::Diag() {
	switch (m_nDevice) {
	case ssi_peripheral::SSI0:
		UARTprintf("\tssi0\n");
		break;
	case ssi_peripheral::SSI1:
		UARTprintf("\tssi1\n");
		break;
	case ssi_peripheral::SSI2:
		UARTprintf("\tssi2\n");
		break;
	case ssi_peripheral::SSI3:
		UARTprintf("\tssi3\n");
		break;
	default:
		UARTprintf("\tssi-void!\n");
		break;
	}
	UARTprintf("\tSRTFE=%5d\n", m_nSRTFE);
	UARTprintf("\tTXEOT=%5d\n", m_nTXEOT);
	UARTprintf("\tDMATX=%5d\n", m_nDMATX);
	UARTprintf("\tDMARX=%5d\n", m_nDMARX);
	UARTprintf("\tTXFF= %5d\n", m_nTXFF);
	UARTprintf("\tRXFF= %5d\n", m_nRXFF);
	UARTprintf("\tRXTO= %5d\n", m_nRXTO);
	UARTprintf("\tRXOR= %5d\n", m_nRXOR);
}
//--------------------------------
void ssi_peripheral::LoadTxFIFO() {
	int32_t nResult = 1;
	for (int nIndex = 0; nResult && (BufferSize > nIndex); nIndex++) {
		nResult = SSIDataPutNonBlocking(m_rSpecification.m_nSSIBase,
				m_nDataTx[nIndex]);
	}
}
//--------------------------------
void ssi_peripheral::UnloadRxFIFO() {
	int32_t nResult = 1;
	m_nRxCount = 0;
	for (int nIndex = 0;
			nResult && (BufferSize > nIndex) && (BufferSize > m_nRxCount);
			nIndex++) {
		nResult = SSIDataGetNonBlocking(m_rSpecification.m_nSSIBase,
				&m_nDataRx[m_nRxCount]);
		m_nRxCount += nResult;
	}
	if (m_nRxCount) {
		OnRx();
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
