#include "../global_root.h"
#include "plant.h"

/******************************************************************

Natural plant structure.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

#if 0

namespace Plant
{
	
	Signature			IntAttribute::this_type;
	Signature			FloatAttribute::this_type;
	Signature			StringAttribute::this_type;
	
		
	Configuration::Configuration()
	{}
	
	Configuration::Configuration(const Configuration&other):string_attributes(other.string_attributes),int_attributes(other.int_attributes),float_attributes(other.float_attributes)
	{
		for (unsigned i = 0; i < other.custom_attributes.count(); i++)
			custom_attributes.append(other.custom_attributes[i]->clone());
	}
	
	Configuration&		Configuration::operator=(const Configuration&other)
	{
		string_attributes = other.string_attributes;
		int_attributes = other.int_attributes;
		float_attributes = other.float_attributes;
		custom_attributes.clear();
		for (unsigned i = 0; i < other.custom_attributes.count(); i++)
			custom_attributes.append(other.custom_attributes[i]->clone());
		return *this;
	}
	
	bool				Configuration::isEmpty()	const
	{
		return !string_attributes.count() && !int_attributes.count() && !float_attributes.count() && !custom_attributes.count();
	}
			
	String&			Configuration::stringAttrib(unsigned index, const String&except)
	{
		if (index >= string_attributes.count())
		{
			string_attributes.setSize(index+1);
			string_attributes[index].value = except;
		}
		return string_attributes[index].value;
	}
	
	int&				Configuration::intAttrib(unsigned index, int except)
	{
		if (index >= int_attributes.count())
		{
			int_attributes.setSize(index+1);
			int_attributes[index].value = except;
		}
		return int_attributes[index].value;
	}
	
	float&				Configuration::floatAttrib(unsigned index, float except)
	{
		if (index >= float_attributes.count())
		{
			float_attributes.setSize(index+1);
			float_attributes[index].value = except;
		}
		return float_attributes[index].value;
	}
	
	
	void		Generator::generateTree(Trunk&target)
	{
		if (target.config.isEmpty())
			target.config = config;
		applyConfiguration(target);
	}
	
	
	
	
	
	
	void	DefaultTreeGenerator::applyConfiguration(Trunk&target)
	{
		/*...*/
	
		set(Seed,random.get());
	}
	
	DefaultTreeGenerator::DefaultTreeGenerator():Generator("Default")
	{
		config.string_attributes.setSize(NumStringAttributes);
		config.int_attributes.setSize(NumIntAttributes);
		config.float_attributes.setSize(NumFloatAttributes);
		

		set(TexturePack,"default");
		
		set(Seed,random.get());
		set(Shadow,1);
		set(AutoBalance,1);
		set(RotateLeaves,1);
		
		set(LevelOfDetail,1);
		set(LeafSize,0.5);
		set(BaseLength,1.25);
		set(BaseRadius,0.25);
		set(Variance,0.125);
		set(SuccessorRadiusScale,0.8);
		set(SuccessorLengthScale,0.9);
		set(BranchRadiusScale,0.8*0.7);
		set(BranchLengthScale,0.9*0.9);
		set(BranchMinDeviation,40);
		set(VerticalOrientation,2.5);
		set(ShadowConeSteepness,0.1);
	}
	
	void		DefaultTreeGenerator::generateHull(const Trunk&source, CGS::Geometry<>&target)
	{
		
	
	}

	
	
}

#endif
