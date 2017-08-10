#ifndef orthographic_comparatorH
#define orthographic_comparatorH

namespace DeltaWorks
{

	/**
		@brief Performs a sequential orthographic comparison between a number of objects

		The current value of the comparator is 0 for equal, and changes status with the first two different objects.
		When adding a comparison between two different objects for the first time, then the status changes to -1 if the first parameter was less than the second parameter and +1 otherwise.<br>
	

	*/
	class OrthographicComparison
	{
	public:
		typedef int					result_t;
		/**/						OrthographicComparison(result_t initial=0):current(initial)
		{}

		template <typename T0, typename T1>
		/**/						OrthographicComparison(const T0&item0, const T1&item1)
		{
			if (item0 > item1)
				current = 1;
			elif (item0 < item1)
				current = -1;
			else
				current = 0;
		}
	

		template <typename T0, typename T1>
			OrthographicComparison&	AddComparison(const T0&object0, const T1&object1)	//!< Adds the result of a comparison via operators of the two objects. No comparison is performed if the local comparison has already come to a decision
			{
				if (current == 0)
				{
					if (object0 > object1)
						current = 1;
					elif (object0 < object1)
						current = -1;
				}
				return *this;
			}
			
		OrthographicComparison&		AddComparisonResult(result_t result)	//! Adds the result of an external comparison
		{
			if (current == 0)
				current = result;
			return *this;
		}
		
		template <typename T0, typename T1>
			OrthographicComparison&	AddCompareTo(const T0&object0, const T1&object1)	//! Adds a comparison by compareTo() method invokation. @a object0 must implement a compareTo() method that accepts @a object1 as parameter. No comparison is performed if the local comparison has already come to a decision
			{
				if (current == 0)
					current = object0.compareTo(object1);
				return *this;
			}



		bool						IsDecided()	const	//!< Checks if a non-equal decision has been reached
									{
										return current != 0;
									}

		operator result_t() const	{return current;}

	private:
		result_t					current;
	};


}

#endif
