#ifndef refH
#define refH

/******************************************************************

Signature-objects.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



class Signature
{};


template <class Type> struct Reference
{
        const Signature	*signature;
        Type           	*reference;
        
						Reference():signature(NULL),reference(NULL){};
		
		Type*			get(const Signature&sig)	const
						{
							if (signature == &sig)
								return reference;
							return NULL;
						}
		void			set(Type*ref, const Signature&sig)
						{
							signature = &sig;
							reference = ref;
						}
		operator bool()	const	{return reference != NULL;}
};

#endif
