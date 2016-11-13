#include "../global_root.h"
#include "delta_crc32.h"

/******************************************************************

crc32-hashsum calculator.

******************************************************************/



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
static bool    crc32_table_set(false);

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

void CRC32::init()
{
    if (crc32_table_set)
        return;
    crc32_table_set = true;
    UINT32 polynomial = 0x04C11DB7;
    for (unsigned i = 0; i <= 0xFF; i++)
    {
        crc32_table[i] = crc32reflect(i,8) << 24;
        for (BYTE j = 0; j < 8; j++)
            crc32_table[i] = (crc32_table[i]<<1) ^ (crc32_table[i] & (1<<31)?polynomial:0);
        crc32_table[i] = crc32reflect(crc32_table[i],32);
    }
}

UINT32 CRC32::getChecksum(const char* str)
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


CRC32::Sequence::Sequence()
{
	reset();
}

void CRC32::Sequence::reset()
{
	init();
	status = 0xFFFFFFFF;
}

void CRC32::Sequence::append(const void*source, size_t size)
{
    const BYTE*data = (const BYTE*)source;
    for (size_t i = 0; i < size; i++)
        status = (status >> 8) ^ crc32_table[(status&0xFF) ^ data[i]];
}

UINT32	CRC32::Sequence::finish()	const
{
	return status ^ 0xFFFFFFFF;
}

