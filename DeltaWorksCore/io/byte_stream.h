#ifndef byte_streamH
#define byte_streamH

#include "../interface/read_stream.h"
#include "../interface/write_stream.h"
#include "../container/array.h"

namespace DeltaWorks
{
	class ByteStream : public IReadStream, public IWriteStream
	{
	private:
		char			*begin,
						*current,
						*end;
		bool			local;
       
		void			_PushData(const void*pntr, size_t size);
		void			_GetData(void*pntr, size_t size);
	public:

		/**/			ByteStream(size_t size=1024);
		virtual			~ByteStream();
       
		void			Reset();
		void			Clear() {Reset();}
		void			Resize(size_t len);
		template <typename T>
		void			Assign(Array<T>&array);
		void			Assign(void*pntr, size_t size);
		char*			pointer();
		const char*		pointer()	const;
		const void*		data()	const;
		size_t			GetStorageSize()	const;
		size_t			GetFillLevel()	const;
		void			pushZero(size_t count);
		template <class C>
		bool			ReadNext(C&out);
		template <class C>
		bool			ReadNext(C*array, count_t count);
		template <class C>
		void			Append(const C&object);
		template <class C>
		void			Append(const C*array, count_t count);
		template <class C>
		ByteStream&		operator<<(const C&object);
		template <class C>
		bool			operator>>(C&object);

		void			CopyToArray(Array<BYTE>&outArray)const	{outArray.resizeAndCopy(begin,GetFillLevel());}

		virtual	void	Write(const void*data, serial_size_t size)	override {_PushData(data,size);}
		virtual	void	Read(void*target_data, serial_size_t size)	override {_GetData(target_data,size);}
		virtual serial_size_t		GetRemainingBytes() const override {return end - current;}
	};

	template <size_t Bytes>
		class ByteStreamT : public IWriteStream
		{
		private:
			char		data[Bytes],
						*current,
						*const end;

			void		_PushData(const void*pntr, size_t size)
			{
				if (current+size > end)
					FATAL__("Internal fault: Byte stream size exhausted");
				//ASSERT_LESS_OR_EQUAL1__(current + size, end, size);
				memcpy(current, pntr, size);
				current += size;
			}
		public:

			/**/			ByteStreamT() : current(data), end(data + Bytes)	{}
			void			Reset()		{ current = data; }
			char*			pointer()	{ return data; }
			const char*		pointer()	const	{ return data; }
			template <class C>
				void		Write(const C&out)	{ _PushData(&out, sizeof(C)); }
			template <class C>
				void		Write(const C*array, count_t count)	{ _PushData(array, sizeof(C)*count); }
			template <class C>
				ByteStreamT<Bytes>&	operator<<(const C&object) { Write(object); return *this; }
			bool			IsFull() const { return current == end; }

			virtual	bool	Write(const void*data, serial_size_t size)	override { _PushData(data, size); return true; }
		};

	class ByteReadStream : public IReadStream
	{
	private:
		const char		*begin,
						*current,
						*end;
	
		void			_GetData(void*pntr, size_t size);
	public:

		/**/			ByteReadStream() :begin(NULL), current(NULL), end(NULL)	{}
		/**/			ByteReadStream(const void*begin, const void*end) :begin((const char*)begin), current((const char*)begin), end((const char*)end)	{}
		/**/			ByteReadStream(const Array<BYTE>&data) : begin((const char*)data.begin()),current((const char*)data.begin()),end((const char*)data.end()){}
	
		void			Rewind()	{ current = begin; }
		bool			ReachedEnd() const { return current == end; }
		template <class C>
			bool		Stream(C&out)	{ return _GetData(&out, sizeof(C)); }
		template <class C>
			bool		Stream(C*array, count_t count)	{ return _GetData(array, sizeof(C)*count); }
		template <class C>
			ByteReadStream&	operator>>(C&object)	{ if (!Stream(object)) throw IO::GeneralFault("Unable to stream required object"); return *this; }

		virtual	void	Read(void*target_data, serial_size_t size)	override { _GetData(target_data, size); }
		virtual serial_size_t		GetRemainingBytes() const override { return end - current; }
	};


	#include "byte_stream.tpl.h"
}

#endif
