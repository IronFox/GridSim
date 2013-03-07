#include "tcp.h"
#include "../remote/bz2/bz2.h"
#include "../global_string.h"


namespace Package
{
	template <ISerializable::serial_size_t max_size>
		class NetString:public String
		{
		protected:
		static	bool			validChar(char c)
				{
					return isgraph((BYTE)c) || isspace((BYTE)c);
				}
		public:
								NetString()
								{}
								NetString(const String&string):String(string)
								{}
								NetString(const char*string):String(string)
								{}
		
		virtual	bool			deserialize(IReadStream&in_stream, serial_size_t fixed_size)
								{
									if (fixed_size == EmbeddedSize)
									{
										serial_size_t len;
										if (!in_stream.readSize(len))
											return false;

										fixed_size = len;
										//cout << size<<endl;
									}
									if (fixed_size > max_size)
										return false;
									bool result = String::deserialize(in_stream,fixed_size);
									if (!result)
										return false;
									for (index_t i = 0; i < length(); i++)
										if (!validChar(get(i)))
										{
											//cout << "invalid char '"<<
											return false;
										}
									return true;
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
											
		virtual	serial_size_t				serialSize(bool export_size) const
				{
					return sizeof(T);
				}
				
		virtual	bool						serialize(IWriteStream&out_stream, bool export_size) const
				{
					return 	out_stream.write(&value,sizeof(value));
				}
				
		virtual	bool						deserialize(IReadStream&in_stream, serial_size_t fixed_size)
				{
					//cout << "deserializing native type of size "<<sizeof(T)<<" and fixed_size parameter = "<<fixed_size<<endl;
					if (fixed_size != EmbeddedSize && fixed_size != sizeof(T))
						return false;
					//cout << "attempting to parse"<<endl;
					return	in_stream.read(&value,sizeof(value));
				}
		virtual	bool						fixedSize() const
				{
					return true;
				}

				void						swap(Native<T>&other)		{swp(value,other.value);}
				void						adoptData(Native<T>&other)	{value = other.value;}
				
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

											Tupel()	{}
											Tupel(const C0&a_, const C1&b_):a(a_),b(b_){}


		
		virtual	serial_size_t				serialSize(bool export_size) const
				{
					if (b.fixedSize())
						return a.serialSize(export_size) + b.serialSize(true);
					if (a.fixedSize())
						return a.serialSize(true) + b.serialSize(export_size);
					return 	a.serialSize(true) + b.serialSize(true);
				}
				
		virtual	bool						fixedSize() const
				{
					return a.fixedSize() && b.fixedSize();
				}				
				
		virtual	bool						serialize(IWriteStream&out_stream, bool export_size) const
				{
					if (b.fixedSize())
						return 	a.serialize(out_stream,export_size) && b.serialize(out_stream,true);
					if (a.fixedSize())
						return 	a.serialize(out_stream,true) && b.serialize(out_stream,export_size);
					return 	a.serialize(out_stream,true) && b.serialize(out_stream,true);
				}
				
		virtual	bool						deserialize(IReadStream&in_stream, serial_size_t fixed_size)
				{
					if (fixed_size != EmbeddedSize)
					{
						if (a.fixedSize())
						{
							if (!a.deserialize(in_stream,EmbeddedSize))
								return false;
							fixed_size -= a.serialSize(true);
							return b.deserialize(in_stream,fixed_size);
						}
						if (b.fixedSize())
						{
							if (!a.deserialize(in_stream,fixed_size-b.serialSize(true)))
								return false;
							return b.deserialize(in_stream,EmbeddedSize);
						}
					}
					//cout << "deserializing tupel elements with embedded size"<<endl;
					return	a.deserialize(in_stream,EmbeddedSize) && b.deserialize(in_stream,EmbeddedSize);
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

	virtual	serial_size_t	serialSize(bool export_size) const
			{
				if (is_compressed)
					return sizeof(is_compressed) + sizeof(UINT32)+compressed.serialSize(export_size);
				return sizeof(is_compressed) + uncompressed.serialSize(export_size);
			}
			
	virtual	bool			serialize(IWriteStream&out_stream, bool export_size) const
			{
				if (!out_stream.write(&is_compressed,sizeof(is_compressed)))
					return false;
				if (is_compressed)
				{
					UINT32 decompressed_size = UINT32(uncompressed.length());
					return out_stream.writeSize(uncompressed.length()) && compressed.serialize(out_stream,export_size);
				}
				return uncompressed.serialize(out_stream,export_size);
			}
			
	virtual	bool			deserialize(IReadStream&in_stream, serial_size_t fixed_size)
			{
				if (!in_stream.read(&is_compressed,sizeof(is_compressed)))
					return false;
				if (fixed_size != EmbeddedSize)
					fixed_size -= sizeof(is_compressed);
				if (is_compressed)
				{
					serial_size_t	decompressed_size;
					if (!in_stream.readSize(decompressed_size))
						return false;
					if (fixed_size != EmbeddedSize)
						fixed_size -= serialSizeOfSize(decompressed_size);
					if (!compressed.deserialize(in_stream,fixed_size))
						return false;
					//cout << "extracted "<<compressed.contentSize()<<" compressed byte(s). uncompressed="<<decompressed_size<<endl;
					uncompressed.setLength(decompressed_size);
					serial_size_t result = (serial_size_t)BZ2::decompress(compressed.pointer(),compressed.contentSize(),uncompressed.mutablePointer(),decompressed_size);
					if (result != decompressed_size)
					{
						if (TCP::verbose)
							cout << "CompressedString::deserialize(): decompression returned "<<result<<". expected "<<decompressed_size<<".ignoring package"<<endl;
						return false;
					}
					for (serial_size_t i = 0; i < decompressed_size; i++)
					{
						char c = uncompressed.get(i);
						if (!isgraph(c) && !isspace(c))
						{
							if (TCP::verbose)
								cout << "CompressedString::deserialize(): encountered invalid character ("<<(int)c<<") at position "<<i<<"/"<<uncompressed.length()<<". ignoring package"<<endl;
							return false;
						}
					}
					return true;
				}
				return uncompressed.deserialize(in_stream,fixed_size);
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
					buffer.setSize(uncompressed.length());
				size_t compressed_size = BZ2::compress(uncompressed.c_str(),uncompressed.length(),buffer.pointer(),buffer.length());
				is_compressed = compressed_size>0 && compressed_size+serialSizeOfSize((serial_size_t)uncompressed.length()) < uncompressed.length();
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
		
		virtual	serial_size_t	serialSize(bool export_size) const
				{
					if (is_compressed)
						return sizeof(is_compressed) + serialSizeOfSize((serial_size_t)serialized.length())+compressed.serialSize(export_size);
					//cout << (sizeof(is_compressed) + serialized.serialSize(true))<<" count="<<uncompressed.count()<<endl;
					return sizeof(is_compressed) + serialized.serialSize(export_size);
				}
				
		virtual	bool			serialize(IWriteStream&out_stream, bool export_size) const
				{
					if (!out_stream.write(&is_compressed,sizeof(is_compressed)))
						return false;
					if (is_compressed)
					{
						size_t decompressed_size = serialized.length();
						return out_stream.writeSize(decompressed_size) && compressed.serialize(out_stream,export_size);
					}
					//cout << "serialize count="<<uncompressed.count()<<endl;
					return serialized.serialize(out_stream,export_size);
				}
				
		virtual	bool			deserialize(IReadStream&in_stream, serial_size_t fixed_size)
				{
					if (!in_stream.read(&is_compressed,sizeof(is_compressed)))
						return false;
					if (fixed_size != EmbeddedSize)
						fixed_size -= sizeof(is_compressed);
					//cout << "fixed size remaining: "<<fixed_size<<endl;
					if (is_compressed)
					{
						//cout << "compressed"<<endl;
						serial_size_t	serial_size;
						if (!in_stream.readSize(serial_size))
							return false;
						if (fixed_size != EmbeddedSize)
							fixed_size -= serialSizeOfSize(serial_size);
						if (!compressed.deserialize(in_stream,fixed_size))
							return false;
						serialized.setSize(serial_size);
						if (BZ2::decompress(compressed.pointer(),compressed.contentSize(),serialized.pointer(),serial_size) != serial_size)
							return false;
						MemReadStream stream(serialized.pointer(),(serial_size_t)serialized.contentSize());
						return uncompressed.deserialize(stream,EmbeddedSize);
					}
					//cout << "deserialized array is not compressed"<<endl;
					if (!serialized.deserialize(in_stream,fixed_size))
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
					MemReadStream	inner_read(serialized.pointer(),(serial_size_t)serialized.contentSize());
					return uncompressed.deserialize(inner_read,EmbeddedSize);
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
						compressed.setSize(compressed_size);
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
