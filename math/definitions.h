#ifndef math_definitionsH
#define math_definitionsH

/******************************************************************

Mathematical definitions.

******************************************************************/



/** math-constants **
I assume that M_PI is the most commonly used constant. therefore should it not be defined it's likely that
all others are undefined as well
*/

#ifndef M_PI
	#define M_E			2.71828182845904523536
	#define M_LOG2E		1.44269504088896340736
	#define M_LOG10E	0.434294481903251827651
	#define M_LN2		0.693147180559945309417
	#define M_LN10		2.30258509299404568402
	#define M_PI		3.14159265358979323846
	#define M_PI_2		1.57079632679489661923
	#define M_PI_4		0.785398163397448309616
	#define M_1_PI		0.318309886183790671538
	#define M_2_PI		0.636619772367581343076
	#define M_1_SQRTPI	0.564189583547756286948
	#define M_2_SQRTPI	1.12837916709551257390
	#define M_SQRT2		1.41421356237309504880
	#define M_SQRT_2	0.707106781186547524401

	#define M_EF		2.71828182845904523536f
	#define M_LOG2EF	1.44269504088896340736f
	#define M_LOG10EF	0.434294481903251827651f
	#define M_LN2F		0.693147180559945309417f
	#define M_LN10F		2.30258509299404568402f
	#define M_PIF		3.14159265358979323846f
	#define M_PI_2F		1.57079632679489661923f
	#define M_PI_4F		0.785398163397448309616f
	#define M_1_PIF		0.318309886183790671538f
	#define M_2_PIF		0.636619772367581343076f
	#define M_1_SQRTPIF	0.564189583547756286948f
	#define M_2_SQRTPIF	1.12837916709551257390f
	#define M_SQRT2F	1.41421356237309504880f
	#define M_SQRT_2F	0.707106781186547524401f
#endif



/** loop over a certain range. pretty simple **/

#define VECTOR_LOOP(x)          for (index_t el = 0; el < (x); el++)


/** error constants **
    use GLOBAL_ERROR when working with floats. otherwise use GLOBAL_D_ERROR for doubles
    and GLOBAL_L_ERROR for long doubles.
*/

#define GLOBAL_ERROR            (1e-6f)
#define GLOBAL_D_ERROR          (1e-12)
#define GLOBAL_L_ERROR          (1e-15)





/** prototype-abbreviations **/


#define		MF_NO_TYPE				__declspec()		//!< Empty type usable in MF_DECLARE() for functions/methods that do not explicitly specify a return type (so far implicit type conversion operators only)

#define		MF_INIT					inline __declspec(nothrow)		//!< Math function initialization. This macro precedes the return type and specifies modifiers to be used for any math function
#define		MF_CALLING_CONVENTION	__fastcall						//!< Calling convention definition. This macro succeeds the return type and specifies the calling convention to be used for the function

#define		MF_DECLARE_TEMPLATE(_name_)	typename _name_				//!< Declares a template type




#define		MF_CC					MF_CALLING_CONVENTION			//!< Short for calling convention. This macro succeeds the return type and specifies the calling convention to be used for the function
#define		MF_CONSTRUCTOR			MF_INIT							//!< Specification to be used for math object constructors


/**
 @brief Declares a generic math function. 
 
 To be used for functions and method with explicit or no template arguments. Must specify the complete return type or MF_NO_TYPE.
 For return types that feature comma-separated template arguments use MF_INIT before the return type and MF_CALLING_CONVENTION behind it.

*/
#define		MF_DECLARE(_return_type_)	MF_INIT _return_type_ MF_CALLING_CONVENTION	

#define		MF_SPECIALIZED(_return_type_)	template <>	MF_DECLARE(_return_type_)		//!< Defines a completely specialized template function or method. 'template<>' succeeded by MF_DECLARE(_return_type_)





#define		MF_DECLARE_TEMPLATES	MF_DECLARE_TEMPLATE(C)

