/*
 * potmeter.cpp
 *
 *  Created on: 14. nov. 2014
 *      Author: Anders
 */

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
//--------------------------------
// Stellaris lm4f120h5qr is Tiva tm4c1233h6pm
//#include "inc/tm4c1233h6pm.h"
#include "inc/tm4c123gh6pm.h"
//--------------------------------
#include "potmeter.h"
//--------------------------------
potmeter* potmeter::m_pTheOneAndOnly = 0;
//--------------------------------
potmeter::potmeter() :
		m_bUpdated(false), m_nSamples(0), m_nErrorCounter(0) {
}
//--------------------------------
potmeter::~potmeter() {
	if (this == m_pTheOneAndOnly) {
		m_pTheOneAndOnly = 0;
	}
	Terminate();
}
//--------------------------------
void potmeter::Initialize() {
	m_pTheOneAndOnly = this;
	MAP_SysCtlPeripheralEnable(GPIOPeripheral);
	MAP_SysCtlPeripheralEnable(ADCPeripheral);
	MAP_GPIOPinTypeADC(potmeter::GPIOBase, potmeter::GPIOPin);
	ADCClockConfigSet(potmeter::ADCBase,
	ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF, 1);
	MAP_ADCHardwareOversampleConfigure(potmeter::ADCBase,
			potmeter::Oversampling);
	MAP_ADCSequenceConfigure(potmeter::ADCBase, potmeter::Sequencer,
	ADC_TRIGGER_PROCESSOR, 0);
	MAP_ADCSequenceStepConfigure(potmeter::ADCBase, potmeter::Sequencer, 0,
			ADCChannel | ADC_CTL_END | ADC_CTL_IE);
	MAP_ADCSequenceEnable(potmeter::ADCBase, potmeter::Sequencer);
	MAP_ADCIntEnable(potmeter::ADCBase, potmeter::Sequencer);
	MAP_IntEnable(potmeter::Interrupt);
}
//--------------------------------
void potmeter::Terminate() {
	MAP_ADCSequenceDisable(potmeter::ADCBase, potmeter::Sequencer);
	MAP_ADCIntDisable(potmeter::ADCBase, potmeter::Sequencer);
	MAP_IntDisable(potmeter::Interrupt);
}
//--------------------------------
void potmeter::OnInterrupt() {
	uint32_t* pBuffer = &m_nSequence[0];
	uint32_t ulStatus = MAP_ADCIntStatus(potmeter::ADCBase, potmeter::Sequencer,
			true);
	MAP_ADCIntClear(potmeter::ADCBase, potmeter::Sequencer);
	m_nSamples = MAP_ADCSequenceDataGet(potmeter::ADCBase, potmeter::Sequencer,
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
extern "C" void OnADCInterrupt(void) {
	if (potmeter::m_pTheOneAndOnly) {
		potmeter::m_pTheOneAndOnly->OnInterrupt();
	}
}
//--------------------------------
void potmeter::Trigger() {
	MAP_ADCProcessorTrigger(potmeter::ADCBase, potmeter::Sequencer);
}
//--------------------------------
uint32_t potmeter::GetValue() {
	uint32_t nValue = m_nChannel[0];
	m_bUpdated = false;
	return nValue;
}
//--------------------------------
uint32_t potmeter::GetErrorCounter() {
	uint32_t nValue = m_nErrorCounter;
	m_nErrorCounter = 0;
	return nValue;
}
//--------------------------------
