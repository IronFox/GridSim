#ifndef exceptionH
#define exceptionH

/*#include <iostream>
using namespace std;*/

/******************************************************************

General exception-container.

******************************************************************/


//#include <global_string.h>

/*
__DATE__  The date the file is compiled; for example, "Jul 14, 1995". This symbol is a predefined macro. 
__FILE__  The name of the file being compiled; for example "prog.c". This symbol is a predefined macro. 
__func__ The name of the function currently being compiled. This predefined identifier is only available under the emerging ANSI/ISO C99 standard. 
__LINE__  The number of the line being compiled (before including any header files). This symbol is a predefined macro. 
__TIME__  The time the file is compiled in 24-hour format; for example, "13:01:45". This symbol is a predefined macro. 
__STDC__  
*/


#include <exception>
#include <iostream>
#include "../interface/string.h"

#ifdef __BORLANDC__
    #define __func__    __FUNC__
#elif  defined(_MSC_VER)
	#define	__func__	__FUNCTION__
#endif


#if SYSTEM==UNIX
	#include <signal.h>
#endif



struct TCodeLocation
{
	char			formatted[512];
    
					TCodeLocation()
					{
						formatted[0] = 0;
					};
                    TCodeLocation(const char*file, const char*method, unsigned line)
					{
						//std::cout << "determining code location for "<<file<<"/"<<method<<" ["<<line<<"]"<<std::endl;
						const char	*in = file,
									*last = file;

						char		modded_file[512];

						char		*out = modded_file;
							
						
						while (*in)
						{
							if (*in == '/' || *in == '\\')
							{
								if (in-last == 2)
								{
									if (last[0] == '.' && last[1] == '.' && out-modded_file>=2)
									{
										out -= 2;
										while (out > modded_file && *out != '/' && *out != '\\')
											out--;
									}
								}
								last = in+1;
							}
							if (out-modded_file >= sizeof(modded_file)-2)
								break;
							(*out++) = (*in++);
						}
						(*out++) = 0;
						unsigned cnt = 0;
						while (out > modded_file && cnt < 2)
						{
							out--;
							if (*out == '/' || *out == '\\')
								cnt++;
						}
						if (cnt == 2)
							out++;
						//std::cout << " ... "<<out<<std::endl;
						
						sprintf_s(formatted,"line %i of %s::%s()",line,out,method);
						//std::cout << " ... "<<formatted<<std::endl;


						char buffer[0x100];
						//std::cin.getline(buffer,sizeof(buffer));
					};
	const char*		ToString()	const
                    {
						return formatted;
					}
};



#include <string.h>

#if 0

	extern TCodeLocation	exception_history[1000];
	extern unsigned			exception_history_length;
	
	#define FUNC_BEGIN		{if (exception_history_length >= ARRAYSIZE(exception_history)) FATAL__("Except history overflow"); exception_history[exception_history_length].file = __FILE__; exception_history[exception_history_length].method = __func__; exception_history[exception_history_length++] = __LINE__;}
	#define FUNC_END		{exception_history_length--;}
#else
	#define FUNC_BEGIN
	#define FUNC_END
#endif






class FatalDescriptor:public TCodeLocation
{
private:
		char		message[10000],
					formatted[11000];

		
		void		copy(const char*message, size_t length) throw()
					{
						if (length > sizeof(this->message)-1)
							length = sizeof(this->message)-1;
						memcpy(this->message,message,length);
						this->message[length] = 0;
						sprintf_s(formatted,"Fatal exception in %s\n\"%s\"",TCodeLocation::formatted,this->message);
					}
public:
                    FatalDescriptor(const TCodeLocation&location, const char*msg):TCodeLocation(location)
                    {
						copy(msg,strlen(msg));
                    }

template <class C>  FatalDescriptor(const TCodeLocation&location, const C&msg):TCodeLocation(location)
                    {
						/*msg.printArchitecture(cout); cout << endl;
						msg.print(cout); cout << endl;*/
						char*terminal = msg.writeTo(message,message+sizeof(message)-1);
						//cout << (unsigned)(terminal-message)<<endl;
						
						*terminal = 0;
						
						//cout << message << endl;
						
						
						sprintf(formatted,"Fatal exception in %s\n\"%s\"",TCodeLocation::formatted,message);
						
						//cout << formatted<<endl;
                    }

