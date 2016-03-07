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
void drv8711::SetDefault() {
	m_oSsiDrv8711.Write(0, 0xF18);  // Microstepping 1/8
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
	int nRegister = drv8711_registers_GetRegisterNumber(zName);
	if (0 <= nRegister) {
		uint32_t nRegisterValue = m_oSsiDrv8711.Read(nRegister);
		uint32_t nFieldValue = 0;
		nStatus = drv8711_registers_GetFieldValue(zName, nRegisterValue,
				nFieldValue);
		if (Success == nStatus) {
			usprintf(zValue, "%d", nFieldValue);
		}
	} else {
		nStatus = No_Such_Attribute_Name;
	}
	return nStatus;
}
//--------------------------------
int drv8711::Set(const char* zName, const char* zValue) {
	int nStatus;
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
	UARTprintf("    %8s= %4d %40s\n", zName, nValue, zDescription);
	SysCtlDelay(SysCtlClockGet() / 500);
	return true;
}
//--------------------------------
void drv8711::PrintAllRegisters() {
	int nValue = 0xFFFF;
	for (int nRegister = 0; 8 > nRegister; nRegister++) {
		nValue = m_oSsiDrv8711.GetRegister(nRegister);
		UARTprintf("  Reg. %d    =  %03X\n", nRegister, nValue);
		drv8711_registers_Print(nRegister, nValue, MyPrintFunction, 0);
	}
	if (nValue) {
		UARTprintf("  -> ");
		PrintStatus(nValue);
	}
}
//--------------------------------
void drv8711::Diag() {
	m_oSsiDrv8711.Diag();
	m_oPwmStepper.Diag();
	ReadAllRegisters();
	PrintAllRegisters();
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------

