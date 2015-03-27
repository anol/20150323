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
class ssi_drv8711 : public ssi_peripheral{

public:
	ssi_drv8711(ssi_peripheral::device_id nDevice);
	virtual ~ssi_drv8711();

public:
	void Initialize();

private:

private:
};
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
#endif /* ssi_drv8711_h */