	const char*		ErrorMessage()	const throw()
                    {
						return formatted;
                    }
	const char*		ToString()	const throw()
					{
						return formatted;
					}
	const char*		getMessage()	const throw()	{return message;}
};


/**
	@brief Except collection
 
	This namespace provides various exceptions for different applications
 */
namespace Except
{
	/**
	 @brief Fatal exception, which should always lead to program termination
	 */
	class Fatal:public ::FatalDescriptor, public std::exception
	{
	public:
						Fatal(const TCodeLocation&location, const char*msg):FatalDescriptor(location,msg),std::exception(ToString(),1)
						{}
	template <class C>  Fatal(const TCodeLocation&location, const C&msg):FatalDescriptor(location,msg),std::exception(ToString(),1)
						{}

	};



	/**
		@brief This type tell exceptions that the provided string is global and should be referenced rather than copied
		
		Use like a function: throw Except::Runtime(Except::globalString("Hello world"));
	 */
	struct globalString
	{
		const char		*address;

						globalString(const char*addr):address(addr)
						{}
	};


	/**
		@brief Except name using constant arrays and automatic concatenation
		
		No dynamic memory allocation is used

	*/
	class Name
	{
	private:
			char			string[0x200];
			size_t			length;
	public:
			
							Name(const char*single)
							{
								if (!single)
								{
									string[0] = 0;
									length = 0;
								}
								else
								{
									length = strlen(single);
									if (length >= sizeof(string))
										length = sizeof(string)-1;
									memcpy(string,single,length);
									string[length] = 0;
								}
							}
							Name(const Name&parent_name, const char*child_name)
							{
								if (!child_name)
								{
									memcpy(string,parent_name.string,parent_name.length);
									length = parent_name.length;
									string[length] = 0;
								}
								else
								{
									memcpy(string,parent_name.string,parent_name.length);
									length = parent_name.length;
									
									size_t len = strlen(child_name);
									if (length+len >= sizeof(string))
										len = sizeof(string)-length-1;
									memcpy(string+length,child_name,len);
									length+=len;
									string[length] = 0;
								}

							}
	
							operator const char*()	const	{return string;}
	};





	//namespace Runtime
	//{

		/**
			@brief Generic runtime exception
		 */
		class Exception:public std::exception
		{
		protected:
			template <typename T>
				static inline const char*		castToChar(const IString<T>&string)
				{
					static StringConverter<char,0x1000>	converter;
					return converter.convert(string);
				}
		public:
										Exception()
										{
										}
										Exception(const char*message):std::exception(message)
										{
										}
										Exception(const globalString&string):std::exception(string.address,1)
										{
										}
										Exception(const std::exception&except):std::exception(except)
										{
										}
		template <class C>				Exception(const IString<C>&string):std::exception(castToChar(string))
										{
										}
		virtual	const char*				getType()	const {return staticGetName();}
		virtual	const char*				what()		const	{static char buffer[0x4000]; sprintf_s<sizeof(buffer)>(buffer,"%s: %s",getType(),std::exception::what()); buffer[sizeof(buffer)-1] = 0; return buffer; };
		static const Name&				staticGetName()	{static const Name	name = Name("Except"); return name;}

				const std::exception&	c_str()	const	{return *this;}		//ok, this is close to a hack. let's forget i had to do this :P

		};



		
		typedef Exception		Inherit;



