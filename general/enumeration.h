#ifndef enumerationH
#define enumerationH






#undef CONSTRUCT_ECASE
#define CONSTRUCT_ECASE(ENUM_NAME,ENUM_VAL)	case ENUM_VAL: return #ENUM_NAME"::"#ENUM_VAL;


#undef CONSTRUCT_ENUM_HEAD
#define CONSTRUCT_ENUM_HEAD(NAME,COUNT)\
struct NAME\
		{\
		static const size_t NumberOfPossibilities = COUNT;\
		enum value_t\
			{\

#undef CONSTRUCT_ENUM_TRANSITION
#define CONSTRUCT_ENUM_TRANSITION(NAME)\
		};\
		static const char*		toString(value_t type)\
		{\
			switch (type)\
			{\


#undef CONSTRUCT_ENUM_TAIL
#define CONSTRUCT_ENUM_TAIL(NAME,DEFAULT_VALUE)\
			}\
			return #NAME"::Invalid enumeration value";\
		}\
		value_t			value;	/*!< Current enumeration value */ \
		NAME():value(DEFAULT_VALUE)				{}\
		NAME(value_t val):value(val)			{}\
		NAME(const NAME&val):value(val.value)	{}\
		const char*	toString() const			{return toString(value);}	/*!< Converts the current enumeration value to a string*/ \
		template <typename T> bool	load(const T&v)	{index_t val = (index_t)(v); if (val < NumberOfPossibilities) {value = (value_t)val; return true;} return false;}\
		template <typename T> static NAME		reinterpret(const T&v)	{index_t val = (index_t)(v); if (val < NumberOfPossibilities) return NAME((value_t)val); return NAME();}\
		NAME&	operator=(value_t value_)		{value = value_; return *this;}\
		NAME&	operator=(const NAME&value_)	{value = value_.value; return *this;}\
		bool	operator==(value_t value_)		{return value == value_;}\
		bool	operator==(const NAME& value_)	{return value == value_.value;}\
		bool	operator!=(value_t value_)		{return value != value_;}\
		bool	operator!=(const NAME& value_)	{return value != value_.value;}\
		operator value_t() const {return value;}\
		};



#undef CONSTRUCT_ENUMERATION2
#define CONSTRUCT_ENUMERATION2(NAME,CONST0,CONST1)\
	CONSTRUCT_ENUM_HEAD(NAME,2)\
	CONST0,\
	CONST1\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION3
#define CONSTRUCT_ENUMERATION3(NAME,CONST0,CONST1,CONST2)\
	CONSTRUCT_ENUM_HEAD(NAME,3)\
	CONST0,\
	CONST1,\
	CONST2\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)

#undef CONSTRUCT_ENUMERATION4
#define CONSTRUCT_ENUMERATION4(NAME,CONST0,CONST1,CONST2,CONST3)\
	CONSTRUCT_ENUM_HEAD(NAME,4)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION5
#define CONSTRUCT_ENUMERATION5(NAME,CONST0,CONST1,CONST2,CONST3,CONST4)\
	CONSTRUCT_ENUM_HEAD(NAME,5)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)

#undef CONSTRUCT_ENUMERATION6
#define CONSTRUCT_ENUMERATION6(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5)\
	CONSTRUCT_ENUM_HEAD(NAME,6)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)

#undef CONSTRUCT_ENUMERATION7
#define CONSTRUCT_ENUMERATION7(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6)\
	CONSTRUCT_ENUM_HEAD(NAME,7)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION8
#define CONSTRUCT_ENUMERATION8(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6,CONST7)\
	CONSTRUCT_ENUM_HEAD(NAME,8)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONST7,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ECASE(NAME,CONST7)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION9
#define CONSTRUCT_ENUMERATION9(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6,CONST7,CONST8)\
	CONSTRUCT_ENUM_HEAD(NAME,9)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONST7,\
	CONST8,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ECASE(NAME,CONST7)\
	CONSTRUCT_ECASE(NAME,CONST8)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION10
