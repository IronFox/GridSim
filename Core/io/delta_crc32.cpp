#include "../global_root.h"
#include "delta_crc32.h"

/******************************************************************

crc32-hashsum calculator.

******************************************************************/

#include <atomic>
#include <mutex>


namespace DeltaWorks
{
	char fastCompare(const void*pntr0, const void*pntr1, size_t size)
	{
		const int*i0 = reinterpret_cast<const int*>(pntr0),
				 *i1 = reinterpret_cast<const int*>(pntr1);
		size_t cnt = size/sizeof(int);
		for (;cnt;--cnt,++i0,++i1)
		{
			if ((*i0) > (*i1))
				return 1;
			if ((*i0) < (*i1))
				return -1;
		}
		const char*c0 = reinterpret_cast<const char*>(i0),
				  *c1 = reinterpret_cast<const char*>(i1);
		for (BYTE k = 0; k < (BYTE)(size%sizeof(int)); ++k, ++c0, ++c1)
		{
			if ((*c0) > (*c1))
				return 1;
			if ((*c0) < (*c1))
				return -1;
		}
		return 0;
	}



	static UINT32  crc32_table[0x100],crc32_last;
	static std::atomic<bool>    crc32_table_set(false);

	UINT32 crc32reflect(UINT32 ref, BYTE ch)
	{
		UINT32 value(0);
		for (UINT16 i = 1; i < (UINT16)(ch+1); i++)
		{
			if(ref & 1)
				value |= 1 << (ch-i);
			ref>>=1;
		}
		return value;
	}

	void Init()
	{
		if (crc32_table_set)
			return;

		static std::mutex mutex;
		mutex.lock();
			if (crc32_table_set)
			{
				mutex.unlock();
				return;
			}

			UINT32 polynomial = 0x04C11DB7;
			for (unsigned i = 0; i <= 0xFF; i++)
			{
				crc32_table[i] = crc32reflect(i,8) << 24;
				for (BYTE j = 0; j < 8; j++)
					crc32_table[i] = (crc32_table[i]<<1) ^ (crc32_table[i] & (1<<31)?polynomial:0);
				crc32_table[i] = crc32reflect(crc32_table[i],32);
			}
			crc32_table_set = true;
		mutex.unlock();
	}

	/*UINT32 CRC32::getChecksum(const char* str)
	{
		return getChecksum(str,strlen(str));
	}

	UINT32 CRC32::getChecksum(const void* pntr,size_t len)
	{
		init();
		const BYTE*data = (const BYTE*)pntr;
		UINT32 crc(0xFFFFFFFF);
		for (size_t i = 0; i < len; i++)
			crc = (crc >> 8) ^ crc32_table[(crc&0xFF) ^ data[i]];
		return crc ^ 0xFFFFFFFF;
	}

	void CRC32::beginSequence()
	{
		init();
		crc32_last = 0xFFFFFFFF;
	}

	void CRC32::calcNext(const void*pntr,size_t len)
	{
		const BYTE*data = (const BYTE*)pntr;
		for (size_t i = 0; i < len; i++)
			crc32_last = (crc32_last >> 8) ^ crc32_table[(crc32_last&0xFF) ^ data[i]];
	}

	UINT32 CRC32::endSequence()
	{
		return crc32_last ^ 0xFFFFFFFF;
	}
*/

	CRC32::Sequence::Sequence()
	{
		Reset();
	}

	void CRC32::Sequence::Reset()
	{
		Init();
		status = 0xFFFFFFFF;
	}

	CRC32::Sequence& CRC32::Sequence::Append(const void*source, size_t size)
	{
		const BYTE*data = (const BYTE*)source;
		for (size_t i = 0; i < size; i++)
			status = (status >> 8) ^ crc32_table[(status&0xFF) ^ data[i]];
		return *this;
	}

	UINT32	CRC32::Sequence::Finish()	const
	{
		return status ^ 0xFFFFFFFF;
	}
}
