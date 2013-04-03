#ifndef math_expressionH
#define math_expressionH


#include "../container/string_list.h"
#include "../container/hashtable.h"

namespace Expression
{

	class Variable
	{
	public:
			String				name;
			float				value;
	static	count_t				counter;
			
								Variable():value(counter++)
								{}
	};
	
	typedef HashContainer<Variable>	VarContainer;

	class Entity
	{
	protected:
			float				adjust(float value)	const;	//applies negated and inverted modifiers
	public:
			struct Type
			{
				enum EVal
				{
									Element,
									ProductGroup,
									SumGroup,
									Determinant2,
									Determinant2x3,
									Determinant3Const,
				
									NumTypes
				};
			};
	
	
			bool				negated,	//true =>  -x
								inverted;	//true => 1/x
			Type::EVal			type;
			
								Entity(Type::EVal t):negated(false),inverted(false),type(t)
								{}
	virtual						~Entity()	{};
	virtual	String				ToString(bool show_markers=false)	const=0;	//!< Recursively assembles the local formula into a mathematical expression.
	virtual	Entity*				clone()		const=0;
	virtual	float				evaluate()	const=0;
	virtual	bool				equals(const Entity*other)	const=0;		//!< Tests for inner equality. Does not heed negated and/or inverted flags
	
			String				signedToString()	const
								{
									return (negated?"-":"")+ToString();
								}
								
	
			bool				isGroup()	const	{return type == Type::ProductGroup || type == Type::SumGroup;};
	};
	
	class Element:public Entity, public String
	{
	public:
			char				component;		//!< vector component. 0 if none
			float				value;
			Variable			*reference;
			
			
								Element():Entity(Type::Element),component(0),value(0),reference(NULL)
								{}
								Element(float v):Entity(Type::Element),component(0),value(v),reference(NULL)
								{
									(*(String*)this) = v;
								}
	virtual	String				ToString(bool show_markers=false)	const;	//!< Recursively assembles the local formula into a mathematical expression.
	virtual	Entity*				clone()		const;
	virtual	float				evaluate()	const;
	virtual	bool				equals(const Entity*other)	const;
	
			
			void				parse(const String&string, VarContainer&var_container);
	};
	
	
	class Group:public Entity	//! Enclosed group
	{
	private:
			bool				formDeterminant3();
	public:
			
			List::Vector<Entity>members;	//!< Member entities
			
								Group(Type::EVal t=Type::SumGroup):Entity(t)
								{}
			
			bool				expandNext();	//!< Recurvively changes content so that this group becomes a sum-group of only product groups and elements. @return true if something changed, false otherwise
			void				expand()	{while (expandNext());}
			
			bool				factorOut(const Variable*variable, index_t power);	//!< Attempts to factor out the specified element. Best executed after expandAll()
			count_t				countOccurrences(const Variable*variable) const;	//!< Counts the number of times an element by the specified name is located in this group. Not recursive
			index_t				find(const Variable*variable)	const;				//!< Returns the first element that matches the specified element name or InvalidIndex if the element was not found
			
	virtual	String				ToString(bool show_markers=false)	const;	//!< Recursively assembles the local formula into a mathematical expression.
			bool				parse(const String&expression, VarContainer&var_container);	//!< Reconfigures the local group to match the given expression. Any data currently held by this group is lost. @param expression Expression to parse @return true on success, false otherwise.
	virtual	Entity*				clone()		const;
	virtual	float				evaluate()	const;
	virtual	bool				equals(const Entity*other)	const;
	
			bool				formDeterminants();	//!< Recursively attempts to combine certain expression patterns into determinant expressions in the form |a b| where both a and b must provide x and y components and occur in the expression in the form a.x*b.y-a.y*b.x. Only works in 2d so far
			bool				formDeterminants3();	//!< Recursively attempts to form 3x3 const determinates of existing determinants

			bool				simplify();			//!< Recursively removes inner groups of opposite nature (x/x  or x-x, etc). Also adds counters for equal groups of identical sum expression (x+x => 2*x)
	
	};
	
	
	class Determinant2:public Entity	//! 2x2 Determinant calculation referencing 2 2d vectors. Elements may not be negated or inverted
	{
	public:
			Element				*x0,*y0,
								*x1,*y1;
			
								Determinant2():Entity(Type::Determinant2),x0(NULL),y0(NULL),x1(NULL),y1(NULL)
								{}
	virtual						~Determinant2()
								{
									if (x0)
										DISCARD(x0);
									if (x1)
										DISCARD(x1);
									if (y0)
										DISCARD(y0);
									if (y1)
										DISCARD(y1);
								}
	
	
	virtual	String				ToString(bool show_markers=false)	const;	//!< Recursively assembles the local formula into a mathematical expression.
	virtual	Entity*				clone()		const;
	virtual	float				evaluate()	const;
	virtual	bool				equals(const Entity*other)	const;		//!< Tests for inner equality. Does not heed negated and/or inverted flags
	};
	
	class Determinant2x3: public Entity	//! 3x3 Determinant calculation referencing 3 2d vectors, implying 1 as third column value. Elements may be negated but not inverted. x[i]->negated == y[i]->negated must be true at all times
	{
	public:
			union
			{
				struct
				{
					Element	*x0, *x1, *x2;
				};
				Element		*x[3];
			};
			union
			{
				struct
				{
					Element	*y0, *y1, *y2;
				};
				Element		*y[3];
			};
				
								Determinant2x3(Type::EVal t=Type::Determinant2x3):Entity(t),x0(NULL),y0(NULL),x1(NULL),y1(NULL),x2(NULL),y2(NULL)
								{}
			
	virtual						~Determinant2x3()
								{
									if (x0)
										DISCARD(x0);
									if (x1)
										DISCARD(x1);
									if (x2)
										DISCARD(x2);
									if (y0)
										DISCARD(y0);
									if (y1)
										DISCARD(y1);
									if (y2)
										DISCARD(y2);
								}

	virtual	String				ToString(bool show_markers=false)	const;	//!< Recursively assembles the local formula into a mathematical expression.
	virtual	Entity*				clone()		const;
	virtual	float				evaluate()	const;
	virtual	bool				equals(const Entity*other)	const;		//!< Tests for inner equality. Does not heed negated and/or inverted flags
	};
	
	
	class Determinant3Const: public Determinant2x3	//! 3x3 Determinant calculation referencing 3 2d vectors and a bottom row of constants. Elements may not be negated or inverted
	{
	public:
			float				c0,c1,c2;
								
								Determinant3Const():Determinant2x3(Type::Determinant3Const),c0(1),c1(1),c2(1)
								{}
	virtual	String				ToString(bool show_markers=false)	const;	//!< Recursively assembles the local formula into a mathematical expression.
	virtual	Entity*				clone()		const;
	virtual	float				evaluate()	const;
	virtual	bool				equals(const Entity*other)	const;		//!< Tests for inner equality. Does not heed negated and/or inverted flags
	};
	
	
	class Expression:public Group
	{
	private:
			class Init
			{
			public:
								Init();
			
			};
	static	Init				init;
	public:
			VarContainer		variables;
			
			bool				parse(const String&expression);	//!< Reconfigures the local group to match the given expression. Any data currently held by this group is lost. @param expression Expression to parse @return true on success, false otherwise.
			bool				factorOut(const String&element);					//!< Attempts to factor out the specified element. Best executed after expandAll()
			
	};






}





#endif
