/*
 * esp8266.cpp
 *
 *  Created on: 24. mars 2015
 *      Author: Anders Emil Olsen
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
//--------------------------------
#include "esp8266.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
struct ESP8266_Command {
	const char* zCommand;
	const char* zResult;
};
//--------------------------------
const ESP8266_Command InitializationCommands[] = {

// Test for ready
		{ "AT", "OK" },
// Quit access point
		{ "AT+CWQAP", "OK" },
// List access points
//		{ "AT+CWLAP", 0 },
// Join access point
		{ "AT+CWJAP=\"Offline\",\"Unsoldered7\"", "OK" },
// Get interface IP address
//		{ "AT+CIFSR", 0 },
// Enable multiple connections
		{ "AT+CIPMUX=1", "OK" },
// Configure as TCP server (default port = 333)
		{ "AT+CIPSERVER=1", "OK" },
// End of table
		{ 0, 0 }

};
//--------------------------------
esp8266::esp8266() {
}
//--------------------------------
esp8266::~esp8266() {
}
//--------------------------------
int esp8266::Initialize() {
	int nCommandIndex = 0;
	UARTEchoSet(false);
	while (InitializationCommands[nCommandIndex].zCommand) {
		if (Invoke(InitializationCommands[nCommandIndex].zCommand,
				InitializationCommands[nCommandIndex].zResult)) {
			nCommandIndex++;
		} else {
			break;
		}
	}
	UARTEchoSet(true);
	return nCommandIndex;
}
//--------------------------------
bool esp8266::Invoke(const char* zCommand, const char* zResult) {
	bool bSuccess = true;
	UARTprintf("<%s>\r\n", zCommand);
	// Get response
	while (UARTPeek('\r') == -1) {
		SysCtlDelay(SysCtlClockGet() / (1000 / 3));
	}
	UARTgets(m_zInput, sizeof(m_zInput));
	if (zResult) {
		bSuccess = strcmp(m_zInput, zResult) ? false : true;
	}
	// Get final CR-LF
	while (UARTPeek('\r') == -1) {
		SysCtlDelay(SysCtlClockGet() / (1000 / 3));
	}
	UARTgets(m_zInput, sizeof(m_zInput));
	//
	return bSuccess;
}
//--------------------------------
void esp8266::Diag() {
	UARTprintf("esp8266::Diag\r\n");
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------

