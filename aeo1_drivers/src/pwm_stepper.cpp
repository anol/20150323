/*
 * pwm_stepper.cpp
 *
 *  Created on: 27. mar. 2015
 *      Author: anol
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "pwm_stepper.h"
//--------------------------------
static aeo1::pwm_stepper* g_pTheStepper = 0;
//--------------------------------
extern "C" void OnPWM1Gen1Interrupt(void) {
	PWMGenIntClear(PWM1_BASE, PWM_GEN_1, PWM_INT_CNT_LOAD);
	if (g_pTheStepper) {
		g_pTheStepper->OnInterrupt();
	}
}

namespace aeo1 {
//--------------------------------
pwm_stepper::pwm_stepper() :
		m_nSteps(0), m_nSpeed(Default_StartSpeed), m_nStartSpeed(
				Default_StartSpeed), m_nTargetSpeed(Default_TargetSpeed), m_nAcceleration(
				Default_Acceleration), m_nDeceleration(Default_Acceleration), m_nPhase(
				Phase_Idle) {
}
//--------------------------------
pwm_stepper::~pwm_stepper() {
}
//--------------------------------
void pwm_stepper::Initialize() {
	g_pTheStepper = this;
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA6_M1PWM2);
	GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_6);
	PWMGenConfigure(Base, Generator, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	// Set the PWM period to 250Hz.  To calculate the appropriate parameter
	// use the following equation: N = (1 / f) * SysClk.  Where N is the
	// function parameter, f is the desired frequency, and SysClk is the
	// system clock frequency.
	// In this case you get: (1 / 250Hz) * 16MHz = 64000 cycles.  Note that
	// the maximum period you can set is 2^16.
	PWMGenPeriodSet(Base, Generator, Default_StartSpeed);
	PWMPulseWidthSet(Base, PWM_OUT_2, 64);
	// Allow PWM1 generated interrupts.  This configuration is done to
	// differentiate fault interrupts from other PWM1 related interrupts.
	PWMIntEnable(Base, PWM_INT_GEN_1);
	// Enable the PWM1 LOAD interrupt on PWM1.
	PWMGenIntTrigEnable(Base, Generator, PWM_INT_CNT_LOAD);
	// Enable the PWM1 interrupts on the processor (NVIC).
	IntEnable(INT_PWM1_1);
	// Enable the PWM1 output signal (PA6).
//	PWMOutputInvert(Base, PWM_OUT_2_BIT, true);
	PWMOutputState(Base, PWM_OUT_2_BIT, true);
}
//--------------------------------
void pwm_stepper::OnInterrupt() {
	m_nSteps--;
	if (0 >= m_nSteps) {
		m_nSteps = 0;
		m_nPhase = Phase_Stop;
	}
	switch (m_nPhase) {
	case Phase_Accel:
		m_nSpeed -= m_nAcceleration;
		if (m_nTargetSpeed >= m_nSpeed) {
			m_nSpeed = m_nTargetSpeed;
			m_nPhase = Phase_Steady;
		}
		PWMGenPeriodSet(Base, Generator, m_nSpeed);
		PWMPulseWidthSet(Base, PWM_OUT_2, 64);
		PWMGenEnable(Base, Generator);
		break;
	case Phase_Steady:
		if (m_nSteps <= ((Default_StartSpeed - m_nSpeed) / m_nDeceleration)) {
			m_nPhase = Phase_Decel;
		}
		break;
	case Phase_Decel:
		m_nSpeed += m_nDeceleration;
		if (Default_StartSpeed <= m_nSpeed) {
			m_nSpeed = Default_StartSpeed;
			m_nPhase = Phase_Stop;
		}
		PWMGenPeriodSet(Base, Generator, m_nSpeed);
		PWMPulseWidthSet(Base, PWM_OUT_2, 64);
		PWMGenEnable(Base, Generator);
		break;
	case Phase_Stop:
		PWMGenDisable(Base, Generator);
		break;
	default:
		break;
	}
}
//--------------------------------
void pwm_stepper::Move(uint32_t nSteps) {
	m_nSteps = nSteps;
	m_nSpeed = m_nStartSpeed;
	m_nPhase = Phase_Accel;
	PWMGenPeriodSet(Base, Generator, m_nSpeed);
	PWMPulseWidthSet(Base, PWM_OUT_2, 64);
	PWMGenEnable(Base, Generator);
}
//--------------------------------
void pwm_stepper::Stop(bool bHard) {
	if (bHard) {
		PWMGenDisable(Base, Generator);
	} else {
		m_nSteps = ((Default_StartSpeed - m_nSpeed) / m_nDeceleration);
	}
}
//--------------------------------
uint32_t pwm_stepper::Get(const char* zName) {
	uint32_t nValue = 0;
	if (0 == strcmp(zName, "pwmstart")) {
		nValue = m_nStartSpeed;
	} else if (0 == strcmp(zName, "pwmtarget")) {
		nValue = m_nTargetSpeed;
	} else if (0 == strcmp(zName, "pwmaccel")) {
		nValue = m_nAcceleration;
	} else if (0 == strcmp(zName, "pwmdecel")) {
		nValue = m_nDeceleration;
	}
	return nValue;
}
//--------------------------------
int pwm_stepper::Get(const char* zName, uint32_t& rFieldValue) {
	uint32_t nValue = Get(zName);
	// Zero is illegal value for all attributes
	return nValue ? 0 : -2;
}
//--------------------------------
int pwm_stepper::Set(const char* zName, uint32_t nFieldValue) {
	int nStatus = 0;
	if (nFieldValue) {
		if (0 == strcmp(zName, "pwmstart")) {
			m_nStartSpeed = nFieldValue;
		} else if (0 == strcmp(zName, "pwmtarget")) {
			m_nTargetSpeed = nFieldValue;
		} else if (0 == strcmp(zName, "pwmaccel")) {
			m_nAcceleration = nFieldValue;
		} else if (0 == strcmp(zName, "pwmdecel")) {
			m_nDeceleration = nFieldValue;
		} else {
			nStatus = -2;
		}
	} else {
		nStatus = -3;
	}
	return nStatus;
}
//--------------------------------
void pwm_stepper::Diag() {
	UARTprintf("\npwm_stepper: Phase=%d, Steps=%d, Speed=%d\n", m_nPhase,
			m_nSteps, m_nSpeed);
	UARTprintf("    %10s= %6d %40s\n", "pwmstart", m_nStartSpeed,
			"Start speed pulse with");
	UARTprintf("    %10s= %6d %40s\n", "pwmtarget", m_nTargetSpeed,
			"Target speed pulse width");
	UARTprintf("    %10s= %6d %40s\n", "pwmaccel", m_nAcceleration,
			"Acceleration rate, -delta width");
	UARTprintf("    %10s= %6d %40s\n", "pwmdecel", m_nDeceleration,
			"Deceleration rate, +delta width");
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
