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

public:
	ssi_drv8711();
	virtual ~ssi_drv8711();

public:
	void Initialize();
	void Diag();
	uint32_t Read(uint32_t nRegister);
	uint32_t Write(uint32_t nRegister, uint32_t nValue);

private:
	uint32_t m_nRegister[NumberOfRegisters];

};
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
#endif /* ssi_drv8711_h */