#define		MF_DECLARE_TEMPLATES1	MF_DECLARE_TEMPLATE(C0)
#define		MF_DECLARE_TEMPLATES2	MF_DECLARE_TEMPLATES1,	MF_DECLARE_TEMPLATE(C1)
#define		MF_DECLARE_TEMPLATES3	MF_DECLARE_TEMPLATES2,	MF_DECLARE_TEMPLATE(C2)
#define		MF_DECLARE_TEMPLATES4	MF_DECLARE_TEMPLATES3,	MF_DECLARE_TEMPLATE(C3)
#define		MF_DECLARE_TEMPLATES5	MF_DECLARE_TEMPLATES4,	MF_DECLARE_TEMPLATE(C4)
#define		MF_DECLARE_TEMPLATES6	MF_DECLARE_TEMPLATES5,	MF_DECLARE_TEMPLATE(C5)
#define		MF_DECLARE_TEMPLATES7	MF_DECLARE_TEMPLATES6,	MF_DECLARE_TEMPLATE(C6)
#define		MF_DECLARE_TEMPLATES8	MF_DECLARE_TEMPLATES7,	MF_DECLARE_TEMPLATE(C7)
#define		MF_DECLARE_TEMPLATES9	MF_DECLARE_TEMPLATES8,	MF_DECLARE_TEMPLATE(C8)
#define		MF_DECLARE_TEMPLATES10	MF_DECLARE_TEMPLATES9,	MF_DECLARE_TEMPLATE(C9)
#define		MF_DECLARE_TEMPLATES11	MF_DECLARE_TEMPLATES10,	MF_DECLARE_TEMPLATE(C10)
#define		MF_DECLARE_TEMPLATES12	MF_DECLARE_TEMPLATES11,	MF_DECLARE_TEMPLATE(C11)
#define		MF_DECLARE_TEMPLATES13	MF_DECLARE_TEMPLATES12,	MF_DECLARE_TEMPLATE(C12)
#define		MF_DECLARE_TEMPLATES14	MF_DECLARE_TEMPLATES13,	MF_DECLARE_TEMPLATE(C13)
#define		MF_DECLARE_TEMPLATES15	MF_DECLARE_TEMPLATES14,	MF_DECLARE_TEMPLATE(C14)
#define		MF_DECLARE_TEMPLATES16	MF_DECLARE_TEMPLATES15,	MF_DECLARE_TEMPLATE(C15)
#define		MF_DECLARE_TEMPLATES17	MF_DECLARE_TEMPLATES16,	MF_DECLARE_TEMPLATE(C16)
#define		MF_DECLARE_TEMPLATES18	MF_DECLARE_TEMPLATES17,	MF_DECLARE_TEMPLATE(C17)
#define		MF_DECLARE_TEMPLATES19	MF_DECLARE_TEMPLATES18,	MF_DECLARE_TEMPLATE(C18)
#define		MF_DECLARE_TEMPLATES20	MF_DECLARE_TEMPLATES19,	MF_DECLARE_TEMPLATE(C19)



#define		MFUNC_N(_template_type_count_,_return_type_)	template< MF_DECLARE_TEMPLATES##_template_type_count_> MF_DECLARE(_return_type_)
/*
 * The following declares function definition macros for the specified number of different template arguments where MFUNCi declares C0 to C<i-1> (e.g. MFUNC3 declares C0, C1, and C2)
 * 
 * For static functions/methods pass 'static' as part of the return parameter (e.g. MFUNC2(static char) myFunction(C0, C1)) or use MF_STATIC(<num_template_parameters>,...)
 * 
 */
#define     MFUNC(_return_type_)	MFUNC_N(,_return_type_)
#define     MFUNC1(_return_type_)	MFUNC_N(1,_return_type_)
#define     MFUNC2(_return_type_)	MFUNC_N(2,_return_type_)
#define     MFUNC3(_return_type_)	MFUNC_N(3,_return_type_)
#define     MFUNC4(_return_type_)	MFUNC_N(4,_return_type_)
#define     MFUNC5(_return_type_)	MFUNC_N(5,_return_type_)
#define     MFUNC6(_return_type_)	MFUNC_N(6,_return_type_)
#define     MFUNC7(_return_type_)	MFUNC_N(7,_return_type_)
#define     MFUNC8(_return_type_)	MFUNC_N(8,_return_type_)
#define     MFUNC9(_return_type_)	MFUNC_N(9,_return_type_)
#define     MFUNC10(_return_type_)  MFUNC_N(10,_return_type_)

#define     MFUNC11(_return_type_)  MFUNC_N(11,_return_type_)
#define     MFUNC12(_return_type_)  MFUNC_N(12,_return_type_)
#define     MFUNC13(_return_type_)  MFUNC_N(13,_return_type_)
#define     MFUNC14(_return_type_)  MFUNC_N(14,_return_type_)
#define     MFUNC15(_return_type_)  MFUNC_N(15,_return_type_)
#define     MFUNC16(_return_type_)  MFUNC_N(16,_return_type_)
#define     MFUNC17(_return_type_)  MFUNC_N(17,_return_type_)

