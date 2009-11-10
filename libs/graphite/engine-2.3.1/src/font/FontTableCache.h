
#include "TtfUtil.h"

namespace gr 
{


// used to allow table sharing among font copies
// TBD: should this be a real class or is it acceptable to 
// make the attributes public
class FontTableCache
{
public:
	FontTableCache()
		: m_fontCount(1)
	{
		for (int i = 0; i<ktiLast; i++)
		{
			m_pTable[i] = NULL;
		}
	}
	~FontTableCache()
	{
		Assert(m_fontCount == 0);
		for (int i = 0; i<ktiLast; i++)
		{
			if (m_pTable[i]) delete[] m_pTable[i];
		}
	}
	const int & getFontCount() { return m_fontCount; }
	void incrementFontCount() { m_fontCount++; }
	void decrementFontCount() { m_fontCount--; Assert(m_fontCount > -1); }
	byte * getTable(TableId id) const { Assert(id < ktiLast); return m_pTable[id]; }
	const long & getTableSize(TableId id) const 
	{ 
		Assert(id < ktiLast); 
		return m_tableSize[id]; 
	}
	byte * allocateTable(TableId id, long size) 
	{ 
		m_pTable[id] = new byte[size]; 
		m_tableSize[id] = size;
		return m_pTable[id];
	}
	
private:
	int m_fontCount; 
	byte * m_pTable[ktiLast];
	long m_tableSize[ktiLast];
};

}
