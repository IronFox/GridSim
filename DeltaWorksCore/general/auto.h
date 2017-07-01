#ifndef autoH
#define autoH


namespace DeltaWorks
{


	template <typename T, int Initial=0>
		class Auto
		{
		public:
			T					value;
	//static const T				Initial = init_value;
	
								Auto():value((T)Initial)
								{}
								Auto(const T&value_):value(value_)
								{}
			Auto<T,Initial>&	operator=(const Auto<T,Initial>&other)
								{
									value = other.value;
									return *this;
								}
			Auto<T,Initial>&	operator=(const T&other)
								{
									value = other;
									return *this;
								}
			T					operator->()
								{
									return value;
								}
			const T				operator->() const
								{
									return value;
								}
			
			operator T&()
			{
				return value;
			}
			
			operator T()	const
			{
				return value;
			}
		};



}

#endif
