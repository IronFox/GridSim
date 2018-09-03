#include <global_root.h>
#include "rendering.h"
#include <math/vclasses.h>



Engine::Display<Renderer> 				display;
Engine::Textout<Engine::GLTextureFont2>			textout;
Engine::AngularCamera<float>			camera;
Engine::OrthographicAspect<float>		hud;
Engine::VectorCamera<float>				shadow_aspect0,
								shadow_aspect1;

Engine::Profiler::StackedGraph<Renderer,Engine::GLTextureFont2>	fps_graph(&Engine::Profiler::fps_data);	//fps graph object

Engine::GLShader::Instance	shader,back_shader,holeShader,wallShader;


Renderer::FBO		shadow_fbo0,shadow_fbo1;

Geometry				scenery,holeScenery,wallScenery,transparentScenery;


//M::Box<>				range = M::Box<>(0,0,0,/*T=*/128,/*S=*/256,1.1);
M::Box<>				range = M::Box<>(0,0,0,/*T=*/64,/*S=*/64,1.1);
float3					markerSteps = float3(16, 16, 0.2);
float3					markerLabelFactor = float3(1,0.5,1);
M::TVec3<const char*>	arrowLabel = {"T","S","P"};

enum class Axis
{
	X,
	Y,
	Z,
};

struct TLabel
{
	float3	coords;
	float	value;
	const char*	cValue=nullptr;
	bool	isFloat = true;
	static bool	enabled;
	static float3 translation;
	static M::TMatrix4<> system;
};

float3 TLabel::translation;
M::TMatrix4<> TLabel::system = M::Matrix<>::eye4;
bool	TLabel::enabled = true;

Buffer0<TLabel>	labels;


void RenderShadows()
{
	glDisable(GL_FACE_CULL);
	glColorMask(false,false,false,false);
	display.Pick(shadow_aspect0);
		display.TargetFBO(shadow_fbo0);
			glEnable(GL_DEPTH_TEST);
			glViewport(0,0,ShadowResolution,ShadowResolution);
			glEnable(GL_CULL_FACE);
			glClear(GL_DEPTH_BUFFER_BIT);
			scenery.Render();
			holeScenery.Render();
//			wallScenery.RenderIgnoreMaterials();
		display.TargetBackbuffer();
	display.Pick(shadow_aspect1);
		display.TargetFBO(shadow_fbo1);
			glEnable(GL_DEPTH_TEST);
			glViewport(0,0,ShadowResolution,ShadowResolution);
			glEnable(GL_CULL_FACE);
			glClear(GL_DEPTH_BUFFER_BIT);
			scenery.Render();
			holeScenery.Render();
//			wallScenery.RenderIgnoreMaterials();
		display.TargetBackbuffer();
		
	glColorMask(true,true,true,true);
}


void RenderPlot()
{
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
			M::TMatrix4<> matrix;
			Mat::Mult(shadow_aspect0.projection,shadow_aspect0.view,matrix);
			glLoadMatrixf(matrix.v);
			display.useTexture(shadow_fbo0.ReferDepth());
			glEnable(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE,GL_INTENSITY);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
				//glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
				//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
		glActiveTexture(GL_TEXTURE1);
			Mat::Mult(shadow_aspect1.projection,shadow_aspect1.view,matrix);
			glLoadMatrixf(matrix.v);
			display.useTexture(shadow_fbo1.ReferDepth());
			glEnable(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE,GL_INTENSITY);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_R_TO_TEXTURE);
				//glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
				//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_MODELVIEW);


	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glsl(wallShader)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		wallScenery.Render();
	}

	glsl(shader)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		//scenery.RenderIgnoreMaterials();
		//scenery.RenderIgnoreMaterials();
		scenery.Render();
		glDepthMask(GL_FALSE);
		transparentScenery.Render();
		transparentScenery.Render();
		glDepthMask(GL_TRUE);
	}

	glsl(holeShader)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		holeScenery.Render();
	}

	{
	if (DoubleSided)
		glsl(back_shader)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			scenery.Render();
			glDepthMask(GL_FALSE);
			transparentScenery.Render();
			transparentScenery.Render();
			glDepthMask(GL_TRUE);
			glCullFace(GL_BACK);
		}
	}	
	glMatrixMode(GL_TEXTURE);
		glActiveTexture(GL_TEXTURE0);
			glLoadIdentity();
			glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE1);
			glLoadIdentity();
			glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_MODELVIEW);

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void 	RenderAxisLabel(float x, float value, Axis axis)
{
	if (!TLabel::enabled)
		return;
	float3 global,projected;
	float3 p = float3(x, 0, 0);
	float dir = Vec::dot(TLabel::system.z.xyz, camera.GetAbsoluteLocation());

	//if (axis == Axis::X && dir > 0)
	//	p.z = 2;
	Mat::Transform(TLabel::system, p, global);
	camera.Project(global, projected);
	labels.Append().coords = projected;
	labels.Last().value = value * markerLabelFactor.v[(int)axis];
}

