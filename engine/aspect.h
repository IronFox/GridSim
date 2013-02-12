#ifndef engine_aspectH
#define engine_aspectH

/******************************************************************

Camera-structures (in general aspects) to simplify usage of
different types of cameras.

******************************************************************/


#include "../math/matrix.h"
#include "../math/complex.h"
#include "enumerations.h"
//#include <general/undef.h>

namespace Engine
{


template <class>	class	Aspect;
template <class>	class	OrthographicAspect;
template <class>	class	Camera;
template <class>	class	AngularCamera;
template <class>	class	VectorCamera;


template <class C=float>
class	Frustum		//! Volume description of the visible area.
{
private:
		MF_DECLARE(void)	defNormal(const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, TVec3<C>&target);
template <typename Subtype>
		MF_DECLARE(void)	defNormalST(const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, TVec3<C>&target);
		MF_STATIC(1,bool)	intersects(const C0&x0, const C0&y0, const C0&z0,const C0&x1, const C0&y1, const C0&z1,const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, const TVec3<C>&p3);
		MF_STATIC(1,bool)	intersects(const TVec3<C0> box[2], const TVec3<C>&p0, const TVec3<C>&p1, const TVec3<C>&p2, const TVec3<C>&p3);

public:
	union
	{
		struct
		{
			TVec3<C>		corner[8];	//!< volume corners. each contains an R3 vector
		};
		struct
		{
			TVec3<C>		near[4],		//!< near volume corners. each contains an R3 vector
							far[4];			//!< far volume corners. each contains an R3 vector
		};
		struct
		{
			TVec3<C>		near_bottom_left,	//!< volume corner in R3
							near_bottom_right,	//!< volume corner in R3
							near_top_right,		//!< volume corner in R3
							near_top_left,		//!< volume corner in R3
							far_bottom_left,	//!< volume corner in R3
							far_bottom_right,	//!< volume corner in R3
							far_top_right,		//!< volume corner in R3
							far_top_left;		//!< volume corner in R3
		};
	};
	union
	{
		TVec3<C>			normal[6];			//!< normalized volume face normals (pointing out)
		struct
		{
			TVec3<C>		near_normal,		//!< normalized volume face normal (pointing out)
							far_normal,			//!< normalized volume face normal (pointing out)
							bottom_normal,		//!< normalized volume face normal (pointing out)
							right_normal,		//!< normalized volume face normal (pointing out)
							top_normal,			//!< normalized volume face normal (pointing out)
							left_normal;		//!< normalized volume face normal (pointing out)
		};
	};
		enum Visibility		//! Visibility check result
		{
			NotVisible,				//!< Checked entity is not visible
			PartiallyVisible,		//!< Checked entity is partially visible (Due to the nature of the approximation, this result does not exclude full visibility)
			FullyVisible			//!< Checked entity is fully visible
		};

MFUNC3	(bool)				visible(const C0&x, const C1&y, const C2&z) const;		//!< Determines visibility of a point \return true if the specified point lies within the local frustum
MFUNC2	(bool)				visible(const TVec3<C0>&center, const C1&radius)	const;	//!< Determines visibility of a sphere \param center Center of the sphere \param radius Radius of the sphere \return true if the specified sphere is inside or intersects the volume, false otherwise
MFUNC2	(bool)				isVisible(const TVec3<C0>&center, const C1&radius)	const;	//!< Identical to visible()
MFUNC2	(Visibility)		checkSphereVisibility(const TVec3<C0>&center, const C1&radius)	const;	//!< Determines visibility of a sphere \param center Center of the sphere \param radius Radius of the sphere \return sphere visibility
MFUNC2	(Visibility)		checkBoxVisibility(const TVec3<C0>&lower_corner, const TVec3<C1>&upper_corner)	const;	 //!< Determines visibility of a box \param lower_corner 3 component vector pointing to the lower corner of the box \param upper_corner 3 component vector pointing to the upper corner of the box \return box visibility
MFUNC2	(bool)				intersectsCone(const TVec3<C0>&center, const C1&radius) const;	//!< Similar to visible(), however ignoring the znear and zfar planes, making the frustum infinitly long
MF_DECLARE(void)			updateNormals();								//!< Recalculate normals from the modified volume edges
MF_DECLARE(void)			updateNormalsP();								
template <typename SubType>
MF_DECLARE(void)			updateNormalsST();								
MF_DECLARE(String)			toString()	const;
MF_DECLARE(void)			flipNormals();

MFUNC1	(Frustum<C>&)		operator=(const Frustum<C0>&other);
};



namespace GlobalAspectConfiguration
{
extern	bool					world_z_is_up,			//!< Declares whether y or z should point up. Default is false, causing y to point up
								load_as_projection;		//!< Declares that the entire aspect (any camera, view and projection) should be loaded into the projection matrix
};

template <class C=float>
class	Aspect		//! Generalized aspect container. It contains various matrices, the current center location and the region on the screen
{
private:
	static	Frustum<C>			result;
public:
	typedef Frustum<C>			Volume;		//!< Visual volume type
	typedef C					Type;				//!< Local type


