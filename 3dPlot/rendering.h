#pragma once

#include <engine/display.h>
#include <engine/renderer/opengl.h>
#include <engine/gl/raster_font.h>
#include <engine/textout.h>
#include <engine/aspect.h>
#include <engine/profiler.h>
#include <engine/auto_display_configure.h>
#include <engine/gl/texture_font2.h>
#include <math/vclasses.h>
#include "geometryPool.h"
//#include <engine/scenery.h>

using namespace Engine;


typedef OpenGL			Renderer;
extern Display<Renderer> 				display;
extern Textout<GLTextureFont2>			textout;
extern AngularCamera<float>			camera;
extern OrthographicAspect<float>		hud;
extern VectorCamera<float>				shadow_aspect0,
										shadow_aspect1;
extern Profiler::StackedGraph<OpenGL,GLTextureFont2>	fps_graph;
extern GLShader::Instance				shader,back_shader,holeShader;
extern Renderer::FBO					shadow_fbo0,shadow_fbo1;
extern M::Box<>						range;
extern float3						markerSteps;
extern float3						markerLabelFactor;
extern Geometry							scenery,holeScenery,transparentScenery;
extern M::TVec3<const char*>			arrowLabel;



static const constexpr unsigned ShadowResolution=2048;
static const constexpr bool		DoubleSided = true;





void	RenderShadows();
void	RenderPlot();
void	RenderExtendedAxes();
void	RenderExtendedAxesLabels();
void	SetupRenderer();
