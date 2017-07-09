#ifndef netContainersH
#define netContainersH

#include "tcp.h"
#include "../third_party/bz2/bz2.h"
#include "../global_string.h"

namespace DeltaWorks
{



	namespace Package
	{
		template <serial_size_t max_size>
			class NetString:public String
			{
			public:
				typedef NetString<max_size>	Self;

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

				friend void			SerialSync(IReadStream&s,Self&v)
				{
					const serial_size_t len=s.ReadSize();
					if (len > max_size)
						throw Except::Memory::SerializationFault(CLOCATION,"Deserialized string length "+String(len)+" exceeds maximum safe size "+String(max_size));
					v.SetLength(len);
					s.Read(v.mutablePointer(),len);
					if (!IsValid(v))
						throw Except::Memory::SerializationFault(CLOCATION,"Deserialized string contains invalid characters");
				}
				
				
				String::operator=;
			};


			#if 0
		template <typename T>
			class Native:public SerializableObject
			{
				typedef Native<T>				Self;
			public:
				T								value;
				
												Native():value()
												{}
												Native(T value_):value(value_)
												{}
											
				virtual	serial_size_t			GetSerialSize() const	override
				{
					return sizeof(T);
				}
				
				virtual	void					Serialize(IWriteStream&outStream) const	override
				{
					outStream.Write(&value,sizeof(value));
				}
				
				virtual	void					Deserialize(IReadStream&inStream)	override
				{
					inStream.Read(&value,sizeof(value));
				}

				virtual	bool					HasFixedSize() const	override
				{
					return true;
				}

				void							swap(Native<T>&other)		{swp(value,other.value);}
				friend void						swap(Self&a, Self&b)		{a.swap(b);}
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
			#endif /*0*/
		
		template <class C0, class C1>
			class Tuple:public SerializableObject
			{
				typedef Tuple<C0,C1>		Self;
			public:
				C0							a;
				C1							b;

				/**/						Self()	{}
				/**/						Self(const C0&a_, const C1&b_):a(a_),b(b_){}


		
				virtual	serial_size_t				GetSerialSize() const	override
				{
					if (b.HasFixedSize())
						return a.GetSerialSize(export_size) + b.GetSerialSize();
					if (a.HasFixedSize())
						return a.GetSerialSize() + b.GetSerialSize(export_size);
					return 	a.GetSerialSize() + b.GetSerialSize();
				}
				
				virtual	bool						HasFixedSize() const	override
				{
					return a.HasFixedSize() && b.HasFixedSize();
				}				
				
				virtual	bool						Serialize(IWriteStream&outStream) const	override
				{
					if (b.HasFixedSize())
						return 	a.Serialize(outStream,export_size) && b.Serialize(outStream,true);
					if (a.HasFixedSize())
						return 	a.Serialize(outStream,true) && b.Serialize(outStream,export_size);
					return 	a.Serialize(outStream,true) && b.Serialize(outStream,true);
				}
				
				virtual	bool						Deserialize(IReadStream&inStream)	override
				{
					if (fixed_size != EmbeddedSize)
					{
						if (a.HasFixedSize())
						{
							if (!a.Deserialize(inStream))
								return false;
							fixed_size -= a.GetSerialSize();
							return b.Deserialize(inStream,fixed_size);
						}
						if (b.HasFixedSize())
						{
							if (!a.Deserialize(inStream,fixed_size-b.GetSerialSize()))
								return false;
							return b.Deserialize(inStream);
						}
					}
					//cout << "deserializing tupel elements with embedded size"<<endl;
					return	a.Deserialize(inStream) && b.Deserialize(inStream);
				}

				void						swap(Self&other)
											{
												a.swap(other.a);
												b.swap(other.b);
											}
				friend void					swap(Self&a, Self&b)		{a.swap(b);}
				void						adoptData(Self&other)
											{
												a.adoptData(other.a);
												b.adoptData(other.b);
											}

				Self&					setA(const C0&a_)
											{
												a = a_;
												return *this;
											}

				Self&						setB(const C1&b_)
											{
												b = b_;
												return *this;
											}
			};
	
	
		
		class CompressedString:public SerializableObject
		{
			typedef CompressedString	Self;
		public:
			Ctr::Array<BYTE>		compressed;
			String			uncompressed;
			bool			is_compressed;

	
			void			swap(CompressedString&other)
							{
								compressed.swap(other.compressed);
								uncompressed.swap(other.uncompressed);
								swp(is_compressed,other.is_compressed);
							}
			friend void		swap(Self&a, Self&b)		{a.swap(b);}
			void			adoptData(CompressedString&other)
							{
								compressed.adoptData(other.compressed);
								uncompressed.adoptData(other.uncompressed);
								is_compressed = other.is_compressed;
							}


			
			friend void SerialSync(IWriteStream&s, const CompressedString&v)
			{
				using Serialization::SerialSync;
				s.WritePrimitive(v.is_compressed);
				if (v.is_compressed)
				{
					UINT32 decompressed_size = UINT32(v.uncompressed.length());
					s.WriteSize(v.uncompressed.length());
					SerialSync(s,v.compressed);
				}
				else
					SerialSync(s,v.uncompressed);
			}
			
