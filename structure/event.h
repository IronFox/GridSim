#ifndef eventH
#define eventH

#include "../global_string.h"
#include "../container/buffer.h"
#include <functional>

/**
	@brief Generalized event object

	An event executes all registered listeners if triggered. A listener can be either a simple function or method, with or without pointer parameter pointing to the domain object that caused the  event.
*/
template <class DomainClass>
	class Event
	{
	public:
			class Callback	//! Listener container
			{
			public:
					enum eType
					{
						None,
						BasicFunction,
						ExtendedFunction,
						BasicMethod,
						ExtendedMethod
					};
					
			protected:
					eType	type;
					union
					{
						void (*basic)();
						void (*extended)(DomainClass*);
						void (DW::Object::*method)();
						void (DW::Object::*extendedMethod)(DomainClass*);
					};
					DW::Object*	object;
			public:
									Callback():type(None)
									{}
									Callback(void(*pointer)()):type(pointer?BasicFunction:None),basic(pointer)
									{}
									Callback(void(*pointer)(DomainClass*)):type(pointer?ExtendedFunction:None),extended(pointer)
									{}
									Callback(DW::Object*object_,void(DW::Object::*pointer)(DomainClass*)):type(object_&&pointer?ExtendedMethod:None),extendedMethod(pointer),object(object_)
									{}
									Callback(DW::Object*object_,void(DW::Object::*pointer)()):type(object_&&pointer?BasicMethod:None),method(pointer),object(object_)
									{}
					bool			operator==(const Callback&other)	const
									{
										return type==other.type && (type==None || basic==other.basic) && (type<=ExtendedFunction || object==other.object);
									}
					void			execute(DomainClass*cl)	const
									{
										switch (type)
										{
											case BasicFunction:
												basic();
											break;
											case ExtendedFunction:
												extended(cl);
											break;
											case BasicMethod:
												(object->*method)();
											break;
											case ExtendedMethod:
												(object->*extendedMethod)(cl);
											break;
										}
									}
					bool			isset()	const
									{
										return type!=None;
									}
			};
				
	
	protected:
			Buffer<Callback,0>		callbacks;
	public:
			void					operator+=(const Callback&callback)
									{
										if (!callback.isset())
											return;
										if (callbacks.contains(callback))
											return;
										callbacks << callback;
									}
			void					operator-=(const Callback&callback)
									{
										callbacks.findAndErase(callback);
									}
			void					operator()(DomainClass*cl)	const
									{
										for (index_t i = 0; i < callbacks.count(); i++)
											callbacks[i].execute(cl);
									}
	};

	class FunctionalEvent
	{
	public:
		typedef std::function<void()>	Callback;
	protected:
		Buffer<Callback,0,Swap>			callbacks;
	public:
		void							operator+=(const Callback&callback)
										{
											callbacks << callback;
										}
		void							operator()()	const
										{
											for (index_t i = 0; i < callbacks.count(); i++)
												callbacks[i]();
										}
		void							clear()		{callbacks.reset();}

	};

#endif
