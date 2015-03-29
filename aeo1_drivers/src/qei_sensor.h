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
	enum configuration {
		DefaultConfiguration = (QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET
				| QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP),
		SwapPins = (QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET
				| QEI_CONFIG_QUADRATURE | QEI_CONFIG_SWAP)
	};
	enum device_id {
		QEI0, QEI1
	};

public:
	typedef void (*OnIndex)(void);

public:
	qei_sensor(device_id nDevice, configuration nConfig = DefaultConfiguration);
	virtual ~qei_sensor();

public:
	void Initialize();
	int32_t Get();
	void Set( int32_t nValue );
	void Zero();
	void Diag();

private:
	device_id m_nDevice;
	uint32_t m_ui32Base;
	configuration m_nConfig;
};

} /* namespace aeo1 */

#endif /* QEI_SENSOR_H_ */
