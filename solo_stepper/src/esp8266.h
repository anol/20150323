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
	enum {
		InputBufferSize = 128
	};

public:
	esp8266();
	virtual ~esp8266();

public:
	int Initialize();
	void Diag();

private:
	bool Invoke(const char* zCommand, const char* zResult);

private:
	char m_zInput[InputBufferSize];

};
} /* namespace aeo1 */
//--------------------------------
#endif /* esp8266_H_ */
//--------------------------------
