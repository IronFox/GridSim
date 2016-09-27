#include <global_root.h>
#include "svg.h"
#include <container/string_list.h>
#include <string/string_converter.h>


namespace SVG
{

	template <typename T>
		static bool Convert(const String&str, T&rs)
		{
			return convert(str.c_str(),str.length(),rs);
		}
	template <typename T>
		static bool Convert(const StringRef&str, T&rs)
		{
			return convert(str.pointer(),str.length(),rs);
		}

	void BaseElement::SetRGB(const String&name, const TVec3<>&rgb)
	{
		TVec3<int> scaled;
		Vec::mul(rgb,255,scaled);
		Vec::clamp(scaled,0,255);
		node.Set(name,"rgb("+String(scaled.r)+','+String(scaled.g)+','+String(scaled.b)+')');
	}

	void Element::Transform(const String&transformation)
	{
		String prev;
		if (node.Query("transform",prev))
			node.Set("transform",prev+' '+transformation);
		else
			node.Set("transform",transformation);
	}

	void Element::TransformFront(const String&transformation)
	{
		String prev;
		if (node.Query("transform",prev))
			node.Set("transform",transformation+' '+prev);
		else
			node.Set("transform",transformation);
	}

	bool BaseElement::IsGroup() const
	{
		return node.name == "g" || node.name=="svg";
	}

	Element BaseElement::CreateGroup()
	{
		DBG_ASSERT__(IsGroup());
		return Element(node.AddChild("g"));
	}

	Element BaseElement::CreateLine(const TVec2<>&p0, const TVec2<>&p1)
	{
		DBG_ASSERT__(IsGroup());
		return Element(node.AddChild("line")
				.SetMore("x1",p0.x)
				.SetMore("y1",p0.y)
				.SetMore("x2",p1.x)
				.SetMore("y2",p1.y)
				);
	}

	Element BaseElement::CreateImage(const Rect<>&rect, const PathString & path)
	{
		DBG_ASSERT__(IsGroup());
		return Element(node.AddChild("image")
				.SetMore("xlink:href",String(path))
				.SetMore("x", rect.x.min)
				.SetMore("y", rect.y.min)
				.SetMore("width", rect.x.GetExtend())
				.SetMore("height", rect.y.GetExtend())
				);
	}
	
	Element BaseElement::CreatePolyline(const ArrayRef<float2>&points)
	{
		DBG_ASSERT__(IsGroup());
		static StringBuffer strPoints;
		strPoints.Clear();
		foreach (points,p)
			strPoints << p->x << ','<<p->y<<' ';
		return Element(node.AddChild("polyline")
				.SetMore("points",strPoints.ToStringRef())
				);
	}

	Element BaseElement::CreatePolygon(const ArrayRef<float2>&points)
	{
		DBG_ASSERT__(IsGroup());
		static StringBuffer strPoints;
		strPoints.Clear();
		foreach (points,p)
			strPoints << p->x << ','<<p->y<<' ';
		return Element(node.AddChild("polygon")
				.SetMore("points",strPoints.ToStringRef())
				);
	}

	Element BaseElement::CreatePolygon(const Rect<>&r)
	{
		DBG_ASSERT__(IsGroup());
		static StringBuffer strPoints;
		strPoints.Clear();
		strPoints << r.x.min << ','<< r.y.min<<' ';
		strPoints << r.x.max << ','<< r.y.min<<' ';
		strPoints << r.x.max << ','<< r.y.max<<' ';
		strPoints << r.x.min << ','<< r.y.max<<' ';

		return Element(node.AddChild("polygon")
				.SetMore("points",strPoints.ToStringRef())
				);
	}

	Element BaseElement::CreateCircle(const TVec2<>& center, float radius)
	{
		DBG_ASSERT__(IsGroup());
		return Element(node.AddChild("circle")
				.SetMore("cx",center.x)
				.SetMore("cy",center.y)
				.SetMore("r",radius)
				);

	}

