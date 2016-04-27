/*
 * primary_activity.cpp
 *
 *  Created on: 26. apr. 2016
 *      Author: Anders
 */

//--------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_hibernate.h"
#include "inc/hw_gpio.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/qei.h"
//--------------------------------
#include "uartstdio.h"
#include "qei_sensor.h"
#include "ssi_display.h"
//--------------------------------
#include "primary_activity.h"
//--------------------------------
primary_activity::primary_activity() :
		m_oDisplay_X(aeo1::ssi_display::SSI1), m_oDisplay_Y(
				aeo1::ssi_display::SSI3), m_oScale_X(aeo1::qei_sensor::QEI0), m_oScale_Y(
				aeo1::qei_sensor::QEI1), m_nPressedCount_X(0), m_nPressedCount_Y(
				0) {
}
//--------------------------------
primary_activity::~primary_activity() {
}
//--------------------------------
static void SetupInput() {
	// PB3, X-Button
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3,
	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	// PC4, IDX1, Y-Index
	// PC7, Y-Button
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_7);
	GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_7,
	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	// PF4, IDX0, X-Index
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4,
	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}
//--------------------------------
static bool GetButton_X() {
	return GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_3) ? false : true;
}
//--------------------------------
static bool GetButton_Y() {
	return GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7) ? false : true;
}
//--------------------------------
static bool GetIndex_X() {
	return GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) ? true : false;
}
//--------------------------------
static bool GetIndex_Y() {
	return GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4) ? true : false;
}
//--------------------------------
void primary_activity::Initialize() {
	SetupInput();
	m_oDisplay_X.Initialize();
	m_oDisplay_Y.Initialize();
	m_oScale_X.Initialize();
	m_oScale_Y.Initialize();
}
//--------------------------------
bool primary_activity::CheckInputs(int& rCount, bool bButton,
		bool bIndex) const {
	enum {
		MinimumPressedCount = 5,
		MaximumShortPressed = 32,
		MinimumLongPressed = 100
	};
	bool bZero = false;
	int nTempCount = rCount;
	if (bButton) {
		nTempCount++;
		if (MinimumLongPressed < nTempCount) { // This is a press and hold
			if (bIndex) { // The scale is at an index point
				nTempCount = 0;
				bZero = true;
			}
		}
	} else if ((MinimumPressedCount < nTempCount)
			&& (MaximumShortPressed > nTempCount)) { // This is a push and release
		nTempCount = 0;
		bZero = true;
	} else {
		nTempCount = 0;
	}
	// Return both results
	rCount = nTempCount;
	return bZero;
}
//--------------------------------
void primary_activity::OnTick() {
	// Check the inputs
	if (CheckInputs(m_nPressedCount_X, GetButton_X(), GetIndex_X())) {
		m_oScale_X.Zero();
	}
	if (CheckInputs(m_nPressedCount_Y, GetButton_Y(), GetIndex_Y())) {
		m_oScale_Y.Zero();
	}
	// Update the displays
	m_oDisplay_X.Set(m_oScale_X.Get());
	m_oDisplay_Y.Set(m_oScale_Y.Get());
}
//--------------------------------
void primary_activity::Diag() {
	UARTprintf("X: button=%5s, index=%5s, pressed=%d\n",
			GetButton_X() ? "true" : "false", GetIndex_X() ? "true" : "false",
			m_nPressedCount_X);
	UARTprintf("Y: button=%5s, index=%5s, pressed=%d\n",
			GetButton_Y() ? "true" : "false", GetIndex_Y() ? "true" : "false",
			m_nPressedCount_Y);
	m_oDisplay_X.Diag();
	m_oDisplay_Y.Diag();
	m_oScale_X.Diag();
	m_oScale_Y.Diag();
}
//--------------------------------
void primary_activity::SetX(int nValue) {
	m_oDisplay_X.Set(nValue, 2);
	m_oScale_X.Set(nValue);
}
//--------------------------------
void primary_activity::SetY(int nValue) {
	m_oDisplay_Y.Set(nValue, 2);
	m_oScale_Y.Set(nValue);
}
//--------------------------------
