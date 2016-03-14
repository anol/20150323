package drv8711;

public class Attribute {
	public Attribute(int nRegister, int nPosition, int nSize, String zName, String zDescription, String zRange) {
		m_nRegister = nRegister;
		m_nPosition = nPosition;
		m_nSize = nSize;
		m_zName = zName;
		m_zDescription = zDescription;
		m_zRange = zRange;
	}

	int m_nRegister;
	int m_nPosition;
	int m_nSize;
	String m_zName;
	String m_zDescription;
	String m_zRange;
}
