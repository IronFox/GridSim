#ifndef managedH
#define managedH

/******************************************************************

managed.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace P
{
	class Accounted
	{
	private:
			LONG				usage_count;
			friend class SharedBase;
	public:
								Accounted():usage_count(0)
								{}
	virtual						~Accounted()	{};
	};


	class SharedBase
	{
	private:
			Accounted			*acc_;


			void				set(Accounted*acc)
								{
									acc_ = acc;
									if (acc_)
										InterlockedIncrement(&acc_->usage_count);
								}
			void				dispose()
								{
									if (acc_ && InterlockedDecrement(&acc_->usage_count) == 0)
										delete acc_;
								}
	public:
								SharedBase(Accounted*acc)
								{
									set(acc);
								}
								SharedBase(const SharedBase&other)
								{
									set(other.acc_);
								}
	virtual						~SharedBase()
								{
									dispose();
								}
			SharedBase&		operator=(const SharedBase&other)
								{
									if (other.acc_ != acc_)
									{
										dispose();
										set(other.acc_);
									}
									return *this;
								}
			SharedBase&		operator=(Accounted*acc)
								{
									if (acc_ != acc)
									{
										dispose();
										set(acc);
									}
									return *this;
								}
	};


	template <typename T>
		class Shared:private SharedBase
		{
		private:
				T*				obj;
		public:
								Shared(T*ptr=NULL):SharedBase(ptr),obj(ptr)
								{}
							template <typename T0>
								Shared(const Shared<T0>&other):SharedBase(other),obj(other.obj)
								{}

				T*				get() const	{return obj;}
				T*				operator->() const {return obj;}
				T&				operator*() const {return obj;}

		};

};












#include "managed.tpl.h"


#endif
