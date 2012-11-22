#ifndef pH
#define pH


template <class C>
	class p
	{
	protected:
			C			*link;
	public:
						p():link(NULL)
						{}
						p(C*pointer):link(pointer)
						{}
	
	inline	C*			operator->()
						{
							return link;
						}
	inline	const C*	operator->() const
						{
							return link;
						}
	inline	C&			operator*()
						{
							return *link;
						}
	inline	const C&	operator*()	const
						{
							return *link;
						}
	inline	operator C*()
						{
							return link;
						}
	inline	operator C*()	const
						{
							return link;
						}
	};





#endif