	TMatrix4<Type>				view,					//!< 4x4 out view matrix (primary). Set to identity by default
								view_invert,			//!< 4x4 in view matrix (inverted). Set to identity by default
								projection,				//!< 4x4 out projection matrix (primary). Set to identity by default
								projection_invert;		//!< 4x4 in projection matrix (inverted). Set to identity by default
	TVec3<Type>					location;				//!< Central aspect location. Set to (0,0,0) by default

	TFloatRect					region;					//!< Aspect screen region (set to 0,0,1,1 by default)
	eDepthTest					depth_test;	//!< Depth test configuration of this aspect ( Engine::VisualEnum::NormalDepthTest by default)

	static const Aspect<C>		identity;	//!< identity aspect using normal depth test
	MF_CONSTRUCTOR				Aspect();
	MF_CONSTRUCTOR				Aspect(VisualEnum::eDepthTest dtest);

	MF_DECLARE(void)			updateInvert();														//!< Updates view_invert and projection_invert from view and projection
	MFUNC3(void)				translate(const C0&x, const C1&y, const C2&z);						//!< Moves the aspect depending on the current view orientation. Auto updates view_invert. \param x Translation in X-direction (horizontal) \param y Translation in Y-direction (vertical) \param Translation in Z-direction (depth)
	MFUNC1(void)				translate(const TVec3<C0>&delta);										//!< Moves the aspect depending on the current view orientation. Auto updates view_invert. \param delta 3 component movement vector (x,y,z)
	MFUNC4(void)				rotate(const C0&angle, const C1&x, const C2&y, const C3&z);			//!< Rotates the view matrix about an arbitrary axis. The axis components are not required to form a normalized vector. Auto updates view_invert. \param Angle to rotate by (0...360) \param x X-component of the rotation axis \param y Y-component of the rotation axis. \param z Z-component of the rotation axis
	MFUNC2(void)				rotate(const C0&angle, const TVec3<C1>&v);								//!< Rotates the view matrix about an arbitrary axis. The axis is not required to form a normalized vector. Auto updates view_invert. \param Angle to rotate by (0...360) \param v Rotation axis (x,y,z)
	MFUNC1(void)				rotate(const TVec4<C0>&quaternion);										//!< Rotates the view matrix via an arbitrary quaternion. Auto updates view_invert. \param quaternion Quaternion to use for the rotation. The quaternion is required to be normalized.
	MFUNC2(bool)				pointToScreen(const TVec3<C0>&point, TVec2<C1>&screen_point)				const;	//!< Projects a 3d point to 2d screen coordinates (using both view and projection transformations). \param point Point to transform \param screen_point 2 component out vector to store the final screen position in. \return true if the projected point lies within the projection depth range, false otherwise.
	MFUNC2(bool)				vectorToScreen(const TVec3<C0>&vector, TVec2<C1>&screen_point)			const;	//!< Projects a 3d vector to 2d screen coordinates (using both view and projection transformations). The vector is rotated and projected but not translated. \param vector Vector to project \param screen_point 2 component out vector to store the final screen position in. \return true if the vector could be projected.
	MFUNC3(void)				screenToVector(const TVec2<C0>&point, TVec3<C1>&position, TVec3<C2>&direction)	const;	//!< Reverse projects a point on the screen to a point and a direction vector. \param point Screen point to reverse project. \param position 3 component out vector to write the final point coordinates to. \param direction 3 component out vector to write the final viewing direction to.
	MFUNC4(void)				screenToVector(const C0&x, const C1&y, TVec3<C2>&position, TVec3<C3>&direction)	const;	//!< Reverse projects a point on the screen to a point and a direction vector. \param x Screen point(x) to reverse project. \param y Screen point(y) to reverse project. \param position 3 component out vector to write the final point coordinates to. \param direction 3 component out vector to write the final viewing direction to.
	MFUNC2(bool)				project(const TVec3<C0>&point, TVec3<C1>&projected)							const;	//!< Transforms the specified point and projectes it to the screen.
	MFUNC2(void)				reverseProject(const TVec3<C0>&point, TVec3<C1>&position)					const;	//!< Reverse projects a point on the screen to its point of origin. \param point 3 component screen point to reverse project.\param position 3 component out vector to write the final point coordinates to. 
	MF_DECLARE(const Volume&)	resolveVolume() 													const;	//!< Extracts a visual volume from the current matrix content. \return Const reference to a global visual volume object that contains the local visual volume.
	MF_DECLARE(const Volume&)	resolveFrustum()													const;	//!< Identical to resolveVolume()
	MF_DECLARE(const Volume&)	getFrustrum()														const;	//!< Identical to resolveVolume()
	MF_DECLARE(void)			resolveVolume(Volume&v) 											const;	//!< Extracts a visual volume from the current matrix content. @param v [out] Reference to a global visual volume object that should contain the local visual volume.
	MF_DECLARE(void)			resolveFrustum(Volume&v)											const;	//!< Identical to resolveVolume()
	MF_DECLARE(void)			getFrustrum(Volume&v)												const;	//!< Identical to resolveVolume()
	template <typename SubType>
		MF_DECLARE(void)		resolveVolumeST(Volume&v)											const;	//!< Similar however potential more precise version of resolveVolume(). Normal calculation is done using the specified sub type instead (double recommended here)
	MF_DECLARE(String)			toString()															const;	//!< Creates a string representation of the local aspect. \return String representation.
	MF_DECLARE(const TVec3<C>&)	absoluteLocation()													const;	//!< Returns the absolute realworld coordinates of the local aspect object
	MF_DECLARE(void)			loadIdentity();														//!< Resets all matrices and vectors to identity. New aspects automatically load identity into every matrix.
	MF_DECLARE(TVec3<C>&)		viewingDirection();													//!< Retrieves the vector describing the local camera's viewing direction
	MF_DECLARE(const TVec3<C>&)	viewingDirection()		const;										//!< \overload
};




template <class C=float>
class	OrthographicAspect:public Aspect<C>	//! Orthographic aspect for non central projecting viewports
{
public:
	#ifdef __GNUC__
		using	Aspect<C>::view;
		using	Aspect<C>::view_invert;
		using	Aspect<C>::projection;
		using	Aspect<C>::projection_invert;
		using	Aspect<C>::region;
		using	Aspect<C>::location;
		using	Aspect<C>::depth_test;
	#endif

