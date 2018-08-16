#ifndef NO_WINDOW

#include <global_root.h>
#include "scene.h"
#include <gl/gl_generators.h>
#include <engine/display.h>
#include "types.h"
#include <image/converter/magic.h>

extern Engine::Display<Engine::OpenGL>	display;


namespace Scene
{
	const constexpr float LayerDelta = 12.f;

	namespace
	{
		class Layer
		{
		public:
			Buffer<Quad>	filled,outlined;
			Buffer<Cube>	cubes,filledCubes;
			Buffer<Line>	lines;
			Buffer<Sphere>	spheres;

			void			swap(Layer&other)
			{
				filled.swap(other.filled);
				outlined.swap(other.outlined);
				cubes.swap(other.cubes);
				filledCubes.swap(other.filledCubes);
				lines.swap(other.lines);
				spheres.swap(other.spheres);
			}
		};

		using namespace Math;

		Box<>	boundingBox;


		GLuint sphere=0,circle=0;
		Buffer0<Layer,Swap>	layers;
		float3 offset;
		Buffer0<float3> offsetBuffer;
		index_t currentLayer = 0;
		float4 color(1);
		
		void	Set(const Vertex&v)
		{
			glColor4fv(v.color.v);
			glTexCoord2fv(v.uv.v);
			glVertex3fv(v.coords.v);
		}
		
		void	SetZDelta(const Vertex&v, float z)
		{
			glColor4fv(v.color.v);
			glTexCoord2fv(v.uv.v);
			glVertex3f(v.coords.x,v.coords.y,v.coords.z+ z);
		}
		Vertex	SimpleVertex(const TVec2<>&relative)
		{
			Vertex rs;
			rs.color = color;
			rs.coords = offset;
			rs.coords.x += relative.x;
			rs.coords.y += relative.y;
			rs.uv = relative;

			boundingBox.Include(rs.coords + float3(0,0,currentLayer));
			return rs;
		}

		Quad	CreateRect(const TVec2<>&a,const TVec2<>&b,const TVec2<>&c,const TVec2<>&d,const Engine::OpenGL::Texture::Reference&ref, const Image*img)
		{
			Quad q;
			q.texture = ref;
			q.image = img;
			q.v[0] = SimpleVertex(a);
			q.v[1] = SimpleVertex(b);
			q.v[2] = SimpleVertex(c);
			q.v[3] = SimpleVertex(d);
			return q;
		}


		Quad	CreateRect(const Rect<>&r, const Engine::OpenGL::Texture::Reference&ref, const Image*img)
		{
			Quad q;
			q.texture = ref;
			q.image = img;
			q.v[0] = SimpleVertex(float2(r.x.min,r.y.min));
			q.v[1] = SimpleVertex(float2(r.x.max,r.y.min));
			q.v[2] = SimpleVertex(float2(r.x.max,r.y.max));
			q.v[3] = SimpleVertex(float2(r.x.min,r.y.max));
			return q;
		}

		Quad	CreateRect(const Engine::OpenGL::Texture::Reference&r, const Image*img)
		{
			return CreateRect(Rect<>(0,0,1,1),r,img);
		}

		void	RenderLayer(const Layer&l, float h)
		{
			glPushMatrix();
				glTranslatef(0,0,h);


				foreach (l.filled,r)
				{
					display.useTexture(r->texture,true);
					glBegin(GL_QUADS);
						for (int i = 0; i < 4; i++)
							Set(r->v[i]);
					glEnd();
				}
				display.useTexture(nullptr);

				glDepthMask(GL_TRUE);
				glBegin(GL_QUADS);
				foreach (l.filledCubes,r)
				{
					for (int i = 0; i < 4; i++)
						SetZDelta(r->base.v[i],r->height);
					for (index_t i = 0; i < 4; i++)
					{
						index_t i2 = (i+1)%4;
						Set(r->base.v[i]);
						Set(r->base.v[i2]);
						SetZDelta(r->base.v[i2],r->height);
						SetZDelta(r->base.v[i],r->height);
					}
				}
				glEnd();
				glDepthMask(GL_FALSE);

				foreach (l.outlined,r)
				{
					glBegin(GL_LINE_LOOP);
						for (int i = 0; i < 4; i++)
							Set(r->v[i]);
					glEnd();
				}
				foreach (l.cubes,r)
				{
					glBegin(GL_LINE_LOOP);
						for (int i = 0; i < 4; i++)
							Set(r->base.v[i]);
					glEnd();
					glBegin(GL_LINE_LOOP);
						for (int i = 0; i < 4; i++)
							SetZDelta(r->base.v[i],r->height);
					glEnd();
					glBegin(GL_LINES);
						for (int i = 0; i < 4; i++)
						{
							Set(r->base.v[i]);
							SetZDelta(r->base.v[i],r->height);
						}
					glEnd();
				}
				glBegin(GL_LINES);
				foreach (l.lines,line)
				{
					Set(line->v[0]);
					Set(line->v[1]);
				}
				glEnd();

				foreach (l.spheres,sph)
				{
					glColor4fv(sph->center.color.v);
					glPushMatrix();
					glTranslatef(sph->center.coords.x,sph->center.coords.y,0);
					Engine::glScale(sph->radius);
					glCallList(sph->isCircle ? circle : sphere);
					glPopMatrix();
				}
			glPopMatrix();
			
		}

