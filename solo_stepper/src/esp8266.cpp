/*
 * esp8266.cpp
 *
 *  Created on: 24. mars 2015
 *      Author: Anders Emil Olsen
 */
//--------------------------------
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_ints.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
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
static esp8266* pTheOneAndOnlyEsp8266 = 0;
//--------------------------------
enum {
	UART_BASE = UART1_BASE, UART_INT = INT_UART1,
};
//--------------------------------
esp8266::esp8266() :
		m_nTxHead(0), m_nTxTail(0), m_nRxHead(0), m_nRxTail(0) {
}
//--------------------------------
esp8266::~esp8266() {
}
//--------------------------------
int esp8266::Initialize() {
	int nCommandIndex = 0;
	pTheOneAndOnlyEsp8266 = this;
	ConfigureUART();
	while (InitializationCommands[nCommandIndex].zCommand) {
		if (Invoke(InitializationCommands[nCommandIndex].zCommand,
				InitializationCommands[nCommandIndex].zResult)) {
			nCommandIndex++;
		} else {
			break;
		}
	}
	return nCommandIndex;
}
//--------------------------------
void esp8266::ConfigureUART() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PC4_U1RX);
	GPIOPinConfigure(GPIO_PC5_U1TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	UARTClockSourceSet(UART_BASE, UART_CLOCK_PIOSC);
	MAP_UARTConfigSetExpClk(UART_BASE, 16000000, 115200,
			(UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));
	MAP_UARTFIFOLevelSet(UART_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
	MAP_UARTIntDisable(UART_BASE, 0xFFFFFFFF);
	MAP_UARTIntEnable(UART_BASE, UART_INT_RX | UART_INT_RT);
	MAP_IntEnable(UART_INT);
	MAP_UARTEnable(UART_BASE);
}
//--------------------------------
void esp8266::OnTransmit() {
	// Do we have any data to transmit?
	if (m_nTxHead != m_nTxTail) {
		// Disable the UART interrupt.  If we don't do this there is a race
		// condition which can cause the read index to be corrupted.
		MAP_IntDisable(UART_INT);
		// Yes - take some characters out of the transmit buffer and feed
		// them to the UART transmit FIFO.
		while (MAP_UARTSpaceAvail(UART_BASE) && (m_nTxHead != m_nTxTail)) {
			MAP_UARTCharPutNonBlocking(UART_BASE, m_cOutput[m_nTxHead]);
			m_nTxHead++;
			m_nTxHead %= OutputBufferSize;
		}
		// Reenable the UART interrupt.
		MAP_IntEnable(UART_INT);
	}
	// If the output buffer is empty, turn off the transmit interrupt.
	if (m_nTxHead == m_nTxTail) {
		MAP_UARTIntDisable(UART_BASE, UART_INT_TX);
	}
}
//--------------------------------
void esp8266::OnReceive() {
	while (MAP_UARTCharsAvail(UART_BASE)) {
		// Read a character
		int32_t i32Char = MAP_UARTCharGetNonBlocking(UART_BASE);
		// If there is space in the receive buffer, put the character
		// there, otherwise throw it away.
		if (m_nRxHead == ((m_nRxTail + 1) % InputBufferSize)) {
			// Store the new character in the receive buffer
			m_cInput[m_nRxTail] = (i32Char & 0xFF);
			m_nRxTail++;
			m_nRxTail %= InputBufferSize;
		}
	}
}
//--------------------------------
void esp8266::OnUart(uint32_t ui32Ints) {
	if (ui32Ints & UART_INT_TX) {
		OnTransmit();
	}
	if (ui32Ints & (UART_INT_RX | UART_INT_RT)) {
		OnReceive();
	}
}
//--------------------------------
extern "C" void esp8266_UARTIntHandler(void) {
	uint32_t ui32Ints = MAP_UARTIntStatus(UART_BASE, true);
	MAP_UARTIntClear(UART_BASE, ui32Ints);
	if (pTheOneAndOnlyEsp8266) {
		pTheOneAndOnlyEsp8266->OnUart(ui32Ints);
	}
}
//--------------------------------
int esp8266::FillOutputBuffer(const char* zString) {
	int nCount = 0;
	while ((*zString) && (m_nTxHead == ((m_nTxTail + 1) % OutputBufferSize))) {
		// Store the new character in the transmit buffer
		m_cOutput[m_nTxTail] = *(zString++);
		m_nTxTail++;
		m_nTxTail %= InputBufferSize;
		nCount++;
	}
	return nCount;
}
//--------------------------------
int esp8266::Write(const char* zString) {
	int nCount = 0;
	char zSize[8];
	nCount += FillOutputBuffer("AT+CIPSEND=1,");
	nCount += FillOutputBuffer(itoa(strlen(zString), zSize));
	nCount += FillOutputBuffer("\r\n");
	nCount += FillOutputBuffer(zString);
	return nCount;
}
//--------------------------------
bool esp8266::Invoke(const char* zCommand, const char* zResult) {
	bool bSuccess = true;
	UARTprintf("%s\r\n", zCommand);
	// Get response
	while (UARTPeek('\r') == -1) {
		SysCtlDelay(SysCtlClockGet() / (1000 / 3));
	}
	UARTgets(m_cInput, sizeof(m_cInput));
	if (zResult) {
		UARTprintf(" <%s >\r\n", m_cInput);
		bSuccess = strcmp(m_zInput, zResult) ? false : true;
	}
	// Get final CR-LF
	while (UARTPeek('\r') == -1) {
		SysCtlDelay(SysCtlClockGet() / (1000 / 3));
	}
	UARTgets(m_cInput, sizeof(m_cInput));
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

