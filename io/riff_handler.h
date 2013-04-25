#ifndef riff_handlerH
#define riff_handlerH
#include <stdio.h>


class RiffFile;
class RiffChunk;
struct SRiffInfo;
struct SRiffChunk;

#include "random_access_file.h"
#include <ctype.h>
#define F_OPEN_CHECK	//undefine to disable file-open-checking
#define RIFF_SENSITIVE	//undefine to disable MS-extended-type-checking
//#define DOUBLE_CHECK_DATA	//define to enable output-data-check

/**
standard-RIFF-format-handler

*/


/** how to use **

RIFF-format (read this if you want to know how it works):

RIFF is a tree-like data-structure containing lists and files.
while files can contain any kind of data, lists represent a tree-node
containing internal lists and files.

example:

riff
{
	file
	list
	{
		list
		{
			file
			file
		}
		file
	}
	file
	list
	{
		file
	}
}

both lists and files are stored in so called chunks containing a type-4-char-id
and an unsigned long byte-size followed by the actual data:

chunk
{
	char		type[4];
	unsigned	size;	//bytes
	[data]
}

data-size has to be multible of 2 (0,2,4,6,...) for speed-reasons.
If a file's data-size is not a multible of 2 a file-end-byte is added.
This end-byte however is not added to the size-value located in the
chunk's head.
for example:

chunk
{
	"some"
	3
	[4-byte-data]
}


type-specifier:
allowed is any 4-char-combination not containing a "\0". Note that
names shorter than 4 chars need to be finalized using
spaces (not \0).
for example:
"Main"
or
"DATA"
or
"VTX "

lists need one of the two following specifiers (both case-sensitive):
"LIST"
or
"RIFF"

Usually the main-list is called "RIFF" but any name will be accepted.
If you create a file using the RiffFile-object "RIFF" will be used.

****** USAGE ******

1) create a new RiffFile object (you can directly open a file by supplying a file-name as parameter)
2) (if you haven't already done so) create or open a file. 
3) (...)
4) call "close()" or delete the object (file will be closed automatically)


reading information from an open RIFF-file:

after opening the file-cursor will be reset.
Otherwise use "reset()" to reset the cursor to the first element of
the RIFF main context.
Since RIFF-files are tree-like data structures, reading the file is
context-orientated.
When you opened or reset the file you will be in the main context at
the first element.
Otherwise use "first()" to return the cursor to the first element of the
current context. "first()" will return false if the present context is
empty (no list elements).
Use "next()" to advance the cursor to the next element.
"next()" will return false if there is no next element in the selected
context.
Example to seek through all elements of one context:

	if (first())
		do
		{
			(...) whatever
		}
		while (next());
		
you can also directly select an element using "select(unsigned)".
"select(...)" will return false if the given unsigned is out of range.
if you use "findFirst(id)" ... "findNext(id)" instead of
"first()" ... "next()" you can find all chunks in the present context
matching the given id (UINT32 or const char*).
For example to find all lists in the present context use:

	if (findFirst("LIST"))
		do
		{
			(...) whatever
		}
		while (findNext("LIST"));

if the present element is a list-chunk you can enter it using "enter()".
This will move the cursor into the selected chunk and redefine the context.
"enter()" will return false if the present element cannot be entered.
To return from a context to a superior context use "dropBack()".
if you already are in the top-most context "dropBack()" will return false.

Retrieving information from the selected chunk:

isID(...)
	returns true if the selected chunk's id matches the given type-id.
	You can either check for an unsigned int or a string.
	examples:
		isID(65273)
		or
		isID("DATA")

getIndex()
	returns selected chunk's index inside present 
	context (0 if first element / 1 if second element ...)

getID()
	returns ID-string (const char*). The string is 0-terminated at
	result[4].

getSize()
	extract data-size of the selected chunk in bytes.

get(void*out_pntr)
	writes chunk-data to "out_pntr" and returns data-size.


*** editing an open riff-file ***

Any edit-command will return false if the file was not opened in write-
mode.


insertBlock(ID, void*data, unsigned data_size)
	inserts a block between the previous and the present chunk
	(if existing). Any data given will be written to the file.
	Works too if the present context is empty or no previous element is
	available.
	
	to insert a new list use
		insertBlock("LIST",NULL,0);
	Note that lists must be called "LIST" or "RIFF" or RiffFile-object
	will not recognize it as list but a file which cannot be entered.

appendBlock(ID, void*data, unsigned data_size)
	appends a block behind the last chunk
	(if existing). Any data given will be written to the file.
	Works too if the present context is empty or no previous element is
	available.
	
	to append a new list use
		appendBlock("LIST",NULL,0);


dropBlock()
	this will drop the selected chunk.
	Note, that all data inside this chunk will be erased (including sub-
	chunks).
	
resizeBlock()	- UNTESTED!!
	this will resize a selected block to the defined new size.
	It should work but remains untested.

*/