		String	Channel(float v)
		{
			return (int)clamp(v*255,0,255);
		}

		String Color(const TVec3<>&c)
		{
			return "rgb("+Channel(c.r)+","+Channel(c.g)+","+Channel(c.b)+")";
		}


		void	BuildSVGLayer(const Layer&l, float h, XML::Node&xml, const PathString&imageFolder)
		{

			XML::Node&group = xml.AddChild("g");
			group.Set("transform","translate("+String(-boundingBox.x.min + boundingBox.y.max)+" "+String(h-boundingBox.y.min-boundingBox.z.min)+") skewX(-45)");
			foreach (l.filled,r)
			{
				if (r->image)
				{
					GUID	guid;
					CoCreateGuid(&guid);
					DBG_ASSERT__(imageFolder.LastChar() ==  (PathString::char_t) '\\');
					PathString name = imageFolder + ToString(guid,true)+".png";
					Image image2 = *r->image;
					image2.ScaleDouble();
					image2.ScaleDouble();
					for (UINT32 x = 0; x < image2.GetWidth(); x++)
						for (UINT32 y = 0; y < image2.GetHeight(); y++)
						{
							BYTE&px = *image2.get(x,y);
							if (px == 0)
								px = 255;
							else
								px = 128 + px/2;
							if (((image2.GetHeight() + x-y)/4)%2)
								px = 255;
							//*px = 255 - *px /2;
						}
					png.SaveToFileQ(image2,name);
					group.AddChild("image")
							.SetMore("xlink:href",String(name))
							.SetMore("x", r->v[0].coords.x)
							.SetMore("y", r->v[0].coords.y)
							.SetMore("width", r->v[2].coords.x-r->v[0].coords.x)
							.SetMore("height", r->v[2].coords.y-r->v[0].coords.y)
							;
				}
				else
				{
					static StringBuffer points;
					points.Clear();
					points << r->v[0].coords.x << ','<<r->v[0].coords.y<<' ';
					points << r->v[1].coords.x << ','<<r->v[1].coords.y<<' ';
					points << r->v[2].coords.x << ','<<r->v[2].coords.y<<' ';
					points << r->v[3].coords.x << ','<<r->v[3].coords.y;
					group.AddChild("polygon")
						.SetMore("points",points.ToStringRef())
						.SetMore("stroke","none")
						.SetMore("fill",Color(r->v[0].color.rgb))
						.SetMore("fill-opacity",r->v[0].color.a)
						;
				}
			}

			foreach (l.outlined,r)
			{
				static StringBuffer points;
				points.Clear();
				points << r->v[0].coords.x << ','<<r->v[0].coords.y<<' ';
				points << r->v[1].coords.x << ','<<r->v[1].coords.y<<' ';
				points << r->v[2].coords.x << ','<<r->v[2].coords.y<<' ';
				points << r->v[3].coords.x << ','<<r->v[3].coords.y;
				group.AddChild("polygon")
					.SetMore("points",points.ToStringRef())
					.SetMore("stroke",Color(r->v[0].color.rgb))
					.SetMore("stroke-opacity",r->v[0].color.a)
					.SetMore("stroke-width","0.06")
					.SetMore("fill","none")
 
					;
			}

			//foreach (l.cubes,r)
			//{
			//	glBegin(GL_LINE_LOOP);
			//		for (int i = 0; i < 4; i++)
			//			Set(r->base.v[i]);
			//	glEnd();
			//	glBegin(GL_LINE_LOOP);
			//		for (int i = 0; i < 4; i++)
			//			SetZDelta(r->base.v[i],r->height);
			//	glEnd();
			//	glBegin(GL_LINES);
			//		for (int i = 0; i < 4; i++)
			//		{
			//			Set(r->base.v[i]);
			//			SetZDelta(r->base.v[i],r->height);
			//		}
			//	glEnd();
			//}

			foreach (l.lines,r)
			{		
				group.AddChild("line")
					.SetMore("x1",r->v[0].coords.x)
					.SetMore("y1",r->v[0].coords.y)
					.SetMore("x2",r->v[1].coords.x)
					.SetMore("y2",r->v[1].coords.y)
					.SetMore("stroke",Color(r->v[0].color.rgb));
					;
			}

			foreach (l.spheres,r)
			{
				String color = Color(r->center.color.rgb);
				//<circle cx="50" cy="50" r="40" stroke="black" stroke-width="3" fill="red" />
				group.AddChild("circle")
					.SetMore("cx",r->center.coords.x)
					.SetMore("cy",r->center.coords.y)
					.SetMore("r",r->radius)
					.SetMore("stroke",r->isCircle ? color : "none")
					.SetMore("fill",r->isCircle ? "white" : color)
					.SetMore("fill-opacity",r->center.color.a)
					.SetMore("stroke-opacity",r->center.color.a)
					;
			}
			
		}
	}


