/*
 * drv8711.cpp
 *
 *  Created on: 24. mars 2015
 *      Author: Anders Emil Olsen
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "ssi_drv8711.h"
#include "pwm_stepper.h"
#include "drv8711.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
drv8711::drv8711() :
		m_oSsiDrv8711(), m_oPwmStepper() {
}
//--------------------------------
drv8711::~drv8711() {
}
//--------------------------------
void drv8711::Initialize() {
	m_oSsiDrv8711.Initialize();
	m_oPwmStepper.Initialize();
}
//--------------------------------
void drv8711::Idle() {
	uint32_t nValue = m_oSsiDrv8711.Read(0);
	nValue &= ~(0x101);
	m_oSsiDrv8711.Write(0, nValue);
	UARTprintf("Idle\n");
}
//--------------------------------
void drv8711::Halt() {
	uint32_t nValue = m_oSsiDrv8711.Read(0);
	nValue |= 0x101;
	m_oSsiDrv8711.Write(0, nValue);
	UARTprintf("Active\n");
}
//--------------------------------
void drv8711::Step() {
	uint32_t nValue = m_oSsiDrv8711.Read(0);
	nValue |= 0x004;
	m_oSsiDrv8711.Write(0, nValue);
	UARTprintf("Step\n");
}
//--------------------------------
void drv8711::Sleep(bool bSleep) {
	m_oSsiDrv8711.Sleep(bSleep);
	if (bSleep) {
		UARTprintf("Sleep\n");
	} else {
		UARTprintf("No sleep\n");
	}
}
//--------------------------------
void drv8711::ClearFaults() {
	m_oSsiDrv8711.Write(7, 0);
	UARTprintf("Clear faults\n");
}
//--------------------------------
void drv8711::Reset() {
	m_oSsiDrv8711.Reset();
	UARTprintf("Reset\n");
}
//--------------------------------
void drv8711::SetTorque(uint32_t nTorque) {
	uint32_t nValue = m_oSsiDrv8711.Read(0);
	nValue |= 0x100;
	m_oSsiDrv8711.Write(0, nValue);
	UARTprintf("Step\n");
}
//--------------------------------
void drv8711::Feed(int32_t nMicrosPerSecond) {
	UARTprintf("Feed: speed=%d(umps)\n", nMicrosPerSecond);
}
//--------------------------------
void drv8711::Move(int32_t nMicros) {
	UARTprintf("Move: length=%d(um)\n", nMicros);
}
//--------------------------------
void drv8711::Stop() {
	UARTprintf("Stop\n");
}
//--------------------------------
void drv8711::Diag() {
	ReadAllRegisters();
	UARTprintf("drv8711\n");
	m_oPwmStepper.Diag();
	m_oSsiDrv8711.Diag();
}
//--------------------------------
void drv8711::ReadAllRegisters() {
	for (int nRegister = 0; ssi_drv8711::NumberOfRegisters > nRegister;
			nRegister++) {
		m_oSsiDrv8711.Read(nRegister);
	}
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------