typedef UINT32				RIFF_INDEX_ID;
typedef const char*			RIFF_STRING_ID;
typedef UINT32				RIFF_ADDR;
typedef UINT32				RIFF_SIZE;



#define RIFF_CHUNK_HEAD_SIZE	(sizeof(RIFF_INDEX_ID)+sizeof(RIFF_SIZE))
#define RIFF_LIST				0x5453494C
#define RIFF_RIFF				0x46464952

#ifdef DOUBLE_CHECK_DATA
extern unsigned char check_buffer[1000000];
#endif


struct SRiffInfo
{
		RIFF_INDEX_ID			id;
		RIFF_SIZE		size;
};

struct SRiffChunk
{
		SRiffInfo		info;
		RIFF_ADDR		addr;
		unsigned		index;
		
		RIFF_ADDR		blockEnd();
		RIFF_ADDR		root();
};

class RiffFile:public RandomAccessFile
{
private:
		SRiffChunk		mark_stack[256];
		BYTE			mark_stacked;

		SRiffChunk		history[256];
		BYTE			husage;

		SRiffChunk		now;
		char			str_out[5];
		bool			streaming;
		RIFF_SIZE		streaming_at;
		
#ifdef	RIFF_SENSITIVE
		RIFF_INDEX_ID			sub_id;
#endif

protected:
		void				mark();
		void				recall();
		void				forget();

public:
		bool				ignore_main_size_;
		
							RiffFile();
							RiffFile(const char*filename);
virtual						~RiffFile();

		bool				open(const char*filename);	//opens an existing archive - closes old if existing
		void				assign(BYTE*data, ULONG size); //retrieves data from external data-field
		bool				reopen();
		const char*			mainID();
#ifdef	RIFF_SENSITIVE
		const char*			subID();
#endif
		RIFF_SIZE			mainSize();
		bool				create(const char*filename);//creates a new archive
		void				close();					//closes open archive
		void				reset();					//resets cursor to file-start
		bool				findFirst(RIFF_INDEX_ID ID);			//select riff-chunk via ID
		bool				findFirst(RIFF_STRING_ID ID);			//same for string-id
		bool				findNext(RIFF_INDEX_ID	ID);			//find next riff-block with this type-ID
		bool				findNext(RIFF_STRING_ID	ID);
		bool				select(unsigned index);		//select riff-chunk via context-index
		bool				isID(RIFF_INDEX_ID ID);				//checks wether or not selected chunk's id is equal to given one
		bool				isID(RIFF_STRING_ID ID);				//same for string_id
		bool				next();						//increase cursor
		bool				first();					//reset cursor in present context
		bool				enter();					//enters an element if its type is "LIST" or "RIFF"
		bool				dropBack();					//returns to the governing context
inline	bool				exit();						//identical to dropBack
		RIFF_SIZE			get(void*out);				//extrudes data
		RIFF_SIZE			get(void*out,size_t max);//extrudes data but not more than max - returns FULL size
template <class C>
		unsigned			get(ArrayData<C>&out);			//extracts data into an array
		const char*			getFileName();
		const char*			getID();					//returns block's type-id
		unsigned			getIndex();					//returns block's index (in present context)
		RIFF_ADDR			getAddr();					//returns block's address
		const SRiffChunk&	getChunk();
		RIFF_SIZE			getSize();					//returns size of block
		bool				multipleOf(size_t size); //returns true if getSize() is multiple of size
		RIFF_ADDR			tell();						//returns present file-cursor-position
		RIFF_SIZE			streamTell()	const	{return streaming?streaming_at:0;};			//!< Returns the number of bytes that have been read via stream so far
		bool				canStreamBytes(size_t bytes)	const;								//!< Checks whether or not the specified number of bytes can be streamed by the next stream call, or if no such amount is left in the remaining block space
		void				openStream();
template <class C>
		bool				stream(C&obj);
template <class C>
		bool				stream(C*obj, count_t num);
		bool				streamPointer(void*target, size_t data_size);
		bool				skip(size_t data_size);
		void				closeStream();
		
