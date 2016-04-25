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
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "ssi_drv8711.h"
#include "pwm_stepper.h"
#include "drv8711.h"
#include "drv8711_registers.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
struct drv8711_registerset {
	int nRegValue[8];
};
//--------------------------------
static const drv8711_registerset RegisterSet_Default = {

0x000, 0x0FF, 0x030, 0x080, 0x110, 0x040, 0x032, 0x000

};
//--------------------------------
static const drv8711_registerset RegisterSet_Alpha = {

0xF1C, 0x0BA, 0x030, 0x108, 0x510, 0xF40, 0x033, 0x000

};
//--------------------------------
static const drv8711_registerset RegisterSet_Guide = {

0xC11, //850 ns dead time, gain of 5, internal stall detect, 1/4 step, enable
		0x1A0, // 100-us sample
		0x032, // Internal indexer, 25 us off time
		0x100, // Adaptive blanking, 1-us current blanking
		0x510, // Use auto mixed decay, mixed decay time has no effect.
		0xA02, // BEMF/8, Stall after 2, approximately 20 mV (requires experimentation)
		0x000, // Minimal drive, minimum OCP deglitch and threshold
		0x000

};
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
void drv8711::SetDefault() {
	// Choose which register-set should be default
	const int* pRegValue = RegisterSet_Guide.nRegValue;
	//
	UARTprintf("drv8711::SetDefault\n");
	for (int nReg = 0; 8 > nReg; nReg++) {
		int nValue = *pRegValue++;
		m_oSsiDrv8711.Write(nReg, nValue);
		UARTprintf("Reg %d = 0x%03X\n", nReg, nValue);
	}
	//
	UARTprintf("\n");
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
	UARTprintf("Halt\n");
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
	SetDefault();
}
//--------------------------------
int drv8711::Get(const char* zName, char* zValue, int nSize) {
	int nStatus;
	uint32_t nFieldValue = 0;
	if (0 == strncmp(zName, "pwm", 3)) {
		nStatus = m_oPwmStepper.Get(zName, nFieldValue);
	} else {
		int nRegister = drv8711_registers_GetRegisterNumber(zName);
		if (0 <= nRegister) {
			uint32_t nRegisterValue = m_oSsiDrv8711.Read(nRegister);
			nStatus = drv8711_registers_GetFieldValue(zName, nRegisterValue,
					nFieldValue);
		} else {
			nStatus = No_Such_Attribute_Name;
		}
	}
	if (Success == nStatus) {
		usprintf(zValue, "%d", nFieldValue);
	}
	return nStatus;
}
//--------------------------------
int drv8711::Set(const char* zName, const char* zValue) {
	int nStatus;
	if (0 == strncmp(zName, "pwm", 3)) {
		int nValue = ustrtoul(zValue, 0, 10);
		nStatus = m_oPwmStepper.Set(zName, nValue);
	} else {
		int nRegister = drv8711_registers_GetRegisterNumber(zName);
		if (0 <= nRegister) {
			uint32_t nRegisterValue = m_oSsiDrv8711.Read(nRegister);
			nStatus = drv8711_registers_SetFieldValue(zName, nRegisterValue,
					zValue);
			if (Success == nStatus) {
				m_oSsiDrv8711.Write(nRegister, nRegisterValue);
			}
		} else {
			nStatus = No_Such_Attribute_Name;
		}
	}
	return nStatus;
}
//--------------------------------
void drv8711::Move(int32_t nSteps) {
	uint32_t nControlRegister = m_oSsiDrv8711.Read(0);
	if (0 > nSteps) {
		nSteps = -nSteps;
		nControlRegister &= ~(0x007);
		nControlRegister |= 0x005;
		UARTprintf("Reverse %d u-steps\n", nSteps);
	} else {
		nControlRegister |= 0x007;
		UARTprintf("Forward %d u-steps\n", nSteps);
	}
	m_oSsiDrv8711.Write(0, nControlRegister);
// nSteps *= 8; // Microstepping 1/8
	m_oPwmStepper.Move(nSteps);
}
//--------------------------------
void drv8711::Stop(bool bHard) {
	if (bHard) {
		UARTprintf("Hard Stop\n");
	} else {
		UARTprintf("Soft Stop\n");
	}
	m_oPwmStepper.Stop(bHard);
}
//--------------------------------
void drv8711::PrintStatus(uint32_t nStatus) {
	struct StatusEntry {
		uint32_t nMask;
		const char* zText;
	};
	const StatusEntry StatusTable[] = { { 0x001, "OverTemp" },
			{ 0x002, "A-amps" }, { 0x004, "B-amps" }, { 0x008, "A-fault" }, {
					0x010, "B-fault" }, { 0x020, "Low-V" }, { 0x040, "Stall" },
			{ 0x080, "Latched" }, { 0, "" } };
	const StatusEntry* pEntry = &StatusTable[0];
	while (pEntry) {
		uint32_t nMask = pEntry->nMask;
		if (!nMask) {
			pEntry = 0;
		} else {
			if (nMask & nStatus) {
				UARTprintf("%s ", pEntry->zText);
			}
			pEntry++;
		}
	}
	UARTprintf("\n");
}
//--------------------------------
void drv8711::ReadAllRegisters() {
	for (int nRegister = 0; ssi_drv8711::NumberOfRegisters > nRegister;
			nRegister++) {
		m_oSsiDrv8711.Read(nRegister);
	}
}
//--------------------------------
static bool MyPrintFunction(const char* zName, int nValue,
		const char* zDescription, void* pUserData) {
	UARTprintf("    %10s= %6d %40s\n", zName, nValue, zDescription);
	SysCtlDelay(SysCtlClockGet() / 500);
	return true;
}
//--------------------------------
void drv8711::PrintAllRegisters() {
	int nValue = 0xFFFF;
	for (int nRegister = 0; 8 > nRegister; nRegister++) {
		nValue = m_oSsiDrv8711.GetRegister(nRegister);
		UARTprintf("  Reg. %d      =  0x%03X\n", nRegister, nValue);
		drv8711_registers_Print(nRegister, nValue, MyPrintFunction, 0);
	}
	if (nValue) {
		UARTprintf("  -> ");
		PrintStatus(nValue);
	}
}
//--------------------------------
// nTorque = 255 & (256 * nIsGain * (nRsense * 1000) * nIfs / 2750 );
// nIfs = ( nTorque * 2750 ) / ( nIsGain * (nRsense * 1000))
void drv8711::PrintDerivedInfo() {
	const int nRsense = 50; // nRsense * 1000
	int nReg0 = m_oSsiDrv8711.GetRegister(0);
	int nReg1 = m_oSsiDrv8711.GetRegister(1);
	uint32_t nIsGain = 0;
	uint32_t nTorque = 0;
	drv8711_registers_GetFieldValue("isgain", nReg0, nIsGain);
	drv8711_registers_GetFieldValue("torque", nReg1, nTorque);
	if (nIsGain && nRsense) {
		int nIfs = (nTorque * 275000) / (nIsGain * nRsense);
		UARTprintf("Target full-scale current (Ifs) = %d.%02d A\n", nIfs / 100,
				nIfs % 2);
	}
}
//--------------------------------
void drv8711::Diag() {
	m_oSsiDrv8711.Diag();
	m_oPwmStepper.Diag();
	ReadAllRegisters();
	PrintAllRegisters();
	PrintDerivedInfo();
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------

