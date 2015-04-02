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
	ssi_peripheral(device_id nDevice, uint32_t nBitRate, bool bNonBlocking =
			true);
	virtual ~ssi_peripheral();

public:
	virtual void Initialize();
	virtual void OnInterrupt();
	virtual void Diag();
	virtual void Terminate();
	virtual void OnRx() {
	}
	virtual void OnTx() {
	}

protected:
	void Put(uint32_t nValue);
	uint32_t Get();
	void LoadTxFIFO();
	void UnloadRxFIFO();
	bool IsEmpty() const {
		return m_bEmpty;
	}

protected:
	enum {
		BufferSize = 8
	};

private:
	const ssi_specification& m_rSpecification;
	device_id m_nDevice;
	uint32_t m_nBitRate;
	uint32_t m_nSRTFE; // SSI Transmit FIFO Empty (status)
	uint32_t m_nTXEOT; // Transmit FIFO is empty (interrupt)
	uint32_t m_nDMATX; // DMA Transmit complete
	uint32_t m_nDMARX; // DMA Receive complete
	uint32_t m_nTXFF; // TX FIFO half full or less
	uint32_t m_nRXFF; // RX FIFO half full or more
	uint32_t m_nRXTO; // RX timeout
	uint32_t m_nRXOR; // RX overrun
	bool m_bEmpty;
	bool m_bNonBlocking;

protected:
	uint32_t m_nRxCount;
	uint32_t m_nDataRx[BufferSize];
	uint32_t m_nDataTx[BufferSize];

};
//--------------------------------
} /* namespace aeo1 */
//--------------------------------
#endif /* ssi_peripheral_h */
