#include "../global_root.h"
#include "riff_handler.h"

namespace Riff
{


	#ifdef DOUBLE_CHECK_DATA
	unsigned char check_buffer[1000000];
	#endif
	/*
	C_String c42ansi(const char*c4)
	{
	C_String rs;
	for (unsigned char i = 0; i < 4; i++)
		rs+=c4[i];
	return rs;
	}
	*/

	//rule: now.addr is at the beginning of its data-block/children

	char			Chunk::str_out[5] = "    ";


	static bool ischar(char c)
	{
		return isupper((BYTE)c) || isdigit((BYTE)c) || c == ' ';
	}


	static RIFF_SIZE	castSize(size_t size)
	{
		if (size > 0xFFFFFFFFULL)
			FATAL__("Parameter size exceeds maximum allowed RIFF size");
		return (RIFF_SIZE)size;
	}

	RIFF_ADDR SRiffChunk::blockEnd()
	{
		return addr+info.size;
	}

	RIFF_ADDR SRiffChunk::root()
	{
		return addr-RIFF_CHUNK_HEAD_SIZE;
	}

	void File::mark()
	{
			mark_stack[mark_stacked++] = now;
	}

	void File::recall()
	{
			if (!mark_stacked)
				return;
			now = mark_stack[--mark_stacked];
	}

	void File::forget()
	{
		mark_stacked--;
	}


	const char*File::MainID()
	{
		memcpy(str_out,&history[0].info.sid,4);
		return str_out;
	}

	#ifdef RIFF_SENSITIVE
		const char*File::SubID()
		{
			memcpy(str_out,&sub_id,sizeof(RIFF_INDEX_ID));
			return str_out;
		}
	#endif

	RIFF_SIZE File::mainSize()
	{
		return history[0].info.size;
	}

	bool File::IsMultipleOf(size_t size)
	{
		return now.info.size && !(now.info.size%size);
	}

	RIFF_SIZE File::GetSize()
	{
		return now.info.size;
	}

	const char*File::GetID()
	{
		memcpy(str_out,&now.info.sid,4);
		return str_out;
	}

	const SRiffChunk& File::GetChunk()
	{
		return now;
	}

	File::File():mark_stacked(0),husage(0),streaming(false),ignore_main_size_(false)
	{
		str_out[4] = 0;
	}

	File::File(const char*filename):mark_stacked(0),husage(0),streaming(false),ignore_main_size_(false)
	{
		str_out[4] = 0;
		file.open(filename);
	}

	File::~File()
	{
		Close();
	}

	bool File::Open(const char*filename)
	{
		Close();
		if (!file.open(filename,RandomAccessFile::Direct))
			return false;
		Reset();
		return true;
	}

	void File::Assign(BYTE*data, ULONG size)
	{
		Close();
		file.assign(data,size);
		Reset();
	}

	bool File::Reopen()
	{
		Close();
		if (!file.reopen(RandomAccessFile::Direct))
			return false;
		Reset();
		return true;
	}

	bool File::Create(const char*filename)
	{
		Close();
		if (!file.create(filename,RandomAccessFile::Direct))
			return false;
		file.append("RIFF",4);
		file.append((BYTE)0,4);
		Reset();
		return true;
	}


	void File::Close()
	{
		if (streaming)
			FATAL__("closing in stream");
		file.close();
	}

	void File::Reset()
	{
		streaming = false;
		if (!file.isActive())
			return;

		file.read(0,now.info);
		if (ignore_main_size_)
			now.info.size = file.size()-RIFF_CHUNK_HEAD_SIZE;
		now.addr = RIFF_CHUNK_HEAD_SIZE;
		now.index = 0;
		#ifdef  RIFF_SENSITIVE
			if (TID(now.info.sid) == RIFF_RIFF && now.info.size >= 8)
			{
				RIFF_INDEX_ID test;
				char*ctest = (char*)&test;
				file.read(RIFF_CHUNK_HEAD_SIZE+4,test);
				if (ischar(ctest[0]) && ischar(ctest[1]) && ischar(ctest[2]) && ischar(ctest[3]))
				{
					file.read(RIFF_CHUNK_HEAD_SIZE,sub_id);
					now.info.size-=4;
					now.addr+=4;
				}
				else
					sub_id = RIFF_RIFF;
			}
		#endif
		history[0] = now;
		husage = 0;
		mark_stacked = 0;
		Enter();
	}

