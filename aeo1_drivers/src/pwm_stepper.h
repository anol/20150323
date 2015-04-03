/*
 * pwm_stepper.h
 *
 *  Created on: 27. mar. 2015
 *      Author: anol
 */
//--------------------------------
#ifndef SRC_PWM_STEPPER_H_
#define SRC_PWM_STEPPER_H_
//--------------------------------
#include "driverlib/pwm.h"
//--------------------------------
namespace aeo1 {
class pwm_stepper {
private:
	enum {
		Base = PWM1_BASE, Generator = PWM_GEN_1
	};
public:
	pwm_stepper();
	virtual ~pwm_stepper();
	void Initialize();
	void Diag();
	void Move(uint32_t nSteps);
	void OnInterrupt();
private:
	uint32_t m_nSteps;
};
} /* namespace aeo1 */
//--------------------------------
#endif /* SRC_PWM_STEPPER_H_ */
//--------------------------------
