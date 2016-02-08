#ifndef drv8711_registers_h
#define drv8711_registers_h

typedef bool (*drv8711_register_PrintFunction)(const char* zName, int nValue,
		const char* zDescription, void* pUserData);

bool drv8711_registers_Print(int nRegister, int nValue,
		drv8711_register_PrintFunction pPrintFunction, void* pUserData);

#endif
