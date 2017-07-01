#ifndef net_remote_shellH
#define net_remote_shellH


#include "tcp.h"
#include <functional>


namespace RemoteShell
{
		namespace Type
		{
			enum Code
			{
				Void,
				List,
				Bool,
				String,
				Float,
				Int8,
				Int16,
				Int32
			};
		};

		namespace Channel
		{
			enum ID
			{
				ClientModeChange = 1,
				FolderChange = 3,
				ExecuteCommand = 10,
				VariableData = 5,	//server->client: send command return/requested variable value. client->server: update variable
				VariableName = 6,	//server->client: 
		1:	[mode:UINT8]				issues mode change. currently supported modes are 0:passive and 1:active
										(no response)
		3:	[path:STRING]				requests folder change to the specified relative path
										-> ch3 on success
										-> ch2:0 on failure
		4:	[path:STRING][value:DATA]	executes remote command
										-> ch5 on success
										-> ch2:4 on failure
		5:	[path:STRING][value:DATA]	updates the specified variable
										-> ch6 on success
										-> ch2:5 on failure
		6:	[path:STRING]				requests variable data
										-> ch5 on success
										-> ch2:2 on failure
		7:	[path:STRING]				requests contents of the specified folder
										-> ch7,ch8,ch9 on success
										-> ch2:3 on failure



			};

		};

		class ShellItem
		{
		private:
				String			var_path,
								var_name;
				friend class RemoteShell;
		public:
		virtual					~ShellItem()	{}

				const String&	name()	const	{return var_name;}
				const String&	path()	const	{return var_path;}
		};

		class AbstractVariable : public ISerializable, public ShellItem
		{};

		class AbstractCommand : public ShellItem
		{
		public:
		virtual	void			execute(IReadStream&parameters, IWriteStream&result) = 0;
		};

	template <typename T>
		class Serializer
		{};

	template <>
		class Serializer<void>
		{
		public:
		static	bool	serialize(bool value, IWriteStream&result)
				{
					return result.writePrimitive((BYTE)Type::Void);
				}

		};

	template <>
		class Serializer<bool>
		{
		public:
		static	bool		serialize(bool value, IWriteStream&result)
				{
					return	result.writePrimitive((BYTE)Type::Bool)
							&&
							result.writePrimitive(value);
				}
		static	size_t		serialSize(bool)
				{
					return 2;
				}
		};
	template <>
		class Serializer<int>
		{
		public:
		static	bool		serialize(int value, IWriteStream&result)
				{
					if (value <= 127 && value >= -128)
					{
						return	result.writePrimitive((BYTE)Type::Int8)
								&& result.writePrimitive((char)value);
					}
					if (value <= 32767 && value >= -32768)
					{
						return	result.writePrimitive((BYTE)Type::Int16)
								&& result.writePrimitive((short)value);
					}
					return	result.writePrimitive((BYTE)Type::Int32)
							&& result.writePrimitive(value);
				}
		static	size_t		serialSize(int value)
				{
					if (value <= 127 && value >= -128)
					{
						return 2;
					}
					else
						if (value <= 32767 && value >= -32768)
						{
							return 3;
						}
						else
						{
							return 5;
						}
				}

		};
	template <>
		class Serializer<float>
		{
		public:
		static	bool		serialize(float value, IWriteStream&result)
				{
					return	result.writePrimitive((BYTE)Type::Float)
							&& result.writePrimitive(value);
				}
		static	size_t		serialSize(float)
				{
					return 5;
				}
		};
	template <>
		class Serializer<String>
		{
		public:
		static	bool		serialize(const String&value, IWriteStream&result)
				{
					return	result.writePrimitive((BYTE)Type::String)
							&& value.serialize(result,true);
				}
		static	size_t		serialSize(const String&value)
				{
					return value.serialSize(true) + 1;
				}

		};

	template <typename T, count_t Dim>
		class Serializer<TVec<T,Dim> >
		{
		public:
		static	bool		serialize(const TVec<T,Dim>&value, IWriteStream&result)
				{
					if (!result.writePrimitive((BYTE)Type::List)
						|| !result.writeSize(Dim))
						return false;
					for (index_t i = 0; i < Dim; i++)
						if (!Serializer<T>::serialize(value.v[i],result))
							return false;
					return true;
				}
		static	size_t		serialSize(const TVec<T,Dim>&value)
				{
					size_t result = 1+serialSizeOfSize(Dim);
					for (index_t i = 0; i < Dim; i++)
						result += Serializer<T>::serialSize(value.v[i]);
					return result;
				}
		};

