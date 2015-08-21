/*
 * potmeter.h
 *
 *  Created on: 3. mars 2015
 *      Author: Anders
 */

#ifndef POTMETER_H_
#define POTMETER_H_

namespace aeo1 {

class adc_potmeter {
public:
	adc_potmeter();
	virtual ~adc_potmeter();

public:
	void Initialize();
	void Terminate();
	void OnInterrupt();
	void Trigger();
	bool IsUpdated() const {return m_bUpdated;};
	uint32_t GetValue();
	uint32_t GetErrorCounter();

public:
	static uint32_t m_nCounter;
	static uint32_t m_nErrorCounter;
	static uint32_t m_nReadCounter;
	static adc_potmeter* m_pTheOneAndOnly;
	static void Diag();

private:
	enum {
		GPIOPeripheral = SYSCTL_PERIPH_GPIOB,
		GPIOBase = GPIO_PORTB_BASE,
		GPIOPin = GPIO_PIN_5,
		ADCPeripheral = SYSCTL_PERIPH_ADC0,
		ADCBase = ADC0_BASE,
		ADCChannel = ADC_CTL_CH11,
		Interrupt = INT_ADC0SS3,
		Sequencer = 3,
		Oversampling = 64,
		NumberOfChannels = 2
	};
	bool m_bUpdated;
	uint32_t m_nSamples;
	uint32_t m_nChannel[NumberOfChannels];
	uint32_t m_nSequence[NumberOfChannels];
};

} /* namespace aeo1 */

#endif /* POTMETER_H_ */
