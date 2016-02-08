/*
 * ssi_drv8711.h
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */

#ifndef ssi_drv8711_h
#define ssi_drv8711_h
//--------------------------------
#include "ssi_peripheral.h"
//--------------------------------
namespace aeo1 {
//--------------------------------
class ssi_drv8711: public ssi_peripheral {
public:
	enum {
		NumberOfRegisters = 8
	};
	enum Event {
		NoStall, StallEvent, NoFault, FaultEvent
	};
public:
	ssi_drv8711();
	virtual ~ssi_drv8711();
	void Initialize();
	void Diag();
	void Sleep(bool bSleep);
	void Reset();
	void OnGpioInterrupt(Event nEvent);
	uint32_t Read(uint32_t nRegister);
	uint32_t Write(uint32_t nRegister, uint32_t nValue);
	uint32_t GetRegister(uint32_t nRegister);
private:
	uint32_t m_nRegister[NumberOfRegisters];
};
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
#endif /* ssi_drv8711_h */
