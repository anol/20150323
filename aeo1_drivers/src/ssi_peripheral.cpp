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
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_ssi.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
//--------------------------------
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
		GPIO_PIN_4, // = m_nGPIOInputPin
		GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5, // = m_nGPIOOutputPins
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
		GPIO_PIN_0, // = m_nGPIOInputPin
		GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, // = m_nGPIOOutputPins
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
		GPIO_PIN_6, // = m_nGPIOInputPin
		GPIO_PIN_4 | GPIO_PIN_7, // = m_nGPIOOutputPins
		SYSCTL_PERIPH_SSI2, // = SSIPeripheral
		SSI2_BASE, // = SSIBase
		GPIO_PB4_SSI2CLK, // = SSIPinClk
		0, //GPIO_PB5_SSI2FSS, // = SSIPinFss
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
		GPIO_PIN_2, // = m_nGPIOInputPin
		GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3, // = m_nGPIOOutputPins
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
ssi_peripheral::ssi_peripheral(device_id nDevice, uint32_t nBitRate,
		bool bNonBlocking /*=true*/) :
		// Use the Right SSI Peripheral Setup
		m_rSpecification(
				(ssi_peripheral::SSI0 == nDevice) ?
						SSI0_Specification :
						((ssi_peripheral::SSI1 == nDevice) ?
								SSI1_Specification :
								((ssi_peripheral::SSI2 == nDevice) ?
										SSI2_Specification : SSI3_Specification))),
		// Other members
		m_nDevice(nDevice), m_nBitRate(nBitRate), m_nSRTFE(0), m_nTXFF(0), m_nRXFF(
				0), m_nRXTO(0), m_nRXOR(0), m_bEmpty(false), m_bNonBlocking(
				bNonBlocking), m_nRxCount(0) {
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
	// Assign the SSI signals to the appropriate pins
	MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinRx);
	MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinClk);
	MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinTx);
	if (m_rSpecification.m_nSSIPinFss) {
		MAP_GPIOPinConfigure(m_rSpecification.m_nSSIPinFss);
	}
	// Set the GPIO AFSEL bits for the appropriate pins
	MAP_GPIOPinTypeSSI(m_rSpecification.m_nGPIOBase,
			m_rSpecification.m_nGPIOPins);
	// Set pull-up on the SSI Rx pin
	GPIOPadConfigSet(m_rSpecification.m_nGPIOBase,
			m_rSpecification.m_nGPIOInputPin, GPIO_STRENGTH_2MA,
			GPIO_PIN_TYPE_STD_WPU);
	// Set standrad on the SSI output pins
	GPIOPadConfigSet(m_rSpecification.m_nGPIOBase,
			m_rSpecification.m_nGPIOOutputPins, GPIO_STRENGTH_2MA,
			GPIO_PIN_TYPE_STD);
	// Configure the SSI peripheral
	SSIConfigSetExpClk(m_rSpecification.m_nSSIBase, SysCtlClockGet(),
	SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, m_nBitRate, 16);
	// Enable the SSI module.
	MAP_SSIEnable(m_rSpecification.m_nSSIBase);
	// Read any residual data from the SSI port.
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
	if (m_bNonBlocking) {
		SSIIntEnable(m_rSpecification.m_nSSIBase, SSI_TXFF); /* SSI_TXFF | SSI_RXFF | SSI_RXTO | SSI_RXOR  */
		MAP_IntEnable(m_rSpecification.m_nInterrupt);
	}
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
		UARTprintf("SSI0, ");
		break;
	case ssi_peripheral::SSI1:
		UARTprintf("SSI1, ");
		break;
	case ssi_peripheral::SSI2:
		UARTprintf("SSI2, ");
		break;
	case ssi_peripheral::SSI3:
		UARTprintf("SSI3, ");
		break;
	default:
		UARTprintf("ssi-void! ");
		break;
	}
	if (m_bNonBlocking) {
		UARTprintf("NonBlocking, ");
	} else {
		UARTprintf("Blocking, ");
	}
	UARTprintf("\n  SRTFE=%d, ", m_nSRTFE);
	UARTprintf("TXFF=%d, ", m_nTXFF);
	UARTprintf("RXFF=%d, ", m_nRXFF);
	UARTprintf("RXTO=%d, ", m_nRXTO);
	UARTprintf("RXOR=%d\n", m_nRXOR);
}
//--------------------------------
void ssi_peripheral::Put(uint32_t nValue) {
	SSIDataPut(m_rSpecification.m_nSSIBase, nValue);
}
//--------------------------------
uint32_t ssi_peripheral::Get() {
	uint32_t nValue = 0;
	SSIDataGet(m_rSpecification.m_nSSIBase, &nValue);
	return nValue;
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
	for (int nIndex = 0; nResult && (BufferSize > nIndex); nIndex++) {
		nResult = SSIDataGetNonBlocking(m_rSpecification.m_nSSIBase,
				&m_nDataRx[nIndex]);
		m_nRxCount += nResult;
	}
	if (m_nRxCount) {
		OnRx();
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