#define CONSTRUCT_ENUMERATION10(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6,CONST7,CONST8,CONST9)\
	CONSTRUCT_ENUM_HEAD(NAME,10)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONST7,\
	CONST8,\
	CONST9,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ECASE(NAME,CONST7)\
	CONSTRUCT_ECASE(NAME,CONST8)\
	CONSTRUCT_ECASE(NAME,CONST9)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION11
#define CONSTRUCT_ENUMERATION11(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6,CONST7,CONST8,CONST9,CONST10)\
	CONSTRUCT_ENUM_HEAD(NAME,11)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONST7,\
	CONST8,\
	CONST9,\
	CONST10,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ECASE(NAME,CONST7)\
	CONSTRUCT_ECASE(NAME,CONST8)\
	CONSTRUCT_ECASE(NAME,CONST9)\
	CONSTRUCT_ECASE(NAME,CONST10)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION12
#define CONSTRUCT_ENUMERATION12(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6,CONST7,CONST8,CONST9,CONST10,CONST11)\
	CONSTRUCT_ENUM_HEAD(NAME,12)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONST7,\
	CONST8,\
	CONST9,\
	CONST10,\
	CONST11,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ECASE(NAME,CONST7)\
	CONSTRUCT_ECASE(NAME,CONST8)\
	CONSTRUCT_ECASE(NAME,CONST9)\
	CONSTRUCT_ECASE(NAME,CONST10)\
	CONSTRUCT_ECASE(NAME,CONST11)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION13
#define CONSTRUCT_ENUMERATION13(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6,CONST7,CONST8,CONST9,CONST10,CONST11,CONST12)\
	CONSTRUCT_ENUM_HEAD(NAME,13)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONST7,\
	CONST8,\
	CONST9,\
	CONST10,\
	CONST11,\
	CONST12,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ECASE(NAME,CONST7)\
	CONSTRUCT_ECASE(NAME,CONST8)\
	CONSTRUCT_ECASE(NAME,CONST9)\
	CONSTRUCT_ECASE(NAME,CONST10)\
	CONSTRUCT_ECASE(NAME,CONST11)\
	CONSTRUCT_ECASE(NAME,CONST12)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)


#undef CONSTRUCT_ENUMERATION14
#define CONSTRUCT_ENUMERATION14(NAME,CONST0,CONST1,CONST2,CONST3,CONST4,CONST5,CONST6,CONST7,CONST8,CONST9,CONST10,CONST11,CONST12,CONST13)\
	CONSTRUCT_ENUM_HEAD(NAME,14)\
	CONST0,\
	CONST1,\
	CONST2,\
	CONST3,\
	CONST4,\
	CONST5,\
	CONST6,\
	CONST7,\
	CONST8,\
	CONST9,\
	CONST10,\
	CONST11,\
	CONST12,\
	CONST13,\
	CONSTRUCT_ENUM_TRANSITION(NAME)\
	CONSTRUCT_ECASE(NAME,CONST0)\
	CONSTRUCT_ECASE(NAME,CONST1)\
	CONSTRUCT_ECASE(NAME,CONST2)\
	CONSTRUCT_ECASE(NAME,CONST3)\
	CONSTRUCT_ECASE(NAME,CONST4)\
	CONSTRUCT_ECASE(NAME,CONST5)\
	CONSTRUCT_ECASE(NAME,CONST6)\
	CONSTRUCT_ECASE(NAME,CONST7)\
	CONSTRUCT_ECASE(NAME,CONST8)\
	CONSTRUCT_ECASE(NAME,CONST9)\
	CONSTRUCT_ECASE(NAME,CONST10)\
	CONSTRUCT_ECASE(NAME,CONST11)\
	CONSTRUCT_ECASE(NAME,CONST12)\
	CONSTRUCT_ECASE(NAME,CONST13)\
	CONSTRUCT_ENUM_TAIL(NAME,CONST0)



#endif
