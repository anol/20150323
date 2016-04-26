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
#include "qei_sensor.h"
#include "ssi_display.h"
#include "primary_activity.h"
//--------------------------------
primary_activity::primary_activity() :
		m_oDisplayX(aeo1::ssi_display::SSI1), m_oDisplayY(
				aeo1::ssi_display::SSI3), m_oScaleX(aeo1::qei_sensor::QEI0), m_oScaleY(
				aeo1::qei_sensor::QEI1) {
}
//--------------------------------
primary_activity::~primary_activity() {
}
//--------------------------------
void primary_activity::Initialize() {
	m_oDisplayX.Initialize();
	m_oDisplayY.Initialize();
	m_oScaleX.Initialize();
	m_oScaleY.Initialize();
}
//--------------------------------
void primary_activity::OnTick() {
	m_oDisplayX.Set(m_oScaleX.Get());
	m_oDisplayY.Set(m_oScaleY.Get());
}
//--------------------------------
void primary_activity::Diag() {
	m_oDisplayX.Diag();
	m_oDisplayY.Diag();
	m_oScaleX.Diag();
	m_oScaleY.Diag();
}
//--------------------------------
void primary_activity::SetX(int nValue) {
	m_oDisplayX.Set(nValue, 2);
	m_oScaleX.Set(nValue);
}
//--------------------------------
void primary_activity::SetY(int nValue) {
	m_oDisplayY.Set(nValue, 2);
	m_oScaleY.Set(nValue);
}
//--------------------------------
