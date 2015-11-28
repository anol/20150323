/*
 * display.h
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

namespace aeo1 {

class display {
public:
	display();
	virtual ~display();

public:
	void Initialize();
	void Terminate();
	void Set(int32_t nValue, int nDecimals = -1);
	void OnInterrupt();

public:
	static uint32_t m_nCounter;
	static uint32_t m_nEotCounter;
	static uint32_t m_nOverflowCounter;
	static display* m_pTheOneAndOnly;
	static void Diag();

private:
	uint32_t Digit2Segments(uint32_t nIndex, uint32_t nValue,
			bool bbDecimalPoint);
	void LoadFIFO();

private:
	enum {
		GPIOPeripheral = SYSCTL_PERIPH_GPIOD,
		GPIOBase = GPIO_PORTD_BASE,
		GPIOPins = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
		SSIPeripheral = SYSCTL_PERIPH_SSI3,
		SSIBase = SSI3_BASE,
		SSIPinClk = GPIO_PD0_SSI3CLK,
		SSIPinFss = GPIO_PD1_SSI3FSS,
		SSIPinRx = GPIO_PD2_SSI3RX,
		SSIPinTx = GPIO_PD3_SSI3TX,
		Interrupt = INT_SSI3,
		BufferSize = 8
	};

private:
	uint32_t m_nDataTx[BufferSize];
	uint32_t m_nDataRx[BufferSize];
	uint32_t m_nIndex;
	bool m_bEmpty;

};

} /* namespace aeo1 */

#endif /* DISPLAY_H_ */
