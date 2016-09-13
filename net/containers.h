#ifndef netContainersH
#define netContainersH

#include "tcp.h"
#include "../third_party/bz2/bz2.h"
#include "../global_string.h"


namespace Package
{
	template <ISerializable::serial_size_t max_size>
		class NetString:public String
		{
		public:
			static const size_t	MaxSize = max_size;
		static	bool			IsValidChar(char c)
				{
					switch (c)
					{
						case 'ä':
						case 'ö':
						case 'ü':
						case 'Ä':
						case 'Ö':
						case 'Ü':
						case 'ß':
							return true;
					}
					bool result = isgraph((BYTE)c) || c == ' ' || c == '\t' || c == '\n';
					return result;
				}
		public:
			/**/				NetString()
								{}
			/**/				NetString(const String&string):String(string)
								{
									ASSERT1__(IsValid(*this),*this);
								}
			/**/				NetString(const char*string):String(string)
								{
									ASSERT1__(IsValid(*this),*this);
								}
			/**/				NetString(const StringRef&string):String(string)
								{
									ASSERT1__(IsValid(*this),*this);
								}

			static bool			IsValid(const String&str)
			{
				if (str.length() > max_size)
					return false;
				for (index_t i = 0; i < str.length(); i++)
					if (!IsValidChar(str.get(i)))
					{
						//cout << "invalid char '"<<
						return false;
					}
				return true;
			}
		
			virtual	bool		Deserialize(IReadStream&in_stream, serial_size_t fixed_size)	override
								{
									if (fixed_size == EmbeddedSize)
									{
										serial_size_t len;
										if (!in_stream.ReadSize(len))
											return false;

										fixed_size = len;
										//cout << size<<endl;
									}
									if (fixed_size > max_size)
										return false;
									bool result = String::Deserialize(in_stream,fixed_size);
									if (!result)
										return false;
									return IsValid(*this);
								}
				
				
				String::operator=;
		};


	
	template <typename T>
		class Native:public SerializableObject
		{
		public:
				T							value;
				
											Native():value()
											{}
											Native(T value_):value(value_)
											{}
											
			virtual	serial_size_t			GetSerialSize(bool export_size) const	override
			{
				return sizeof(T);
			}
				
			virtual	bool					Serialize(IWriteStream&out_stream, bool export_size) const	override
			{
				return 	out_stream.Write(&value,sizeof(value));
			}
				
			virtual	bool					Deserialize(IReadStream&in_stream, serial_size_t fixed_size)	override
			{
				//cout << "deserializing native type of size "<<sizeof(T)<<" and fixed_size parameter = "<<fixed_size<<endl;
				if (fixed_size != EmbeddedSize && fixed_size != sizeof(T))
					return false;
				//cout << "attempting to parse"<<endl;
				return	in_stream.Read(&value,sizeof(value));
			}

			virtual	bool					HasFixedSize() const	override
			{
				return true;
			}

			void							swap(Native<T>&other)		{swp(value,other.value);}
			void							adoptData(Native<T>&other)	{value = other.value;}
			
			void							operator|=(const T&other)	{value |= other;}
			void							operator&=(const T&other)	{value &= other;}
			void							operator+=(const T&other)	{value += other;}
			void							operator-=(const T&other)	{value -= other;}
			void							operator*=(const T&other)	{value *= other;}
			void							operator/=(const T&other)	{value /= other;}
			template <typename T1>
			bool							operator==(const T1&other) const
			{
				return value == other;
			}
//			T								operator&(const T&other) const	{return value&other;}
				
			Native<T>&					operator=(T id_)
			{
				value = id_;
				return *this;
			}
				
			operator T()	const
			{
				return value;
			}				
		};
		
	template <class C0, class C1>
		class Tupel:public SerializableObject
		{
		public:
			C0							a;
			C1							b;

			/**/						Tupel()	{}
			/**/						Tupel(const C0&a_, const C1&b_):a(a_),b(b_){}


		
			virtual	serial_size_t				GetSerialSize(bool export_size) const	override
			{
				if (b.HasFixedSize())
					return a.GetSerialSize(export_size) + b.GetSerialSize(true);
				if (a.HasFixedSize())
					return a.GetSerialSize(true) + b.GetSerialSize(export_size);
				return 	a.GetSerialSize(true) + b.GetSerialSize(true);
			}
				
			virtual	bool						HasFixedSize() const	override
			{
				return a.HasFixedSize() && b.HasFixedSize();
			}				
				
			virtual	bool						Serialize(IWriteStream&out_stream, bool export_size) const	override
			{
				if (b.HasFixedSize())
					return 	a.Serialize(out_stream,export_size) && b.Serialize(out_stream,true);
				if (a.HasFixedSize())
					return 	a.Serialize(out_stream,true) && b.Serialize(out_stream,export_size);
				return 	a.Serialize(out_stream,true) && b.Serialize(out_stream,true);
			}
				
