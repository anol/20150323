/*
 * qei_sensor.cpp
 *
 *  Created on: 23. mars 2015
 *      Author: Anders
 */
//--------------------------------
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/qei.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "qei_sensor.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
const uint32_t MaxPosition = 100000000;
//--------------------------------
qei_sensor::qei_sensor(device_id nDevice,
		configuration nConfig /* =DefaultConfiguration */) :
		m_nDevice(nDevice), m_ui32Base(
				(QEI0 == nDevice) ? QEI0_BASE : QEI1_BASE), m_nConfig(nConfig) {
}
//--------------------------------
qei_sensor::~qei_sensor() {

}
//--------------------------------
static void ConfigureQei0(uint32_t nConfig) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);

	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

	GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7,
	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA,
	GPIO_PIN_TYPE_STD_WPU);
	GPIOPinTypeQEI(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	GPIOPinTypeQEI(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPinConfigure(GPIO_PF4_IDX0);
	GPIOPinConfigure(GPIO_PD6_PHA0);
	GPIOPinConfigure(GPIO_PD7_PHB0);
	QEIConfigure(QEI0_BASE, nConfig, MaxPosition);
	QEIEnable(QEI0_BASE);
}
//--------------------------------
static void ConfigureQei1(uint32_t nConfig) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);
	GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6,
	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	GPIOPinConfigure(GPIO_PC4_IDX1);
	GPIOPinConfigure(GPIO_PC5_PHA1);
	GPIOPinConfigure(GPIO_PC6_PHB1);
	QEIConfigure(QEI1_BASE, nConfig, MaxPosition);
	QEIEnable(QEI1_BASE);
}
//--------------------------------
void qei_sensor::Initialize() {
	switch (m_nDevice) {
	case QEI0:
		ConfigureQei0(m_nConfig);
		break;
	case QEI1:
		ConfigureQei1(m_nConfig);
		break;
	default:
		break;
	}
}
//--------------------------------
bool qei_sensor::GetIndex() {
	switch (m_nDevice) {
	case QEI0:
		return GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) ? true : false;
	case QEI1:
		return GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4) ? true : false;
	default:
		return false;
	}
}
//--------------------------------
int32_t qei_sensor::Get() {
	uint32_t nPosition = QEIPositionGet(m_ui32Base);
	int32_t nValue;
	if ((MaxPosition / 2) > nPosition) {
		nValue = nPosition;
	} else {
		nValue = -((int32_t) (MaxPosition - nPosition));
	}
	return nValue;
}
//--------------------------------
void qei_sensor::Set(int32_t nValue) {
	uint32_t nPosition;
	if (0 > nValue) {
		nPosition = nValue;
	} else {
		nPosition = MaxPosition + nValue;
	}
	QEIPositionSet(m_ui32Base, nPosition);
}
//--------------------------------
void qei_sensor::Zero() {
	QEIPositionSet(m_ui32Base, 0);
}
//--------------------------------
void qei_sensor::Diag() {
	switch (m_nDevice) {
	case QEI0:
		UARTprintf("qei0: val=%d, pos=%d\n", Get(), QEIPositionGet(QEI0_BASE));
		break;
	case QEI1:
		UARTprintf("qei1: val=%d, pos=%d\n", Get(), QEIPositionGet(QEI1_BASE));
		break;
	default:
		UARTprintf("qei-void!\n");
		break;
	}

}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
