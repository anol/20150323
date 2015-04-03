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
		m_nSteps(0) {
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
	PWMGenConfigure(Base, Generator,
	PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	// Set the PWM period to 250Hz.  To calculate the appropriate parameter
	// use the following equation: N = (1 / f) * SysClk.  Where N is the
	// function parameter, f is the desired frequency, and SysClk is the
	// system clock frequency.
	// In this case you get: (1 / 250Hz) * 16MHz = 64000 cycles.  Note that
	// the maximum period you can set is 2^16.
	PWMGenPeriodSet(Base, Generator, 64000);
	PWMPulseWidthSet(Base, PWM_OUT_2, 64);
	// Allow PWM1 generated interrupts.  This configuration is done to
	// differentiate fault interrupts from other PWM1 related interrupts.
	PWMIntEnable(Base, PWM_INT_GEN_1);
	// Enable the PWM1 LOAD interrupt on PWM1.
	PWMGenIntTrigEnable(Base, Generator, PWM_INT_CNT_LOAD);
	// Enable the PWM1 interrupts on the processor (NVIC).
	IntEnable(INT_PWM1_1);
	// Enable the PWM1 output signal (PA6).
	PWMOutputState(Base, PWM_OUT_2_BIT, true);
}
//--------------------------------
void pwm_stepper::OnInterrupt() {
	if (0 < m_nSteps) {
		m_nSteps--;
		if (0 == m_nSteps) {
			PWMGenDisable(Base, Generator);
		}
	}
}
//--------------------------------
void pwm_stepper::Move(uint32_t nSteps) {
	m_nSteps = nSteps;
	UARTprintf("Move %d steps\n", nSteps);
	// Enables the PWM generator block.
	PWMGenEnable(Base, Generator);
}
//--------------------------------
void pwm_stepper::Diag() {
	UARTprintf("pwm_stepper\n");
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