			virtual	bool						Deserialize(IReadStream&in_stream, serial_size_t fixed_size)	override
			{
				if (fixed_size != EmbeddedSize)
				{
					if (a.HasFixedSize())
					{
						if (!a.Deserialize(in_stream,EmbeddedSize))
							return false;
						fixed_size -= a.GetSerialSize(true);
						return b.Deserialize(in_stream,fixed_size);
					}
					if (b.HasFixedSize())
					{
						if (!a.Deserialize(in_stream,fixed_size-b.GetSerialSize(true)))
							return false;
						return b.Deserialize(in_stream,EmbeddedSize);
					}
				}
				//cout << "deserializing tupel elements with embedded size"<<endl;
				return	a.Deserialize(in_stream,EmbeddedSize) && b.Deserialize(in_stream,EmbeddedSize);
			}

			void						swap(Tupel<C0,C1>&other)
										{
											a.swap(other.a);
											b.swap(other.b);
										}
			void						adoptData(Tupel<C0,C1>&other)
										{
											a.adoptData(other.a);
											b.adoptData(other.b);
										}

			Tupel<C0,C1>&				setA(const C0&a_)
										{
											a = a_;
											return *this;
										}

			Tupel<C0,C1>&				setB(const C1&b_)
										{
											b = b_;
											return *this;
										}
		};
	
	
		
	class CompressedString:public SerializableObject
	{
	public:
		Array<BYTE>		compressed;
		String			uncompressed;
		bool			is_compressed;

	
		void			swap(CompressedString&other)
						{
							compressed.swap(other.compressed);
							uncompressed.swap(other.uncompressed);
							swp(is_compressed,other.is_compressed);
						}
		void			adoptData(CompressedString&other)
						{
							compressed.adoptData(other.compressed);
							uncompressed.adoptData(other.uncompressed);
							is_compressed = other.is_compressed;
						}

		virtual	serial_size_t	GetSerialSize(bool export_size) const	override
		{
			if (is_compressed)
				return sizeof(is_compressed) + sizeof(UINT32)+compressed.GetSerialSize(export_size);
			return sizeof(is_compressed) + uncompressed.GetSerialSize(export_size);
		}
			
		virtual	bool			Serialize(IWriteStream&out_stream, bool export_size) const	override
		{
			if (!out_stream.Write(&is_compressed,sizeof(is_compressed)))
				return false;
			if (is_compressed)
			{
				UINT32 decompressed_size = UINT32(uncompressed.length());
				return out_stream.WriteSize(uncompressed.length()) && compressed.Serialize(out_stream,export_size);
			}
			return uncompressed.Serialize(out_stream,export_size);
		}
			
		virtual	bool			Deserialize(IReadStream&in_stream, serial_size_t fixed_size)	override
		{
			if (!in_stream.Read(&is_compressed,sizeof(is_compressed)))
				return false;
			if (fixed_size != EmbeddedSize)
				fixed_size -= sizeof(is_compressed);
			if (is_compressed)
			{
				serial_size_t	decompressed_size;
				if (!in_stream.ReadSize(decompressed_size))
					return false;
				if (fixed_size != EmbeddedSize)
					fixed_size -= GetSerialSizeOfSize(decompressed_size);
				if (!compressed.Deserialize(in_stream,fixed_size))
					return false;
				//cout << "extracted "<<compressed.GetContentSize()<<" compressed byte(s). uncompressed="<<decompressed_size<<endl;
				uncompressed.setLength(decompressed_size);
				serial_size_t result = (serial_size_t)BZ2::decompress(compressed.pointer(),compressed.GetContentSize(),uncompressed.mutablePointer(),decompressed_size);
				if (result != decompressed_size)
				{
					if (TCP::verbose)
						std::cout << "CompressedString::Deserialize(): decompression returned "<<result<<". expected "<<decompressed_size<<".ignoring package"<<std::endl;
					return false;
				}
				for (serial_size_t i = 0; i < decompressed_size; i++)
				{
					char c = uncompressed.get(i);
					if (!isgraph(c) && !isspace(c))
					{
						if (TCP::verbose)
							std::cout << "CompressedString::Deserialize(): encountered invalid character ("<<(int)c<<") at position "<<i<<"/"<<uncompressed.length()<<". ignoring package"<<std::endl;
						return false;
					}
				}
				return true;
			}
			return uncompressed.Deserialize(in_stream,fixed_size);
		}	
			
		void			set(const String&string)
		{
			uncompressed = string;
			update();
		}
			