		#undef BEGIN_EXCEPTION_GROUP
		#define BEGIN_EXCEPTION_GROUP(_name_)\
			struct PreType##_name_: public Inherit\
			{\
													PreType##_name_()\
													{\
													}\
													PreType##_name_(const char*message):Inherit(message)\
													{\
													}\
													PreType##_name_(const globalString&string):Inherit(string)\
													{\
													}\
				template <class C>					PreType##_name_(const IString<C>&string):Inherit(string)\
													{\
													}\
				static const Name&					staticGetName()	{static const Name name=Name(Inherit::staticGetName(),"/"#_name_); return name;}\
				virtual	const char*					getType()	const						{return staticGetName();}\
			};\
			struct _name_: public PreType##_name_\
			{\
				typedef PreType##_name_	Inherit;\
													_name_()\
													{\
													}\
													_name_(const char*message):Inherit(message)\
													{\
													}\
													_name_(const globalString&string):Inherit(string)\
													{\
													}\
				template <class C>					_name_(const IString<C>&string):Inherit(string)\
													{\
													}

		#undef END_EXCEPTION_GROUP
		#define END_EXCEPTION_GROUP\
			};


		#undef DEFINE_MEMBER_EXCEPTION
		#define DEFINE_MEMBER_EXCEPTION(_name_)\
					BEGIN_EXCEPTION_GROUP(_name_)\
					END_EXCEPTION_GROUP


		/**
			@brief Program logic related exceptions
		 */
		BEGIN_EXCEPTION_GROUP(Program)
			/**
				@brief General program logic fault
			 */
			DEFINE_MEMBER_EXCEPTION(GeneralFault)
			/**
				@brief An function or method internal self-test has failed
			 */
			DEFINE_MEMBER_EXCEPTION(InternalVerificationFault)
			/**
				@brief A method or function's functionality is not yet implemented (or may never be)
			 */
			DEFINE_MEMBER_EXCEPTION(FunctionalityNotImplemented)

			/**
				@brief A method or function invocation could not satisfy a requested functionality
				
				This most typically revolves around dynamic functionality requests via parameter.
			 */
			DEFINE_MEMBER_EXCEPTION(UnsupportedRequest)

			/**
				@brief Program execution exception due to invalid parameters
				*/
			DEFINE_MEMBER_EXCEPTION(ParameterFault)

			/**
				@brief Program execution exception due to a parameter not being unique (this generally applies to names during insert operations)
				*/
			DEFINE_MEMBER_EXCEPTION(UniquenessViolation)

			/**
				@brief The internal data consistency of a module is compromised
				*/
			DEFINE_MEMBER_EXCEPTION(DataConsistencyFault)

			/**
				@brief The resquested member element could not be found
			*/
			DEFINE_MEMBER_EXCEPTION(MemberNotFound)

		END_EXCEPTION_GROUP

		/**
			@brief Memory management related exceptions
		 */
		BEGIN_EXCEPTION_GROUP(Memory)
			/**
				@brief General memory fault
			 */
			DEFINE_MEMBER_EXCEPTION(GeneralFault)

		END_EXCEPTION_GROUP

		/**
			@brief Rendering related exceptions
		 */
		BEGIN_EXCEPTION_GROUP(Renderer)
			/**
				@brief General rendering exception
			 */
			DEFINE_MEMBER_EXCEPTION(GeneralFault)

			DEFINE_MEMBER_EXCEPTION(ShaderRejected)
			/**
				@brief Texture transfer (upload or download) related exceptions
			 */
			BEGIN_EXCEPTION_GROUP(TextureTransfer)
				/**
					@brief General texture transfer exception
				 */
				DEFINE_MEMBER_EXCEPTION(GeneralFault)
				/**
					@brief Texture transfer exception due to invalid parameters
				 */
				DEFINE_MEMBER_EXCEPTION(ParameterFault)

			END_EXCEPTION_GROUP

					


			/**
				@brief Geometry transfer (upload or download) related exceptions
			 */
			BEGIN_EXCEPTION_GROUP(GeometryTransfer)
				/**
					@brief General geometry transfer exception
				 */
				DEFINE_MEMBER_EXCEPTION(GeneralFault)
				/**
					@brief Geometry transfer exception due to invalid parameters
				 */
				DEFINE_MEMBER_EXCEPTION(ParameterFault)

			END_EXCEPTION_GROUP

		END_EXCEPTION_GROUP


		/**
			@brief IO (storage or network) related exceptions
		 */
		BEGIN_EXCEPTION_GROUP(IO)

			/**
				@brief General IO fault
			 */
			DEFINE_MEMBER_EXCEPTION(GeneralFault)

			/**
				@brief IO exception due to invalid parameters
				*/
			DEFINE_MEMBER_EXCEPTION(ParameterFault)


			/**
				@brief The assembly of a complex resource structure from various sources has failed
				
				This exception would be thrown if a resource structure combines various different
				 data structures and finds that some of them do not match. It may also be used if
				 the current data composition is not fit for export or processing in general.
			*/
			DEFINE_MEMBER_EXCEPTION(StructureCompositionFault)


			/**
				@brief Network (UDP, TCP, Pipe) related exceptions
			*/
			BEGIN_EXCEPTION_GROUP(Network)
				/**
					@brief General network fault
				 */
				DEFINE_MEMBER_EXCEPTION(GeneralFault)

				DEFINE_MEMBER_EXCEPTION(HostNotFound)

				DEFINE_MEMBER_EXCEPTION(ConnectionFailed)

				DEFINE_MEMBER_EXCEPTION(ConnectionLost)

			END_EXCEPTION_GROUP

			/**
				@brief Drive (HDD or removable) IO related exceptions
			 */
			BEGIN_EXCEPTION_GROUP(DriveAccess)
				/**
					@brief General drive fault
				 */
				DEFINE_MEMBER_EXCEPTION(GeneralFault)

				DEFINE_MEMBER_EXCEPTION(FileOpenFault)

				/**
					@brief File reading failed
					*/
				DEFINE_MEMBER_EXCEPTION(DataReadFault)

				/**
					@brief File writing failed
					*/
				DEFINE_MEMBER_EXCEPTION(DataWriteFault)


				/**
					@brief File data not formatted as expected
				 */
				DEFINE_MEMBER_EXCEPTION(FileFormatFault)

				
				/**
					@brief File data not as expected
					
					May also apply to data already loaded and stored.
					DataFault should be thrown where FileFormatFault is inappropriate
				 */
				DEFINE_MEMBER_EXCEPTION(FileDataFault)


			END_EXCEPTION_GROUP

		END_EXCEPTION_GROUP

	//}

}

using namespace Except;




#if SYSTEM==WINDOWS
    #define DISPLAY__(_MESSAGE_)    {MessageBoxA(NULL,(_MESSAGE_),"Fatal Error",MB_OK|MB_ICONERROR);}
#elif SYSTEM==UNIX
	#include <iostream>
    #define DISPLAY__(_MESSAGE_)    {std::cout<<"Fatal Error\n"<<(_MESSAGE_)<<std::endl;};
#endif

#define CLOCATION   TCodeLocation((const char*)__FILE__,__func__,__LINE__)


namespace Except
{
	extern	void (*onFatal)(const FatalDescriptor&descriptor);

