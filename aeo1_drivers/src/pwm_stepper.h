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
		Base = PWM1_BASE,
		Generator = PWM_GEN_1,
		StartSpeed = 16000,
		MaxSpeed = 8000,
		Acceleration = 10
	};
	enum Phase {
		Phase_Idle,
		Phase_Accel,
		Phase_Steady,
		Phase_Decel,
		Phase_Stop,
		Phase_Halt
	};
public:
	pwm_stepper();
	virtual ~pwm_stepper();
	void Initialize();
	void Diag();
	void Move(uint32_t nSteps);
	void OnInterrupt();
private:
	int32_t m_nSteps;
	int32_t m_nSpeed;
	int32_t m_nTargetSpeed;
	int32_t m_nAcceleration;
	int32_t m_nDeceleration;
	Phase m_nPhase;
};
} /* namespace aeo1 */
//--------------------------------
#endif /* SRC_PWM_STEPPER_H_ */
//--------------------------------