	Element BaseElement::CreateText(const TVec2<>&p, const String & text)
	{
		DBG_ASSERT__(IsGroup());
		Element rs = Element(node.AddChild("text")
				.SetMore("x",p.x)
				.SetMore("y",p.y)
				);
		rs.node.inner_content= text;
		return rs;
	}

	Element & Element::NoStroke()
	{
		node.Set("stroke","none");
		return *this;
	}

	Element & Element::Stroke(const float3 &c)
	{
		SetRGB("stroke",c);
		return *this;
	}

	Element & Element::Stroke(const float4 &c)
	{
		SetRGB("stroke",c.rgb);
		node.Set("stroke-opacity",c.a);
		return *this;
	}

	Element & Element::NoFill()
	{
		node.Set("fill","none");
		return *this;
	}

	Element & Element::SetFontSize(float size)
	{
		node.Set("font-size",size);
		return *this;
	}

	Element & Element::SetFontFamily(const String &family)
	{
		node.Set("font-family",family);
		return *this;
	}

	Element & Element::SetTextAnchor(const String &a)
	{
		node.Set("text-anchor",a);
		return *this;
	}

	Element & Element::Fill(const float3 &c)
	{
		SetRGB("fill",c);
		return *this;
	}

	Element & Element::Fill(const float4 &c)
	{
		SetRGB("fill",c.rgb);
		node.Set("fill-opacity",c.a);
		return *this;
	}

	Element & Element::FillPattern(const String & patternName)
	{
		node.Set("fill","url(#"+patternName+")");
		return *this;
	}

	Element & Element::Scale(const TVec2<>&s)
	{
		TransformFront("scale("+String(s.x)+' '+String(s.y)+")");
		return *this;
	}

	Element & Element::Translate(const TVec2<>&t)
	{
		TransformFront("translate("+String(t.x)+' '+String(t.y)+")");
		return *this;
	}

	Element & Element::Rotate(float angle)
	{
		TransformFront("rotate("+String(angle)+")");
		return *this;
	}

	Element & Element::Rotate(float angle, const TVec2<>&pivot)
	{
		TransformFront("rotate("+String(angle)+' '+String(pivot.x)+' '+String(pivot.y)+")");
		return *this;
	}

	Element & Element::SkewX(float v)
	{
		TransformFront("skewX("+String(v)+")");
		return *this;
	}

	Element & Element::SkewY(float v)
	{
		TransformFront("skewY("+String(v)+")");
		return *this;
	}

	Element & Element::SetStrokeWidth(float w)
	{
		node.Set("stroke-width",w);
		return *this;
	}

	static void TransMatrix(TMatrix3<>&mat, float m00,float m10,float m01, float m11,float m02, float m12)
	{
		TMatrix3<> sub,temp;
		sub.x.x = m00;
		sub.x.y = m10;
		sub.y.x = m01;
		sub.y.y = m11;
		sub.z.x = m02;
		sub.z.y = m12;
		Mat::resetBottomRow(sub);
		Mat::mult(mat,sub,temp);
		mat = temp;
	}

	static void Include(Rect<>&r, const TMatrix3<>&transform, const TVec2<>&p)
	{
		TVec2<> final=transform.z.xy;
		Vec::mad(final,transform.x.xy,p.x);
		Vec::mad(final,transform.y.xy,p.y);
		r.Include(final);
	}
	static void Include(Rect<>&r, const TMatrix3<>&transform, const Rect<>&outline)
	{
		Include(r,transform,outline.min());
		Include(r,transform,float2(outline.x.max,outline.y.min));
		Include(r,transform,outline.max());
		Include(r,transform,float2(outline.x.min,outline.y.max));
	}

	static void Get(const XML::Node&n, const char*p, float&rs)
	{
		StringRef val;
		ASSERT1__(n.Query(p,val),p);
		ASSERT1__(Convert(val,rs),val);
	}

