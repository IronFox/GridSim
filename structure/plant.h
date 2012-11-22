#ifndef plantH
#define plantH

/******************************************************************

Natural plant structure.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#if 0

#include "../math/vector.h"
#include "../container/lvector.h"
#include "../general/ref.h"
#include "../general/random.h"
#include "../geometry/cgs.h"

namespace Plant	//! Natural plant namespace
{
	class Branch;
	
	struct TVertex	//! Hull vertex
	{
			float				position[3],	//!< Vertex position (relative to the tree root)
								normal[3],		//!< Vertex normal
								texcoords[2],	//!< Texcoords (assuming the branch texture is roled around the branches)
								tangent[3],		//!< Tangent vector for normal mapping. \b tangent and \b normal are supposed to be orthogonal
								agility;		//!< Agility based on the generation and branch thickness (for wind calculation)
	};
	

	

	class Node	//! Tree node
	{
	public:
			Array<Branch>		branches;		//!< Sub branches
			float				position[3],	//!< Node position
								direction[3],	//!< Node direction
								radius,			//!< Radius (decreases each node generation)
								length;			//!< Node length (decreases each node generation)
	};

	class Branch	//! Tree branch. A branch is a collection of nodes. Each node may have sub branches
	{
	public:
			Array<Node>		nodes;	//!< Nodes of this branch (first node = root node)
			unsigned			generation;		//!< Generation of the first node of this branch (0=tree root node, increases by one each node)
	};

	
	class Attribute		//! Generic tree attribute
	{
	public:
			String				name;		//!< Attribute name
			unsigned			elements;	//!< Number of elements this attribute provides (1 for common attributes, 3 for vectors, etc)
			String 			type;		//!< Type name
			Signature			*type_signature;	//!< Type signature for direct comparison
								
								Attribute(const String&type_, Signature*signature, unsigned elements_=1):elements(elements_),type(type_),type_signature(signature)	{};
			
	virtual	int					getAsInt()			const {return 0;};	//!< Retrieves the current attribute value as a single integer or 0 if not applicable
	virtual	const int*			getAsIntArray()		const {return NULL;};	//!< Retrieves the current attribute value as an array of integers or NULL if not applicable
	virtual	float				getAsFloat()		const {return 0;};		//!< Retrieves the current attribute value as a single float or 0 if not applicable
	virtual	const float*		getAsFloatArray()	const {return NULL;};	//!< Retrieves the current attribute value as an array of floats or NULL if not applicable
	virtual	String				toString()			const {return "";};		//!< Converts the current attribute value to a string
	
	virtual	bool				setAsString(const char*)	{return false;};	//!< Attempts to update the local attribute value from the specified string
	virtual	bool				setAsInt(int)				{return false;};	//!< Attempts to update the local attribute value from the specified integer
	virtual	bool				setAsIntArray(const int*)	{return false;};	//!< Attempts to update the local attribute value from the specified array of integers
	virtual	bool				setAsFloat(float)			{return false;};	//!< Attempts to update the local attribute value from the specified float
	virtual	bool				setAsFloatArray(const float*){return false;};	//!< Attempts to update the local attribute value from the specified array of floats
	virtual	Attribute*			clone()	const=0;								//!< Creates an exact duplicate of the local attribute. The client application is responsible for discarding the returned pointer. It's adviced to insert the resulting new object into a list.
	};
	
	class IntAttribute:public Attribute	//! Integer attribute
	{
	public:
	static	Signature			this_type;	//!< Type signature
			int					value;		//!< Current value
			
								IntAttribute():Attribute("Integer",&this_type),value(0)	{};
	
	virtual	int					getAsInt()			const {return value;};
	virtual	const int*			getAsIntArray()		const {return &value;};
	virtual	float				getAsFloat()		const {return value;};
	virtual	String				toString()			const {return String(value);};
	
	virtual	bool				setAsString(const char*str)	{return convert(str,value);};
	virtual	bool				setAsInt(int val)			{value = val; return true;};
	virtual	bool				setAsIntArray(const int*p)	{value = p[0]; return true;};
	virtual	bool				setAsFloat(float val)		{value = (int)val; return true;};
	virtual	bool				setAsFloatArray(const float*p){value = (int)p[0]; return true;};
	virtual	Attribute*			clone()		const			{return SHIELDED(new IntAttribute(*this));}				
	};
	
	class FloatAttribute:public Attribute	//! Floating point attribute
	{
	public:
	static	Signature			this_type;	//!< Type signature
			float				value;		//!< Current value
			
								FloatAttribute():Attribute("Float",&this_type),value(0)	{};
	
	virtual	int					getAsInt()			const {return (int)value;};
	virtual	float				getAsFloat()		const {return value;};
	virtual	const float*		getAsFloatArray()	const {return &value;};
	virtual	String				toString()			const {return String(value);};
	
	virtual	bool				setAsString(const char*str)	{return convert(str,value);};
	virtual	bool				setAsInt(int val)			{value = val; return true;};
	virtual	bool				setAsIntArray(const int*p)	{value = p[0]; return true;};
	virtual	bool				setAsFloat(float val)		{value = val; return true;};
	virtual	bool				setAsFloatArray(const float*p){value = p[0]; return true;};
	virtual	Attribute*			clone()		const			{return SHIELDED(new FloatAttribute(*this));}
	};
	
	class StringAttribute:public Attribute	//! String attribute
	{
	public:
	static	Signature			this_type;		//!< Type signature
			String				value;			//!< Current value
			
								StringAttribute():Attribute("String",&this_type)	{};
	
	virtual	int					getAsInt()			const {int temp(0); convert(value.c_str(),temp); return temp;};
	virtual	float				getAsFloat()		const {float temp(0); convert(value.c_str(),temp); return temp;};
	virtual	String				toString()			const {return value;};
	
	virtual	bool				setAsString(const char*str)	{value = str; return true;};
	virtual	bool				setAsInt(int val)			{value = String(val); return true;};
	virtual	bool				setAsIntArray(const int*p)	{value = String(p[0]); return true;};
	virtual	bool				setAsFloat(float val)		{value = String(val); return true;};
	virtual	bool				setAsFloatArray(const float*p){value = String(p[0]); return true;};
	virtual	Attribute*			clone()		const			{return SHIELDED(new StringAttribute(*this));}
	};
	
	
	class Configuration	//! Tree configuration
	{
	public:
			Array<StringAttribute>		string_attributes;	//!< Array of string attributes
			Array<IntAttribute>			int_attributes;		//!< Array of integer attributes
			Array<FloatAttribute>		float_attributes;	//!< Array of float attributes
			List::Vector<Attribute>		custom_attributes;	//!< List of other custom attributes
			
										Configuration();
										Configuration(const Configuration&other);	//!< Copy constructor using the Attribute::clone() method for custom attributes
			Configuration&				operator=(const Configuration&other);		//!< Copy operator using the Attribute::clone() method for custom attributes \return Reference to this
			bool						isEmpty()	const;							//!< Detects if the local configuration is empty causing the generator to overwrite it with its own configuration
			
			String&						stringAttrib(unsigned index, const String&except="");			//!< Retrieves a string attribute. If the respective attribute does not exist, it is created and initialized using \b except.
			int&						intAttrib(unsigned index, int except=0);					//!< Retrieves a integer attribute. If the respective attribute does not exist, it is created and initialized using \b except.
			float&						floatAttrib(unsigned index, float except=0);				//!< Retrieves a float attribute. If the respective attribute does not exist, it is created and initialized using \b except.
	};
	
	class Generator;
	
	class Trunk:public Branch	//!< Tree trunk
	{
	public:
			Configuration				config;		//!< Configuration used to generate this tree
			Generator					*generator;	//!< Generator used to generate this tree

	};
	
	class Generator	//! Abstract tree generator
	{
	protected:
	virtual	void						applyConfiguration(Trunk&target)=0;	//!< Applies the tree's current configuration. If the current global configuration features a seed then this method may generate a new seed after application
	public:
			Configuration				config;	//!< Active global configuration (constructed by the generator constructor)
			
			const String				name;
			
										Generator(const String&name_):name(name_)	{};
										
	/*!
		\brief Generates the target tree
		
		generate() (re)generates the target tree based on its current configuration if not empty or the global
		configuration. The method Configuration::isEmpty() is used to determine whether
		or not the current tree configuration is empty.
	
	*/
			void						generateTree(Trunk&target);
	virtual	void						generateHull(const Trunk&source, CGS::Geometry<>&target)=0;	//!< Generates a hull geometry from the specified target
	};
	
	
	
	class DefaultTreeGenerator:public Generator	//! Default tree generator
	{
	private:
			Random						random;
	protected:
	virtual	void						applyConfiguration(Trunk&target);
	public:
			enum eStringAttribute
			{
				TexturePack,
				NumStringAttributes
			};
			
			enum eIntAttribute
			{
				Seed,
				Shadow,
				AutoBalance,
				RotateLeaves,
				NumIntAttributes
			};
			
			enum eFloatAttribute
			{
				LevelOfDetail,
				LeafSize,
				BaseLength,
				BaseRadius,
				Variance,
				SuccessorRadiusScale,
				SuccessorLengthScale,
				BranchRadiusScale,
				BranchLengthScale,
				BranchMinDeviation,
				VerticalOrientation,
				ShadowConeSteepness,
				NumFloatAttributes
			};
			
										DefaultTreeGenerator();
	virtual	void						generateHull(const Trunk&source, CGS::Geometry<>&target);
	
	inline	void						set(eStringAttribute attrib, const String&val)
										{
											config.string_attributes[attrib].value = val;
										}
	inline	void						set(eIntAttribute attrib, int val)
										{
											config.int_attributes[attrib].value = val;
										}
	inline	void						set(eFloatAttribute attrib, float val)
										{
											config.float_attributes[attrib].value = val;
										}
										
	};
}

typedef Plant::Trunk	Plant;


#endif




#endif
