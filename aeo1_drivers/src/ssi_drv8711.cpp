/*
 * ssi_drv8711.cpp
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
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
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "inc/tm4c123gh6pm.h"
//--------------------------------
#include "ssi_drv8711.h"
#include "alpha7segment.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
ssi_drv8711::ssi_drv8711(ssi_peripheral::device_id nDevice) :
		ssi_peripheral(nDevice) {
}
//--------------------------------
ssi_drv8711::~ssi_drv8711() {
}
//--------------------------------
void ssi_drv8711::Initialize() {
	ssi_peripheral::Initialize();
}
//--------------------------------
void ssi_drv8711::Diag() {
	UARTprintf("ssi_drv8711\n");
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
