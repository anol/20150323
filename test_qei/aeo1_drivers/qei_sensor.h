/*
 * qei_sensor.h
 *
 *  Created on: 23. mars 2015
 *      Author: Anders
 */

#ifndef QEI_SENSOR_H_
#define QEI_SENSOR_H_

namespace aeo1 {

class qei_sensor {
public:
	enum device_id {
		QEI0, QEI1
	};

public:
	qei_sensor(device_id nDevice);
	virtual ~qei_sensor();

public:
	void Initialize();
	int32_t Get();
	static void Diag();

private:
	device_id m_nDevice;
};

} /* namespace aeo1 */

#endif /* QEI_SENSOR_H_ */
