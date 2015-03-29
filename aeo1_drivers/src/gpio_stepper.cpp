/*
 * gpio_stepper.cpp
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
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "gpio_stepper.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
gpio_stepper::gpio_stepper() {
}
//--------------------------------
gpio_stepper::~gpio_stepper() {
}
//--------------------------------
void gpio_stepper::Diag() {
	UARTprintf("gpio_stepper\n");
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