	void	endFatal();		//!< Leaves synchronizes fatal invokation
	bool	enterFatalPhase();	//!< Attempts to enter fatal phase and return false if such is not possible. Only the first thread is allowed to enter fatal phase. All following threads are rejected
	bool	isInFatalPhase();	//!< Returns true if the local thread is currently in fatal phase

	void	holyShit(const char*message);		//!< This is when shit hit the fan, most commonly somewhere where fatal event handling is no longer possible. Immediately shuts down application
	void	terminateApplication();				//!< Immediately terminates the application

	template <class C>
		inline void	finalizeFatalPhase(const TCodeLocation&location, const C&message)
		{
			if (!isInFatalPhase())
				return;

				FatalDescriptor	descriptor(location,message);
				onFatal(descriptor);
				terminateApplication();
			endFatal();
		}


	//#define EBACK__         throw(C_Exception)

	/**
		@brief Invokes fatal exception handler, then terminates application.

		Only the first thread to call this function, actually invokes the handler. Subsequent invocations of this function by any thread are rejected
	*/
	template <class C>
		inline void fatal(const TCodeLocation&location, const C&message)
		{
			if (!enterFatalPhase())
			{
				exit(-1);	//exit this thread
				return;
			}
			finalizeFatalPhase(location,message);
		}
}

#define FATAL__(_MESSAGE_)					{if (Except::enterFatalPhase()) Except::finalizeFatalPhase(CLOCATION,(_MESSAGE_));}

#define THROW_FATAL__(_message_)			throw Except::Fatal(CLOCATION,message);
#define THROW_FATAL_EXCEPTION__(_message_)	THROW_FATAL__(_message_)



#define ASSERT__(_EXPRESSION_)	{if (!(_EXPRESSION_)) FATAL__("[" #_EXPRESSION_ "] evaluates to false");}
#define ASSERT_NOT_NULL__(_EXPRESSION_)	{if ((_EXPRESSION_)==NULL) FATAL__("[" #_EXPRESSION_ "] evaluates to NULL");}
#define ASSERT_NOT_NULL1__(_EXPRESSION_,_PARAMETER0_)	{if ((_EXPRESSION_)==NULL) FATAL__(#_EXPRESSION_ " (with "#_PARAMETER0_"='"+String(_PARAMETER0_)+"') evaluates to NULL");}
#define ASSERT_IS_NULL__(_EXPRESSION_)	{if ((_EXPRESSION_)!=NULL) FATAL__("[" #_EXPRESSION_ "] evaluates to not NULL");}

#define ASSERT1__(_EXPRESSION_,_PARAMETER0_)	{if (!(_EXPRESSION_)) FATAL__(#_EXPRESSION_ " (with "#_PARAMETER0_"='"+String(_PARAMETER0_)+"') evaluates to false");}
#define ASSERT2__(_EXPRESSION_,_PARAMETER0_,_PARAMETER1_)	{if (!(_EXPRESSION_)) FATAL__(#_EXPRESSION_ " (with "#_PARAMETER0_"='"+String(_PARAMETER0_)+"' and "#_PARAMETER1_"='"+String(_PARAMETER1_)+"') evaluates to false");}
#define ASSERT3__(_EXPRESSION_,_PARAMETER0_,_PARAMETER1_,_PARAMETER2_)	{if (!(_EXPRESSION_)) FATAL__( #_EXPRESSION_ " (with "#_PARAMETER0_"='"+String(_PARAMETER0_)+"', "#_PARAMETER1_"='"+String(_PARAMETER1_)+"', and "#_PARAMETER2_"='"+String(_PARAMETER2_)+"') evaluates to false");}
#define ASSERT4__(_EXPRESSION_,_PARAMETER0_,_PARAMETER1_,_PARAMETER2_,_PARAMETER3_)	{if (!(_EXPRESSION_)) FATAL__( #_EXPRESSION_ " (with "#_PARAMETER0_"='"+String(_PARAMETER0_)+"', "#_PARAMETER1_"='"+String(_PARAMETER1_)+"', "#_PARAMETER2_"='"+String(_PARAMETER2_)+"', and "#_PARAMETER3_"='"+String(_PARAMETER3_)+"') evaluates to false");}

#define CONSTRAINT_VIOLATION3__(component,vector,lower_bounds,upper_bounds)	FATAL__("Constraint violation of component "#component" of vector "#vector" ("+String((vector)[0])+", "+String((vector)[1])+", "+String((vector)[2])+") against constraint ["+String(lower_bounds)+", "+String(upper_bounds)+"]")
#define ASSERT_COMPONENT_IS_CONSTRAINED3__(component,vector,lower_bounds,upper_bounds)	if ((vector)[component]<(lower_bounds) || (vector)[component]>(upper_bounds)) CONSTRAINT_VIOLATION3__(component,vector,lower_bounds,upper_bounds)
#define	ASSERT_IS_CONSTRAINED3__(vector,lower_bounds,upper_bounds)	{ASSERT_COMPONENT_IS_CONSTRAINED3__(0,vector,lower_bounds,upper_bounds);ASSERT_COMPONENT_IS_CONSTRAINED3__(1,vector,lower_bounds,upper_bounds);ASSERT_COMPONENT_IS_CONSTRAINED3__(2,vector,lower_bounds,upper_bounds);}
#define	ASSERT_IS_CONSTRAINED__(value,lower_bounds,upper_bounds)	{if ((value)<(lower_bounds) || (value)>(upper_bounds)) FATAL__("Constraint violation of "#value" ("+String(value)+") against constraint ["+String(lower_bounds)+", "+String(upper_bounds)+"]");}

#define	ASSERT_EQUAL__(exp0,exp1)						ASSERT2__((exp0)==(exp1),exp0,exp1)
#define	ASSERT_EQUAL1__(exp0,exp1,exp2)					ASSERT3__((exp0)==(exp1),exp0,exp1,exp2)
#define	ASSERT_EQUAL2__(exp0,exp1,exp2,exp3)			ASSERT4__((exp0)==(exp1),exp0,exp1,exp2,exp3)

#define	ASSERT_NOT_EQUAL__(exp0,exp1)					ASSERT2__((exp0)!=(exp1),exp0,exp1)
#define	ASSERT_NOT_EQUAL1__(exp0,exp1,exp2)				ASSERT3__((exp0)!=(exp1),exp0,exp1,exp2)
#define	ASSERT_NOT_EQUAL2__(exp0,exp1,exp2,exp3)		ASSERT4__((exp0)!=(exp1),exp0,exp1,exp2,exp3)

#define	ASSERT_LESS__(exp0,exp1)						ASSERT2__((exp0)<(exp1),exp0,exp1)
#define	ASSERT_LESS1__(exp0,exp1,exp2)					ASSERT3__((exp0)<(exp1),exp0,exp1,exp2)
#define	ASSERT_LESS2__(exp0,exp1,exp2,exp3)				ASSERT4__((exp0)<(exp1),exp0,exp1,exp2,exp3)

#define	ASSERT_GREATER__(exp0,exp1)						ASSERT2__((exp0)>(exp1),exp0,exp1)
#define	ASSERT_GREATER1__(exp0,exp1,exp2)				ASSERT3__((exp0)>(exp1),exp0,exp1,exp2)
#define	ASSERT_GREATER2__(exp0,exp1,exp2,exp3)			ASSERT4__((exp0)>(exp1),exp0,exp1,exp2,exp3)

#define	ASSERT_LESS_OR_EQUAL__(exp0,exp1)				ASSERT2__((exp0)<=(exp1),exp0,exp1)
#define	ASSERT_LESS_OR_EQUAL1__(exp0,exp1,exp2)			ASSERT3__((exp0)<=(exp1),exp0,exp1,exp2)
#define	ASSERT_LESS_OR_EQUAL2__(exp0,exp1,exp2,exp3)	ASSERT4__((exp0)<=(exp1),exp0,exp1,exp2,exp3)

#define	ASSERT_GREATER_OR_EQUAL__(exp0,exp1)			ASSERT2__((exp0)>=(exp1),exp0,exp1)
#define	ASSERT_GREATER_OR_EQUAL1__(exp0,exp1,exp2)		ASSERT3__((exp0)>=(exp1),exp0,exp1,exp2)
#define	ASSERT_GREATER_OR_EQUAL2__(exp0,exp1,exp2,exp3)	ASSERT4__((exp0)>=(exp1),exp0,exp1,exp2,exp3)


#ifdef _DEBUG
	#define DBG_ASSERT__			ASSERT__
	#define DGB_ASSERT1__(exp0,exp1)	ASSERT1__(exp0,exp1)
	#define DBG_ASSERT2__			ASSERT2__
	#define DBG_ASSERT3__			ASSERT3__
	#define DBG_ASSERT4__			ASSERT4__

