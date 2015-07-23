/*
 * ain7.h
 *
 *  Created on: 14. nov. 2014
 *      Author: Anders
 */

#ifndef AIN7_H_
#define AIN7_H_

class ain7 {
public:
	ain7();
	virtual ~ain7();
	void Initialize();
	void Terminate();
	void Trigger();
	bool IsUpdated() const {return m_bUpdated;};
	uint32_t GetValue();
	uint32_t GetErrorCounter();

public:
	static ain7* m_pTheOneAndOnly;
	void OnInterrupt();

private:
	enum {
		GPIOBase = GPIO_PORTD_BASE,
		GPIOPin = GPIO_PIN_0,
		ADCBase = ADC0_BASE,
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

#endif /* AIN7_H_ */
