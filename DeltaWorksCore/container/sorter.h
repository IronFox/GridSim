#ifndef sorterH
#define sorterH

/******************************************************************

Generalized sorters for lists and fields.

******************************************************************/


namespace DeltaWorks
{

	namespace Sorting
	{
		/**
			@brief Data selector class

			The Accessor class defines access-type and -method for a container type.
			Common containers are passed as reference to the sorter. Some containers, however, can be reduced to pointers first, thus removing the overhead of []-operator access.
		*/
		template <typename container_t>
			class Accessor
			{
			public:
			typedef	container_t&		accessor_t;
			typedef const container_t&	const_accessor_t;

		
			static	accessor_t				resolve(container_t&container)
										{
											return container;
										}
			static	const_accessor_t		resolve(const container_t&container)
										{
											return container;
										}
			};

		
		template <typename Entry, typename MyStrategy>
			class Accessor<Ctr::Array<Entry,MyStrategy> >
			{
			public:
			typedef	Entry*				accessor_t;
			typedef const Entry*		const_accessor_t;

		
			static	accessor_t			resolve(Ctr::Array<Entry,MyStrategy>&container)
										{
											return container.pointer();
										}
			static	const_accessor_t	resolve(const Ctr::Array<Entry,MyStrategy>&container)
										{
											return container.pointer();
										}
			};

		template <typename Entry>
			class Accessor<Ctr::ArrayData<Entry> >
			{
			public:
			typedef	Entry*				accessor_t;
			typedef const Entry*		const_accessor_t;

		
			static	accessor_t			resolve(Ctr::ArrayData<Entry>&container)
										{
											return container.pointer();
										}
			static	const_accessor_t	resolve(const Ctr::ArrayData<Entry>&container)
										{
											return container.pointer();
										}
			};
				

		/**
			@brief Sorting methods that rely on a compareTo(...) method to be implemented by each element

		*/
		namespace ByMethod
		{
			template <typename accessor_t>
				bool isSorted(accessor_t field, count_t elements)
				{
					for (index_t i = 1; i < elements; i++)
						if (field[i-1].compareTo(field[i])>0)
							return false;
					return true;
				}


			template <typename Container>
				bool isSorted(const Container&container)
				{
					return isSorted(Accessor<Container>::resolve(container),container.size());
				}
			template <typename Container>
				bool IsSorted(const Container&container)	{return isSorted(container);}

			template <typename accessor_t, typename MyStrategy>
				void	quickSort(accessor_t data, index_t lo, index_t hi)
				{
					if (lo >= hi)
						return;

					index_t pivot_index = (lo+hi+1)/2;
					MyStrategy::swap(data[pivot_index],data[hi]);
					auto	pivot = &data[hi];
					size_t write=lo;

					for (size_t i = lo; i < hi; i++)
						if (data[i].compareTo(*pivot) < 0)
							MyStrategy::swap(data[write++],data[i]);

					MyStrategy::swap(data[write],*pivot);

					if (write>1)
						quickSort<accessor_t,MyStrategy>(data,lo,write-1);
					quickSort<accessor_t,MyStrategy>(data,write+1,hi);
				}

			template <typename Container>
				void	quickSort(Container&container)
				{
					if (container.size()<=1)
						return;
					typedef typename Accessor<Container>::accessor_t	accessor_t;
					accessor_t data = Accessor<Container>::resolve(container);
					//typedef typename StrategySelector<decltype(data.operator[](index_t(0)))>::Default	Strategy;
					typedef typename StrategySelector<decltype(data[index_t(0)])>::Default	Strategy;
					quickSort<accessor_t,Strategy>(data,0,container.size()-1);
				}
			template <typename Container>
				void	QuickSort(Container&container)	{quickSort(container);}

			template <typename Type>
				void	QuickSortField(Type*data, count_t numElements)
				{
					if (numElements<=1)
						return;
					typedef typename StrategySelector<Type>::Default	Strategy;
					quickSort<Type*,Strategy>(data,0,numElements-1);
				}
		}


		/**
			@brief Sorting methods that rely on a operators to be implemented by each element

			Assuming the used data type implements the appropriate operators (&lt; and &gt; are sufficient), this is actually a more generally applicable version.
			Most (currently all) functions in this namespace will only ever use one type of operator (either operator&lt; _or_ operator&gt; , not both),
			so the efficiency difference between a&lt;b and a.compareTo(b)&lt;0 is rather neclectible.

		*/
		namespace ByOperator
		{
			template <typename accessor_t>
				bool isSorted(accessor_t field, count_t elements)
				{
					for (index_t i = 1; i < elements; i++)
						if (field[i-1] > field[i])
							return false;
					return true;
				}


