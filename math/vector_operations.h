#ifndef vector_operationsH
#define vector_operationsH
/*

Warning: date(): It is not safe to rely on the system's timezone settings. You are *required* to use the date.timezone setting or the date_default_timezone_set() function. In case you used any of those methods and you are still getting this warning, you most likely misspelled the timezone identifier. We selected the timezone 'UTC' for now, but please set date.timezone to select your timezone. in E:\include\math\update.php on line 1656

Warning: date(): It is not safe to rely on the system's timezone settings. You are *required* to use the date.timezone setting or the date_default_timezone_set() function. In case you used any of those methods and you are still getting this warning, you most likely misspelled the timezone identifier. We selected the timezone 'UTC' for now, but please set date.timezone to select your timezone. in E:\include\math\update.php on line 1656
This file was generated from template definition 'vector.template.php' on 2015 September 6th 15:13:31
Do not edit
*/


namespace Vec
{
	template <typename T0, typename T1>
		inline	T0	__fastcall	Operator0_sumD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			T0 result = __p0[0]*__p1[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result + __p0[iterator__]*__p1[iterator__];
			return result;
		}

	template <typename T0>
		inline	T0	__fastcall	Operator1_sumD(const T0*__p0, count_t dimensions)throw()
		{
			T0 result = __p0[0]*__p0[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result + __p0[iterator__]*__p0[iterator__];
			return result;
		}

	template <typename T0>
		inline	T0	__fastcall	Operator2_sumD(const T0*__p0, count_t dimensions)throw()
		{
			T0 result = __p0[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result + __p0[iterator__];
			return result;
		}

	template <typename T0>
		inline	bool	__fastcall	LogicOperator3_andD(const T0*__p0, count_t dimensions)throw()
		{
			bool result = !__p0[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result && !__p0[iterator__];
			return result;
		}

	template <typename T0, typename T1>
		inline	T0	__fastcall	Operator4_sumD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			T0 result = sqr(__p0[0] - __p1[0]);
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result + sqr(__p0[iterator__] - __p1[iterator__]);
			return result;
		}

	template <typename T0, typename T1>
		inline	bool	__fastcall	LogicOperator5_andD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			bool result = __p0[0] == __p1[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result && __p0[iterator__] == __p1[iterator__];
			return result;
		}

	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	Operator6_sumD(const T0*__p0, const T1*__p1, T2 __p2, count_t dimensions)throw()
		{
			T0 result = sqr(__p0[0] - __p1[0]);
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result + sqr(__p0[iterator__] - __p1[iterator__]);
			return result;
		}

	template <typename T0, typename T1>
		inline	T0	__fastcall	Functional7_maxD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			T0 result = vabs(__p0[0]-__p1[0]);
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = vmax(result,vabs(__p0[iterator__]-__p1[iterator__]));
			return result;
		}

	template <typename T0>
		inline	bool	__fastcall	LogicOperator8_orD(const T0*__p0, count_t dimensions)throw()
		{
			bool result = isnan(__p0[0]);
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result || isnan(__p0[iterator__]);
			return result;
		}

	template <typename T0>
		inline	T0	__fastcall	Functional9_minD(const T0*__p0, count_t dimensions)throw()
		{
			T0 result = __p0[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = vmin(result,__p0[iterator__]);
			return result;
		}

	template <typename T0>
		inline	T0	__fastcall	Functional10_maxD(const T0*__p0, count_t dimensions)throw()
		{
			T0 result = __p0[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = vmax(result,__p0[iterator__]);
			return result;
		}

	template <typename T0, typename T1>
		inline	bool	__fastcall	LogicOperator11_orD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			bool result = __p0[0] < __p1[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result || __p0[iterator__] < __p1[iterator__];
			return result;
		}

	template <typename T0, typename T1>
		inline	bool	__fastcall	LogicOperator12_orD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			bool result = __p0[0] > __p1[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result || __p0[iterator__] > __p1[iterator__];
			return result;
		}

	template <typename T0, typename T1>
		inline	bool	__fastcall	LogicOperator13_andD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			bool result = __p0[0] < __p1[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result && __p0[iterator__] < __p1[iterator__];
			return result;
		}

	template <typename T0, typename T1>
		inline	bool	__fastcall	LogicOperator14_andD(const T0*__p0, const T1*__p1, count_t dimensions)throw()
		{
			bool result = __p0[0] > __p1[0];
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result && __p0[iterator__] > __p1[iterator__];
			return result;
		}

	template <typename T0>
		inline	String	__fastcall	Operator15_glueD(const T0*__p0, count_t dimensions)throw()
		{
			String result = String(__p0[0]);
			for (register index_t iterator__ = 1; iterator__ < dimensions; iterator__++)
				result = result +", "+ String(__p0[iterator__]);
			return result;
		}


		/* ----- Now dynamic_dimensions instances ----- */
	//now implementing template definition 'void addValue|addVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		addValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addValueD(const T0 *v, T1 value, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] + value;
			}
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		addVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addValD(const T0 *v, T1 value, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] + value;
			}
		}