	count_t CountLayers()
	{
		return layers.Count();
	}

	void Create()
	{
		sphere = Generator::glSphere(1.f,5);
		circle = Generator::glCircle(1.f,20,2);
		layers.Append();
	}

	void Clear()
	{
		layers.Clear();
		layers.Append();
		boundingBox = Box<>::Invalid();
	}


	void	BuildSVG(XML::Node&c, const PathString&imageFolder, float2&outBounds)
	{
		outBounds.x = vmax(outBounds.x,(boundingBox.x.GetExtent() + boundingBox.y.GetExtent()) * 50.f);
		outBounds.y = vmax(outBounds.y,(boundingBox.z.GetExtent()*0.75f + boundingBox.y.GetExtent()) * 35.f);

		XML::Node&g = c.AddChild("g").SetMore("transform","scale(50 35)").SetMore("stroke-width","0.03");

		for (index_t i = 0; i < layers.Count(); i++)
		{
			float h = 0.75f 
					* (layers.Count()-1 - i);
			BuildSVGLayer(layers[i],h, g, imageFolder);
		}

	}


	void Render(const Engine::Camera<>&c)
	{
		float center = (layers.Count()-1)/2.f * LayerDelta;
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		for (index_t i = 0; i < layers.Count(); i++)
		{
			float h = -LayerDelta * (layers.Count()-1 - i);
			h+= center;
			if (h >= c.GetAbsoluteLocation().z)
				break;
			RenderLayer(layers[i],h);
		}
		for (index_t i = layers.Count()-1; i < layers.Count(); i--)
		{
			float h = -LayerDelta * (layers.Count()-1 - i);
			h+= center;
			if (h < c.GetAbsoluteLocation().z)
				break;
			RenderLayer(layers[i],h);
		}
		glDepthMask(GL_TRUE);
	}
	void PushOffset()
	{
		ASSERT__(offsetBuffer.Count() < 64);
		offsetBuffer << offset;
	}
	void AlterOffset(const TVec2<>&v)
	{
		offset.xy += v;
	}
	void AlterOffset(const TVec3<>&v)
	{
		offset += v;
	}
	void AlterZOffset(float z)
	{
		offset.z += z;
	}

	void PopOffset()
	{
		ASSERT__(offsetBuffer.Count() > 0);
		offset = offsetBuffer.Pop();
	}

	index_t GetLayer()
	{
		return currentLayer;
	}

	void SetLayer(index_t index)
	{
		while (layers.Count() <= index)
			layers.Append();
		currentLayer = index;
	}
	void SetColor3ub(BYTE r, BYTE g, BYTE b)
	{
		color = float4(r,g,b,255)/255.f;
	}
	void	SetColor(const TVec3<>&rgb, float a /*= 1.f*/)
	{
		color.rgb = rgb;
		color.a = a;
	}
	void	SetColor(const TVec4<>&rgba)
	{
		color = rgba;
	}

	void SetColor(float r, float g, float b, float a)
	{
		color = float4(r,g,b,a);
	}
	void PutSphere(float radius)
	{
		Sphere sphere;
		sphere.center.coords = offset;
		sphere.center.color = color;
		sphere.radius = radius;

		float3 l = float3(0,0,currentLayer);

		boundingBox.Include(Box<>(offset - radius + l,offset+radius+l));

		layers[currentLayer].spheres.Append(sphere);
	}
	void PutCircle(float radius)
	{
		Sphere sphere;
		sphere.center.coords = offset;
		sphere.center.color = color;
		sphere.radius = radius;
		sphere.isCircle = true;
		layers[currentLayer].spheres.Append(sphere);
	}
	void PutRegularOutlineRect()
	{
		layers[currentLayer].outlined << CreateRect(Engine::OpenGL::Texture::Reference(),nullptr);
	}

