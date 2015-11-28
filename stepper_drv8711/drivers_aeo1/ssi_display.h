/*
 * ssi_display.h
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */

#ifndef ssi_display_h
#define ssi_display_h

namespace aeo1 {

struct ssi_display_specification {
	uint32_t m_nGPIOPeripheral;
	uint32_t m_nGPIOBase;
	uint32_t m_nGPIOPins;
	uint32_t m_nSSIPeripheral;
	uint32_t m_nSSIBase;
	uint32_t m_nSSIPinClk;
	uint32_t m_nSSIPinFss;
	uint32_t m_nSSIPinRx;
	uint32_t m_nSSIPinTx;
	uint32_t m_nSSI_CR1_R;
	uint32_t m_nInterrupt;
};

class ssi_display {
public:
	enum device_id {
		SSI0, SSI1, SSI2, SSI3
	};

public:
	ssi_display(device_id nDevice);
	virtual ~ssi_display();

public:
	void Initialize();
	void Terminate();
	void Set(int32_t nValue, int nDecimals = -1);
	void OnInterrupt();

public:
	static uint32_t m_nCounter;
	static uint32_t m_nEotCounter;
	static uint32_t m_nOverflowCounter;
	static ssi_display* m_pTheSSI1;
	static ssi_display* m_pTheSSI3;
	static void Diag();

private:
	uint32_t Digit2Segments(uint32_t nIndex, uint32_t nValue,
			bool bbDecimalPoint);
	void LoadFIFO();

private:
	enum {
		BufferSize = 8
	};

private:
	device_id m_nDevice;
	const ssi_display_specification& m_rSpecification;
	uint32_t m_nDataTx[BufferSize];
	uint32_t m_nDataRx[BufferSize];
	uint32_t m_nIndex;
	bool m_bEmpty;

};

} /* namespace aeo1 */

#endif /* ssi_display_h */