	template <typename T>
		class Serializer<TMatrix4<T> >
		{
		public:
		static	bool		serialize(const TMatrix4<T>&value, IWriteStream&result)
				{
					if (!result.writePrimitive((BYTE)Type::List)
						|| !result.writeSize(16))
						return false;
					for (index_t i = 0; i < 16; i++)
						if (!Serializer<T>::serialize(value.v[i],result))
							return false;
					return true;
				}
		static	size_t		serialSize(const TMatrix4<T>&value)
				{
					size_t result = 1+serialSizeOfSize(16);
					for (index_t i = 0; i < 16; i++)
						result += Serializer<T>::serialSize(value.v[i]);
					return result;
				}
		};
	template <typename T>
		class Serializer<TMatrix3<T> >
		{
		public:
		static	bool		serialize(const TMatrix3<T>&value, IWriteStream&result)
				{
					if (!result.writePrimitive((BYTE)Type::List)
						|| !result.writeSize(9))
						return false;
					for (index_t i = 0; i < 9; i++)
						if (!Serializer<T>::serialize(value.v[i],result))
							return false;
					return true;
				}
		static	size_t		serialSize(const TMatrix3<T>&value)
				{
					size_t result = 1+serialSizeOfSize(9);
					for (index_t i = 0; i < 9; i++)
						result += Serializer<T>::serialSize(value.v[i]);
					return result;
				}
		};
	template <typename T>
		class Serializer<TMatrix2<T> >
		{
		public:
		static	bool		serialize(const TMatrix2<T>&value, IWriteStream&result)
				{
					if (!result.writePrimitive((BYTE)Type::List)
						|| !result.writeSize(4))
						return false;
					for (index_t i = 0; i < 4; i++)
						if (!Serializer<T>::serialize(value.v[i],result))
							return false;
					return true;
				}
		static	size_t		serialSize(const TMatrix2<T>&value)
				{
					size_t result = 1+serialSizeOfSize(4);
					for (index_t i = 0; i < 4; i++)
						result += Serializer<T>::serialSize(value.v[i]);
					return result;
				}
		};


	template <typename T>
		class Serializer<ArrayData<T> >
		{
		public:
		static	bool		serialize(const ArrayData<T>&value, IWriteStream&result)
				{
					if (!result.writePrimitive((BYTE)Type::List)
						|| !result.writeSize(value.length()))
						return false;
					for (index_t i = 0; i < value.length(); i++)
						if (!Serializer<T>::serialize(value[i],result))
							return false;
					return true;
				}
		static	size_t		serialSize(const ArrayData<T>&value)
				{
					size_t result = 1+serialSizeOfSize(value.length());
					for (index_t i = 0; i < value.length(); i++)
						result += Serializer<T>::serialSize(value[i]);
					return result;
				}
		};
	template <typename T, typename Strategy>
		class Serializer<Array<T,Strategy> > : public Serializer<ArrayData<T> >
		{};


	template <typename T>
		class ListToTypeDeserializer
		{
		public:
		static	void	deserialize(count_t num_elements, IReadStream&stream, T&out);
		};

	template <typename T>
		class ListToTypeDeserializer<TVec2<T> >
		{
		public:
		static	void	deserialize(count_t num_elements, IReadStream&stream, TVec2<T>&out);
		};
	template <typename T>
		class ListToTypeDeserializer<TVec3<T> >
		{
		public:
		static	void	deserialize(count_t num_elements, IReadStream&stream, TVec3<T>&out);
		};
	template <typename T>
		class ListToTypeDeserializer<TVec4<T> >
		{
		public:
		static	void	deserialize(count_t num_elements, IReadStream&stream, TVec4<T>&out);
		};
	template <typename T>
		class ListToTypeDeserializer<TMat2<T> >
		{
		public:
		static	void	deserialize(count_t num_elements, IReadStream&stream, TMat2<T>&out);
		};
	template <typename T>
		class ListToTypeDeserializer<TMat3<T> >
		{
		public:
		static	void	deserialize(count_t num_elements, IReadStream&stream, TMat3<T>&out);
		};
	template <typename T>
		class ListToTypeDeserializer<TMat4<T> >
		{
		public:
		static	void	deserialize(count_t num_elements, IReadStream&stream, TMat4<T>&out);
		};
	template <typename T>
		class ListToTypeDeserializer<ArrayData<T> >
		{
		public:
				void	deserialize(count_t num_elements, IReadStream&stream, ArrayData<T>&out);
		};
	template <typename T, class Strategy>
		class ListToTypeDeserializer<Array<T,Strategy> > : public ListToTypeDeserializer<ArrayData<T> >
		{};