			template <typename Container>
				bool isSorted(const Container&container)
				{
					return isSorted(Accessor<Container>::resolve(container),container.size());
				}
			template <typename Container>
				bool IsSorted(const Container&container)	{return isSorted(container);}

			template <typename accessor_t, typename MyStrategy>
				void	quickSort(accessor_t data, index_t lo, index_t hi)
				{
					if (lo >= hi)
						return;

					index_t pivot_index = (lo+hi+1)/2;
					MyStrategy::swap(data[pivot_index],data[hi]);
					auto	pivot = &data[hi];
					size_t write=lo;

					for (size_t i = lo; i < hi; i++)
						if (data[i] < *pivot)
							MyStrategy::swap(data[write++],data[i]);

					MyStrategy::swap(data[write],*pivot);

					if (write>1)
						quickSort<accessor_t,MyStrategy>(data,lo,write-1);
					quickSort<accessor_t,MyStrategy>(data,write+1,hi);
				}

			template <typename Container>
				void	quickSort(Container&container)
				{
					if (container.size()<=1)
						return;

					typedef typename Accessor<Container>::accessor_t	accessor_t;
					accessor_t data = Accessor<Container>::resolve(container);
					typedef typename StrategySelector<decltype(data[0])>::Default	Strategy;
					quickSort<accessor_t,Strategy>(data,0,container.size()-1);
				}
			template <typename Container>
				void	QuickSort(Container&container)	{quickSort(container);}

			template <typename Type>
				void	QuickSortField(Type*data, count_t numElements)
				{
					if (numElements<=1)
						return;
					typedef typename StrategySelector<Type>::Default	Strategy;
					quickSort<Type*,Strategy>(data,0,numElements-1);
				}
		}




		/**
			@brief Sorting methods that rely on a comparator function or object to be passed as parameter
		*/
		namespace ByComparator
		{
			template <typename accessor_t, typename Comparator>
				bool isSorted(accessor_t field, count_t elements, const Comparator&comparator)
				{
					for (index_t i = 1; i < elements; i++)
						if (comparator(field[i-1],field[i])>0)
							return false;
					return true;
				}


			template <typename Container, typename Comparator>
				bool isSorted(const Container&container, const Comparator&comparator)
				{
					return isSorted(Accessor<Container>::resolve(container),container.size(),comparator);
				}
			template <typename Container, typename Comparator>
				bool IsSorted(const Container&container, const Comparator&comparator)	{return isSortted(container,comparator);}

			template <typename accessor_t, typename MyStrategy, typename Comparator>
				void	quickSort(accessor_t data, index_t lo, index_t hi,const Comparator&comparator)
				{
					if (lo >= hi)
						return;

					index_t pivot_index = (lo+hi+1)/2;
					MyStrategy::swap(data[pivot_index],data[hi]);
					auto	pivot = &data[hi];
					size_t write=lo;

					for (size_t i = lo; i < hi; i++)
						if (comparator(data[i],*pivot)<0)
							MyStrategy::swap(data[write++],data[i]);

					MyStrategy::swap(data[write],*pivot);

					if (write>1)
						quickSort<accessor_t,MyStrategy,Comparator>(data,lo,write-1,comparator);
					quickSort<accessor_t,MyStrategy,Comparator>(data,write+1,hi,comparator);
				}

			template <typename Container, typename Comparator>
				void	quickSort(Container&container,const Comparator&comparator)
				{
					if (container.size()<=1)
						return;

					typedef typename Accessor<Container>::accessor_t	accessor_t;
					accessor_t data = Accessor<Container>::resolve(container);
					typedef typename StrategySelector<decltype(data[0])>::Default	Strategy;
					quickSort<accessor_t,Strategy,Comparator>(data,0,container.size()-1,comparator);
				}
			template <typename Container, typename Comparator>
				void	QuickSort(Container&container,const Comparator&comparator)	{quickSort(container,comparator);}

			template <typename Type, typename Comparator>
				void	QuickSortField(Type*data, count_t numElements,const Comparator&comparator)
				{
					if (numElements<=1)
						return;
					typedef typename StrategySelector<Type>::Default	Strategy;
					quickSort<Type*,Strategy,Comparator>(data,0,numElements-1,comparator);
				}
		}
	}


	#include "sorter.tpl.h"
}

#endif