	bool File::FindFirst(TID ID)
	{
		mark();
		if (First())
			do
				if (IsID(ID))
				{
					forget();
					return true;
				}
			while (Next());
		recall();
		return false;
	}



	bool File::FindNext(TID ID)
	{
		mark();
		while (Next())
			if (IsID(ID))
			{
				forget();
				return true;
			}
		recall();
		return false;
	}

	bool File::Select(unsigned index)
	{
		if (index < now.index)
			First();
		while (now.index != index && Next());
		return now.index == index;
	}

	unsigned File::GetIndex()
	{
		return now.index;
	}


	bool File::IsID(TID ID)
	{
		return ID == now.info.sid;
	}


	RIFF_ADDR File::Tell()
	{
		return now.addr+streaming?streaming_at:0;
	}


	bool File::Next()
	{
		RIFF_SIZE size = now.info.size+now.info.size%2;
		if (now.addr+size+RIFF_CHUNK_HEAD_SIZE > history[husage-1].blockEnd())
			return false;
		now.addr+=size+RIFF_CHUNK_HEAD_SIZE;
		file.read(now.addr-RIFF_CHUNK_HEAD_SIZE,now.info);
		now.index++;
		return true;
	}

	bool File::First()
	{
		if (streaming)
			FATAL__("seeking first while streaming");
		if (!history[husage-1].info.size || !file.isActive())
			return false;
		file.read(history[husage-1].addr,now.info);
		now.addr = history[husage-1].addr+RIFF_CHUNK_HEAD_SIZE;
		now.index = 0;
		return true;
	}

	bool File::Enter()
	{
		if (streaming)
			FATAL__("trying to enter while streaming");
		if ((!IsID("LIST") && !IsID("RIFF") && husage)) return false;
		history[husage++] = now;
		file.read(now.addr,now.info);
		now.addr+=RIFF_CHUNK_HEAD_SIZE;
		now.index = 0;
		return true;
	}

	bool File::DropBack()
	{
		if (streaming)
			FATAL__("trying to drop back while streaming");
		if (husage <= 1)
			return false;
		now = history[--husage];
	//    ShowMessage("dropping back "+IntToStr(now.addr));
		return true;
	}

	RIFF_SIZE File::Get(void*out)
	{
		if (streaming)
			FATAL__("get does not work during streaming-operation");
		file.extract(now.addr,out,now.info.size);
		return now.info.size;
	}

	RIFF_SIZE File::Get(void*out, size_t max)
	{
		if (streaming)
			FATAL__("get does not work during streaming-operation");
		RIFF_SIZE reading = now.info.size<max?now.info.size:castSize(max);
		file.extract(now.addr,out,reading);
		return now.info.size;
	}

	RIFF_ADDR File::GetAddr()
	{
		return now.addr;
	}

	const char* File::GetFileName()
	{
		return file.getFilename();
	}


	void File::OpenStream()
	{
		if (streaming)
			FATAL__("already streaming");
		streaming = true;
		streaming_at = 0;
		mark();
	}

	void File::CloseStream()
	{
		if (!streaming)
			FATAL__("trying to close non-existing stream");
		streaming = false;
		recall();
	}

	bool File::Skip(size_t data_size)
	{
		if (!streaming)
			FATAL__("no stream open");
		if (streaming_at+data_size>now.info.size)
			return false;
		streaming_at+=castSize(data_size);
		return true;
	}

	bool	File::CanStreamBytes(size_t bytes)	const
	{
		if (!streaming)
			return false;
		return ((size_t)streaming_at+bytes <= (size_t)now.info.size);
	}

	bool File::StreamPointer(void*target, size_t data_size)
	{
		if (!streaming)
			FATAL__("no stream open");
		if ((size_t)streaming_at+data_size > (size_t)now.info.size)
			return false;
		file.extract(now.addr+streaming_at,target,castSize(data_size));
		streaming_at+=castSize(data_size);
		return true;
	}

