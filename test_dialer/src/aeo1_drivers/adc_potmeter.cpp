/*
 * potmeter.cpp
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */
//--------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inc/hw_memmap.h>
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"
//--------------------------------
// Stellaris lm4f120h5qr is Tiva tm4c1233h6pm
//#include "inc/tm4c1233h6pm.h"
#include "inc/tm4c123gh6pm.h"
//--------------------------------
#include "adc_potmeter.h"
//--------------------------------
extern "C" void OnADCInterrupt(void) {
	if (aeo1::adc_potmeter::m_pTheOneAndOnly) {
		aeo1::adc_potmeter::m_pTheOneAndOnly->OnInterrupt();
	}
}
//--------------------------------
namespace aeo1 {
//--------------------------------
adc_potmeter* adc_potmeter::m_pTheOneAndOnly = 0;
uint32_t adc_potmeter::m_nCounter = 0;
uint32_t adc_potmeter::m_nErrorCounter = 0;
uint32_t adc_potmeter::m_nReadCounter = 0;
//--------------------------------
adc_potmeter::adc_potmeter() :
		m_bUpdated(false), m_nSamples(0)  {
}
//--------------------------------
adc_potmeter::~adc_potmeter() {
	if (this == m_pTheOneAndOnly) {
		m_pTheOneAndOnly = 0;
	}
	Terminate();
}
//--------------------------------
void adc_potmeter::Initialize() {
	m_pTheOneAndOnly = this;
	MAP_SysCtlPeripheralEnable(GPIOPeripheral);
	MAP_SysCtlPeripheralEnable(ADCPeripheral);
	MAP_GPIOPinTypeADC(adc_potmeter::GPIOBase, adc_potmeter::GPIOPin);
	ADCClockConfigSet(adc_potmeter::ADCBase,
	ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF, 1);
	MAP_ADCHardwareOversampleConfigure(adc_potmeter::ADCBase,
			adc_potmeter::Oversampling);
	MAP_ADCSequenceConfigure(adc_potmeter::ADCBase, adc_potmeter::Sequencer,
	ADC_TRIGGER_PROCESSOR, 0);
	MAP_ADCSequenceStepConfigure(adc_potmeter::ADCBase, adc_potmeter::Sequencer, 0,
			ADCChannel | ADC_CTL_END | ADC_CTL_IE);
	MAP_ADCSequenceEnable(adc_potmeter::ADCBase, adc_potmeter::Sequencer);
	MAP_ADCIntEnable(adc_potmeter::ADCBase, adc_potmeter::Sequencer);
	MAP_IntEnable(adc_potmeter::Interrupt);
}
//--------------------------------
void adc_potmeter::Terminate() {
	MAP_ADCSequenceDisable(adc_potmeter::ADCBase, adc_potmeter::Sequencer);
	MAP_ADCIntDisable(adc_potmeter::ADCBase, adc_potmeter::Sequencer);
	MAP_IntDisable(adc_potmeter::Interrupt);
}
//--------------------------------
void adc_potmeter::Diag() {
	UARTprintf("POT: %d/%d/%d\n", m_nErrorCounter, m_nReadCounter, m_nCounter );
}
//--------------------------------
void adc_potmeter::OnInterrupt() {
	uint32_t* pBuffer = &m_nSequence[0];
	uint32_t ulStatus = MAP_ADCIntStatus(adc_potmeter::ADCBase, adc_potmeter::Sequencer,
			true);
	m_nCounter++;
	MAP_ADCIntClear(adc_potmeter::ADCBase, adc_potmeter::Sequencer);
	m_nSamples = MAP_ADCSequenceDataGet(adc_potmeter::ADCBase, adc_potmeter::Sequencer,
			pBuffer);
	if (0 < m_nSamples) {
		for (int nChannel = 0; m_nSamples > nChannel; nChannel++) {
			m_nChannel[nChannel] = m_nSequence[nChannel];
		}
		m_bUpdated = true;
	} else {
		m_nErrorCounter++;
	}
}
//--------------------------------
void adc_potmeter::Trigger() {
	MAP_ADCProcessorTrigger(adc_potmeter::ADCBase, adc_potmeter::Sequencer);
}
//--------------------------------
uint32_t adc_potmeter::GetValue() {
	uint32_t nValue = m_nChannel[0];
	m_bUpdated = false;
	m_nReadCounter++;
	return nValue;
}
//--------------------------------
uint32_t adc_potmeter::GetErrorCounter() {
	uint32_t nValue = m_nErrorCounter;
	m_nErrorCounter = 0;
	return nValue;
}
//--------------------------------
} /* namespace aeo1 */