		TMatrix3<C>			orientation;
		
	MF_CONSTRUCTOR			OrthographicAspect(VisualEnum::eDepthTest depthTest = VisualEnum::NoDepthTest);

MF_DECLARE(void)			make(const C&pixel_aspect, const C&zoom, const C&zNear, const C&zFar); 							//!< Create the local projection matrix via aspect, zoom, znear and zfar. Also updates the projection invert. \param pixel_aspect Global pixel aspect. Should not contain the local region aspect. \param zoom Aspect zoom. \param zNear Near z plane. \param zFar Far z plane.
MF_DECLARE(void)			make(const C&left, const C&bottom, const C&right, const C&top,const C&zNear, const C&zFar);	//!< Create the local projection matrix via borders. Also updates the projection invert. \param left Left border. \param bottom Bottom border. \param right Right border. \param top Top border. \param zNear Near z plane. \param zFar Far z plane.
MFUNC1 (void)				make(const Rect<C0>&area, const C&zNear, const C&zFar);											//!< Create the local projection matrix via borders. Also updates the projection invert. \param area Planar viewing area \param zNear Near z plane. \param zFar Far z plane.
MFUNC2 (void)				alterDepthRange(const C0&zNear, const C1&zFar);														//!< Changes the depth range of the local projection matrix. Also updates the projection invert. \param zNear New near z plane. \param zFar New far z plane.
MF_DECLARE(void)			build();																							//!< (Re)assembles the view matrix using the local orientation matrix. Also updates the view invert.
MF_DECLARE(void)			buildScaled();																					//!< Identical to build() except that it allows orientation to consist of non-normalized vectors.
};


template <class C=float>
class	Camera:public Aspect<C>	//! Generalized camera object
{
public:
	#ifdef __GNUC__
		using	Aspect<C>::view;
		using	Aspect<C>::view_invert;
		using	Aspect<C>::projection;
		using	Aspect<C>::projection_invert;
		using	Aspect<C>::region;
		using	Aspect<C>::location;
		using	Aspect<C>::depth_test;
	#endif

