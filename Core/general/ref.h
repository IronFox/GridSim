#ifndef refH
#define refH

/******************************************************************

Signature-objects.

******************************************************************/

#if 0

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
		void			Set(Type*ref, const Signature&sig)
						{
							signature = &sig;
							reference = ref;
						}
		operator bool()	const	{return reference != NULL;}
};

#endif /*0*/

#endif