	static void SetRotatedRect(Quad&q,float orientation, const float2&scale)
	{
		float3 x = float3(cos(orientation),sin(orientation),0.f);
		float3 y = float3(float2::Reinterpret(x.xy).Normal(),0.f);
		x *= scale.x;
		y *= scale.y;

		float3 l = float3(0,0,currentLayer);
		boundingBox.Include(Box<>(offset-x-y+l, offset+x+y+l ));

		q.v[0].coords = offset - x - y;
		q.v[1].coords = offset + x - y;
		q.v[2].coords = offset + x + y;
		q.v[3].coords = offset - x + y;
		q.v[0].color = color;
		q.v[1].color = color;
		q.v[2].color = color;
		q.v[3].color = color;
	}

	void	PutRotatedOutlineRect(float orientation, const float2&scale)
	{
		auto&q = layers[currentLayer].outlined.Append();
		SetRotatedRect(q,orientation,scale);
	}
	void	PutRotatedOutlineCube(float orientation, const float3&scale)
	{
		auto&c = layers[currentLayer].cubes.Append();
		float2 x = float2(cos(orientation),sin(orientation));
		float2 y = x.Normal();
		x *= scale.x;
		y *= scale.y;
		auto&q = c.base;
		SetRotatedRect(q,orientation,scale.xy);
		c.height = scale.z;
	}
	void	PutRotatedFilledCube(float orientation, const float3&scale)
	{
		auto&c = layers[currentLayer].filledCubes.Append();
		float2 x = float2(cos(orientation),sin(orientation));
		float2 y = x.Normal();
		x *= scale.x;
		y *= scale.y;
		auto&q = c.base;
		SetRotatedRect(q,orientation,scale.xy);
		c.height = scale.z;
	}

	void	PutInterlayerBeam(index_t targetLayer, float radius, float heightFactor/*=1.f*/)
	{
		const index_t from = std::min(targetLayer,currentLayer);
		const index_t to = std::max(targetLayer,currentLayer);

		auto&c = layers[from].filledCubes.Append();
		SetRotatedRect(c.base,0,float2(radius));
		c.height = heightFactor* LayerDelta;
	}


	void	PutRotatedFilledRect(float orientation, const float2&scale)
	{
		auto&q = layers[currentLayer].filled.Append();
		SetRotatedRect(q,orientation,scale);
	}

	void	PutFilledQuad(const TVec2<>&a, const TVec2<>&b, const TVec2<>&c, const TVec2<>&d)
	{
		layers[currentLayer].filled << CreateRect(a,b,c,d,Engine::OpenGL::Texture::Reference(), nullptr);
	}

	void	PutOutlinedQuad(const TVec2<>&a, const TVec2<>&b, const TVec2<>&c, const TVec2<>&d)
	{
		layers[currentLayer].outlined << CreateRect(a,b,c,d,Engine::OpenGL::Texture::Reference(), nullptr);
	}

	void PutFilledRect(const Rect<>&r)
	{
		layers[currentLayer].filled << CreateRect(r,Engine::OpenGL::Texture::Reference(), nullptr);
	}
	void PutOutlinedRect(const Rect<>&r)
	{
		layers[currentLayer].outlined << CreateRect(r,Engine::OpenGL::Texture::Reference(), nullptr);
	}
	void PutRegularFilledRect()
	{
		layers[currentLayer].filled << CreateRect(Engine::OpenGL::Texture::Reference(), nullptr);
	}
	void PutRegularTexturedRect(const Engine::OpenGL::Texture::Reference &r, const Image&img)
	{
		layers[currentLayer].filled << CreateRect(r,&img);
	}
	void PutLineTo(const TVec2<>&target)
	{
		Line&line = layers[currentLayer].lines.Append();
		line.v[0] = SimpleVertex(float2());
		line.v[1] = SimpleVertex(target);
	}
}





namespace Scene3D
{
	typedef float3 Color;

	float3	currentOffset;
	Color	currentColor;


	struct TPrimitive
	{
		Color	color = currentColor;
	};

	struct TBox : public TPrimitive
	{
		M::Box<>space;
	};

	struct TLine : public TPrimitive
	{
		float3	p0,p1;
	};

	Buffer0<TBox>	solidBoxes,wireBoxes;
	Buffer0<TLine>	lines;


	void	SetOffset(float x, float y, float z)
	{
		Vec::def(currentOffset,x,y,z);
	}

	void	Clear()
	{
		wireBoxes.Clear();
		solidBoxes.Clear();
		lines.Clear();
	}