		TVec3<C>			retraction;			//!< Retraction vector. The retraction causes the camera to move away from its control center. Use negative z-coordinates for a 3rd person like camera.
		TMatrix3<C>			orientation,
							system;				//!< 3x3 system matrix. This matrix is not needed in the local camera object but by all derived camera objects.
		C					vfov;					//!< Vertical field of view (in degrees).



MF_CONSTRUCTOR				Camera();
MF_DECLARE(void)			make(const C&pixel_aspect, const C&zNear=0.1, const C&zFar=100, const C&vFov=45);	//!< Create the local projection matrix via aspect, znear, zfar and vertical field of view. Also updates the projection invert. \param pixel_aspect Global pixel aspect. Should not contain the local region aspect. \param zNear Near z plane. \param zFar Far z plane. \param vFov Vertical field of view in degrees.
MFUNC2	(void)				alterDepthRange(const C0&zNear, const C1&zFar);											//!< Changes the depth range of the local projection matrix. Also updates the projection invert. \param zNear New near z plane. \param zFar New far z plane.
MF_DECLARE(void)			build();																				//!< (Re)assembles the view matrix using the local orientation matrix. Also updates the view invert.
MFUNC3	(void)				translate(const C0&x, const C1&y, const C2&z);						//!< Moves the camera depending on its current orientation. Auto updates view_invert. \param x Translation in X-direction (horizontal) \param y Translation in Y-direction (vertical) \param Translation in Z-direction (depth)
MFUNC1	(void)				translate(const TVec3<C0>&delta);										//!< Moves the camera depending on its current orientation. Auto updates view_invert. \param delta 3 component movement vector (x,y,z)
MFUNC3	(void)				locate(const C0&x, const C1&y, const C2&z);							//!< Redefines the camera center and rebuilds. \param x Camera x-coordinate \param y Camera y-coordinate \param z Camera z-coordinate
MFUNC1	(void)				locate(const TVec3<C0>&position);										//!< Redefines the camera center and rebuilds. \param position New camera position (x,y,z)
MFUNC2	(void) 				extractDepthRange(C0&zNear, C1&zFar)	const;						//!< Extracts the zNear and zFar values from the projection matrix
};

template <class C=float>
class	AngularCamera:public Camera<C>	//! Camera object using angles to determine the viewing direction
{
public:
	#ifdef __GNUC__
		using	Aspect<C>::view;
		using	Aspect<C>::view_invert;
		using	Aspect<C>::projection;
		using	Aspect<C>::projection_invert;
		using	Aspect<C>::region;
		using	Aspect<C>::location;
		using	Aspect<C>::depth_test;
		
		using	Camera<C>::retraction;
		using	Camera<C>::vfov;
		using	Camera<C>::orientation;
		using	Camera<C>::system;
	#endif