	//now implementing template definition 'void addValue|addVal (2..4) (<[*] v>, <value>) direct=1'
	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		addValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addValueD(T0 *v, T1 value, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] += value;
			}
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		addVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addValD(T0 *v, T1 value, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] += value;
			}
		}

	//now implementing template definition 'void add (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	addD(const T0 *u, const T1 *v, const T2 *w, T3 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = u[iterator__] + v[iterator__] + w[iterator__];
			}
		}

	//now implementing template definition 'void add (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__]+w[iterator__];
			}
		}

	//now implementing template definition 'void add (2..4) (<[*] v>, <const [*] w>) direct=1'
	/**
		@brief <br>
		<br>
		add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addD(T0 *v, const T1 *w, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] += w[iterator__];
			}
		}

	//now implementing template definition 'void subtract|sub (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		subtract() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtractD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] - w[iterator__];
			}
		}

	/**
		@brief <br>
		<br>
		sub() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] - w[iterator__];
			}
		}

	//now implementing template definition 'void subtract|sub (2..4) (<[*] v>, <const [*] w>) direct=1'
	/**
		@brief <br>
		<br>
		subtract() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtractD(T0 *v, const T1 *w, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] -= w[iterator__];
			}
		}

	/**
		@brief <br>
		<br>
		sub() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subD(T0 *v, const T1 *w, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] -= w[iterator__];
			}
		}

	//now implementing template definition 'void subtractValue|subVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		subtractValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtractValueD(const T0 *v, T1 value, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] - value;
			}
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		subVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subValD(const T0 *v, T1 value, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] - value;
			}
		}

	//now implementing template definition 'void subtractValue|subVal (2..4) (<[*] v>, <value>) direct=1'
	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		subtractValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtractValueD(T0 *v, T1 value, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] -= value;
			}
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		subVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subValD(T0 *v, T1 value, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] -= value;
			}
		}

	//now implementing template definition 'void multiply|mult|mul (2..4) (<const [*] v>, <factor>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		multiply() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multiplyD(const T0 *v, T1 factor, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] * factor;
			}
		}

	/**
		@brief <br>
		<br>
		mult() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multD(const T0 *v, T1 factor, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] * factor;
			}
		}

	/**
		@brief <br>
		<br>
		mul() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mulD(const T0 *v, T1 factor, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] * factor;
			}
		}

	//now implementing template definition 'void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1'
	/**
		@brief <br>
		<br>
		multiply() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	multiplyD(T0 *v, T1 factor, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] *= factor;
			}
		}

	/**
		@brief <br>
		<br>
		mult() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	multD(T0 *v, T1 factor, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] *= factor;
			}
		}

	/**
		@brief <br>
		<br>
		mul() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	mulD(T0 *v, T1 factor, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] *= factor;
			}
		}

	//now implementing template definition 'void divide|div (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		divide() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	divideD(const T0 *v, T1 value, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] / value;
			}
		}

	/**
		@brief <br>
		<br>
		div() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	divD(const T0 *v, T1 value, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] / value;
			}
		}

	//now implementing template definition 'void divide|div (2..4) (<[*] v>, <value>) direct=1'
	/**
		@brief <br>
		<br>
		divide() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	divideD(T0 *v, T1 value, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] /= value;
			}
		}

	/**
		@brief <br>
		<br>
		div() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	divD(T0 *v, T1 value, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] /= value;
			}
		}

	//now implementing template definition 'void resolve (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		resolve() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	resolveD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] / w[iterator__];
			}
		}

	//now implementing template definition 'void stretch (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the component-wise product of v and w to result<br>
		<br>
		stretch() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	stretchD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] * w[iterator__];
			}
		}

	//now implementing template definition 'void interpolate (2..4) (<const [*] v>, <const [*] w>, <f>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		interpolate() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] f 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	interpolateD(const T0 *v, const T1 *w, T2 f, T3 *result, count_t dimensions)throw()
		{
			T3 i_ = T3(1)-f;
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = v[iterator__] * i_ + w[iterator__] * f;
			}
		}

	//now implementing template definition 'void center (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the arithmetic center of v and w to result<br>
		<br>
		center() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	centerD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = (v[iterator__] + w[iterator__])/T2(2);
			}
		}

	//now implementing template definition 'void center (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the arithmetic center of u, v, and w to result<br>
		<br>
		center() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	centerD(const T0 *u, const T1 *v, const T2 *w, T3 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = (u[iterator__] + v[iterator__] + w[iterator__])/T3(3);
			}
		}

	//now implementing template definition 'void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1'
	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		multAdd() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multAddD(T0 *current, const T1 *vector, T2 scalar, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				current[iterator__] += vector[iterator__] * scalar;
			}
		}

	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		mad() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	madD(T0 *current, const T1 *vector, T2 scalar, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				current[iterator__] += vector[iterator__] * scalar;
			}
		}

	//now implementing template definition 'void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1'
	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		multAdd() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	multAddD(const T0 *base, const T1 *vector, T2 scalar, T3 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = base[iterator__] + vector[iterator__] * scalar;
			}
		}

	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		mad() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	madD(const T0 *base, const T1 *vector, T2 scalar, T3 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = base[iterator__] + vector[iterator__] * scalar;
			}
		}

	//now implementing template definition 'T0 dot (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Calculates the dot product of @a v and @a w<br>
		<br>
		dot() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	dotD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::Operator0_sumD(v, w, dimensions)/*v:i*w:i*/;
		}

	//now implementing template definition 'T0 dot (2..4) (<const [*] v>) direct='
	/**
		@brief Calculates the dot product @a v * @a v<br>
		<br>
		dot() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	dotD(const T0 *v, count_t dimensions)throw()
		{
			return Vec::Operator1_sumD(v, dimensions)/*v:i*v:i*/;
		}

	//now implementing template definition 'T0 sum (2..4) (<const [*] v>) direct='
	/**
		@brief <br>
		<br>
		sum() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	sumD(const T0 *v, count_t dimensions)throw()
		{
			return Vec::Operator2_sumD(v, dimensions)/*v:i*/;
		}

	//now implementing template definition 'bool zero (2..4) (<const [*] v>) direct='
	/**
		@brief <br>
		<br>
		zero() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	zeroD(const T0 *v, count_t dimensions)throw()
		{
			return Vec::LogicOperator3_andD(v, dimensions)/*!v:i*/;
		}

	//now implementing template definition 'T0 length (2..4) (<const [*] v>) direct='
	/**
		@brief Determines the length of @a v<br>
		<br>
		length() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v Vector to determine the length of
		@param dimensions [in] Number of dimensions to process
		@return Length of <paramref>v</paramref> 
	*/
	template <typename T0>
		inline	T0	__fastcall	lengthD(const T0 *v, count_t dimensions)throw()
		{
			return vsqrt((Vec::Operator1_sumD(v, dimensions)/*v:i*v:i*/));
		}

	//now implementing template definition 'T0 quadraticDistance (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief <br>
		<br>
		quadraticDistance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	quadraticDistanceD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::Operator4_sumD(v, w, dimensions)/*sqr(v:i - w:i)*/;
		}

	//now implementing template definition 'T0 distance (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Calculates the distance between @a v0 and @a v1<br>
		<br>
		distance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	distanceD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return vsqrt( (Vec::Operator4_sumD(v, w, dimensions)/*sqr(v:i - w:i)*/) );
		}

	//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		reflectN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] p The point being reflected
		@param[out] out [out] Result
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflectND(const T0 *base, const T1 *normal, const T2 *p, T3 *out, count_t dimensions)throw()
		{
			T3 fc = 2*( (Vec::Operator0_sumD(base, normal, dimensions)/*base:i*normal:i*/) - (Vec::Operator0_sumD(p, normal, dimensions)/*p:i*normal:i*/) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={p, normal, fc, out}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					out[iterator__] = p[iterator__] + normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		reflectN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] p [inout] The point being reflected
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectND(const T0 *base, const T1 *normal, T2 *p, count_t dimensions)throw()
		{
			T2 fc = 2*( (Vec::Operator0_sumD(base, normal, dimensions)/*base:i*normal:i*/) - (Vec::Operator0_sumD(p, normal, dimensions)/*p:i*normal:i*/) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={p, normal, fc}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					p[iterator__] += normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		reflect() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] p The point being reflected
		@param[out] out [out] Result
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflectD(const T0 *base, const T1 *normal, const T2 *p, T3 *out, count_t dimensions)throw()
		{
			T3 fc = 2*( (Vec::Operator0_sumD(base, normal, dimensions)/*base:i*normal:i*/) - (Vec::Operator0_sumD(p, normal, dimensions)/*p:i*normal:i*/) ) / (Vec::Operator1_sumD(normal, dimensions)/*normal:i*normal:i*/);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={p, normal, fc, out}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					out[iterator__] = p[iterator__] + normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		reflect() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] p The point being reflected
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectD(const T0 *base, const T1 *normal, T2 *p, count_t dimensions)throw()
		{
			T2 fc = 2*( (Vec::Operator0_sumD(base, normal, dimensions)/*base:i*normal:i*/) - (Vec::Operator0_sumD(p, normal, dimensions)/*p:i*normal:i*/) ) / (Vec::Operator1_sumD(normal, dimensions)/*normal:i*normal:i*/);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={p, normal, fc}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					p[iterator__] += normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		reflectVectorN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out Reflection result
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVectorND(const T0 *normal, const T1 *v, T2 *out, count_t dimensions)throw()
		{
			T2 fc = 2*( - (Vec::Operator0_sumD(v, normal, dimensions)/*v:i*normal:i*/) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={v, normal, fc, out}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					out[iterator__] = v[iterator__] + normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <[*] v>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		reflectVectorN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] v [inout] The vector being reflected. May be of any length.
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVectorND(const T0 *normal, T1 *v, count_t dimensions)throw()
		{
			T1 fc = 2*( - (Vec::Operator0_sumD(v, normal, dimensions)/*v:i*normal:i*/) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={v, normal, fc}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					v[iterator__] += normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		reflectVector() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out [out] Reflection result
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVectorD(const T0 *normal, const T1 *v, T2 *out, count_t dimensions)throw()
		{
			T2 fc = 2*( - (Vec::Operator0_sumD(v, normal, dimensions)/*v:i*normal:i*/) )/(Vec::Operator1_sumD(normal, dimensions)/*normal:i*normal:i*/);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={v, normal, fc, out}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					out[iterator__] = v[iterator__] + normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <[*] v>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		reflectVector() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] v [inout] The vector being reflected. May be of any length.
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVectorD(const T0 *normal, T1 *v, count_t dimensions)throw()
		{
			T1 fc = 2*( - (Vec::Operator0_sumD(v, normal, dimensions)/*v:i*normal:i*/) )/(Vec::Operator1_sumD(normal, dimensions)/*normal:i*normal:i*/);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={v, normal, fc}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					v[iterator__] += normal[iterator__] * fc;
				}
			};
		}

	//now implementing template definition 'void resolveUCBS (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		resolveUCBS() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBSD(const T0 *p0, const T1 *p1, const T2 *p2, const T3 *p3, T4 t, T5 *result, count_t dimensions)throw()
		{
			T5	//i = 1-t,
					f0 = (-t*t*t+3*t*t-3*t+1)/6,
					f1 = (3*t*t*t-6*t*t+4)/6,
					f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
					f3 = (t*t*t)/6;
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = p0[iterator__]*f0 + p1[iterator__]*f1 + p2[iterator__]*f2 + p3[iterator__]*f3;
			}
		}

	//now implementing template definition 'void resolveUCBSaxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		resolveUCBSaxis() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBSaxisD(const T0 *p0, const T1 *p1, const T2 *p2, const T3 *p3, T4 t, T5 *result, count_t dimensions)throw()
		{
			T5	//i = 1-t,
					f0 = (t*t-2*t+1)/2,
					f1 = (-2*t*t+2*t+1)/2,
					f2 = (t*t)/2;
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = (p1[iterator__]-p0[iterator__])*f0 + (p2[iterator__]-p1[iterator__])*f1 + (p3[iterator__]-p2[iterator__])*f2;
			}
		}

	//now implementing template definition 'void resolveBezierCurvePoint (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		resolveBezierCurvePoint() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurvePointD(const T0 *p0, const T1 *p1, const T2 *p2, const T3 *p3, T4 t, T5 *result, count_t dimensions)throw()
		{
			T5	i = 1-t,
					f3 = t*t*t,
					f2 = t*t*i*3,
					f1 = t*i*i*3,
					f0 = i*i*i;
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = p0[iterator__]*f0 + p1[iterator__]*f1 + p2[iterator__]*f2 + p3[iterator__]*f3;
			}
		}

	//now implementing template definition 'void resolveBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		resolveBezierCurveAxis() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurveAxisD(const T0 *p0, const T1 *p1, const T2 *p2, const T3 *p3, T4 t, T5 *result, count_t dimensions)throw()
		{
			T5	i = 1-t,
					f2 = t*t,
					f1 = t*i*2,
					f0 = i*i;
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = (p1[iterator__] - p0[iterator__])*f0 + (p2[iterator__] - p1[iterator__])*f1 + (p3[iterator__] - p2[iterator__])*f2;
			}
			
			//block inlining void normalize0 (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='DynamicPointers', parameters={result}...
			{
				T5 len = (Vec::Operator1_sumD(result, dimensions)/*result:i*result:i*/);
				if (isnan(len) || len <= TypeInfo<T5>::error)
				{
					result[0] = 1;
					for (register index_t iterator__=1; iterator__ < dimensions; iterator__++)
					{
						result[iterator__] = 0;
					}
					return;
				}
				len = vsqrt(len);
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					result[iterator__] /= len;
				}
			};
		}

	//now implementing template definition 'void SplitBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] out0>, <[*] out1>, <[*] out2>, <[*] out3>) direct=1'
	/**
		@brief <br>
		<br>
		SplitBezierCurveAxis() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] out0 
		@param[out] out1 
		@param[out] out2 
		@param[out] out3 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
		inline	void	__fastcall	SplitBezierCurveAxisD(const T0 *p0, const T1 *p1, const T2 *p2, const T3 *p3, T4 t, T5 *out0, T6 *out1, T7 *out2, T8 *out3, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				{
					T5 x01 = (p1[iterator__] - p0[iterator__])*t + p0[iterator__];
					T5 x12 = (p2[iterator__] - p1[iterator__])*t + p1[iterator__];
					T5 x23 = (p3[iterator__] - p2[iterator__])*t + p2[iterator__];
					T5 x012 = (x12 - x01) * t + x01;
					T5 x123 = (x23 - x12) * t + x12;
					T5 x0123 = (x123 - x012) * t + x012;
					out0[iterator__] = p0[iterator__];
					out1[iterator__] = x01;
					out2[iterator__] = x012;
					out3[iterator__] = x0123;
				}
			}
		}

	//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>, <tolerance>) direct=1'
	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		compare() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@param dimensions [in] Number of dimensions to process
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compareD(const T0 *v0, const T1 *v1, T2 tolerance, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				{
					T0 delta = v0[iterator__] - v1[iterator__];
					if (delta < -tolerance)
						return -1;
					if (delta > tolerance)
						return 1;
				}
			}
			return 0;
		}

	//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>) direct='
	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		compare() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v0 
		@param[in] v1 
		@param dimensions [in] Number of dimensions to process
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compareD(const T0 *v0, const T1 *v1, count_t dimensions)throw()
		{
			return compareD(v0,v1,(TypeInfo<T0>::error),dimensions);
		}

	//now implementing template definition 'bool equal (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief <br>
		<br>
		equal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	equalD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::LogicOperator5_andD(v, w, dimensions)/*v:i == w:i*/;
		}

	//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>, <tolerance>) direct='
	/**
		@brief Determines whether @a v and @a w are similar (within the specified tolerance proximity)<br>
		<br>
		similar() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] tolerance maximum distance between <paramref>v</paramref> and <paramref>w</paramref> to still be considered similar
		@param dimensions [in] Number of dimensions to process
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar in accordance to the specified distance, false otherwise 
	*/
	template <typename T0, typename T1, typename T2>
		inline	bool	__fastcall	similarD(const T0 *v, const T1 *w, T2 tolerance, count_t dimensions)throw()
		{
			return Vec::Operator6_sumD(v, w, tolerance, dimensions)/*sqr(v:i - w:i)*/ < tolerance*tolerance;
		}

	//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines whether @a v and @a w are similar (within a type-specific tolerance proximity)<br>
		<br>
		similar() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar, false otherwise 
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	similarD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return (Vec::Operator6_sumD(v, w, (TypeInfo<T0>::error), dimensions)/*sqr(v:i - w:i)*/ < (TypeInfo<T0>::error)*(TypeInfo<T0>::error));
		}

	//now implementing template definition 'T0 maxAxisDistance (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines the greatest difference along any axis between @a v and @a w<br>
		<br>
		maxAxisDistance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return Greatest axial distance between v and w 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	maxAxisDistanceD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::Functional7_maxD(v, w, dimensions)/*vabs(v:i-w:i)*/;
		}

	//now implementing template definition 'T0 planePointDistanceN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
	/**
		@brief Determines the absolute distance between a plane and a point. The specified normal is assumed to be of length 1<br>
		<br>
		planePointDistanceN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; must be normalized
		@param[in] p point to determine the distance of
		@param dimensions [in] Number of dimensions to process
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistanceND(const T0 *base, const T1 *normal, const T2 *p, count_t dimensions)throw()
		{
			return ((Vec::Operator0_sumD(p, normal, dimensions)/*p:i*normal:i*/) - (Vec::Operator0_sumD(base, normal, dimensions)/*base:i*normal:i*/));
		}

	//now implementing template definition 'T0 planePointDistance (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
	/**
		@brief Determines the absolute distance between a plane and a point<br>
		<br>
		planePointDistance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; may be of any length greater 0
		@param[in] p point to determine the distance of
		@param dimensions [in] Number of dimensions to process
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistanceD(const T0 *base, const T1 *normal, const T2 *p, count_t dimensions)throw()
		{
			return ((Vec::Operator0_sumD(p, normal, dimensions)/*p:i*normal:i*/) - (Vec::Operator0_sumD(base, normal, dimensions)/*base:i*normal:i*/)) / (vsqrt((Vec::Operator1_sumD(normal, dimensions)/*normal:i*normal:i*/)));
		}

	//now implementing template definition 'void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1'
	/**
		@brief Scales the distance between @a center and @a current by the specified @a factor<br>
		<br>
		scale() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] factor 
		@param[out] current 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleD(const T0 *center, T1 factor, T2 *current, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				current[iterator__] = center[iterator__]+(current[iterator__]-center[iterator__])*factor;
			}
		}

	//now implementing template definition 'void scaleAbsolute (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Behavior is undefined if @a center and @a current are identical.<br>
		<br>
		scaleAbsolute() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsoluteD(const T0 *center, T1 distance, T2 *current, count_t dimensions)throw()
		{
			T2 len = (vsqrt( (Vec::Operator4_sumD(current, center, dimensions)/*sqr(current:i - center:i)*/) ));
			
			//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={center, (distance/len), current}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					current[iterator__] = center[iterator__]+(current[iterator__]-center[iterator__])*(distance/len);
				}
			};
		}

	//now implementing template definition 'void scaleAbsolute0 (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Also checks if @a center and @a current are identical and reacts appropriately.<br>
		<br>
		scaleAbsolute0() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsolute0D(const T0 *center, T1 distance, T2 *current, count_t dimensions)throw()
		{
			T2 len = (vsqrt( (Vec::Operator4_sumD(current, center, dimensions)/*sqr(current:i - center:i)*/) ));
			if (len > TypeInfo<T2>::error)
			{
				
				//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={center, (distance/len), current}...
				{
					for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
					{
						current[iterator__] = center[iterator__]+(current[iterator__]-center[iterator__])*(distance/len);
					}
				};
			}
			else
				current[0] = len;
		}

	//now implementing template definition 'void normalize (2..4) (<[*] vector>) direct='
	/**
		@brief <br>
		<br>
		normalize() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] vector 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0>
		inline	void	__fastcall	normalizeD(T0 *vector, count_t dimensions)throw()
		{
			T0 len = vsqrt((Vec::Operator1_sumD(vector, dimensions)/*vector:i*vector:i*/));
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				vector[iterator__] /= len;
			}
		}

	//now implementing template definition 'void normalize0 (2..4) (<[*] vector>) direct='
	/**
		@brief <br>
		<br>
		normalize0() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] vector 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0>
		inline	void	__fastcall	normalize0D(T0 *vector, count_t dimensions)throw()
		{
			T0 len = (Vec::Operator1_sumD(vector, dimensions)/*vector:i*vector:i*/);
			if (isnan(len) || len <= TypeInfo<T0>::error)
			{
				vector[0] = 1;
				for (register index_t iterator__=1; iterator__ < dimensions; iterator__++)
				{
					vector[iterator__] = 0;
				}
				return;
			}
			len = vsqrt(len);
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				vector[iterator__] /= len;
			}
		}

	//now implementing template definition 'void abs (2..4) (<[*] v>) direct=1'
	/**
		@brief <br>
		<br>
		abs() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] v 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0>
		inline	void	__fastcall	absD(T0 *v, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] = vabs(v[iterator__]);
			}
		}

	//now implementing template definition 'void setLen (2..4) (<[*] current>, <length>) direct='
	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		setLen() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLenD(T0 *current, T1 length, count_t dimensions)throw()
		{
			T0 len = vsqrt( (Vec::Operator1_sumD(current, dimensions)/*current:i*current:i*/) );
			T0 fc = T0(length)/len;
			
			//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={current, fc}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					current[iterator__] *= fc;
				}
			};
		}

	//now implementing template definition 'void setLen0 (2..4) (<[*] current>, <length>) direct='
	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		setLen0() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLen0D(T0 *current, T1 length, count_t dimensions)throw()
		{
			T0 len = vsqrt( (Vec::Operator1_sumD(current, dimensions)/*current:i*current:i*/) );
			if (vabs(len) > TypeInfo<T0>::error)
			{
				T0 fc = T0(length)/len;
				
				//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={current, fc}...
				{
					for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
					{
						current[iterator__] *= fc;
					}
				};
			}
			else
				current[0] = len;
		}

	//now implementing template definition 'T0 angle (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines the angle between @a v and @a w<br>
		<br>
		angle() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@param dimensions [in] Number of dimensions to process
		@return angle in the range [0,180] 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	angleD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return acos(Math::clamp((Vec::Operator0_sumD(v, w, dimensions)/*v:i*w:i*/)/((vsqrt((Vec::Operator1_sumD(v, dimensions)/*v:i*v:i*/)))*(vsqrt((Vec::Operator1_sumD(w, dimensions)/*w:i*w:i*/)))),-1,1))*180/M_PI;
		}

	//now implementing template definition 'bool isNAN (2..4) (<const [*] v>) direct='
	/**
		@brief Tests if at least one component of @a v is NAN (Not A Number). Not applicable to integer types<br>
		<br>
		isNAN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	isNAND(const T0 *v, count_t dimensions)throw()
		{
			return Vec::LogicOperator8_orD(v, dimensions)/*isnan(v:i)*/;
		}

	//now implementing template definition 'T0 intensity (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines the light intensity (the cos of the angle) between @a v and @a w<br>
		<br>
		intensity() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	intensityD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return (Vec::Operator0_sumD(v, w, dimensions)/*v:i*w:i*/)/((vsqrt((Vec::Operator1_sumD(v, dimensions)/*v:i*v:i*/)))*(vsqrt((Vec::Operator1_sumD(w, dimensions)/*w:i*w:i*/))));
		}

	//now implementing template definition 'void set (2..16) (<[*] v>, <value>) direct=1'
	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		set() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setD(T0 *v, T1 value, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] = value;
			}
		}

	//now implementing template definition 'void clamp (2..4) (<[*] v>, <min>, <max>) direct=1'
	/**
		@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
		<br>
		clamp() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] min 
		@param[in] max 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	clampD(T0 *v, T1 min, T2 max, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] = clamped(v[iterator__],min,max);
			}
		}

	//now implementing template definition 'void clear (2..4) (<[*] v>) direct=1'
	/**
		@brief Sets all components of @a v to 0<br>
		<br>
		clear() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] v 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0>
		inline	void	__fastcall	clearD(T0 *v, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				v[iterator__] = 0;
			}
		}

	//now implementing template definition 'T0 min (2..4) (<const [*] v>) direct='
	/**
		@brief Determines the minimum coordinate value of @a v<br>
		<br>
		min() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	minD(const T0 *v, count_t dimensions)throw()
		{
			return Vec::Functional9_minD(v, dimensions)/*v:i*/;
		}

	//now implementing template definition 'T0 max (2..4) (<const [*] v>) direct='
	/**
		@brief Determines the maximum coordinate value of @a v<br>
		<br>
		max() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	maxD(const T0 *v, count_t dimensions)throw()
		{
			return Vec::Functional10_maxD(v, dimensions)/*v:i*/;
		}

	//now implementing template definition 'void max (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
		<br>
		max() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	maxD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = vmax( v[iterator__], w[iterator__]);
			}
		}

	//now implementing template definition 'void min (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
		<br>
		min() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	minD(const T0 *v, const T1 *w, T2 *result, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				result[iterator__] = vmin( v[iterator__], w[iterator__]);
			}
		}

	//now implementing template definition 'bool oneLess (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if at least one element of @a v is less than the respective element of @a w<br>
		<br>
		oneLess() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneLessD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::LogicOperator11_orD(v, w, dimensions)/*v:i < w:i*/;
		}

	//now implementing template definition 'bool oneGreater (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if at least one element of @a v is greater than the respective element of @a w<br>
		<br>
		oneGreater() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneGreaterD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::LogicOperator12_orD(v, w, dimensions)/*v:i > w:i*/;
		}

	//now implementing template definition 'bool allLess (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if all elements of @a v are less than the respective element of @a w<br>
		<br>
		allLess() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allLessD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::LogicOperator13_andD(v, w, dimensions)/*v:i < w:i*/;
		}

	//now implementing template definition 'bool allGreater (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if all elements of @a v are greater than the respective element of @a w<br>
		<br>
		allGreater() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] v 
		@param[in] w 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allGreaterD(const T0 *v, const T1 *w, count_t dimensions)throw()
		{
			return Vec::LogicOperator14_andD(v, w, dimensions)/*v:i > w:i*/;
		}

	//now implementing template definition 'void vectorSort (2..4) (<[*] v>, <[*] w>) direct=1'
	/**
		@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
		<br>
		vectorSort() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[out] v 
		@param[out] w 
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	vectorSortD(T0 *v, T1 *w, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				if (w[iterator__]<v[iterator__])
					swp(v[iterator__],w[iterator__]);
			}
		}

	//now implementing template definition 'void copy|c (2..16) (<const [*] origin>, <[*] destination>) direct=1'
	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		copy() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
		@param dimensions [in] Number of dimensions to process
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copyD(const T0 *origin, T1 *destination, count_t dimensions)throw()
		{
			for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
			{
				destination[iterator__] = (T1)origin[iterator__];
			}
		}

	//now implementing template definition 'T3 intercept (2..4) (<const [*] position>, <const [*] velocity>, <interception_velocity>, <[*] result>) direct='
	/**
		@brief Calculates the time and direction of interception of an object at @a position moving with a specific constant speed and direction @a velocity<br>
		<br>
		intercept() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] position 
		@param[in] velocity 
		@param[in] interception_velocity 
		@param[out] result normalized direction of intersection, if any; the value of this variable remains unchanged if no interseption could be determined
		@param dimensions [in] Number of dimensions to process
		@return time of intersection, or 0 if no such could be determined 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	T3	__fastcall	interceptD(const T0 *position, const T1 *velocity, T2 interception_velocity, T3 *result, count_t dimensions)throw()
		{
			T3	rs[2],
					a = (Vec::Operator1_sumD(velocity, dimensions)/*velocity:i*velocity:i*/) - interception_velocity*interception_velocity,
					b = 2*(Vec::Operator0_sumD(position, velocity, dimensions)/*position:i*velocity:i*/),
					c = (Vec::Operator1_sumD(position, dimensions)/*position:i*position:i*/);
			BYTE num_rs = solveSqrEquation(a, b, c, rs);
			if (!num_rs)
				return 0;
			T3	t = smallestPositiveResult(rs,num_rs);
			if (t<=0)
				return 0;
			T3 f = T3(1)/t;
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='DynamicPointers', parameters={velocity, position, f, result}...
			{
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					result[iterator__] = velocity[iterator__] + position[iterator__] * f;
				}
			};
			
			//block inlining void normalize (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='DynamicPointers', parameters={result}...
			{
				T3 len = vsqrt((Vec::Operator1_sumD(result, dimensions)/*result:i*result:i*/));
				for (register index_t iterator__=0; iterator__ < dimensions; iterator__++)
				{
					result[iterator__] /= len;
				}
			};
			return t;
		}

	//now implementing template definition 'String toString (2..4) (<const [*] vector>) direct='
	/**
		@brief <br>
		<br>
		toString() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (@a dimensions in general for this version)<br>
	
		@param[in] vector 
		@param dimensions [in] Number of dimensions to process
		@return  
	*/
	template <typename T0>
		inline	String	__fastcall	toStringD(const T0 *vector, count_t dimensions)throw()
		{
			return '('+ Vec::Operator15_glueD(vector, dimensions)/*", "*/+')';
		}


		/* ----- Now implementing fixed_dimensions instances ----- */
	//now implementing template definition 'void addValue|addVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		2 dimensional specialized version of addValue/addVal()<br>
		addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addValue(const TVec2<T0>& v, T1 value, TVec2<T2>& result)throw()
		{
			result.x = v.x + value;
			result.y = v.y + value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		2 dimensional specialized version of addValue/addVal()<br>
		addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addVal(const TVec2<T0>& v, T1 value, TVec2<T2>& result)throw()
		{
			result.x = v.x + value;
			result.y = v.y + value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		3 dimensional specialized version of addValue/addVal()<br>
		addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addValue(const TVec3<T0>& v, T1 value, TVec3<T2>& result)throw()
		{
			result.x = v.x + value;
			result.y = v.y + value;
			result.z = v.z + value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		3 dimensional specialized version of addValue/addVal()<br>
		addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addVal(const TVec3<T0>& v, T1 value, TVec3<T2>& result)throw()
		{
			result.x = v.x + value;
			result.y = v.y + value;
			result.z = v.z + value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		4 dimensional specialized version of addValue/addVal()<br>
		addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addValue(const TVec4<T0>& v, T1 value, TVec4<T2>& result)throw()
		{
			result.x = v.x + value;
			result.y = v.y + value;
			result.z = v.z + value;
			result.w = v.w + value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		4 dimensional specialized version of addValue/addVal()<br>
		addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to add to
		@param[in] value Value to add
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	addVal(const TVec4<T0>& v, T1 value, TVec4<T2>& result)throw()
		{
			result.x = v.x + value;
			result.y = v.y + value;
			result.z = v.z + value;
			result.w = v.w + value;
		}

	//now implementing template definition 'void addValue|addVal (2..4) (<[*] v>, <value>) direct=1'
	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		2 dimensional specialized version of addValue/addVal()<br>
		addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addValue(TVec2<T0>& v, T1 value)throw()
		{
			v.x += value;
			v.y += value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		2 dimensional specialized version of addValue/addVal()<br>
		addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addVal(TVec2<T0>& v, T1 value)throw()
		{
			v.x += value;
			v.y += value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		3 dimensional specialized version of addValue/addVal()<br>
		addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addValue(TVec3<T0>& v, T1 value)throw()
		{
			v.x += value;
			v.y += value;
			v.z += value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		3 dimensional specialized version of addValue/addVal()<br>
		addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addVal(TVec3<T0>& v, T1 value)throw()
		{
			v.x += value;
			v.y += value;
			v.z += value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		4 dimensional specialized version of addValue/addVal()<br>
		addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addValue(TVec4<T0>& v, T1 value)throw()
		{
			v.x += value;
			v.y += value;
			v.z += value;
			v.w += value;
		}

	/**
		@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		4 dimensional specialized version of addValue/addVal()<br>
		addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to add to
		@param[in] value Value to add
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	addVal(TVec4<T0>& v, T1 value)throw()
		{
			v.x += value;
			v.y += value;
			v.z += value;
			v.w += value;
		}

	//now implementing template definition 'void add (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	add(const TVec2<T0>& u, const TVec2<T1>& v, const TVec2<T2>& w, TVec2<T3>& result)throw()
		{
			result.x = u.x + v.x + w.x;
			result.y = u.y + v.y + w.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	add(const TVec3<T0>& u, const TVec3<T1>& v, const TVec3<T2>& w, TVec3<T3>& result)throw()
		{
			result.x = u.x + v.x + w.x;
			result.y = u.y + v.y + w.y;
			result.z = u.z + v.z + w.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	add(const TVec4<T0>& u, const TVec4<T1>& v, const TVec4<T2>& w, TVec4<T3>& result)throw()
		{
			result.x = u.x + v.x + w.x;
			result.y = u.y + v.y + w.y;
			result.z = u.z + v.z + w.z;
			result.w = u.w + v.w + w.w;
		}

	//now implementing template definition 'void add (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	add(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = v.x+w.x;
			result.y = v.y+w.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	add(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = v.x+w.x;
			result.y = v.y+w.y;
			result.z = v.z+w.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	add(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = v.x+w.x;
			result.y = v.y+w.y;
			result.z = v.z+w.z;
			result.w = v.w+w.w;
		}

	//now implementing template definition 'void add (2..4) (<[*] v>, <const [*] w>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	add(TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			v.x += w.x;
			v.y += w.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	add(TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			v.x += w.x;
			v.y += w.y;
			v.z += w.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of add()<br>
		add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	add(TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			v.x += w.x;
			v.y += w.y;
			v.z += w.z;
			v.w += w.w;
		}

	//now implementing template definition 'void subtract|sub (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of subtract/sub()<br>
		subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtract(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = v.x - w.x;
			result.y = v.y - w.y;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of subtract/sub()<br>
		sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	sub(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = v.x - w.x;
			result.y = v.y - w.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of subtract/sub()<br>
		subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtract(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = v.x - w.x;
			result.y = v.y - w.y;
			result.z = v.z - w.z;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of subtract/sub()<br>
		sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	sub(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = v.x - w.x;
			result.y = v.y - w.y;
			result.z = v.z - w.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of subtract/sub()<br>
		subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtract(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = v.x - w.x;
			result.y = v.y - w.y;
			result.z = v.z - w.z;
			result.w = v.w - w.w;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of subtract/sub()<br>
		sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	sub(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = v.x - w.x;
			result.y = v.y - w.y;
			result.z = v.z - w.z;
			result.w = v.w - w.w;
		}

	//now implementing template definition 'void subtract|sub (2..4) (<[*] v>, <const [*] w>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of subtract/sub()<br>
		subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtract(TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			v.x -= w.x;
			v.y -= w.y;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of subtract/sub()<br>
		sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	sub(TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			v.x -= w.x;
			v.y -= w.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of subtract/sub()<br>
		subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtract(TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			v.x -= w.x;
			v.y -= w.y;
			v.z -= w.z;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of subtract/sub()<br>
		sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	sub(TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			v.x -= w.x;
			v.y -= w.y;
			v.z -= w.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of subtract/sub()<br>
		subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtract(TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			v.x -= w.x;
			v.y -= w.y;
			v.z -= w.z;
			v.w -= w.w;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of subtract/sub()<br>
		sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	sub(TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			v.x -= w.x;
			v.y -= w.y;
			v.z -= w.z;
			v.w -= w.w;
		}

	//now implementing template definition 'void subtractValue|subVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		2 dimensional specialized version of subtractValue/subVal()<br>
		subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtractValue(const TVec2<T0>& v, T1 value, TVec2<T2>& result)throw()
		{
			result.x = v.x - value;
			result.y = v.y - value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		2 dimensional specialized version of subtractValue/subVal()<br>
		subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subVal(const TVec2<T0>& v, T1 value, TVec2<T2>& result)throw()
		{
			result.x = v.x - value;
			result.y = v.y - value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		3 dimensional specialized version of subtractValue/subVal()<br>
		subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtractValue(const TVec3<T0>& v, T1 value, TVec3<T2>& result)throw()
		{
			result.x = v.x - value;
			result.y = v.y - value;
			result.z = v.z - value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		3 dimensional specialized version of subtractValue/subVal()<br>
		subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subVal(const TVec3<T0>& v, T1 value, TVec3<T2>& result)throw()
		{
			result.x = v.x - value;
			result.y = v.y - value;
			result.z = v.z - value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		4 dimensional specialized version of subtractValue/subVal()<br>
		subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subtractValue(const TVec4<T0>& v, T1 value, TVec4<T2>& result)throw()
		{
			result.x = v.x - value;
			result.y = v.y - value;
			result.z = v.z - value;
			result.w = v.w - value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
		<br>
		4 dimensional specialized version of subtractValue/subVal()<br>
		subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v Vector to subtract from
		@param[in] value Value to subtract
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	subVal(const TVec4<T0>& v, T1 value, TVec4<T2>& result)throw()
		{
			result.x = v.x - value;
			result.y = v.y - value;
			result.z = v.z - value;
			result.w = v.w - value;
		}

	//now implementing template definition 'void subtractValue|subVal (2..4) (<[*] v>, <value>) direct=1'
	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		2 dimensional specialized version of subtractValue/subVal()<br>
		subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtractValue(TVec2<T0>& v, T1 value)throw()
		{
			v.x -= value;
			v.y -= value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		2 dimensional specialized version of subtractValue/subVal()<br>
		subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subVal(TVec2<T0>& v, T1 value)throw()
		{
			v.x -= value;
			v.y -= value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		3 dimensional specialized version of subtractValue/subVal()<br>
		subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtractValue(TVec3<T0>& v, T1 value)throw()
		{
			v.x -= value;
			v.y -= value;
			v.z -= value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		3 dimensional specialized version of subtractValue/subVal()<br>
		subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subVal(TVec3<T0>& v, T1 value)throw()
		{
			v.x -= value;
			v.y -= value;
			v.z -= value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		4 dimensional specialized version of subtractValue/subVal()<br>
		subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subtractValue(TVec4<T0>& v, T1 value)throw()
		{
			v.x -= value;
			v.y -= value;
			v.z -= value;
			v.w -= value;
		}

	/**
		@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
		<br>
		4 dimensional specialized version of subtractValue/subVal()<br>
		subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v Vector to subtract from
		@param[in] value Value to subtract
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	subVal(TVec4<T0>& v, T1 value)throw()
		{
			v.x -= value;
			v.y -= value;
			v.z -= value;
			v.w -= value;
		}

	//now implementing template definition 'void multiply|mult|mul (2..4) (<const [*] v>, <factor>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of multiply/mult/mul()<br>
		multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multiply(const TVec2<T0>& v, T1 factor, TVec2<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of multiply/mult/mul()<br>
		mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mult(const TVec2<T0>& v, T1 factor, TVec2<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of multiply/mult/mul()<br>
		mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mul(const TVec2<T0>& v, T1 factor, TVec2<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of multiply/mult/mul()<br>
		multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multiply(const TVec3<T0>& v, T1 factor, TVec3<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
			result.z = v.z * factor;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of multiply/mult/mul()<br>
		mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mult(const TVec3<T0>& v, T1 factor, TVec3<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
			result.z = v.z * factor;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of multiply/mult/mul()<br>
		mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mul(const TVec3<T0>& v, T1 factor, TVec3<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
			result.z = v.z * factor;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of multiply/mult/mul()<br>
		multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multiply(const TVec4<T0>& v, T1 factor, TVec4<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
			result.z = v.z * factor;
			result.w = v.w * factor;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of multiply/mult/mul()<br>
		mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mult(const TVec4<T0>& v, T1 factor, TVec4<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
			result.z = v.z * factor;
			result.w = v.w * factor;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of multiply/mult/mul()<br>
		mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] factor 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mul(const TVec4<T0>& v, T1 factor, TVec4<T2>& result)throw()
		{
			result.x = v.x * factor;
			result.y = v.y * factor;
			result.z = v.z * factor;
			result.w = v.w * factor;
		}

	//now implementing template definition 'void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of multiply/mult/mul()<br>
		multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	multiply(TVec2<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of multiply/mult/mul()<br>
		mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	mult(TVec2<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of multiply/mult/mul()<br>
		mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	mul(TVec2<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of multiply/mult/mul()<br>
		multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	multiply(TVec3<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
			v.z *= factor;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of multiply/mult/mul()<br>
		mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	mult(TVec3<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
			v.z *= factor;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of multiply/mult/mul()<br>
		mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	mul(TVec3<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
			v.z *= factor;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of multiply/mult/mul()<br>
		multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	multiply(TVec4<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
			v.z *= factor;
			v.w *= factor;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of multiply/mult/mul()<br>
		mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	mult(TVec4<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
			v.z *= factor;
			v.w *= factor;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of multiply/mult/mul()<br>
		mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] factor 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	mul(TVec4<T0>& v, T1 factor)throw()
		{
			v.x *= factor;
			v.y *= factor;
			v.z *= factor;
			v.w *= factor;
		}

	//now implementing template definition 'void divide|div (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of divide/div()<br>
		divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	divide(const TVec2<T0>& v, T1 value, TVec2<T2>& result)throw()
		{
			result.x = v.x / value;
			result.y = v.y / value;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of divide/div()<br>
		div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	div(const TVec2<T0>& v, T1 value, TVec2<T2>& result)throw()
		{
			result.x = v.x / value;
			result.y = v.y / value;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of divide/div()<br>
		divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	divide(const TVec3<T0>& v, T1 value, TVec3<T2>& result)throw()
		{
			result.x = v.x / value;
			result.y = v.y / value;
			result.z = v.z / value;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of divide/div()<br>
		div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	div(const TVec3<T0>& v, T1 value, TVec3<T2>& result)throw()
		{
			result.x = v.x / value;
			result.y = v.y / value;
			result.z = v.z / value;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of divide/div()<br>
		divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	divide(const TVec4<T0>& v, T1 value, TVec4<T2>& result)throw()
		{
			result.x = v.x / value;
			result.y = v.y / value;
			result.z = v.z / value;
			result.w = v.w / value;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of divide/div()<br>
		div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] value 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	div(const TVec4<T0>& v, T1 value, TVec4<T2>& result)throw()
		{
			result.x = v.x / value;
			result.y = v.y / value;
			result.z = v.z / value;
			result.w = v.w / value;
		}

	//now implementing template definition 'void divide|div (2..4) (<[*] v>, <value>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of divide/div()<br>
		divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	divide(TVec2<T0>& v, T1 value)throw()
		{
			v.x /= value;
			v.y /= value;
		}

	/**
		@brief <br>
		<br>
		2 dimensional specialized version of divide/div()<br>
		div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	div(TVec2<T0>& v, T1 value)throw()
		{
			v.x /= value;
			v.y /= value;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of divide/div()<br>
		divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	divide(TVec3<T0>& v, T1 value)throw()
		{
			v.x /= value;
			v.y /= value;
			v.z /= value;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of divide/div()<br>
		div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	div(TVec3<T0>& v, T1 value)throw()
		{
			v.x /= value;
			v.y /= value;
			v.z /= value;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of divide/div()<br>
		divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	divide(TVec4<T0>& v, T1 value)throw()
		{
			v.x /= value;
			v.y /= value;
			v.z /= value;
			v.w /= value;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of divide/div()<br>
		div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	div(TVec4<T0>& v, T1 value)throw()
		{
			v.x /= value;
			v.y /= value;
			v.z /= value;
			v.w /= value;
		}

	//now implementing template definition 'void resolve (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of resolve()<br>
		resolve() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	resolve(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = v.x / w.x;
			result.y = v.y / w.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of resolve()<br>
		resolve() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	resolve(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = v.x / w.x;
			result.y = v.y / w.y;
			result.z = v.z / w.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of resolve()<br>
		resolve() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	resolve(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = v.x / w.x;
			result.y = v.y / w.y;
			result.z = v.z / w.z;
			result.w = v.w / w.w;
		}

	//now implementing template definition 'void stretch (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the component-wise product of v and w to result<br>
		<br>
		2 dimensional specialized version of stretch()<br>
		stretch() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	stretch(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = v.x * w.x;
			result.y = v.y * w.y;
		}

	/**
		@brief Writes the component-wise product of v and w to result<br>
		<br>
		3 dimensional specialized version of stretch()<br>
		stretch() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	stretch(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = v.x * w.x;
			result.y = v.y * w.y;
			result.z = v.z * w.z;
		}

	/**
		@brief Writes the component-wise product of v and w to result<br>
		<br>
		4 dimensional specialized version of stretch()<br>
		stretch() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	stretch(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = v.x * w.x;
			result.y = v.y * w.y;
			result.z = v.z * w.z;
			result.w = v.w * w.w;
		}

	//now implementing template definition 'void interpolate (2..4) (<const [*] v>, <const [*] w>, <f>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of interpolate()<br>
		interpolate() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] f 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	interpolate(const TVec2<T0>& v, const TVec2<T1>& w, T2 f, TVec2<T3>& result)throw()
		{
			T3 i_ = T3(1)-f;
			result.x = v.x * i_ + w.x * f;
			result.y = v.y * i_ + w.y * f;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of interpolate()<br>
		interpolate() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] f 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	interpolate(const TVec3<T0>& v, const TVec3<T1>& w, T2 f, TVec3<T3>& result)throw()
		{
			T3 i_ = T3(1)-f;
			result.x = v.x * i_ + w.x * f;
			result.y = v.y * i_ + w.y * f;
			result.z = v.z * i_ + w.z * f;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of interpolate()<br>
		interpolate() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] f 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	interpolate(const TVec4<T0>& v, const TVec4<T1>& w, T2 f, TVec4<T3>& result)throw()
		{
			T3 i_ = T3(1)-f;
			result.x = v.x * i_ + w.x * f;
			result.y = v.y * i_ + w.y * f;
			result.z = v.z * i_ + w.z * f;
			result.w = v.w * i_ + w.w * f;
		}

	//now implementing template definition 'void center (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the arithmetic center of v and w to result<br>
		<br>
		2 dimensional specialized version of center()<br>
		center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	center(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = (v.x + w.x)/T2(2);
			result.y = (v.y + w.y)/T2(2);
		}

	/**
		@brief Writes the arithmetic center of v and w to result<br>
		<br>
		3 dimensional specialized version of center()<br>
		center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	center(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = (v.x + w.x)/T2(2);
			result.y = (v.y + w.y)/T2(2);
			result.z = (v.z + w.z)/T2(2);
		}

	/**
		@brief Writes the arithmetic center of v and w to result<br>
		<br>
		4 dimensional specialized version of center()<br>
		center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	center(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = (v.x + w.x)/T2(2);
			result.y = (v.y + w.y)/T2(2);
			result.z = (v.z + w.z)/T2(2);
			result.w = (v.w + w.w)/T2(2);
		}

	//now implementing template definition 'void center (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the arithmetic center of u, v, and w to result<br>
		<br>
		2 dimensional specialized version of center()<br>
		center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	center(const TVec2<T0>& u, const TVec2<T1>& v, const TVec2<T2>& w, TVec2<T3>& result)throw()
		{
			result.x = (u.x + v.x + w.x)/T3(3);
			result.y = (u.y + v.y + w.y)/T3(3);
		}

	/**
		@brief Writes the arithmetic center of u, v, and w to result<br>
		<br>
		3 dimensional specialized version of center()<br>
		center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	center(const TVec3<T0>& u, const TVec3<T1>& v, const TVec3<T2>& w, TVec3<T3>& result)throw()
		{
			result.x = (u.x + v.x + w.x)/T3(3);
			result.y = (u.y + v.y + w.y)/T3(3);
			result.z = (u.z + v.z + w.z)/T3(3);
		}

	/**
		@brief Writes the arithmetic center of u, v, and w to result<br>
		<br>
		4 dimensional specialized version of center()<br>
		center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	center(const TVec4<T0>& u, const TVec4<T1>& v, const TVec4<T2>& w, TVec4<T3>& result)throw()
		{
			result.x = (u.x + v.x + w.x)/T3(3);
			result.y = (u.y + v.y + w.y)/T3(3);
			result.z = (u.z + v.z + w.z)/T3(3);
			result.w = (u.w + v.w + w.w)/T3(3);
		}

	//now implementing template definition 'void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1'
	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		2 dimensional specialized version of multAdd/mad()<br>
		multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multAdd(TVec2<T0>& current, const TVec2<T1>& vector, T2 scalar)throw()
		{
			current.x += vector.x * scalar;
			current.y += vector.y * scalar;
		}

	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		2 dimensional specialized version of multAdd/mad()<br>
		mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mad(TVec2<T0>& current, const TVec2<T1>& vector, T2 scalar)throw()
		{
			current.x += vector.x * scalar;
			current.y += vector.y * scalar;
		}

	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		3 dimensional specialized version of multAdd/mad()<br>
		multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multAdd(TVec3<T0>& current, const TVec3<T1>& vector, T2 scalar)throw()
		{
			current.x += vector.x * scalar;
			current.y += vector.y * scalar;
			current.z += vector.z * scalar;
		}

	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		3 dimensional specialized version of multAdd/mad()<br>
		mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mad(TVec3<T0>& current, const TVec3<T1>& vector, T2 scalar)throw()
		{
			current.x += vector.x * scalar;
			current.y += vector.y * scalar;
			current.z += vector.z * scalar;
		}

	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		4 dimensional specialized version of multAdd/mad()<br>
		multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	multAdd(TVec4<T0>& current, const TVec4<T1>& vector, T2 scalar)throw()
		{
			current.x += vector.x * scalar;
			current.y += vector.y * scalar;
			current.z += vector.z * scalar;
			current.w += vector.w * scalar;
		}

	/**
		@brief Adds @a vector * @a scalar to @a current<br>
		<br>
		4 dimensional specialized version of multAdd/mad()<br>
		mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current 
		@param[in] vector 
		@param[in] scalar 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	mad(TVec4<T0>& current, const TVec4<T1>& vector, T2 scalar)throw()
		{
			current.x += vector.x * scalar;
			current.y += vector.y * scalar;
			current.z += vector.z * scalar;
			current.w += vector.w * scalar;
		}

	//now implementing template definition 'void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1'
	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		2 dimensional specialized version of multAdd/mad()<br>
		multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	multAdd(const TVec2<T0>& base, const TVec2<T1>& vector, T2 scalar, TVec2<T3>& result)throw()
		{
			result.x = base.x + vector.x * scalar;
			result.y = base.y + vector.y * scalar;
		}

	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		2 dimensional specialized version of multAdd/mad()<br>
		mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	mad(const TVec2<T0>& base, const TVec2<T1>& vector, T2 scalar, TVec2<T3>& result)throw()
		{
			result.x = base.x + vector.x * scalar;
			result.y = base.y + vector.y * scalar;
		}

	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		3 dimensional specialized version of multAdd/mad()<br>
		multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	multAdd(const TVec3<T0>& base, const TVec3<T1>& vector, T2 scalar, TVec3<T3>& result)throw()
		{
			result.x = base.x + vector.x * scalar;
			result.y = base.y + vector.y * scalar;
			result.z = base.z + vector.z * scalar;
		}

	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		3 dimensional specialized version of multAdd/mad()<br>
		mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	mad(const TVec3<T0>& base, const TVec3<T1>& vector, T2 scalar, TVec3<T3>& result)throw()
		{
			result.x = base.x + vector.x * scalar;
			result.y = base.y + vector.y * scalar;
			result.z = base.z + vector.z * scalar;
		}

	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		4 dimensional specialized version of multAdd/mad()<br>
		multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	multAdd(const TVec4<T0>& base, const TVec4<T1>& vector, T2 scalar, TVec4<T3>& result)throw()
		{
			result.x = base.x + vector.x * scalar;
			result.y = base.y + vector.y * scalar;
			result.z = base.z + vector.z * scalar;
			result.w = base.w + vector.w * scalar;
		}

	/**
		@brief Writes @a base + @a vector * @a scalar to @a result<br>
		<br>
		4 dimensional specialized version of multAdd/mad()<br>
		mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] base 
		@param[in] vector 
		@param[in] scalar 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	mad(const TVec4<T0>& base, const TVec4<T1>& vector, T2 scalar, TVec4<T3>& result)throw()
		{
			result.x = base.x + vector.x * scalar;
			result.y = base.y + vector.y * scalar;
			result.z = base.z + vector.z * scalar;
			result.w = base.w + vector.w * scalar;
		}

	//now implementing template definition 'T0 dot (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Calculates the dot product of @a v and @a w<br>
		<br>
		2 dimensional specialized version of dot()<br>
		dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	dot(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return v.x*w.x + v.y*w.y;
		}

	/**
		@brief Calculates the dot product of @a v and @a w<br>
		<br>
		3 dimensional specialized version of dot()<br>
		dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	dot(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return v.x*w.x + v.y*w.y + v.z*w.z;
		}

	/**
		@brief Calculates the dot product of @a v and @a w<br>
		<br>
		4 dimensional specialized version of dot()<br>
		dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	dot(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return v.x*w.x + v.y*w.y + v.z*w.z + v.w*w.w;
		}

	//now implementing template definition 'T0 dot (2..4) (<const [*] v>) direct='
	/**
		@brief Calculates the dot product @a v * @a v<br>
		<br>
		2 dimensional specialized version of dot()<br>
		dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	dot(const TVec2<T0>& v)throw()
		{
			return v.x*v.x + v.y*v.y;
		}

	/**
		@brief Calculates the dot product @a v * @a v<br>
		<br>
		3 dimensional specialized version of dot()<br>
		dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	dot(const TVec3<T0>& v)throw()
		{
			return v.x*v.x + v.y*v.y + v.z*v.z;
		}

	/**
		@brief Calculates the dot product @a v * @a v<br>
		<br>
		4 dimensional specialized version of dot()<br>
		dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	dot(const TVec4<T0>& v)throw()
		{
			return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
		}

	//now implementing template definition 'T0 sum (2..4) (<const [*] v>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of sum()<br>
		sum() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	sum(const TVec2<T0>& v)throw()
		{
			return v.x + v.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of sum()<br>
		sum() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	sum(const TVec3<T0>& v)throw()
		{
			return v.x + v.y + v.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of sum()<br>
		sum() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	sum(const TVec4<T0>& v)throw()
		{
			return v.x + v.y + v.z + v.w;
		}

	//now implementing template definition 'bool zero (2..4) (<const [*] v>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of zero()<br>
		zero() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	zero(const TVec2<T0>& v)throw()
		{
			return !v.x && !v.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of zero()<br>
		zero() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	zero(const TVec3<T0>& v)throw()
		{
			return !v.x && !v.y && !v.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of zero()<br>
		zero() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	zero(const TVec4<T0>& v)throw()
		{
			return !v.x && !v.y && !v.z && !v.w;
		}

	//now implementing template definition 'T0 length (2..4) (<const [*] v>) direct='
	/**
		@brief Determines the length of @a v<br>
		<br>
		2 dimensional specialized version of length()<br>
		length() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v Vector to determine the length of
		@return Length of <paramref>v</paramref> 
	*/
	template <typename T0>
		inline	T0	__fastcall	length(const TVec2<T0>& v)throw()
		{
			return vsqrt((v.x*v.x + v.y*v.y));
		}

	/**
		@brief Determines the length of @a v<br>
		<br>
		3 dimensional specialized version of length()<br>
		length() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v Vector to determine the length of
		@return Length of <paramref>v</paramref> 
	*/
	template <typename T0>
		inline	T0	__fastcall	length(const TVec3<T0>& v)throw()
		{
			return vsqrt((v.x*v.x + v.y*v.y + v.z*v.z));
		}

	/**
		@brief Determines the length of @a v<br>
		<br>
		4 dimensional specialized version of length()<br>
		length() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v Vector to determine the length of
		@return Length of <paramref>v</paramref> 
	*/
	template <typename T0>
		inline	T0	__fastcall	length(const TVec4<T0>& v)throw()
		{
			return vsqrt((v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w));
		}

	//now implementing template definition 'T0 quadraticDistance (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of quadraticDistance()<br>
		quadraticDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	quadraticDistance(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return sqr(v.x - w.x) + sqr(v.y - w.y);
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of quadraticDistance()<br>
		quadraticDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	quadraticDistance(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z);
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of quadraticDistance()<br>
		quadraticDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	quadraticDistance(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z) + sqr(v.w - w.w);
		}

	//now implementing template definition 'T0 distance (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Calculates the distance between @a v0 and @a v1<br>
		<br>
		2 dimensional specialized version of distance()<br>
		distance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	distance(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return vsqrt( (sqr(v.x - w.x) + sqr(v.y - w.y)) );
		}

	/**
		@brief Calculates the distance between @a v0 and @a v1<br>
		<br>
		3 dimensional specialized version of distance()<br>
		distance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	distance(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return vsqrt( (sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z)) );
		}

	/**
		@brief Calculates the distance between @a v0 and @a v1<br>
		<br>
		4 dimensional specialized version of distance()<br>
		distance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	distance(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return vsqrt( (sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z) + sqr(v.w - w.w)) );
		}

	//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		2 dimensional specialized version of reflectN()<br>
		reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] p The point being reflected
		@param[out] out [out] Result
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflectN(const TVec2<T0>& base, const TVec2<T1>& normal, const TVec2<T2>& p, TVec2<T3>& out)throw()
		{
			T3 fc = 2*( (base.x*normal.x + base.y*normal.y) - (p.x*normal.x + p.y*normal.y) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={p, normal, fc, out}...
			{
				out.x = p.x + normal.x * fc;
				out.y = p.y + normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		3 dimensional specialized version of reflectN()<br>
		reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] p The point being reflected
		@param[out] out [out] Result
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflectN(const TVec3<T0>& base, const TVec3<T1>& normal, const TVec3<T2>& p, TVec3<T3>& out)throw()
		{
			T3 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z) - (p.x*normal.x + p.y*normal.y + p.z*normal.z) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc, out}...
			{
				out.x = p.x + normal.x * fc;
				out.y = p.y + normal.y * fc;
				out.z = p.z + normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		4 dimensional specialized version of reflectN()<br>
		reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] p The point being reflected
		@param[out] out [out] Result
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflectN(const TVec4<T0>& base, const TVec4<T1>& normal, const TVec4<T2>& p, TVec4<T3>& out)throw()
		{
			T3 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z + base.w*normal.w) - (p.x*normal.x + p.y*normal.y + p.z*normal.z + p.w*normal.w) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={p, normal, fc, out}...
			{
				out.x = p.x + normal.x * fc;
				out.y = p.y + normal.y * fc;
				out.z = p.z + normal.z * fc;
				out.w = p.w + normal.w * fc;
			};
		}

	//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		2 dimensional specialized version of reflectN()<br>
		reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] p [inout] The point being reflected
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectN(const TVec2<T0>& base, const TVec2<T1>& normal, TVec2<T2>& p)throw()
		{
			T2 fc = 2*( (base.x*normal.x + base.y*normal.y) - (p.x*normal.x + p.y*normal.y) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={p, normal, fc}...
			{
				p.x += normal.x * fc;
				p.y += normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		3 dimensional specialized version of reflectN()<br>
		reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] p [inout] The point being reflected
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectN(const TVec3<T0>& base, const TVec3<T1>& normal, TVec3<T2>& p)throw()
		{
			T2 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z) - (p.x*normal.x + p.y*normal.y + p.z*normal.z) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc}...
			{
				p.x += normal.x * fc;
				p.y += normal.y * fc;
				p.z += normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
		<br>
		4 dimensional specialized version of reflectN()<br>
		reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] p [inout] The point being reflected
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectN(const TVec4<T0>& base, const TVec4<T1>& normal, TVec4<T2>& p)throw()
		{
			T2 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z + base.w*normal.w) - (p.x*normal.x + p.y*normal.y + p.z*normal.z + p.w*normal.w) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={p, normal, fc}...
			{
				p.x += normal.x * fc;
				p.y += normal.y * fc;
				p.z += normal.z * fc;
				p.w += normal.w * fc;
			};
		}

	//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		2 dimensional specialized version of reflect()<br>
		reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] p The point being reflected
		@param[out] out [out] Result
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflect(const TVec2<T0>& base, const TVec2<T1>& normal, const TVec2<T2>& p, TVec2<T3>& out)throw()
		{
			T3 fc = 2*( (base.x*normal.x + base.y*normal.y) - (p.x*normal.x + p.y*normal.y) ) / (normal.x*normal.x + normal.y*normal.y);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={p, normal, fc, out}...
			{
				out.x = p.x + normal.x * fc;
				out.y = p.y + normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		3 dimensional specialized version of reflect()<br>
		reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] p The point being reflected
		@param[out] out [out] Result
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflect(const TVec3<T0>& base, const TVec3<T1>& normal, const TVec3<T2>& p, TVec3<T3>& out)throw()
		{
			T3 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z) - (p.x*normal.x + p.y*normal.y + p.z*normal.z) ) / (normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc, out}...
			{
				out.x = p.x + normal.x * fc;
				out.y = p.y + normal.y * fc;
				out.z = p.z + normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		4 dimensional specialized version of reflect()<br>
		reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] p The point being reflected
		@param[out] out [out] Result
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	reflect(const TVec4<T0>& base, const TVec4<T1>& normal, const TVec4<T2>& p, TVec4<T3>& out)throw()
		{
			T3 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z + base.w*normal.w) - (p.x*normal.x + p.y*normal.y + p.z*normal.z + p.w*normal.w) ) / (normal.x*normal.x + normal.y*normal.y + normal.z*normal.z + normal.w*normal.w);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={p, normal, fc, out}...
			{
				out.x = p.x + normal.x * fc;
				out.y = p.y + normal.y * fc;
				out.z = p.z + normal.z * fc;
				out.w = p.w + normal.w * fc;
			};
		}

	//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		2 dimensional specialized version of reflect()<br>
		reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] p The point being reflected
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflect(const TVec2<T0>& base, const TVec2<T1>& normal, TVec2<T2>& p)throw()
		{
			T2 fc = 2*( (base.x*normal.x + base.y*normal.y) - (p.x*normal.x + p.y*normal.y) ) / (normal.x*normal.x + normal.y*normal.y);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={p, normal, fc}...
			{
				p.x += normal.x * fc;
				p.y += normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		3 dimensional specialized version of reflect()<br>
		reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] p The point being reflected
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflect(const TVec3<T0>& base, const TVec3<T1>& normal, TVec3<T2>& p)throw()
		{
			T2 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z) - (p.x*normal.x + p.y*normal.y + p.z*normal.z) ) / (normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc}...
			{
				p.x += normal.x * fc;
				p.y += normal.y * fc;
				p.z += normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
		<br>
		4 dimensional specialized version of reflect()<br>
		reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base The base point of the reflecting surface
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] p The point being reflected
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflect(const TVec4<T0>& base, const TVec4<T1>& normal, TVec4<T2>& p)throw()
		{
			T2 fc = 2*( (base.x*normal.x + base.y*normal.y + base.z*normal.z + base.w*normal.w) - (p.x*normal.x + p.y*normal.y + p.z*normal.z + p.w*normal.w) ) / (normal.x*normal.x + normal.y*normal.y + normal.z*normal.z + normal.w*normal.w);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={p, normal, fc}...
			{
				p.x += normal.x * fc;
				p.y += normal.y * fc;
				p.z += normal.z * fc;
				p.w += normal.w * fc;
			};
		}

	//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		2 dimensional specialized version of reflectVectorN()<br>
		reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out Reflection result
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVectorN(const TVec2<T0>& normal, const TVec2<T1>& v, TVec2<T2>& out)throw()
		{
			T2 fc = 2*( - (v.x*normal.x + v.y*normal.y) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={v, normal, fc, out}...
			{
				out.x = v.x + normal.x * fc;
				out.y = v.y + normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		3 dimensional specialized version of reflectVectorN()<br>
		reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out Reflection result
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVectorN(const TVec3<T0>& normal, const TVec3<T1>& v, TVec3<T2>& out)throw()
		{
			T2 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc, out}...
			{
				out.x = v.x + normal.x * fc;
				out.y = v.y + normal.y * fc;
				out.z = v.z + normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		4 dimensional specialized version of reflectVectorN()<br>
		reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out Reflection result
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVectorN(const TVec4<T0>& normal, const TVec4<T1>& v, TVec4<T2>& out)throw()
		{
			T2 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z + v.w*normal.w) );
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={v, normal, fc, out}...
			{
				out.x = v.x + normal.x * fc;
				out.y = v.y + normal.y * fc;
				out.z = v.z + normal.z * fc;
				out.w = v.w + normal.w * fc;
			};
		}

	//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <[*] v>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		2 dimensional specialized version of reflectVectorN()<br>
		reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] v [inout] The vector being reflected. May be of any length.
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVectorN(const TVec2<T0>& normal, TVec2<T1>& v)throw()
		{
			T1 fc = 2*( - (v.x*normal.x + v.y*normal.y) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={v, normal, fc}...
			{
				v.x += normal.x * fc;
				v.y += normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		3 dimensional specialized version of reflectVectorN()<br>
		reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] v [inout] The vector being reflected. May be of any length.
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVectorN(const TVec3<T0>& normal, TVec3<T1>& v)throw()
		{
			T1 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc}...
			{
				v.x += normal.x * fc;
				v.y += normal.y * fc;
				v.z += normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		4 dimensional specialized version of reflectVectorN()<br>
		reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. Must be normalized
		@param[out] v [inout] The vector being reflected. May be of any length.
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVectorN(const TVec4<T0>& normal, TVec4<T1>& v)throw()
		{
			T1 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z + v.w*normal.w) );
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={v, normal, fc}...
			{
				v.x += normal.x * fc;
				v.y += normal.y * fc;
				v.z += normal.z * fc;
				v.w += normal.w * fc;
			};
		}

	//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		2 dimensional specialized version of reflectVector()<br>
		reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out [out] Reflection result
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVector(const TVec2<T0>& normal, const TVec2<T1>& v, TVec2<T2>& out)throw()
		{
			T2 fc = 2*( - (v.x*normal.x + v.y*normal.y) )/(normal.x*normal.x + normal.y*normal.y);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={v, normal, fc, out}...
			{
				out.x = v.x + normal.x * fc;
				out.y = v.y + normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		3 dimensional specialized version of reflectVector()<br>
		reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out [out] Reflection result
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVector(const TVec3<T0>& normal, const TVec3<T1>& v, TVec3<T2>& out)throw()
		{
			T2 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z) )/(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc, out}...
			{
				out.x = v.x + normal.x * fc;
				out.y = v.y + normal.y * fc;
				out.z = v.z + normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		4 dimensional specialized version of reflectVector()<br>
		reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[in] v The vector being reflected. May be of any length.
		@param[out] out [out] Reflection result
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	reflectVector(const TVec4<T0>& normal, const TVec4<T1>& v, TVec4<T2>& out)throw()
		{
			T2 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z + v.w*normal.w) )/(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z + normal.w*normal.w);
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={v, normal, fc, out}...
			{
				out.x = v.x + normal.x * fc;
				out.y = v.y + normal.y * fc;
				out.z = v.z + normal.z * fc;
				out.w = v.w + normal.w * fc;
			};
		}

	//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <[*] v>) direct='
	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		2 dimensional specialized version of reflectVector()<br>
		reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] v [inout] The vector being reflected. May be of any length.
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVector(const TVec2<T0>& normal, TVec2<T1>& v)throw()
		{
			T1 fc = 2*( - (v.x*normal.x + v.y*normal.y) )/(normal.x*normal.x + normal.y*normal.y);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={v, normal, fc}...
			{
				v.x += normal.x * fc;
				v.y += normal.y * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		3 dimensional specialized version of reflectVector()<br>
		reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] v [inout] The vector being reflected. May be of any length.
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVector(const TVec3<T0>& normal, TVec3<T1>& v)throw()
		{
			T1 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z) )/(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc}...
			{
				v.x += normal.x * fc;
				v.y += normal.y * fc;
				v.z += normal.z * fc;
			};
		}

	/**
		@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
		<br>
		4 dimensional specialized version of reflectVector()<br>
		reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] normal The normal of the reflecting surface. May be of any length > 0
		@param[out] v [inout] The vector being reflected. May be of any length.
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	reflectVector(const TVec4<T0>& normal, TVec4<T1>& v)throw()
		{
			T1 fc = 2*( - (v.x*normal.x + v.y*normal.y + v.z*normal.z + v.w*normal.w) )/(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z + normal.w*normal.w);
			
			//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={v, normal, fc}...
			{
				v.x += normal.x * fc;
				v.y += normal.y * fc;
				v.z += normal.z * fc;
				v.w += normal.w * fc;
			};
		}

	//now implementing template definition 'void resolveUCBS (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of resolveUCBS()<br>
		resolveUCBS() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBS(const TVec2<T0>& p0, const TVec2<T1>& p1, const TVec2<T2>& p2, const TVec2<T3>& p3, T4 t, TVec2<T5>& result)throw()
		{
			T5	//i = 1-t,
					f0 = (-t*t*t+3*t*t-3*t+1)/6,
					f1 = (3*t*t*t-6*t*t+4)/6,
					f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
					f3 = (t*t*t)/6;
			result.x = p0.x*f0 + p1.x*f1 + p2.x*f2 + p3.x*f3;
			result.y = p0.y*f0 + p1.y*f1 + p2.y*f2 + p3.y*f3;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of resolveUCBS()<br>
		resolveUCBS() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBS(const TVec3<T0>& p0, const TVec3<T1>& p1, const TVec3<T2>& p2, const TVec3<T3>& p3, T4 t, TVec3<T5>& result)throw()
		{
			T5	//i = 1-t,
					f0 = (-t*t*t+3*t*t-3*t+1)/6,
					f1 = (3*t*t*t-6*t*t+4)/6,
					f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
					f3 = (t*t*t)/6;
			result.x = p0.x*f0 + p1.x*f1 + p2.x*f2 + p3.x*f3;
			result.y = p0.y*f0 + p1.y*f1 + p2.y*f2 + p3.y*f3;
			result.z = p0.z*f0 + p1.z*f1 + p2.z*f2 + p3.z*f3;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of resolveUCBS()<br>
		resolveUCBS() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBS(const TVec4<T0>& p0, const TVec4<T1>& p1, const TVec4<T2>& p2, const TVec4<T3>& p3, T4 t, TVec4<T5>& result)throw()
		{
			T5	//i = 1-t,
					f0 = (-t*t*t+3*t*t-3*t+1)/6,
					f1 = (3*t*t*t-6*t*t+4)/6,
					f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
					f3 = (t*t*t)/6;
			result.x = p0.x*f0 + p1.x*f1 + p2.x*f2 + p3.x*f3;
			result.y = p0.y*f0 + p1.y*f1 + p2.y*f2 + p3.y*f3;
			result.z = p0.z*f0 + p1.z*f1 + p2.z*f2 + p3.z*f3;
			result.w = p0.w*f0 + p1.w*f1 + p2.w*f2 + p3.w*f3;
		}

	//now implementing template definition 'void resolveUCBSaxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of resolveUCBSaxis()<br>
		resolveUCBSaxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBSaxis(const TVec2<T0>& p0, const TVec2<T1>& p1, const TVec2<T2>& p2, const TVec2<T3>& p3, T4 t, TVec2<T5>& result)throw()
		{
			T5	//i = 1-t,
					f0 = (t*t-2*t+1)/2,
					f1 = (-2*t*t+2*t+1)/2,
					f2 = (t*t)/2;
			result.x = (p1.x-p0.x)*f0 + (p2.x-p1.x)*f1 + (p3.x-p2.x)*f2;
			result.y = (p1.y-p0.y)*f0 + (p2.y-p1.y)*f1 + (p3.y-p2.y)*f2;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of resolveUCBSaxis()<br>
		resolveUCBSaxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBSaxis(const TVec3<T0>& p0, const TVec3<T1>& p1, const TVec3<T2>& p2, const TVec3<T3>& p3, T4 t, TVec3<T5>& result)throw()
		{
			T5	//i = 1-t,
					f0 = (t*t-2*t+1)/2,
					f1 = (-2*t*t+2*t+1)/2,
					f2 = (t*t)/2;
			result.x = (p1.x-p0.x)*f0 + (p2.x-p1.x)*f1 + (p3.x-p2.x)*f2;
			result.y = (p1.y-p0.y)*f0 + (p2.y-p1.y)*f1 + (p3.y-p2.y)*f2;
			result.z = (p1.z-p0.z)*f0 + (p2.z-p1.z)*f1 + (p3.z-p2.z)*f2;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of resolveUCBSaxis()<br>
		resolveUCBSaxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveUCBSaxis(const TVec4<T0>& p0, const TVec4<T1>& p1, const TVec4<T2>& p2, const TVec4<T3>& p3, T4 t, TVec4<T5>& result)throw()
		{
			T5	//i = 1-t,
					f0 = (t*t-2*t+1)/2,
					f1 = (-2*t*t+2*t+1)/2,
					f2 = (t*t)/2;
			result.x = (p1.x-p0.x)*f0 + (p2.x-p1.x)*f1 + (p3.x-p2.x)*f2;
			result.y = (p1.y-p0.y)*f0 + (p2.y-p1.y)*f1 + (p3.y-p2.y)*f2;
			result.z = (p1.z-p0.z)*f0 + (p2.z-p1.z)*f1 + (p3.z-p2.z)*f2;
			result.w = (p1.w-p0.w)*f0 + (p2.w-p1.w)*f1 + (p3.w-p2.w)*f2;
		}

	//now implementing template definition 'void resolveBezierCurvePoint (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of resolveBezierCurvePoint()<br>
		resolveBezierCurvePoint() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurvePoint(const TVec2<T0>& p0, const TVec2<T1>& p1, const TVec2<T2>& p2, const TVec2<T3>& p3, T4 t, TVec2<T5>& result)throw()
		{
			T5	i = 1-t,
					f3 = t*t*t,
					f2 = t*t*i*3,
					f1 = t*i*i*3,
					f0 = i*i*i;
			result.x = p0.x*f0 + p1.x*f1 + p2.x*f2 + p3.x*f3;
			result.y = p0.y*f0 + p1.y*f1 + p2.y*f2 + p3.y*f3;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of resolveBezierCurvePoint()<br>
		resolveBezierCurvePoint() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurvePoint(const TVec3<T0>& p0, const TVec3<T1>& p1, const TVec3<T2>& p2, const TVec3<T3>& p3, T4 t, TVec3<T5>& result)throw()
		{
			T5	i = 1-t,
					f3 = t*t*t,
					f2 = t*t*i*3,
					f1 = t*i*i*3,
					f0 = i*i*i;
			result.x = p0.x*f0 + p1.x*f1 + p2.x*f2 + p3.x*f3;
			result.y = p0.y*f0 + p1.y*f1 + p2.y*f2 + p3.y*f3;
			result.z = p0.z*f0 + p1.z*f1 + p2.z*f2 + p3.z*f3;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of resolveBezierCurvePoint()<br>
		resolveBezierCurvePoint() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurvePoint(const TVec4<T0>& p0, const TVec4<T1>& p1, const TVec4<T2>& p2, const TVec4<T3>& p3, T4 t, TVec4<T5>& result)throw()
		{
			T5	i = 1-t,
					f3 = t*t*t,
					f2 = t*t*i*3,
					f1 = t*i*i*3,
					f0 = i*i*i;
			result.x = p0.x*f0 + p1.x*f1 + p2.x*f2 + p3.x*f3;
			result.y = p0.y*f0 + p1.y*f1 + p2.y*f2 + p3.y*f3;
			result.z = p0.z*f0 + p1.z*f1 + p2.z*f2 + p3.z*f3;
			result.w = p0.w*f0 + p1.w*f1 + p2.w*f2 + p3.w*f3;
		}

	//now implementing template definition 'void resolveBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of resolveBezierCurveAxis()<br>
		resolveBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurveAxis(const TVec2<T0>& p0, const TVec2<T1>& p1, const TVec2<T2>& p2, const TVec2<T3>& p3, T4 t, TVec2<T5>& result)throw()
		{
			T5	i = 1-t,
					f2 = t*t,
					f1 = t*i*2,
					f0 = i*i;
			result.x = (p1.x - p0.x)*f0 + (p2.x - p1.x)*f1 + (p3.x - p2.x)*f2;
			result.y = (p1.y - p0.y)*f0 + (p2.y - p1.y)*f1 + (p3.y - p2.y)*f2;
			
			//block inlining void normalize0 (2..4) (<[*] vector>) direct= for dimensions=2, assembly_mode='Objects', parameters={result}...
			{
				T5 len = (result.x*result.x + result.y*result.y);
				if (isnan(len) || len <= TypeInfo<T5>::error)
				{
					result.x = 1;
					result.y = 0;
					return;
				}
				len = vsqrt(len);
				result.x /= len;
				result.y /= len;
			};
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of resolveBezierCurveAxis()<br>
		resolveBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurveAxis(const TVec3<T0>& p0, const TVec3<T1>& p1, const TVec3<T2>& p2, const TVec3<T3>& p3, T4 t, TVec3<T5>& result)throw()
		{
			T5	i = 1-t,
					f2 = t*t,
					f1 = t*i*2,
					f0 = i*i;
			result.x = (p1.x - p0.x)*f0 + (p2.x - p1.x)*f1 + (p3.x - p2.x)*f2;
			result.y = (p1.y - p0.y)*f0 + (p2.y - p1.y)*f1 + (p3.y - p2.y)*f2;
			result.z = (p1.z - p0.z)*f0 + (p2.z - p1.z)*f1 + (p3.z - p2.z)*f2;
			
			//block inlining void normalize0 (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='Objects', parameters={result}...
			{
				T5 len = (result.x*result.x + result.y*result.y + result.z*result.z);
				if (isnan(len) || len <= TypeInfo<T5>::error)
				{
					result.x = 1;
					result.y = 0;
					result.z = 0;
					return;
				}
				len = vsqrt(len);
				result.x /= len;
				result.y /= len;
				result.z /= len;
			};
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of resolveBezierCurveAxis()<br>
		resolveBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
		inline	void	__fastcall	resolveBezierCurveAxis(const TVec4<T0>& p0, const TVec4<T1>& p1, const TVec4<T2>& p2, const TVec4<T3>& p3, T4 t, TVec4<T5>& result)throw()
		{
			T5	i = 1-t,
					f2 = t*t,
					f1 = t*i*2,
					f0 = i*i;
			result.x = (p1.x - p0.x)*f0 + (p2.x - p1.x)*f1 + (p3.x - p2.x)*f2;
			result.y = (p1.y - p0.y)*f0 + (p2.y - p1.y)*f1 + (p3.y - p2.y)*f2;
			result.z = (p1.z - p0.z)*f0 + (p2.z - p1.z)*f1 + (p3.z - p2.z)*f2;
			result.w = (p1.w - p0.w)*f0 + (p2.w - p1.w)*f1 + (p3.w - p2.w)*f2;
			
			//block inlining void normalize0 (2..4) (<[*] vector>) direct= for dimensions=4, assembly_mode='Objects', parameters={result}...
			{
				T5 len = (result.x*result.x + result.y*result.y + result.z*result.z + result.w*result.w);
				if (isnan(len) || len <= TypeInfo<T5>::error)
				{
					result.x = 1;
					result.y = 0;
					result.z = 0;
					result.w = 0;
					return;
				}
				len = vsqrt(len);
				result.x /= len;
				result.y /= len;
				result.z /= len;
				result.w /= len;
			};
		}

	//now implementing template definition 'void SplitBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] out0>, <[*] out1>, <[*] out2>, <[*] out3>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of SplitBezierCurveAxis()<br>
		SplitBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] out0 
		@param[out] out1 
		@param[out] out2 
		@param[out] out3 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
		inline	void	__fastcall	SplitBezierCurveAxis(const TVec2<T0>& p0, const TVec2<T1>& p1, const TVec2<T2>& p2, const TVec2<T3>& p3, T4 t, TVec2<T5>& out0, TVec2<T6>& out1, TVec2<T7>& out2, TVec2<T8>& out3)throw()
		{
			{
				T5 x01 = (p1.x - p0.x)*t + p0.x;
				T5 x12 = (p2.x - p1.x)*t + p1.x;
				T5 x23 = (p3.x - p2.x)*t + p2.x;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.x = p0.x;
				out1.x = x01;
				out2.x = x012;
				out3.x = x0123;
			}
			{
				T5 x01 = (p1.y - p0.y)*t + p0.y;
				T5 x12 = (p2.y - p1.y)*t + p1.y;
				T5 x23 = (p3.y - p2.y)*t + p2.y;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.y = p0.y;
				out1.y = x01;
				out2.y = x012;
				out3.y = x0123;
			}
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of SplitBezierCurveAxis()<br>
		SplitBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] out0 
		@param[out] out1 
		@param[out] out2 
		@param[out] out3 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
		inline	void	__fastcall	SplitBezierCurveAxis(const TVec3<T0>& p0, const TVec3<T1>& p1, const TVec3<T2>& p2, const TVec3<T3>& p3, T4 t, TVec3<T5>& out0, TVec3<T6>& out1, TVec3<T7>& out2, TVec3<T8>& out3)throw()
		{
			{
				T5 x01 = (p1.x - p0.x)*t + p0.x;
				T5 x12 = (p2.x - p1.x)*t + p1.x;
				T5 x23 = (p3.x - p2.x)*t + p2.x;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.x = p0.x;
				out1.x = x01;
				out2.x = x012;
				out3.x = x0123;
			}
			{
				T5 x01 = (p1.y - p0.y)*t + p0.y;
				T5 x12 = (p2.y - p1.y)*t + p1.y;
				T5 x23 = (p3.y - p2.y)*t + p2.y;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.y = p0.y;
				out1.y = x01;
				out2.y = x012;
				out3.y = x0123;
			}
			{
				T5 x01 = (p1.z - p0.z)*t + p0.z;
				T5 x12 = (p2.z - p1.z)*t + p1.z;
				T5 x23 = (p3.z - p2.z)*t + p2.z;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.z = p0.z;
				out1.z = x01;
				out2.z = x012;
				out3.z = x0123;
			}
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of SplitBezierCurveAxis()<br>
		SplitBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] p0 
		@param[in] p1 
		@param[in] p2 
		@param[in] p3 
		@param[in] t 
		@param[out] out0 
		@param[out] out1 
		@param[out] out2 
		@param[out] out3 
	*/
	template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
		inline	void	__fastcall	SplitBezierCurveAxis(const TVec4<T0>& p0, const TVec4<T1>& p1, const TVec4<T2>& p2, const TVec4<T3>& p3, T4 t, TVec4<T5>& out0, TVec4<T6>& out1, TVec4<T7>& out2, TVec4<T8>& out3)throw()
		{
			{
				T5 x01 = (p1.x - p0.x)*t + p0.x;
				T5 x12 = (p2.x - p1.x)*t + p1.x;
				T5 x23 = (p3.x - p2.x)*t + p2.x;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.x = p0.x;
				out1.x = x01;
				out2.x = x012;
				out3.x = x0123;
			}
			{
				T5 x01 = (p1.y - p0.y)*t + p0.y;
				T5 x12 = (p2.y - p1.y)*t + p1.y;
				T5 x23 = (p3.y - p2.y)*t + p2.y;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.y = p0.y;
				out1.y = x01;
				out2.y = x012;
				out3.y = x0123;
			}
			{
				T5 x01 = (p1.z - p0.z)*t + p0.z;
				T5 x12 = (p2.z - p1.z)*t + p1.z;
				T5 x23 = (p3.z - p2.z)*t + p2.z;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.z = p0.z;
				out1.z = x01;
				out2.z = x012;
				out3.z = x0123;
			}
			{
				T5 x01 = (p1.w - p0.w)*t + p0.w;
				T5 x12 = (p2.w - p1.w)*t + p1.w;
				T5 x23 = (p3.w - p2.w)*t + p2.w;
				T5 x012 = (x12 - x01) * t + x01;
				T5 x123 = (x23 - x12) * t + x12;
				T5 x0123 = (x123 - x012) * t + x012;
				out0.w = p0.w;
				out1.w = x01;
				out2.w = x012;
				out3.w = x0123;
			}
		}

	//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>, <tolerance>) direct=1'
	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		2 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec2<T0>& v0, const TVec2<T1>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		3 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec3<T0>& v0, const TVec3<T1>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		4 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec4<T0>& v0, const TVec4<T1>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		5 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,5>& v0, const TVec<T1,5>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		6 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,6>& v0, const TVec<T1,6>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		7 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,7>& v0, const TVec<T1,7>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		8 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,8>& v0, const TVec<T1,8>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		9 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,9>& v0, const TVec<T1,9>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		10 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,10>& v0, const TVec<T1,10>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[9] - v1.v[9];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		11 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,11>& v0, const TVec<T1,11>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[9] - v1.v[9];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[10] - v1.v[10];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		12 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,12>& v0, const TVec<T1,12>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[9] - v1.v[9];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[10] - v1.v[10];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[11] - v1.v[11];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		13 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,13>& v0, const TVec<T1,13>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[9] - v1.v[9];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[10] - v1.v[10];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[11] - v1.v[11];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[12] - v1.v[12];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		14 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,14>& v0, const TVec<T1,14>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[9] - v1.v[9];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[10] - v1.v[10];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[11] - v1.v[11];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[12] - v1.v[12];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[13] - v1.v[13];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		15 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,15>& v0, const TVec<T1,15>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[9] - v1.v[9];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[10] - v1.v[10];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[11] - v1.v[11];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[12] - v1.v[12];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[13] - v1.v[13];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[14] - v1.v[14];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		16 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v0 
		@param[in] v1 
		@param[in] tolerance 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1, typename T2>
		inline	char	__fastcall	compare(const TVec<T0,16>& v0, const TVec<T1,16>& v1, T2 tolerance)throw()
		{
			{
				T0 delta = v0.x - v1.x;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.y - v1.y;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.z - v1.z;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.w - v1.w;
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[4] - v1.v[4];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[5] - v1.v[5];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[6] - v1.v[6];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[7] - v1.v[7];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[8] - v1.v[8];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[9] - v1.v[9];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[10] - v1.v[10];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[11] - v1.v[11];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[12] - v1.v[12];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[13] - v1.v[13];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[14] - v1.v[14];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			{
				T0 delta = v0.v[15] - v1.v[15];
				if (delta < -tolerance)
					return -1;
				if (delta > tolerance)
					return 1;
			}
			return 0;
		}

	//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>) direct='
	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		2 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec2<T0>& v0, const TVec2<T1>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		3 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec3<T0>& v0, const TVec3<T1>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		4 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec4<T0>& v0, const TVec4<T1>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		5 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,5>& v0, const TVec<T1,5>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		6 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,6>& v0, const TVec<T1,6>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		7 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,7>& v0, const TVec<T1,7>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		8 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,8>& v0, const TVec<T1,8>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		9 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,9>& v0, const TVec<T1,9>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		10 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,10>& v0, const TVec<T1,10>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		11 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,11>& v0, const TVec<T1,11>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		12 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,12>& v0, const TVec<T1,12>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		13 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,13>& v0, const TVec<T1,13>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		14 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,14>& v0, const TVec<T1,14>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		15 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,15>& v0, const TVec<T1,15>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	/**
		@brief Compares two vectors for lexicographic order<br>
		<br>
		16 dimensional specialized version of compare()<br>
		compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v0 
		@param[in] v1 
		@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
	*/
	template <typename T0, typename T1>
		inline	char	__fastcall	compare(const TVec<T0,16>& v0, const TVec<T1,16>& v1)throw()
		{
			return compare(v0,v1,(TypeInfo<T0>::error));
		}

	//now implementing template definition 'bool equal (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of equal()<br>
		equal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	equal(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return v.x == w.x && v.y == w.y;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of equal()<br>
		equal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	equal(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return v.x == w.x && v.y == w.y && v.z == w.z;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of equal()<br>
		equal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	equal(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return v.x == w.x && v.y == w.y && v.z == w.z && v.w == w.w;
		}

	//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>, <tolerance>) direct='
	/**
		@brief Determines whether @a v and @a w are similar (within the specified tolerance proximity)<br>
		<br>
		2 dimensional specialized version of similar()<br>
		similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] tolerance maximum distance between <paramref>v</paramref> and <paramref>w</paramref> to still be considered similar
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar in accordance to the specified distance, false otherwise 
	*/
	template <typename T0, typename T1, typename T2>
		inline	bool	__fastcall	similar(const TVec2<T0>& v, const TVec2<T1>& w, T2 tolerance)throw()
		{
			return sqr(v.x - w.x) + sqr(v.y - w.y) < tolerance*tolerance;
		}

	/**
		@brief Determines whether @a v and @a w are similar (within the specified tolerance proximity)<br>
		<br>
		3 dimensional specialized version of similar()<br>
		similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] tolerance maximum distance between <paramref>v</paramref> and <paramref>w</paramref> to still be considered similar
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar in accordance to the specified distance, false otherwise 
	*/
	template <typename T0, typename T1, typename T2>
		inline	bool	__fastcall	similar(const TVec3<T0>& v, const TVec3<T1>& w, T2 tolerance)throw()
		{
			return sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z) < tolerance*tolerance;
		}

	/**
		@brief Determines whether @a v and @a w are similar (within the specified tolerance proximity)<br>
		<br>
		4 dimensional specialized version of similar()<br>
		similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] v 
		@param[in] w 
		@param[in] tolerance maximum distance between <paramref>v</paramref> and <paramref>w</paramref> to still be considered similar
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar in accordance to the specified distance, false otherwise 
	*/
	template <typename T0, typename T1, typename T2>
		inline	bool	__fastcall	similar(const TVec4<T0>& v, const TVec4<T1>& w, T2 tolerance)throw()
		{
			return sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z) + sqr(v.w - w.w) < tolerance*tolerance;
		}

	//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines whether @a v and @a w are similar (within a type-specific tolerance proximity)<br>
		<br>
		2 dimensional specialized version of similar()<br>
		similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar, false otherwise 
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	similar(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return (sqr(v.x - w.x) + sqr(v.y - w.y) < (TypeInfo<T0>::error)*(TypeInfo<T0>::error));
		}

	/**
		@brief Determines whether @a v and @a w are similar (within a type-specific tolerance proximity)<br>
		<br>
		3 dimensional specialized version of similar()<br>
		similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar, false otherwise 
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	similar(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return (sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z) < (TypeInfo<T0>::error)*(TypeInfo<T0>::error));
		}

	/**
		@brief Determines whether @a v and @a w are similar (within a type-specific tolerance proximity)<br>
		<br>
		4 dimensional specialized version of similar()<br>
		similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar, false otherwise 
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	similar(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return (sqr(v.x - w.x) + sqr(v.y - w.y) + sqr(v.z - w.z) + sqr(v.w - w.w) < (TypeInfo<T0>::error)*(TypeInfo<T0>::error));
		}

	//now implementing template definition 'T0 maxAxisDistance (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines the greatest difference along any axis between @a v and @a w<br>
		<br>
		2 dimensional specialized version of maxAxisDistance()<br>
		maxAxisDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return Greatest axial distance between v and w 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	maxAxisDistance(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return vmax(vabs(v.x-w.x), vabs(v.y-w.y));
		}

	/**
		@brief Determines the greatest difference along any axis between @a v and @a w<br>
		<br>
		3 dimensional specialized version of maxAxisDistance()<br>
		maxAxisDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return Greatest axial distance between v and w 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	maxAxisDistance(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return vmax(vabs(v.x-w.x), vmax(vabs(v.y-w.y), vabs(v.z-w.z)));
		}

	/**
		@brief Determines the greatest difference along any axis between @a v and @a w<br>
		<br>
		4 dimensional specialized version of maxAxisDistance()<br>
		maxAxisDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return Greatest axial distance between v and w 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	maxAxisDistance(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return vmax(vmax(vabs(v.x-w.x), vabs(v.y-w.y)), vmax(vabs(v.z-w.z), vabs(v.w-w.w)));
		}

	//now implementing template definition 'T0 planePointDistanceN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
	/**
		@brief Determines the absolute distance between a plane and a point. The specified normal is assumed to be of length 1<br>
		<br>
		2 dimensional specialized version of planePointDistanceN()<br>
		planePointDistanceN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; must be normalized
		@param[in] p point to determine the distance of
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistanceN(const TVec2<T0>& base, const TVec2<T1>& normal, const TVec2<T2>& p)throw()
		{
			return ((p.x*normal.x + p.y*normal.y) - (base.x*normal.x + base.y*normal.y));
		}

	/**
		@brief Determines the absolute distance between a plane and a point. The specified normal is assumed to be of length 1<br>
		<br>
		3 dimensional specialized version of planePointDistanceN()<br>
		planePointDistanceN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; must be normalized
		@param[in] p point to determine the distance of
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistanceN(const TVec3<T0>& base, const TVec3<T1>& normal, const TVec3<T2>& p)throw()
		{
			return ((p.x*normal.x + p.y*normal.y + p.z*normal.z) - (base.x*normal.x + base.y*normal.y + base.z*normal.z));
		}

	/**
		@brief Determines the absolute distance between a plane and a point. The specified normal is assumed to be of length 1<br>
		<br>
		4 dimensional specialized version of planePointDistanceN()<br>
		planePointDistanceN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; must be normalized
		@param[in] p point to determine the distance of
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistanceN(const TVec4<T0>& base, const TVec4<T1>& normal, const TVec4<T2>& p)throw()
		{
			return ((p.x*normal.x + p.y*normal.y + p.z*normal.z + p.w*normal.w) - (base.x*normal.x + base.y*normal.y + base.z*normal.z + base.w*normal.w));
		}

	//now implementing template definition 'T0 planePointDistance (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
	/**
		@brief Determines the absolute distance between a plane and a point<br>
		<br>
		2 dimensional specialized version of planePointDistance()<br>
		planePointDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; may be of any length greater 0
		@param[in] p point to determine the distance of
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistance(const TVec2<T0>& base, const TVec2<T1>& normal, const TVec2<T2>& p)throw()
		{
			return ((p.x*normal.x + p.y*normal.y) - (base.x*normal.x + base.y*normal.y)) / (vsqrt((normal.x*normal.x + normal.y*normal.y)));
		}

	/**
		@brief Determines the absolute distance between a plane and a point<br>
		<br>
		3 dimensional specialized version of planePointDistance()<br>
		planePointDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; may be of any length greater 0
		@param[in] p point to determine the distance of
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistance(const TVec3<T0>& base, const TVec3<T1>& normal, const TVec3<T2>& p)throw()
		{
			return ((p.x*normal.x + p.y*normal.y + p.z*normal.z) - (base.x*normal.x + base.y*normal.y + base.z*normal.z)) / (vsqrt((normal.x*normal.x + normal.y*normal.y + normal.z*normal.z)));
		}

	/**
		@brief Determines the absolute distance between a plane and a point<br>
		<br>
		4 dimensional specialized version of planePointDistance()<br>
		planePointDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] base plane base point
		@param[in] normal plane normal; may be of any length greater 0
		@param[in] p point to determine the distance of
		@return absolute distance 
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	planePointDistance(const TVec4<T0>& base, const TVec4<T1>& normal, const TVec4<T2>& p)throw()
		{
			return ((p.x*normal.x + p.y*normal.y + p.z*normal.z + p.w*normal.w) - (base.x*normal.x + base.y*normal.y + base.z*normal.z + base.w*normal.w)) / (vsqrt((normal.x*normal.x + normal.y*normal.y + normal.z*normal.z + normal.w*normal.w)));
		}

	//now implementing template definition 'void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1'
	/**
		@brief Scales the distance between @a center and @a current by the specified @a factor<br>
		<br>
		2 dimensional specialized version of scale()<br>
		scale() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] factor 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scale(const TVec2<T0>& center, T1 factor, TVec2<T2>& current)throw()
		{
			current.x = center.x+(current.x-center.x)*factor;
			current.y = center.y+(current.y-center.y)*factor;
		}

	/**
		@brief Scales the distance between @a center and @a current by the specified @a factor<br>
		<br>
		3 dimensional specialized version of scale()<br>
		scale() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] factor 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scale(const TVec3<T0>& center, T1 factor, TVec3<T2>& current)throw()
		{
			current.x = center.x+(current.x-center.x)*factor;
			current.y = center.y+(current.y-center.y)*factor;
			current.z = center.z+(current.z-center.z)*factor;
		}

	/**
		@brief Scales the distance between @a center and @a current by the specified @a factor<br>
		<br>
		4 dimensional specialized version of scale()<br>
		scale() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] factor 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scale(const TVec4<T0>& center, T1 factor, TVec4<T2>& current)throw()
		{
			current.x = center.x+(current.x-center.x)*factor;
			current.y = center.y+(current.y-center.y)*factor;
			current.z = center.z+(current.z-center.z)*factor;
			current.w = center.w+(current.w-center.w)*factor;
		}

	//now implementing template definition 'void scaleAbsolute (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Behavior is undefined if @a center and @a current are identical.<br>
		<br>
		2 dimensional specialized version of scaleAbsolute()<br>
		scaleAbsolute() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsolute(const TVec2<T0>& center, T1 distance, TVec2<T2>& current)throw()
		{
			T2 len = (vsqrt( (sqr(current.x - center.x) + sqr(current.y - center.y)) ));
			
			//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={center, (distance/len), current}...
			{
				current.x = center.x+(current.x-center.x)*(distance/len);
				current.y = center.y+(current.y-center.y)*(distance/len);
			};
		}

	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Behavior is undefined if @a center and @a current are identical.<br>
		<br>
		3 dimensional specialized version of scaleAbsolute()<br>
		scaleAbsolute() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsolute(const TVec3<T0>& center, T1 distance, TVec3<T2>& current)throw()
		{
			T2 len = (vsqrt( (sqr(current.x - center.x) + sqr(current.y - center.y) + sqr(current.z - center.z)) ));
			
			//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={center, (distance/len), current}...
			{
				current.x = center.x+(current.x-center.x)*(distance/len);
				current.y = center.y+(current.y-center.y)*(distance/len);
				current.z = center.z+(current.z-center.z)*(distance/len);
			};
		}

	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Behavior is undefined if @a center and @a current are identical.<br>
		<br>
		4 dimensional specialized version of scaleAbsolute()<br>
		scaleAbsolute() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsolute(const TVec4<T0>& center, T1 distance, TVec4<T2>& current)throw()
		{
			T2 len = (vsqrt( (sqr(current.x - center.x) + sqr(current.y - center.y) + sqr(current.z - center.z) + sqr(current.w - center.w)) ));
			
			//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={center, (distance/len), current}...
			{
				current.x = center.x+(current.x-center.x)*(distance/len);
				current.y = center.y+(current.y-center.y)*(distance/len);
				current.z = center.z+(current.z-center.z)*(distance/len);
				current.w = center.w+(current.w-center.w)*(distance/len);
			};
		}

	//now implementing template definition 'void scaleAbsolute0 (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Also checks if @a center and @a current are identical and reacts appropriately.<br>
		<br>
		2 dimensional specialized version of scaleAbsolute0()<br>
		scaleAbsolute0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsolute0(const TVec2<T0>& center, T1 distance, TVec2<T2>& current)throw()
		{
			T2 len = (vsqrt( (sqr(current.x - center.x) + sqr(current.y - center.y)) ));
			if (len > TypeInfo<T2>::error)
			{
				
				//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={center, (distance/len), current}...
				{
					current.x = center.x+(current.x-center.x)*(distance/len);
					current.y = center.y+(current.y-center.y)*(distance/len);
				};
			}
			else
				current[0] = len;
		}

	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Also checks if @a center and @a current are identical and reacts appropriately.<br>
		<br>
		3 dimensional specialized version of scaleAbsolute0()<br>
		scaleAbsolute0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsolute0(const TVec3<T0>& center, T1 distance, TVec3<T2>& current)throw()
		{
			T2 len = (vsqrt( (sqr(current.x - center.x) + sqr(current.y - center.y) + sqr(current.z - center.z)) ));
			if (len > TypeInfo<T2>::error)
			{
				
				//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={center, (distance/len), current}...
				{
					current.x = center.x+(current.x-center.x)*(distance/len);
					current.y = center.y+(current.y-center.y)*(distance/len);
					current.z = center.z+(current.z-center.z)*(distance/len);
				};
			}
			else
				current[0] = len;
		}

	/**
		@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Also checks if @a center and @a current are identical and reacts appropriately.<br>
		<br>
		4 dimensional specialized version of scaleAbsolute0()<br>
		scaleAbsolute0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] center 
		@param[in] distance 
		@param[out] current 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	scaleAbsolute0(const TVec4<T0>& center, T1 distance, TVec4<T2>& current)throw()
		{
			T2 len = (vsqrt( (sqr(current.x - center.x) + sqr(current.y - center.y) + sqr(current.z - center.z) + sqr(current.w - center.w)) ));
			if (len > TypeInfo<T2>::error)
			{
				
				//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={center, (distance/len), current}...
				{
					current.x = center.x+(current.x-center.x)*(distance/len);
					current.y = center.y+(current.y-center.y)*(distance/len);
					current.z = center.z+(current.z-center.z)*(distance/len);
					current.w = center.w+(current.w-center.w)*(distance/len);
				};
			}
			else
				current[0] = len;
		}

	//now implementing template definition 'void normalize (2..4) (<[*] vector>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of normalize()<br>
		normalize() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] vector 
	*/
	template <typename T0>
		inline	void	__fastcall	normalize(TVec2<T0>& vector)throw()
		{
			T0 len = vsqrt((vector.x*vector.x + vector.y*vector.y));
			vector.x /= len;
			vector.y /= len;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of normalize()<br>
		normalize() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] vector 
	*/
	template <typename T0>
		inline	void	__fastcall	normalize(TVec3<T0>& vector)throw()
		{
			T0 len = vsqrt((vector.x*vector.x + vector.y*vector.y + vector.z*vector.z));
			vector.x /= len;
			vector.y /= len;
			vector.z /= len;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of normalize()<br>
		normalize() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] vector 
	*/
	template <typename T0>
		inline	void	__fastcall	normalize(TVec4<T0>& vector)throw()
		{
			T0 len = vsqrt((vector.x*vector.x + vector.y*vector.y + vector.z*vector.z + vector.w*vector.w));
			vector.x /= len;
			vector.y /= len;
			vector.z /= len;
			vector.w /= len;
		}

	//now implementing template definition 'void normalize0 (2..4) (<[*] vector>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of normalize0()<br>
		normalize0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] vector 
	*/
	template <typename T0>
		inline	void	__fastcall	normalize0(TVec2<T0>& vector)throw()
		{
			T0 len = (vector.x*vector.x + vector.y*vector.y);
			if (isnan(len) || len <= TypeInfo<T0>::error)
			{
				vector.x = 1;
				vector.y = 0;
				return;
			}
			len = vsqrt(len);
			vector.x /= len;
			vector.y /= len;
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of normalize0()<br>
		normalize0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] vector 
	*/
	template <typename T0>
		inline	void	__fastcall	normalize0(TVec3<T0>& vector)throw()
		{
			T0 len = (vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
			if (isnan(len) || len <= TypeInfo<T0>::error)
			{
				vector.x = 1;
				vector.y = 0;
				vector.z = 0;
				return;
			}
			len = vsqrt(len);
			vector.x /= len;
			vector.y /= len;
			vector.z /= len;
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of normalize0()<br>
		normalize0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] vector 
	*/
	template <typename T0>
		inline	void	__fastcall	normalize0(TVec4<T0>& vector)throw()
		{
			T0 len = (vector.x*vector.x + vector.y*vector.y + vector.z*vector.z + vector.w*vector.w);
			if (isnan(len) || len <= TypeInfo<T0>::error)
			{
				vector.x = 1;
				vector.y = 0;
				vector.z = 0;
				vector.w = 0;
				return;
			}
			len = vsqrt(len);
			vector.x /= len;
			vector.y /= len;
			vector.z /= len;
			vector.w /= len;
		}

	//now implementing template definition 'void abs (2..4) (<[*] v>) direct=1'
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of abs()<br>
		abs() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
	*/
	template <typename T0>
		inline	void	__fastcall	abs(TVec2<T0>& v)throw()
		{
			v.x = vabs(v.x);
			v.y = vabs(v.y);
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of abs()<br>
		abs() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
	*/
	template <typename T0>
		inline	void	__fastcall	abs(TVec3<T0>& v)throw()
		{
			v.x = vabs(v.x);
			v.y = vabs(v.y);
			v.z = vabs(v.z);
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of abs()<br>
		abs() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
	*/
	template <typename T0>
		inline	void	__fastcall	abs(TVec4<T0>& v)throw()
		{
			v.x = vabs(v.x);
			v.y = vabs(v.y);
			v.z = vabs(v.z);
			v.w = vabs(v.w);
		}

	//now implementing template definition 'void setLen (2..4) (<[*] current>, <length>) direct='
	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		2 dimensional specialized version of setLen()<br>
		setLen() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLen(TVec2<T0>& current, T1 length)throw()
		{
			T0 len = vsqrt( (current.x*current.x + current.y*current.y) );
			T0 fc = T0(length)/len;
			
			//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={current, fc}...
			{
				current.x *= fc;
				current.y *= fc;
			};
		}

	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		3 dimensional specialized version of setLen()<br>
		setLen() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLen(TVec3<T0>& current, T1 length)throw()
		{
			T0 len = vsqrt( (current.x*current.x + current.y*current.y + current.z*current.z) );
			T0 fc = T0(length)/len;
			
			//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={current, fc}...
			{
				current.x *= fc;
				current.y *= fc;
				current.z *= fc;
			};
		}

	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		4 dimensional specialized version of setLen()<br>
		setLen() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLen(TVec4<T0>& current, T1 length)throw()
		{
			T0 len = vsqrt( (current.x*current.x + current.y*current.y + current.z*current.z + current.w*current.w) );
			T0 fc = T0(length)/len;
			
			//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={current, fc}...
			{
				current.x *= fc;
				current.y *= fc;
				current.z *= fc;
				current.w *= fc;
			};
		}

	//now implementing template definition 'void setLen0 (2..4) (<[*] current>, <length>) direct='
	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		2 dimensional specialized version of setLen0()<br>
		setLen0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLen0(TVec2<T0>& current, T1 length)throw()
		{
			T0 len = vsqrt( (current.x*current.x + current.y*current.y) );
			if (vabs(len) > TypeInfo<T0>::error)
			{
				T0 fc = T0(length)/len;
				
				//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={current, fc}...
				{
					current.x *= fc;
					current.y *= fc;
				};
			}
			else
				current.x = len;
		}

	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		3 dimensional specialized version of setLen0()<br>
		setLen0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLen0(TVec3<T0>& current, T1 length)throw()
		{
			T0 len = vsqrt( (current.x*current.x + current.y*current.y + current.z*current.z) );
			if (vabs(len) > TypeInfo<T0>::error)
			{
				T0 fc = T0(length)/len;
				
				//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={current, fc}...
				{
					current.x *= fc;
					current.y *= fc;
					current.z *= fc;
				};
			}
			else
				current.x = len;
		}

	/**
		@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
		<br>
		4 dimensional specialized version of setLen0()<br>
		setLen0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] current vector to normalize, must not be of length 0
		@param[in] length new length
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	setLen0(TVec4<T0>& current, T1 length)throw()
		{
			T0 len = vsqrt( (current.x*current.x + current.y*current.y + current.z*current.z + current.w*current.w) );
			if (vabs(len) > TypeInfo<T0>::error)
			{
				T0 fc = T0(length)/len;
				
				//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={current, fc}...
				{
					current.x *= fc;
					current.y *= fc;
					current.z *= fc;
					current.w *= fc;
				};
			}
			else
				current.x = len;
		}

	//now implementing template definition 'T0 angleOne (<x>, <y>) direct='
	/**
		@brief Determines the rotational angle of @a v. The returned angle is measured counter-clockwise starting at 0 for points along the positive x axis<br>
		<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] x 
		@param[in] y 
		@return Absolute angle of @v in the range [0,1]. The result is 0 for (0,0) 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	angleOne(T0 x, T1 y)throw()
		{
			if (!x && !y)
				return 0;
			T0 rs = (T0)vatan2(y,x)/(2*M_PI);
			if (rs < 0)
				rs = ((T0)1)+rs;
			return rs;
		}

	//now implementing template definition 'T0 angleOne (<const [2] v>) direct='
	/**
		@brief Determines the rotational angle of @a v<br>
		<br>
		angleOne() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return absolute angle of @v in the range [0,1] 
	*/
	template <typename T0>
		inline	T0	__fastcall	angleOne(const TVec2<T0>& v)throw()
		{
			return angleOne((v.x),(v.y));
		}

	//now implementing template definition 'T0 angle360 (<x>, <y>) direct='
	/**
		@brief Determines the rotational angle of @a v<br>
		<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] x 
		@param[in] y 
		@return absolute angle of @v in the range [0,360] 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	angle360(T0 x, T1 y)throw()
		{
			return angleOne(x,y) *360;
		}

	//now implementing template definition 'T0 angle360 (<const [2] v>) direct='
	/**
		@brief Determines the rotational angle of @a v<br>
		<br>
		angle360() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return absolute angle of @v in the range [0,360] 
	*/
	template <typename T0>
		inline	T0	__fastcall	angle360(const TVec2<T0>& v)throw()
		{
			return (angleOne((v.x),(v.y))) *360;
		}

	//now implementing template definition 'T0 angle2PI (<x>, <y>) direct='
	/**
		@brief Determines the rotational angle of @a v. The returned angle is measured counter-clockwise starting at 0 for points along the positive x axis<br>
		<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] x 
		@param[in] y 
		@return absolute angle of @v in the range [0,2*PI] 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	angle2PI(T0 x, T1 y)throw()
		{
			if (!x && !y)
				return 0;
			T0 rs = (T0)vatan2(y,x);
			if (rs < 0)
				rs = ((T0)M_PI*2)+rs;
			return rs;
		}

	//now implementing template definition 'T0 angle2PI (<const [2] v>) direct='
	/**
		@brief Determines the rotational angle of @a v<br>
		<br>
		angle2PI() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v vector to determine the angle of; not required to be normalized
		@return absolute angle of @v in the range [0,2*PI] 
	*/
	template <typename T0>
		inline	T0	__fastcall	angle2PI(const TVec2<T0>& v)throw()
		{
			return angle2PI((v.x),(v.y));
		}

	//now implementing template definition 'T0 angle (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines the angle between @a v and @a w<br>
		<br>
		2 dimensional specialized version of angle()<br>
		angle() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@return angle in the range [0,180] 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	angle(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return acos(Math::clamp((v.x*w.x + v.y*w.y)/((vsqrt((v.x*v.x + v.y*v.y)))*(vsqrt((w.x*w.x + w.y*w.y)))),-1,1))*180/M_PI;
		}

	/**
		@brief Determines the angle between @a v and @a w<br>
		<br>
		3 dimensional specialized version of angle()<br>
		angle() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@return angle in the range [0,180] 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	angle(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return acos(Math::clamp((v.x*w.x + v.y*w.y + v.z*w.z)/((vsqrt((v.x*v.x + v.y*v.y + v.z*v.z)))*(vsqrt((w.x*w.x + w.y*w.y + w.z*w.z)))),-1,1))*180/M_PI;
		}

	/**
		@brief Determines the angle between @a v and @a w<br>
		<br>
		4 dimensional specialized version of angle()<br>
		angle() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@return angle in the range [0,180] 
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	angle(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return acos(Math::clamp((v.x*w.x + v.y*w.y + v.z*w.z + v.w*w.w)/((vsqrt((v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w)))*(vsqrt((w.x*w.x + w.y*w.y + w.z*w.z + w.w*w.w)))),-1,1))*180/M_PI;
		}

	//now implementing template definition 'bool isNAN (2..4) (<const [*] v>) direct='
	/**
		@brief Tests if at least one component of @a v is NAN (Not A Number). Not applicable to integer types<br>
		<br>
		2 dimensional specialized version of isNAN()<br>
		isNAN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	isNAN(const TVec2<T0>& v)throw()
		{
			return isnan(v.x) || isnan(v.y);
		}

	/**
		@brief Tests if at least one component of @a v is NAN (Not A Number). Not applicable to integer types<br>
		<br>
		3 dimensional specialized version of isNAN()<br>
		isNAN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	isNAN(const TVec3<T0>& v)throw()
		{
			return isnan(v.x) || isnan(v.y) || isnan(v.z);
		}

	/**
		@brief Tests if at least one component of @a v is NAN (Not A Number). Not applicable to integer types<br>
		<br>
		4 dimensional specialized version of isNAN()<br>
		isNAN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	bool	__fastcall	isNAN(const TVec4<T0>& v)throw()
		{
			return isnan(v.x) || isnan(v.y) || isnan(v.z) || isnan(v.w);
		}

	//now implementing template definition 'T0 intensity (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Determines the light intensity (the cos of the angle) between @a v and @a w<br>
		<br>
		2 dimensional specialized version of intensity()<br>
		intensity() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	intensity(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return (v.x*w.x + v.y*w.y)/((vsqrt((v.x*v.x + v.y*v.y)))*(vsqrt((w.x*w.x + w.y*w.y))));
		}

	/**
		@brief Determines the light intensity (the cos of the angle) between @a v and @a w<br>
		<br>
		3 dimensional specialized version of intensity()<br>
		intensity() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	intensity(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return (v.x*w.x + v.y*w.y + v.z*w.z)/((vsqrt((v.x*v.x + v.y*v.y + v.z*v.z)))*(vsqrt((w.x*w.x + w.y*w.y + w.z*w.z))));
		}

	/**
		@brief Determines the light intensity (the cos of the angle) between @a v and @a w<br>
		<br>
		4 dimensional specialized version of intensity()<br>
		intensity() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v first vector; not required to be normalized, but must be non-0
		@param[in] w second vector; not required to be normalized, but must be non-0
		@return  
	*/
	template <typename T0, typename T1>
		inline	T0	__fastcall	intensity(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return (v.x*w.x + v.y*w.y + v.z*w.z + v.w*w.w)/((vsqrt((v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w)))*(vsqrt((w.x*w.x + w.y*w.y + w.z*w.z + w.w*w.w))));
		}

	//now implementing template definition 'void set (2..16) (<[*] v>, <value>) direct=1'
	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		2 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec2<T0>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		3 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec3<T0>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		4 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec4<T0>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		5 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,5>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		6 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,6>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		7 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,7>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		8 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,8>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		9 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,9>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		10 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,10>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
			v.v[9] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		11 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,11>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
			v.v[9] = value;
			v.v[10] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		12 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,12>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
			v.v[9] = value;
			v.v[10] = value;
			v.v[11] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		13 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,13>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
			v.v[9] = value;
			v.v[10] = value;
			v.v[11] = value;
			v.v[12] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		14 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,14>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
			v.v[9] = value;
			v.v[10] = value;
			v.v[11] = value;
			v.v[12] = value;
			v.v[13] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		15 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,15>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
			v.v[9] = value;
			v.v[10] = value;
			v.v[11] = value;
			v.v[12] = value;
			v.v[13] = value;
			v.v[14] = value;
		}

	/**
		@brief Sets all components of @a v to @a value<br>
		<br>
		16 dimensional specialized version of set()<br>
		set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] value 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	set(TVec<T0,16>& v, T1 value)throw()
		{
			v.x = value;
			v.y = value;
			v.z = value;
			v.w = value;
			v.v[4] = value;
			v.v[5] = value;
			v.v[6] = value;
			v.v[7] = value;
			v.v[8] = value;
			v.v[9] = value;
			v.v[10] = value;
			v.v[11] = value;
			v.v[12] = value;
			v.v[13] = value;
			v.v[14] = value;
			v.v[15] = value;
		}

	//now implementing template definition 'void clamp (2..4) (<[*] v>, <min>, <max>) direct=1'
	/**
		@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
		<br>
		2 dimensional specialized version of clamp()<br>
		clamp() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] min 
		@param[in] max 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	clamp(TVec2<T0>& v, T1 min, T2 max)throw()
		{
			v.x = clamped(v.x,min,max);
			v.y = clamped(v.y,min,max);
		}

	/**
		@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
		<br>
		3 dimensional specialized version of clamp()<br>
		clamp() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] min 
		@param[in] max 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	clamp(TVec3<T0>& v, T1 min, T2 max)throw()
		{
			v.x = clamped(v.x,min,max);
			v.y = clamped(v.y,min,max);
			v.z = clamped(v.z,min,max);
		}

	/**
		@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
		<br>
		4 dimensional specialized version of clamp()<br>
		clamp() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[out] v 
		@param[in] min 
		@param[in] max 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	clamp(TVec4<T0>& v, T1 min, T2 max)throw()
		{
			v.x = clamped(v.x,min,max);
			v.y = clamped(v.y,min,max);
			v.z = clamped(v.z,min,max);
			v.w = clamped(v.w,min,max);
		}

	//now implementing template definition 'void clear (2..4) (<[*] v>) direct=1'
	/**
		@brief Sets all components of @a v to 0<br>
		<br>
		2 dimensional specialized version of clear()<br>
		clear() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
	*/
	template <typename T0>
		inline	void	__fastcall	clear(TVec2<T0>& v)throw()
		{
			v.x = 0;
			v.y = 0;
		}

	/**
		@brief Sets all components of @a v to 0<br>
		<br>
		3 dimensional specialized version of clear()<br>
		clear() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
	*/
	template <typename T0>
		inline	void	__fastcall	clear(TVec3<T0>& v)throw()
		{
			v.x = 0;
			v.y = 0;
			v.z = 0;
		}

	/**
		@brief Sets all components of @a v to 0<br>
		<br>
		4 dimensional specialized version of clear()<br>
		clear() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
	*/
	template <typename T0>
		inline	void	__fastcall	clear(TVec4<T0>& v)throw()
		{
			v.x = 0;
			v.y = 0;
			v.z = 0;
			v.w = 0;
		}

	//now implementing template definition 'T0 min (2..4) (<const [*] v>) direct='
	/**
		@brief Determines the minimum coordinate value of @a v<br>
		<br>
		2 dimensional specialized version of min()<br>
		min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	min(const TVec2<T0>& v)throw()
		{
			return vmin(v.x, v.y);
		}

	/**
		@brief Determines the minimum coordinate value of @a v<br>
		<br>
		3 dimensional specialized version of min()<br>
		min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	min(const TVec3<T0>& v)throw()
		{
			return vmin(v.x, vmin(v.y, v.z));
		}

	/**
		@brief Determines the minimum coordinate value of @a v<br>
		<br>
		4 dimensional specialized version of min()<br>
		min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	min(const TVec4<T0>& v)throw()
		{
			return vmin(vmin(v.x, v.y), vmin(v.z, v.w));
		}

	//now implementing template definition 'T0 max (2..4) (<const [*] v>) direct='
	/**
		@brief Determines the maximum coordinate value of @a v<br>
		<br>
		2 dimensional specialized version of max()<br>
		max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	max(const TVec2<T0>& v)throw()
		{
			return vmax(v.x, v.y);
		}

	/**
		@brief Determines the maximum coordinate value of @a v<br>
		<br>
		3 dimensional specialized version of max()<br>
		max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	max(const TVec3<T0>& v)throw()
		{
			return vmax(v.x, vmax(v.y, v.z));
		}

	/**
		@brief Determines the maximum coordinate value of @a v<br>
		<br>
		4 dimensional specialized version of max()<br>
		max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@return  
	*/
	template <typename T0>
		inline	T0	__fastcall	max(const TVec4<T0>& v)throw()
		{
			return vmax(vmax(v.x, v.y), vmax(v.z, v.w));
		}

	//now implementing template definition 'void max (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
		<br>
		2 dimensional specialized version of max()<br>
		max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	max(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = vmax( v.x, w.x);
			result.y = vmax( v.y, w.y);
		}

	/**
		@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
		<br>
		3 dimensional specialized version of max()<br>
		max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	max(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = vmax( v.x, w.x);
			result.y = vmax( v.y, w.y);
			result.z = vmax( v.z, w.z);
		}

	/**
		@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
		<br>
		4 dimensional specialized version of max()<br>
		max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	max(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = vmax( v.x, w.x);
			result.y = vmax( v.y, w.y);
			result.z = vmax( v.z, w.z);
			result.w = vmax( v.w, w.w);
		}

	//now implementing template definition 'void min (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
	/**
		@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
		<br>
		2 dimensional specialized version of min()<br>
		min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	min(const TVec2<T0>& v, const TVec2<T1>& w, TVec2<T2>& result)throw()
		{
			result.x = vmin( v.x, w.x);
			result.y = vmin( v.y, w.y);
		}

	/**
		@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
		<br>
		3 dimensional specialized version of min()<br>
		min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	min(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = vmin( v.x, w.x);
			result.y = vmin( v.y, w.y);
			result.z = vmin( v.z, w.z);
		}

	/**
		@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
		<br>
		4 dimensional specialized version of min()<br>
		min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	min(const TVec4<T0>& v, const TVec4<T1>& w, TVec4<T2>& result)throw()
		{
			result.x = vmin( v.x, w.x);
			result.y = vmin( v.y, w.y);
			result.z = vmin( v.z, w.z);
			result.w = vmin( v.w, w.w);
		}

	//now implementing template definition 'bool oneLess (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if at least one element of @a v is less than the respective element of @a w<br>
		<br>
		2 dimensional specialized version of oneLess()<br>
		oneLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneLess(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return v.x < w.x || v.y < w.y;
		}

	/**
		@brief Tests if at least one element of @a v is less than the respective element of @a w<br>
		<br>
		3 dimensional specialized version of oneLess()<br>
		oneLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneLess(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return v.x < w.x || v.y < w.y || v.z < w.z;
		}

	/**
		@brief Tests if at least one element of @a v is less than the respective element of @a w<br>
		<br>
		4 dimensional specialized version of oneLess()<br>
		oneLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneLess(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return v.x < w.x || v.y < w.y || v.z < w.z || v.w < w.w;
		}

	//now implementing template definition 'bool oneGreater (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if at least one element of @a v is greater than the respective element of @a w<br>
		<br>
		2 dimensional specialized version of oneGreater()<br>
		oneGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneGreater(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return v.x > w.x || v.y > w.y;
		}

	/**
		@brief Tests if at least one element of @a v is greater than the respective element of @a w<br>
		<br>
		3 dimensional specialized version of oneGreater()<br>
		oneGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneGreater(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return v.x > w.x || v.y > w.y || v.z > w.z;
		}

	/**
		@brief Tests if at least one element of @a v is greater than the respective element of @a w<br>
		<br>
		4 dimensional specialized version of oneGreater()<br>
		oneGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	oneGreater(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return v.x > w.x || v.y > w.y || v.z > w.z || v.w > w.w;
		}

	//now implementing template definition 'bool allLess (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if all elements of @a v are less than the respective element of @a w<br>
		<br>
		2 dimensional specialized version of allLess()<br>
		allLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allLess(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return v.x < w.x && v.y < w.y;
		}

	/**
		@brief Tests if all elements of @a v are less than the respective element of @a w<br>
		<br>
		3 dimensional specialized version of allLess()<br>
		allLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allLess(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return v.x < w.x && v.y < w.y && v.z < w.z;
		}

	/**
		@brief Tests if all elements of @a v are less than the respective element of @a w<br>
		<br>
		4 dimensional specialized version of allLess()<br>
		allLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allLess(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return v.x < w.x && v.y < w.y && v.z < w.z && v.w < w.w;
		}

	//now implementing template definition 'bool allGreater (2..4) (<const [*] v>, <const [*] w>) direct='
	/**
		@brief Tests if all elements of @a v are greater than the respective element of @a w<br>
		<br>
		2 dimensional specialized version of allGreater()<br>
		allGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allGreater(const TVec2<T0>& v, const TVec2<T1>& w)throw()
		{
			return v.x > w.x && v.y > w.y;
		}

	/**
		@brief Tests if all elements of @a v are greater than the respective element of @a w<br>
		<br>
		3 dimensional specialized version of allGreater()<br>
		allGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allGreater(const TVec3<T0>& v, const TVec3<T1>& w)throw()
		{
			return v.x > w.x && v.y > w.y && v.z > w.z;
		}

	/**
		@brief Tests if all elements of @a v are greater than the respective element of @a w<br>
		<br>
		4 dimensional specialized version of allGreater()<br>
		allGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1>
		inline	bool	__fastcall	allGreater(const TVec4<T0>& v, const TVec4<T1>& w)throw()
		{
			return v.x > w.x && v.y > w.y && v.z > w.z && v.w > w.w;
		}

	//now implementing template definition 'void vectorSort (2..4) (<[*] v>, <[*] w>) direct=1'
	/**
		@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
		<br>
		2 dimensional specialized version of vectorSort()<br>
		vectorSort() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[out] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	vectorSort(TVec2<T0>& v, TVec2<T1>& w)throw()
		{
			if (w.x<v.x)
				swp(v.x,w.x);
			if (w.y<v.y)
				swp(v.y,w.y);
		}

	/**
		@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
		<br>
		3 dimensional specialized version of vectorSort()<br>
		vectorSort() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[out] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	vectorSort(TVec3<T0>& v, TVec3<T1>& w)throw()
		{
			if (w.x<v.x)
				swp(v.x,w.x);
			if (w.y<v.y)
				swp(v.y,w.y);
			if (w.z<v.z)
				swp(v.z,w.z);
		}

	/**
		@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
		<br>
		4 dimensional specialized version of vectorSort()<br>
		vectorSort() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[out] w 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	vectorSort(TVec4<T0>& v, TVec4<T1>& w)throw()
		{
			if (w.x<v.x)
				swp(v.x,w.x);
			if (w.y<v.y)
				swp(v.y,w.y);
			if (w.z<v.z)
				swp(v.z,w.z);
			if (w.w<v.w)
				swp(v.w,w.w);
		}

	//now implementing template definition 'void sphereCoords (<planar_angle>, <height_angle>, <radius>, <[3] result>) direct='
	/**
		@brief <br>
		<br>
		sphereCoords() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] planar_angle 
		@param[in] height_angle 
		@param[in] radius 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	void	__fastcall	sphereCoords(T0 planar_angle, T1 height_angle, T2 radius, TVec3<T3>& result)throw()
		{
			result.y = vsin(height_angle*M_PI/180)*radius;
			T3	r2 = vcos(height_angle*M_PI/180)*radius;
			result.x = vcos(planar_angle*M_PI/180)*r2;
			result.z = vsin(planar_angle*M_PI/180)*r2;
		}

	//now implementing template definition 'void sphereCoordsRad (<planar_angle>, <height_angle>, <[3] result>) direct='
	/**
		@brief <br>
		<br>
		sphereCoordsRad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] planar_angle 
		@param[in] height_angle 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	sphereCoordsRad(T0 planar_angle, T1 height_angle, TVec3<T2>& result)throw()
		{
			result.y = vsin(height_angle);
			T2	r2 = vcos(height_angle);
			result.x = vcos(planar_angle)*r2;
			result.z = vsin(planar_angle)*r2;
		}

	//now implementing template definition 'void copy|c (2..16) (<const [*] origin>, <[*] destination>) direct=1'
	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		2 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec2<T0>& origin, TVec2<T1>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		2 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec2<T0>& origin, TVec2<T1>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		3 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec3<T0>& origin, TVec3<T1>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		3 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec3<T0>& origin, TVec3<T1>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		4 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec4<T0>& origin, TVec4<T1>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		4 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec4<T0>& origin, TVec4<T1>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		5 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,5>& origin, TVec<T1,5>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		5 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,5>& origin, TVec<T1,5>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		6 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,6>& origin, TVec<T1,6>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		6 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,6>& origin, TVec<T1,6>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		7 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,7>& origin, TVec<T1,7>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		7 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,7>& origin, TVec<T1,7>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		8 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,8>& origin, TVec<T1,8>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		8 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,8>& origin, TVec<T1,8>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		9 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,9>& origin, TVec<T1,9>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		9 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,9>& origin, TVec<T1,9>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		10 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,10>& origin, TVec<T1,10>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		10 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,10>& origin, TVec<T1,10>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		11 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,11>& origin, TVec<T1,11>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		11 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,11>& origin, TVec<T1,11>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		12 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,12>& origin, TVec<T1,12>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		12 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,12>& origin, TVec<T1,12>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		13 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,13>& origin, TVec<T1,13>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		13 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,13>& origin, TVec<T1,13>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		14 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,14>& origin, TVec<T1,14>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
			destination.v[13] = (T1)origin.v[13];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		14 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,14>& origin, TVec<T1,14>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
			destination.v[13] = (T1)origin.v[13];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		15 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,15>& origin, TVec<T1,15>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
			destination.v[13] = (T1)origin.v[13];
			destination.v[14] = (T1)origin.v[14];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		15 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,15>& origin, TVec<T1,15>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
			destination.v[13] = (T1)origin.v[13];
			destination.v[14] = (T1)origin.v[14];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		16 dimensional specialized version of copy/c()<br>
		copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	copy(const TVec<T0,16>& origin, TVec<T1,16>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
			destination.v[13] = (T1)origin.v[13];
			destination.v[14] = (T1)origin.v[14];
			destination.v[15] = (T1)origin.v[15];
		}

	/**
		@brief Copies elements from @a origin to @a destination<br>
		<br>
		16 dimensional specialized version of copy/c()<br>
		c() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] origin array to copy from
		@param[out] destination array to copy to
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	c(const TVec<T0,16>& origin, TVec<T1,16>& destination)throw()
		{
			destination.x = (T1)origin.x;
			destination.y = (T1)origin.y;
			destination.z = (T1)origin.z;
			destination.w = (T1)origin.w;
			destination.v[4] = (T1)origin.v[4];
			destination.v[5] = (T1)origin.v[5];
			destination.v[6] = (T1)origin.v[6];
			destination.v[7] = (T1)origin.v[7];
			destination.v[8] = (T1)origin.v[8];
			destination.v[9] = (T1)origin.v[9];
			destination.v[10] = (T1)origin.v[10];
			destination.v[11] = (T1)origin.v[11];
			destination.v[12] = (T1)origin.v[12];
			destination.v[13] = (T1)origin.v[13];
			destination.v[14] = (T1)origin.v[14];
			destination.v[15] = (T1)origin.v[15];
		}

	//now implementing template definition 'void cross (<const [3] v>, <const [3] w>, <[3] result>) direct='
	/**
		@brief <br>
		<br>
		cross() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[in] w 
		@param[out] result 
	*/
	template <typename T0, typename T1, typename T2>
		inline	void	__fastcall	cross(const TVec3<T0>& v, const TVec3<T1>& w, TVec3<T2>& result)throw()
		{
			result.x = v.y*w.z - v.z*w.y;
			result.y = v.z*w.x - v.x*w.z;
			result.z = v.x*w.y - v.y*w.x;
		}

	//now implementing template definition 'T0 crossDot (<const [3] u>, <const [3] v>, <const [3] w>) direct='
	/**
		@brief <br>
		<br>
		crossDot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] u 
		@param[in] v 
		@param[in] w 
		@return  
	*/
	template <typename T0, typename T1, typename T2>
		inline	T0	__fastcall	crossDot(const TVec3<T0>& u, const TVec3<T1>& v, const TVec3<T2>& w)throw()
		{
			return	(u.y*v.z - u.z*v.y) * w.x
					+(u.z*v.x - u.x*v.z) * w.y
					+(u.x*v.y - u.y*v.x) * w.z;
		}

	//now implementing template definition 'void crossVertical (<const [3] v>, <[3] result>) direct='
	/**
		@brief <br>
		<br>
		crossVertical() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[out] result 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	crossVertical(const TVec3<T0>& v, TVec3<T1>& result)throw()
		{
			result.x = -v.z;
			result.y = 0;
			result.z = v.x;
		}

	//now implementing template definition 'void crossZ (<const [3] v>, <[3] result>) direct='
	/**
		@brief <br>
		<br>
		crossZ() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[out] result 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	crossZ(const TVec3<T0>& v, TVec3<T1>& result)throw()
		{
			result.x = v.y;
			result.y = -v.x;
			result.z = 0;
		}

	//now implementing template definition 'void normal (<const [3] v>, <[3] result>) direct='
	/**
		@brief <br>
		<br>
		normal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] v 
		@param[out] result 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	normal(const TVec3<T0>& v, TVec3<T1>& result)throw()
		{
			TVec3<T1> help;
			help.x = vabs((T1)v.y)+1;
			help.y = (T1)v.z;
			help.z = (T1)v.x;
			
			//block inlining void cross (<const [3] v>, <const [3] w>, <[3] result>) direct= for dimensions=3, assembly_mode='Objects', parameters={v, help, result}...
			{
				result.x = v.y*help.z - v.z*help.y;
				result.y = v.z*help.x - v.x*help.z;
				result.z = v.x*help.y - v.y*help.x;
			};
		}

	//now implementing template definition 'void makeNormalTo (<[3] v>, <const [3] reference>) direct='
	/**
		@brief <br>
		<br>
		makeNormalTo() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[out] v 
		@param[in] reference 
	*/
	template <typename T0, typename T1>
		inline	void	__fastcall	makeNormalTo(TVec3<T0>& v, const TVec3<T1>& reference)throw()
		{
			TVec3<T0> temp;
			
			//block inlining void cross (<const [3] v>, <const [3] w>, <[3] result>) direct= for dimensions=3, assembly_mode='Objects', parameters={v, reference, temp}...
			{
				temp.x = v.y*reference.z - v.z*reference.y;
				temp.y = v.z*reference.x - v.x*reference.z;
				temp.z = v.x*reference.y - v.y*reference.x;
			};
			
			//block inlining void cross (<const [3] v>, <const [3] w>, <[3] result>) direct= for dimensions=3, assembly_mode='Objects', parameters={reference, temp, v}...
			{
				v.x = reference.y*temp.z - reference.z*temp.y;
				v.y = reference.z*temp.x - reference.x*temp.z;
				v.z = reference.x*temp.y - reference.y*temp.x;
			};
		}

	//now implementing template definition 'T3 intercept (2..4) (<const [*] position>, <const [*] velocity>, <interception_velocity>, <[*] result>) direct='
	/**
		@brief Calculates the time and direction of interception of an object at @a position moving with a specific constant speed and direction @a velocity<br>
		<br>
		2 dimensional specialized version of intercept()<br>
		intercept() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] position 
		@param[in] velocity 
		@param[in] interception_velocity 
		@param[out] result normalized direction of intersection, if any; the value of this variable remains unchanged if no interseption could be determined
		@return time of intersection, or 0 if no such could be determined 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	T3	__fastcall	intercept(const TVec2<T0>& position, const TVec2<T1>& velocity, T2 interception_velocity, TVec2<T3>& result)throw()
		{
			T3	rs[2],
					a = (velocity.x*velocity.x + velocity.y*velocity.y) - interception_velocity*interception_velocity,
					b = 2*(position.x*velocity.x + position.y*velocity.y),
					c = (position.x*position.x + position.y*position.y);
			BYTE num_rs = solveSqrEquation(a, b, c, rs);
			if (!num_rs)
				return 0;
			T3	t = smallestPositiveResult(rs,num_rs);
			if (t<=0)
				return 0;
			T3 f = T3(1)/t;
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=2, assembly_mode='Objects', parameters={velocity, position, f, result}...
			{
				result.x = velocity.x + position.x * f;
				result.y = velocity.y + position.y * f;
			};
			
			//block inlining void normalize (2..4) (<[*] vector>) direct= for dimensions=2, assembly_mode='Objects', parameters={result}...
			{
				T3 len = vsqrt((result.x*result.x + result.y*result.y));
				result.x /= len;
				result.y /= len;
			};
			return t;
		}

	/**
		@brief Calculates the time and direction of interception of an object at @a position moving with a specific constant speed and direction @a velocity<br>
		<br>
		3 dimensional specialized version of intercept()<br>
		intercept() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] position 
		@param[in] velocity 
		@param[in] interception_velocity 
		@param[out] result normalized direction of intersection, if any; the value of this variable remains unchanged if no interseption could be determined
		@return time of intersection, or 0 if no such could be determined 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	T3	__fastcall	intercept(const TVec3<T0>& position, const TVec3<T1>& velocity, T2 interception_velocity, TVec3<T3>& result)throw()
		{
			T3	rs[2],
					a = (velocity.x*velocity.x + velocity.y*velocity.y + velocity.z*velocity.z) - interception_velocity*interception_velocity,
					b = 2*(position.x*velocity.x + position.y*velocity.y + position.z*velocity.z),
					c = (position.x*position.x + position.y*position.y + position.z*position.z);
			BYTE num_rs = solveSqrEquation(a, b, c, rs);
			if (!num_rs)
				return 0;
			T3	t = smallestPositiveResult(rs,num_rs);
			if (t<=0)
				return 0;
			T3 f = T3(1)/t;
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={velocity, position, f, result}...
			{
				result.x = velocity.x + position.x * f;
				result.y = velocity.y + position.y * f;
				result.z = velocity.z + position.z * f;
			};
			
			//block inlining void normalize (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='Objects', parameters={result}...
			{
				T3 len = vsqrt((result.x*result.x + result.y*result.y + result.z*result.z));
				result.x /= len;
				result.y /= len;
				result.z /= len;
			};
			return t;
		}

	/**
		@brief Calculates the time and direction of interception of an object at @a position moving with a specific constant speed and direction @a velocity<br>
		<br>
		4 dimensional specialized version of intercept()<br>
		intercept() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
	
		@param[in] position 
		@param[in] velocity 
		@param[in] interception_velocity 
		@param[out] result normalized direction of intersection, if any; the value of this variable remains unchanged if no interseption could be determined
		@return time of intersection, or 0 if no such could be determined 
	*/
	template <typename T0, typename T1, typename T2, typename T3>
		inline	T3	__fastcall	intercept(const TVec4<T0>& position, const TVec4<T1>& velocity, T2 interception_velocity, TVec4<T3>& result)throw()
		{
			T3	rs[2],
					a = (velocity.x*velocity.x + velocity.y*velocity.y + velocity.z*velocity.z + velocity.w*velocity.w) - interception_velocity*interception_velocity,
					b = 2*(position.x*velocity.x + position.y*velocity.y + position.z*velocity.z + position.w*velocity.w),
					c = (position.x*position.x + position.y*position.y + position.z*position.z + position.w*position.w);
			BYTE num_rs = solveSqrEquation(a, b, c, rs);
			if (!num_rs)
				return 0;
			T3	t = smallestPositiveResult(rs,num_rs);
			if (t<=0)
				return 0;
			T3 f = T3(1)/t;
			
			//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=4, assembly_mode='Objects', parameters={velocity, position, f, result}...
			{
				result.x = velocity.x + position.x * f;
				result.y = velocity.y + position.y * f;
				result.z = velocity.z + position.z * f;
				result.w = velocity.w + position.w * f;
			};
			
			//block inlining void normalize (2..4) (<[*] vector>) direct= for dimensions=4, assembly_mode='Objects', parameters={result}...
			{
				T3 len = vsqrt((result.x*result.x + result.y*result.y + result.z*result.z + result.w*result.w));
				result.x /= len;
				result.y /= len;
				result.z /= len;
				result.w /= len;
			};
			return t;
		}

	//now implementing template definition 'String toString (2..4) (<const [*] vector>) direct='
	/**
		@brief <br>
		<br>
		2 dimensional specialized version of toString()<br>
		toString() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] vector 
		@return  
	*/
	template <typename T0>
		inline	String	__fastcall	toString(const TVec2<T0>& vector)throw()
		{
			return '('+ String(vector.x) +", "+ String(vector.y)+')';
		}

	/**
		@brief <br>
		<br>
		3 dimensional specialized version of toString()<br>
		toString() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] vector 
		@return  
	*/
	template <typename T0>
		inline	String	__fastcall	toString(const TVec3<T0>& vector)throw()
		{
			return '('+ String(vector.x) +", "+ String(vector.y) +", "+ String(vector.z)+')';
		}

	/**
		@brief <br>
		<br>
		4 dimensional specialized version of toString()<br>
		toString() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
	
		@param[in] vector 
		@return  
	*/
	template <typename T0>
		inline	String	__fastcall	toString(const TVec4<T0>& vector)throw()
		{
			return '('+ String(vector.x) +", "+ String(vector.y) +", "+ String(vector.z) +", "+ String(vector.w)+')';
		}

};

template <count_t Current, count_t Dimensions, bool Terminal>
	class VecV_Include__
	{};
template <count_t Current, count_t Dimensions>
	class VecV_Include__<Current,Dimensions,true>
	{
	public:

		/* ----- Now implementing pointer based recursion terminators ----- */
		//now implementing template definition 'void addValue|addVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addValue(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] + value;
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addVal(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] + value;
			}

		//now implementing template definition 'void addValue|addVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addValue(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] += value;
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addVal(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] += value;
			}

		//now implementing template definition 'void add (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	add(const T0 u[Dimensions], const T1 v[Dimensions], const T2 w[Dimensions], T3 result[Dimensions])throw()
			{
				result[Current] = u[Current] + v[Current] + w[Current];
			}

		//now implementing template definition 'void add (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	add(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current]+w[Current];
			}

		//now implementing template definition 'void add (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	add(T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				v[Current] += w[Current];
			}

		//now implementing template definition 'void subtract|sub (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtract(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - w[Current];
			}

		/**
			@brief <br>
			<br>
			sub() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	sub(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - w[Current];
			}

		//now implementing template definition 'void subtract|sub (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtract(T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				v[Current] -= w[Current];
			}

		/**
			@brief <br>
			<br>
			sub() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	sub(T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				v[Current] -= w[Current];
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subtractValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtractValue(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - value;
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subVal(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - value;
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subtractValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtractValue(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] -= value;
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subVal(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] -= value;
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<const [*] v>, <factor>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multiply(const T0 v[Dimensions], T1 factor, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * factor;
			}

		/**
			@brief <br>
			<br>
			mult() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mult(const T0 v[Dimensions], T1 factor, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * factor;
			}

		/**
			@brief <br>
			<br>
			mul() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mul(const T0 v[Dimensions], T1 factor, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * factor;
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	multiply(T0 v[Dimensions], T1 factor)throw()
			{
				v[Current] *= factor;
			}

		/**
			@brief <br>
			<br>
			mult() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mult(T0 v[Dimensions], T1 factor)throw()
			{
				v[Current] *= factor;
			}

		/**
			@brief <br>
			<br>
			mul() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mul(T0 v[Dimensions], T1 factor)throw()
			{
				v[Current] *= factor;
			}

		//now implementing template definition 'void divide|div (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	divide(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] / value;
			}

		/**
			@brief <br>
			<br>
			div() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	div(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] / value;
			}

		//now implementing template definition 'void divide|div (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	divide(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] /= value;
			}

		/**
			@brief <br>
			<br>
			div() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	div(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] /= value;
			}

		//now implementing template definition 'void resolve (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			resolve() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	resolve(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] / w[Current];
			}

		//now implementing template definition 'void stretch (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise product of v and w to result<br>
			<br>
			stretch() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	stretch(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * w[Current];
			}

		//now implementing template definition 'void center (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of v and w to result<br>
			<br>
			center() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	center(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = (v[Current] + w[Current])/T2(2);
			}

		//now implementing template definition 'void center (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of u, v, and w to result<br>
			<br>
			center() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	center(const T0 u[Dimensions], const T1 v[Dimensions], const T2 w[Dimensions], T3 result[Dimensions])throw()
			{
				result[Current] = (u[Current] + v[Current] + w[Current])/T3(3);
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1'
		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			multAdd() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multAdd(T0 current[Dimensions], const T1 vector[Dimensions], T2 scalar)throw()
			{
				current[Current] += vector[Current] * scalar;
			}

		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			mad() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mad(T0 current[Dimensions], const T1 vector[Dimensions], T2 scalar)throw()
			{
				current[Current] += vector[Current] * scalar;
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1'
		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			multAdd() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	multAdd(const T0 base[Dimensions], const T1 vector[Dimensions], T2 scalar, T3 result[Dimensions])throw()
			{
				result[Current] = base[Current] + vector[Current] * scalar;
			}

		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			mad() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	mad(const T0 base[Dimensions], const T1 vector[Dimensions], T2 scalar, T3 result[Dimensions])throw()
			{
				result[Current] = base[Current] + vector[Current] * scalar;
			}

		//now implementing template definition 'void SplitBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] out0>, <[*] out1>, <[*] out2>, <[*] out3>) direct=1'
		/**
			@brief <br>
			<br>
			SplitBezierCurveAxis() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] out0 
			@param[out] out1 
			@param[out] out2 
			@param[out] out3 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static	inline	void	__fastcall	SplitBezierCurveAxis(const T0 p0[Dimensions], const T1 p1[Dimensions], const T2 p2[Dimensions], const T3 p3[Dimensions], T4 t, T5 out0[Dimensions], T6 out1[Dimensions], T7 out2[Dimensions], T8 out3[Dimensions])throw()
			{
				{
					T5 x01 = (p1[Current] - p0[Current])*t + p0[Current];
					T5 x12 = (p2[Current] - p1[Current])*t + p1[Current];
					T5 x23 = (p3[Current] - p2[Current])*t + p2[Current];
					T5 x012 = (x12 - x01) * t + x01;
					T5 x123 = (x23 - x12) * t + x12;
					T5 x0123 = (x123 - x012) * t + x012;
					out0[Current] = p0[Current];
					out1[Current] = x01;
					out2[Current] = x012;
					out3[Current] = x0123;
				}
			}

		//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>, <tolerance>) direct=1'
		/**
			@brief Compares two vectors for lexicographic order<br>
			<br>
			compare() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v0 
			@param[in] v1 
			@param[in] tolerance 
			@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	char	__fastcall	compare(const T0 v0[Dimensions], const T1 v1[Dimensions], T2 tolerance)throw()
			{
				{
					T0 delta = v0[Current] - v1[Current];
					if (delta < -tolerance)
						return -1;
					if (delta > tolerance)
						return 1;
				}
				return 0;
			}

		//now implementing template definition 'void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1'
		/**
			@brief Scales the distance between @a center and @a current by the specified @a factor<br>
			<br>
			scale() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] factor 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scale(const T0 center[Dimensions], T1 factor, T2 current[Dimensions])throw()
			{
				current[Current] = center[Current]+(current[Current]-center[Current])*factor;
			}

		//now implementing template definition 'void abs (2..4) (<[*] v>) direct=1'
		/**
			@brief <br>
			<br>
			abs() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	abs(T0 v[Dimensions])throw()
			{
				v[Current] = vabs(v[Current]);
			}

		//now implementing template definition 'void set (2..16) (<[*] v>, <value>) direct=1'
		/**
			@brief Sets all components of @a v to @a value<br>
			<br>
			set() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	set(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] = value;
			}

		//now implementing template definition 'void clamp (2..4) (<[*] v>, <min>, <max>) direct=1'
		/**
			@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
			<br>
			clamp() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] min 
			@param[in] max 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	clamp(T0 v[Dimensions], T1 min, T2 max)throw()
			{
				v[Current] = clamped(v[Current],min,max);
			}

		//now implementing template definition 'void clear (2..4) (<[*] v>) direct=1'
		/**
			@brief Sets all components of @a v to 0<br>
			<br>
			clear() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	clear(T0 v[Dimensions])throw()
			{
				v[Current] = 0;
			}

		//now implementing template definition 'void max (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
			<br>
			max() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	max(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = vmax( v[Current], w[Current]);
			}

		//now implementing template definition 'void min (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
			<br>
			min() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	min(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = vmin( v[Current], w[Current]);
			}

		//now implementing template definition 'void vectorSort (2..4) (<[*] v>, <[*] w>) direct=1'
		/**
			@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
			<br>
			vectorSort() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[out] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	vectorSort(T0 v[Dimensions], T1 w[Dimensions])throw()
			{
				if (w[Current]<v[Current])
					swp(v[Current],w[Current]);
			}

		//now implementing template definition 'void copy|c (2..16) (<const [*] origin>, <[*] destination>) direct=1'
		/**
			@brief Copies elements from @a origin to @a destination<br>
			<br>
			copy() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] origin array to copy from
			@param[out] destination array to copy to
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	copy(const T0 origin[Dimensions], T1 destination[Dimensions])throw()
			{
				destination[Current] = (T1)origin[Current];
			}


		/* ----- Now implementing object based recursion terminators ----- */
		//now implementing template definition 'void addValue|addVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addValue(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] + value;
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addVal(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] + value;
			}

		//now implementing template definition 'void addValue|addVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addValue(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] += value;
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addVal(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] += value;
			}

		//now implementing template definition 'void add (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	add(const TVec<T0,Dimensions>& u, const TVec<T1,Dimensions>& v, const TVec<T2,Dimensions>& w, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = u.v[Current] + v.v[Current] + w.v[Current];
			}

		//now implementing template definition 'void add (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	add(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current]+w.v[Current];
			}

		//now implementing template definition 'void add (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	add(TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				v.v[Current] += w.v[Current];
			}

		//now implementing template definition 'void subtract|sub (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtract(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - w.v[Current];
			}

		/**
			@brief <br>
			<br>
			sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	sub(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - w.v[Current];
			}

		//now implementing template definition 'void subtract|sub (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtract(TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				v.v[Current] -= w.v[Current];
			}

		/**
			@brief <br>
			<br>
			sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	sub(TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				v.v[Current] -= w.v[Current];
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtractValue(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - value;
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subVal(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - value;
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtractValue(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] -= value;
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subVal(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] -= value;
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<const [*] v>, <factor>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multiply(const TVec<T0,Dimensions>& v, T1 factor, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * factor;
			}

		/**
			@brief <br>
			<br>
			mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mult(const TVec<T0,Dimensions>& v, T1 factor, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * factor;
			}

		/**
			@brief <br>
			<br>
			mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mul(const TVec<T0,Dimensions>& v, T1 factor, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * factor;
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	multiply(TVec<T0,Dimensions>& v, T1 factor)throw()
			{
				v.v[Current] *= factor;
			}

		/**
			@brief <br>
			<br>
			mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mult(TVec<T0,Dimensions>& v, T1 factor)throw()
			{
				v.v[Current] *= factor;
			}

		/**
			@brief <br>
			<br>
			mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mul(TVec<T0,Dimensions>& v, T1 factor)throw()
			{
				v.v[Current] *= factor;
			}

		//now implementing template definition 'void divide|div (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	divide(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] / value;
			}

		/**
			@brief <br>
			<br>
			div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	div(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] / value;
			}

		//now implementing template definition 'void divide|div (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	divide(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] /= value;
			}

		/**
			@brief <br>
			<br>
			div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	div(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] /= value;
			}

		//now implementing template definition 'void resolve (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			resolve() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	resolve(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] / w.v[Current];
			}

		//now implementing template definition 'void stretch (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise product of v and w to result<br>
			<br>
			stretch() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	stretch(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * w.v[Current];
			}

		//now implementing template definition 'void center (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of v and w to result<br>
			<br>
			center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	center(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = (v.v[Current] + w.v[Current])/T2(2);
			}

		//now implementing template definition 'void center (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of u, v, and w to result<br>
			<br>
			center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	center(const TVec<T0,Dimensions>& u, const TVec<T1,Dimensions>& v, const TVec<T2,Dimensions>& w, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = (u.v[Current] + v.v[Current] + w.v[Current])/T3(3);
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1'
		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multAdd(TVec<T0,Dimensions>& current, const TVec<T1,Dimensions>& vector, T2 scalar)throw()
			{
				current.v[Current] += vector.v[Current] * scalar;
			}

		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mad(TVec<T0,Dimensions>& current, const TVec<T1,Dimensions>& vector, T2 scalar)throw()
			{
				current.v[Current] += vector.v[Current] * scalar;
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1'
		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	multAdd(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& vector, T2 scalar, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = base.v[Current] + vector.v[Current] * scalar;
			}

		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	mad(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& vector, T2 scalar, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = base.v[Current] + vector.v[Current] * scalar;
			}

		//now implementing template definition 'void SplitBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] out0>, <[*] out1>, <[*] out2>, <[*] out3>) direct=1'
		/**
			@brief <br>
			<br>
			SplitBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] out0 
			@param[out] out1 
			@param[out] out2 
			@param[out] out3 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static	inline	void	__fastcall	SplitBezierCurveAxis(const TVec<T0,Dimensions>& p0, const TVec<T1,Dimensions>& p1, const TVec<T2,Dimensions>& p2, const TVec<T3,Dimensions>& p3, T4 t, TVec<T5,Dimensions>& out0, TVec<T6,Dimensions>& out1, TVec<T7,Dimensions>& out2, TVec<T8,Dimensions>& out3)throw()
			{
				{
					T5 x01 = (p1.v[Current] - p0.v[Current])*t + p0.v[Current];
					T5 x12 = (p2.v[Current] - p1.v[Current])*t + p1.v[Current];
					T5 x23 = (p3.v[Current] - p2.v[Current])*t + p2.v[Current];
					T5 x012 = (x12 - x01) * t + x01;
					T5 x123 = (x23 - x12) * t + x12;
					T5 x0123 = (x123 - x012) * t + x012;
					out0.v[Current] = p0.v[Current];
					out1.v[Current] = x01;
					out2.v[Current] = x012;
					out3.v[Current] = x0123;
				}
			}

		//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>, <tolerance>) direct=1'
		/**
			@brief Compares two vectors for lexicographic order<br>
			<br>
			compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v0 
			@param[in] v1 
			@param[in] tolerance 
			@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	char	__fastcall	compare(const TVec<T0,Dimensions>& v0, const TVec<T1,Dimensions>& v1, T2 tolerance)throw()
			{
				{
					T0 delta = v0.v[Current] - v1.v[Current];
					if (delta < -tolerance)
						return -1;
					if (delta > tolerance)
						return 1;
				}
				return 0;
			}

		//now implementing template definition 'void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1'
		/**
			@brief Scales the distance between @a center and @a current by the specified @a factor<br>
			<br>
			scale() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] factor 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scale(const TVec<T0,Dimensions>& center, T1 factor, TVec<T2,Dimensions>& current)throw()
			{
				current.v[Current] = center.v[Current]+(current.v[Current]-center.v[Current])*factor;
			}

		//now implementing template definition 'void abs (2..4) (<[*] v>) direct=1'
		/**
			@brief <br>
			<br>
			abs() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	abs(TVec<T0,Dimensions>& v)throw()
			{
				v.v[Current] = vabs(v.v[Current]);
			}

		//now implementing template definition 'void set (2..16) (<[*] v>, <value>) direct=1'
		/**
			@brief Sets all components of @a v to @a value<br>
			<br>
			set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	set(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] = value;
			}

		//now implementing template definition 'void clamp (2..4) (<[*] v>, <min>, <max>) direct=1'
		/**
			@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
			<br>
			clamp() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] min 
			@param[in] max 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	clamp(TVec<T0,Dimensions>& v, T1 min, T2 max)throw()
			{
				v.v[Current] = clamped(v.v[Current],min,max);
			}

		//now implementing template definition 'void clear (2..4) (<[*] v>) direct=1'
		/**
			@brief Sets all components of @a v to 0<br>
			<br>
			clear() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	clear(TVec<T0,Dimensions>& v)throw()
			{
				v.v[Current] = 0;
			}

		//now implementing template definition 'void max (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
			<br>
			max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	max(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = vmax( v.v[Current], w.v[Current]);
			}

		//now implementing template definition 'void min (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
			<br>
			min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	min(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = vmin( v.v[Current], w.v[Current]);
			}

		//now implementing template definition 'void vectorSort (2..4) (<[*] v>, <[*] w>) direct=1'
		/**
			@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
			<br>
			vectorSort() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[out] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	vectorSort(TVec<T0,Dimensions>& v, TVec<T1,Dimensions>& w)throw()
			{
				if (w.v[Current]<v.v[Current])
					swp(v.v[Current],w.v[Current]);
			}

		//now implementing template definition 'void copy|c (2..16) (<const [*] origin>, <[*] destination>) direct=1'
		/**
			@brief Copies elements from @a origin to @a destination<br>
			<br>
			copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] origin array to copy from
			@param[out] destination array to copy to
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	copy(const TVec<T0,Dimensions>& origin, TVec<T1,Dimensions>& destination)throw()
			{
				destination.v[Current] = (T1)origin.v[Current];
			}


		/* ----- Now implementing helper class terminators ----- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4>
			static inline	void	__fastcall	User16_interpolate(T0 __p0, const T1 __p1[Dimensions], const T2 __p2[Dimensions], T3 __p3, T4 __p4[Dimensions])throw()
			{
				__p4[Current] = __p1[Current] * __p0 + __p2[Current] * __p3;
			}

		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator17_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current]*__p1[Current];
			}

		template <typename T0>
			static inline	T0	__fastcall	Operator18_sum(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current]*__p0[Current];
			}

		template <typename T0>
			static inline	T0	__fastcall	Operator19_sum(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current];
			}

		template <typename T0>
			static inline	bool	__fastcall	LogicOperator20_and(const T0 __p0[Dimensions])throw()
			{
				return !__p0[Current];
			}

		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator21_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return sqr(__p0[Current] - __p1[Current]);
			}

		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static inline	void	__fastcall	User22_resolveUCBS(T0 __p0, T1 __p1, T2 __p2, T3 __p3, const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], const T7 __p7[Dimensions], T8 __p8[Dimensions])throw()
			{
				__p8[Current] = __p4[Current]*__p0 + __p5[Current]*__p1 + __p6[Current]*__p2 + __p7[Current]*__p3;
			}

		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User23_resolveUCBSaxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current]-__p3[Current])*__p0 + (__p5[Current]-__p4[Current])*__p1 + (__p6[Current]-__p5[Current])*__p2;
			}

		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User24_resolveBezierCurveAxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current] - __p3[Current])*__p0 + (__p5[Current] - __p4[Current])*__p1 + (__p6[Current] - __p5[Current])*__p2;
			}

		template <typename T0>
			static inline	void	__fastcall	User25_normalize0(T0 __p0[Dimensions])throw()
			{
					__p0[Current] = 0;
			}

		template <typename T0, typename T1>
			static inline	void	__fastcall	User26_normalize0(T0 __p0[Dimensions], T1 __p1)throw()
			{
				__p0[Current] /= __p1;
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator27_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] == __p1[Current];
			}

		template <typename T0, typename T1, typename T2>
			static inline	T0	__fastcall	Operator28_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions], T2 __p2)throw()
			{
				return sqr(__p0[Current] - __p1[Current]);
			}

		template <typename T0, typename T1>
			static inline	T0	__fastcall	Functional29_max(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return vabs(__p0[Current]-__p1[Current]);
			}

		template <typename T0>
			static inline	bool	__fastcall	LogicOperator30_or(const T0 __p0[Dimensions])throw()
			{
				return isnan(__p0[Current]);
			}

		template <typename T0>
			static inline	T0	__fastcall	Functional31_min(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current];
			}

		template <typename T0>
			static inline	T0	__fastcall	Functional32_max(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator33_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] < __p1[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator34_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] > __p1[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator35_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] < __p1[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator36_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] > __p1[Current];
			}

		template <typename T0>
			static inline	String	__fastcall	Operator37_glue(const T0 __p0[Dimensions])throw()
			{
				return String(__p0[Current]);
			}

		template <typename T0, typename T1, typename T2, typename T3, typename T4>
			static inline	void	__fastcall	User38_interpolate(T0 __p0, const T1 __p1[Dimensions], const T2 __p2[Dimensions], T3 __p3, T4 __p4[Dimensions])throw()
			{
				__p4[Current] = __p1[Current] * __p0 + __p2[Current] * __p3;
			}

		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator39_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current]*__p1[Current];
			}

		template <typename T0>
			static inline	T0	__fastcall	Operator40_sum(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current]*__p0[Current];
			}

		template <typename T0>
			static inline	T0	__fastcall	Operator41_sum(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current];
			}

		template <typename T0>
			static inline	bool	__fastcall	LogicOperator42_and(const T0 __p0[Dimensions])throw()
			{
				return !__p0[Current];
			}

		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator43_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return sqr(__p0[Current] - __p1[Current]);
			}

		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static inline	void	__fastcall	User44_resolveUCBS(T0 __p0, T1 __p1, T2 __p2, T3 __p3, const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], const T7 __p7[Dimensions], T8 __p8[Dimensions])throw()
			{
				__p8[Current] = __p4[Current]*__p0 + __p5[Current]*__p1 + __p6[Current]*__p2 + __p7[Current]*__p3;
			}

		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User45_resolveUCBSaxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current]-__p3[Current])*__p0 + (__p5[Current]-__p4[Current])*__p1 + (__p6[Current]-__p5[Current])*__p2;
			}

		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User46_resolveBezierCurveAxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current] - __p3[Current])*__p0 + (__p5[Current] - __p4[Current])*__p1 + (__p6[Current] - __p5[Current])*__p2;
			}

		template <typename T0>
			static inline	void	__fastcall	User47_normalize0(T0 __p0[Dimensions])throw()
			{
					__p0[Current] = 0;
			}

		template <typename T0, typename T1>
			static inline	void	__fastcall	User48_normalize0(T0 __p0[Dimensions], T1 __p1)throw()
			{
				__p0[Current] /= __p1;
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator49_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] == __p1[Current];
			}

		template <typename T0, typename T1, typename T2>
			static inline	T0	__fastcall	Operator50_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions], T2 __p2)throw()
			{
				return sqr(__p0[Current] - __p1[Current]);
			}

		template <typename T0, typename T1>
			static inline	T0	__fastcall	Functional51_max(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return vabs(__p0[Current]-__p1[Current]);
			}

		template <typename T0>
			static inline	bool	__fastcall	LogicOperator52_or(const T0 __p0[Dimensions])throw()
			{
				return isnan(__p0[Current]);
			}

		template <typename T0>
			static inline	T0	__fastcall	Functional53_min(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current];
			}

		template <typename T0>
			static inline	T0	__fastcall	Functional54_max(const T0 __p0[Dimensions])throw()
			{
				return __p0[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator55_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] < __p1[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator56_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] > __p1[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator57_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] < __p1[Current];
			}

		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator58_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return __p0[Current] > __p1[Current];
			}

		template <typename T0>
			static inline	String	__fastcall	Operator59_glue(const T0 __p0[Dimensions])throw()
			{
				return String(__p0[Current]);
			}

	};

template <count_t Current, count_t Dimensions>
	class VecV_Include__<Current,Dimensions,false>
	{
	public:

		/* ----- Now implementing helper class iterators ----- */

		/* --- Now processing 'Pointers/User:|||const [*]|const [*]||[*]|<>|__p4:i = __p1:i * __p0 + __p2:i * __p3;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4>
			static inline	void	__fastcall	User16_interpolate(T0 __p0, const T1 __p1[Dimensions], const T2 __p2[Dimensions], T3 __p3, T4 __p4[Dimensions])throw()
			{
				__p4[Current] = __p1[Current] * __p0 + __p2[Current] * __p3;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User16_interpolate(__p0,__p1,__p2,__p3,__p4);
			}


		/* --- Now processing 'Pointers/Operator:+||const [*]|const [*]|<>|v:i*w:i' --- */
		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator17_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current]*__p1[Current] +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator17_sum(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/Operator:+||const [*]|<>|v:i*v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Operator18_sum(const T0 __p0[Dimensions])throw()
			{
				return (
						__p0[Current]*__p0[Current] +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator18_sum(__p0)
					);
			}


		/* --- Now processing 'Pointers/Operator:+||const [*]|<>|v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Operator19_sum(const T0 __p0[Dimensions])throw()
			{
				return (
						__p0[Current] +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator19_sum(__p0)
					);
			}


		/* --- Now processing 'Pointers/LogicOperator:&&||const [*]|<>|!v:i' --- */
		template <typename T0>
			static inline	bool	__fastcall	LogicOperator20_and(const T0 __p0[Dimensions])throw()
			{
				return (
						!__p0[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator20_and(__p0)
					);
			}


		/* --- Now processing 'Pointers/Operator:+||const [*]|const [*]|<>|sqr(v:i - w:i)' --- */
		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator21_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						sqr(__p0[Current] - __p1[Current]) +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator21_sum(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/User:||||||const [*]|const [*]|const [*]|const [*]|[*]|<>|__p8:i = __p4:i*__p0 + __p5:i*__p1 + __p6:i*__p2 + __p7:i*__p3;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static inline	void	__fastcall	User22_resolveUCBS(T0 __p0, T1 __p1, T2 __p2, T3 __p3, const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], const T7 __p7[Dimensions], T8 __p8[Dimensions])throw()
			{
				__p8[Current] = __p4[Current]*__p0 + __p5[Current]*__p1 + __p6[Current]*__p2 + __p7[Current]*__p3;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User22_resolveUCBS(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7,__p8);
			}


		/* --- Now processing 'Pointers/User:|||||const [*]|const [*]|const [*]|const [*]|[*]|<>|__p7:i = (__p4:i-__p3:i)*__p0 + (__p5:i-__p4:i)*__p1 + (__p6:i-__p5:i)*__p2;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User23_resolveUCBSaxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current]-__p3[Current])*__p0 + (__p5[Current]-__p4[Current])*__p1 + (__p6[Current]-__p5[Current])*__p2;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User23_resolveUCBSaxis(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7);
			}


		/* --- Now processing 'Pointers/User:|||||const [*]|const [*]|const [*]|const [*]|[*]|<>|__p7:i = (__p4:i - __p3:i)*__p0 + (__p5:i - __p4:i)*__p1 + (__p6:i - __p5:i)*__p2;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User24_resolveBezierCurveAxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current] - __p3[Current])*__p0 + (__p5[Current] - __p4[Current])*__p1 + (__p6[Current] - __p5[Current])*__p2;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User24_resolveBezierCurveAxis(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7);
			}


		/* --- Now processing 'Pointers/User:||[*]|<>|__p0:i = 0;' --- */
		template <typename T0>
			static inline	void	__fastcall	User25_normalize0(T0 __p0[Dimensions])throw()
			{
					__p0[Current] = 0;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User25_normalize0(__p0);
			}


		/* --- Now processing 'Pointers/User:||[*]||<>|__p0:i /= __p1;' --- */
		template <typename T0, typename T1>
			static inline	void	__fastcall	User26_normalize0(T0 __p0[Dimensions], T1 __p1)throw()
			{
				__p0[Current] /= __p1;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User26_normalize0(__p0,__p1);
			}


		/* --- Now processing 'Pointers/LogicOperator:&&||const [*]|const [*]|<>|v:i == w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator27_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] == __p1[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator27_and(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/Operator:+||const [*]|const [*]||<>|sqr(v:i - w:i)' --- */
		template <typename T0, typename T1, typename T2>
			static inline	T0	__fastcall	Operator28_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions], T2 __p2)throw()
			{
				return (
						sqr(__p0[Current] - __p1[Current]) +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator28_sum(__p0,__p1,__p2)
					);
			}


		/* --- Now processing 'Pointers/Functional:vmax||const [*]|const [*]|<>|vabs(v:i-w:i)' --- */
		template <typename T0, typename T1>
			static inline	T0	__fastcall	Functional29_max(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return vmax(
						vabs(__p0[Current]-__p1[Current]),
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Functional29_max(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/LogicOperator:||||const [*]|<>|isnan(v:i)' --- */
		template <typename T0>
			static inline	bool	__fastcall	LogicOperator30_or(const T0 __p0[Dimensions])throw()
			{
				return (
						isnan(__p0[Current]) ||
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator30_or(__p0)
					);
			}


		/* --- Now processing 'Pointers/Functional:vmin||const [*]|<>|v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Functional31_min(const T0 __p0[Dimensions])throw()
			{
				return vmin(
						__p0[Current],
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Functional31_min(__p0)
					);
			}


		/* --- Now processing 'Pointers/Functional:vmax||const [*]|<>|v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Functional32_max(const T0 __p0[Dimensions])throw()
			{
				return vmax(
						__p0[Current],
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Functional32_max(__p0)
					);
			}


		/* --- Now processing 'Pointers/LogicOperator:||||const [*]|const [*]|<>|v:i < w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator33_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] < __p1[Current] ||
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator33_or(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/LogicOperator:||||const [*]|const [*]|<>|v:i > w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator34_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] > __p1[Current] ||
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator34_or(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/LogicOperator:&&||const [*]|const [*]|<>|v:i < w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator35_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] < __p1[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator35_and(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/LogicOperator:&&||const [*]|const [*]|<>|v:i > w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator36_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] > __p1[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator36_and(__p0,__p1)
					);
			}


		/* --- Now processing 'Pointers/Operator:+", "+||const [*]|<>|String(vector:i)' --- */
		template <typename T0>
			static inline	String	__fastcall	Operator37_glue(const T0 __p0[Dimensions])throw()
			{
				return (
						String(__p0[Current]) +", "+
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator37_glue(__p0)
					);
			}


		/* --- Now processing 'Objects/User:|||const [*]|const [*]||[*]|<>|__p4:i = __p1:i * __p0 + __p2:i * __p3;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4>
			static inline	void	__fastcall	User38_interpolate(T0 __p0, const T1 __p1[Dimensions], const T2 __p2[Dimensions], T3 __p3, T4 __p4[Dimensions])throw()
			{
				__p4[Current] = __p1[Current] * __p0 + __p2[Current] * __p3;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User38_interpolate(__p0,__p1,__p2,__p3,__p4);
			}


		/* --- Now processing 'Objects/Operator:+||const [*]|const [*]|<>|v:i*w:i' --- */
		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator39_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current]*__p1[Current] +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator39_sum(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/Operator:+||const [*]|<>|v:i*v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Operator40_sum(const T0 __p0[Dimensions])throw()
			{
				return (
						__p0[Current]*__p0[Current] +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator40_sum(__p0)
					);
			}


		/* --- Now processing 'Objects/Operator:+||const [*]|<>|v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Operator41_sum(const T0 __p0[Dimensions])throw()
			{
				return (
						__p0[Current] +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator41_sum(__p0)
					);
			}


		/* --- Now processing 'Objects/LogicOperator:&&||const [*]|<>|!v:i' --- */
		template <typename T0>
			static inline	bool	__fastcall	LogicOperator42_and(const T0 __p0[Dimensions])throw()
			{
				return (
						!__p0[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator42_and(__p0)
					);
			}


		/* --- Now processing 'Objects/Operator:+||const [*]|const [*]|<>|sqr(v:i - w:i)' --- */
		template <typename T0, typename T1>
			static inline	T0	__fastcall	Operator43_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						sqr(__p0[Current] - __p1[Current]) +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator43_sum(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/User:||||||const [*]|const [*]|const [*]|const [*]|[*]|<>|__p8:i = __p4:i*__p0 + __p5:i*__p1 + __p6:i*__p2 + __p7:i*__p3;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static inline	void	__fastcall	User44_resolveUCBS(T0 __p0, T1 __p1, T2 __p2, T3 __p3, const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], const T7 __p7[Dimensions], T8 __p8[Dimensions])throw()
			{
				__p8[Current] = __p4[Current]*__p0 + __p5[Current]*__p1 + __p6[Current]*__p2 + __p7[Current]*__p3;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User44_resolveUCBS(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7,__p8);
			}


		/* --- Now processing 'Objects/User:|||||const [*]|const [*]|const [*]|const [*]|[*]|<>|__p7:i = (__p4:i-__p3:i)*__p0 + (__p5:i-__p4:i)*__p1 + (__p6:i-__p5:i)*__p2;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User45_resolveUCBSaxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current]-__p3[Current])*__p0 + (__p5[Current]-__p4[Current])*__p1 + (__p6[Current]-__p5[Current])*__p2;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User45_resolveUCBSaxis(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7);
			}


		/* --- Now processing 'Objects/User:|||||const [*]|const [*]|const [*]|const [*]|[*]|<>|__p7:i = (__p4:i - __p3:i)*__p0 + (__p5:i - __p4:i)*__p1 + (__p6:i - __p5:i)*__p2;' --- */
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
			static inline	void	__fastcall	User46_resolveBezierCurveAxis(T0 __p0, T1 __p1, T2 __p2, const T3 __p3[Dimensions], const T4 __p4[Dimensions], const T5 __p5[Dimensions], const T6 __p6[Dimensions], T7 __p7[Dimensions])throw()
			{
				__p7[Current] = (__p4[Current] - __p3[Current])*__p0 + (__p5[Current] - __p4[Current])*__p1 + (__p6[Current] - __p5[Current])*__p2;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User46_resolveBezierCurveAxis(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7);
			}


		/* --- Now processing 'Objects/User:||[*]|<>|__p0:i = 0;' --- */
		template <typename T0>
			static inline	void	__fastcall	User47_normalize0(T0 __p0[Dimensions])throw()
			{
					__p0[Current] = 0;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User47_normalize0(__p0);
			}


		/* --- Now processing 'Objects/User:||[*]||<>|__p0:i /= __p1;' --- */
		template <typename T0, typename T1>
			static inline	void	__fastcall	User48_normalize0(T0 __p0[Dimensions], T1 __p1)throw()
			{
				__p0[Current] /= __p1;
				VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::User48_normalize0(__p0,__p1);
			}


		/* --- Now processing 'Objects/LogicOperator:&&||const [*]|const [*]|<>|v:i == w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator49_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] == __p1[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator49_and(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/Operator:+||const [*]|const [*]||<>|sqr(v:i - w:i)' --- */
		template <typename T0, typename T1, typename T2>
			static inline	T0	__fastcall	Operator50_sum(const T0 __p0[Dimensions], const T1 __p1[Dimensions], T2 __p2)throw()
			{
				return (
						sqr(__p0[Current] - __p1[Current]) +
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator50_sum(__p0,__p1,__p2)
					);
			}


		/* --- Now processing 'Objects/Functional:vmax||const [*]|const [*]|<>|vabs(v:i-w:i)' --- */
		template <typename T0, typename T1>
			static inline	T0	__fastcall	Functional51_max(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return vmax(
						vabs(__p0[Current]-__p1[Current]),
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Functional51_max(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/LogicOperator:||||const [*]|<>|isnan(v:i)' --- */
		template <typename T0>
			static inline	bool	__fastcall	LogicOperator52_or(const T0 __p0[Dimensions])throw()
			{
				return (
						isnan(__p0[Current]) ||
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator52_or(__p0)
					);
			}


		/* --- Now processing 'Objects/Functional:vmin||const [*]|<>|v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Functional53_min(const T0 __p0[Dimensions])throw()
			{
				return vmin(
						__p0[Current],
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Functional53_min(__p0)
					);
			}


		/* --- Now processing 'Objects/Functional:vmax||const [*]|<>|v:i' --- */
		template <typename T0>
			static inline	T0	__fastcall	Functional54_max(const T0 __p0[Dimensions])throw()
			{
				return vmax(
						__p0[Current],
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Functional54_max(__p0)
					);
			}


		/* --- Now processing 'Objects/LogicOperator:||||const [*]|const [*]|<>|v:i < w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator55_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] < __p1[Current] ||
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator55_or(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/LogicOperator:||||const [*]|const [*]|<>|v:i > w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator56_or(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] > __p1[Current] ||
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator56_or(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/LogicOperator:&&||const [*]|const [*]|<>|v:i < w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator57_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] < __p1[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator57_and(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/LogicOperator:&&||const [*]|const [*]|<>|v:i > w:i' --- */
		template <typename T0, typename T1>
			static inline	bool	__fastcall	LogicOperator58_and(const T0 __p0[Dimensions], const T1 __p1[Dimensions])throw()
			{
				return (
						__p0[Current] > __p1[Current] &&
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::LogicOperator58_and(__p0,__p1)
					);
			}


		/* --- Now processing 'Objects/Operator:+", "+||const [*]|<>|String(vector:i)' --- */
		template <typename T0>
			static inline	String	__fastcall	Operator59_glue(const T0 __p0[Dimensions])throw()
			{
				return (
						String(__p0[Current]) +", "+
						VecV_Include__<Current+1,Dimensions, GreaterOrEqual<Current+2,Dimensions>::eval>::Operator59_glue(__p0)
					);
			}


		/* ----- Now implementing pointer based recursion terminators ----- */
		//now implementing template definition 'void addValue|addVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addValue(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] + value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value, result);
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addVal(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] + value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value, result);
			}

		//now implementing template definition 'void addValue|addVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addValue(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] += value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value);
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addVal(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] += value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value);
			}

		//now implementing template definition 'void add (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	add(const T0 u[Dimensions], const T1 v[Dimensions], const T2 w[Dimensions], T3 result[Dimensions])throw()
			{
				result[Current] = u[Current] + v[Current] + w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::add(u, v, w, result);
			}

		//now implementing template definition 'void add (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	add(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current]+w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::add(v, w, result);
			}

		//now implementing template definition 'void add (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	add(T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				v[Current] += w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::add(v, w);
			}

		//now implementing template definition 'void subtract|sub (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtract(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w, result);
			}

		/**
			@brief <br>
			<br>
			sub() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	sub(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w, result);
			}

		//now implementing template definition 'void subtract|sub (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtract(T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				v[Current] -= w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w);
			}

		/**
			@brief <br>
			<br>
			sub() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	sub(T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				v[Current] -= w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w);
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subtractValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtractValue(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value, result);
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subVal(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] - value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value, result);
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subtractValue() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtractValue(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] -= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value);
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subVal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subVal(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] -= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value);
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<const [*] v>, <factor>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multiply(const T0 v[Dimensions], T1 factor, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor, result);
			}

		/**
			@brief <br>
			<br>
			mult() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mult(const T0 v[Dimensions], T1 factor, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor, result);
			}

		/**
			@brief <br>
			<br>
			mul() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mul(const T0 v[Dimensions], T1 factor, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor, result);
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	multiply(T0 v[Dimensions], T1 factor)throw()
			{
				v[Current] *= factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor);
			}

		/**
			@brief <br>
			<br>
			mult() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mult(T0 v[Dimensions], T1 factor)throw()
			{
				v[Current] *= factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor);
			}

		/**
			@brief <br>
			<br>
			mul() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mul(T0 v[Dimensions], T1 factor)throw()
			{
				v[Current] *= factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor);
			}

		//now implementing template definition 'void divide|div (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	divide(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] / value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value, result);
			}

		/**
			@brief <br>
			<br>
			div() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	div(const T0 v[Dimensions], T1 value, T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] / value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value, result);
			}

		//now implementing template definition 'void divide|div (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	divide(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] /= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value);
			}

		/**
			@brief <br>
			<br>
			div() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	div(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] /= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value);
			}

		//now implementing template definition 'void resolve (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			resolve() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	resolve(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] / w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::resolve(v, w, result);
			}

		//now implementing template definition 'void stretch (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise product of v and w to result<br>
			<br>
			stretch() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	stretch(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = v[Current] * w[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::stretch(v, w, result);
			}

		//now implementing template definition 'void center (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of v and w to result<br>
			<br>
			center() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	center(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = (v[Current] + w[Current])/T2(2);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::center(v, w, result);
			}

		//now implementing template definition 'void center (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of u, v, and w to result<br>
			<br>
			center() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	center(const T0 u[Dimensions], const T1 v[Dimensions], const T2 w[Dimensions], T3 result[Dimensions])throw()
			{
				result[Current] = (u[Current] + v[Current] + w[Current])/T3(3);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::center(u, v, w, result);
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1'
		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			multAdd() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multAdd(T0 current[Dimensions], const T1 vector[Dimensions], T2 scalar)throw()
			{
				current[Current] += vector[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(current, vector, scalar);
			}

		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			mad() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mad(T0 current[Dimensions], const T1 vector[Dimensions], T2 scalar)throw()
			{
				current[Current] += vector[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(current, vector, scalar);
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1'
		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			multAdd() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	multAdd(const T0 base[Dimensions], const T1 vector[Dimensions], T2 scalar, T3 result[Dimensions])throw()
			{
				result[Current] = base[Current] + vector[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(base, vector, scalar, result);
			}

		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			mad() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	mad(const T0 base[Dimensions], const T1 vector[Dimensions], T2 scalar, T3 result[Dimensions])throw()
			{
				result[Current] = base[Current] + vector[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(base, vector, scalar, result);
			}

		//now implementing template definition 'void SplitBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] out0>, <[*] out1>, <[*] out2>, <[*] out3>) direct=1'
		/**
			@brief <br>
			<br>
			SplitBezierCurveAxis() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] out0 
			@param[out] out1 
			@param[out] out2 
			@param[out] out3 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static	inline	void	__fastcall	SplitBezierCurveAxis(const T0 p0[Dimensions], const T1 p1[Dimensions], const T2 p2[Dimensions], const T3 p3[Dimensions], T4 t, T5 out0[Dimensions], T6 out1[Dimensions], T7 out2[Dimensions], T8 out3[Dimensions])throw()
			{
				{
					T5 x01 = (p1[Current] - p0[Current])*t + p0[Current];
					T5 x12 = (p2[Current] - p1[Current])*t + p1[Current];
					T5 x23 = (p3[Current] - p2[Current])*t + p2[Current];
					T5 x012 = (x12 - x01) * t + x01;
					T5 x123 = (x23 - x12) * t + x12;
					T5 x0123 = (x123 - x012) * t + x012;
					out0[Current] = p0[Current];
					out1[Current] = x01;
					out2[Current] = x012;
					out3[Current] = x0123;
				}
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::SplitBezierCurveAxis(p0, p1, p2, p3, t, out0, out1, out2, out3);
			}

		//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>, <tolerance>) direct=1'
		/**
			@brief Compares two vectors for lexicographic order<br>
			<br>
			compare() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v0 
			@param[in] v1 
			@param[in] tolerance 
			@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	char	__fastcall	compare(const T0 v0[Dimensions], const T1 v1[Dimensions], T2 tolerance)throw()
			{
				{
					T0 delta = v0[Current] - v1[Current];
					if (delta < -tolerance)
						return -1;
					if (delta > tolerance)
						return 1;
				}
				return VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::compare(v0, v1, tolerance);
			}

		//now implementing template definition 'void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1'
		/**
			@brief Scales the distance between @a center and @a current by the specified @a factor<br>
			<br>
			scale() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] factor 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scale(const T0 center[Dimensions], T1 factor, T2 current[Dimensions])throw()
			{
				current[Current] = center[Current]+(current[Current]-center[Current])*factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::scale(center, factor, current);
			}

		//now implementing template definition 'void abs (2..4) (<[*] v>) direct=1'
		/**
			@brief <br>
			<br>
			abs() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	abs(T0 v[Dimensions])throw()
			{
				v[Current] = vabs(v[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::abs(v);
			}

		//now implementing template definition 'void set (2..16) (<[*] v>, <value>) direct=1'
		/**
			@brief Sets all components of @a v to @a value<br>
			<br>
			set() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	set(T0 v[Dimensions], T1 value)throw()
			{
				v[Current] = value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::set(v, value);
			}

		//now implementing template definition 'void clamp (2..4) (<[*] v>, <min>, <max>) direct=1'
		/**
			@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
			<br>
			clamp() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] min 
			@param[in] max 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	clamp(T0 v[Dimensions], T1 min, T2 max)throw()
			{
				v[Current] = clamped(v[Current],min,max);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::clamp(v, min, max);
			}

		//now implementing template definition 'void clear (2..4) (<[*] v>) direct=1'
		/**
			@brief Sets all components of @a v to 0<br>
			<br>
			clear() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	clear(T0 v[Dimensions])throw()
			{
				v[Current] = 0;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::clear(v);
			}

		//now implementing template definition 'void max (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
			<br>
			max() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	max(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = vmax( v[Current], w[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::max(v, w, result);
			}

		//now implementing template definition 'void min (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
			<br>
			min() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	min(const T0 v[Dimensions], const T1 w[Dimensions], T2 result[Dimensions])throw()
			{
				result[Current] = vmin( v[Current], w[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::min(v, w, result);
			}

		//now implementing template definition 'void vectorSort (2..4) (<[*] v>, <[*] w>) direct=1'
		/**
			@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
			<br>
			vectorSort() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] v 
			@param[out] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	vectorSort(T0 v[Dimensions], T1 w[Dimensions])throw()
			{
				if (w[Current]<v[Current])
					swp(v[Current],w[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::vectorSort(v, w);
			}

		//now implementing template definition 'void copy|c (2..16) (<const [*] origin>, <[*] destination>) direct=1'
		/**
			@brief Copies elements from @a origin to @a destination<br>
			<br>
			copy() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] origin array to copy from
			@param[out] destination array to copy to
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	copy(const T0 origin[Dimensions], T1 destination[Dimensions])throw()
			{
				destination[Current] = (T1)origin[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::copy(origin, destination);
			}


		/* ----- Now implementing object based recursion terminators ----- */
		//now implementing template definition 'void addValue|addVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addValue(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] + value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value, result);
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to add to
			@param[in] value Value to add
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	addVal(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] + value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value, result);
			}

		//now implementing template definition 'void addValue|addVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addValue(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] += value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value);
			}

		/**
			@brief Adds the specified value to each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			addVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to add to
			@param[in] value Value to add
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	addVal(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] += value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::addValue(v, value);
			}

		//now implementing template definition 'void add (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	add(const TVec<T0,Dimensions>& u, const TVec<T1,Dimensions>& v, const TVec<T2,Dimensions>& w, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = u.v[Current] + v.v[Current] + w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::add(u, v, w, result);
			}

		//now implementing template definition 'void add (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	add(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current]+w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::add(v, w, result);
			}

		//now implementing template definition 'void add (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			add() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	add(TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				v.v[Current] += w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::add(v, w);
			}

		//now implementing template definition 'void subtract|sub (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtract(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w, result);
			}

		/**
			@brief <br>
			<br>
			sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	sub(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w, result);
			}

		//now implementing template definition 'void subtract|sub (2..4) (<[*] v>, <const [*] w>) direct=1'
		/**
			@brief <br>
			<br>
			subtract() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtract(TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				v.v[Current] -= w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w);
			}

		/**
			@brief <br>
			<br>
			sub() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[in] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	sub(TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				v.v[Current] -= w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtract(v, w);
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subtractValue(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value, result);
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result in the respective element of @a result<br>
			<br>
			subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v Vector to subtract from
			@param[in] value Value to subtract
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	subVal(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] - value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value, result);
			}

		//now implementing template definition 'void subtractValue|subVal (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subtractValue() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subtractValue(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] -= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value);
			}

		/**
			@brief Subtracts the specified value from each element of @a v and stores the result back in the respective element of @a v<br>
			<br>
			subVal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v Vector to subtract from
			@param[in] value Value to subtract
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	subVal(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] -= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::subtractValue(v, value);
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<const [*] v>, <factor>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multiply(const TVec<T0,Dimensions>& v, T1 factor, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor, result);
			}

		/**
			@brief <br>
			<br>
			mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mult(const TVec<T0,Dimensions>& v, T1 factor, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor, result);
			}

		/**
			@brief <br>
			<br>
			mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] factor 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mul(const TVec<T0,Dimensions>& v, T1 factor, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor, result);
			}

		//now implementing template definition 'void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1'
		/**
			@brief <br>
			<br>
			multiply() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	multiply(TVec<T0,Dimensions>& v, T1 factor)throw()
			{
				v.v[Current] *= factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor);
			}

		/**
			@brief <br>
			<br>
			mult() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mult(TVec<T0,Dimensions>& v, T1 factor)throw()
			{
				v.v[Current] *= factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor);
			}

		/**
			@brief <br>
			<br>
			mul() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] factor 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	mul(TVec<T0,Dimensions>& v, T1 factor)throw()
			{
				v.v[Current] *= factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(v, factor);
			}

		//now implementing template definition 'void divide|div (2..4) (<const [*] v>, <value>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	divide(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] / value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value, result);
			}

		/**
			@brief <br>
			<br>
			div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] value 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	div(const TVec<T0,Dimensions>& v, T1 value, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] / value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value, result);
			}

		//now implementing template definition 'void divide|div (2..4) (<[*] v>, <value>) direct=1'
		/**
			@brief <br>
			<br>
			divide() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	divide(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] /= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value);
			}

		/**
			@brief <br>
			<br>
			div() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	div(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] /= value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::divide(v, value);
			}

		//now implementing template definition 'void resolve (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief <br>
			<br>
			resolve() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	resolve(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] / w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::resolve(v, w, result);
			}

		//now implementing template definition 'void stretch (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise product of v and w to result<br>
			<br>
			stretch() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	stretch(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = v.v[Current] * w.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::stretch(v, w, result);
			}

		//now implementing template definition 'void center (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of v and w to result<br>
			<br>
			center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	center(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = (v.v[Current] + w.v[Current])/T2(2);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::center(v, w, result);
			}

		//now implementing template definition 'void center (2..4) (<const [*] u>, <const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the arithmetic center of u, v, and w to result<br>
			<br>
			center() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] u 
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	center(const TVec<T0,Dimensions>& u, const TVec<T1,Dimensions>& v, const TVec<T2,Dimensions>& w, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = (u.v[Current] + v.v[Current] + w.v[Current])/T3(3);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::center(u, v, w, result);
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1'
		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	multAdd(TVec<T0,Dimensions>& current, const TVec<T1,Dimensions>& vector, T2 scalar)throw()
			{
				current.v[Current] += vector.v[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(current, vector, scalar);
			}

		/**
			@brief Adds @a vector * @a scalar to @a current<br>
			<br>
			mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current 
			@param[in] vector 
			@param[in] scalar 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	mad(TVec<T0,Dimensions>& current, const TVec<T1,Dimensions>& vector, T2 scalar)throw()
			{
				current.v[Current] += vector.v[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(current, vector, scalar);
			}

		//now implementing template definition 'void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1'
		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			multAdd() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	multAdd(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& vector, T2 scalar, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = base.v[Current] + vector.v[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(base, vector, scalar, result);
			}

		/**
			@brief Writes @a base + @a vector * @a scalar to @a result<br>
			<br>
			mad() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] base 
			@param[in] vector 
			@param[in] scalar 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	mad(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& vector, T2 scalar, TVec<T3,Dimensions>& result)throw()
			{
				result.v[Current] = base.v[Current] + vector.v[Current] * scalar;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(base, vector, scalar, result);
			}

		//now implementing template definition 'void SplitBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] out0>, <[*] out1>, <[*] out2>, <[*] out3>) direct=1'
		/**
			@brief <br>
			<br>
			SplitBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] out0 
			@param[out] out1 
			@param[out] out2 
			@param[out] out3 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
			static	inline	void	__fastcall	SplitBezierCurveAxis(const TVec<T0,Dimensions>& p0, const TVec<T1,Dimensions>& p1, const TVec<T2,Dimensions>& p2, const TVec<T3,Dimensions>& p3, T4 t, TVec<T5,Dimensions>& out0, TVec<T6,Dimensions>& out1, TVec<T7,Dimensions>& out2, TVec<T8,Dimensions>& out3)throw()
			{
				{
					T5 x01 = (p1.v[Current] - p0.v[Current])*t + p0.v[Current];
					T5 x12 = (p2.v[Current] - p1.v[Current])*t + p1.v[Current];
					T5 x23 = (p3.v[Current] - p2.v[Current])*t + p2.v[Current];
					T5 x012 = (x12 - x01) * t + x01;
					T5 x123 = (x23 - x12) * t + x12;
					T5 x0123 = (x123 - x012) * t + x012;
					out0.v[Current] = p0.v[Current];
					out1.v[Current] = x01;
					out2.v[Current] = x012;
					out3.v[Current] = x0123;
				}
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::SplitBezierCurveAxis(p0, p1, p2, p3, t, out0, out1, out2, out3);
			}

		//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>, <tolerance>) direct=1'
		/**
			@brief Compares two vectors for lexicographic order<br>
			<br>
			compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v0 
			@param[in] v1 
			@param[in] tolerance 
			@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	char	__fastcall	compare(const TVec<T0,Dimensions>& v0, const TVec<T1,Dimensions>& v1, T2 tolerance)throw()
			{
				{
					T0 delta = v0.v[Current] - v1.v[Current];
					if (delta < -tolerance)
						return -1;
					if (delta > tolerance)
						return 1;
				}
				return VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::compare(v0, v1, tolerance);
			}

		//now implementing template definition 'void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1'
		/**
			@brief Scales the distance between @a center and @a current by the specified @a factor<br>
			<br>
			scale() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] factor 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scale(const TVec<T0,Dimensions>& center, T1 factor, TVec<T2,Dimensions>& current)throw()
			{
				current.v[Current] = center.v[Current]+(current.v[Current]-center.v[Current])*factor;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::scale(center, factor, current);
			}

		//now implementing template definition 'void abs (2..4) (<[*] v>) direct=1'
		/**
			@brief <br>
			<br>
			abs() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	abs(TVec<T0,Dimensions>& v)throw()
			{
				v.v[Current] = vabs(v.v[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::abs(v);
			}

		//now implementing template definition 'void set (2..16) (<[*] v>, <value>) direct=1'
		/**
			@brief Sets all components of @a v to @a value<br>
			<br>
			set() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] value 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	set(TVec<T0,Dimensions>& v, T1 value)throw()
			{
				v.v[Current] = value;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::set(v, value);
			}

		//now implementing template definition 'void clamp (2..4) (<[*] v>, <min>, <max>) direct=1'
		/**
			@brief Clamps all components of @a v to the range [@a min, @a max ]<br>
			<br>
			clamp() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] v 
			@param[in] min 
			@param[in] max 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	clamp(TVec<T0,Dimensions>& v, T1 min, T2 max)throw()
			{
				v.v[Current] = clamped(v.v[Current],min,max);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::clamp(v, min, max);
			}

		//now implementing template definition 'void clear (2..4) (<[*] v>) direct=1'
		/**
			@brief Sets all components of @a v to 0<br>
			<br>
			clear() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
		*/
		template <typename T0>
			static	inline	void	__fastcall	clear(TVec<T0,Dimensions>& v)throw()
			{
				v.v[Current] = 0;
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::clear(v);
			}

		//now implementing template definition 'void max (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise maximum of @a v and @a w to @a result<br>
			<br>
			max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	max(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = vmax( v.v[Current], w.v[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::max(v, w, result);
			}

		//now implementing template definition 'void min (2..4) (<const [*] v>, <const [*] w>, <[*] result>) direct=1'
		/**
			@brief Writes the component-wise minimum of @a v and @a w to @a result<br>
			<br>
			min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	min(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, TVec<T2,Dimensions>& result)throw()
			{
				result.v[Current] = vmin( v.v[Current], w.v[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::min(v, w, result);
			}

		//now implementing template definition 'void vectorSort (2..4) (<[*] v>, <[*] w>) direct=1'
		/**
			@brief Swaps an element of @a v with the respective element of @a w if it is greater. Once done each element of @a v is less or equal than the respective one of @a w<br>
			<br>
			vectorSort() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] v 
			@param[out] w 
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	vectorSort(TVec<T0,Dimensions>& v, TVec<T1,Dimensions>& w)throw()
			{
				if (w.v[Current]<v.v[Current])
					swp(v.v[Current],w.v[Current]);
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::vectorSort(v, w);
			}

		//now implementing template definition 'void copy|c (2..16) (<const [*] origin>, <[*] destination>) direct=1'
		/**
			@brief Copies elements from @a origin to @a destination<br>
			<br>
			copy() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] origin array to copy from
			@param[out] destination array to copy to
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	copy(const TVec<T0,Dimensions>& origin, TVec<T1,Dimensions>& destination)throw()
			{
				destination.v[Current] = (T1)origin.v[Current];
				VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::copy(origin, destination);
			}


		/* ----- Now implementing pointer based recursion-indirect functions ----- */
		//now implementing template definition 'void interpolate (2..4) (<const [*] v>, <const [*] w>, <f>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			interpolate() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] w 
			@param[in] f 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	interpolate(const T0 v[Dimensions], const T1 w[Dimensions], T2 f, T3 result[Dimensions])throw()
			{
				T3 i_ = T3(1)-f;
				VecV_Include__<0,Dimensions,false>::User16_interpolate(i_, v, w, f, result);
			}

		//now implementing template definition 'T0 dot (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Calculates the dot product of @a v and @a w<br>
			<br>
			dot() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	dot(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator17_sum(v, w)/*v:i*w:i*/;
			}

		//now implementing template definition 'T0 dot (2..4) (<const [*] v>) direct='
		/**
			@brief Calculates the dot product @a v * @a v<br>
			<br>
			dot() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	dot(const T0 v[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator18_sum(v)/*v:i*v:i*/;
			}

		//now implementing template definition 'T0 sum (2..4) (<const [*] v>) direct='
		/**
			@brief <br>
			<br>
			sum() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	sum(const T0 v[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator19_sum(v)/*v:i*/;
			}

		//now implementing template definition 'bool zero (2..4) (<const [*] v>) direct='
		/**
			@brief <br>
			<br>
			zero() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	bool	__fastcall	zero(const T0 v[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator20_and(v)/*!v:i*/;
			}

		//now implementing template definition 'T0 length (2..4) (<const [*] v>) direct='
		/**
			@brief Determines the length of @a v<br>
			<br>
			length() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v Vector to determine the length of
			@return Length of <paramref>v</paramref> 
		*/
		template <typename T0>
			static	inline	T0	__fastcall	length(const T0 v[Dimensions])throw()
			{
				return vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(v)/*v:i*v:i*/));
			}

		//now implementing template definition 'T0 quadraticDistance (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief <br>
			<br>
			quadraticDistance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	quadraticDistance(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator21_sum(v, w)/*sqr(v:i - w:i)*/;
			}

		//now implementing template definition 'T0 distance (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Calculates the distance between @a v0 and @a v1<br>
			<br>
			distance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	distance(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return vsqrt( (VecV_Include__<0,Dimensions,false>::Operator21_sum(v, w)/*sqr(v:i - w:i)*/) );
			}

		//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
			<br>
			reflectN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[in] p The point being reflected
			@param[out] out [out] Result
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	reflectN(const T0 base[Dimensions], const T1 normal[Dimensions], const T2 p[Dimensions], T3 out[Dimensions])throw()
			{
				T3 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator17_sum(base, normal)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator17_sum(p, normal)/*p:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={p, normal, fc, out}...
				{
					out[Current] = p[Current] + normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
			<br>
			reflectN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[out] p [inout] The point being reflected
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflectN(const T0 base[Dimensions], const T1 normal[Dimensions], T2 p[Dimensions])throw()
			{
				T2 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator17_sum(base, normal)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator17_sum(p, normal)/*p:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={p, normal, fc}...
				{
					p[Current] += normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc);
				};
			}

		//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
			<br>
			reflect() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[in] p The point being reflected
			@param[out] out [out] Result
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	reflect(const T0 base[Dimensions], const T1 normal[Dimensions], const T2 p[Dimensions], T3 out[Dimensions])throw()
			{
				T3 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator17_sum(base, normal)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator17_sum(p, normal)/*p:i*normal:i*/) ) / (VecV_Include__<0,Dimensions,false>::Operator18_sum(normal)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={p, normal, fc, out}...
				{
					out[Current] = p[Current] + normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
			<br>
			reflect() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[out] p The point being reflected
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflect(const T0 base[Dimensions], const T1 normal[Dimensions], T2 p[Dimensions])throw()
			{
				T2 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator17_sum(base, normal)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator17_sum(p, normal)/*p:i*normal:i*/) ) / (VecV_Include__<0,Dimensions,false>::Operator18_sum(normal)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={p, normal, fc}...
				{
					p[Current] += normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc);
				};
			}

		//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVectorN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[in] v The vector being reflected. May be of any length.
			@param[out] out Reflection result
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflectVectorN(const T0 normal[Dimensions], const T1 v[Dimensions], T2 out[Dimensions])throw()
			{
				T2 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator17_sum(v, normal)/*v:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={v, normal, fc, out}...
				{
					out[Current] = v[Current] + normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <[*] v>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVectorN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[out] v [inout] The vector being reflected. May be of any length.
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	reflectVectorN(const T0 normal[Dimensions], T1 v[Dimensions])throw()
			{
				T1 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator17_sum(v, normal)/*v:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={v, normal, fc}...
				{
					v[Current] += normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc);
				};
			}

		//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVector() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[in] v The vector being reflected. May be of any length.
			@param[out] out [out] Reflection result
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflectVector(const T0 normal[Dimensions], const T1 v[Dimensions], T2 out[Dimensions])throw()
			{
				T2 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator17_sum(v, normal)/*v:i*normal:i*/) )/(VecV_Include__<0,Dimensions,false>::Operator18_sum(normal)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={v, normal, fc, out}...
				{
					out[Current] = v[Current] + normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <[*] v>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVector() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[out] v [inout] The vector being reflected. May be of any length.
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	reflectVector(const T0 normal[Dimensions], T1 v[Dimensions])throw()
			{
				T1 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator17_sum(v, normal)/*v:i*normal:i*/) )/(VecV_Include__<0,Dimensions,false>::Operator18_sum(normal)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={v, normal, fc}...
				{
					v[Current] += normal[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc);
				};
			}

		//now implementing template definition 'void resolveUCBS (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveUCBS() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveUCBS(const T0 p0[Dimensions], const T1 p1[Dimensions], const T2 p2[Dimensions], const T3 p3[Dimensions], T4 t, T5 result[Dimensions])throw()
			{
				T5	//i = 1-t,
						f0 = (-t*t*t+3*t*t-3*t+1)/6,
						f1 = (3*t*t*t-6*t*t+4)/6,
						f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
						f3 = (t*t*t)/6;
				VecV_Include__<0,Dimensions,false>::User22_resolveUCBS(f0, f1, f2, f3, p0, p1, p2, p3, result);
			}

		//now implementing template definition 'void resolveUCBSaxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveUCBSaxis() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveUCBSaxis(const T0 p0[Dimensions], const T1 p1[Dimensions], const T2 p2[Dimensions], const T3 p3[Dimensions], T4 t, T5 result[Dimensions])throw()
			{
				T5	//i = 1-t,
						f0 = (t*t-2*t+1)/2,
						f1 = (-2*t*t+2*t+1)/2,
						f2 = (t*t)/2;
				VecV_Include__<0,Dimensions,false>::User23_resolveUCBSaxis(f0, f1, f2, p0, p1, p2, p3, result);
			}

		//now implementing template definition 'void resolveBezierCurvePoint (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveBezierCurvePoint() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveBezierCurvePoint(const T0 p0[Dimensions], const T1 p1[Dimensions], const T2 p2[Dimensions], const T3 p3[Dimensions], T4 t, T5 result[Dimensions])throw()
			{
				T5	i = 1-t,
						f3 = t*t*t,
						f2 = t*t*i*3,
						f1 = t*i*i*3,
						f0 = i*i*i;
				VecV_Include__<0,Dimensions,false>::User22_resolveUCBS(f0, f1, f2, f3, p0, p1, p2, p3, result);
			}

		//now implementing template definition 'void resolveBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveBezierCurveAxis() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveBezierCurveAxis(const T0 p0[Dimensions], const T1 p1[Dimensions], const T2 p2[Dimensions], const T3 p3[Dimensions], T4 t, T5 result[Dimensions])throw()
			{
				T5	i = 1-t,
						f2 = t*t,
						f1 = t*i*2,
						f0 = i*i;
				VecV_Include__<0,Dimensions,false>::User24_resolveBezierCurveAxis(f0, f1, f2, p0, p1, p2, p3, result);
				
				//block inlining void normalize0 (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='Pointers', parameters={result}...
				{
					T5 len = (VecV_Include__<0,Dimensions,false>::Operator18_sum(result)/*result:i*result:i*/);
					if (isnan(len) || len <= TypeInfo<T5>::error)
					{
						result[0] = 1;
						VecV_Include__<1,Dimensions,false>::User25_normalize0(result);
						return;
					}
					len = vsqrt(len);
					VecV_Include__<0,Dimensions,false>::User26_normalize0(result, len);
				};
			}

		//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>) direct='
		/**
			@brief Compares two vectors for lexicographic order<br>
			<br>
			compare() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v0 
			@param[in] v1 
			@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
		*/
		template <typename T0, typename T1>
			static	inline	char	__fastcall	compare(const T0 v0[Dimensions], const T1 v1[Dimensions])throw()
			{
				return compare3(v0,v1,(TypeInfo<T0>::error));
			}

		//now implementing template definition 'bool equal (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief <br>
			<br>
			equal() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	equal(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator27_and(v, w)/*v:i == w:i*/;
			}

		//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>, <tolerance>) direct='
		/**
			@brief Determines whether @a v and @a w are similar (within the specified tolerance proximity)<br>
			<br>
			similar() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] w 
			@param[in] tolerance maximum distance between <paramref>v</paramref> and <paramref>w</paramref> to still be considered similar
			@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar in accordance to the specified distance, false otherwise 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	bool	__fastcall	similar(const T0 v[Dimensions], const T1 w[Dimensions], T2 tolerance)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator28_sum(v, w, tolerance)/*sqr(v:i - w:i)*/ < tolerance*tolerance;
			}

		//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines whether @a v and @a w are similar (within a type-specific tolerance proximity)<br>
			<br>
			similar() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar, false otherwise 
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	similar(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return (VecV_Include__<0,Dimensions,false>::Operator28_sum(v, w, (TypeInfo<T0>::error))/*sqr(v:i - w:i)*/ < (TypeInfo<T0>::error)*(TypeInfo<T0>::error));
			}

		//now implementing template definition 'T0 maxAxisDistance (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines the greatest difference along any axis between @a v and @a w<br>
			<br>
			maxAxisDistance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return Greatest axial distance between v and w 
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	maxAxisDistance(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::Functional29_max(v, w)/*vabs(v:i-w:i)*/;
			}

		//now implementing template definition 'T0 planePointDistanceN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
		/**
			@brief Determines the absolute distance between a plane and a point. The specified normal is assumed to be of length 1<br>
			<br>
			planePointDistanceN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] base plane base point
			@param[in] normal plane normal; must be normalized
			@param[in] p point to determine the distance of
			@return absolute distance 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	T0	__fastcall	planePointDistanceN(const T0 base[Dimensions], const T1 normal[Dimensions], const T2 p[Dimensions])throw()
			{
				return ((VecV_Include__<0,Dimensions,false>::Operator17_sum(p, normal)/*p:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator17_sum(base, normal)/*base:i*normal:i*/));
			}

		//now implementing template definition 'T0 planePointDistance (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
		/**
			@brief Determines the absolute distance between a plane and a point<br>
			<br>
			planePointDistance() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] base plane base point
			@param[in] normal plane normal; may be of any length greater 0
			@param[in] p point to determine the distance of
			@return absolute distance 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	T0	__fastcall	planePointDistance(const T0 base[Dimensions], const T1 normal[Dimensions], const T2 p[Dimensions])throw()
			{
				return ((VecV_Include__<0,Dimensions,false>::Operator17_sum(p, normal)/*p:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator17_sum(base, normal)/*base:i*normal:i*/)) / (vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(normal)/*normal:i*normal:i*/)));
			}

		//now implementing template definition 'void scaleAbsolute (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
		/**
			@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Behavior is undefined if @a center and @a current are identical.<br>
			<br>
			scaleAbsolute() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] distance 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scaleAbsolute(const T0 center[Dimensions], T1 distance, T2 current[Dimensions])throw()
			{
				T2 len = (vsqrt( (VecV_Include__<0,Dimensions,false>::Operator21_sum(current, center)/*sqr(current:i - center:i)*/) ));
				
				//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={center, (distance/len), current}...
				{
					current[Current] = center[Current]+(current[Current]-center[Current])*(distance/len);
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::scale(center, (distance/len), current);
				};
			}

		//now implementing template definition 'void scaleAbsolute0 (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
		/**
			@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Also checks if @a center and @a current are identical and reacts appropriately.<br>
			<br>
			scaleAbsolute0() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] distance 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scaleAbsolute0(const T0 center[Dimensions], T1 distance, T2 current[Dimensions])throw()
			{
				T2 len = (vsqrt( (VecV_Include__<0,Dimensions,false>::Operator21_sum(current, center)/*sqr(current:i - center:i)*/) ));
				if (len > TypeInfo<T2>::error)
				{
					
					//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={center, (distance/len), current}...
					{
						current[Current] = center[Current]+(current[Current]-center[Current])*(distance/len);
						VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::scale(center, (distance/len), current);
					};
				}
				else
					current[0] = len;
			}

		//now implementing template definition 'void normalize (2..4) (<[*] vector>) direct='
		/**
			@brief <br>
			<br>
			normalize() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] vector 
		*/
		template <typename T0>
			static	inline	void	__fastcall	normalize(T0 vector[Dimensions])throw()
			{
				T0 len = vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(vector)/*vector:i*vector:i*/));
				VecV_Include__<0,Dimensions,false>::User26_normalize0(vector, len);
			}

		//now implementing template definition 'void normalize0 (2..4) (<[*] vector>) direct='
		/**
			@brief <br>
			<br>
			normalize0() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[out] vector 
		*/
		template <typename T0>
			static	inline	void	__fastcall	normalize0(T0 vector[Dimensions])throw()
			{
				T0 len = (VecV_Include__<0,Dimensions,false>::Operator18_sum(vector)/*vector:i*vector:i*/);
				if (isnan(len) || len <= TypeInfo<T0>::error)
				{
					vector[0] = 1;
					VecV_Include__<1,Dimensions,false>::User25_normalize0(vector);
					return;
				}
				len = vsqrt(len);
				VecV_Include__<0,Dimensions,false>::User26_normalize0(vector, len);
			}

		//now implementing template definition 'void setLen (2..4) (<[*] current>, <length>) direct='
		/**
			@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
			<br>
			setLen() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current vector to normalize, must not be of length 0
			@param[in] length new length
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	setLen(T0 current[Dimensions], T1 length)throw()
			{
				T0 len = vsqrt( (VecV_Include__<0,Dimensions,false>::Operator18_sum(current)/*current:i*current:i*/) );
				T0 fc = T0(length)/len;
				
				//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={current, fc}...
				{
					current[Current] *= fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(current, fc);
				};
			}

		//now implementing template definition 'void setLen0 (2..4) (<[*] current>, <length>) direct='
		/**
			@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
			<br>
			setLen0() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current vector to normalize, must not be of length 0
			@param[in] length new length
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	setLen0(T0 current[Dimensions], T1 length)throw()
			{
				T0 len = vsqrt( (VecV_Include__<0,Dimensions,false>::Operator18_sum(current)/*current:i*current:i*/) );
				if (vabs(len) > TypeInfo<T0>::error)
				{
					T0 fc = T0(length)/len;
					
					//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={current, fc}...
					{
						current[Current] *= fc;
						VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(current, fc);
					};
				}
				else
					current[0] = len;
			}

		//now implementing template definition 'T0 angle (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines the angle between @a v and @a w<br>
			<br>
			angle() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v first vector; not required to be normalized, but must be non-0
			@param[in] w second vector; not required to be normalized, but must be non-0
			@return angle in the range [0,180] 
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	angle(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return acos(Math::clamp((VecV_Include__<0,Dimensions,false>::Operator17_sum(v, w)/*v:i*w:i*/)/((vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(v)/*v:i*v:i*/)))*(vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(w)/*w:i*w:i*/)))),-1,1))*180/M_PI;
			}

		//now implementing template definition 'bool isNAN (2..4) (<const [*] v>) direct='
		/**
			@brief Tests if at least one component of @a v is NAN (Not A Number). Not applicable to integer types<br>
			<br>
			isNAN() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	bool	__fastcall	isNAN(const T0 v[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator30_or(v)/*isnan(v:i)*/;
			}

		//now implementing template definition 'T0 intensity (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines the light intensity (the cos of the angle) between @a v and @a w<br>
			<br>
			intensity() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v first vector; not required to be normalized, but must be non-0
			@param[in] w second vector; not required to be normalized, but must be non-0
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	intensity(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return (VecV_Include__<0,Dimensions,false>::Operator17_sum(v, w)/*v:i*w:i*/)/((vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(v)/*v:i*v:i*/)))*(vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(w)/*w:i*w:i*/))));
			}

		//now implementing template definition 'T0 min (2..4) (<const [*] v>) direct='
		/**
			@brief Determines the minimum coordinate value of @a v<br>
			<br>
			min() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	min(const T0 v[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::Functional31_min(v)/*v:i*/;
			}

		//now implementing template definition 'T0 max (2..4) (<const [*] v>) direct='
		/**
			@brief Determines the maximum coordinate value of @a v<br>
			<br>
			max() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	max(const T0 v[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::Functional32_max(v)/*v:i*/;
			}

		//now implementing template definition 'bool oneLess (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if at least one element of @a v is less than the respective element of @a w<br>
			<br>
			oneLess() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	oneLess(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator33_or(v, w)/*v:i < w:i*/;
			}

		//now implementing template definition 'bool oneGreater (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if at least one element of @a v is greater than the respective element of @a w<br>
			<br>
			oneGreater() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	oneGreater(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator34_or(v, w)/*v:i > w:i*/;
			}

		//now implementing template definition 'bool allLess (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if all elements of @a v are less than the respective element of @a w<br>
			<br>
			allLess() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	allLess(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator35_and(v, w)/*v:i < w:i*/;
			}

		//now implementing template definition 'bool allGreater (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if all elements of @a v are greater than the respective element of @a w<br>
			<br>
			allGreater() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	allGreater(const T0 v[Dimensions], const T1 w[Dimensions])throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator36_and(v, w)/*v:i > w:i*/;
			}

		//now implementing template definition 'T3 intercept (2..4) (<const [*] position>, <const [*] velocity>, <interception_velocity>, <[*] result>) direct='
		/**
			@brief Calculates the time and direction of interception of an object at @a position moving with a specific constant speed and direction @a velocity<br>
			<br>
			intercept() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] position 
			@param[in] velocity 
			@param[in] interception_velocity 
			@param[out] result normalized direction of intersection, if any; the value of this variable remains unchanged if no interseption could be determined
			@return time of intersection, or 0 if no such could be determined 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	T3	__fastcall	intercept(const T0 position[Dimensions], const T1 velocity[Dimensions], T2 interception_velocity, T3 result[Dimensions])throw()
			{
				T3	rs[2],
						a = (VecV_Include__<0,Dimensions,false>::Operator18_sum(velocity)/*velocity:i*velocity:i*/) - interception_velocity*interception_velocity,
						b = 2*(VecV_Include__<0,Dimensions,false>::Operator17_sum(position, velocity)/*position:i*velocity:i*/),
						c = (VecV_Include__<0,Dimensions,false>::Operator18_sum(position)/*position:i*position:i*/);
				BYTE num_rs = solveSqrEquation(a, b, c, rs);
				if (!num_rs)
					return 0;
				T3	t = smallestPositiveResult(rs,num_rs);
				if (t<=0)
					return 0;
				T3 f = T3(1)/t;
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Pointers', parameters={velocity, position, f, result}...
				{
					result[Current] = velocity[Current] + position[Current] * f;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(velocity, position, f, result);
				};
				
				//block inlining void normalize (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='Pointers', parameters={result}...
				{
					T3 len = vsqrt((VecV_Include__<0,Dimensions,false>::Operator18_sum(result)/*result:i*result:i*/));
					VecV_Include__<0,Dimensions,false>::User26_normalize0(result, len);
				};
				return t;
			}

		//now implementing template definition 'String toString (2..4) (<const [*] vector>) direct='
		/**
			@brief <br>
			<br>
			toString() requires array pointers to operate on. Make sure all passed array pointers provide at least as many elements as required (<Dimensions> in general for this version)<br>
		
			@param[in] vector 
			@return  
		*/
		template <typename T0>
			static	inline	String	__fastcall	toString(const T0 vector[Dimensions])throw()
			{
				return '('+ VecV_Include__<0,Dimensions,false>::Operator37_glue(vector)/*", "*/+')';
			}


		/* ----- Now implementing object based recursion-indirect functions ----- */
		//now implementing template definition 'void interpolate (2..4) (<const [*] v>, <const [*] w>, <f>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			interpolate() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] w 
			@param[in] f 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	interpolate(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, T2 f, TVec<T3,Dimensions>& result)throw()
			{
				T3 i_ = T3(1)-f;
				VecV_Include__<0,Dimensions,false>::User38_interpolate(i_, v.v, w.v, f, result.v);
			}

		//now implementing template definition 'T0 dot (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Calculates the dot product of @a v and @a w<br>
			<br>
			dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	dot(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator39_sum(v.v, w.v)/*v:i*w:i*/;
			}

		//now implementing template definition 'T0 dot (2..4) (<const [*] v>) direct='
		/**
			@brief Calculates the dot product @a v * @a v<br>
			<br>
			dot() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	dot(const TVec<T0,Dimensions>& v)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator40_sum(v.v)/*v:i*v:i*/;
			}

		//now implementing template definition 'T0 sum (2..4) (<const [*] v>) direct='
		/**
			@brief <br>
			<br>
			sum() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	sum(const TVec<T0,Dimensions>& v)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator41_sum(v.v)/*v:i*/;
			}

		//now implementing template definition 'bool zero (2..4) (<const [*] v>) direct='
		/**
			@brief <br>
			<br>
			zero() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	bool	__fastcall	zero(const TVec<T0,Dimensions>& v)throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator42_and(v.v)/*!v:i*/;
			}

		//now implementing template definition 'T0 length (2..4) (<const [*] v>) direct='
		/**
			@brief Determines the length of @a v<br>
			<br>
			length() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v Vector to determine the length of
			@return Length of <paramref>v</paramref> 
		*/
		template <typename T0>
			static	inline	T0	__fastcall	length(const TVec<T0,Dimensions>& v)throw()
			{
				return vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(v.v)/*v:i*v:i*/));
			}

		//now implementing template definition 'T0 quadraticDistance (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief <br>
			<br>
			quadraticDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	quadraticDistance(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator43_sum(v.v, w.v)/*sqr(v:i - w:i)*/;
			}

		//now implementing template definition 'T0 distance (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Calculates the distance between @a v0 and @a v1<br>
			<br>
			distance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	distance(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return vsqrt( (VecV_Include__<0,Dimensions,false>::Operator43_sum(v.v, w.v)/*sqr(v:i - w:i)*/) );
			}

		//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
			<br>
			reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[in] p The point being reflected
			@param[out] out [out] Result
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	reflectN(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& normal, const TVec<T2,Dimensions>& p, TVec<T3,Dimensions>& out)throw()
			{
				T3 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator39_sum(base.v, normal.v)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator39_sum(p.v, normal.v)/*p:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc, out}...
				{
					out.v[Current] = p.v[Current] + normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflectN (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and (normalized) normal<br>
			<br>
			reflectN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[out] p [inout] The point being reflected
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflectN(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& normal, TVec<T2,Dimensions>& p)throw()
			{
				T2 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator39_sum(base.v, normal.v)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator39_sum(p.v, normal.v)/*p:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc}...
				{
					p.v[Current] += normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc);
				};
			}

		//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
			<br>
			reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[in] p The point being reflected
			@param[out] out [out] Result
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	void	__fastcall	reflect(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& normal, const TVec<T2,Dimensions>& p, TVec<T3,Dimensions>& out)throw()
			{
				T3 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator39_sum(base.v, normal.v)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator39_sum(p.v, normal.v)/*p:i*normal:i*/) ) / (VecV_Include__<0,Dimensions,false>::Operator40_sum(normal.v)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc, out}...
				{
					out.v[Current] = p.v[Current] + normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflect (2..4) (<const [*] base>, <const [*] normal>, <[*] p>) direct='
		/**
			@brief Calculates the reflection of a point across the plane, specified by base and normal<br>
			<br>
			reflect() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] base The base point of the reflecting surface
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[out] p The point being reflected
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflect(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& normal, TVec<T2,Dimensions>& p)throw()
			{
				T2 fc = 2*( (VecV_Include__<0,Dimensions,false>::Operator39_sum(base.v, normal.v)/*base:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator39_sum(p.v, normal.v)/*p:i*normal:i*/) ) / (VecV_Include__<0,Dimensions,false>::Operator40_sum(normal.v)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={p, normal, fc}...
				{
					p.v[Current] += normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(p, normal, fc);
				};
			}

		//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[in] v The vector being reflected. May be of any length.
			@param[out] out Reflection result
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflectVectorN(const TVec<T0,Dimensions>& normal, const TVec<T1,Dimensions>& v, TVec<T2,Dimensions>& out)throw()
			{
				T2 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator39_sum(v.v, normal.v)/*v:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc, out}...
				{
					out.v[Current] = v.v[Current] + normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflectVectorN (2..4) (<const [*] normal>, <[*] v>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVectorN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] normal The normal of the reflecting surface. Must be normalized
			@param[out] v [inout] The vector being reflected. May be of any length.
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	reflectVectorN(const TVec<T0,Dimensions>& normal, TVec<T1,Dimensions>& v)throw()
			{
				T1 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator39_sum(v.v, normal.v)/*v:i*normal:i*/) );
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc}...
				{
					v.v[Current] += normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc);
				};
			}

		//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <const [*] v>, <[*] out>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[in] v The vector being reflected. May be of any length.
			@param[out] out [out] Reflection result
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	reflectVector(const TVec<T0,Dimensions>& normal, const TVec<T1,Dimensions>& v, TVec<T2,Dimensions>& out)throw()
			{
				T2 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator39_sum(v.v, normal.v)/*v:i*normal:i*/) )/(VecV_Include__<0,Dimensions,false>::Operator40_sum(normal.v)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc, out}...
				{
					out.v[Current] = v.v[Current] + normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc, out);
				};
			}

		//now implementing template definition 'void reflectVector (2..4) (<const [*] normal>, <[*] v>) direct='
		/**
			@brief Calculates the reflection of a direction vector across the plane, specified by its (normalized) normal<br>
			<br>
			reflectVector() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] normal The normal of the reflecting surface. May be of any length > 0
			@param[out] v [inout] The vector being reflected. May be of any length.
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	reflectVector(const TVec<T0,Dimensions>& normal, TVec<T1,Dimensions>& v)throw()
			{
				T1 fc = 2*( - (VecV_Include__<0,Dimensions,false>::Operator39_sum(v.v, normal.v)/*v:i*normal:i*/) )/(VecV_Include__<0,Dimensions,false>::Operator40_sum(normal.v)/*normal:i*normal:i*/);
				
				//block inlining void multAdd|mad (2..4) (<[*] current>, <const [*] vector>, <scalar>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={v, normal, fc}...
				{
					v.v[Current] += normal.v[Current] * fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(v, normal, fc);
				};
			}

		//now implementing template definition 'void resolveUCBS (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveUCBS() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveUCBS(const TVec<T0,Dimensions>& p0, const TVec<T1,Dimensions>& p1, const TVec<T2,Dimensions>& p2, const TVec<T3,Dimensions>& p3, T4 t, TVec<T5,Dimensions>& result)throw()
			{
				T5	//i = 1-t,
						f0 = (-t*t*t+3*t*t-3*t+1)/6,
						f1 = (3*t*t*t-6*t*t+4)/6,
						f2 = (-3*t*t*t+3*t*t+3*t+1)/6,
						f3 = (t*t*t)/6;
				VecV_Include__<0,Dimensions,false>::User44_resolveUCBS(f0, f1, f2, f3, p0.v, p1.v, p2.v, p3.v, result.v);
			}

		//now implementing template definition 'void resolveUCBSaxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveUCBSaxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveUCBSaxis(const TVec<T0,Dimensions>& p0, const TVec<T1,Dimensions>& p1, const TVec<T2,Dimensions>& p2, const TVec<T3,Dimensions>& p3, T4 t, TVec<T5,Dimensions>& result)throw()
			{
				T5	//i = 1-t,
						f0 = (t*t-2*t+1)/2,
						f1 = (-2*t*t+2*t+1)/2,
						f2 = (t*t)/2;
				VecV_Include__<0,Dimensions,false>::User45_resolveUCBSaxis(f0, f1, f2, p0.v, p1.v, p2.v, p3.v, result.v);
			}

		//now implementing template definition 'void resolveBezierCurvePoint (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveBezierCurvePoint() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveBezierCurvePoint(const TVec<T0,Dimensions>& p0, const TVec<T1,Dimensions>& p1, const TVec<T2,Dimensions>& p2, const TVec<T3,Dimensions>& p3, T4 t, TVec<T5,Dimensions>& result)throw()
			{
				T5	i = 1-t,
						f3 = t*t*t,
						f2 = t*t*i*3,
						f1 = t*i*i*3,
						f0 = i*i*i;
				VecV_Include__<0,Dimensions,false>::User44_resolveUCBS(f0, f1, f2, f3, p0.v, p1.v, p2.v, p3.v, result.v);
			}

		//now implementing template definition 'void resolveBezierCurveAxis (2..4) (<const [*] p0>, <const [*] p1>, <const [*] p2>, <const [*] p3>, <t>, <[*] result>) direct='
		/**
			@brief <br>
			<br>
			resolveBezierCurveAxis() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] p0 
			@param[in] p1 
			@param[in] p2 
			@param[in] p3 
			@param[in] t 
			@param[out] result 
		*/
		template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
			static	inline	void	__fastcall	resolveBezierCurveAxis(const TVec<T0,Dimensions>& p0, const TVec<T1,Dimensions>& p1, const TVec<T2,Dimensions>& p2, const TVec<T3,Dimensions>& p3, T4 t, TVec<T5,Dimensions>& result)throw()
			{
				T5	i = 1-t,
						f2 = t*t,
						f1 = t*i*2,
						f0 = i*i;
				VecV_Include__<0,Dimensions,false>::User46_resolveBezierCurveAxis(f0, f1, f2, p0.v, p1.v, p2.v, p3.v, result.v);
				
				//block inlining void normalize0 (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='Objects', parameters={result}...
				{
					T5 len = (VecV_Include__<0,Dimensions,false>::Operator40_sum(result.v)/*result:i*result:i*/);
					if (isnan(len) || len <= TypeInfo<T5>::error)
					{
						result.x = 1;
						VecV_Include__<1,Dimensions,false>::User47_normalize0(result.v);
						return;
					}
					len = vsqrt(len);
					VecV_Include__<0,Dimensions,false>::User48_normalize0(result.v, len);
				};
			}

		//now implementing template definition 'char compare (2..16) (<const [*] v0>, <const [*] v1>) direct='
		/**
			@brief Compares two vectors for lexicographic order<br>
			<br>
			compare() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v0 
			@param[in] v1 
			@return -1 if <paramref>v0</paramref> is lexicographically less than <paramref>v1</paramref>, 0 if they are identical, +1 if <paramref>v0</paramref> is lexicographically greater than <paramref>v1</paramref> 
		*/
		template <typename T0, typename T1>
			static	inline	char	__fastcall	compare(const TVec<T0,Dimensions>& v0, const TVec<T1,Dimensions>& v1)throw()
			{
				return compare(v0,v1,(TypeInfo<T0>::error));
			}

		//now implementing template definition 'bool equal (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief <br>
			<br>
			equal() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	equal(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator49_and(v.v, w.v)/*v:i == w:i*/;
			}

		//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>, <tolerance>) direct='
		/**
			@brief Determines whether @a v and @a w are similar (within the specified tolerance proximity)<br>
			<br>
			similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] v 
			@param[in] w 
			@param[in] tolerance maximum distance between <paramref>v</paramref> and <paramref>w</paramref> to still be considered similar
			@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar in accordance to the specified distance, false otherwise 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	bool	__fastcall	similar(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w, T2 tolerance)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Operator50_sum(v.v, w.v, tolerance)/*sqr(v:i - w:i)*/ < tolerance*tolerance;
			}

		//now implementing template definition 'bool similar (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines whether @a v and @a w are similar (within a type-specific tolerance proximity)<br>
			<br>
			similar() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return true, if <paramref>v</paramref> and <paramref>w</paramref> are similar, false otherwise 
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	similar(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return (VecV_Include__<0,Dimensions,false>::Operator50_sum(v.v, w.v, (TypeInfo<T0>::error))/*sqr(v:i - w:i)*/ < (TypeInfo<T0>::error)*(TypeInfo<T0>::error));
			}

		//now implementing template definition 'T0 maxAxisDistance (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines the greatest difference along any axis between @a v and @a w<br>
			<br>
			maxAxisDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return Greatest axial distance between v and w 
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	maxAxisDistance(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Functional51_max(v.v, w.v)/*vabs(v:i-w:i)*/;
			}

		//now implementing template definition 'T0 planePointDistanceN (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
		/**
			@brief Determines the absolute distance between a plane and a point. The specified normal is assumed to be of length 1<br>
			<br>
			planePointDistanceN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] base plane base point
			@param[in] normal plane normal; must be normalized
			@param[in] p point to determine the distance of
			@return absolute distance 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	T0	__fastcall	planePointDistanceN(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& normal, const TVec<T2,Dimensions>& p)throw()
			{
				return ((VecV_Include__<0,Dimensions,false>::Operator39_sum(p.v, normal.v)/*p:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator39_sum(base.v, normal.v)/*base:i*normal:i*/));
			}

		//now implementing template definition 'T0 planePointDistance (2..4) (<const [*] base>, <const [*] normal>, <const [*] p>) direct='
		/**
			@brief Determines the absolute distance between a plane and a point<br>
			<br>
			planePointDistance() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] base plane base point
			@param[in] normal plane normal; may be of any length greater 0
			@param[in] p point to determine the distance of
			@return absolute distance 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	T0	__fastcall	planePointDistance(const TVec<T0,Dimensions>& base, const TVec<T1,Dimensions>& normal, const TVec<T2,Dimensions>& p)throw()
			{
				return ((VecV_Include__<0,Dimensions,false>::Operator39_sum(p.v, normal.v)/*p:i*normal:i*/) - (VecV_Include__<0,Dimensions,false>::Operator39_sum(base.v, normal.v)/*base:i*normal:i*/)) / (vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(normal.v)/*normal:i*normal:i*/)));
			}

		//now implementing template definition 'void scaleAbsolute (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
		/**
			@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Behavior is undefined if @a center and @a current are identical.<br>
			<br>
			scaleAbsolute() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] distance 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scaleAbsolute(const TVec<T0,Dimensions>& center, T1 distance, TVec<T2,Dimensions>& current)throw()
			{
				T2 len = (vsqrt( (VecV_Include__<0,Dimensions,false>::Operator43_sum(current.v, center.v)/*sqr(current:i - center:i)*/) ));
				
				//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={center, (distance/len), current}...
				{
					current.v[Current] = center.v[Current]+(current.v[Current]-center.v[Current])*(distance/len);
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::scale(center, (distance/len), current);
				};
			}

		//now implementing template definition 'void scaleAbsolute0 (2..4) (<const [*] center>, <distance>, <[*] current>) direct='
		/**
			@brief Scales the distance between @a center and @a current to the specified absolute @a distance. Also checks if @a center and @a current are identical and reacts appropriately.<br>
			<br>
			scaleAbsolute0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] center 
			@param[in] distance 
			@param[out] current 
		*/
		template <typename T0, typename T1, typename T2>
			static	inline	void	__fastcall	scaleAbsolute0(const TVec<T0,Dimensions>& center, T1 distance, TVec<T2,Dimensions>& current)throw()
			{
				T2 len = (vsqrt( (VecV_Include__<0,Dimensions,false>::Operator43_sum(current.v, center.v)/*sqr(current:i - center:i)*/) ));
				if (len > TypeInfo<T2>::error)
				{
					
					//block inlining void scale (2..4) (<const [*] center>, <factor>, <[*] current>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={center, (distance/len), current}...
					{
						current.v[Current] = center.v[Current]+(current.v[Current]-center.v[Current])*(distance/len);
						VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::scale(center, (distance/len), current);
					};
				}
				else
					current[0] = len;
			}

		//now implementing template definition 'void normalize (2..4) (<[*] vector>) direct='
		/**
			@brief <br>
			<br>
			normalize() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] vector 
		*/
		template <typename T0>
			static	inline	void	__fastcall	normalize(TVec<T0,Dimensions>& vector)throw()
			{
				T0 len = vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(vector.v)/*vector:i*vector:i*/));
				VecV_Include__<0,Dimensions,false>::User48_normalize0(vector.v, len);
			}

		//now implementing template definition 'void normalize0 (2..4) (<[*] vector>) direct='
		/**
			@brief <br>
			<br>
			normalize0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[out] vector 
		*/
		template <typename T0>
			static	inline	void	__fastcall	normalize0(TVec<T0,Dimensions>& vector)throw()
			{
				T0 len = (VecV_Include__<0,Dimensions,false>::Operator40_sum(vector.v)/*vector:i*vector:i*/);
				if (isnan(len) || len <= TypeInfo<T0>::error)
				{
					vector.x = 1;
					VecV_Include__<1,Dimensions,false>::User47_normalize0(vector.v);
					return;
				}
				len = vsqrt(len);
				VecV_Include__<0,Dimensions,false>::User48_normalize0(vector.v, len);
			}

		//now implementing template definition 'void setLen (2..4) (<[*] current>, <length>) direct='
		/**
			@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
			<br>
			setLen() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current vector to normalize, must not be of length 0
			@param[in] length new length
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	setLen(TVec<T0,Dimensions>& current, T1 length)throw()
			{
				T0 len = vsqrt( (VecV_Include__<0,Dimensions,false>::Operator40_sum(current.v)/*current:i*current:i*/) );
				T0 fc = T0(length)/len;
				
				//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={current, fc}...
				{
					current.v[Current] *= fc;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(current, fc);
				};
			}

		//now implementing template definition 'void setLen0 (2..4) (<[*] current>, <length>) direct='
		/**
			@brief Updates the length of @a current so that its length matches @a length. @a current may be of any length greater 0<br>
			<br>
			setLen0() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[out] current vector to normalize, must not be of length 0
			@param[in] length new length
		*/
		template <typename T0, typename T1>
			static	inline	void	__fastcall	setLen0(TVec<T0,Dimensions>& current, T1 length)throw()
			{
				T0 len = vsqrt( (VecV_Include__<0,Dimensions,false>::Operator40_sum(current.v)/*current:i*current:i*/) );
				if (vabs(len) > TypeInfo<T0>::error)
				{
					T0 fc = T0(length)/len;
					
					//block inlining void multiply|mult|mul (2..4) (<[*] v>, <factor>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={current, fc}...
					{
						current.v[Current] *= fc;
						VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multiply(current, fc);
					};
				}
				else
					current.x = len;
			}

		//now implementing template definition 'T0 angle (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines the angle between @a v and @a w<br>
			<br>
			angle() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v first vector; not required to be normalized, but must be non-0
			@param[in] w second vector; not required to be normalized, but must be non-0
			@return angle in the range [0,180] 
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	angle(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return acos(Math::clamp((VecV_Include__<0,Dimensions,false>::Operator39_sum(v.v, w.v)/*v:i*w:i*/)/((vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(v.v)/*v:i*v:i*/)))*(vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(w.v)/*w:i*w:i*/)))),-1,1))*180/M_PI;
			}

		//now implementing template definition 'bool isNAN (2..4) (<const [*] v>) direct='
		/**
			@brief Tests if at least one component of @a v is NAN (Not A Number). Not applicable to integer types<br>
			<br>
			isNAN() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	bool	__fastcall	isNAN(const TVec<T0,Dimensions>& v)throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator52_or(v.v)/*isnan(v:i)*/;
			}

		//now implementing template definition 'T0 intensity (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Determines the light intensity (the cos of the angle) between @a v and @a w<br>
			<br>
			intensity() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v first vector; not required to be normalized, but must be non-0
			@param[in] w second vector; not required to be normalized, but must be non-0
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	T0	__fastcall	intensity(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return (VecV_Include__<0,Dimensions,false>::Operator39_sum(v.v, w.v)/*v:i*w:i*/)/((vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(v.v)/*v:i*v:i*/)))*(vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(w.v)/*w:i*w:i*/))));
			}

		//now implementing template definition 'T0 min (2..4) (<const [*] v>) direct='
		/**
			@brief Determines the minimum coordinate value of @a v<br>
			<br>
			min() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	min(const TVec<T0,Dimensions>& v)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Functional53_min(v.v)/*v:i*/;
			}

		//now implementing template definition 'T0 max (2..4) (<const [*] v>) direct='
		/**
			@brief Determines the maximum coordinate value of @a v<br>
			<br>
			max() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@return  
		*/
		template <typename T0>
			static	inline	T0	__fastcall	max(const TVec<T0,Dimensions>& v)throw()
			{
				return VecV_Include__<0,Dimensions,false>::Functional54_max(v.v)/*v:i*/;
			}

		//now implementing template definition 'bool oneLess (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if at least one element of @a v is less than the respective element of @a w<br>
			<br>
			oneLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	oneLess(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator55_or(v.v, w.v)/*v:i < w:i*/;
			}

		//now implementing template definition 'bool oneGreater (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if at least one element of @a v is greater than the respective element of @a w<br>
			<br>
			oneGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	oneGreater(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator56_or(v.v, w.v)/*v:i > w:i*/;
			}

		//now implementing template definition 'bool allLess (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if all elements of @a v are less than the respective element of @a w<br>
			<br>
			allLess() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	allLess(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator57_and(v.v, w.v)/*v:i < w:i*/;
			}

		//now implementing template definition 'bool allGreater (2..4) (<const [*] v>, <const [*] w>) direct='
		/**
			@brief Tests if all elements of @a v are greater than the respective element of @a w<br>
			<br>
			allGreater() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] v 
			@param[in] w 
			@return  
		*/
		template <typename T0, typename T1>
			static	inline	bool	__fastcall	allGreater(const TVec<T0,Dimensions>& v, const TVec<T1,Dimensions>& w)throw()
			{
				return VecV_Include__<0,Dimensions,false>::LogicOperator58_and(v.v, w.v)/*v:i > w:i*/;
			}

		//now implementing template definition 'T3 intercept (2..4) (<const [*] position>, <const [*] velocity>, <interception_velocity>, <[*] result>) direct='
		/**
			@brief Calculates the time and direction of interception of an object at @a position moving with a specific constant speed and direction @a velocity<br>
			<br>
			intercept() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
			Scalar values are passed as copies rather than by const reference. Use this version for primitive types only.<br>
		
			@param[in] position 
			@param[in] velocity 
			@param[in] interception_velocity 
			@param[out] result normalized direction of intersection, if any; the value of this variable remains unchanged if no interseption could be determined
			@return time of intersection, or 0 if no such could be determined 
		*/
		template <typename T0, typename T1, typename T2, typename T3>
			static	inline	T3	__fastcall	intercept(const TVec<T0,Dimensions>& position, const TVec<T1,Dimensions>& velocity, T2 interception_velocity, TVec<T3,Dimensions>& result)throw()
			{
				T3	rs[2],
						a = (VecV_Include__<0,Dimensions,false>::Operator40_sum(velocity.v)/*velocity:i*velocity:i*/) - interception_velocity*interception_velocity,
						b = 2*(VecV_Include__<0,Dimensions,false>::Operator39_sum(position.v, velocity.v)/*position:i*velocity:i*/),
						c = (VecV_Include__<0,Dimensions,false>::Operator40_sum(position.v)/*position:i*position:i*/);
				BYTE num_rs = solveSqrEquation(a, b, c, rs);
				if (!num_rs)
					return 0;
				T3	t = smallestPositiveResult(rs,num_rs);
				if (t<=0)
					return 0;
				T3 f = T3(1)/t;
				
				//block inlining void multAdd|mad (2..4) (<const [*] base>, <const [*] vector>, <scalar>, <[*] result>) direct=1 for dimensions=3, assembly_mode='Objects', parameters={velocity, position, f, result}...
				{
					result.v[Current] = velocity.v[Current] + position.v[Current] * f;
					VecV_Include__<Current+1,Dimensions,GreaterOrEqual<Current+2,Dimensions>::eval>::multAdd(velocity, position, f, result);
				};
				
				//block inlining void normalize (2..4) (<[*] vector>) direct= for dimensions=3, assembly_mode='Objects', parameters={result}...
				{
					T3 len = vsqrt((VecV_Include__<0,Dimensions,false>::Operator40_sum(result.v)/*result:i*result:i*/));
					VecV_Include__<0,Dimensions,false>::User48_normalize0(result.v, len);
				};
				return t;
			}

		//now implementing template definition 'String toString (2..4) (<const [*] vector>) direct='
		/**
			@brief <br>
			<br>
			toString() requires vector objects to operate on, rather than raw pointers. Use ref*() to create a temporary reference object to existing array pointers<br>
		
			@param[in] vector 
			@return  
		*/
		template <typename T0>
			static	inline	String	__fastcall	toString(const TVec<T0,Dimensions>& vector)throw()
			{
				return '('+ VecV_Include__<0,Dimensions,false>::Operator59_glue(vector.v)/*", "*/+')';
			}

	};

template <count_t Dimensions, count_t First=0>
	class VecUnroll:public VecV_Include__<First,Dimensions,GreaterOrEqual<First+1,Dimensions>::eval> 
	{};

#endif