	#define DBG_VERIFY__			ASSERT__
	#define DGB_VERIFY1__			ASSERT1__
	#define DBG_VERIFY2__			ASSERT2__
	#define DBG_VERIFY3__			ASSERT3__
	#define DBG_VERIFY4__			ASSERT4__

	#define DBG_ASSERT_NOT_NULL__	ASSERT_NOT_NULL__
	#define DBG_ASSERT_NOT_NULL1__	ASSERT_NOT_NULL1__
	#define DBG_ASSERT_IS_NULL__	ASSERT_IS_NULL__

	#define DBG_VERIFY_NOT_NULL__	ASSERT_NOT_NULL__
	#define DBG_VERIFY_NOT_NULL1__	ASSERT_NOT_NULL1__
	#define DBG_VERIFY_IS_NULL__	ASSERT_IS_NULL__

	#define DBG_CONSTRAINT_VIOLATION3__				CONSTRAINT_VIOLATION3__
	#define DBG_ASSERT_COMPONENT_IS_CONSTRAINED3__	ASSERT_COMPONENT_IS_CONSTRAINED3__
	#define	DBG_ASSERT_IS_CONSTRAINED3__			ASSERT_IS_CONSTRAINED3__
	#define	DBG_ASSERT_IS_CONSTRAINED__				ASSERT_IS_CONSTRAINED__

