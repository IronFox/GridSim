#include "../global_string.h"
#include "../container/hashtable.h"
#include "../interface/read_stream.h"
#include "../string/encoding.h"
#include <functional>


namespace DeltaWorks
{

	/**
	General purpose JSON handler.
	All strings are encoded in UTF8 without exception
	*/
	namespace JSON
	{
		class Entity;
		class Object;
	
	

		struct LineCounter
		{
			index_t				inLine = 1;


			void				Feed(char);

		};


		class Entity
		{
		public:
			enum type_t
			{
				Int64,	//Number
				Double,	//Number
				StringValue,
				Bool,
				ArrayValue,
				ObjectValue,
				Null
			};
		
			/**/	Entity(){}
			String	CastToString() const;
			INT64	CastToInt() const;
			double	CastToDouble() const;
		
			void			Clear();
			void			SetNull() {Clear();}
		
			void			Set(const String&string)
			{
				Clear();
				type = StringValue;
				stringValue = string;
			}
		
			void			Set(INT64 v)
			{
				Clear();
				type = Int64;
				intBoolValue = v;
			}
		
			void			Set(double v)
			{
				Clear();
				type = Double;
				doubleValue = v;
			}
		
			void			Set(bool b)
			{
				Clear();
				type = Bool;
				intBoolValue = b;
			}
		
			Object&			SetObject();
		
			Ctr::Array<Entity>&	SetArray()
			{
				Clear();
				type = ArrayValue;
				return arrayValue;
			}

			void			swap(Entity&other)
			{
				swp(type,other.type);
				stringValue.swap(other.stringValue);
				arrayValue.swap(other.arrayValue);
				objectValue.swap(other.objectValue);
				swp(intBoolValue,other.intBoolValue);
				swp(doubleValue,other.doubleValue);
			}
		
			type_t			GetType() const {return type;}
			const String&	AsString() const {return stringValue;}
			INT64			AsInt() const {return intBoolValue;}
			bool			AsBool() const {return intBoolValue!=0;}
			const Ctr::Array<Entity>& AsArray() const {return arrayValue;}
			const Object&	AsObject() const {return *objectValue;}
			double			AsDouble() const {return doubleValue;}
		
		private:
			type_t			type = Null;
			String			stringValue;
			Ctr::Array<Entity>	arrayValue;
			std::shared_ptr<Object>			objectValue;
			INT64			intBoolValue=0;
			double			doubleValue=0;
		
		};
	
	
		class Object
		{
			Container::StringTable<Entity>	entities;
		public:
	
			void		Clear()	{entities.Clear();}

			void		swap(Object&other)
			{
				entities.swap(other.entities);
			}

			void		Deserialize(IReadStream&stream,LineCounter&lineCounter);
			void		Deserialize(IReadStream&stream);
		};
	

	
	
	
	
	}
}
