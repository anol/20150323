/*
 * ain7.cpp
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
#include "inc/tm4c1233h6pm.h"
//--------------------------------
#include "ain7.h"
//--------------------------------
ain7* ain7::m_pTheOneAndOnly = 0;
//--------------------------------
ain7::ain7() :
		m_bUpdated(false), m_nSamples(0), m_nErrorCounter(0) {
}
//--------------------------------
ain7::~ain7() {
	if (this == m_pTheOneAndOnly) {
		m_pTheOneAndOnly = 0;
	}
	Terminate();
}
//--------------------------------
void ain7::Initialize() {
	m_pTheOneAndOnly = this;
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	MAP_GPIOPinTypeADC(ain7::GPIOBase, ain7::GPIOPin);
	ADCClockConfigSet(ain7::ADCBase, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF,
			1);
	MAP_ADCHardwareOversampleConfigure(ain7::ADCBase, ain7::Oversampling);
	MAP_ADCSequenceConfigure(ain7::ADCBase, ain7::Sequencer,
	ADC_TRIGGER_PROCESSOR, 0);
	MAP_ADCSequenceStepConfigure(ain7::ADCBase, ain7::Sequencer, 0,
	ADC_CTL_CH7 | ADC_CTL_END | ADC_CTL_IE);
	MAP_ADCSequenceEnable(ain7::ADCBase, ain7::Sequencer);
	MAP_ADCIntEnable(ain7::ADCBase, ain7::Sequencer);
	MAP_IntEnable(ain7::Interrupt);
}
//--------------------------------
void ain7::Terminate() {
	MAP_ADCSequenceDisable(ain7::ADCBase, ain7::Sequencer);
	MAP_ADCIntDisable(ain7::ADCBase, ain7::Sequencer);
	MAP_IntDisable(ain7::Interrupt);
}
//--------------------------------
void ain7::OnInterrupt() {
	uint32_t* pBuffer = &m_nSequence[0];
	uint32_t ulStatus = MAP_ADCIntStatus(ain7::ADCBase, ain7::Sequencer, true);
	MAP_ADCIntClear(ain7::ADCBase, ain7::Sequencer);
	m_nSamples = MAP_ADCSequenceDataGet(ain7::ADCBase, ain7::Sequencer,
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
	if (ain7::m_pTheOneAndOnly) {
		ain7::m_pTheOneAndOnly->OnInterrupt();
	}
}
//--------------------------------
void ain7::Trigger() {
	MAP_ADCProcessorTrigger(ain7::ADCBase, ain7::Sequencer);
}
//--------------------------------
uint32_t ain7::GetValue() {
	uint32_t nValue = m_nChannel[0];
	m_bUpdated = false;
	return nValue;
}
//--------------------------------
uint32_t ain7::GetErrorCounter() {
	uint32_t nValue = m_nErrorCounter;
	m_nErrorCounter = 0;
	return nValue;
}
//--------------------------------