	#define	DBG_ASSERT_EQUAL__		ASSERT_EQUAL__
	#define	DBG_ASSERT_EQUAL1__		ASSERT_EQUAL1__
	#define	DBG_ASSERT_EQUAL2__		ASSERT_EQUAL2__

	#define	DBG_ASSERT_NOT_EQUAL__	ASSERT_NOT_EQUAL__
	#define	DBG_ASSERT_NOT_EQUAL1__	ASSERT_NOT_EQUAL1__
	#define	DBG_ASSERT_NOT_EQUAL2__	ASSERT_NOT_EQUAL2__

	#define	DBG_ASSERT_LESS__		ASSERT_LESS__
	#define	DBG_ASSERT_LESS1__		ASSERT_LESS1__
	#define	DBG_ASSERT_LESS2__		ASSERT_LESS2__

	#define	DBG_ASSERT_GREATER__	ASSERT_GREATER__
	#define	DBG_ASSERT_GREATER1__	ASSERT_GREATER1__
	#define	DBG_ASSERT_GREATER2__	ASSERT_GREATER2__

	#define	DBG_ASSERT_LESS_OR_EQUAL__	ASSERT_LESS_OR_EQUAL__
	#define	DBG_ASSERT_LESS_OR_EQUAL1__	ASSERT_LESS_OR_EQUAL1__
	#define	DBG_ASSERT_LESS_OR_EQUAL2__	ASSERT_LESS_OR_EQUAL2__