	void	PutLine(const float3&p0, const float3&p1)
	{
		TLine&l = lines.Append();
		l.p0 = p0 + currentOffset;
		l.p1 = p1 + currentOffset;
	}

	void	PutWireframe(const M::Box<>&box)
	{
		TBox&b = wireBoxes.Append();
		b.space = box;
		b.space.Translate(currentOffset);
	}

	void	PutSolid(const M::Box<>&box)
	{
		TBox&b = solidBoxes.Append();
		b.space = box;
		b.space.Translate(currentOffset);
	}

	void	SetColor(float r, float g, float b)
	{
		Vec::def(currentColor,r,g,b);
	}

	void PutVertex(const Color&c, float x, float y, float z)
	{
		glColor3fv(c.v);
		glVertex3f(x,y,z);
	}
	void PutVertex(const Color&c, const M::TVec3<>&p)
	{
		glColor3fv(c.v);
		glVertex3fv(p.v);
	}

	void	Render()
	{
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
		foreach (solidBoxes,b)
		{
			PutVertex(b->color, b->space.x.min,b->space.y.min,b->space.z.min);
			PutVertex(b->color, b->space.x.min,b->space.y.max,b->space.z.min);
			PutVertex(b->color, b->space.x.max,b->space.y.max,b->space.z.min);
			PutVertex(b->color, b->space.x.max,b->space.y.min,b->space.z.min);
			PutVertex(b->color, b->space.x.min,b->space.y.max,b->space.z.max);
			PutVertex(b->color, b->space.x.min,b->space.y.min,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.min,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.max,b->space.z.max);


			PutVertex(b->color, b->space.x.min,b->space.y.min,b->space.z.min);
			PutVertex(b->color, b->space.x.min,b->space.y.min,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.min,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.min,b->space.z.min);
			PutVertex(b->color, b->space.x.min,b->space.y.max,b->space.z.min);
			PutVertex(b->color, b->space.x.min,b->space.y.max,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.max,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.max,b->space.z.min);

			PutVertex(b->color, b->space.x.min,b->space.y.min,b->space.z.min);
			PutVertex(b->color, b->space.x.min,b->space.y.max,b->space.z.min);
			PutVertex(b->color, b->space.x.min,b->space.y.max,b->space.z.max);
			PutVertex(b->color, b->space.x.min,b->space.y.min,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.min,b->space.z.min);
			PutVertex(b->color, b->space.x.max,b->space.y.max,b->space.z.min);
			PutVertex(b->color, b->space.x.max,b->space.y.max,b->space.z.max);
			PutVertex(b->color, b->space.x.max,b->space.y.min,b->space.z.max);

		}
		glEnd();
		
		
		foreach (wireBoxes,b)
		{
			glBegin(GL_LINE_LOOP);
				PutVertex(b->color,b->space.x.min,b->space.y.min,b->space.z.min);
				PutVertex(b->color,b->space.x.min,b->space.y.min,b->space.z.max);
				PutVertex(b->color,b->space.x.min,b->space.y.max,b->space.z.max);
				PutVertex(b->color,b->space.x.min,b->space.y.max,b->space.z.min);
			glEnd();
			glBegin(GL_LINE_LOOP);
				PutVertex(b->color,b->space.x.max,b->space.y.min,b->space.z.min);
				PutVertex(b->color,b->space.x.max,b->space.y.min,b->space.z.max);
				PutVertex(b->color,b->space.x.max,b->space.y.max,b->space.z.max);
				PutVertex(b->color,b->space.x.max,b->space.y.max,b->space.z.min);
			glEnd();
			glBegin(GL_LINES);
				PutVertex(b->color,b->space.x.min,b->space.y.min,b->space.z.min);
				PutVertex(b->color,b->space.x.max,b->space.y.min,b->space.z.min);
				PutVertex(b->color,b->space.x.min,b->space.y.min,b->space.z.max);
				PutVertex(b->color,b->space.x.max,b->space.y.min,b->space.z.max);
				PutVertex(b->color,b->space.x.min,b->space.y.max,b->space.z.max);
				PutVertex(b->color,b->space.x.max,b->space.y.max,b->space.z.max);
				PutVertex(b->color,b->space.x.min,b->space.y.max,b->space.z.min);
				PutVertex(b->color,b->space.x.max,b->space.y.max,b->space.z.min);
			glEnd();
		}

		glBegin(GL_LINES);
			foreach (lines,l)
			{
				PutVertex(l->color,l->p0);
				PutVertex(l->color,l->p1);
			}
		glEnd();

	}
}

#endif
