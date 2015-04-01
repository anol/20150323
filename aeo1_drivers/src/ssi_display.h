/*
 * ssi_display.h
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */

#ifndef ssi_display_h
#define ssi_display_h
//--------------------------------
#include "ssi_peripheral.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
class ssi_display: public ssi_peripheral {

public:
	ssi_display(ssi_peripheral::device_id nDevice);
	virtual ~ssi_display();

public:
	void Initialize();
	void Diag();
	void Set(int32_t nValue, int nDecimals = 0);
	void Set(const char* zString);
	void OnTx() {
		LoadTxFIFO();
	}

private:
	uint32_t Digit2Segments(uint32_t nIndex, uint32_t nValue,
			bool bbDecimalPoint);

private:
	bool m_bEmpty;
};
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
#endif /* ssi_display_h */