	bool File::AppendBlock(TID id, const void*data, size_t size)
	{
	//    bool catch_newone = !history[husage-1].info.size;
		RIFF_SIZE isize = RIFF_CHUNK_HEAD_SIZE+castSize(size+size%2);
		BYTE*insert = SHIELDED_ARRAY(new BYTE[isize],isize);
		char c[4];
		id.CopyTo(c);
		memcpy(insert,c,4);
		memcpy(&insert[4],&size,sizeof(size));
		memcpy(&insert[RIFF_CHUNK_HEAD_SIZE],data,size);
		RIFF_ADDR offset = history[husage-1].addr+history[husage-1].info.size+history[husage-1].info.size%2;
		memcpy(str_out,c,4);
	//    ShowMessage(C_String(str_out)+" "+IntToStr(offset));
		if (!file.insert(offset,insert,isize))
		{
			DISCARD_ARRAY(insert);
			return false;
		}
		for (BYTE i = 0; i < husage; i++)
		{
	//        ShowMessage("adding: "+IntToStr(isize)+" to "+IntToStr(i));
			history[i].info.size+=isize;
			file.overwrite(history[i].root(),history[i].info);
		}
	/*    if (catch_newone)
		{*/
			id.CopyTo(now.info.sid);
			now.info.size = castSize(size);
			now.addr = offset+RIFF_CHUNK_HEAD_SIZE;
			now.index = UNSIGNED_UNDEF;
	//    }
		DISCARD_ARRAY(insert);
    
		return true;
	}

	bool File::InsertBlock(TID id, const void*data, size_t size)
	{
		if (!history[husage-1].info.size)
			return AppendBlock(id,data,size);
		RIFF_SIZE isize = RIFF_CHUNK_HEAD_SIZE+castSize(size+size%2);
		BYTE*insert = SHIELDED_ARRAY(new BYTE[isize],isize);
		char c[4];
		id.CopyTo(c);
		memcpy(insert,c,4);
		memcpy(&insert[4],&size,sizeof(size));
		memcpy(&insert[RIFF_CHUNK_HEAD_SIZE],data,size);
		if (!file.insert(now.addr-RIFF_CHUNK_HEAD_SIZE,insert,isize))
		{
			DISCARD_ARRAY(insert);
			return false;
		}
		id.CopyTo(now.info.sid);
		now.info.size = castSize(size);
		for (BYTE i = 0; i < husage; i++)
		{
			history[i].info.size+=isize;
			file.overwrite(history[i].root(),history[i].info);
		}
		DISCARD_ARRAY(insert);
		return true;
	}

	bool File::DropBlock()
	{
		if (!history[husage-1].info.size || !file.isActive() || streaming)
			return false;
		RIFF_SIZE total_size = now.info.size+now.info.size%2+RIFF_CHUNK_HEAD_SIZE;
		if (!file.erase(now.addr-RIFF_CHUNK_HEAD_SIZE,total_size))
			return false;
		for (BYTE i = 0; i < husage; i++)
		{
			history[i].info.size-=total_size;
			file.overwrite(history[i].root(),history[i].info);
		}
		First();    
		return true;
	}

	bool File::Overwrite(const void*data, size_t check_size)
	{
		if (size_t(now.info.size) != check_size || !file.isActive() || streaming)
			return false;
		return file.overwrite(now.addr,data,castSize(check_size));
	}

	bool File::ResizeBlock(size_t size)
	{
		bool success;
		RIFF_ADDR end = now.addr+now.info.size+now.info.size%2;
		int difference = castSize((size+size%2)-(now.info.size+now.info.size%2));
		if (difference > 0)
			success = file.insert(end,(BYTE)0,difference);
		else
			if (difference < 0)
				success = file.erase(end+difference,-difference);
			else
				return true;
		if (!success)
			return false;
		now.info.size = castSize(size);
		file.overwrite(now.root(),now.info);
		for (BYTE i = 0; i < husage; i++)
		{
			history[i].info.size+=difference;
			file.overwrite(history[i].root(),history[i].info);
		}
		return true;
	}

	bool File::SwapBlocks(unsigned index0, unsigned index1)
	{
		if (index0 > index1)
		{
			unsigned save = index0;
			index0 = index1;
			index1 = save;
		}
		mark();
		if (!Select(index0))
		{
			recall();
			return false;
		}
		SRiffChunk first = now;
		BYTE*buffer = file.extract(now.addr-RIFF_CHUNK_HEAD_SIZE,now.info.size+RIFF_CHUNK_HEAD_SIZE);
		if (!Select(index1))
		{
			recall();
			dealloc(buffer);
			return false;
		}
		SRiffChunk second = now;
		BYTE*second_buffer = file.extract(now.addr-RIFF_CHUNK_HEAD_SIZE,now.info.size+RIFF_CHUNK_HEAD_SIZE);
		ResizeBlock(first.info.size);
		file.overwrite(second.addr-RIFF_CHUNK_HEAD_SIZE,buffer,first.info.size+RIFF_CHUNK_HEAD_SIZE);
		Select(index0);
		ResizeBlock(second.info.size);
		file.overwrite(first.addr-RIFF_CHUNK_HEAD_SIZE,second_buffer,second.info.size+RIFF_CHUNK_HEAD_SIZE);
		recall();
		dealloc(buffer);
		dealloc(second_buffer);
		return true;
	}