		TVec3<C>			angle;			//!< Current angles about the x, y, and z axes (in that order).

MF_CONSTRUCTOR				AngularCamera();
MF_DECLARE(void)			build();													//!< (Re)assembles the view matrix using the local angles. Also updates the view invert.
MFUNC3	(void)				translatePlanar(const C0&x, const C1&y, const C2&z);		//!< Moves the camera depending on its current viewing direction. The angle around the x-axis is ignored. Auto updates view_invert. \param x Translation in X-direction (horizontal) \param y Translation in Y-direction (vertical) \param Translation in Z-direction (depth)
MFUNC1	(void)				translatePlanar(const TVec3<C0>&delta);					//!< Moves the camera depending on its current viewing direction. The angle around the x-axis is ignored. Auto updates view_invert. \param delta 3 component movement vector (x,y,z)
MFUNC3	(void)				alterAngles(const C0&alpha, const C1&beta, const C2&gamma);	//!< Alters the local viewing angles and rebuilds. \param alpha Alpha angle (about the x-axis) delta. \param beta Beta angle (around the y-axis) delta. \param gamma Gamma angle (around the z-axis) delta.
MFUNC1	(void)				alterAngles(const TVec3<C0>&delta);						//!< Alters the local viewing angles and rebuilds. \param delta 3 component angle delta vector (alpha, beta, gamma).
MFUNC1	(void)				lookAtPlanar(const TVec3<C0>&point);						//!< Rotates the camera towards the specified point. Does not alter the angle around the x-axis. \param point 3d point to look at.
MFUNC1	(void)				lookAt(const TVec3<C0>&point);							//!< Rotates the camera towards the specified point. \param point 3d point to look at.
MF_DECLARE(VectorCamera<C>)toVectorCamera();											//!< Converts the local angular camera to a vector camera. \return Vector camera object matching the local position and orientation.
};


template <class C=float>
class	VectorCamera:public Camera<C>	//! Camera object using vectors to determine the viewing direction
{
private:
		MF_DECLARE(void)	adjustUpAxis();

public:
	#ifdef __GNUC__
		using	Aspect<C>::view;
		using	Aspect<C>::view_invert;
		using	Aspect<C>::projection;
		using	Aspect<C>::projection_invert;
		using	Aspect<C>::region;
		using	Aspect<C>::location;
		using	Aspect<C>::depth_test;
		
		using	Camera<C>::retraction;
		using	Camera<C>::vfov;
		using	Camera<C>::orientation;
		using	Camera<C>::system;
	#endif

		TVec3<C>			direction,				//!< Current viewing direction
							upAxis;				//!< Vertical orientation vector
		TMatrix3<C>			vsystem;					//!< Vector system matrix

MF_CONSTRUCTOR				VectorCamera();
MF_CONSTRUCTOR				VectorCamera(const Camera<C>&);
MF_DECLARE(void)			build();													//!< (Re)assembles the view matrix using the local vectors. Also updates the view invert.
MFUNC3	(void)				translatePlanar(const C0&x, const C1&y, const C2&z);		//!< Moves the camera depending on its current viewing direction. Any up or down orientation of the direction vector is ignored. Auto updates view_invert. \param x Translation in X-direction (horizontal) \param y Translation in Y-direction (upAxis) \param Translation in Z-direction (depth)
MFUNC1	(void)				translatePlanar(const TVec3<C0>&delta);							//!< Moves the camera depending on its current viewing direction. Any up or down orientation of the direction vector is ignored. Auto updates view_invert. \param delta Translation delta vector
MFUNC2	(void)				rotatePlanar(const C0&alpha, const C1&beta);				//!< Rotates the local camera object to the left/right(beta) and up/down(alpha) and rebuilds. \param alpha Angle to rotate up or down  \param beta Angle to rotate about the vertical axis (to the left/right)
MFUNC1	(void)				rotateDirectional(const C0&alpha, bool rebuild=true);		//!< Rotates the local camera object clockwise around the current viewing direction and rebuilds. \param alpha Angle to rotate about the viewing axis \param rebuild Rebuild the local camera object after rotation
MFUNC1	(void)				setDirection(const TVec3<C0>&dir,bool do_build=true);			//!< Modifies the direction vector to match the specified one and rebuilds. \param dir New direction vector
MFUNC1	(void)				setUpAxis(const TVec3<C0>&vert,bool do_build=true);			//!< Modifies the vertical vector to match the specified one and rebuilds. \param vert New vertical orientation vector.
MFUNC2	(void)				align(const TVec3<C0>&dir, const TVec3<C1>&vert);					//!< Modifies both the direction and vertical vectors and rebuilds.  \param dir New direction vector \param vert New vertical orientation vector.
MF_DECLARE	(AngularCamera<C>)	toAngularCamera();											//!< Converts the local vector camera to an angular camera. <b>Not implemented yet. Do not use</b>
};

}

#include "aspect.tpl.h"


#endif
