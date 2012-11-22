#ifndef crc32H
#define crc32H

/******************************************************************

crc32-hashsum calculator.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

/**
\brief CRC32 Checksum calculation

Calculates a sequenced CRC32 checksum. 

*/

namespace CRC32
{
    UINT32  getChecksum(const void* pntr,size_t len);	//!< Directly calculates a checksum of the specified data field \param pntr Pointer to the first byte of the data field \param len Length of the data field in bytes \return CRC32 checksum
	UINT32	getChecksum(const char* str);			//!< Directly calculates a checksum of the specified string \param str Pointer to the first character of the zero terminated string \return CRC32 checksum
	template <typename T>
    inline UINT32  getChecksum(const ArrayData<T>&array)	//!< Directly calculates a checksum of the specified data field \param array Array to determine the checksum of. Only actual data is read. Not recommended for dynamic structures such as strings \return CRC32 checksum
	{
		return getChecksum(array.pointer(),array.contentSize());
	}

    void    beginSequence();					//!< Begins a new checksum calculation sequence
    void    calcNext(const void*pntr,size_t len);		//!< Adds the specified data field to the checksum sequence  \param pntr Pointer to the first byte of the data field \param len Length of the data field in bytes
    UINT32  endSequence();					//!< Finishes a checksum calculation sequence and returns the checksum \return CRC32 checksum
	
	void 	init();							//!< Inits CRC32 calculation. Automatically invoked by all crc32 functions and classes if necessary
	
	class	Sequence
	{
	private:
			UINT32		status;
	public:
						Sequence();
			void		reset();								//!< Restarts the checksum sequence (this method is automatically called by the constructor)
			void		append(const void*source, size_t size);	//!< Appends data to the CRC32 volume and advances the internal status @param source Binary data to insert into the calculation @param size Number of bytes that should be inserted into the calculation. @a source must provide at least this many bytes
			UINT32		finish()	const;						//!< Resolves a result based on the current internal status. The method does not actually change anything and CRC32 calculation may be continued if desired
	};
}

/**
	\brief Compares two given data fields for equality
	\param pntr0 Pointer to the first byte of the first data field
	\param pntr1 Pointer to the first byte of the second data field
	\param size Size of both data fields in bytes
	\return 0 if both fields are identical, -1 if pntr0 is less than pntr1 and 1 if pntr0 is greater than pntr1

	fastCompare compares the given data fields and returns a linear order based on the content of the specified data fields.

*/
char        fastCompare(const void*pntr0, const void*pntr1, size_t size);




#endif
