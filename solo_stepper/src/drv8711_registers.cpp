#include <stdint.h>
#include <string.h>
#include "utils/ustdlib.h"

#include "drv8711_registers.h"

struct drv8711_field {
	int nRegister;
	int nPosition;
	int nSize;
	char* zName;
	char* zDescription;
};

const int DRV8711_Masks[] =
		{ 0, 1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047 };

// Register, Position, Size, Name, Description
const drv8711_field DRV8711_Fields[] =
		{

// 0: Control Register

				{ 0, 0, 1, "ENBL", "Enable" },

				{ 0, 1, 1, "RDIR", "Reverse direction" },

				{ 0, 2, 1, "RSTEP", "Advance indexer one step" },

				{ 0, 3, 4, "MODE", "Step mode: Full-step, half-step, � step, �" },

				{ 0, 7, 1, "EXSTALL", "Use external stall detection" },

				{ 0, 8, 2, "ISGAIN", "Amplifier gain: *5, *10, *20, *40" },

				{ 0, 10, 2, "DTIME", "Dead time: 400ns, 450ns, 650ns, 850ns" },

// 1: Torque Register

				{ 1, 0, 8, "TORQUE", "Full-scale output current" },

				{ 1, 8, 3, "SMPLTH",
						"Back EMF sample threshold: 50us, 100us, 200us, �" },

// 2: Off Time Register

				{ 2, 0, 8, "TOFF", "Fixed off time: 500ns + N*500ns" },

				{ 2, 8, 1, "PWMMODE", "Bypass indexer" },

// 3: Blanking Register

				{ 3, 0, 8, "TBLANK", "Current trip blanking time: 1us + N*20ns" },

				{ 3, 8, 1, "ABT", "Enable adaptive blanking time" },

// 4: Decay Register

				{ 4, 0, 8, "TDECAY", "Mixed decay transition time: N*500ns" },

				{ 4, 8, 3, "DECMOD", "Decay mode: Slow, Mixed, Fast, Auto, �" },

// 5: Stall Detect Register

				{ 5, 0, 8, "SDTHR", "Stall detect threshold" },

				{ 5, 8, 2, "SDCNT",
						"Stall detect asserted count-down: 1, 2, 4, 8 steps" },

				{ 5, 10, 2, "VDIV", "Back EMF divider: 1/32, 1/16, 1/8, 1/4" },

// 6: Drive Register

				{ 6, 0, 2, "OCPTH", "OCP threshold: 250mV, 500mV, 750mV, 1000mV" },

				{ 6, 2, 2, "OCPDEG", "OCP deglitch time: 1us, 2us, 4us, 8us" },

				{ 6, 4, 2, "TDRIVEN",
						"Low-side gate drive time: 250ns, 500ns, 1us, 2us" },

				{ 6, 6, 2, "TDRIVEP",
						"High-side gate drive time: 250ns, 500ns, 1us, 2us" },

				{ 6, 8, 2, "IDRIVEN",
						"Low-side gate drive peak current: 100mA, 200mA, �" },

				{ 6, 10, 2, "IDRIVEP",
						"High-side gate drive peak current: 50mA, 100mA, �" },

// 7: Status Register

				{ 7, 0, 1, "OTS", "Over-temperature shutdown" },

				{ 7, 1, 1, "AOCP", "Channel A over-current shutdown" },

				{ 7, 2, 1, "BOCP", "Channel B over-current shutdown" },

				{ 7, 3, 1, "APDF", "Channel A pre-driver fault" },

				{ 7, 4, 1, "BPDF", "Channel B pre-driver fault" },

				{ 7, 5, 1, "UVLO", "Under-voltage lockout" },

				{ 7, 6, 1, "STD", "Stall detected" },

				{ 7, 7, 1, "STDLAT", "Latched stall detect" },

// End of table

				{ 0, 0, 0, 0, 0 },

		};
//--------------------------------
static int GetFieldValue(int nValue, int nPosition, int nSize) {
	int nFieldValue = nValue >>= nPosition;
	if ((0 <= nSize) && (nSize < (sizeof(DRV8711_Masks) / sizeof(int)))) {
		nFieldValue &= DRV8711_Masks[nSize];
	}
	return nFieldValue;
}
//--------------------------------
static int SetFieldValue(int rRegisterValue, int nFieldValue, int nPosition,
		int nSize) {
	if ((0 <= nSize) && (nSize < (sizeof(DRV8711_Masks) / sizeof(int)))) {
		nFieldValue <<= nPosition;
		nFieldValue &= DRV8711_Masks[nSize];
		rRegisterValue &= ~(DRV8711_Masks[nSize]);
		rRegisterValue |= nFieldValue;
	}
	return rRegisterValue;
}
//--------------------------------
bool drv8711_registers_Print(int nRegister, int nValue,
		drv8711_register_PrintFunction pPrintFunction, void* pUserData) {
	bool bContinue;
	if (pPrintFunction) {
		int nIndex = 0;
		int nPosition = 0;
		bContinue = true;
		while (bContinue && DRV8711_Fields[nIndex].nSize) {
			if (DRV8711_Fields[nIndex].nRegister == nRegister) {
				if (DRV8711_Fields[nIndex].nPosition == nPosition) {
					int nFieldValue = GetFieldValue(nValue, nPosition,
							DRV8711_Fields[nIndex].nSize);
					bContinue = pPrintFunction(DRV8711_Fields[nIndex].zName,
							nFieldValue, DRV8711_Fields[nIndex].zDescription,
							pUserData);
					nPosition += DRV8711_Fields[nIndex].nSize;
				}
			}
			nIndex++;
		}
	} else {
		bContinue = false;
	}
	return bContinue;
}
//--------------------------------
int drv8711_registers_GetRegisterNumber(const char* zName) {
	int nRegister = -1;
	int nIndex = 0;
	while ((0 > nRegister) && DRV8711_Fields[nIndex].nSize) {
		if (0 == strcmp(DRV8711_Fields[nIndex].zName, zName)) {
			nRegister = DRV8711_Fields[nIndex].nRegister;
		} else {
			nIndex++;
		}
	}
	return nRegister;
}
//--------------------------------
int drv8711_registers_GetFieldValue(const char* zName, int nRegisterValue,
		uint32_t& rFieldValue) {
	int nIndex = 0;
	int nSize = 0;
	int nPosition = 0;
	while ((0 == nSize) && DRV8711_Fields[nIndex].nSize) {
		if (0 == strcmp(DRV8711_Fields[nIndex].zName, zName)) {
			nSize = DRV8711_Fields[nIndex].nSize;
			nPosition = DRV8711_Fields[nIndex].nPosition;
			rFieldValue = GetFieldValue(nRegisterValue, nPosition, nSize);
		} else {
			nIndex++;
		}
	}
	return nSize ? 0 : -1;
}
//--------------------------------
int drv8711_registers_SetFieldValue(const char* zName, uint32_t& rRegisterValue,
		const char* zValue) {
	int nIndex = 0;
	int nSize = 0;
	int nPosition = 0;
	int nValue = ustrtoul(zValue, 0, 10);
	while ((0 == nSize) && DRV8711_Fields[nIndex].nSize) {
		if (0 == strcmp(DRV8711_Fields[nIndex].zName, zName)) {
			nSize = DRV8711_Fields[nIndex].nSize;
			nPosition = DRV8711_Fields[nIndex].nPosition;
			rRegisterValue = SetFieldValue(rRegisterValue, nValue, nPosition,
					nSize);
		} else {
			nIndex++;
		}
	}
	return nSize ? 0 : -1;
}
//--------------------------------
