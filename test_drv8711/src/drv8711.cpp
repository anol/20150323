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
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "ssi_drv8711.h"
#include "pwm_stepper.h"
#include "drv8711.h"
/*
 DRV8711 Register Settings
 0: def=0xxxx, ref=0xF1C
 1: def=0x0FF, ref=0x0BA
 2: def=0x030, ref=0x030
 3: def=0x080, ref=0x108
 4: def=0x110, ref=0x510
 5: def=0x040, ref=0xF40
 6: def=0x032, ref=0x033
 7: def=0x000, ref=0x000
 */
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
	SysCtlDelay(10000);
	SetDefault();
}
//--------------------------------
void drv8711::Idle() {
	uint32_t nValue = m_oSsiDrv8711.Read(0);
	nValue &= ~(0x101);
	m_oSsiDrv8711.Write(0, nValue);
	UARTprintf("Idle\n");
}
//--------------------------------
void drv8711::SetDefault() {
	m_oSsiDrv8711.Write(0, 0xF18);
	m_oSsiDrv8711.Write(1, 0x0BA);
	m_oSsiDrv8711.Write(2, 0x030);
	m_oSsiDrv8711.Write(3, 0x108);
	m_oSsiDrv8711.Write(4, 0x310);
	m_oSsiDrv8711.Write(5, 0xF40);
	m_oSsiDrv8711.Write(6, 0x055);
	m_oSsiDrv8711.Write(7, 0x000);
	UARTprintf("SetDefault\n");
}
//--------------------------------
void drv8711::Halt() {
	uint32_t nValue = m_oSsiDrv8711.Read(0);
	nValue |= 0x101;
	m_oSsiDrv8711.Write(0, nValue);
	UARTprintf("Halt\n");
}
//--------------------------------
void drv8711::Step(uint32_t nSteps, bool bForward) {
	uint32_t nValue = m_oSsiDrv8711.Read(0);
	if (bForward) {
		nValue |= 0x007;
	} else {
		nValue &= ~(0x007);
		nValue |= 0x005;
	}
	for (int nStep = 0; nSteps > nStep; nStep++) {
		m_oSsiDrv8711.Write(0, nValue);
		SysCtlDelay(10000);
	}
	UARTprintf("%d steps\n");
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
	uint32_t nSteps = nMicros / 20;
	UARTprintf("Move %d um\n", nMicros);
	m_oPwmStepper.Move(nSteps);
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

