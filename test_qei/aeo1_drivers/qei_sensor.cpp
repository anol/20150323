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
qei_sensor::qei_sensor(device_id nDevice) :
		m_nDevice(nDevice) {
}
//--------------------------------
qei_sensor::~qei_sensor() {
}
//--------------------------------
static void ConfigureQei0() {
	const uint32_t ui32Config = (QEI_CONFIG_CAPTURE_A | QEI_CONFIG_RESET_IDX
			| QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP);
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
	QEIConfigure(QEI0_BASE, ui32Config, MaxPosition);
	QEIEnable(QEI0_BASE);
}
//--------------------------------
static void ConfigureQei1() {
	const uint32_t ui32Config = (QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET
			| QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);
	GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6,
	GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	GPIOPinConfigure(GPIO_PC4_IDX1);
	GPIOPinConfigure(GPIO_PC5_PHA1);
	GPIOPinConfigure(GPIO_PC6_PHB1);
	QEIConfigure(QEI1_BASE, ui32Config, MaxPosition);
	QEIEnable(QEI1_BASE);
}
//--------------------------------
void qei_sensor::Initialize() {
	switch (m_nDevice) {
	case QEI0:
		ConfigureQei0();
		break;
	case QEI1:
		ConfigureQei1();
		break;
	default:
		break;
	}
}
//--------------------------------
static int32_t GetValue(uint32_t ui32Base) {
	uint32_t nPostion = QEIPositionGet(ui32Base);
	int32_t nValue;
	if ((MaxPosition / 2) > nPostion) {
		nValue = nPostion;
	} else {
		nValue = -((int32_t) (MaxPosition - nPostion));
	}
	nValue /= 2;
	return nValue;
}
//--------------------------------
int32_t qei_sensor::Get() {
	switch (m_nDevice) {
	case QEI0:
		return GetValue(QEI0_BASE);
	case QEI1:
		return GetValue(QEI1_BASE);
	default:
		return 0;
	}
}
//--------------------------------
void qei_sensor::Diag() {
	UARTprintf("QEI: 0=%d(%d), 1=%d(%d)\n", GetValue(QEI0_BASE),
			QEIPositionGet(QEI0_BASE), GetValue(QEI1_BASE),
			QEIPositionGet(QEI1_BASE));
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