	bool File::AppendBlocks(File&other)
	{
		mark();
		other.mark();
		if (other.First())
			do
			{
				RIFF_SIZE size = other.GetSize();
				char*buffer = alloc<char>(size);
				other.Get(buffer);
				if (!AppendBlock(other.now.info.sid,buffer,size))
				{
					dealloc(buffer);
					recall();
					other.recall();
					return false;
				}
				dealloc(buffer);
			}
			while (other.Next());
		other.recall();
		recall();
		return true;
	}


	//------------------------------------------------------------------------------

	Chunk::Chunk():_data(NULL),_first(NULL),_next(NULL),_current(NULL),_previous(NULL),_parent(NULL),_index(0),_streaming(false)
	{
		TID(RIFF_RIFF).CopyTo(_info.sid);
	}

	Chunk::Chunk(Chunk*parent):_data(NULL),_first(NULL),_next(NULL),_current(NULL),_previous(NULL),_parent(parent),_index(0),_streaming(false)
	{
		TID(RIFF_RIFF).CopyTo(_info.sid);
		//_info.id = RIFF_RIFF;
	}

	Chunk::Chunk(Chunk*parent, TID id, const void*data, size_t size):_first(NULL),_next(NULL),_current(NULL),_previous(NULL),_parent(parent),_index(0),_streaming(false)
	{
		id.CopyTo(_info.sid);
		_info.size = castSize(size);
		RIFF_SIZE i_size = castSize(size + size%2);
		alloc(_data,i_size);
		memcpy(_data,data,size);
	}

	void Chunk::SetData(const void*data, size_t dataSize)
	{
		memcpy(SetData(dataSize),data,dataSize);
	}
	void* Chunk::SetData(size_t dataSize)
	{
		dealloc(_data);
		RIFF_SIZE i_size = castSize(dataSize + dataSize%2);
		alloc(_data,i_size);
		_info.size = castSize(dataSize);

		return _data;

	}


	Chunk::~Chunk()
	{
		dealloc(_data);
		if (_first)
			DISCARD(_first);
		if (_next)
			DISCARD(_next);
	}

    
	bool Chunk::LoadFromFile(const char*filename)
	{
		Clear();
		FILE*f = fopen(filename,"rb");
		if (!f)
			return false;
		Clear();
		fseek(f,0,SEEK_END);
		RIFF_SIZE max_len = ftell(f);
		fseek(f,0,SEEK_SET); 
		FromFile(f,max_len,true);
		fclose(f);
		return true;
	}

	RIFF_SIZE Chunk::FromFile(FILE*f, size_t size, bool force_list)
	{
		Clear();
		if (size < sizeof(TRiffInfo))
			return 0;
		if (!fread(&_info,sizeof(_info),1,f))
			return 0;
		if (size-sizeof(TRiffInfo) < _info.size)
		{
			throw IO::DriveAccess::FileFormatFault("RIFF size violation");
			return sizeof(TRiffInfo);
		}
		RIFF_SIZE local = _info.size;
		_previous = NULL;
		_index = 0;
		_current = NULL;
		#ifdef RIFF_SENSITIVE
			if (TID(_info.sid) == RIFF_RIFF && _info.size >= 8)
			{
				RIFF_INDEX_ID test;
				char*ctest = (char*)&test;
				fseek(f,4,SEEK_CUR);
				if (!fread(&test,sizeof(test),1,f))
					return 0;
				if (ischar(ctest[0]) && ischar(ctest[1]) && ischar(ctest[2]) && ischar(ctest[3]))
					_info.size-=4;
				else
					fseek(f,-4,SEEK_CUR);
				fseek(f,-4,SEEK_CUR);
			}
		#endif
		if (TID(_info.sid) == RIFF_LIST || force_list)
			while (local >= sizeof(TRiffInfo))
			{
				if (!_current)
				{
					_current = SHIELDED(new Chunk(this));
					_first = _current;
				}
				else
				{
					_current->_next = SHIELDED(new Chunk(this));
					_current = _current->_next;
				}
				local-=_current->FromFile(f,local);
			}
		else
		{
			RIFF_SIZE i_size = local+local%2;
			re_alloc(_data,i_size);
			if (fread(_data,1,i_size,f)!=i_size)
				return 0;
		}
		_current = NULL;
		return _info.size+_info.size%2+sizeof(TRiffInfo);
	}