	template <typename T>
		class Deserializer1
		{
		public:
		static	void	deserialize(IReadStream&stream, T&out)
				{
					BYTE type;
					if (!stream.readPrimitive(type))
						throw Exception("Unable to read parameter type from stream");
					switch (type)
					{
						default:
							throw Exception("Unexpected parameter type encountered");
						return;
						case Type::List:
						{
							count_t	num_elements;
							if (!stream.readSize(num_elements))
								throw Exception("Unable to read list size from stream");
							ListToTypeDeserializer<T>::deserialize(num_elements,stream,out);
						}
						return;
						case Type::Bool:
						{
							bool b;
							if (!stream.readPrimitive(b))
								throw Exception("Unable to read value from stream");
							Caster<bool,T>::cast(b,out);
						}
						return;
						case Type::Int8:
						{
							char v;
							if (!stream.readPrimitive(v))
								throw Exception("Unable to read value from stream");
							Caster<char,T>::cast(v,out);
						}
						return;
						case Type::Int16:
						{
							short v;
							if (!stream.readPrimitive(v))
								throw Exception("Unable to read value from stream");
							Caster<short,T>::cast(v,out);
						}
						return;
						case Type::Int32:
						{
							int v;
							if (!stream.readPrimitive(v))
								throw Exception("Unable to read value from stream");
							Caster<int,T>::cast(v,out);
						}
						return;
						case Type::Float:
						{
							float v;
							if (!stream.readPrimitive(v))
								throw Exception("Unable to read value from stream");
							Caster<float,T>::cast(v,out);
						}
						return;
					}
				}
		};

	template <typename T>
		void	ListToTypeDeserializer<T>::deserialize(count_t num_elements,IReadStream&stream, T&out)
		{
			//generic type (no vector, matrix, array, or otherwise)

			if (num_elements != 1)
				throw Exception("Expected one element in list, but encountered "+String(num_elements)+" element(s)");
			Deserializer1<T>::deserialize(stream,out);
		}

