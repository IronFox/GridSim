#include "simple_geometry.h"

/******************************************************************

Language-dependent simple geometry-container.

******************************************************************/

namespace Engine
{

	SimpleGeometry::SimpleGeometry():at(0),m(Triangles),sealed(false)
	{
	    M::Vec::def(v.color,1,1,1,1);
	}

	void SimpleGeometry::resize(unsigned len_)
	{
		field[0].ResizePreserveContent(len_);
		field[1].ResizePreserveContent(len_);
		field[2].ResizePreserveContent(len_);
	}
	
	void SimpleGeometry::reset()
	{
		field[0].reset();
		field[1].reset();
		field[2].reset();
	}

	void SimpleGeometry::pushVertex()
	{
	    if (sealed)
	        FATAL__("object is sealed");
		field[m] << v;
	}

	void SimpleGeometry::select(Mode mode)
	{
	    if (sealed)
	        FATAL__("object is sealed");
	    m = mode;
	    at = 0;
	}

	void SimpleGeometry::seal()
	{
	    sealed = true;
		compact();
	}
	
	void SimpleGeometry::compact()
	{
		for (unsigned i = 0; i < ARRAYSIZE(field); i++)
			field[i].compact();
	}

	void SimpleGeometry::translate(float x_delta, float y_delta, float z_delta)
	{
	    M::TVec3<>	delta = {x_delta,y_delta,z_delta};
	    for (unsigned i = 0; i < field[Triangles].length(); i++)
	        M::Vec::add(field[Triangles][i].xyz,delta);
	        
	    for (unsigned i = 0; i < field[Quads].length(); i++)
	        M::Vec::add(field[Quads][i].xyz,delta);

	    for (unsigned i = 0; i < field[Lines].length(); i++)
	        M::Vec::add(field[Lines][i].xyz,delta);
	}


	void SimpleGeometry::color(float r, float g, float b, float a)
	{
	    M::Vec::def(v.color,r,g,b,a);
	}

	void SimpleGeometry::color(const M::TVec4<>&color)
	{
		v.color = color;
	}

	void SimpleGeometry::color(const M::TVec3<>&color)
	{
		v.color.rgb = color;
		v.color.alpha = 1;
	}
	void SimpleGeometry::color(const M::TVec3<>&color, float alpha)
	{
		v.color.rgb = color;
		v.color.alpha = alpha;
	}


	void	SimpleGeometry::color3fv(const float*c)
	{
		M::Vec::def(v.color,c[0],c[1],c[2],1);
	}
	
	void	SimpleGeometry::color4fv(const float*c)
	{
		M::Vec::def(v.color,c[0],c[1],c[2],c[3]);
	}
	
	void	SimpleGeometry::color3fva(const float*c, float a)
	{
		M::Vec::def(v.color,c[0],c[1],c[2],a);
	}
	

	void SimpleGeometry::vertex(float x, float y, float z, float w)
	{
		M::Vec::def(v,x,y,z,w);
	    pushVertex();
	}
}
