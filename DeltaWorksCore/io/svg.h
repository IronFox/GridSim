#pragma once

#include "xml.h"
#include "../container/buffer.h"
#include "../math/vclasses.h"
#include "../math/matrix.h"


namespace SVG
{

	class Element;

	class BaseElement
	{
	protected:
		friend class Document;

		XML::Node	&node;
		void		SetRGB(const String&keyName, const TVec3<>&rgb);

		/**/		BaseElement(XML::Node&node):node(node)	{}
	public:
		bool		IsGroup() const;
		/**Creates a new group child element in the local SVG element. Local element must be a group*/
		Element		CreateGroup();
		/**Creates a new line child element in the local SVG element. Local element must be a group*/
		Element		CreateLine(const TVec2<>&,const TVec2<>&);
		/**Creates a new image child element in the local SVG element. Local element must be a group*/
		Element		CreateImage(const Rect<>&, const PathString&path);
		/**Creates a new polyline child element in the local SVG element. Local element must be a group*/
		Element		CreatePolyline(const ArrayRef<float2>&);
		/**Creates a new polygon child element in the local SVG element. Local element must be a group*/
		Element		CreatePolygon(const ArrayRef<float2>&);
		/**Creates a new polygon child element in the local SVG element. Local element must be a group*/
		Element		CreatePolygon(const Rect<>&);
		/**Creates a new circle child element in the local SVG element. Local element must be a group*/
		Element		CreateCircle(const TVec2<>&center, float radius);
		Element		CreateText(const TVec2<>&,const String&text);

		Rect<>		GetBoundingBox(TMatrix3<> transform, float strokeWidth, float fontSize, const String&textAnchor) const;

	};

	class Document : public BaseElement
	{
		typedef BaseElement Super;
		XML::Container	container;
	public:
		/**/		Document():Super(container.root_node)
		{
			container.root_node.name = "svg";
			container.root_node.SetMore("xmlns","http://www.w3.org/2000/svg").SetMore("xmlns:xlink","http://www.w3.org/1999/xlink").SetMore("version","1.1");
			container.root_node.AddChild("defs");
		}
		Rect<>		GetBoundingBox() const	{return Super::GetBoundingBox(Matrix<>::eye3,1.f,1.f,"start");}

		void		AutoDetectDocumentSize(const TVec2<>&extra);
		void		DefineCustomDocumentSize(const TVec2<>&size);
		void		SaveToFile(const PathString&) const;
		Element		CreatePattern(const String&name, const Rect<>&);
	};


	class Element : public BaseElement
	{
		friend class Document;
		friend class BaseElement;
		typedef BaseElement	Super;
		/**
		Attaches a transformation to the end of the transformation list, thus affecting contained coordinates first
		*/
		void		Transform(const String&transformation);
		/**
		Attaches a transformation to the front of the transformation list, thus affecting contained coordinates last
		*/
		void		TransformFront(const String&transformation);
		
		/**/		Element(XML::Node&node):Super(node)	{}
	public:

		Element&	Scale(const TVec2<>&);
		Element&	Translate(const TVec2<>&);
		Element&	Rotate(float angle);
		Element&	Rotate(float angle, const TVec2<>&rotationPivot);
		Element&	SkewX(float);
		Element&	SkewY(float);



		Element&	SetStrokeWidth(float w);
		Element&	Stroke(const float3&);
		Element&	Stroke(const float4&);
		Element&	NoStroke();
		Element&	Fill(const float3&);
		Element&	Fill(const float4&);
		Element&	FillPattern(const String&patternName);
		Element&	NoFill();
		Element&	SetFontSize(float);
		Element&	SetFontFamily(const String&);
		Element&	SetTextAnchor(const String&);
	};




}