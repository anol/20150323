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
		m_nTxHead(0), m_nTxFill(0), m_nRxHead(0), m_nRxFill(0), m_bRxEndOfLine(
				false) {
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
bool esp8266::Invoke(const char* zCommand, const char* zResult) {
	bool bSuccess = true;
	UARTprintf("<%s>\r\n", zCommand);
	FillOutputBuffer(zCommand);
	FillOutputBuffer("\r\n");
	// Get response
	while (!RxEndOfLine()) {
		SysCtlDelay(SysCtlClockGet() / (1000 / 3));
	}
	ReadLine();
	// Get final CR-LF
	while (!RxEndOfLine()) {
		SysCtlDelay(SysCtlClockGet() / (1000 / 3));
	}
	ReadLine();
	//
	return bSuccess;
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
	if (m_nTxHead != m_nTxFill) {
		MAP_IntDisable(UART_INT);
		while (MAP_UARTSpaceAvail(UART_BASE) && (m_nTxHead != m_nTxFill)) {
			m_nTxHead = ((m_nTxHead + 1) % OutputBufferSize);
			MAP_UARTCharPutNonBlocking(UART_BASE, m_cOutput[m_nTxHead]);
		}
		MAP_IntEnable(UART_INT);
	}
	if (m_nTxHead == m_nTxFill) {
		MAP_UARTIntDisable(UART_BASE, UART_INT_TX);
	}
}
//--------------------------------
void esp8266::OnReceive() {
	while (MAP_UARTCharsAvail(UART_BASE)) {
		// Read a character
		char cSymb = (0xFF & MAP_UARTCharGetNonBlocking(UART_BASE));
		if ('\n' == cSymb) {
			m_bRxEndOfLine = true;
		}
		if (m_nRxHead != ((m_nRxFill + 1) % InputBufferSize)) {
			// Store the new character in the receive buffer
			m_nRxFill = ((m_nRxFill + 1) % InputBufferSize);
			m_cInput[m_nRxFill] = cSymb;
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
	char cSymb = 0;
	while ((*zString) && (m_nTxHead != ((m_nTxFill + 1) % OutputBufferSize))) {
		cSymb = *(zString++);
		m_nTxFill = ((m_nTxFill + 1) % OutputBufferSize);
		m_cOutput[m_nTxFill] = cSymb;
		nCount++;

		// TODO:
		UARTprintf("%c", cSymb);

	}
	OnTransmit();
	MAP_UARTIntEnable(UART_BASE, UART_INT_TX);
	return nCount;
}
//--------------------------------
int esp8266::ReadLine() {
	int nCount = 0;
	char cSymb = 0;
	while (('\n' != cSymb) && (m_nRxHead != m_nRxFill)) {
		m_nRxHead++;
		m_nRxHead %= InputBufferSize;
		cSymb = m_cInput[m_nRxHead];
		nCount++;

		// TODO:
		UARTprintf("%c", cSymb);

	}
	return nCount;
}
//--------------------------------
int esp8266::Write(const char* zString) {
	int nCount = 0;
	char zSize[8];
	std::ltoa(strlen(zString), zSize);
	nCount += FillOutputBuffer("AT+CIPSEND=1,");
	nCount += FillOutputBuffer(zSize);
	nCount += FillOutputBuffer("\r\n");
	nCount += FillOutputBuffer(zString);
	return nCount;
}
//--------------------------------
void esp8266::Diag() {
	UARTprintf("esp8266::Diag\r\n");
}
//--------------------------------
} /* namespace aeo1 */
//--------------------------------

