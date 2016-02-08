/*
 * esp8266.h
 *
 *  Created on: 08. february 2016
 *      Author: Anders Emil Olsen
 */
//--------------------------------
#ifndef esp8266_H_
#define esp8266_H_
//--------------------------------
namespace aeo1 {
class esp8266 {
public:
	enum {
		AtHeaderSize = 20, InputBufferSize = 128, OutputBufferSize = 128
	};

public:
	esp8266();
	virtual ~esp8266();

public:
	int Initialize();
	void Diag();
	void OnUart(uint32_t ui32Ints);
	int Write(const char* zString);
	int ReadLine(char* zString, int nSize);
	bool RxEndOfLine() {
		bool bRxEndOfLine = m_bRxEndOfLine;
		m_bRxEndOfLine = false;
		return bRxEndOfLine;
	}

private:
	void ConfigureUART(uint32_t nBps);
	void SetBitrate(uint32_t nBps);
	void OnTransmit();
	void OnReceive();
	int FillOutputBuffer(const char* zString);
	bool Invoke(const char* zCommand, const char* zResult);

private:
	int m_nTxHead;
	int m_nTxFill;
	int m_nRxHead;
	int m_nRxFill;
	bool m_bRxEndOfLine;
	char m_cInput[InputBufferSize];
	char m_cOutput[OutputBufferSize];

};
} /* namespace aeo1 */
//--------------------------------
#endif /* esp8266_H_ */
//--------------------------------
