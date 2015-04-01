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
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "pwm_stepper.h"
//--------------------------------

namespace aeo1 {
//--------------------------------
pwm_stepper::pwm_stepper() {
}
//--------------------------------
pwm_stepper::~pwm_stepper() {
}
//--------------------------------
void pwm_stepper::Initialize() {
}
//--------------------------------
void pwm_stepper::Diag() {
	UARTprintf("pwm_stepper\n");
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