	static TVec2<> MakePoint(const XML::Node&n, const char*p0, const char*p1)
	{
		TVec2<> rs;
		Get(n,p0,rs.x);
		Get(n,p1,rs.y);
		return rs;
	}

	Rect<> BaseElement::GetBoundingBox(TMatrix3<> transform, float strokeWidth, float fontSize, const String&textAnchor) const
	{
		StringRef attrib;
		if (node.Query("stroke-width",attrib))
			ASSERT1__(Convert(attrib,strokeWidth),attrib);
		if (node.Query("font-size",attrib))
			ASSERT1__(Convert(attrib,fontSize),attrib);
		String localAnchor;
		const String*outAnchor = &textAnchor;
		if (node.Query("text-anchor",attrib) && attrib.compareSegment("inherit",7)!=0)
		{
			localAnchor = attrib;
			outAnchor = &localAnchor;
		}

		StringRef strans;
		if (node.Query("transform",strans))
		{
			static Tokenizer::Configuration config;
			if (!config.recursion_break)
			{
				config.recursion_up = '(';
				config.recursion_down = ')';
				config.recursion_break = true;
				config.main_separator = " ,";
			}
			static Buffer0<StringRef> parts;
			ASSERT__(Tokenizer::Tokenize(strans,config,parts));

			ASSERT__(!(parts.Count()%2));
			for (index_t i = 0; i < parts.Count(); i+=2)
			{
				static Buffer0<StringRef> sp;
				StringRef stringParameters = parts[i+1];
				if (stringParameters.GetFirstChar() == '(' && stringParameters.GetLastChar() == ')')
				{
					stringParameters.DropFirstChar();
					stringParameters.DropLastChar();
				}
				ASSERT__(Tokenizer::Tokenize(stringParameters,config,sp));
				Array<float> p(sp.Count());
				for (index_t i = 0; i < sp.Count(); i++)
				{
					const StringRef&s = sp[i];
					if (!Convert(s,p[i]))
					{
						Tokenizer::Tokenize(stringParameters,config,sp);

					}
				}

				const StringRef&trans = parts[i];
				if (trans == "translate")
				{
					ASSERT_EQUAL__(p.Count(),2);
					TransMatrix(transform,1,0,0,1,p.First(),p.Last());
				}
				elif (trans == "matrix")
				{
					ASSERT_EQUAL__(p.Count(),6);
					TransMatrix(transform,p[0],p[1],p[2],p[3],p[4],p[5]);
				}
				elif (trans == "scale")
				{
					ASSERT_EQUAL__(p.Count(),2);
					TransMatrix(transform,p[0],0,0,p[1],0,0);
				}
				elif (trans == "rotate")
				{
					ASSERT_GREATER_OR_EQUAL__(p.Count(),1);
					if (p.Count() == 3)
					{
						TransMatrix(transform,1,0,0,1,p[1],p[2]);
					}
					float a= p.First()*M_PIF/180.f;
					TransMatrix(transform,cos(a),sin(a),-sin(a),cos(a),0,0);
					if (p.Count() == 3)
					{
						TransMatrix(transform,1,0,0,1,-p[1],-p[2]);
					}
				}
				elif (trans == "skewY")
				{
					float a= p.First()*M_PIF/180.f;
					TransMatrix(transform,1,tan(a),0,1,0,0);
				}
				elif (trans == "skewX")
				{
					float a= p.First()*M_PIF/180.f;
					TransMatrix(transform,1,0,tan(a),1,0,0);
				}
				else
					FATAL__("Unsupported transformation '"+trans+"'");
			}
		}
		const float strokeExtend = strokeWidth * sqrt(vmax(Vec::dot(transform.x.xy),Vec::dot(transform.y.xy))) / 2.f;
		Rect<> rs = Rect<>::Invalid;
		if (node.name=="line")
		{
			Rect<> line = Rect<>::Invalid;
			Include(line, transform,MakePoint(node,"x1","y1"));
			Include(line, transform,MakePoint(node,"x2","y2"));
			line.Expand(strokeExtend);
			rs.Include(line);

		}
		elif (node.name=="polygon" || node.name =="polyline")
		{
			const String&spoints = node.attributes["points"]->value;
			Array<StringRef> points,coords;
			explode(' ',spoints,points);
			Rect<> primitive = Rect<>::Invalid;
			foreach (points,p)
			{
				explode(',',*p,coords);
				ASSERT_EQUAL__(coords.Count(),2);
				TVec2<> p2;
				ASSERT1__(Convert(coords[0],p2.x),coords[0]);
				ASSERT1__(Convert(coords[1],p2.y),coords[1]);
				Include(primitive,transform,p2);
			}
			primitive.Expand(strokeExtend);
			rs.Include(primitive);
		}
		elif (node.name=="image")
		{
			float x,y,w,h;
			Get(node,"x",x);
			Get(node,"y",y);
			Get(node,"width",w);
			Get(node,"height",h);
			Rect<> primitive = Rect<>::Invalid;
			Include(primitive,transform,float2(x,y));
			Include(primitive,transform,float2(x+w,y));
			Include(primitive,transform,float2(x+w,y+h));
			Include(primitive,transform,float2(x,y+h));
			primitive.Expand(strokeExtend);
			rs.Include(primitive);
		}
		elif (node.name=="circle")
		{
			float x,y,r;
			Get(node,"cx",x);
			Get(node,"cy",y);
			Get(node,"r",r);
			Rect<> primitive = Rect<>::Invalid;
			Include(primitive,transform,float2(x-r,y-r));
			Include(primitive,transform,float2(x+r,y-r));
			Include(primitive,transform,float2(x+r,y+r));
			Include(primitive,transform,float2(x-r,y+r));
			primitive.Expand(strokeExtend);
			rs.Include(primitive);
		}
		elif (node.name=="text")
		{
			float x,y,s;
			Get(node,"x",x);
			Get(node,"y",y);
			
			Rect<> primitive = Rect<>::Invalid;
			Rect<> rect;
			const float w = fontSize*0.5f *node.inner_content.length();
			if (*outAnchor == "start")
				rect = Rect<>(x,y-fontSize,x+w,y);
			elif (*outAnchor == "middle")
				rect = Rect<>(x-w/2.f,y-fontSize,x+w/2.f,y);
			elif (*outAnchor == "end")
				rect = Rect<>(x-w,y-fontSize,x,y);
			else
				return rs;
			Include(primitive,transform,rect);
			primitive.Expand(strokeExtend);
			rs.Include(primitive);
		}
		elif (IsGroup())
		{
			foreach(node.children,ch)
			{
				if (ch->name!="defs")
					rs.Include(Element(*ch).GetBoundingBox(transform,strokeWidth,fontSize,*outAnchor));
			}
		}
		else
			FATAL__("Unsupported primitive '"+node.name+"'");
		return rs;
	}


	void		Document::AutoDetectDocumentSize(const TVec2<>&extra)
	{
		Rect<> rect = this->GetBoundingBox();
		DefineCustomDocumentSize(rect.max()+extra);
	}

	void		Document::DefineCustomDocumentSize(const TVec2<>&size)
	{
		container.root_node.SetMore("width",size.x);
		container.root_node.SetMore("height",size.y);
	}

	void		Document::SaveToFile(const PathString&path) const
	{
		container.SaveToFile(path);
	}

	Element SVG::Document::CreatePattern(const String & name, const Rect<>&rect)
	{
		XML::Node* defs=container.root_node.Find("defs");
		return Element(defs->AddChild("pattern")
						.SetMore("id",name)
						.SetMore("x",rect.x.min)
						.SetMore("y",rect.y.min)
						.SetMore("width",rect.x.GetExtend())
						.SetMore("height",rect.y.GetExtend())
						.SetMore("patternUnits","userSpaceOnUse")
						.AddChild("g")
					);
	}

}
