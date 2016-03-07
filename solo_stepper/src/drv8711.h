/*
 * drv8711.h
 *
 *  Created on: 24. mars 2015
 *      Author: Anders Emil Olsen
 */
//--------------------------------
#ifndef DRV8711_H_
#define DRV8711_H_
//--------------------------------
#include "ssi_drv8711.h"
#include "pwm_stepper.h"
//--------------------------------
namespace aeo1 {
class drv8711 {
public:
	enum {
		Success, No_Such_Attribute_Name, Illegal_Attribute_Value
	};
public:
	drv8711();
	virtual ~drv8711();

public:
	void Initialize();
	void Idle();
	void Halt();
	int Get(const char* zName, char* zValue, int nSize);
	int Set(const char* zName, const char* zValue);
	void Move(int32_t nSteps);
	void Stop(bool bHard);
	void Diag();
	void Sleep(bool bSleep);
	void ClearFaults();
	void Reset();

private:
	void SetDefault();
	void ReadAllRegisters();
	void PrintAllRegisters();
	void PrintStatus(uint32_t nStatus);

private:
	ssi_drv8711 m_oSsiDrv8711;
	pwm_stepper m_oPwmStepper;
};
} /* namespace aeo1 */
//--------------------------------
#endif /* DRV8711_H_ */
//--------------------------------
