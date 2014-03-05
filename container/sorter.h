#ifndef sorterH
#define sorterH

/******************************************************************

Generalized sorters for lists and fields.

******************************************************************/


#include "sort_classes.h"


//#include <iostream>
//using namespace std;

template <class Class> class Sort
{
public:
template <class List> static    void    mergeSortSTL(List&list);
template <class List> static    void    mergeSortRAL(List&list);
template <class Type> static    void    mergeSortField(Type field[], size_t len);
template <class Type> static    void    mergeSortField(Array<Type>&field);
template <class List> static    void    bubbleSortSTL(List&list);
template <class List> static    void    bubbleSortRAL(List&list);
template <class Type> static    void    bubbleSortField(Type field[], size_t len);
template <class Type> static    void    bubbleSortField(Array<Type>&field);
template <class List> static    void    quickSortSTL(List&list);
template <class List> static    void    quickSortRAL(List&list);
template <class Type> static    void    quickSortField(Type field[], size_t len);
template <class Type> static    void    quickSortField(Array<Type>&field);
template <class List> static    void    heapSortRAL(List&list);
template <class Type> static    void    heapSortField(Type field[], size_t len);
template <class Type> static    void    heapSortField(Array<Type>&field);

template <class List> static    bool    isSorted(const List&list);
template <class Type> static    bool    isSorted(const Type field[], size_t len);
template <class Type> static    bool    isSortedArray(const Array<Type>&field);
};


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

		
	template <typename Entry, typename Strategy>
		class Accessor<Array<Entry,Strategy> >
		{
		public:
		typedef	Entry*				accessor_t;
		typedef const Entry*		const_accessor_t;

		
		static	accessor_t				resolve(Array<Entry,Strategy>&container)
									{
										return container.pointer();
									}
		static	const_accessor_t		resolve(const Array<Entry,Strategy>&container)
									{
										return container.pointer();
									}
		};

	template <typename Entry>
		class Accessor<ArrayData<Entry> >
		{
		public:
		typedef	Entry*				accessor_t;
		typedef const Entry*		const_accessor_t;

		
		static	accessor_t			resolve(ArrayData<Entry>&container)
									{
										return container.pointer();
									}
		static	const_accessor_t	resolve(const ArrayData<Entry>&container)
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

		template <typename accessor_t, typename Strategy>
			void	quickSort(accessor_t data, index_t lo, index_t hi)
			{
				if (lo >= hi)
					return;

				index_t pivot_index = (lo+hi+1)/2;
				Strategy::swap(data[pivot_index],data[hi]);
				auto	pivot = &data[hi];
				size_t write=lo;

				for (size_t i = lo; i < hi; i++)
					if (data[i].compareTo(*pivot) < 0)
						Strategy::swap(data[write++],data[i]);

				Strategy::swap(data[write],*pivot);

				if (write>1)
					quickSort<accessor_t,Strategy>(data,lo,write-1);
				quickSort<accessor_t,Strategy>(data,write+1,hi);
			}

		template <typename Container>
			void	quickSort(Container&container)
			{
				if (container.size()<=1)
					return;
				typedef Accessor<Container>::accessor_t	accessor_t;
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

		template <typename accessor_t, typename Strategy>
			void	quickSort(accessor_t data, index_t lo, index_t hi)
			{
				if (lo >= hi)
					return;

				index_t pivot_index = (lo+hi+1)/2;
				Strategy::swap(data[pivot_index],data[hi]);
				auto	pivot = &data[hi];
				size_t write=lo;

				for (size_t i = lo; i < hi; i++)
					if (data[i] < *pivot)
						Strategy::swap(data[write++],data[i]);

				Strategy::swap(data[write],*pivot);

				if (write>1)
					quickSort<accessor_t,Strategy>(data,lo,write-1);
				quickSort<accessor_t,Strategy>(data,write+1,hi);
			}

		template <typename Container>
			void	quickSort(Container&container)
			{
				if (container.size()<=1)
					return;

				typedef Accessor<Container>::accessor_t	accessor_t;
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

		template <typename accessor_t, typename Strategy, typename Comparator>
			void	quickSort(accessor_t data, index_t lo, index_t hi,const Comparator&comparator)
			{
				if (lo >= hi)
					return;

				index_t pivot_index = (lo+hi+1)/2;
				Strategy::swap(data[pivot_index],data[hi]);
				auto	pivot = &data[hi];
				size_t write=lo;

				for (size_t i = lo; i < hi; i++)
					if (comparator(data[i],*pivot)<0)
						Strategy::swap(data[write++],data[i]);

				Strategy::swap(data[write],*pivot);

				if (write>1)
					quickSort<accessor_t,Strategy,Comparator>(data,lo,write-1,comparator);
				quickSort<accessor_t,Strategy,Comparator>(data,write+1,hi,comparator);
			}

		template <typename Container, typename Comparator>
			void	quickSort(Container&container,const Comparator&comparator)
			{
				if (container.size()<=1)
					return;

				typedef Accessor<Container>::accessor_t	accessor_t;
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

using namespace Sorting;

#include "sorter.tpl.h"

#endif