	#define	DBG_ASSERT_GREATER_OR_EQUAL__	ASSERT_GREATER_OR_EQUAL__
	#define	DBG_ASSERT_GREATER_OR_EQUAL1__	ASSERT_GREATER_OR_EQUAL1__
	#define	DBG_ASSERT_GREATER_OR_EQUAL2__	ASSERT_GREATER_OR_EQUAL2__

#else
	#define DBG_ASSERT__(exp0)
	#define DGB_ASSERT1__(exp0,exp1)
	#define DBG_ASSERT2__(exp0,exp1,exp2)
	#define DBG_ASSERT3__(exp0,exp1,exp2,exp3)
	#define DBG_ASSERT4__(exp0,exp1,exp2,exp3,exp4)

	#define DBG_VERIFY__(exp0)						exp0
	#define DGB_VERIFY1__(exp0,exp1)				exp0
	#define DBG_VERIFY2__(exp0,exp1,exp2)			exp0
	#define DBG_VERIFY3__(exp0,exp1,exp2,exp3)		exp0
	#define DBG_VERIFY4__(exp0,exp1,exp2,exp3,exp4)	exp0

	#define DBG_ASSERT_NOT_NULL__(exp0)
	#define DBG_ASSERT_NOT_NULL1__(exp0,exp1)
	#define DBG_ASSERT_IS_NULL__(exp0)

	#define DBG_VERIFY_NOT_NULL__(exp0)				exp0
	#define DBG_VERIFY_NOT_NULL1__(exp0,exp1)		exp0
	#define DBG_VERIFY_IS_NULL__(exp0)				exp0

	#define DBG_CONSTRAINT_VIOLATION3__(exp0,exp1,exp2,exp3)
	#define DBG_ASSERT_COMPONENT_IS_CONSTRAINED3__(exp0,exp1,exp2,exp3)
	#define	DBG_ASSERT_IS_CONSTRAINED3__(exp0,exp1,exp2,exp3)
	#define	DBG_ASSERT_IS_CONSTRAINED__(exp0,exp1,exp2,exp3)

	#define	DBG_ASSERT_EQUAL__(exp0,exp1)
	#define	DBG_ASSERT_EQUAL1__(exp0,exp1,exp2)
	#define	DBG_ASSERT_EQUAL2__(exp0,exp1,exp2,exp3)

	#define	DBG_ASSERT_NOT_EQUAL__(exp0,exp1)
	#define	DBG_ASSERT_NOT_EQUAL1__(exp0,exp1,exp2)
	#define	DBG_ASSERT_NOT_EQUAL2__(exp0,exp1,exp2,exp3)

	#define	DBG_ASSERT_LESS__(exp0,exp1)
	#define	DBG_ASSERT_LESS1__(exp0,exp1,exp2)
	#define	DBG_ASSERT_LESS2__(exp0,exp1,exp2,exp3)