	void Chunk::LoadFromData(const void*data, size_t size)
	{
		Clear();
		FromData((const BYTE*)data,size, true);
	}

	RIFF_SIZE Chunk::FromData(const BYTE*data, size_t size, bool force_list)
	{
		Clear();
		if (size < sizeof(TRiffInfo))
			return 0;
		memcpy(&_info,data,sizeof(_info));
		if (size-sizeof(TRiffInfo) < _info.size)
			return sizeof(TRiffInfo);
		RIFF_SIZE local = _info.size;
		_previous = NULL;
		_index = 0;
		_current = NULL;
		RIFF_ADDR offset = sizeof(TRiffInfo);
		if (TID(_info.sid) == RIFF_LIST || force_list)
			while (local >= sizeof(TRiffInfo))
			{
				if (!_current)
				{
					_current = SHIELDED(new Chunk(this));
					_first = _current;
				}
				else
				{
					_current->_next = SHIELDED(new Chunk(this));
					_current = _current->_next;
				}
				offset+=_current->FromData(&data[offset],local-offset);
			}
		else
		{
			RIFF_SIZE i_size = local+local%2;
			re_alloc(_data,i_size);
			memcpy(_data,&data[offset],i_size);
		}
		_current = NULL;
		return _info.size+_info.size%2+sizeof(TRiffInfo);
	}


	bool Chunk::SaveToFile(const char*filename)
	{
		FILE*f = fopen(filename,"wb");
		if (!f)
			return false;
		ResolveSize(true);
		ToFile(f, true);
		fclose(f);
		return true;
	}

	void Chunk::ToFile(FILE*f, bool force_list)
	{
		if (!fwrite(&_info,sizeof(_info),1,f))
			return;
		if (TID(_info.sid) == RIFF_LIST || force_list)
		{
			Chunk*chunk = _first;
			while (chunk)
			{
				chunk->ToFile(f);
				chunk = chunk->_next;
			}
		}
		else
			ASSERT__(fwrite(_data,1,_info.size+_info.size%2,f)==_info.size+_info.size%2);
	}

	RIFF_SIZE Chunk::SaveToData(void*out)
	{
		ResolveSize(true);
		ToData((BYTE*)out,true);
		return sizeof(_info)+_info.size+_info.size%2;
	}

	void Chunk::ToData(BYTE*out,bool force_list)
	{
		memcpy(out,&_info,sizeof(_info));
		RIFF_ADDR offset = sizeof(TRiffInfo);
		if (TID(_info.sid) == RIFF_LIST || force_list)
		{
			Chunk*chunk = _first;
			while (chunk)
			{
				chunk->ToData(&out[offset]);
				offset+=chunk->_info.size+chunk->_info.size%2+sizeof(TRiffInfo);
				chunk = chunk->_next;
			}
		}
		else
			memcpy(&out[offset],_data,_info.size + _info.size%2);
	}

	RIFF_SIZE Chunk::ResolveSize(bool force_list)
	{
		if (TID(_info.sid) == RIFF_LIST || force_list)
		{
			_info.size = 0;
			Chunk*chunk = _first;
			while (chunk)
			{
				_info.size += chunk->ResolveSize(false);
				chunk = chunk->_next;
			}
			return sizeof(_info) + _info.size;
		}
		else
			return sizeof(_info) + _info.size + _info.size%2;
	}


	void Chunk::Clear()
	{
		if (_first)
			DISCARD(_first);
		_first = nullptr;
		_current = nullptr;
		_previous = nullptr;
		_index = 0;
		dealloc(_data);
		_data = nullptr;
		_info.size = 0;
	}

	Chunk*Chunk::FindFirst(TID id)
	{
		if (Chunk*chunk = First())
			do
			{
				if (TID(chunk->_info.sid) == id)
					return chunk;
			}
			while (chunk = Next());
		return nullptr;
	}


	Chunk*Chunk::FindNext(TID ID)
	{
		while (Chunk*chunk = Next())
			if (TID(chunk->_info.sid) == ID)
				return chunk;
		return nullptr;
	}