		void			update()
		{
			static Array<BYTE>	buffer;
			if (buffer.length() < uncompressed.length())
				buffer.SetSize(uncompressed.length());
			size_t compressed_size = BZ2::compress(uncompressed.c_str(),uncompressed.length(),buffer.pointer(),buffer.length());
			is_compressed = compressed_size>0 && compressed_size+GetSerialSizeOfSize((serial_size_t)uncompressed.length()) < uncompressed.length();
			if (is_compressed)
				compressed.copyFrom(buffer.pointer(),(Arrays::count_t)compressed_size);
			else
				compressed.free();
		}
	
	};
	
	template <class C>
		class CompressedArray:public SerializableObject
		{
		public:
			Array<BYTE>		compressed,
							serialized;
			Array<C>		uncompressed;
			bool			is_compressed;

			void			swap(CompressedArray<C>&other)
							{
								compressed.swap(other.compressed);
								serialized.swap(other.serialized);
								uncompressed.swap(other.uncompressed);
								swp(is_compressed,other.is_compressed);
							}
			void			adoptData(CompressedArray<C>&other)
							{
								compressed.adoptData(other.compressed);
								serialized.adoptData(other.serialized);
								uncompressed.adoptData(other.uncompressed);
								is_compressed = other.is_compressed;
							}
		
			virtual	serial_size_t	GetSerialSize(bool export_size) const	override
							{
								if (is_compressed)
									return sizeof(is_compressed) + GetSerialSizeOfSize((serial_size_t)serialized.length())+compressed.GetSerialSize(export_size);
								//cout << (sizeof(is_compressed) + serialized.GetSerialSize(true))<<" count="<<uncompressed.count()<<endl;
								return sizeof(is_compressed) + serialized.GetSerialSize(export_size);
							}
				
			virtual	bool			Serialize(IWriteStream&out_stream, bool export_size) const	override
							{
								if (!out_stream.Write(&is_compressed,sizeof(is_compressed)))
									return false;
								if (is_compressed)
								{
									size_t decompressed_size = serialized.length();
									return out_stream.WriteSize(decompressed_size) && compressed.Serialize(out_stream,export_size);
								}
								//cout << "serialize count="<<uncompressed.count()<<endl;
								return serialized.Serialize(out_stream,export_size);
							}
				
			virtual	bool			Deserialize(IReadStream&in_stream, serial_size_t fixed_size)	override
							{
								if (!in_stream.Read(&is_compressed,sizeof(is_compressed)))
									return false;
								if (fixed_size != EmbeddedSize)
									fixed_size -= sizeof(is_compressed);
								//cout << "fixed size remaining: "<<fixed_size<<endl;
								if (is_compressed)
								{
									//cout << "compressed"<<endl;
									serial_size_t	serial_size;
									if (!in_stream.ReadSize(serial_size))
										return false;
									if (fixed_size != EmbeddedSize)
										fixed_size -= GetSerialSizeOfSize(serial_size);
									if (!compressed.Deserialize(in_stream,fixed_size))
										return false;
									serialized.SetSize(serial_size);
									if (BZ2::decompress(compressed.pointer(),compressed.GetContentSize(),serialized.pointer(),serial_size) != serial_size)
										return false;
									MemReadStream stream(serialized.pointer(),(serial_size_t)serialized.GetContentSize());
									return uncompressed.Deserialize(stream,EmbeddedSize);
								}
								//cout << "deserialized array is not compressed"<<endl;
								if (!serialized.Deserialize(in_stream,fixed_size))
									return false;
								//cout << "primary deserialization succeeded"<<endl;
								/*for (index_t i = 0; i < serialized.size(); i++)
								{
									if (Template::isalnum(serialized[i]))
										cout << ' '<<serialized[i];
									else
										cout << ' '<<String((UINT)serialized[i]);
								}
								cout << endl;*/
								MemReadStream	inner_read(serialized.pointer(),(serial_size_t)serialized.GetContentSize());
								return uncompressed.Deserialize(inner_read,EmbeddedSize);
							}	
				
			void			serializeAndCompress()
			{
				WriteBuffer<>	buffer;
				uncompressed.serialize(buffer,true);
				buffer.moveToArray(serialized);
				size_t compressed_size = serialized.length() > 256 ? BZ2::compress(serialized.pointer(),serialized.length(),buffer.pointer(),buffer.length()) : 0;
				is_compressed = compressed_size>0 && compressed_size < serialized.length();	//the latter comparison should be redundant...
				if (is_compressed)
				{
					compressed.SetSize(compressed_size);
					compressed.copyFrom(buffer.pointer());
				}
				else
					compressed.free();
			}
		
		};		
	}



DECLARE__(Package::CompressedString,Swappable);
DECLARE_T__(Package::CompressedArray,Swappable);
DECLARE_T__(Package::Native,Swappable);
DECLARE_T2__(Package::Tupel,typename,typename,Swappable);

namespace Strategy
{
template <ISerializable::serial_size_t max_size>
	class StrategySelector<Package::NetString<max_size> >
	{
	public:
	typedef	Strategy::Swappable	Default;
	};
}

#endif
