#ifndef pngH
#define pngH


#define PNG_FILE_IO_ERROR   1
#define PNG_OK              0
#define PNG_NOT_SUPPORTED   (-1)
#define PNG_INTERNAL        (-2)
#define PNG_ERROR           (-3)


#include "img_format.h"
#ifdef __BORLANDC__
	#undef FAR
	#define FAR
#endif
#include "../../remote/png/png.h"


class PNG:public ImageFormat
{
public:
	PNG()				:ImageFormat("PNG","png") {magic_bytes.set4(0x89,0x50,0x4e,0x47);}
	virtual	void		saveToFilePointer(const Image&image, FILE*file)			override;
	virtual	void		loadFromFilePointer(Image&image, FILE*file)				override;
	void				saveToFileQ(const Image&image, const String&filename);	//!< Identical to saveToFile() using weaker compression
	static	void		compressToArray(const Image&image, Array<BYTE>&data);
	static	void		compressToStream(const Image&image, IWriteStream&stream);
	static	void		decompressArray(Image&image, const Array<BYTE>&data);
	static	void		decompressData(Image&image, const void*data, size_t data_size);
};

extern PNG		png;

#endif