	Chunk*Chunk::Select(unsigned index)
	{
		if (index < _index)
			First();
		while (_index < index && Next());

		if (_index == index)
			return _current;
		return nullptr;
	}

	Chunk*Chunk::First()
	{
		_current = _first;
		_previous = nullptr;
		_index = 0;
		return _current;    
	}

	Chunk*Chunk::Next()
	{
		if (!_current)
			return nullptr;
		_previous = _current;
		_current = _current->_next;
		_index++;
		return _current;    
	}

	void Chunk::SetID(TID id)
	{
		id.CopyTo(_info.sid);
	}


	TID Chunk::GetID()	const
	{
		return _info.sid;
	}


	TID::TID(RIFF_STRING_ID sid):value(0)
	{
		if (!sid)
			return;
		for (int i = 0; i < 4; i++)
		{
			if (!sid[i])
				return;
			value |= ((UINT32)(BYTE)sid[i])<<(i*8);
		}
	}

	const char* TID::ToString() const
	{
		static char field[5] = "    ";
		CopyTo(field);
		return field;
	}

	void		TID::CopyTo(char field[4])	const
	{
		for (int i = 0; i < 4; i++)
		{
			field[i] = (char)(((value)>>(i*8)) & 0xFF);
		}
	}




	RIFF_SIZE Chunk::Get(void*out)	const
	{
		memcpy(out,_data,_info.size);
		return _info.size;
	}

	RIFF_SIZE Chunk::Get(void*out,size_t max)	const
	{
		RIFF_SIZE size = _info.size;
		if (size > max)
			size = castSize(max);
		memcpy(out,_data,size);
		return size;        
	}

	unsigned Chunk::CurrentIndex()	const
	{
		return _index;
	}

	Chunk*Chunk::Current()
	{
		return _current;
	}


	bool Chunk::IsID(TID id) const
	{
		return id == _info.sid;
	}


	RIFF_SIZE Chunk::size() const
	{
		return _info.size;
	}

	bool Chunk::IsMultipleOf(size_t size) const
	{
		return _info.size && !(_info.size%size);
	}

	void Chunk::OpenStream()
	{
		_offset = 0;
		_streaming = true;
	}

	bool Chunk::StreamPointer(void*target, size_t data_size)
	{
		if (data_size+_offset > _info.size)
			return false;
		memcpy(target,&_data[_offset],data_size);
		_offset += castSize(data_size);
		return true;
	}

	bool Chunk::CloseStream()
	{
		_streaming = false;
		return _offset == _info.size;
	}
        
	bool Chunk::EraseCurrent()
	{
		if (!_current)
			return false;
		if (_previous)
			_previous->_next = _current->_next;
		else
			_first = _current->_next;
		_current->_next = nullptr;
		DISCARD(_current);
		_current = _previous->_next;
		return true;
	}

	bool Chunk::ResizeCurrent(size_t size)
	{
		if (!_current)
			return false;
		if (size == _current->_info.size)
			return true;
		BYTE*temp = alloc<BYTE>(size+size%2);
		RIFF_SIZE move = castSize(size);
		if (move > _current->_info.size)
			move = _current->_info.size;
		memcpy(temp,_current->_data,move);
		dealloc(_current->_data);
		_current->_data = temp;
		_current->_info.size = castSize(size);
		return true;
	}

	Chunk*Chunk::InsertBlock(TID ID, const void*data, size_t size)
	{
		if (!_current)
			return AppendBlock(ID,data,size);
		Chunk*chunk = SHIELDED(new Chunk(this,ID,data,size));
		if (!_previous)
		{
			chunk->_next = _first;
			_first = chunk;
		}
		else
		{
			chunk->_next = _previous->_next;
			_previous->_next = chunk;
		}
		_current = chunk;
		return chunk;
	}

	Chunk*Chunk::AppendBlock(TID ID, const void*data, size_t size)
	{
		if (!_current)
			First();
		while (Next());
		Chunk*chunk = SHIELDED(new Chunk(this,ID,data,size));
		if (_previous)
			_previous->_next = chunk;
		else
			_first = chunk;
		_current = chunk;
		return chunk;
	}

	bool Chunk::Overwrite(const void*data, size_t check_size)
	{
		if (_info.size != check_size)
			return false;
		memcpy(_data,data,check_size);
		return true;
	}

}