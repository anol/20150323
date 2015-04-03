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
	drv8711();
	virtual ~drv8711();

public:
	void Initialize();
	void Idle();
	void Halt();
	void Feed(int32_t nMicrosPerSecond);
	void Move(int32_t nMicros);
	void SetTorque(uint32_t nTorque);
	void SetDefault();
	void Stop();
	void Diag();
	void Step(uint32_t nSteps, bool bForward);
	void Sleep(bool bSleep);
	void ClearFaults();
	void Reset();

private:
	void ReadAllRegisters();

private:
	ssi_drv8711 m_oSsiDrv8711;
	pwm_stepper m_oPwmStepper;
};
} /* namespace aeo1 */
//--------------------------------
#endif /* DRV8711_H_ */
//--------------------------------
