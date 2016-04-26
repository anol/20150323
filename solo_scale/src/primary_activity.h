/*
 * primary_activity.h
 *
 *  Created on: 26. apr. 2016
 *      Author: Anders
 */

#ifndef primary_activity_h
#define primary_activity_h

class primary_activity {
public:
	primary_activity();
	virtual ~primary_activity();

public:
	void Initialize();
	void OnTick();
	void Diag();
	void SetX(int nValue);
	void SetY(int nValue);

private:
	aeo1::ssi_display m_oDisplayX;
	aeo1::ssi_display m_oDisplayY;
	aeo1::qei_sensor m_oScaleX;
	aeo1::qei_sensor m_oScaleY;

};

#endif /* primary_activity_h */