#define		MF_STATIC(_template_type_count_,_return_type_)	MFUNC##_template_type_count_(static _return_type_)

#define		MFUNCV_N(_template_type_count_,_return_type_)	template< MF_DECLARE_TEMPLATES##_template_type_count_, count_t Dimensions> MF_DECLARE(_return_type_)

#define     MFUNCV(_return_type_)  MFUNCV_N(,_return_type_)
#define     MFUNC2V(_return_type_) MFUNCV_N(2,_return_type_)
#define     MFUNC3V(_return_type_) MFUNCV_N(3,_return_type_)
#define     MFUNC4V(_return_type_) MFUNCV_N(4,_return_type_)
#define     MFUNC5V(_return_type_) MFUNCV_N(5,_return_type_)
#define     MFUNC6V(_return_type_) MFUNCV_N(6,_return_type_)


#define		MF_CONSTRUCTOR_N(_template_type_count_)	template<MF_DECLARE_TEMPLATES##_template_type_count_> MF_CONSTRUCTOR
#define		MF_CONSTRUCTOR1			MF_CONSTRUCTOR_N(1)
#define		MF_CONSTRUCTOR2			MF_CONSTRUCTOR_N(2)
#define		MF_CONSTRUCTOR3			MF_CONSTRUCTOR_N(3)
#define		MF_CONSTRUCTOR4			MF_CONSTRUCTOR_N(4)



/** recursive template-classes **
these helper-classes recursively loop over a certain range executing commands provided by core or root_core respectively.
using these processing static arrays now is as fast as repeating commands
*/

/** query-classes **

query-classes can be used to return values from array-operations (i.e. when calculating the dot-product)
the arrays will be processed in backwards-order. The last elements of the arrays form the root, which will be
processed first.

core-code-convention:
    in general you can address the respective arrays using the names a ... d.
    To execute a certain operation on the respective layer of the arrays use (*a) ... (*d)
    Example: (*c) = (*a) + (*b)  will execute c = a + b operation on all elements of the arrays
    class names are C for 1-array-operations and C0 ... CN for multi-array-operations.


will create two recursive helper-classes using the following parameters:
    [name]      :=      name of the class(es)
    [rtype]     :=      return-type
    [const0..constN]:=  'const' if you want the respective array to be constant. otherwise ''.
    [root_core] :=      code executed at the array-root without trailing ';' (don't forget the return-statement)
    [core]      :=      code executed with each consecutive array-level without trailing ';'. use 'previous'
                        the get the value of the preceding result. (don't forget the return-statement)
                        
    Example:    QUERY_CLASS1(QDot,C0,const,const,return (*a)*(*b),return previous+(*a)*(*b))
                will create a query-class called QDot that returns the dot-product of a and b
                
                float dot_product = QDot<float,float,3>::perform(vector0,vector1);
                would then calculate the dot-product for the two given vectors vector0 and vector1 each
                featuring at least three elements.
                of course the class QDot already exists. it's recommend to use the function _dotV instead though
**/

#define QUERY_CLASS(name,rtype,const0,root_core,core)\
    template <class C, count_t Count> class name {public: static rtype perform(const0 C*a) {rtype previous = name<C,Count-1>::perform(a+1);core;}};\
    template <class C> class name<C,1> {public: static rtype perform(const0 C*a) {root_core;}};


#define QUERY_CLASS2(name,rtype,const0,const1,root_core,core)\
    template <class C0, class C1, count_t Count> class name {public: static rtype perform(const0 C0*a,const1 C1*b) {rtype previous = name<C0,C1,Count-1>::perform(a+1,b+1);core;}};\
    template <class C0, class C1> class name<C0,C1,1> {public: static rtype perform(const0 C0*a,const1 C1*b) {root_core;}};