	template <typename T>
		void	ListToTypeDeserializer<TVec2<T> >::deserialize(count_t num_elements, IReadStream&stream, TVec2<T>&out)
		{
			if (num_elements != 2)
				throw Exception("Expected two elements in list, but encountered "+String(num_elements)+" element(s)");
			Deserializer1<T>::deserialize(stream,out.x);
			Deserializer1<T>::deserialize(stream,out.y);
		}
	template <typename T>
		void	ListToTypeDeserializer<TVec3<T> >::deserialize(count_t num_elements, IReadStream&stream, TVec3<T>&out)
		{
			if (num_elements != 3)
				throw Exception("Expected three elements in list, but encountered "+String(num_elements)+" element(s)");
			Deserializer1<T>::deserialize(stream,out.x);
			Deserializer1<T>::deserialize(stream,out.y);
			Deserializer1<T>::deserialize(stream,out.z);
		}
	template <typename T>
		void	ListToTypeDeserializer<TVec4<T> >::deserialize(count_t num_elements, IReadStream&stream, TVec4<T>&out)
		{
			if (num_elements != 4)
				throw Exception("Expected four elements in list, but encountered "+String(num_elements)+" element(s)");
			Deserializer1<T>::deserialize(stream,out.x);
			Deserializer1<T>::deserialize(stream,out.y);
			Deserializer1<T>::deserialize(stream,out.z);
			Deserializer1<T>::deserialize(stream,out.w);
		}
	template <typename T>
		void	ListToTypeDeserializer<TMat2<T> >::deserialize(count_t num_elements, IReadStream&stream, TMat2<T>&out)
		{
			if (num_elements != 4)
				throw Exception("Expected four elements in list, but encountered "+String(num_elements)+" element(s)");
			for (index_t i = 0; i < 4; i++)
				Deserializer1<T>::deserialize(stream,out.v[i]);
		}
	template <typename T>
		void	ListToTypeDeserializer<TMat3<T> >::deserialize(count_t num_elements, IReadStream&stream, TMat3<T>&out)
		{
			if (num_elements != 9)
				throw Exception("Expected 9 elements in list, but encountered "+String(num_elements)+" element(s)");
			for (index_t i = 0; i < 9; i++)
				Deserializer1<T>::deserialize(stream,out.v[i]);
		}
	template <typename T>
		void	ListToTypeDeserializer<TMat4<T> >::deserialize(count_t num_elements, IReadStream&stream, TMat4<T>&out)
		{
			if (num_elements != 16)
				throw Exception("Expected 16 elements in list, but encountered "+String(num_elements)+" element(s)");
			for (index_t i = 0; i < 16; i++)
				Deserializer1<T>::deserialize(stream,out.v[i]);
		}
	template <typename T>
		void	ListToTypeDeserializer<ArrayData<T> >::deserialize(count_t num_elements, IReadStream&stream, ArrayData<T>&out)
		{
			out.setSize(num_elements);
			for (index_t i = 0; i < num_elements; i++)
				Deserializer1<T>::deserialize(stream,out[i]);
		}


	template <typename T0, typename T1>
		class Deserializer2
		{
		public:
		static	void	deserialize(IReadStream&stream, T0&out0, T1&out1)
				{
					BYTE type;
					if (!stream.readPrimitive(type))
						throw Exception("Unable to read parameter type from stream");
					if (type != Type::List)
						throw Exception("Expected list type for 2 parameters");
					count_t	num_elements;
					if (!stream.readSize(num_elements))
						throw Exception("Unable to read list size from stream");
					if (num_elements != 2)
						throw Exception("Unexpected list size encountered. Expected 2 elements.");
					Deserializer1<T0>::deserialize(stream,out0);
					Deserializer1<T1>::deserialize(stream,out1);
				}
		};
	template <typename T0, typename T1, typename T2>
		class Deserializer3
		{
		public:
		static	void	deserialize(IReadStream&stream, T0&out0, T1&out1, T2&out2)
				{
					BYTE type;
					if (!stream.readPrimitive(type))
						throw Exception("Unable to read parameter type from stream");
					if (type != Type::List)
						throw Exception("Expected list type for 3 parameters");
					count_t	num_elements;
					if (!stream.readSize(num_elements))
						throw Exception("Unable to read list size from stream");
					if (num_elements != 3)
						throw Exception("Unexpected list size encountered. Expected 3 elements.");
					Deserializer1<T0>::deserialize(stream,out0);
					Deserializer1<T1>::deserialize(stream,out1);
					Deserializer1<T2>::deserialize(stream,out2);
				}
		};
	template <typename T0, typename T1, typename T2, typename T3>
		class Deserializer4
		{
		public:
		static	void	deserialize(IReadStream&stream, T0&out0, T1&out1, T2&out2, T3&out3)
				{
					BYTE type;
					if (!stream.readPrimitive(type))
						throw Exception("Unable to read parameter type from stream");
					if (type != Type::List)
						throw Exception("Expected list type for 4 parameters");
					count_t	num_elements;
					if (!stream.readSize(num_elements))
						throw Exception("Unable to read list size from stream");
					if (num_elements != 4)
						throw Exception("Unexpected list size encountered. Expected 4 elements.");
					Deserializer1<T0>::deserialize(stream,out0);
					Deserializer1<T1>::deserialize(stream,out1);
					Deserializer1<T2>::deserialize(stream,out2);
					Deserializer1<T3>::deserialize(stream,out3);
				}
		};
	template <typename T0, typename T1, typename T2, typename T3, typename T4>
		class Deserializer5
		{
		public:
		static	void	deserialize(IReadStream&stream, T0&out0, T1&out1, T2&out2, T3&out3, T4&out4)
				{
					BYTE type;
					if (!stream.readPrimitive(type))
						throw Exception("Unable to read parameter type from stream");
					if (type != Type::List)
						throw Exception("Expected list type for 5 parameters");
					count_t	num_elements;
					if (!stream.readSize(num_elements))
						throw Exception("Unable to read list size from stream");
					if (num_elements != 5)
						throw Exception("Unexpected list size encountered. Expected 5 elements.");
					Deserializer1<T0>::deserialize(stream,out0);
					Deserializer1<T1>::deserialize(stream,out1);
					Deserializer1<T2>::deserialize(stream,out2);
					Deserializer1<T3>::deserialize(stream,out3);
					Deserializer1<T4>::deserialize(stream,out4);
				}
		};
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		class Deserializer6
		{
		public:
		static	void	deserialize(IReadStream&stream, T0&out0, T1&out1, T2&out2, T3&out3, T4&out4, T5&out5)
				{
					BYTE type;
					if (!stream.readPrimitive(type))
						throw Exception("Unable to read parameter type from stream");
					if (type != Type::List)
						throw Exception("Expected list type for 6 parameters");
					count_t	num_elements;
					if (!stream.readSize(num_elements))
						throw Exception("Unable to read list size from stream");
					if (num_elements != 6)
						throw Exception("Unexpected list size encountered. Expected 6 elements.");
					Deserializer1<T0>::deserialize(stream,out0);
					Deserializer1<T1>::deserialize(stream,out1);
					Deserializer1<T2>::deserialize(stream,out2);
					Deserializer1<T3>::deserialize(stream,out3);
					Deserializer1<T4>::deserialize(stream,out4);
					Deserializer1<T5>::deserialize(stream,out5);
				}
		};