void 	PutStringLabel(float x, const char*value)
{
	if (!TLabel::enabled)
		return;
	float3 global, projected;
	Mat::Transform(TLabel::system, float3(x, 0, 0), global);
	camera.Project(global, projected);
	labels.Append().coords = projected;
	labels.Last().cValue = value;
	labels.Last().isFloat = false;
}


M::TMatrix4<> mat = M::Matrix<>::eye4;
void RenderAxis(const M::TFloatRange<>&range, float labelSteps, const M::TMatrix3<>&system, float length, const char*label, Axis axis)
{
	glPushMatrix();
		Mat::copyOrientation(system, mat);
		float dir = Vec::dot(system.z, camera.GetAbsoluteLocation());
		if (axis != Axis::Z && dir > 0)
			Vec::mul(system.z, 2, mat.w.xyz);
		else
			Vec::clear(mat.w.xyz);

		TLabel::system = mat;
		TLabel::system.w.xyz += TLabel::translation;

			//glTranslatef(xExtent, 0, 0);


		glMultMatrixf(mat.v);
		glBegin(GL_LINES);
			glVertex2f(0,0);
			glVertex2f(length + 0.1f,0);
		glEnd();

		static const float w = 0.03f;
		const float h = length + 0.1;
		Engine::glBlack();
		glBegin(GL_TRIANGLES);
			glVertex3f(h, w, w);
			glVertex3f(h, w, -w);
			glVertex3f(h + 0.1, 0, 0);

			glVertex3f(h, -w, w);
			glVertex3f(h, -w, -w);
			glVertex3f(h+0.1, 0, 0);

			glVertex3f(h, -w, -w);
			glVertex3f(h, w, -w);
			glVertex3f(h+0.1, 0, 0);

			glVertex3f(h, -w, w);
			glVertex3f(h, w, w);
			glVertex3f(h+0.1, 0, 0);
		glEnd();

		int begin = (int)ceil(range.min / labelSteps);
		int end = (int)ceil(range.max / labelSteps);
		if (!begin)
			begin++;	//skip 0
		for (int i = begin; i < end; i++)
		{
			RenderAxisLabel(range.Relativate(i*labelSteps)*length, i*labelSteps,axis);
			//break;
		}

		PutStringLabel(h + 0.2, label);


		Vec::clear(mat.w.xyz);


	glPopMatrix();

}


void RenderGrid(const M::TMatrix3<>&sys, float xExtent, const M::TFloatRange<>&yRange, const M::TFloatRange<>&zRange, float yMarkerSteps, float zMarkerSteps, float yExtent, float zExtent)
{
	float dir = Vec::dot(sys.x, camera.GetAbsoluteLocation());

	glPushMatrix();
		Mat::copyOrientation(sys, mat);
		glMultMatrixf(mat.v);
		if (dir < 0)
			glTranslatef(xExtent, 0, 0);

		int ybegin = (int)ceil(yRange.min / yMarkerSteps);
		int yend = (int)ceil(yRange.max / yMarkerSteps);

		int zbegin = (int)ceil(zRange.min / zMarkerSteps);
		int zend = (int)ceil(zRange.max / zMarkerSteps);

		float yBegin = yRange.Relativate(ybegin * yMarkerSteps)* yExtent;
		float yEnd = yRange.Relativate(yend * yMarkerSteps)* yExtent;

		float zBegin = zRange.Relativate(zbegin * zMarkerSteps)* zExtent;
		float zEnd = zRange.Relativate(zend * zMarkerSteps)* zExtent;

		glBegin(GL_LINE_LOOP);
			glVertex3f(0, 0, 0);
			glVertex3f(0,yExtent, 0);
			glVertex3f(0, yExtent, zExtent);
			glVertex3f(0, 0, zExtent);
		glEnd();

		glBegin(GL_LINES);
			for (int i = zbegin; i < zend; i++)
			{
				float z = zRange.Relativate(i * zMarkerSteps)* zExtent;
				glVertex3f(0, 0, z);
				glVertex3f(0, yExtent, z);
			}
			for (int i = ybegin; i < yend; i++)
			{
				float y = yRange.Relativate(i * yMarkerSteps)* yExtent;
				glVertex3f(0, y, 0);
				glVertex3f(0, y, zExtent);
			}
		glEnd();

	glPopMatrix();
}


