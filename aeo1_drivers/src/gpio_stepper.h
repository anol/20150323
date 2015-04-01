/*
 * gpio_stepper.h
 *
 *  Created on: 27. mar. 2015
 *      Author: anol
 */

#ifndef SRC_GPIO_STEPPER_H_
#define SRC_GPIO_STEPPER_H_

namespace aeo1 {

class gpio_stepper {
public:
	gpio_stepper();
	virtual ~gpio_stepper();

public:
	void Initialize();
	void Diag();
};

} /* namespace aeo1 */

#endif /* SRC_GPIO_STEPPER_H_ */
