#ifndef callbackH
#define callbackH


#include "../global_root.h"

namespace DeltaWorks
{

	class Callback
	{
	public:
		class Context
		{};
		
		typedef void (Context::*pPlainMethod)();
		typedef void (Context::*pByteMethod)(BYTE);
		typedef void (Context::*pIntMethod)(int);
		typedef void (Context::*pPointerMethod)(void*);
	
		union
		{
			void	(*plainEvent)();
			void	(*byteEvent)(BYTE);
			void	(*intEvent)(int);
			void	(*pointerEvent)(void*);
			pPlainMethod	plainMethod;
			pByteMethod		byteMethod;
			pIntMethod		intMethod;
			pPointerMethod	pointerMethod;
		};
		enum Type
		{
			TypeUndef,
			TypePlain,
			TypeByte,
			TypeInt,
			TypePointer,
			TypePlainMethod,
			TypeByteMethod,
			TypeIntMethod,
			TypePointerMethod
		};
		
		Type		type;
		Context		*context;
					
					Callback():plainEvent(NULL),type(TypeUndef)
					{}
					Callback(void (*event)()):plainEvent(event),type(TypePlain)
					{}
					Callback(void (*event)(BYTE)):byteEvent(event),type(TypeByte)
					{}
					Callback(void (*event)(int)):intEvent(event),type(TypeInt)
					{}
					Callback(void (*event)(void*)):pointerEvent(event),type(TypePointer)
					{}
					Callback(Context*object, pPlainMethod event):plainMethod(event),type(TypePlainMethod),context(object)
					{}
					Callback(Context*object, pByteMethod event):byteMethod(event),type(TypeByteMethod),context(object)
					{}
					Callback(Context*object, pIntMethod event):intMethod(event),type(TypeIntMethod),context(object)
					{}
					Callback(Context*object, pPointerMethod event):pointerMethod(event),type(TypePointerMethod),context(object)
					{}
		
		bool		operator()(int parameter)	const
					{
						switch (type)
						{
							case TypePlain:
								plainEvent();
							return true;
							case TypeByte:
								byteEvent((BYTE)parameter);
							return true;
							case TypeInt:
								intEvent(parameter);
							return true;
							case TypePointer:
								pointerEvent((void*)parameter);
							return true;
							case TypePlainMethod:
								(context->*plainMethod)();
							return true;
							case TypeByteMethod:
								(context->*byteMethod)((BYTE)parameter);
							return true;
							case TypeIntMethod:
								(context->*intMethod)(parameter);
							return true;
							case TypePointerMethod:
								(context->*pointerMethod)((void*)parameter);
							return true;
						}
						return false;
					}
		bool		operator()(void* parameter)	const
					{
						switch (type)
						{
							case TypePlain:
								plainEvent();
							return true;
							case TypeByte:
								byteEvent((BYTE)(int)parameter);
							return true;
							case TypeInt:
								intEvent((int)parameter);
							return true;
							case TypePointer:
								pointerEvent(parameter);
							return true;
							case TypePlainMethod:
								(context->*plainMethod)();
							return true;
							case TypeByteMethod:
								(context->*byteMethod)((BYTE)(int)parameter);
							return true;
							case TypeIntMethod:
								(context->*intMethod)((int)parameter);
							return true;
							case TypePointerMethod:
								(context->*pointerMethod)(parameter);
							return true;
						}
						return false;
					}
		bool		operator==(const Callback&other)	const
					{
						return type == other.type && (type == TypeUndef || plainEvent == other.plainEvent) && (type < TypePlainMethod || context == other.context);
					}
		bool		isset()	const
					{
						return type != TypeUndef;
					}
	};
}


#endif