	template <typename F>
		class TemplateCommand : public AbstractCommand
		{};

	template <typename R>
		class TemplateCommand<R()> : public AbstractCommand
		{
		public:
				function<R()>	func;

		virtual	void			execute(IReadStream&parameters, IWriteStream&result)	override
				{
					R	rs = func();
					Serializer<R>::serialize(rs,result);
				}
		};
	template <typename R, typename P0>
		class TemplateCommand<R(P0)> : public AbstractCommand
		{
		public:
				function<R(P0)>	func;

		virtual	void			execute(IReadStream&parameters, IWriteStream&result)	override
				{
					P0	p0;
					Deserializer1<P0>::deserialize(parameters,p0);
					R	rs = func(p0);
					Serializer<R>::serialize(rs,result);
				}
		};
	template <typename R, typename P0, typename P1>
		class TemplateCommand<R(P0,P1)> : public AbstractCommand
		{
		public:
				function<R(P0,P1)>	func;

		virtual	void			execute(IReadStream&parameters, IWriteStream&result)	override
				{
					P0	p0;
					P1	p1;
					Deserializer2<P0,P1>::deserialize(parameters,p0,p1);
					R	rs = func(p0,p1);
					Serializer<R>::serialize(rs,result);
				}
		};
	template <typename R, typename P0, typename P1, typename P2>
		class TemplateCommand<R(P0,P1,P2)> : public AbstractCommand
		{
		public:
				function<R(P0,P1,P2)>	func;

		virtual	void			execute(IReadStream&parameters, IWriteStream&result)	override
				{
					P0	p0;
					P1	p1;
					P2	p2;
					Deserializer3<P0,P1,P2>::deserialize(parameters,p0,p1,p2);
					R	rs = func(p0,p1,p2);
					Serializer<R>::serialize(rs,result);
				}
		};
	template <typename R, typename P0, typename P1, typename P2, typename P3>
		class TemplateCommand<R(P0,P1,P2,P3)> : public AbstractCommand
		{
		public:
				function<R(P0,P1,P2,P3)>	func;

		virtual	void			execute(IReadStream&parameters, IWriteStream&result)	override
				{
					P0	p0;
					P1	p1;
					P2	p2;
					P3	p3;
					Deserializer3<P0,P1,P2>::deserialize(parameters,p0,p1,p2,p3);
					R	rs = func(p0,p1,p2,p3);
					Serializer<R>::serialize(rs,result);
				}
		};
	template <typename R, typename P0, typename P1, typename P2, typename P3, typename P4>
		class TemplateCommand<R(P0,P1,P2,P3,P4)> : public AbstractCommand
		{
		public:
				function<R(P0,P1,P2,P3,P4)>	func;