		bool				insertBlock(RIFF_INDEX_ID ID, const void*data=NULL, size_t size=0);	//insert a new block between the previous and the present block. Works too if inside an empty list-block. DATA can be NULL.
		bool				insertBlock(RIFF_STRING_ID ID, const void*data=NULL, size_t size=0);	//same for string_id
template <typename ID, typename T>
		bool				insertBlock(ID id, const ArrayData<T>&data);
		bool				appendBlock(RIFF_INDEX_ID ID, const void*data=NULL, size_t size=0);	//append a new block behind the last block. Works too if inside an empty list-block. DATA can be NULL.
		bool				appendBlock(RIFF_STRING_ID ID, const void*data=NULL, size_t size=0);	//same for string_id
template <typename ID, typename T>
		bool				appendBlock(ID id, const ArrayData<T>&data);
		bool				appendBlocks(RiffFile&other);
		bool				dropBlock();	//erase selected block (and all subblocks)
		bool				overwrite(const void*data, size_t check_size);
		bool				resizeBlock(size_t size);	//resizes selected block
		bool				swapBlocks(unsigned index0, unsigned index1);
		RandomAccessFile::size;
		RandomAccessFile::isActive;
};



class RiffChunk
{
protected:
		RIFF_ADDR		_offset;
		BYTE			*_data;
		RiffChunk		*_first,*_next,*_current,*_previous,*_parent;
		unsigned		_index;
		SRiffInfo		_info;
		bool			_streaming;
static	char			str_out[5];

public:

						RiffChunk();
						RiffChunk(RiffChunk*parent);
						RiffChunk(RiffChunk*parent, RIFF_INDEX_ID id, const void*data, size_t size);
virtual					~RiffChunk();

	
		bool			loadFromFile(const char*filename);
		RIFF_SIZE		fromFile(FILE*f, size_t size, bool force_list=false);
		void			loadFromData(const void*data, size_t size);
		RIFF_SIZE		fromData(const BYTE*data, size_t size, bool force_list=false);
		bool			saveToFile(const char*filename);
		void			toFile(FILE*f, bool force_list=false);
		RIFF_SIZE		saveToData(void*out);
		void			toData(BYTE*out, bool force_list=false);
		void			clear();
		RIFF_SIZE		resolveSize(bool force_list=true);


		RIFF_SIZE		get(void*out)	const;				//extracts data
		RIFF_SIZE		get(void*out,size_t max)	const;//extracts data but not more than max - returns FULL size
		const BYTE*		pointer()	const	/** @brief Retrieves the pointer to the beginning of the data of the local chunk*/	{return _data;}
	template <typename T>
		unsigned		get(ArrayData<T>&out)	const;
		void			setID(RIFF_INDEX_ID ID);
		void			setID(RIFF_STRING_ID ID);
		RIFF_INDEX_ID	getID() const;
		const char*		getIDString()	const;
		bool			isID(RIFF_INDEX_ID ID)	const;				//checks wether or not this chunk's id is equal to given one
		bool			isID(RIFF_STRING_ID ID)	const;				//same for string_id
		RIFF_SIZE		size()	const;						//returns this chunk's data-size
		bool			multipleOf(size_t size)	const; //returns true if size_ is multiple of size
		void			openStream();
	template <class C>
		bool			stream(C&obj);
	template <class C>
		bool			stream(C*obj, count_t num);
		bool			streamPointer(void*target, size_t data_size);
		void			closeStream();
		bool			overwrite(const void*data, size_t check_size);



		RiffChunk*		findFirst(RIFF_INDEX_ID ID);			//select riff-chunk via ID
		RiffChunk*		findFirst(RIFF_STRING_ID ID);			//same for string-id
		RiffChunk*		findNext(RIFF_INDEX_ID	ID);			//find next riff-block with this type-ID
		RiffChunk*		findNext(RIFF_STRING_ID	ID);
		RiffChunk*		select(unsigned index);		//select riff-chunk via context-index
		RiffChunk*		next();						//increase cursor
		RiffChunk*		first();					//reset cursor in present context
		unsigned		currentIndex()	const;					//returns block's index
		RiffChunk*		current();
		bool			eraseCurrent();
		bool			resizeCurrent(size_t size);
		RiffChunk*		insertBlock(RIFF_INDEX_ID ID, const void*data=NULL, size_t size=0);	//insert a new block between the previous and the present block. Works too if inside an empty list-block. DATA can be NULL.
		RiffChunk*		insertBlock(RIFF_STRING_ID ID, const void*data=NULL, size_t size=0);	//same for string_id
template<typename ID, typename T>
		RiffChunk*		insertBlock(ID id, const ArrayData<T>&data);
		RiffChunk*		appendBlock(RIFF_INDEX_ID ID, const void*data=NULL, size_t size=0);	//insert a new block between the previous and the present block. Works too if inside an empty list-block. DATA can be NULL.
		RiffChunk*		appendBlock(RIFF_STRING_ID ID, const void*data=NULL, size_t size=0);	//same for string_id
template<typename ID, typename T>
		RiffChunk*		appendBlock(ID id, const ArrayData<T>&data);
};





#include "riff_handler.tpl.h"


#endif