/** performer-classes **

more simplistic version of the operation-class described further down. simple processes executed over a number of given
arrays.

use the following methods to create helper-classes for 1..4 arrays using the parameters:
    [name]      :=      name of the class(es)
    [const0..constN]:=  'const' if you want the respective array to be constant. otherwise ''.
    [core]      :=      code executed on each layer of the arrays
    
    Example:    PERFORMER_CLASS2(QAdd,const,const,const,,(*d)=(*a)+(*b)+(*c))
                will create a performer-class, that calculates d = a + b + c for n-dimensional vectors.
                
                QAdd<float,float,float,float,5>::perform(v0,v1,v2,result);
                would execute the calculation for 5-dimensional vectors.
*/

#define PERFORMER_CLASS(name,core)\
    template <class C, count_t Count> class name {public: static void perform(C*a) {core; name<C,Count-1>::perform(a+1);}};\
    template <class C> class name<C,1> {public: static void perform(C*a) {core;}};

#define PERFORMER_CLASS2(name,const0,const1,core)\
    template <class C0, class C1, count_t Count> class name {public: static void perform(const0 C0*a,const1 C1*b) {core; name<C0,C1,Count-1>::perform(a+1,b+1);}};\
    template <class C0, class C1> class name<C0,C1,1> {public: static void perform(const0 C0*a,const1 C1*b) {core;}};

#define PERFORMER_CLASS3(name,const0,const1,const2,core)\
    template <class C0, class C1, class C2, count_t Count> class name {public: static void perform(const0 C0*a,const1 C1*b, const2 C2*c) {core; name<C0,C1,C2,Count-1>::perform(a+1,b+1,c+1);}};\
    template <class C0, class C1, class C2> class name<C0,C1,C2,1> {public: static void perform(const0 C0*a,const1 C1*b, const2 C2*c) {core;}};

#define PERFORMER_CLASS4(name,const0,const1,const2,const3,core)\
    OPERATION_CLASS4(name,const0 C0*a,const1 C1*b, const C2*c, const C3*d,a+1,b+1,c+1,d+1,core)


/** operation-classes **

operation-classes give you greater freedom in defining and naming the parameters used by the operation.
operation-classes do not provide return-values.

use the following parameters to create a operation-class:
    [name]      :=      name of the class(es)
    [p/p0..pN]  :=      full parameter-description (i.e. const C0*vector or const C2&scalar)
    [op/op0..opN]:=     recursive output-description. use the same order of the parameters as in the parameter-
                        descriptions. giving just the name of the parameter will pass that parameter unchanged.
                        using [parameter]+1 will increase the value by one element.
    [core]      :=      code executed on each layer of the arrays.
    
    Example:    OPERATION_CLASS2(QAddVal,const C0*v,const C1&val,C2*out,v+1,val,out+1,(*out)=(*v)+val)
                would create a operation-class that adds a scalar to a vector and puts the result into a seperate
                vector.
                note the out-parameters: v+1 and out+1 will advance those vectors by one, while val is passed unchanged.
                of course you could use v-1 or any other value as well. Be sure to know what you're doing though.

    Note:       when describing the input-parameters you must use the class-names C/C0 ... CN or the compiler will not
                recognize your input.
*/


#define OPERATION_CLASS(name,p,op,core)\
    template <class C, count_t Count> class name {public: static void perform(p) {core; name<C,Count-1>::perform(op);}};\
    template <class C> class name<C,1>{public: static void perform(p) {core;}};

#define OPERATION_CLASS2(name,p0,p1,op0,op1,core)\
    template <class C0, class C1, count_t Count> class name {public: static void perform(p0,p1) {core; name<C0,C1,Count-1>::perform(op0,op1);}};\
    template <class C0, class C1> class name<C0,C1,1>{public: static void perform(p0,p1) {core;}};


#define OPERATION_CLASS3(name,p0,p1,p2,op0,op1,op2,core)\
    template <class C0, class C1, class C2, count_t Count> class name {public: static void perform(p0,p1,p2) {core; name<C0,C1,C2,Count-1>::perform(op0,op1,op2);}};\
    template <class C0, class C1, class C2> class name<C0,C1,C2,1>{public: static void perform(p0,p1,p2) {core;}};


#define OPERATION_CLASS4(name,p0,p1,p2,p3,op0,op1,op2,op3,core)\
    template <class C0, class C1, class C2, class C3, count_t Count> class name {public: static void perform(p0,p1,p2,p3) {core; name<C0,C1,C2,C3,Count-1>::perform(op0,op1,op2,op3);}};\
    template <class C0, class C1, class C2, class C3> class name<C0,C1,C2,C3,1>{public: static void perform(p0,p1,p2,p3) {core;}};



#endif