	#define	DBG_ASSERT_GREATER__(exp0,exp1)
	#define	DBG_ASSERT_GREATER1__(exp0,exp1,exp2)
	#define	DBG_ASSERT_GREATER2__(exp0,exp1,exp2,exp3)

	#define	DBG_ASSERT_LESS_OR_EQUAL__(exp0,exp1)
	#define	DBG_ASSERT_LESS_OR_EQUAL1__(exp0,exp1,exp2)
	#define	DBG_ASSERT_LESS_OR_EQUAL2__(exp0,exp1,exp2,exp3)

	#define	DBG_ASSERT_GREATER_OR_EQUAL__(exp0,exp1)
	#define	DBG_ASSERT_GREATER_OR_EQUAL1__(exp0,exp1,exp2)
	#define	DBG_ASSERT_GREATER_OR_EQUAL2__(exp0,exp1,exp2,exp3)
#endif






#define EXCEPT_ASSERT__(ExceptionClass, expression)		if (!(expression)) throw ExceptionClass(globalString("Assertion failed: '"#expression"'"));
	
#define FILE_READ_ASSERT_ZERO__(expression) EXCEPT_ASSERT__(Except::IO::DriveAccess::DataReadFault,expression==0)
	


#endif

/**
C++Builder predefines certain global identifiers known as manifest constants. Most global identifiers begin and end with two underscores (__).

Note: For readability, underscores are often separated by a single blank space. In your source code, you should never insert whitespace between underscores.

Macro	Value	Description

_ _BCOPT_ _	1	Defined in any compiler that has an optimizer.
_ _BCPLUSPLUS_ _	0x0560	Defined if you've selected C++ compilation; will increase in later releases.
_ _BORLANDC_ _	0x0560	Version number.
_ _CDECL_ _ 	1	Defined if Calling Convention is set to cdecl; otherwise undefined.
_CHAR_UNSIGNED	1	Defined by default indicating that the default char is unsigned char. Use the -K compiler option to undefine this macro.

_ _CODEGUARD_ _	 	Defined whenever one of the CodeGuard compiler options is used; otherwise it is undefined.
_ _CONSOLE_ _	1 	When defined, the macro indicates that the program is a console application.
_CPPUNWIND	1	Enable stack unwinding. This is true by default; use -xd- to disable.
_ _cplusplus	1	Defined if in C++ mode; otherwise, undefined.
_ _DATE_ _	String literal	Date when processing began on the current file.

_ _DLL_ _	1	Defined whenever the -WD compiler option is used; otherwise it is undefined.
_ _FILE_ _	String literal	Name of the current file being processed.
_ _FLAT_ _	1	Defined when compiling in 32-bit flat memory model.
_ _FUNC_ _	String literal	Name of the current function being processed. More details.
_ _LINE_ _	Decimal constant	Number of the current source file line being processed.

_M_IX86	0x12c	Always defined. The default  value is 300. You can change the value to 400 or 500 by using the /4 or /5 compiler options.
_ _MT_ _	1	Defined only if the -tWM option is used. It specifies that the multithread library is to be linked.
_ _PASCAL_ _ 	1	Defined if Calling Convention is set to Pascal; otherwise undefined.

_ _STDC_ _	1	Defined if you compile with the -A compiler option; otherwise, it is undefined.
_ _TCPLUSPLUS_ _ 	0x0560	Version number.
_ _TEMPLATES_ _	1	Defined as 1 for C++ files (meaning that templates are supported); otherwise, it is undefined.
_ _TIME_ _	String literal	Time when processing began on the current file.
_ _TLS_ _	1	Thread Local Storage. Always true in C++Builder.
_ _TURBOC_ _ 	0x0560	Will increase in later releases.

_WCHAR_T		Defined only for C++ programs to indicate that wchar_t is an intrinsically defined data type.
_WCHAR_T_DEFINED		Defined only for C++ programs to indicate that wchar_t is an intrinsically defined data type.
_Windows	 	Defined for Windows-only code.
_ _WIN32_ _	1	Defined for console and GUI applications.

Note:	_ _DATE_ _, _ _FILE_ _ , _ _ FUNC_ _, _ _LINE_ _, _ _STDC_ _, and _ _TIME_ _ cannot be redefined or undefined.
*/
