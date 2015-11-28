/*
 * potmeter.h
 *
 *  Created on: 14. nov. 2014
 *      Author: Anders
 */

#ifndef potmeter_h
#define potmeter_h

class potmeter {
public:
	potmeter();
	virtual ~potmeter();
	void Initialize();
	void Terminate();
	void Trigger();
	bool IsUpdated() const {return m_bUpdated;};
	uint32_t GetValue();
	uint32_t GetErrorCounter();

public:
	static potmeter* m_pTheOneAndOnly;
	void OnInterrupt();

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
	uint32_t m_nErrorCounter;
	uint32_t m_nChannel[NumberOfChannels];
	uint32_t m_nSequence[NumberOfChannels];
};

#endif
