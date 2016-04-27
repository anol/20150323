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
	bool CheckInputs(int& rCount, bool bButton, bool bIndex) const;

private:
	aeo1::ssi_display m_oDisplay_X;
	aeo1::ssi_display m_oDisplay_Y;
	aeo1::qei_sensor m_oScale_X;
	aeo1::qei_sensor m_oScale_Y;
	int m_nPressedCount_X;
	int m_nPressedCount_Y;
};

#endif /* primary_activity_h */
