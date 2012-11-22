#ifndef gl_generatorsH
#define gl_generatorsH


/******************************************************************

Predefined OpenGL display-list generators.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include "../math/matrix.h"
#include "gl.h"


namespace Generator
{

	GLuint glGrid(unsigned width, unsigned height, unsigned depth);            //!< Creates a 3d-grid-object using one line per passed unit \param width Number of units in x direction \param height Number of units in y direction \param depth Number of units in z direction
	GLuint glBlendGrid(unsigned width, unsigned height, float red, float green, float blue);	// creates a 2d grid-object using one line per passed unit. Uses more transparent lines for less important grid lines
	GLuint glGridSystem(unsigned width, unsigned height, unsigned depth);      //creates just the system-lines (arrows)
	GLuint glCross(float size);                                                //creates a cross-object to use as a cursor
	GLuint glSimpleCross(float size);                                          //creates a simplified cross-object to use as a cursor
	GLuint glCube(float size);                                                 //creates a cube-object of the specified size including normals
	GLuint glTangentMappedCube(float size);										//!< Creates a six-quad cube with normals, 2d texcoords and the tangent as color
	GLuint glTextureCube(float size);                                          //similar to the above including texture-coordinates
	GLuint glSphere(float radius, unsigned iterations);                        //creates a sphere using the specified radius and iterations. the iterations specify the number of intersections along each axis (linear)
	GLuint glTextureSphere(float radius, unsigned iterations);                 //similar to the above including texture-coordinates
	GLuint glCubeCoordSphere(float radius, unsigned iterations);               //similar to the above using normals as 3d-texture-coordinates
	GLuint glSphereSide(float radius, unsigned iterations);                    //creates the top-side of a sphere (similar to gl_Sphere)
	GLuint glWireEightSided(float size);                                       //creates a wireframe octahedron
	GLuint glWireBox(float size);                                              //creates a wireframe box
	GLuint glCornerWireBox(float size);                                        //creates a wireframe box with lines only near the corners
	GLuint glArrow(float length, float radius, unsigned steps, BYTE direction_component);  //creates a 3d-arrow using a cone as peak. length specifies the length of the arrow,radius the radius of the peak, steps the number of iterations and direction_component the axis, the arrow should point to.
	GLuint glCircle(float radius, unsigned iterations, BYTE zero_component);               //creates a circle.
	GLuint glFilledCircle(float radius, unsigned iterations);                              //" a filled circle.
	GLuint glCylinderX(float bottom, float top, float radius, unsigned iterations);         //" a cylinder-object including bottom and top caps (and normals) pointing in X-direction
	GLuint glCylinderY(float bottom, float top, float radius, unsigned iterations);         //" a cylinder-object including bottom and top caps (and normals) pointing in Y-direction
	GLuint glCylinderZ(float bottom, float top, float radius, unsigned iterations);         //" a cylinder-object including bottom and top caps (and normals) pointing in Z-direction
	GLuint glCylinderSideY(float radius, float floor, float top, unsigned segments, bool texcoords=true);        //" a cylinder-object excluding bottom and top caps pointinh in Y-direction
	GLuint glTangentMappedCylinderY(float bottom, float top, float radius, unsigned iterations);         //" a cylinder-object including bottom and top caps (and normals) pointing in Z-direction
	GLuint glMassiveDepthArrow(float width, float height);                                 //creates arrow pointing in z-direction
	GLuint glWiredDepthArrow(float width, float height);                                   //similar to the above but as wireframe
	GLuint glGeoSphere(float radius, unsigned segments);                                   //creates a wireframe-approximation of a sphere
	GLuint glGeoHull(float radius, unsigned segments);                                     //creates a filled sphere using polar coordinate-system (including normals and texture-coordinates)
	
	GLuint glFiveEdgedTwelveSided(float radius);
	GLuint glMultiFacedSphere(float radius);

	//experimental or depreciated
	GLuint glCappedCursor(float size, float cap_width);
	GLuint glConeSegment(float angle, float inner_radius, float outer_radius, float segment_begin, float segment_end, unsigned segments);
	GLuint glGround(double*map, unsigned map_width, unsigned map_height,double min_x, double min_z, double max_x, double max_z);
	GLuint glSkyBox();
	GLuint glHudGrid(float min_x, float min_y, float max_x, float max_y, unsigned xcells, unsigned ycells);	//!< Generates a 2-dimensional hud grid stretched over the specified render region
	GLuint glPlane(float min_x, float min_y, float max_x, float max_y, unsigned xcells, unsigned ycells);	//!< Generates a plane composed of the specified amount of quad segments, including texture coordinates
}

namespace Generators
{
	using namespace Generator;
}


#endif