			friend void SerialSync(IReadStream&s, CompressedString&v)
			{
				using Serialization::SerialSync;
				s.ReadPrimitive(v.is_compressed);
				if (v.is_compressed)
				{
					const serial_size_t	decompressed_size = s.ReadSize();
					SerialSync(s,v.compressed);
					//cout << "extracted "<<compressed.GetContentSize()<<" compressed byte(s). uncompressed="<<decompressed_size<<endl;
					v.uncompressed.setLength(decompressed_size);
					const serial_size_t result = (serial_size_t)BZ2::decompress(v.compressed.pointer(),v.compressed.GetContentSize(),v.uncompressed.mutablePointer(),decompressed_size);
					if (result != decompressed_size)
						throw Except::Memory::SerializationFault(CLOCATION,"Decompression returned "+String(result)+". Expected "+String(decompressed_size)+".ignoring package");
				}
				else
					SerialSync(s,v.uncompressed);

				if (!NetString<256>::IsValid(v.uncompressed))
					throw Except::Memory::SerializationFault(CLOCATION,"Decompression string contains invalid character");
			}	
			
			void			set(const String&string)
			{
				uncompressed = string;
				update();
			}
			
			void			update()
			{
				static Ctr::Array<BYTE>	buffer;
				if (buffer.length() < uncompressed.length())
					buffer.SetSize(uncompressed.length());
				size_t compressed_size = BZ2::compress(uncompressed.c_str(),uncompressed.length(),buffer.pointer(),buffer.length());
				is_compressed = compressed_size>0 && compressed_size+SerialSizeScanner::GetSerialSizeOfSize((serial_size_t)uncompressed.length()) < uncompressed.length();
				if (is_compressed)
					compressed.copyFrom(buffer.pointer(),(count_t)compressed_size);
				else
					compressed.free();
			}
	
		};
	
		template <class C>
			class CompressedArray:public SerializableObject
			{
				typedef CompressedArray<C>		Self;
			public:
				Ctr::Array<BYTE>		compressed,
								serialized;
				Ctr::Array<C>		uncompressed;
				bool			is_compressed;

				void			swap(CompressedArray<C>&other)
								{
									compressed.swap(other.compressed);
									serialized.swap(other.serialized);
									uncompressed.swap(other.uncompressed);
									swp(is_compressed,other.is_compressed);
								}
				friend void		swap(Self&a, Self&b)		{a.swap(b);}
				void			adoptData(CompressedArray<C>&other)
								{
									compressed.adoptData(other.compressed);
									serialized.adoptData(other.serialized);
									uncompressed.adoptData(other.uncompressed);
									is_compressed = other.is_compressed;
								}
		
				virtual	serial_size_t	GetSerialSize() const	override
								{
									if (is_compressed)
										return sizeof(is_compressed) + GetSerialSizeOfSize((serial_size_t)serialized.length())+compressed.GetSerialSize(export_size);
									//cout << (sizeof(is_compressed) + serialized.GetSerialSize())<<" count="<<uncompressed.count()<<endl;
									return sizeof(is_compressed) + serialized.GetSerialSize(export_size);
								}
				
				virtual	bool			Serialize(IWriteStream&outStream) const	override
								{
									if (!outStream.Write(&is_compressed,sizeof(is_compressed)))
										return false;
									if (is_compressed)
									{
										size_t decompressed_size = serialized.length();
										return outStream.WriteSize(decompressed_size) && compressed.Serialize(outStream,export_size);
									}
									//cout << "serialize count="<<uncompressed.count()<<endl;
									return serialized.Serialize(outStream,export_size);
								}
				
				virtual	bool			Deserialize(IReadStream&inStream)	override
								{
									if (!inStream.Read(&is_compressed,sizeof(is_compressed)))
										return false;
									if (fixed_size != EmbeddedSize)
										fixed_size -= sizeof(is_compressed);
									//cout << "fixed size remaining: "<<fixed_size<<endl;
									if (is_compressed)
									{
										//cout << "compressed"<<endl;
										serial_size_t	serial_size;
										if (!inStream.ReadSize(serial_size))
											return false;
										if (fixed_size != EmbeddedSize)
											fixed_size -= GetSerialSizeOfSize(serial_size);
										if (!compressed.Deserialize(inStream,fixed_size))
											return false;
										serialized.SetSize(serial_size);
										if (BZ2::decompress(compressed.pointer(),compressed.GetContentSize(),serialized.pointer(),serial_size) != serial_size)
											return false;
										MemReadStream stream(serialized.pointer(),(serial_size_t)serialized.GetContentSize());
										return uncompressed.Deserialize(stream);
									}
									//cout << "deserialized array is not compressed"<<endl;
									if (!serialized.Deserialize(inStream,fixed_size))
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
									return uncompressed.Deserialize(inner_read);
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




	namespace Strategy
	{
	template <serial_size_t max_size>
		class StrategySelector<Package::NetString<max_size> >
		{
		public:
		typedef	Strategy::Swappable	Default;
		};
	}
}
DECLARE__(Package::CompressedString,Swappable);
DECLARE_T__(Package::CompressedArray,Swappable);
//DECLARE_T__(Package::Native,Swappable);
DECLARE_T2__(Package::Tuple,typename,typename,Swappable);

#endif
