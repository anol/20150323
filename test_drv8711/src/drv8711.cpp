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
		m_oSsiDrv8711(ssi_peripheral::SSI2), m_oPwmStepper() {
}
//--------------------------------
drv8711::~drv8711() {
}
//--------------------------------
void drv8711::Initialize() {
	m_oSsiDrv8711.Initialize();
	m_oPwmStepper.Initialize();
	m_oSsiDrv8711.Read();
}
//--------------------------------
void drv8711::Idle() {
	UARTprintf("Idle\n");
}
//--------------------------------
void drv8711::Halt() {
	UARTprintf("Halt\n");
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
	m_oSsiDrv8711.Read();
	UARTprintf("drv8711\n");
	m_oPwmStepper.Diag();
	m_oSsiDrv8711.Diag();
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------