void RenderExtendedAxes()
{
	labels.Clear();

	display.useTexture(NULL);

	glEnable(GL_DEPTH_TEST);
	glPushMatrix();
		TLabel::translation = float3(-1, -1, 0);
		glTranslatef(-1, -1, 0);
		Engine::glBlack();
		//glScalef(2, 2, 2);
		glDisable(GL_CULL_FACE);

		static const M::TMatrix3<> xSystem =
		{
			1,0,0,
			0,0,1,
			0,1,0,
		};
		static const M::TMatrix3<> zSystem =
		{
			0,0,1,
			1,0,0,
			0,1,0,
		};

		static const M::TMatrix3<> ySystem =
		{
			0,1,0,
			0,0,1,
			1,0,0,
		};
		textout.SetColor(0, 0, 0);
		
		textout.PushState();
		float fontZoom = 2;
		textout.Scale(fontZoom);

		RenderAxis(range.x, markerSteps.x, xSystem, 2, arrowLabel.x ,Axis::X);
		RenderAxis(range.y, markerSteps.y, ySystem, 2, arrowLabel.y, Axis::Y);
		RenderAxis(range.z, markerSteps.z, zSystem, 1, arrowLabel.z,Axis::Z);

		Engine::glBlack(0.5);
		RenderGrid(xSystem, 2, range.z, range.y, markerSteps.z, markerSteps.y, 1, 2);
		RenderGrid(ySystem, 2, range.z, range.x, markerSteps.z, markerSteps.x, 1, 2);
		RenderGrid(zSystem, 1, range.x, range.y, markerSteps.x, markerSteps.y, 2, 2);

		textout.PopState();

		glDisable(GL_DEPTH_TEST);

	glPopMatrix();
}

void RenderExtendedAxesLabels()
{
	display.Pick(hud);
	textout.PushState();
		textout.SetColor(1, 1, 1, 1);
		textout.Scale(3);
		foreach(labels, l)
		{
			textout.MoveTo(l->coords.x*0.5+0.5, l->coords.y*0.5+0.5);
			if (l->isFloat)
			{
				int s = M::Sign(l->value);
				float v =fabs(l->value);

				if (v == 0)
					textout << '0';
				else
				{
					int degree = log(v) / log(10.f);
					float factor = M::Pow10(degree-1);
					v = round(v / factor) * factor * s;
					textout << v;
				}
			}
			else
				textout << l->cValue;
		}
	textout.PopState();
}



