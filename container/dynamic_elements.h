#ifndef dynamic_elementsH
#define dynamic_elementsH

#include "strategy.h"



/*!
	\brief One directional linked carrier
*/
template <class C> struct SDynamicPath
{
		SDynamicPath<C>		*next;		   //!< Link to next carrier
		C					element;		//!< Carrier content
};

/*!
	\brief Bidirectional linked carrier
*/
template <class C> struct SLinkedCarrier
{
		SLinkedCarrier<C>	*next,		   //!< Link to next carrier
							*prev;		   //!< Link to previous carrier
		C					*element;		//!< Link to element
};

/*!
	\brief Bidirectional iterator class
*/
template <class Carrier, class Entry> class LinkedIterator
{
protected:
		Carrier		   			*carrier;   //!< Link to current carrier

public:
		typedef LinkedIterator<Carrier,Entry>	Iterator;
		
									LinkedIterator(Carrier*c):carrier(c)
									{}
									
		Carrier*					getCarrier()
									{
										return carrier;
									}
		const Carrier*				getCarrier() const
									{
										return carrier;
									}

		Entry						operator*()
									{
										return carrier->getElement();
									}
									
		const Entry					operator*()	const
									{
										return carrier->getElement();
									}
									

		Iterator&					operator++()
									{
										carrier = carrier->getNext();
										return *this;
									}

		Iterator					operator++(int)
									{
										carrier = carrier->getNext();
										return Iterator(carrier->getPrev());
									}

		Iterator&					operator+=(int delta)
									{
										while (delta > 0)
										{
											delta--;
											carrier = carrier->getNext();
										}
										while (delta < 0)
										{
											delta++;
											carrier = carrier->getPrev();
										}
										return *this;
									}

		Iterator					operator+(int delta)				const
									{
										Iterator rs(carrier);
										return rs+=delta;
									}

		Iterator&					operator--()
									{
										carrier = carrier->getPrev();
										return *this;
									}

		Iterator					operator--(int)						const
									{
										carrier = carrier->getPrev();
										return Iterator(carrier->getNext());
									}

		Iterator&					operator-=(int delta)
									{
										while (delta > 0)
										{
											delta--;
											carrier = carrier->getPrev();
										}
										while (delta < 0)
										{
											delta++;
											carrier = carrier->getNext();
										}
										return *this;
									}

		Iterator					operator-(int delta)				const
									{
										Iterator rs(carrier);
										return rs-=delta;
									}

		bool						operator==(const Iterator&other)	const
									{
										return carrier == other.carrier;
									}

		bool						operator!=(const Iterator&other)	const
									{
										return carrier != other.carrier;
									}
};


#endif

