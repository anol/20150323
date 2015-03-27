/*
 * drv8711.cpp
 *
 *  Created on: 24. mars 2015
 *      Author: Anders
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
//--------------------------------
#include "ssi_drv8711.h"
#include "gpio_stepper.h"
#include "pwm_stepper.h"
#include "drv8711.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
//--------------------------------
drv8711::drv8711() :
		m_oSsiDrv8711(ssi_peripheral::SSI2), m_oGpioStepper(), m_oPwmStepper() {
}
//--------------------------------
drv8711::~drv8711() {
}
//--------------------------------
void drv8711::Initialize() {
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------