		virtual	void			execute(IReadStream&parameters, IWriteStream&result)	override
				{
					P0	p0;
					P1	p1;
					P2	p2;
					P3	p3;
					P4	p4;
					Deserializer3<P0,P1,P2>::deserialize(parameters,p0,p1,p2,p3,p4);
					R	rs = func(p0,p1,p2,p3,p4);
					Serializer<R>::serialize(rs,result);
				}
		};



		template <typename T>
			class TemplateVariable : public AbstractVariable
			{
			public:
			virtual	T							getValue()	const=0;
			virtual	void						setValue(const T&value)	{throw Exception(var_name+" does not implement a set operation");}

			private:
			virtual	serial_size_t				serialSize(bool export_size) const	override
			{
				return	var_path.serialSize(true)
						+
						Serializer<T>::serialSize(getValue());
			}
			virtual	bool						serialize(IWriteStream&out_stream, bool export_size) const	override
			{
				return	var_path.serialize(out_stream,true)
						&&
						Serializer<T>::serialize(getValue(),out_stream);
			}

			virtual	bool						deserialize(IReadStream&in_stream, serial_size_t fixed_size)	override
			{
				try
				{
					T	value:
					Deserializer1<T>::deserialize(in_stream,value);
					setValue(value);
					return true;
				}
				catch (const std::exception&e)
				{
					return false;
				}

			}
			virtual	bool						fixedSize() const override {return true;}
			};


		class VariableFeed : public Array<BYTE>, public IReadStream
		{
		private:
				const BYTE*			read_cursor;
				String				var_path;
		public:
									VariableFeed(serial_size_t fixed_size):Array<BYTE>(fixed_size)
									{
										ASSERT_NOT_EQUAL__(fixed_size,ISerializable::EmbeddedSize);
										read_cursor = pointer();
									}
				bool				feed(IReadStream&source)
									{
										read_cursor = pointer();
										return  source.read(pointer(),size())
												&& var_path.deserialize(*this,ISerializable::EmbeddedSize);
									}

				const String&		path()	const	{return var_path;}
		virtual	bool				read(void*target_data, serial_size_t size)	override
									{
										if (read_cursor + size > end())
											return false;
										memcpy(target_data,read_cursor,size);
										read_cursor += size;
										return true;
									}

		};

		class VariableChannel : public TCP::RootChannel
		{
		public:
				typedef TCP::RootChannel	Super;

									VariableChannel(UINT32 channel_id):Super(channel_id)
									{}

				bool				send(TCP::Destination*destination, const AbstractVariable&var)
									{
										Super::sendObject(destination,var);
									}
				bool				send(TCP::Destination*destination, TCP::Peer*exclude, const AbstractVariable&var)
									{
										Super::sendObject(destination,exclude,var);
									}

							
									/**
										@brief Deserializes the stream into a new object

										This method is called asynchronously by the respective connection's worker thread.
									*/
		virtual	SerializableObject*	deserialize(IReadStream&stream,serial_size_t fixed_size,TCP::Peer*sender)	override
									{
										VariableFeed*result = new VariableFeed(fixed_size);
										if (!result->feed(stream))
										{
											delete result;
											return NULL;
										}
										return result;
									}
									/**
										@brief Dispatches a successfully deserialized object (returned by deserialize() )

										The method is invoked only, if deserialize() returned a new non-NULL object.
										Depending on whether the connection is synchronous or asynchronous, this method is called my the main thread, or the connection's worker thread.
									*/
		virtual	void				handle(SerializableObject*serializable,TCP::Peer*sender)	override;
									/*{
										FeedData*data = (FeedData*)serializable;
										String var_path;
										var_path.deserialize(*data,ISerializable::EmbeddedSize);


									}*/


		};

		
	template<typename F>
		void		reg(const String&path, const function<F>&func);

		void		reg(const String&path, const shared_ptr<AbstractVariable>&var);
		void		reg(const String&path, AbstractVariable*var)			{reg(path,shared_ptr<AbstractVariable>(var));}

		void		unreg(const String&path);

};





#endif
