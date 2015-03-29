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
namespace aeo1 {
class pwm_stepper {
public:
	pwm_stepper();
	virtual ~pwm_stepper();

public:
	void Diag();

};
} /* namespace aeo1 */
//--------------------------------
#endif /* SRC_PWM_STEPPER_H_ */
//--------------------------------
