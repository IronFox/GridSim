#pragma once

#include <engine/renderer/opengl.h>
#include <math/vclasses.h>
#include <engine/aspect.h>
#include <io/xml.h>

using namespace DeltaWorks;
using namespace DeltaWorks::Math;

namespace Scene3D
{

	void	SetOffset(float x, float y, float z);
	void	Clear();
	void	PutLine(const float3&, const float3&);
	void	PutWireframe(const Box<>&);
	void	PutSolid(const Box<>&);
	void	SetColor(float r, float g, float b);



	void	Render();
}

#ifndef D3
namespace Scene
{
	struct Vertex
	{
		TVec3<>	coords;
		TVec2<>	uv;
		TVec4<>	color;
	};
	
	struct Quad
	{
		Engine::OpenGL::Texture::Reference	texture;
		const Image							*image = nullptr;
		Vertex	v[4];
	};
	
	struct Cube
	{
		Quad	base;
		float	height;
	};
	
	struct Line
	{
		Vertex	v[2];
	};
	
	struct Sphere
	{
		Vertex	center;
		float	radius;
		bool	isCircle = false;
	};
	
	


	void	PushOffset();
	void	AlterOffset(const TVec2<>&);
	void	AlterOffset(const TVec3<>&);
	void	AlterZOffset(float);
	void	PopOffset();
	void	SetLayer(index_t);
	index_t	GetLayer();

	void	SetColor3ub(BYTE r, BYTE g, BYTE b);
	void	SetColor(float r, float g, float b, float a);
	void	SetColor(const TVec3<>&rgb, float a = 1.f);
	void	SetColor(const TVec4<>&rgba);
	
	void	PutInterlayerBeam(index_t targetLayer, float radius, float heightFactor=1.f);
	void	PutSphere(float radius);
	void	PutCircle(float radius);
	void	PutRegularOutlineRect();
	void	PutRotatedOutlineRect(float orientation, const float2&scale);
	void	PutRotatedOutlineCube(float orientation, const float3&scale);
	void	PutRotatedFilledCube(float orientation, const float3&scale);
	void	PutRotatedFilledRect(float orientation, const float2&scale);
	void	PutFilledRect(const Rect<>&);
	void	PutOutlinedRect(const Rect<>&);
	void	PutFilledQuad(const TVec2<>&a, const TVec2<>&b, const TVec2<>&c, const TVec2<>&d);
	void	PutOutlinedQuad(const TVec2<>&a, const TVec2<>&b, const TVec2<>&c, const TVec2<>&d);
	/**
	Places a rectangle in the range (0,0)-(1,1), using the current color and offset
	*/
	void	PutRegularFilledRect();
	void	PutRegularTexturedRect(const Engine::OpenGL::Texture::Reference&, const Image&);
	void	PutLineTo(const TVec2<>&);
	
	count_t	CountLayers();
	void	Create();
	void	Clear();
	void	Render(const Engine::Camera<>&);

	void	BuildSVG(XML::Node&, const PathString&imageFolder, float2&outBounds);
}
#endif

