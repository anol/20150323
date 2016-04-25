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
		AtHeaderSize = 20,
		InputBufferSize = 256,
		OutputBufferSize = 128,
		ReplyBufferSize = 128
	};

public:
	esp8266();
	virtual ~esp8266();

public:
	bool Initialize();
	void Reset();
	bool Setup();
	void Diag();
	void OnUart(uint32_t ui32Ints);
	bool Write(const char* zString);
	int ReadLine(char* zString, int nSize);
	bool RxEndOfLine();

private:
	void ConfigureUART(uint32_t nBps);
	void SetBitrate(uint32_t nBps);
	void OnTransmit();
	void OnReceive();
	int FillOutputBuffer(const char* zString);
	bool Invoke(const char* zCommand, const char* zSuccess,
			const char* zFailure, int nWaitCount);

private:
	int m_nTxHead;
	int m_nTxFill;
	int m_nRxHead;
	int m_nRxFill;
	int m_nRxEndOfLine;
	char m_cInput[InputBufferSize];
	char m_cOutput[OutputBufferSize];
	char m_zReply[ReplyBufferSize];

};
} /* namespace aeo1 */
//--------------------------------
#endif /* esp8266_H_ */
//--------------------------------
