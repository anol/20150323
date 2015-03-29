#ifndef ssi_peripheral_h
#define ssi_peripheral_h
//--------------------------------
namespace aeo1 {
//--------------------------------
struct ssi_specification {
	uint32_t m_nGPIOPeripheral;
	uint32_t m_nGPIOBase;
	uint32_t m_nGPIOPins;
	uint32_t m_nSSIPeripheral;
	uint32_t m_nSSIBase;
	uint32_t m_nSSIPinClk;
	uint32_t m_nSSIPinFss;
	uint32_t m_nSSIPinRx;
	uint32_t m_nSSIPinTx;
	uint32_t m_nSSI_CR1_R;
	uint32_t m_nInterrupt;
};
//--------------------------------
class ssi_peripheral {
public:
	enum device_id {
		SSI0, SSI1, SSI2, SSI3
	};

public:
	ssi_peripheral(device_id nDevice);
	virtual ~ssi_peripheral();

public:
	virtual void Initialize();
	virtual void OnInterrupt();
	virtual void Diag();
	virtual void Terminate();
	virtual void LoadFIFO();

protected:
	enum {
		BufferSize = 8
	};

protected:
	device_id m_nDevice;
	const ssi_specification& m_rSpecification;
	uint32_t m_nDataTx[BufferSize];
	uint32_t m_nDataRx[BufferSize];
	uint32_t m_nIndex;
};
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
#endif /* ssi_peripheral_h */