void SetupRenderer()
{
	shadow_fbo0.Create(Resolution(ShadowResolution, ShadowResolution),Engine::DepthStorage::Texture,0,nullptr);
	shadow_fbo1.Create(Resolution(ShadowResolution, ShadowResolution),Engine::DepthStorage::Texture,0,nullptr);
		
	ASSERT__(shadow_fbo0.ReferDepth().IsNotEmpty());
	ASSERT__(shadow_fbo1.ReferDepth().IsNotEmpty());
		
	const float3				light_direction0 = float3{-0.577f,0.577f,0.577f}.Normalized(),
								//light_direction1 = float3{0.577f,0.577f,0.8f}.Normalized();
								light_direction1 = float3{0,0,1}.Normalized();

		
	static const constexpr float r = 1.7f;//1.5f;
		
	shadow_aspect0.projection.x *= 1.f/r;
	shadow_aspect0.projection.y *= 1.f/r;
	shadow_aspect0.projection.z *= -1.f/r;
	shadow_aspect1.projection = shadow_aspect0.projection;

	shadow_aspect0.direction = light_direction0;
	shadow_aspect1.direction = light_direction1;

	shadow_aspect0.UpdateView();
	shadow_aspect1.UpdateView();

	shadow_aspect0.UpdateInvert();
	shadow_aspect1.UpdateInvert();



	String intensityFunction =
		"const float m_fTextureSize=2048.0;\n"
		"const float m_fTexelSize=1.0/m_fTextureSize;\n"
		"float intensity(sampler2DShadow shadow, vec3 shadow_coord)\n"
		"{\n"
			"shadow_coord.z += 0.0001;\n"//z tolerance
			//"return shadow2D(shadow,shadow_coord).r;\n"
			"vec3 vShadowTexDDX = dFdx(shadow_coord);\n"
			"vec3 vShadowTexDDY = dFdy(shadow_coord);\n"
			"mat2 matScreentoShadow = mat2(vShadowTexDDX.xy, vShadowTexDDY.xy);\n"
			//"mat2 matShadowToScreen = inverse(	matScreentoShadow);\n"
			"float fDeterminant = determinant(matScreentoShadow);\n"

			"if (abs(fDeterminant) < 0.000000001) return shadow2D(shadow,shadow_coord).r;\n"

			"float fInvDeterminant = 1.0f / fDeterminant;\n"

			"mat2 matShadowToScreen = mat2("
				"matScreentoShadow._22 * fInvDeterminant,"
				"matScreentoShadow._12 * -fInvDeterminant,"
				"matScreentoShadow._21 * -fInvDeterminant,"
				"matScreentoShadow._11 * fInvDeterminant);\n"
			//"matShadowToScreen = transpose(matShadowToScreen);\n"
			"vec2 vRightShadowTexelLocation = vec2(m_fTexelSize, 0.0f);\n"
			"vec2 vUpShadowTexelLocation = vec2(0.0f, m_fTexelSize);\n"
			"vec2 vRightTexelDepthRatio = mul(matShadowToScreen, vRightShadowTexelLocation);\n"
			"vec2 vUpTexelDepthRatio = mul(matShadowToScreen, vUpShadowTexelLocation);\n"

			"float fUpTexelDepthDelta = vUpTexelDepthRatio.x * vShadowTexDDX.z+ vUpTexelDepthRatio.y * vShadowTexDDY.z;\n"
			"float fRightTexelDepthDelta = vRightTexelDepthRatio.x * vShadowTexDDX.z + vRightTexelDepthRatio.y * vShadowTexDDY.z;\n"


			//"const float step=0.001;\n"
			"float sum = 0.0;\n"
			"for (int x = -2; x <= 2; x++)\n"
				"for (int y = -2; y <= 2; y++)\n"
				"{\n"
					"vec2 uv = vec2(x,y) * m_fTexelSize;\n"
						
					"sum += shadow2D(shadow,shadow_coord+vec3(uv,fRightTexelDepthDelta * float(x) + fUpTexelDepthDelta * float(y))).r;\n"
				"}\n"
				
			"return max(0.0,sum/25.0);\n"
		"}\n"
	;


	if (!shader.LoadComposition(
		"[shared]\n"
			"varying float h;\n"
			"varying vec3 normal;\n"
			"varying vec4 color;\n"
			"varying vec3 shadow_coord0, shadow_coord1;\n"
			"uniform vec3 light0;\n"
			"uniform vec3 light1;\n"
		"[vertex]\n"
		"void main()\n"
		"{\n"
			"gl_Position = ftransform();\n"
			"h = gl_MultiTexCoord0.x;\n"
			"normal = gl_Normal;\n"
			"color = gl_Color;\n"
			"shadow_coord0 = (gl_TextureMatrix[0]*gl_Vertex).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
			"shadow_coord0.z -= 0.01;\n"
			"shadow_coord1 = (gl_TextureMatrix[1]*gl_Vertex).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
			"shadow_coord1.z -= 0.01;\n"
		"}\n"
		"[fragment]\n"
		"uniform sampler2DShadow shadow0,\n"
								"shadow1;\n"
		+intensityFunction+

		"void main()\n"
		"{\n"
			//"gl_Position = ftransform();\n"
			//"float md = fmod(gl_Color.a,0.2);\n"
			//"md *= 5.0;\n"
			"float my_intensity = 0.2+0.3*max(0.0,dot(normalize(normal),light0))*intensity(shadow0,shadow_coord0)+0.6*max(0.0,dot(normalize(normal),light1))*intensity(shadow1,shadow_coord1);\n"
			"gl_FragColor.rgb = color.rgb * my_intensity;\n"
			"gl_FragColor.a = color.a;\n"
			"if (h > 0.1 && h < 0.9){\n"
				"float md = mod(h-0.1,0.2)*5.0;\n"
				"if (md > 0.475 && md < 0.525)\n" 
					"gl_FragColor.rgb*=0.6;\n"
			"}\n"
			//"gl_FragColor.rgb = normal*0.5 + 0.5;\n"
			//"gl_FragColor.rgb *= texture2D(shadow,shadow_coord).rgb;\n"
			//"gl_FragColor.rgb = shadow_coord;\n"

		"}\n"))
		FATAL__(shader.Report());

	shader.Install();
		shader.FindVariable("shadow1").seti(1);
		shader.FindVariable("light0").Set(light_direction0);
		shader.FindVariable("light1").Set(light_direction1);
	shader.Uninstall();


	if (!holeShader.LoadComposition(
		"[shared]\n"
			"varying float h;\n"
			"varying vec3 normal;\n"
			"varying vec3 shadow_coord0, shadow_coord1;\n"
			"varying vec4 color;\n"
			"uniform vec3 light0;\n"
			"uniform vec3 light1;\n"
		"[vertex]\n"
		"void main()\n"
		"{\n"
			"gl_Position = ftransform();\n"
			"h = gl_MultiTexCoord0.x;\n"
			"normal = gl_Normal;\n"
			"color = gl_Color;\n"
			"shadow_coord0 = (gl_TextureMatrix[0]*gl_Vertex).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
			"shadow_coord0.z -= 0.01;\n"
			"shadow_coord1 = (gl_TextureMatrix[1]*gl_Vertex).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
			"shadow_coord1.z -= 0.01;\n"
		"}\n"
		"[fragment]\n"
		"uniform sampler2DShadow shadow0,\n"
								"shadow1;\n"
		//"uniform sampler2D shadow;\n"
		+intensityFunction+
		"void main()\n"
		"{\n"
			//"gl_Position = ftransform();\n"
			//"float md = fmod(gl_Color.a,0.2);\n"
		
			//"md *= 5.0;\n"
			"float my_intensity = 0.2+0.5*max(0.0,dot(normalize(normal),light0))*intensity(shadow0,shadow_coord0)+0.5*max(0.0,dot(normalize(normal),light1))*intensity(shadow1,shadow_coord1);\n"
			"gl_FragColor = color;\n"
			"gl_FragColor.rgb *= 0.8;\n"
			"gl_FragColor.rgb *= my_intensity;\n"
			"float md = mod(abs(gl_FragCoord.x*2 + gl_FragCoord.y),20.0);\n"
			"if (md < 10.0)\n"
				"gl_FragColor.rgb *= 0.65;\n"
			//"gl_FragColor.rgb = dot(normalize(normal),light0);\n"
			//"gl_FragColor.rgb *= texture2D(shadow,shadow_coord).rgb;\n"
			//"gl_FragColor.rgb = shadow_coord;\n"

		"}\n"))
		FATAL__(holeShader.Report());

		
	holeShader.Install();
		holeShader.FindVariable("shadow1",false).seti(1);
		holeShader.FindVariable("light0",false).Set(light_direction0);
		holeShader.FindVariable("light1",false).Set(light_direction1);
	holeShader.Uninstall();
	if (!back_shader.LoadComposition(
		"[shared]\n"
			"varying float h;\n"
			"varying vec3 normal;\n"
			"varying vec4 color;\n"
			"varying vec3 shadow_coord0, shadow_coord1;\n"
			"uniform vec3 light0;\n"
			"uniform vec3 light1;\n"
		"[vertex]\n"
		"void main()\n"
		"{\n"
			"gl_Position = ftransform();\n"
			"h = gl_MultiTexCoord0.x;\n"
			"normal = -gl_Normal;\n"
			"color = gl_Color;\n"
			"shadow_coord0 = (gl_TextureMatrix[0]*gl_Vertex).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
			"shadow_coord0.z -= 0.01;\n"
			"shadow_coord1 = (gl_TextureMatrix[1]*gl_Vertex).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
			"shadow_coord1.z -= 0.01;\n"
		"}\n"
		"[fragment]\n"
		"uniform sampler2DShadow shadow0,\n"
								"shadow1;\n"
		+intensityFunction+

		"void main()\n"
		"{\n"
			//"gl_Position = ftransform();\n"
			//"float md = fmod(gl_Color.a,0.2);\n"
			//"md *= 5.0;\n"
			"float my_intensity = 0.2+0.5*max(0.0,dot(normalize(normal),light0))*intensity(shadow0,shadow_coord0)+0.5*max(0.0,dot(normalize(normal),light1))*intensity(shadow1,shadow_coord1);\n"
			"gl_FragColor.rgb = color.rgb * my_intensity;\n"
			"gl_FragColor.a = color.a;\n"
			"if (h > 0.1 && h < 0.9){\n"
				"float md = mod(h-0.1,0.2)*5.0;\n"
				"if (md > 0.475 && md < 0.525)\n" 
					"gl_FragColor.rgb*=0.6;\n"
			"}\n"
			//"gl_FragColor.rgb = normal*0.5 + 0.5;\n"
			//"gl_FragColor.rgb *= texture2D(shadow,shadow_coord).rgb;\n"
			//"gl_FragColor.rgb = shadow_coord;\n"

		"}\n"))
		FATAL__(back_shader.Report());
		
	back_shader.Install();
		back_shader.FindVariable("shadow1").seti(1);
	back_shader.Uninstall();


	if (!wallShader.LoadComposition(
		"[shared]\n"
			"varying vec3 shadow_coord0, shadow_coord1;\n"
			"varying vec3 world;\n"
		"[vertex]\n"
		"void main()\n"
		"{\n"
			"gl_Position = ftransform();\n"
			"vec4 v = gl_Vertex;\n"
			"v.x = clamp(v.x,-0.99,0.99);\n"
			"shadow_coord0 = (gl_TextureMatrix[0]*v).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
			"shadow_coord1 = (gl_TextureMatrix[1]*v).xyz*0.5+0.5;\n"		//orthographic projection, so no division needed
		"}\n"
		"[fragment]\n"
		"uniform sampler2DShadow shadow0,\n"
								"shadow1;\n"
		"uniform sampler2D shadow;\n"
		+intensityFunction+
		"void main()\n"
		"{\n"
			"float my_intensity = 0.2 + 0.5 * intensity(shadow0,shadow_coord0)+0.5*intensity(shadow1,shadow_coord1);\n"
			//"float my_intensity = texture2D(shadow1,shadow_coord1).r;\n"
			//"float my_intensity = intensity(shadow1,shadow_coord1);\n"
			"gl_FragColor.rgb = vec3(0);\n"
			//"gl_FragColor.rgb = shadow_coord1*0.5 + 0.5;\n"
			"gl_FragColor.a = 1.0 - min(1.0,my_intensity);\n"
		"}\n"))
		FATAL__(wallShader.Report());
		
	wallShader.Install();
		wallShader.FindVariable("shadow1",false).seti(1);
	wallShader.Uninstall();



	CGS::Constructor<>	constructor(1,CGS::HasNormalFlag|CGS::HasColorFlag);

	auto&obj = constructor.AppendObject();

	float z = -0.01f;
	float x = 1.01f;
	float y = x;
	float h = 1.4f;
	float4 c(h,h,h,1);
	obj.SetVertexOffsetToCurrent();
	obj.MakeVertex(float3(-x,-y,z),c);
	obj.MakeVertex(float3(x,-y,z),c);
	obj.MakeVertex(float3(x,y,z),c);
	obj.MakeVertex(float3(-x,y,z),c);
	obj.MakeQuad(0,1,2,3);

	obj.SetVertexOffsetToCurrent();
	obj.MakeVertex(float3(-x,-y,1),c);
	obj.MakeVertex(float3(x,-y,1),c);
	obj.MakeVertex(float3(x,-y,z),c);
	obj.MakeVertex(float3(-x,-y,z),c);
	obj.MakeQuad(0,1,2,3);

	obj.SetVertexOffsetToCurrent();
	obj.MakeVertex(float3(x,-y,1),c);
	obj.MakeVertex(float3(x,y,1),c);
	obj.MakeVertex(float3(x,y,z),c);
	obj.MakeVertex(float3(x,-y,z),c);
	obj.MakeQuad(0,1,2,3);



	obj.ComputeNormals();

	wallScenery.Embed(obj);

}
