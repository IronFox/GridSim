#include "../../global_root.h"
#include "gui.h"

namespace Engine
{
	namespace GUI
	{

		#ifdef DEEP_GUI
			MAKE_SORT_CLASS(DepthSort,current_center.shellRadius);
		#endif
		
		static InputProfile	my_profile(true,false);
		

		Operator::Operator(Display<OpenGL>&display_, const Mouse&mouse_, InputMap&input_, mode_t mode_):owns_mouse_down(false),display(&display_),mouse(&mouse_),input(&input_),mode(mode_),created(false),stack_changed(false),layer_texture(0)
		{}

		namespace
		{

			bool 							settingChanged(true),
											blurEffect(true),
											refractEffect(true),
											bumpEffect(true),
											handlingEvent(false);
			Component::TExtEventResult		ext;


			float							time_since_last_tick=0;

			shared_ptr<Component>			clicked,//component that caught the mouse down event
											focused,//last clicked component. unlike 'clicked' this variable will not be set to NULL if the mouse button is released
											hovered;//last hovered component
		
		
			TVec2<float>					last;
			shared_ptr<Window>				dragging;
			Window::ClickResult::value_t		dragType = Window::ClickResult::Missed;
		

			GLShader::Template				windowShader;
			GLShader::Instance				normalShader,
											*currentWindowShader(NULL);
			index_t							blurEffectSwitch,
											refractEffectSwitch,
											bumpEffectSwitch;

		


			void							Initialize(bool force = false)
			{
				static bool initialized = false;
				if (initialized || force)
					return;
				initialized = true;

				if (!glExtensions.Init(EXT_SHADER_OBJECTS_BIT))
					FATAL__("Failed to initialized shader objects");

				windowShader.Clear();
				if (!windowShader.LoadComposition(
					"[shared]\n"
					"varying vec2 eye;\n"
					
					"varying vec3 interpolatedNormal, interpolatedTangent, interpolatedBinormal;\n"
					"[vertex]\n"
					"void main()\n"
					"{\n"
						"gl_Position = ftransform();\n"
						"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
						"eye = gl_Position.xy/gl_Position.w/2.0+0.5;\n"
						"vec3	tangent = normalize(gl_NormalMatrix*gl_MultiTexCoord1.xyz),\n"
						"		normal = normalize(gl_NormalMatrix*gl_Normal);\n"
						"interpolatedNormal = normal;\n"
						"gl_FrontColor = gl_Color;\n"
						//"#if bump_mapping||refract\n"
							"interpolatedBinormal = -normalize(cross(normal,tangent));\n"
							"interpolatedTangent = cross(normal,interpolatedBinormal);\n"
						//"#endif\n"
					"}\n"
					"[fragment]\n"
					"uniform sampler2D background;\n"
					"uniform sampler2D normal_map;\n"
					"uniform sampler2D color_map;\n"
					//"uniform sampler2D sky_map;\n"
					//"const vec3 light = {0.57735026918962576450914878050196,0.57735026918962576450914878050196,0.57735026918962576450914878050196};\n"
					//"const vec3 light = {0,0.70710678118654752440084436210485,0.70710678118654752440084436210485};\n"
					"const vec3 light = vec3(0,0.70710678118654752440084436210485,0.70710678118654752440084436210485);\n"
					"float sqr(vec2 vec)	{return dot(vec,vec);}\n"
					"vec3 toneBackground(vec3 color)\n"
					"{\n"
						"float brightness = dot(color,vec3(0.35,0.5,0.15));\n"
						"return color / (1.0 + gl_Color.r * pow(brightness,4.0)*0.25);\n"
					"}\n"
					"void main()\n"
					"{\n"
						"vec3 color = vec3(0.0);\n"
						"float edge = 0.0;\n"
						"#if bump_mapping||refract||blur\n"	//need normal alpha for blur
							"vec4 normal = texture2D(normal_map,gl_TexCoord[0].xy);\n"
							"vec3 rawNormal = (normal.xyz*2.0-1.0);\n"
							"edge = 1.0 - pow(rawNormal.z,2);\n"
							"#if bump_mapping||refract\n"
								"normal.xyz = normalize(gl_Color.a * interpolatedBinormal*rawNormal.x + gl_Color.a * interpolatedTangent*rawNormal.y + interpolatedNormal*rawNormal.z);\n"
							"#endif\n"
						"#endif\n"
						"vec4 color_sample = texture2D(color_map,gl_TexCoord[0].xy);\n"
						//"if (normal.a == 0.0)\n"
						//	"discard;\n"
						"#if bump_mapping\n"
							//"vec3 reflected = reflect(vec3(gl_TexCoord[0].xy*2.0-1.0,-1.0),normal.xyz);\n"
							"vec3 reflected = reflect(vec3(0.0,0.0,-1.0),normal.xyz);\n"
						"#endif\n"
						"#if refract\n"
							"vec3 refracted = refract(vec3(0.0,0.0,-1.0),normal.xyz,0.7);\n"
							"vec2 mod = clamp((refracted).xy*0.07,-0.005,0.005);\n"
							"vec2 sample = 0.5+(eye+mod-0.5)*0.98;\n"
						"#else\n"
							"vec2 sample = 0.5+(eye-0.5)*0.98;\n"
						"#endif\n"
						"vec4 overlayColor;\n"
						"vec4 bumpColor;\n"
						"#if blur\n"
							"vec4 accumulated = vec4(0);\n"
							"for (int x = -2; x <= 2; x++)\n"
								"for (int y = -2; y <= 2; y++)\n"
								"{\n"
									"float w = mix(1.0,gl_Color.a,max(abs(float(x)),abs(float(y)))/2.0);\n"
									"vec2 coord = (sample+vec2(float(x)*1.3/1280.0,float(y)*1.3/1024.0f));\n"
									"accumulated.rgb += texture2D(background,coord).rgb*w;\n"
									"accumulated.a += w;\n"
								"}\n"
							"color = accumulated.rgb / accumulated.w;\n"
							"color = toneBackground(color);\n"
							"bumpColor = vec4(color,normal.a);\n"
							"overlayColor = color_sample;\n"
							"#if bump_mapping\n"
								"float intensity = pow(max(dot(reflected,light),0.0),7.0)*0.5*gl_Color.a+0.05;\n"
								"bumpColor.rgb += intensity;\n"
								"overlayColor.rgb *= (1.0+intensity);\n"
							"#endif\n"
						"#elif refract\n"
							"color = texture2D(background,sample).rgb;\n"
							"color = toneBackground(color);\n"
							"bumpColor = vec4(color,normal.a);\n"
							"overlayColor = color_sample;\n"
							"#if bump_mapping\n"
								"float intensity = pow(max(dot(reflected,light),0.0),7.0)*0.5*gl_Color.a+0.05;\n"
								"bumpColor.rgb += intensity;\n"
								"overlayColor.rgb *= (1.0+intensity);\n"
							"#endif\n"
						"#else\n"
							"bumpColor = vec4(0.6,0.6,0.6,normal.a*0.5);\n"
							"overlayColor = color_sample;\n"
							"#if bump_mapping\n"
								"float intensity = pow(max(dot(reflected,light),0.0),7.0)*0.5*gl_Color.a+0.05;\n"
								"overlayColor.rgb += intensity;\n"
								"overlayColor.a *= (1.0 + intensity);\n"
								"bumpColor.rgb += intensity;\n"
							"#endif\n"
						"#endif\n"
						"overlayColor.a *= (0.75 + 0.25 * gl_Color.a);"
						"float div = bumpColor.a + overlayColor.a - bumpColor.a * overlayColor.a;\n"
						"gl_FragColor.a = div;\n"
						"if (div == 0.0) gl_FragColor.rgb = vec3(1.0,0.0,1.0);\n"
						"else gl_FragColor.rgb = (bumpColor.rgb * bumpColor.a + overlayColor.rgb * overlayColor.a - bumpColor.rgb * bumpColor.a * overlayColor.a) / div;"
						"float brightness = dot(color,vec3(0.35,0.5,0.15));\n"
						"gl_FragColor.rgb *= 1.0 - edge*pow(brightness,4);"
						//"gl_FragColor.a *= 0.75+0.25*gl_Color.a;\n"
						//"gl_FragColor = vec4(0.0,1.0,0.0,1.0);\n"

						//"gl_FragColor.a *= 0.9+0.1*gl_Color.a;\n"
						/*"gl_FragColor.xy = sample;\n"
						"gl_FragColor.z = 0;\n"*/
						//"gl_FragColor.xyz = mix(vec3(1),gl_FragColor.rgb,gl_FragColor.a);\n"
						//"gl_FragColor.xyz = vec3(1.0-gl_FragColor.a);\n"
						//"gl_FragColor.a = 1;\n"
						//"gl_FragColor.xyz = vec3(0.0);\n"
						//"gl_FragColor.xyz = color_sample.rgb;\n"
						//"gl_FragColor = color_sample;\n"
						//"gl_FragColor.xyz = normal.xyz*0.5+0.5;\n"
						//"gl_FragColor.xyz = rawNormal.xyz*0.5+0.5;\n"
						//"gl_FragColor.xyz = interpolatedBinormal*0.5+0.5;\n"
						//"gl_FragColor.xyz = vec3(length(interpolatedNormal));\n"
						
						//"gl_FragColor = color_sample;\n"
					"}\n"
				))
				{
					FATAL__("GUI Window shader compilation exception:\n"+windowShader.Report());
				}
				else
				{
					blurEffectSwitch = windowShader.FindVariable("blur");
					refractEffectSwitch = windowShader.FindVariable("refract");
					bumpEffectSwitch = windowShader.FindVariable("bump_mapping");
					windowShader.PredefineUniformInt("normal_map",1);
					windowShader.PredefineUniformInt("color_map",2);
				}
			
				normalShader.Clear();
				if (!normalShader.LoadComposition(
					"[vertex]\n"
					"varying float h;\n"
					"void main()\n"
					"{\n"
						"gl_Position = ftransform();\n"
						"gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;\n"
						"h = gl_MultiTexCoord1.x;\n"
					"}\n"
					"[fragment]\n"
					"uniform sampler2D normal_map;\n"
					//"uniform sampler2D sky_map;\n"
					"const vec3 light = vec3(0.57735026918962576450914878050196,0.57735026918962576450914878050196,0.57735026918962576450914878050196);\n"
					"varying float h;\n"
					"void main()\n"
					"{\n"
						"vec3 color = vec3(0.0);\n"
						"vec4 normal_sample = texture2D(normal_map,gl_TexCoord[0].xy);\n"
						"gl_FragColor.xyz = 0.5+(gl_TextureMatrix[0]*vec4(normal_sample.xyz*2.0-1.0,0.0)).xyz*0.5;\n"
						//"gl_FragColor.b = h*0.5+0.5;\n"
						//"gl_FragColor.z = 1.0-gl_FragColor.z;\n"
						"gl_FragColor.a = normal_sample.a;\n"
					"}\n"
				))
				{
					FATAL__("GUI Normal shader compilation exception:\n"+normalShader.Report());
				}
			
				/*Image	image;
				if (Magic::LoadFromFile(image,"sky.png"))
					sky_texture = display->textureObject(image);*/
	 
				//windowShader.suppressWarnings();
				//windowShader.locate("sky_map").seti(3);
			
			
			
				glGetError();
			


			}
		}

		PComponent Operator::HoveringOver()	const
		{
			return hovered;
		}

		Textout<GLTextureFont2>				ColorRenderer::textout;




		Layout								Window::commonStyle,
											Window::menuStyle,
											Window::hintStyle;

		//static Log							logfile("gui.log",true);

		
		static Keyboard::charReader		oldReader=NULL;
		static	bool						oldRead = false;
		
		//Display<OpenGL>					*display(NULL);
		
		/*TCell::~TCell()
		{
			if (display)
			{
				display->discardObject(normalTexture);
				display->discardObject(colorTexture);
			}
		}*/

		TCell&	TCell::operator=(const TCell&other)
		{
			return *this;
		}

		void						Operator::showMenu(const shared_ptr<Window>&menuWindow)
		{
			/*			result->x = region.x.center()-display->clientWidth()/2;
				result->y = region.y.center()-display->clientHeight()/2;*/

			float	w = display->clientWidth()/2,
					h = display->clientHeight()/2;
			if (menuWindow->x - menuWindow->fsize.x/2 < -w)
				menuWindow->x = -w+menuWindow->fsize.x/2;
			if (menuWindow->x + menuWindow->fsize.x/2 > w)
				menuWindow->x = w-menuWindow->fsize.x/2;
			
			if (menuWindow->y - menuWindow->fsize.y/2 < -h)
				menuWindow->y = -h+menuWindow->fsize.y/2;
			if (menuWindow->y + menuWindow->fsize.y/2 > h)
				menuWindow->y = h-menuWindow->fsize.y/2;
			
			menu_stack << menuWindow;
		}
		
		void						Operator::hideMenus()
		{
			if (!this)
				return;

			for (index_t i = 0 ; i < menu_stack.count(); i++)
			{
				shared_ptr<Window>	window = menu_stack[i].lock();
				if (window)
					window->hidden = timing.now64;
			}
			menu_stack.reset();
		}

		
		void		ColorRenderer::Paint(const TFreeCell&cell)
		{
			if (cell.color->IsEmpty())	//NULL-pointer sensitive
				return;
			_UpdateState(cell.color);
			switch (cell.orientation)
			{
				case 0:
					glBegin(GL_QUADS);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
				case 1:
					glBegin(GL_QUADS);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
				case 2:
					glBegin(GL_QUADS);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
				case 3:
					glBegin(GL_QUADS);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
			}
			layerIsDirty = true;
		}

		void		NormalRenderer::Paint(const TFreeCell&cell, bool invertNormals)
		{
			if (cell.normal->isEmpty())	//NULL-pointer sensitive
				return;
			_UpdateState(cell.normal);
			PushNormalMatrix();
			if (invertNormals)
				ScaleNormals(-1,-1,1);

			switch (cell.orientation)
			{
				case 0:
					glBegin(GL_QUADS);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
				case 1:
				{
					//-90 degrees z
					TMatrix3<> rotation = {Vector<>::negative_y_axis,Vector<>::x_axis,Vector<>::z_axis};
					TransformNormals(rotation);
					glBegin(GL_QUADS);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				}
				break;
				case 2:
				{
					TMatrix3<> rotation = {Vector<>::negative_x_axis,Vector<>::negative_y_axis,Vector<>::z_axis};
					TransformNormals(rotation);
					glBegin(GL_QUADS);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				}
				break;
				case 3:
				{
					TMatrix3<> rotation = {Vector<>::y_axis,Vector<>::negative_x_axis,Vector<>::z_axis};
					TransformNormals(rotation);
					glBegin(GL_QUADS);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				}
				break;
			}
			PopNormalMatrix();
			layerIsDirty = true;
		}
				
		
		void	TTexture::load(XML::Node*node, const char*aname, FileSystem::Folder&folder, TTexture&out, float scale)
		{
			String attrib;
			if (!node->query(aname,attrib))
				throw IO::DriveAccess::FileFormatFault("Failed to query '"+String(aname)+"' of XML node '"+node->name+"'");

			FileSystem::File	file;
			Image image;
			out.width = 0;
			out.height = 0;
			if (folder.findFile("color/"+attrib,file))
			{
				Magic::LoadFromFile(image,file.getLocation());
				out.color.load(image,global_anisotropy,true);
				out.width = image.width();
				out.height = image.height();
			}
			if (folder.findFile("bump/"+attrib,file))
			{
				loadBump(file.getLocation(),image);

				if (out.width != 0 && (out.width != image.width() || out.height != image.height()))
				{
					throw IO::StructureCompositionFault("Trying to match bump texture of different size (color image was "+String(out.width)+"*"+String(out.height)+", bump texture is "+image.ToString()+")");
				}
				out.normal.load(image,global_anisotropy,true);
				out.width = image.width();
				out.height = image.height();
			}
			out.width *= scale;
			out.height *= scale;
		}
		
		/*static*/ void Component::ReadChar(char c)
		{
			eEventResult rs = Unsupported;
			if (focused)
			{
				rs = focused->OnChar(c);
				focused->GetWindow()->Apply(rs);
			}
			if (rs == Unsupported && oldRead)
				oldReader(c);
		}

		void					Component::SignalLayoutChange()	const
		{
			shared_ptr<Window>	wnd = GetWindow();
			if (wnd)
				wnd->layoutChanged = wnd->visualChanged = true;
		}
		void					Component::SignalVisualChange()	const
		{
			shared_ptr<Window>	wnd = GetWindow();
			if (wnd)
				wnd->visualChanged = true;
		}


		/*virtual*/	void					Component::SetEnabled(bool enabled)
		{
			if (this->enabled != enabled)
			{
				this->enabled = enabled;
				SignalVisualChange();
			}
		}
	
		/*virtual*/	void					Component::SetVisible(bool visible)
		{
			if (this->visible != visible)
			{
				this->visible = visible;
				SignalVisualChange();
			}
		}
	
		shared_ptr<Operator>	Component::GetOperator() const
		{
			if (windowLink.expired())
				return shared_ptr<Operator>();
			return windowLink.lock()->operatorLink.lock();
		}

		shared_ptr<Operator>	Component::RequireOperator() const
		{
			if (windowLink.expired())
			{
				FATAL__("Operator required! Window link not set to component of type "+typeName);
				return shared_ptr<Operator>();
			}
			shared_ptr<Operator> rs = windowLink.lock()->operatorLink.lock();
			if (!rs)
				FATAL__("Operator required! Window link set but operator link not set");
			return rs;
		}

		/*virtual*/ void			Component::SetWindow(const weak_ptr<Window>&wnd)
		{
			windowLink = wnd;

			for (index_t i = 0; i < CountChildren(); i++)
				GetChild(i)->SetWindow(wnd);
		}
		
		void Component::ResetFocused()
		{
			SetFocused(shared_ptr<Component>());
		}

		void Component::SetFocused(const shared_ptr<Component>&component)
		{
			if (focused == component)
				return;
			time_since_last_tick = 0;
			if (focused)
			{
				eEventResult result = focused->OnFocusLost();
				shared_ptr<Window> wnd = focused->GetWindow();
				if (wnd)
					wnd->Apply(result);
				if (!component)
				{
					input.popProfile();
					keyboard.reader = oldReader;
					keyboard.read = oldRead;
				}
			}
			elif (component)
			{
				oldReader = keyboard.reader;
				oldRead = keyboard.read;
				keyboard.reader = ReadChar;
				keyboard.read = true;
				if (!focused)
				{
					input.pushProfile();
					input.bindProfile(my_profile);
				}
			}
			focused = component;
			if (focused)
			{
				focused->GetWindow()->Apply(focused->OnFocusGained());
			}
		}


		void Operator::cylindricVertex(float px, float py, float tx, float ty, float r/* =1*/) const
		{
			float	radial = px/3,
					h = sqrt(r*r-sqr(radial));
				glNormal3f(-radial/r,0,h/r); 
				glTangent3f(0,1,0);
				glTexCoord2f(tx,ty);
			//glVertex3f(px*2.f,py*2.f,-h);	//magnified
			glVertex3f(px,py,-h);
		}
		
		void Operator::cylindricVertex(Window*window, float x, float y)	const
		{
			#ifdef DEEP_GUI
				float	px = (window->current_center.x*window->current_center.shellRadius+window->fsize.x*x/2)/display->clientWidth()*2,
						py = (window->current_center.y*window->current_center.shellRadius+window->fsize.y*y/2)/display->clientHeight()*2;
				project(px,py,(x*0.5+0.5)*window->usage.x,(y*0.5+0.5)*window->usage.y,window->current_center.shellRadius);
			#else
				float	px = (window->x+window->fsize.x*x/2)/display->clientWidth()*2,
						py = (window->y+window->fsize.y*y/2)/display->clientHeight()*2;
				cylindricVertex(px,py,(x*0.5f+0.5f)*window->usage.x,(y*0.5f+0.5f)*window->usage.y);
			#endif
		}


		void Operator::planarVertex(float px, float py, float tx, float ty) const
		{
				glTexCoord2f(tx,ty);
			glVertex3f(px,py,0);
		}
		
		void Operator::planarVertex(Window*window, float x, float y)	const
		{
			float	px = (window->x+window->fsize.x*x/2)/display->clientWidth()*2,
					py = (window->y+window->fsize.y*y/2)/display->clientHeight()*2;
			planarVertex(px,py,(x*0.5f+0.5f)*window->usage.x,(y*0.5f+0.5f)*window->usage.y);
		}
		
		
		void Operator::project(Window*window, float x, float y, TVec2<float>&p)	const
		{
			/*cout << "x="<<x<<endl;
			cout << "y="<<y<<endl;
			cout << "shell.r="<<window->current_center.shellRadius<<endl;
			cout << "shell.x="<<window->current_center.x<<endl;
			cout << "shell.y="<<window->current_center.y<<endl;
			cout << "fsize.x="<<window->fsize.x<<endl;
			cout << "fsize.y="<<window->fsize.y<<endl;
			cout << "client width="<<display->clientWidth()<<endl;
			cout << "client height="<<display->clientHeight()<<endl;*/
			#ifdef DEEP_GUI
				float	r = window->current_center.shellRadius,
						px = (window->current_center.x*r+window->fsize.x*x/2)/display->clientWidth()*2,
						py = (window->current_center.y*r+window->fsize.y*y/2)/display->clientHeight()*2,
						radial = px/3,
						h = sqrt(r*r-sqr(radial));
			#else
				float	px = (window->x+window->fsize.x*x/2)/display->clientWidth()*2,
						py = (window->y+window->fsize.y*y/2)/display->clientHeight()*2,
						radial = px/3,
						h = sqrt(1.0f-sqr(radial));
			#endif
			
			/*cout << "radial="<<radial<<endl;
			cout << "equation=sqrt("<<r<<"*"<<r<<"-sqr("<<radial<<"))"<<endl;*/
			TVec3<float>	mp = {px,py,-h};
			//cout << "mp="<<_toString(mp)<<endl;
			projected_space.PointToScreen(mp,p);
			p.x = (p.x*0.5+0.5)*(float)display->clientWidth();
			p.y = (p.y*0.5+0.5)*(float)display->clientHeight();
		}
		
		void Operator::unproject(const TVec3<float>&f_, TVec2<float>&p) const	//f required to be normalized
		{
			TVec3<float> f = {f_.x/3,0,f_.z};
			float	y=f_.y;
			float len = Vec::length(f);
			Vec::div(f,len);
			y/=len;
			float x = f.x * 3;

			p.x = x * (float)display->clientWidth()/2.0f;
			p.y = y * (float)display->clientHeight()/2.0f;
		}

		void	Operator::unprojectMouse(TVec2<float>&p)	const
		{
			TVec3<float> f;
			projected_space.ScreenToVector(mouse->location.windowRelative.x*2.f-1.f,mouse->location.windowRelative.y*2.f-1.f,Vector<>::dummy,f);
			Vec::normalize0(f);
			unproject(f,p);
		}

			
			
		static inline void drawRect(const Rect<float>&rect)
		{
			glBegin(GL_LINE_LOOP);
				glVertex2f(rect.x.min,rect.y.min);
				glVertex2f(rect.x.max,rect.y.min);
				glVertex2f(rect.x.max,rect.y.max);
				glVertex2f(rect.x.min,rect.y.max);
			glEnd();
		}
		
		
		void					loadBump(const String&filename, Image&target)
		{
			Image image;
			Magic::loadFromFile(image,filename);

			bool alpha = image.channels()==4;
			target.setSize(image.width(),image.height(),4);
			
			float	x_bump_scale = (float)image.width()/512.0f/5.0f,
					y_bump_scale = (float)image.height()/512.0f/5.0f;
			for (unsigned x = 0; x < image.width(); x++)
				for (unsigned y = 0; y < image.height(); y++)
				{
					const BYTE	*p = image.get(x,y),
								*px1 = image.get(min(x+1,image.width()-1),y),
								*py1 = image.get(x,min(y+1,image.height()-1)),
								*px0 = image.get(x?x-1:0,y),
								*py0 = image.get(x,y?y-1:0);
					if (image.channels())
					{
						if (px0[3] == 0)
							px0 = p;
						if (px1[3] == 0)
							px1 = p;
						if (py1[3] == 0)
							py1 = p;
						if (py0[3] == 0)
							py0 = p;
					}
					float	dx = ((float)px1[0]-(float)px0[0])*x_bump_scale,
							dy = ((float)py1[0]-(float)py0[0])*y_bump_scale;
					TVec3<float>		n = {-dx,-dy,1};
					Vec::normalize(n);
					target.setNormal(x,y,n);
					target.get(x,y)[3] = alpha?p[3]:255;
				}						
			if (target.width() == 1)
				target.scaleTo(64,target.height());
			if (target.height() == 1)
				target.scaleTo(target.width(),64);
			
		}
		
		
		void	loadBump(const String&filename,OpenGL::Texture&target)
		{
			Image	image;
			loadBump(filename,image);
			target.load(image,global_anisotropy,true);
		}
		
		void	loadColor(const String&filename,OpenGL::Texture&target)
		{
			Image	image;
			Magic::loadFromFile(image,filename);
			target.load(image,global_anisotropy,true);
		}

		
		Layout::Layout():minWidth(1),minHeight(1),variableRows(1),cellCount(0),override(NULL)
		{}
		
	
		void	Layout::LoadFromFile(const String&filename, float scale)
		{
			rows.free();
			
			
			FileSystem::Folder	folder(FileSystem::extractFilePath(filename));
			
			titlePosition.setAll(0);
			
			List::Vector<TIORow>	iorows;
			
			XML::Container	xml;
			xml.LoadFromFile(filename);
				
			const XML::Node*xlayout = xml.find("layout");
			if (!xlayout)
				throw IO::DriveAccess::FileFormatFault(globalString("XML file lacks 'layout' root node"));

			String attrib;
			if (xlayout->query("title_position",attrib))
			{
				Array<String>	segments;
				explode(',',attrib,segments);
				if (segments.count() != 4)
					throw IO::DriveAccess::FileFormatFault(globalString("'title_position' attribute of XML 'layout' node does not contain 4 comma-separated segments"));

				if (!convert(segments[0].c_str(),titlePosition.x.min)
					||
					!convert(segments[1].c_str(),titlePosition.y.min)
					||
					!convert(segments[2].c_str(),titlePosition.x.max)
					||
					!convert(segments[3].c_str(),titlePosition.y.max))
				{
					throw IO::DriveAccess::FileFormatFault(globalString("One or more segments of 'title_position' attribute of XML 'layout' node could not be converted to float"));
				}
				titlePosition *= scale;
			}
			{
				if (!xlayout->query("border_edge",attrib))
					throw IO::DriveAccess::FileFormatFault(globalString("XML node 'layout' lacks 'border_edge' attribute"));
					
				Array<String>	segments;
				explode(',',attrib,segments);
				if (segments.count() != 4)
					throw IO::DriveAccess::FileFormatFault(globalString("'border_edge' attribute of XML 'layout' node does not contain 4 comma-separated segments"));

				if (!convert(segments[0].c_str(),borderEdge.left)
					||
					!convert(segments[1].c_str(),borderEdge.bottom)
					||
					!convert(segments[2].c_str(),borderEdge.right)
					||
					!convert(segments[3].c_str(),borderEdge.top))
				{
					throw IO::DriveAccess::FileFormatFault(globalString("One or more segments of 'border_edge' attribute of XML 'layout' node could not be converted to float"));
				}
				borderEdge *= scale;
			}
			{
				if (!xlayout->query("client_edge",attrib))
					throw IO::DriveAccess::FileFormatFault(globalString("XML node 'layout' lacks 'client_edge' attribute"));

				Array<String>	segments;
				explode(',',attrib,segments);
				if (segments.count() != 4)
					throw IO::DriveAccess::FileFormatFault(globalString("'client_edge' attribute of XML 'layout' node does not contain 4 comma-separated segments"));

				if (!convert(segments[0].c_str(),clientEdge.left)
					||
					!convert(segments[1].c_str(),clientEdge.bottom)
					||
					!convert(segments[2].c_str(),clientEdge.right)
					||
					!convert(segments[3].c_str(),clientEdge.top))
				{
					throw IO::DriveAccess::FileFormatFault(globalString("One or more segments of 'client_edge' attribute of XML 'layout' node could not be converted to float"));
				}
				clientEdge *= scale;
			}
			
			XML::Node*xrows = xml.find("layout/rows");
			if (!xrows)
				throw IO::DriveAccess::FileFormatFault(globalString("XML file lacks 'layout/rows' node"));
			
			
			for (index_t i = 0; i < xrows->children.count(); i++)
			{
				const XML::Node&xrow = xrows->children[i];
				if (xrow.name == "row")
				{
					if (!xrow.query("type",attrib))
					{
						ErrMessage("type attribute not defined of row. Ignoring row.");
						continue;
					}
					
					
					TIORow*row = iorows.append();
					row->variable_height = attrib == "stretch";
					
					for (index_t j = 0; j < xrow.children.count(); j++)
					{
						const XML::Node&xcell = xrow.children[j];
						if (xcell.name == "cell")
						{
							if (!xcell.query("type",attrib))
							{
								ErrMessage("type attribute not defined of cell. Ignoring cell.");
								continue;
							}
							TIOCell*cell = row->cells.append();
							cell->variableWidth = attrib == "stretch";
							
							String error;
							if (xcell.query("background",attrib))
							{
								FileSystem::File	file;
								if (folder.findFile("bump/"+attrib,file))
								{
									loadBump(file.getLocation(),cell->normal);
								}
								else
									ErrMessage("Failed to locate bump component of '"+attrib+"'");
								if (folder.findFile("color/"+attrib,file))
								{
									Magic::loadFromFile(cell->color,file.getLocation());
									if (cell->color.width() == 1)
										cell->color.scaleTo(64,cell->color.height());
									if (cell->color.height() == 1)
										cell->color.scaleTo(cell->color.width(),64);
								}
								if (!cell->color.width() || !cell->color.height())
								{
									cell->color.setSize(32,32,4);
									cell->color.fill(0,0,0,0);
								}
							}
						}
					}
				}
			}
			
			minWidth = 0;
			minHeight = 0;
			variableRows = 0;
			cellCount = 0;
			rows.setSize(iorows);
			for (unsigned i = 0; i < iorows; i++)
			{
				TRow&row = rows[i];
				row.cells.setSize(iorows[i]->cells);
				row.variable_height = iorows[i]->variable_height;
				row.height = 0;
				row.variable_cells = 0;
				row.fixed_width = 0;
				
				cellCount += row.cells.count();
				
				for (unsigned j = 0; j < row.cells.count(); j++)
				{
					TIOCell*icell = iorows[i]->cells[j];
					TCell&cell = row.cells[j];
					cell.width = icell->normal.width()*scale;
					cell.variableWidth = icell->variableWidth;
					
					cell.normalTexture.load(icell->normal,global_anisotropy,true,TextureFilter::Trilinear,false);
					cell.colorTexture.load(icell->color,global_anisotropy,true,TextureFilter::Trilinear,false);
					if (icell->normal.height()*scale>rows[i].height)
						row.height = icell->normal.height()*scale;
					if (cell.variableWidth)
						row.variable_cells++;
					else
						row.fixed_width += cell.width;
				}
				if (row.fixed_width > minWidth)
					minWidth = row.fixed_width;
				if (!row.variable_height)
					minHeight += row.height;
				else
					variableRows ++;
				if (!row.variable_cells)
					row.variable_cells++;
			}
			if (!variableRows)
				variableRows++;
		}
		
		void	Layout::applyArea(Rect<float>&target, const Rect<float>&window, const Rect<float>&relative)
		{
			for (BYTE k = 0; k < 4; k++)
			{
				if (relative[k] < 0)
					target[k] = window[2+(k%2)]+relative[k];
				else
					target[k] = window[(k%2)]+relative[k];
			}
		}
		
		void	Layout::UpdateCells(const Rect<float>&window_location, TCellLayout&layout)	const
		{
			layout.cells.setSize(cellCount);
			
			//applyArea(layout.body,window_location,body_location);
			//applyArea(layout.title,window_location,title_location);
			float	width = window_location.width(),
				height = window_location.height();

			float variable_height = (height-minHeight)/variableRows;
			if (variable_height < 0)
				variable_height = 0;
			
			float y = window_location.y.max;
			
			unsigned cell_index = 0;
			
			layout.title.x.min = titlePosition.x.min>=0?window_location.x.min + titlePosition.x.min:window_location.x.max+titlePosition.x.min;
			layout.title.x.max = titlePosition.x.max>=0?window_location.x.min + titlePosition.x.max:window_location.x.max+titlePosition.x.max;
			
			layout.title.y.min = titlePosition.y.min>=0?window_location.y.min + titlePosition.y.min:window_location.y.max+titlePosition.y.min;
			layout.title.y.max = titlePosition.y.max>=0?window_location.y.min + titlePosition.y.max:window_location.y.max+titlePosition.y.max;
			
			
			
			for (index_t i = 0; i < rows.count(); i++)
			{
				const TRow&row = rows[i];
				
				
				float row_height = row.variable_height?variable_height:row.height;
				
				float variableWidth = (width-row.fixed_width)/row.variable_cells;
				if (variableWidth < 0)
					variableWidth = 0;
				float x = window_location.x.min;
				for (unsigned j = 0; j < row.cells.count(); j++)
				{

					const TCell&cell = row.cells[j];
					float cell_width = cell.variableWidth?variableWidth:cell.width;
					
					ASSERT2__(cell_index < cellCount,cell_index,cellCount);
					TCellInstance&instance = layout.cells[cell_index++];
					
					instance.region.x.min = x;
					instance.region.x.max = x+cell_width;
					instance.region.y.max = y;
					instance.region.y.min = y-row_height;
					//instance.width = cell.width;
					instance.colorTexture = &cell.colorTexture;
					instance.normalTexture = &cell.normalTexture;
					x+= cell_width;
					
					
				}
				y-=row_height;
			}
			ASSERT_EQUAL__(cell_index,layout.cells.count());
			layout.client.x.min = /* floor */round(window_location.x.min+clientEdge.left);
			layout.client.x.max = /* ceil */round(window_location.x.max-clientEdge.right);
			layout.client.y.min = /* floor */round(window_location.y.min+clientEdge.bottom);
			layout.client.y.max = /* ceil */round(window_location.y.max-clientEdge.top);
			layout.border.x.min = window_location.x.min+borderEdge.left;
			layout.border.x.max = window_location.x.max-borderEdge.right;
			layout.border.y.min = window_location.y.min+borderEdge.bottom;
			layout.border.y.max = window_location.y.max-borderEdge.top;
		}
		

		void TCellLayout::Clear(const Rect<>&windowLocation)
		{
			border = client = windowLocation;
			title = Rect<>(0,0,0,0);
			cells.Free();
		}
		
		
		/*static*/ PWindow			Window::CreateNew(const NewWindowConfig&config, const shared_ptr<Component>&component/*=shared_ptr<Component>()*/)
		{
			return CreateNew(config,&Window::commonStyle,component);
		}
		/*static*/ PWindow			Window::CreateNew(const NewWindowConfig&config, Layout*layout,const shared_ptr<Component>&component)
		{
			shared_ptr<Window> result(new Window(config.isModal,layout));
		
			#ifdef DEEP_GUI
				result->destination.x = config.initialPosition.center.x;
				result->destination.y = config.initialPosition.center.y;
				
				for (index_t i = 0; i < windowStack.count(); i++)
					windowStack[i]->destination.shellRadius = radiusOf(i);
				//result->destination.shellRadius = radiusOf(windowStack.count()-1);
				//result->destination.x *= result->destination.shellRadius;
				//result->destination.y *= result->destination.shellRadius;
				result->origin.x = 0;
				result->origin.y = 0;
				result->origin.shellRadius = 1000;
				result->origin = result->destination;
				result->current_center = result->origin;
			#else
				result->x = config.initialPosition.center.x;
				result->y = config.initialPosition.center.y;
			#endif
			ASSERT_GREATER__(config.initialPosition.size.x,0);
			ASSERT_GREATER__(config.initialPosition.size.y,0);
			result->fsize.x = config.initialPosition.size.x;
			result->fsize.y = config.initialPosition.size.y;
			result->size.width = (unsigned)round(result->fsize.x);
			result->size.height = (unsigned)round(result->fsize.y);
			result->SetComponent(component);
			result->title = config.windowName;
			result->sizeChange = config.initialPosition.sizeChange;
			result->fixedPosition = config.initialPosition.fixedPosition;
			Rect<float> reg(0,0,result->size.width,result->size.height);
			if (layout)
				layout->UpdateCells(reg,result->cellLayout);
			else
				result->cellLayout.Clear(reg);//cellLayout.border = result->cellLayout.client = reg;
			return result;
		}

		
		
		bool	Window::Hide()
		{
			shared_ptr<Operator>	op = operatorLink.lock();
			if (op)
			{
				return op->HideWindow(shared_from_this());
			}
			return false;
		}


		Window::Window(bool modal, Layout*style):isModal(modal),toneBackground(true),appearsFocused(false),exp(Vector2<UINT>::zero),size(1,1),progress(0),fsize(Vector2<>::one),usage(Vector2<>::zero),layout(style),title(""),sizeChanged(true),layoutChanged(true),visualChanged(true),fixedPosition(false),hidden(timer.now())
		{
			#ifdef DEEP_GUI
				_clear(destination.coord);
				_clear(origin.coord);
				_clear(current_center.coord);
			#else
				x = 0;
				y = 0;
			#endif
		}
		
		void	Operator::render(const shared_ptr<Window>&window, float w, float h, bool is_menu)
		{
			#ifdef DEEP_GUI
				ASSERT_IS_CONSTRAINED__(window->current_center.x,-100000,100000);
				ASSERT_IS_CONSTRAINED__(window->current_center.y,-100000,100000);
				ASSERT_IS_CONSTRAINED__(window->current_center.shellRadius,-100000,100000);
			#else
				ASSERT_IS_CONSTRAINED__(window->x,-100000,100000);
				ASSERT_IS_CONSTRAINED__(window->y,-100000,100000);
			#endif
			TVec2<> p;
			project(window.get(),-1,-1,p);
			Rect<>	rect(p,p);
			project(window.get(),1,-1,p);
			rect.include(p);
			project(window.get(),1,1,p);
			rect.include(p);
			project(window.get(),-1,1,p);
			rect.include(p);
			
			if (window == windowStack.last())	//top most window also copies for menu windows, which override the parent window rather than blurring over it
				for (index_t i = 0; i < menu_stack.count(); i++)
				{
					shared_ptr<Window> menu_ = menu_stack[i].lock();
					if (!menu_)
					{
						menu_stack.erase(i--);
						continue;
					}
					Window*menu = menu_.get();
					project(menu,-1,-1,p);
					rect.include(p);
					project(menu,1,-1,p);
					rect.include(p);
					project(menu,1,1,p);
					rect.include(p);
					project(menu,-1,1,p);
					rect.include(p);
				}
			
			
			rect.expand(5);
			rect.constrainBy(Rect<float>(0,0,w,h));
			if (rect.width() <= 0 || rect.height() <= 0)
				return;
			
			if ((blurEffect || refractEffect) && !is_menu)	//menu reuses the copy before the top most non-menu window
			{
				glGetError();
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, (int)rect.x.min, (int)rect.y.min, (int)rect.x.min, (int)rect.y.min,  (int)rect.width(), (int)rect.height());
				if (glGetError())
				{
					//FATAL__("Failed to copy "+rect.ToString());
					return;
				}
			}
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,window->normalBuffer.colorTarget[0].textureHandle);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,window->colorBuffer.colorTarget[0].textureHandle);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			glActiveTexture(GL_TEXTURE0);
			
			glColor4f(window->toneBackground,0,0,is_menu || window->appearsFocused || windowStack.last()==window);


			//ASSERT__(currentWindowShader->isInstalled());
			glThrowError();

			switch (mode)
			{
				case Cylindrical:
				{
					//float r = 1.0f+((float)windowStack.count()-window->has_depth)*0.1;
					#ifdef DEEP_GUI
						unsigned res = (unsigned)((float)window->size.width/window->current_center.shellRadius/50);
					#else
						unsigned res = (unsigned)((float)window->size.width/50);
					#endif
				
					//ShowMessage(String(fsize.x)+" => "+String(size.width)+" => "+String(res));
					ASSERT1__(res < 1000,res);
			
					if (res<1)
						res = 1;
					float step = 1.0f/(float)res;

					glBegin(GL_TRIANGLE_STRIP);
						for (unsigned x = 0; x <= res; x++)
						{
							float	fx = (float)x*step*2-1;
							cylindricVertex(window.get(),fx,1);
							cylindricVertex(window.get(),fx,-1);
						}
					glEnd();
				}
				break;
				case Planar:
					glBegin(GL_QUADS);
						{
							glNormal3f(0,0,1); 
							glTangent3f(0,1,0);

							planarVertex(window.get(),-1,-1);
							planarVertex(window.get(),1,-1);
							planarVertex(window.get(),1,1);
							planarVertex(window.get(),-1,1);
						}
					glEnd();

				break;
			}
		}


		void	Operator::renderBox(const shared_ptr<Window>&window, float w, float h, bool is_menu)
		{
			glRed(0.5);
			switch (mode)
			{
				case Cylindrical:
				{
					//float r = 1.0f+((float)windowStack.count()-window->has_depth)*0.1;
					#ifdef DEEP_GUI
						unsigned res = (unsigned)((float)window->size.width/window->current_center.shellRadius/50);
					#else
						unsigned res = (unsigned)((float)window->size.width/50);
					#endif
				
					//ShowMessage(String(fsize.x)+" => "+String(size.width)+" => "+String(res));
					ASSERT1__(res < 1000,res);
			
					if (res<1)
						res = 1;
					float step = 1.0f/(float)res;

					glBegin(GL_LINE_LOOP);
						for (unsigned x = 0; x <= res; x++)
						{
							float	fx = (float)x*step*2-1;
							cylindricVertex(window.get(),fx,-1);
						}
						for (unsigned x = res; x <= res; x--)
						{
							float	fx = (float)x*step*2-1;
							cylindricVertex(window.get(),fx,1);
						}
					glEnd();
				}
				break;
				case Planar:
					glBegin(GL_LINE_LOOP);
						{
							glNormal3f(0,0,1); 
							glTangent3f(0,1,0);

							planarVertex(window.get(),-1,-1);
							planarVertex(window.get(),1,-1);
							planarVertex(window.get(),1,1);
							planarVertex(window.get(),-1,1);
						}
					glEnd();

				break;
			}
		}
		
		
		void	Window::SetComponent(const shared_ptr<Component>&component)
		{
			rootComponent = component;
			if (component)
				component->SetWindow(shared_from_this());
			layoutChanged = visualChanged = true;
		}
		
		void	Window::Drag(const TVec2<float>&d)
		{
			#ifdef DEEP_GUI
				destination.x += d.x;
				destination.y += d.y;
				origin.x += d.x;
				origin.y += d.y;
				current_center.x += d.x;
				current_center.y += d.y;
			#else
				x += d.x;
				y += d.y;
			#endif
		}
		
		void	Window::DragResize(TVec2<float>&d,ClickResult::value_t click_result)
		{
			float	dx = d.x,
					dy = d.y;
			switch (click_result)
			{
				case ClickResult::ResizeTopRight:
				break;
				case ClickResult::ResizeRight:
					d.y = 0;
				break;
				case ClickResult::ResizeBottomRight:
					dy *= -1;
				break;
				case ClickResult::ResizeBottom:
					dy *= -1;
					d.x = 0;
				break;
				case ClickResult::ResizeBottomLeft:
					dy *= -1;
					dx *= -1;
				break;
				case ClickResult::ResizeLeft:
					dx *= -1;
					d.y = 0;
				break;
				case ClickResult::ResizeTopLeft:
					dx *= -1;
				break;
				case ClickResult::ResizeTop:
					d.x = 0;
				break;
			}
			
			float	mw = GetMinWidth(),
					mh = GetMinHeight();
			if (fsize.x + dx < mw && dx != 0)
			{
				d.x *= (mw-fsize.x)/dx;
				dx = mw-fsize.x;
			}
			if (fsize.y + dy < mh && dy != 0)
			{
				d.y *= (mh-fsize.y)/dy;
				dy = mh-fsize.y;
			}
			
			if (d.x)
				fsize.x  += dx;
			if (d.y)
				fsize.y += dy;
			#ifdef DEEP_GUI
				current_center.x += d.x/2;
				current_center.y += d.y/2;
				origin.x += d.x/2;
				origin.y += d.y/2;
				destination.x += d.x/2;
				destination.y += d.y/2;
			#else
				x += d.x /2;
				y += d.y /2;
			#endif
			sizeChanged = layoutChanged = visualChanged = true;
			size.width = (unsigned)round(fsize.x);
			size.height = (unsigned)round(fsize.y);
			//UpdateLayout();
		}
		
		
		void	Window::UpdateLayout()
		{
			Rect<float>	reg(0,0,/*ceil*/(size.width),/*ceil*/(size.height));
			if (layout)
				layout->UpdateCells(reg,cellLayout);
			else
				cellLayout.Clear(reg);
			const Rect<float>&client = cellLayout.client;
			if (rootComponent)
				rootComponent->UpdateLayout(client);
			layoutChanged = false;			
		}
		
		float	Window::GetMinWidth()	const
		{
			float rs=0;
			if (rootComponent)
				rs = rootComponent->GetMinWidth(true);
			if (layout)
			{
				rs += layout->clientEdge.left+layout->clientEdge.right;
				if (rs < layout->minWidth)
					rs = layout->minWidth;
			}
			return rs;
		}
		
		void	Window::SetSize(float width, float height)
		{
			fsize.x = width;
			size.width = (unsigned)round(fsize.x);
			fsize.y = height;
			size.height = (unsigned)round(fsize.y);
			sizeChanged = layoutChanged = visualChanged = true;
		}
		
		void	Window::SetHeight(float height)
		{
			unsigned heighti = (unsigned)round(height);
			{
				fsize.y = height;
				if (size.height != heighti)
				{
					size.height = heighti;
					sizeChanged = layoutChanged = visualChanged = true;
				}
			}

		}
		
		void	Window::SetWidth(float width)
		{
			unsigned widthi = (unsigned)round(width);
			fsize.x = width;
			if (widthi != size.width)
			{
				size.width = widthi;
				sizeChanged = layoutChanged = visualChanged = true;
			}
		}
		
		float	Window::GetMinHeight()	const
		{
			float rs=0;
			if (rootComponent)
				rs = rootComponent->GetMinHeight(true);
			if (layout)
			{
				rs += layout->clientEdge.bottom+layout->clientEdge.top;
				if (rs < layout->minHeight)
					rs = layout->minHeight;
			}
			return rs;
		}
		
		#ifdef DEEP_GUI
			void	Window::setShellDestination(float new_shell_radius)
			{
				if (destination.shellRadius == new_shell_radius)
					return;
				origin = current_center;
				destination.shellRadius = new_shell_radius;
				progress = 0;
			}
		#endif
		
		
		void			Window::SignalMouseUp(float x, float y)
		{
			#ifdef DEEP_GUI
				x = (x-current_center.x)*current_center.shellRadius;
				y = (y-current_center.y)*current_center.shellRadius;
			#else
				x = (x-this->x);
				y = (y-this->y);
			#endif
			
			x += fsize.x*0.5;
			y += fsize.y*0.5;
			Apply(clicked->OnMouseUp(x,y));
		}
		

		Window::ClickResult::value_t			Window::Resolve(float x_, float y_, float&inner_x, float&inner_y)
		{
			#ifdef DEEP_GUI
				float x = (x_-current_center.x)*current_center.shellRadius+fsize.x/2;
				float y = (y_-current_center.y)*current_center.shellRadius+fsize.y/2;
			#else
				float x = (x_-this->x)+fsize.x/2;
				float y = (y_-this->y)+fsize.y/2;
			#endif
			inner_x = x;
			inner_y = y;
			
			if (!cellLayout.border.contains(x,y))
				return ClickResult::Missed;
			
			if (!fixedPosition)
			{
				if (sizeChange == SizeChange::Free || sizeChange == SizeChange::FixedHeight)
				{
					if (x > cellLayout.border.x.max-BorderWidth)
					{
						if (sizeChange != SizeChange::FixedHeight)
						{
							if (y > cellLayout.border.y.max-BorderWidth)
								return ClickResult::ResizeTopRight;
							if (y < cellLayout.border.y.min+BorderWidth)
								return ClickResult::ResizeBottomRight;
						}
						return ClickResult::ResizeRight;
					}
					if (x < cellLayout.border.x.min+BorderWidth)
					{
						if (sizeChange != SizeChange::FixedHeight)
						{
							if (y > cellLayout.border.y.max-BorderWidth)
								return ClickResult::ResizeTopLeft;
							if (y < cellLayout.border.y.min+BorderWidth)
								return ClickResult::ResizeBottomLeft;
						}
						return ClickResult::ResizeLeft;
					}
				}
				
				if (sizeChange != SizeChange::FixedHeight && sizeChange != SizeChange::Fixed)
				{
					if (y > cellLayout.border.y.max-BorderWidth)
						return ClickResult::ResizeTop;
					if (y < cellLayout.border.y.min+BorderWidth)
						return ClickResult::ResizeBottom;
				}
			}
				
			if (rootComponent && (!layout || cellLayout.client.contains(x,y))&&rootComponent->visible&&rootComponent->enabled&&rootComponent->cellLayout.border.contains(x,y))
				return ClickResult::Component;
			if (fixedPosition)
				return ClickResult::Ignored;
			return ClickResult::DragWindow;
		}

		
		void	Window::Apply(Component::eEventResult rs)
		{
			switch (rs)
			{
				case Component::RequestingReshape:
					layoutChanged = true;
				case Component::RequestingRepaint:
					visualChanged = true;
				break;
			}
		}

		
		
		float Operator::radiusOf(index_t stack_layer)	const
		{
			return 0.9+0.1*(windowStack.count()-stack_layer);
		}


		/*static*/ GLShader::Instance	Renderer::layerMerger;
		/*static*/ GLShader::Variable	Renderer::clearColorVariable;
		/*static*/ GLShader::Instance	NormalRenderer::normalRenderer;
		/*static*/ GLShader::Variable	NormalRenderer::normalSystemVariable;


		void		Renderer::_SetView(const Rect<int>&port)
		{
			static const float zFar = 1.f;
			static const float zNear = -1.f;
			TMatrix4<>	projection;
			float	xscale = 1.f/float(port.width())*2.f,
					yscale = 1.f/float(port.height())*2.f,
					zscale = 1.f/(zFar-zNear)*2.f,
					xoffset = -float(port.x.min)*xscale-1.f,
					yoffset = -float(port.y.min)*yscale-1.f,
					zoffset = -(zNear+zFar)/2.f*zscale;

			Vec::def(projection.x,	xscale,0,0,0);
			Vec::def(projection.y, 0,yscale,0,0);
			Vec::def(projection.z, 0,0,-zscale,0);
			Vec::def(projection.w, xoffset,yoffset,zoffset,1);

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(projection.v);
			glMatrixMode(GL_MODELVIEW);
		}

		
		
		void		Renderer::_Apply(const Rect<int>&port)
		{
			glViewport(port.x.min,port.y.min,port.width(),port.height());
			_SetView(port);
		}
		
		void		Renderer::Clip(const Rect<float>&region)	//!< Focuses on an area by applying the current viewport and translation to the specified region and further limiting the viewport. The existing translation will be modified by dx and dy
		{
			Rect<int>&next = clipStack.append();
			//next = region;
			next.x.min = (int)floor(region.x.min);
			next.y.min = (int)floor(region.y.min);
			next.x.max = (int)ceil(region.x.max);
			next.y.max = (int)ceil(region.y.max);
			if (clipStack.count() > 1)
			{
				const Rect<int>&prev = clipStack.fromEnd(1);
				next.constrainBy(prev);
			}
			_Apply(next);
		}
		
		void		Renderer::Unclip()	//!< Reverts the focus process by jumping back to the next upper focus
		{
			ASSERT__(clipStack.isNotEmpty());
			clipStack.eraseLast();
			if (clipStack.isNotEmpty())
				_Apply(clipStack.last());
			else
				_Apply(Rect<int>(0,0,subRes.width,subRes.height));
		}

		void		Renderer::TextureRect(const Rect<>&rect)
		{
			glBegin(GL_QUADS);
				glTexCoord2f(0,0); glVertex2f(rect.x.min,rect.y.min);
				glTexCoord2f(1,0); glVertex2f(rect.x.max,rect.y.min);
				glTexCoord2f(1,1); glVertex2f(rect.x.max,rect.y.max);
				glTexCoord2f(0,1); glVertex2f(rect.x.min,rect.y.max);
			glEnd();
			layerIsDirty = true;
		}
		void		Renderer::TextureRect(const Rect<>&rect,const Rect<>&texCoordRect)
		{
			glBegin(GL_QUADS);
				glTexCoord2f(texCoordRect.x.min,texCoordRect.y.min); glVertex2f(rect.x.min,rect.y.min);
				glTexCoord2f(texCoordRect.x.max,texCoordRect.y.min); glVertex2f(rect.x.max,rect.y.min);
				glTexCoord2f(texCoordRect.x.max,texCoordRect.y.max); glVertex2f(rect.x.max,rect.y.max);
				glTexCoord2f(texCoordRect.x.min,texCoordRect.y.max); glVertex2f(rect.x.min,rect.y.max);
			glEnd();
			layerIsDirty = true;
		}
		void		Renderer::TextureQuad(const TVec2<>&p0, const TVec2<>&p1, const TVec2<>&p2, const TVec2<>&p3)
		{
			glBegin(GL_QUADS);
				glTexCoord2f(0,0); glVertex2fv(p0.v);
				glTexCoord2f(1,0); glVertex2fv(p1.v);
				glTexCoord2f(1,1); glVertex2fv(p2.v);
				glTexCoord2f(0,1); glVertex2fv(p3.v);
			glEnd();
			layerIsDirty = true;
		}

		void		Renderer::FillRect(const Rect<>&rect)
		{
			glBegin(GL_QUADS);
				glVertex2f(rect.x.min,rect.y.min);
				glVertex2f(rect.x.max,rect.y.min);
				glVertex2f(rect.x.max,rect.y.max);
				glVertex2f(rect.x.min,rect.y.max);
			glEnd();
			layerIsDirty = true;
		}

		void		Renderer::FillQuad(const TVec2<>&p0, const TVec2<>&p1, const TVec2<>&p2, const TVec2<>&p3)
		{
			glBegin(GL_QUADS);
				glVertex2fv(p0.v);
				glVertex2fv(p1.v);
				glVertex2fv(p2.v);
				glVertex2fv(p3.v);
			glEnd();
			layerIsDirty = true;
		}



		void		Renderer::Configure(const TFrameBuffer&buffer, const Resolution& usage)
		{
			stackedTargets[1] = buffer;
			if (layerTarget.frameBuffer == 0)
			{
				layerTarget = glExtensions.CreateFrameBuffer(buffer.resolution,Engine::DepthStorage::NoDepthStencil,1,&GLType<GLbyte>::rgba_type_constant);
				stackedTargets[0] = glExtensions.CreateFrameBuffer(buffer.resolution,Engine::DepthStorage::NoDepthStencil,1,&GLType<GLbyte>::rgba_type_constant);
			}
			else
			{
				bool resize = false;
				Resolution res = layerTarget.resolution;
				if (usage.width > res.width)
				{
					res.width = buffer.resolution.width;
					resize = true;
				}
				if (usage.height > res.height)
				{
					res.height = buffer.resolution.height;
					resize = true;
				}
				if (resize)
				{
					glExtensions.ResizeFrameBuffer(layerTarget,res);
					glExtensions.ResizeFrameBuffer(stackedTargets[0],res);
				}
			}
			layerCounter = 0;
			targetingFinal = true;
			layerIsDirty = false;
			clipStack.clear();
			subRes = usage;
			glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_VIEWPORT_BIT);
			glExtensions.BindFrameBuffer(buffer);
			glViewport(0,0,subRes.width,subRes.height);
			glClearColor(clearColor.r,clearColor.g,clearColor.b,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_BLEND);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			_SetView(Rect<int>(0,0,subRes.width,subRes.height));




			if (layerMerger.IsEmpty())
			{
				ASSERT1__(layerMerger.LoadComposition(
					"vertex{void main(){gl_Position=gl_Vertex;gl_TexCoord[0]=gl_MultiTexCoord0;gl_TexCoord[1]=gl_MultiTexCoord1;}}"
					"fragment{"
						"uniform sampler2D lowerLayer,upperLayer;"
						"uniform vec3 clearColor;"
						"void main(){"
							"vec4 lowerSample = texture2DLod(lowerLayer,gl_TexCoord[0].xy,0.0);"
							"vec4 upperSample = texture2DLod(upperLayer,gl_TexCoord[1].xy,0.0);"
							"float div = lowerSample.a + upperSample.a - lowerSample.a * upperSample.a;"
							"gl_FragColor.a = div;"
							"if (div == 0.0) gl_FragColor.rgb = clearColor;"
							"else gl_FragColor.rgb = (lowerSample.rgb * lowerSample.a + upperSample.rgb * upperSample.a - lowerSample.rgb * lowerSample.a * upperSample.a) / div;"
							//"if (gl_FragColor.a > 0 && length(gl_FragColor.rgb) > 0.0) gl_FragColor.rgb = normalize(gl_FragColor.rgb*2.0 - 1.0)*0.5 + 0.5; else gl_FragColor.rgb = vec3(0.5,0.5,1.0);"
						"}"
					"}"
					),layerMerger.Report());
				layerMerger.FindVariable("upperLayer").SetInt(1);
				clearColorVariable = layerMerger.FindVariable("clearColor");
			}

			clearColorVariable.Set(clearColor);

		}

		void		Renderer::MarkNewLayer()
		{
			if (!layerIsDirty)
				return;
			layerIsDirty = false;
			
			glExtensions.UnbindFrameBuffer();

			if (layerCounter == 0)
			{
				targetingFinal = false;
			}
			else
			{
				//if (layerCounter <= 2)
				{
					TVec2<>	targetUsage = {float(subRes.width) / float(stackedTargets[!targetingFinal].resolution.width),
										float(subRes.height) / float(stackedTargets[!targetingFinal].resolution.height)};
					TVec2<>	layerUsage = {float(subRes.width) / float(layerTarget.resolution.width),
										float(subRes.height) / float(layerTarget.resolution.height)};

					glBindTexture(GL_TEXTURE_2D,stackedTargets[!targetingFinal].colorTarget[0].textureHandle);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D,layerTarget.colorTarget[0].textureHandle);
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
					glExtensions.BindFrameBuffer(stackedTargets[targetingFinal]);
					glViewport(0,0,subRes.width,subRes.height);
					layerMerger.Install();
					glBegin(GL_QUADS);
						glTexCoord2f(0,0); glMultiTexCoord2f(GL_TEXTURE1,0,0); glVertex2f(-1,-1);
						glTexCoord2f(targetUsage.x,0); glMultiTexCoord2f(GL_TEXTURE1,layerUsage.x,0); glVertex2f(1,-1);
						glTexCoord2f(targetUsage.x,targetUsage.y); glMultiTexCoord2f(GL_TEXTURE1,layerUsage.x,layerUsage.y); glVertex2f(1,1);
						glTexCoord2f(0,targetUsage.y); glMultiTexCoord2f(GL_TEXTURE1,0,layerUsage.y); glVertex2f(-1,1);
					glEnd();
					layerMerger.Uninstall();
					glExtensions.UnbindFrameBuffer();
					glBindTexture(GL_TEXTURE_2D,0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D,0);
					targetingFinal = !targetingFinal;
				}
			}
			layerCounter++;
			glExtensions.BindFrameBuffer(layerTarget);
			glViewport(0,0,subRes.width,subRes.height);
			glClearColor(clearColor.r,clearColor.g,clearColor.b,0);
			glClear(GL_COLOR_BUFFER_BIT);


			if (clipStack.isNotEmpty())
				_Apply(clipStack.last());
			else
				_Apply(Rect<int>(0,0,subRes.width,subRes.height));


		}


		void		Renderer::Finish()
		{
			ASSERT__(clipStack.isEmpty());

			MarkNewLayer();
			bool mustCopy = layerCounter > 0 && targetingFinal;

			glExtensions.UnbindFrameBuffer();


			if (mustCopy)
				ASSERT__(glExtensions.CopyFrameBuffer(stackedTargets[0],stackedTargets[1],subRes,true,false));

			glPopAttrib();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			glBindTexture(GL_TEXTURE_2D,stackedTargets[1].colorTarget[0].textureHandle);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			//glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,0);
		}


		/*virtual*/					Renderer::~Renderer()
		{
			if (layerTarget.frameBuffer != 0)
			{
				glExtensions.DestroyFrameBuffer(layerTarget);
				glExtensions.DestroyFrameBuffer(stackedTargets[0]);
			}
		}




		void					ColorRenderer::_UpdateState()
		{
			glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,0);
			glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,0);
		}

		void					ColorRenderer::_UpdateState(const GL::Texture::Reference&ref)
		{
			glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,0);
			glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,ref.GetHandle());
		}
		void					ColorRenderer::_UpdateState(const GL::Texture::Reference&ref0,const GL::Texture::Reference&ref1)
		{
			glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,ref1.GetHandle());
			glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,ref0.GetHandle());
		}
		

		void					ColorRenderer::ModulateColor(const TVec4<>&color)
		{
			ModulateColor(color.red,color.green,color.blue,color.alpha);
		}
		
		void					ColorRenderer::ModulateColor(const TVec3<>&color, float alpha/*=1.f*/)
		{
			ModulateColor(color.red,color.green,color.blue,alpha);
		}
		void					ColorRenderer::ModulateColor(float greyTone, float alpha/*=1.f*/)
		{
			ModulateColor(greyTone,greyTone,greyTone,alpha);
		}
		
		void					ColorRenderer::ModulateColor(float r, float g, float b, float a /*= 1.f*/)
		{
			color.red *= r;
			color.green *= g;
			color.blue *= b;
			color.alpha *= a;
			glColor4fv(color.v);
		}

		void					ColorRenderer::SetTextPosition(float x, float y)
		{
			textout.locate(x,y);
		}

		void					ColorRenderer::WriteText(const String&text)
		{
			textout.color(color);
			textout.write(text);
			layerIsDirty = true;
		}

		void					ColorRenderer::WriteText(const char*text, count_t numChars)
		{
			textout.color(color);
			textout.write(text,numChars);
			layerIsDirty = true;
		}

		float					ColorRenderer::GetUnscaledWidth(const char*text, count_t numChars)
		{
			return textout.unscaledLength(text,numChars);
		}

		void					ColorRenderer::SetPointSize(float size)
		{
			glPointSize(size);
		}
		void					ColorRenderer::PaintPoint(float x, float y)
		{
			glBegin(GL_POINTS);
				glVertex2f(x,y);
			glEnd();
			layerIsDirty = true;
		}
		void					ColorRenderer::RenderLine(float x0, float y0, float x1, float y1)
		{
			glBegin(GL_LINES);
				glVertex2f(x0,y0);
				glVertex2f(x1,y1);
			glEnd();
			layerIsDirty = true;
		}

		void					ColorRenderer::FillRect(const Rect<>&rect)
		{
			_UpdateState();
			Renderer::FillRect(rect);
		}


		void		ColorRenderer::FillQuad(const TVec2<>&p0, const TVec4<>&color0, const TVec2<>&p1, const TVec4<>&color1, const TVec2<>&p2, const TVec4<>&color2, const TVec2<>&p3,const TVec4<>&color3)
		{
			_UpdateState();
			glBegin(GL_QUADS);
				glColor4f(color0.r * color.r, color0.g * color.g, color0.b * color.b, color0.a * color.a);
				glVertex2fv(p0.v);
				glColor4f(color1.r * color.r, color1.g * color.g, color1.b * color.b, color1.a * color.a);
				glVertex2fv(p1.v);
				glColor4f(color2.r * color.r, color2.g * color.g, color2.b * color.b, color2.a * color.a);
				glVertex2fv(p2.v);
				glColor4f(color3.r * color.r, color3.g * color.g, color3.b * color.b, color3.a * color.a);
				glVertex2fv(p3.v);
			glEnd();
			glColor4fv(color.v);
			layerIsDirty = true;
		}
		void					ColorRenderer::TextureRect(const Rect<>&rect,const GL::Texture::Reference&ref)
		{
			_UpdateState(ref);
			Renderer::TextureRect(rect);
		}

		void					ColorRenderer::TextureRect(const Rect<>&rect, const Rect<>&texCoordRect,const GL::Texture::Reference&ref)
		{
			_UpdateState(ref);
			Renderer::TextureRect(rect,texCoordRect);
		}
		
		void					ColorRenderer::TextureRect(const Rect<>&rect,const GL::Texture::Reference&ref0,const GL::Texture::Reference&ref1)
		{
			_UpdateState(ref0,ref1);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0); glMultiTexCoord2f(GL_TEXTURE1,0,0); glVertex2f(rect.x.min,rect.y.min);
				glTexCoord2f(1,0); glMultiTexCoord2f(GL_TEXTURE1,1,0); glVertex2f(rect.x.max,rect.y.min);
				glTexCoord2f(1,1); glMultiTexCoord2f(GL_TEXTURE1,1,1); glVertex2f(rect.x.max,rect.y.max);
				glTexCoord2f(0,1); glMultiTexCoord2f(GL_TEXTURE1,0,1); glVertex2f(rect.x.min,rect.y.max);
			glEnd();
			layerIsDirty = true;
		}
		
		void					ColorRenderer::TextureRect(const Rect<>&rect, const Rect<>&texCoordRect,const GL::Texture::Reference&ref0,const GL::Texture::Reference&ref1)
		{
			_UpdateState(ref0,ref1);
			glBegin(GL_QUADS);
				glTexCoord2f(texCoordRect.x.min,texCoordRect.y.min); glMultiTexCoord2f(GL_TEXTURE1,texCoordRect.x.min,texCoordRect.y.min); glVertex2f(rect.x.min,rect.y.min);
				glTexCoord2f(texCoordRect.x.max,texCoordRect.y.min); glMultiTexCoord2f(GL_TEXTURE1,texCoordRect.x.max,texCoordRect.y.min); glVertex2f(rect.x.max,rect.y.min);
				glTexCoord2f(texCoordRect.x.max,texCoordRect.y.max); glMultiTexCoord2f(GL_TEXTURE1,texCoordRect.x.max,texCoordRect.y.max); glVertex2f(rect.x.max,rect.y.max);
				glTexCoord2f(texCoordRect.x.min,texCoordRect.y.max); glMultiTexCoord2f(GL_TEXTURE1,texCoordRect.x.min,texCoordRect.y.max); glVertex2f(rect.x.min,rect.y.max);
			glEnd();
			layerIsDirty = true;
		}

		void					ColorRenderer::TextureQuad(const TVec2<>&p0, const TVec2<>&p1, const TVec2<>&p2, const TVec2<>&p3, const GL::Texture::Reference&ref)
		{
			_UpdateState(ref);
			Renderer::TextureQuad(p0,p1,p2,p3);
		}
		void					ColorRenderer::TextureQuad(const TVec2<>&p0, const TVec2<>&p1, const TVec2<>&p2, const TVec2<>&p3, const GL::Texture::Reference&ref0, const GL::Texture::Reference&ref1)
		{
			_UpdateState(ref0,ref1);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0); glMultiTexCoord2f(GL_TEXTURE1,0,0); glVertex2fv(p0.v);
				glTexCoord2f(1,0); glMultiTexCoord2f(GL_TEXTURE1,1,0); glVertex2fv(p1.v);
				glTexCoord2f(1,1); glMultiTexCoord2f(GL_TEXTURE1,1,1); glVertex2fv(p2.v);
				glTexCoord2f(0,1); glMultiTexCoord2f(GL_TEXTURE1,0,1); glVertex2fv(p3.v);
			glEnd();
			layerIsDirty = true;
		}

		void					ColorRenderer::PushColor()
		{
			colorStack << color;
		}

		void					ColorRenderer::PopColor()
		{
			color = colorStack.pop();
			glColor4fv(color.v);
		}
		
		void					ColorRenderer::PeekColor()
		{
			color = colorStack.last();
			glColor4fv(color.v);
		}


		void					ColorRenderer::MarkNewLayer()
		{
			_UpdateState();
			Renderer::MarkNewLayer();
			glColor4fv(color.v);
		}


		void					ColorRenderer::Configure(const TFrameBuffer&buffer, const Resolution& usage)
		{
			Vec::set(color,1);
			glWhite();
			colorStack.clear();
			glEnable(GL_POINT_SMOOTH);
			Renderer::Configure(buffer,usage);
		}

		void					ColorRenderer::Finish()
		{
			_UpdateState();
			ASSERT__(colorStack.isEmpty());
			Renderer::Finish();
		}


		void					NormalRenderer::ScaleNormals(float x, float y)
		{
			Vec::mult(normalSystem.x,x);
			Vec::mult(normalSystem.y,y);
			DBG_VERIFY__(normalSystemVariable.Set(normalSystem));
		}

		void					NormalRenderer::ScaleNormals(const TVec2<>&v)
		{
			ScaleNormals(v.x,v.y);
		}

		void					NormalRenderer::ScaleNormals(float x, float y, float z)
		{
			Vec::mult(normalSystem.x,x);
			Vec::mult(normalSystem.y,y);
			Vec::mult(normalSystem.z,z);
			DBG_VERIFY__(normalSystemVariable.Set(normalSystem));
		}

		void					NormalRenderer::ScaleNormals(const TVec3<>&v)
		{
			ScaleNormals(v.x,v.y,v.z);
		}

		void					NormalRenderer::PushNormalMatrix()
		{
			normalSystemStack << normalSystem;
		}

		void					NormalRenderer::PopNormalMatrix()
		{
			normalSystem = normalSystemStack.pop();
			DBG_VERIFY__(normalSystemVariable.Set(normalSystem));
		}

		void					NormalRenderer::PeekNormalMatrix()
		{
			normalSystem = normalSystemStack.last();
			DBG_VERIFY__(normalSystemVariable.Set(normalSystem));
		}

		void					NormalRenderer::TransformNormals(const TMatrix3<>&m)
		{
			TMatrix3<> temp;
			Mat::mult(m,normalSystem,temp);
			normalSystem = temp;
			DBG_VERIFY__(normalSystemVariable.Set(normalSystem));
		}



		void					NormalRenderer::TextureRect(const Rect<>&rect,const GL::Texture::Reference&ref)
		{
			_UpdateState(ref);
			Renderer::TextureRect(rect);
		}

		void					NormalRenderer::TextureRect(const Rect<>&rect, const Rect<>&texCoordRect,const GL::Texture::Reference&ref)
		{
			_UpdateState(ref);
			Renderer::TextureRect(rect,texCoordRect);
		}

		void					NormalRenderer::TextureQuad(const TVec2<>&p0, const TVec2<>&p1, const TVec2<>&p2, const TVec2<>&p3, const GL::Texture::Reference&ref)
		{
			_UpdateState(ref);
			Renderer::TextureQuad(p0,p1,p2,p3);
		}

		void					NormalRenderer::MarkNewLayer()
		{
			normalRenderer.Uninstall();
			glBindTexture(GL_TEXTURE_2D,0);
			Renderer::MarkNewLayer();
			normalRenderer.Install();
		}

		void					NormalRenderer::_UpdateState(const GL::Texture::Reference&ref)
		{
			glBindTexture(GL_TEXTURE_2D,ref.GetHandle());
		}
	
		void					NormalRenderer::Configure(const TFrameBuffer&buffer, const Resolution& usage)
		{
			if (normalRenderer.IsEmpty())
			{
				ASSERT1__(normalRenderer.LoadComposition(
					"vertex{void main(){gl_Position=ftransform();gl_TexCoord[0]=gl_MultiTexCoord0;}}"
					"fragment{"
						"uniform sampler2D texture;"
						"uniform mat3 normalSystem;"
						"void main(){"
							"vec4 normalSample = texture2D(texture,gl_TexCoord[0].xy);"
							"gl_FragColor.a = normalSample.a;"
							"gl_FragColor.rgb = normalize(normalSystem * (normalSample.xyz*2.0 - 1.0)) * 0.5 + 0.5;"
						"}"
					"}"
					),normalRenderer.Report());
				normalSystemVariable = normalRenderer.FindVariable("normalSystem");
			}

			Renderer::Configure(buffer,usage);

			normalRenderer.Install();
			Mat::eye(normalSystem);
			DBG_VERIFY__(normalSystemVariable.Set(normalSystem));
		}

		void					NormalRenderer::Finish()
		{
			normalRenderer.Uninstall();
			ASSERT__(normalSystemStack.isEmpty());

			Renderer::Finish();
			glDisable(GL_TEXTURE_2D);
		}





		Component::Component(const String&type_name_):currentRegion(0,0,1,1),offset(0,0,0,0),anchored(false,false,false,false),width(1),height(1),typeName(type_name_),layout(NULL),tickInterval(0.2),enabled(true),visible(true)
		{}
		
		void	Component::SignalKeyDown(int key)
		{
			//cout << "caught "<<key<<endl;
			eEventResult rs = Unsupported;
			if (focused)
			{
				rs = focused->OnKeyDown((Key::Name)key);
				if (focused && !focused->windowLink.expired())	//key down may do all sorts of things, unset focused or remove it from its window
					focused->windowLink.lock()->Apply(rs);
				if (focused && rs == Unsupported && key == Key::Tab)
				{
					shared_ptr<Component> next=focused;
					while ((next = focused->windowLink.lock()->rootComponent->GetSuccessorOfChild(next)) && !next->IsTabFocusable());
					if (next)
						SetFocused(next);
					return;
				}
			}
			if (rs == Unsupported)
			{
				//cout << "forwarding down"<<endl;
				input.cascadeKeyDown((Key::Name)key);
			}
		}
		
		void	Component::SignalKeyUp(int key)
		{
			eEventResult rs = Unsupported;
			if (focused)
			{
				rs = focused->OnKeyUp((Key::Name)key);
				focused->windowLink.lock()->Apply(rs);
			}
			if (rs == Unsupported)
				input.cascadeKeyUp((Key::Name)key);
		}
		
		
		
		
		Component::~Component()
		{
			/*auto shared_this = shared_from_this();
			if (handlingEvent && ext.caught_by == shared_this)
				ext.caught_by.reset();
			if (focused == shared_this)
				focused.reset();
			if (hovered == shared_this)
				hovered.reset();
			if (clicked == shared_this)
				clicked.reset();*/
			//stupid, because destructor will only be invoked once no shared reference exists
		}
		
		bool		Component::IsFocused()	const
		{
			return focused.get() == this;
		}
		
		void		Component::Locate(const Rect<float>&parent_region,Rect<float>&region)	const
		{
			float	w = parent_region.width(),
					h = parent_region.height();
			for (BYTE k = 0; k < 4; k++)
				if (anchored.value[k])
					region[k] = parent_region[k]+offset.value[k];
			if (!anchored.left)
				if (anchored.right)
					region.x.min = region.x.max-width;
				else
				{
					region.x.min = parent_region.x.center()-width/2;
					region.x.max = region.x.min+width;
				}
			else
				if (!anchored.right)
					region.x.max = region.x.min+width;

			if (!anchored.bottom)
				if (anchored.top)
					region.y.min = region.y.max-height;
				else
				{
					region.y.min = parent_region.y.center()-height/2;
					region.y.max = region.y.min+height;
				}
			else
				if (!anchored.top)
					region.y.max = region.y.min+height;
		
		}
		
		void		Component::UpdateLayout(const Rect<float>&parent_region)
		{
			Locate(parent_region,currentRegion);
			
			if (layout)
				layout->UpdateCells(currentRegion,cellLayout);
			else
				cellLayout.Clear(currentRegion);
		}
		
		float		Component::GetMinWidth(bool includeOffsets)	const
		{
			float rs = GetClientMinWidth();
			if (anchored.right && includeOffsets)
				rs -= offset.right;
			if (anchored.left && includeOffsets)
				rs += offset.left;
			if (layout)
			{
				rs += layout->clientEdge.left+layout->clientEdge.right;
				if (rs < layout->minWidth)
					rs = layout->minWidth;
			}
			return rs;
		}
		
		float		Component::GetMinHeight(bool includeOffsets)	const
		{
			float rs = GetClientMinHeight();
			if (anchored.top && includeOffsets)
				rs -= offset.top;
			if (anchored.bottom && includeOffsets)
				rs += offset.bottom;
			if (layout)
			{
				rs += layout->clientEdge.bottom+layout->clientEdge.top;
				if (rs < layout->minHeight)
					rs = layout->minHeight;
			}
			return rs;
		}
		
		
		
		
		
		
		void			Window::SetLayout(Layout*layout)
		{
			this->layout = layout;
			UpdateLayout();
		}
		

		
		void			Window::RenderBuffers(Display<OpenGL>&display)
		{
			//cout << "rendering window "<<x<<", "<<y<<" / "<<fsize.x<<", "<<fsize.y<<endl;
			if (layoutChanged)
				UpdateLayout();
			layoutChanged = false;
			visualChanged = false;
			if (sizeChanged)
				onResize();
			sizeChanged = false;
			ASSERT_GREATER__(fsize.x,0);
			ASSERT_GREATER__(fsize.y,0);
			unsigned	ex = (unsigned)ceil(log((float)size.width)/M_LN2),
						ey = (unsigned)ceil(log((float)size.height)/M_LN2);
			if (!ex || !ey)
				return;
			bool do_resize = ex > exp.x || ey > exp.y || ex+1 < exp.x || ey+1 < exp.y;

			Resolution target_resolution (1<<ex,1<<ey);
			
			if (!colorBuffer.colorTarget[0].textureHandle)
			{
				colorBuffer = glExtensions.CreateFrameBuffer(target_resolution,Engine::DepthStorage::NoDepthStencil,1,&GLType<GLbyte>::rgba_type_constant);
				normalBuffer = glExtensions.CreateFrameBuffer(target_resolution,Engine::DepthStorage::NoDepthStencil,1,&GLType<GLbyte>::rgba_type_constant);
				exp.x = ex;
				exp.y = ey;
			}
			elif (do_resize)
			{
				ASSERT__(glExtensions.ResizeFrameBuffer(colorBuffer,target_resolution));
				ASSERT__(glExtensions.ResizeFrameBuffer(normalBuffer,target_resolution));
				exp.x = ex;
				exp.y = ey;
			}
			
			unsigned	resx = (1<<exp.x),
						resy = (1<<exp.y);
			ASSERT__(colorBuffer.colorTarget[0].textureHandle != 0);
			ASSERT__(normalBuffer.colorTarget[0].textureHandle != 0);
			ASSERT2__(size.width <= resx,size.width,resx);
			ASSERT2__(size.height <= resy,size.height,resy);
			
			//height = ceil(height);
			usage.x = /*ceil*/(float)(size.width)/(float)resx;
			usage.y = /*ceil*/(float)(size.height)/(float)resy;
			
			
			glWhite();
		
			//normal renderer:
			//display.lockRegion();
			//
			//normalShader.install();
			//glMatrixMode(GL_TEXTURE);
			//glLoadIdentity();
			//glMatrixMode(GL_MODELVIEW);
			//
			//Rect<float>	view(0,0,size.width,size.height);
				//ASSERT__(glExtensions.bindFrameBuffer(normalBuffer));
				//glClearColor(0,0,0,0);
				//glClear(GL_COLOR_BUFFER_BIT);
				//glDisable(GL_BLEND);
				//op->resetFocus();
				//op->focus(view);

			//finish:
			//	normalShader.uninstall();
			//ASSERT__(glExtensions.bindFrameBuffer(colorBuffer));
			//glClearColor(0,0,0,0);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glDisable(GL_BLEND);

			//color renderer:
			//			ASSERT__(glExtensions.bindFrameBuffer(colorBuffer));
			//glClearColor(0,0,0,0);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glDisable(GL_BLEND);
			//all finish:
			//display.unlockRegion();
			//glExtensions.unbindFrameBuffer();


			
			shared_ptr<Operator> op = operatorLink.lock();
			if (op)
			{
				op->normalRenderer.Configure(normalBuffer,Resolution(size.width,size.height));

				for (index_t j = 0; j < cellLayout.cells.count(); j++)
				{
					const TCellInstance&cell = cellLayout.cells[j];
					op->normalRenderer.TextureRect(cell.region,cell.normalTexture);
				}
				if (rootComponent && rootComponent->visible)
				{
					rootComponent->OnNormalPaint(op->normalRenderer,true);
				}
				op->normalRenderer.Finish();
		

				op->colorRenderer.Configure(colorBuffer,Resolution(size.width,size.height));
				for (index_t j = 0; j < cellLayout.cells.count(); j++)
				{
					const TCellInstance&cell = cellLayout.cells[j];
					op->colorRenderer.TextureRect(cell.region,cell.colorTexture);
				}
				if (cellLayout.title.width()>0)
				{
					op->colorRenderer.MarkNewLayer();
					op->colorRenderer.SetTextPosition(cellLayout.title.x.min,cellLayout.title.y.center()-ColorRenderer::textout.getFont().getHeight()/2+font_offset);
					op->colorRenderer.WriteText(title);
				}
				if (rootComponent && rootComponent->visible)
					rootComponent->OnColorPaint(op->colorRenderer,true);
			
				op->colorRenderer.Finish();
			}
			
			
		}

		shared_ptr<Component>		Component::GetSuccessorOfChild(const shared_ptr<Component>&child_link)
		{
			count_t num_children = CountChildren();
			for (index_t i = 0; i < num_children; i++)
			{
				shared_ptr<Component> link = GetChild(i);
				if (link == child_link)
				{
					if (i < num_children-1)
					{
						shared_ptr<Component> next = GetChild(i+1);
						while (next->CountChildren())
							next = next->GetChild(0);
						return next;
					}
					return shared_from_this();
				}
				shared_ptr<Component> successor = link->GetSuccessorOfChild(child_link);
				if (successor)
					return successor;
			}
			return shared_ptr<Component>();
		}
		
		index_t		Component::GetIndexOfChild(const shared_ptr<Component>&child_) const
		{
			count_t num_children = CountChildren();
			for (index_t i = 0; i < num_children; i++)
				if (GetChild(i) == child_)
					return i;
			return TypeInfo<index_t>::max;
		}
		
		
		void			Component::OnColorPaint(ColorRenderer&renderer, bool parentIsEnabled)
		{
			renderer.MarkNewLayer();
			if (!layout)
				return;
			renderer.PushColor();
				if (!enabled || !parentIsEnabled)
					renderer.ModulateColor(0.5);

				for (index_t j = 0; j < cellLayout.cells.count(); j++)
				{
					const TCellInstance&cell = cellLayout.cells[j];
					renderer.TextureRect(cell.region,cell.colorTexture);
				}
			renderer.PopColor();
			renderer.MarkNewLayer();
		}
		
		void			Component::OnNormalPaint(NormalRenderer&renderer, bool parentIsEnabled)
		{
			renderer.MarkNewLayer();
			if (!layout)
				return;
			
			renderer.PushNormalMatrix();
				if (!enabled || !parentIsEnabled)
					renderer.ScaleNormals(0.1f,0.1f,1.f);

				for (index_t j = 0; j < cellLayout.cells.count(); j++)
				{
					const TCellInstance&cell = cellLayout.cells[j];
					renderer.TextureRect(cell.region,cell.normalTexture);
				}
			renderer.PopNormalMatrix();
			renderer.MarkNewLayer();
		}
		
		static void setCursor(Mouse::eCursor cursor)
		{
			static bool was_default = true;
			if (cursor == Mouse::CursorType::Default && was_default)
				return;
			
			mouse.setCursor(cursor);
			was_default = cursor == Mouse::CursorType::Default;
		}
		

		
		
		/**
			Performs a onMouseHover operation on the specified window's component
			
			@return true if the mouse has been set, false otherwise
		*/
		static bool MouseHover(const shared_ptr<Window>&window, const PComponent&component, float x, float y)
		{
			if (!component || !component->IsEnabled())
				return false;
			handlingEvent = true;
			ext.customCursor = Mouse::CursorType::Default;
			Component::eEventResult rs = component->OnMouseHover(x,y,ext);
			if (rs != Component::Unsupported)
			{
				window->Apply(rs);
				setCursor(ext.customCursor);
			}
			
			if (hovered != component)
			{
				if (hovered)
					hovered->GetWindow()->Apply(hovered->OnMouseExit());
				hovered = component;
			}
			handlingEvent = false;
			return rs != Component::Unsupported;
		}
		

		PComponent					Operator::GetComponentUnderMouse(bool*enabledOut/*=NULL*/)	const
		{
			TVec2<float> m;
			unprojectMouse(m);
		
			for (index_t i = windowStack.count()-1; i < windowStack.count(); i--)
			{
				const shared_ptr<Window>&window = windowStack[i];
				float rx,ry;
				Window::ClickResult::value_t rs = window->Resolve(m.x,m.y,rx,ry);
				if (rs != Window::ClickResult::Missed)
				{
					bool IsEnabled = true;
					if (enabledOut)
						*enabledOut = true;
					return window->rootComponent->GetComponent(rx,ry,Component::GenericRequest,enabledOut?*enabledOut:IsEnabled);
				}
			}
			return PComponent();
		}
		PWindow			Operator::GetWindowUnderMouse()	const
		{
			TVec2<float> m;
			unprojectMouse(m);
		
			for (index_t i = windowStack.count()-1; i < windowStack.count(); i--)
			{
				const shared_ptr<Window>&window = windowStack[i];
				float rx,ry;
				Window::ClickResult::value_t rs = window->Resolve(m.x,m.y,rx,ry);
				if (rs != Window::ClickResult::Missed)
					return window;
			}
			return PWindow();
		}


		void			Operator::render()
		{
			if (!display)
				FATAL__("GUI not properly initialized. Display is NULL");
			
			
			//backup aspect configuration
			bool aspect_was_in_projection = GlobalAspectConfiguration::loadAsProjection;
			GlobalAspectConfiguration::loadAsProjection = false;
			bool world_z_was_up = GlobalAspectConfiguration::worldZIsUp;
			GlobalAspectConfiguration::worldZIsUp = false;
			display->storeCamera();


			
			
			if (focused)
			{
				time_since_last_tick += timing.delta;
				if (time_since_last_tick > focused->tickInterval)
				{
					time_since_last_tick = 0;
					focused->GetWindow()->Apply(focused->OnTick());
				}
			}
				
			TVec2<float> m;
			static	TVec2<float> last_m={-100000,-10000};
			unprojectMouse(m);
			if (!Vec::similar(m,last_m) || stack_changed)
			{
				stack_changed = false;
				last_m = m;
				Window::ClickResult::value_t cursor_mode = Window::ClickResult::Missed;
				if (mouse->buttons.down[0] && owns_mouse_down && (windowStack.isNotEmpty() || menu_stack.isNotEmpty()))
				{
					if (dragging)
					{
						cursor_mode = dragType;
						TVec2<float>	d =	{m.x-last.x,
											m.y-last.y};
						if (d.x || d.y)
						{
							if (dragType == Window::ClickResult::DragWindow)
								dragging->Drag(d);
							else
								dragging->DragResize(d,dragType);
							last.x += d.x;
							last.y += d.y;
						}
					}
					else
					{
					/*	if (last_x != mouse.location.fx || last_y != mouse.location.fy)
						{
							windowStack.last()->mouseMove((float)(mouse.location.fx*display->clientWidth()),(float)(mouse.location.fy*display->clientHeight()));
							last_x = mouse.location.fx;
							last_y = mouse.location.fy;
						}*/
					}
				}
				if (clicked)
				{
					shared_ptr<Window>	window = menu_stack.isNotEmpty()?menu_stack.last().lock():windowStack.last();

					if (!window && menu_stack.isNotEmpty())	//last window has been erased
					{
						hideMenus();
						window = windowStack.last();
					}

					#ifdef DEEP_GUI
						float	rx = (m.x-window->current_center.x)*window->current_center.shellRadius,
								ry = (m.y-window->current_center.y)*window->current_center.shellRadius;
					#else
						float	rx = (m.x-window->x),
								ry = (m.y-window->y);
					#endif
					rx += window->fsize.x*0.5;
					ry += window->fsize.y*0.5;
					ASSERT_NOT_NULL1__(clicked->GetWindow(),clicked->typeName);
					clicked->GetWindow()->Apply(clicked->OnMouseDrag(rx,ry));
					cursor_mode = Window::ClickResult::Component;
				}
				elif (cursor_mode == Window::ClickResult::Missed)
				{
					bool isModal = false;
					for (index_t i = menu_stack.count()-1; i < menu_stack.count(); i--)
					{
						shared_ptr<Window> window = menu_stack[i].lock();
						if (!window)
						{
							menu_stack.erase(i);
							continue;
						}
						window->fixedPosition = true;
						float rx,ry;
						Window::ClickResult::value_t rs = window->Resolve(m.x,m.y,rx,ry);
						if (rs != Window::ClickResult::Missed)
						{
							if (isModal)
								rs = Window::ClickResult::Ignored;
						
							cursor_mode = rs;
							if (rs == Window::ClickResult::Component)
							{
								bool IsEnabled = true;
								PComponent component = window->rootComponent->GetComponent(rx,ry, Component::HoverRequest, IsEnabled);
								if (IsEnabled && !MouseHover(window,component,rx,ry))
									cursor_mode = Window::ClickResult::DragWindow;
							}
							isModal = true;
						break;
						}
						isModal = true;
					}
					for (index_t i = windowStack.count()-1; i < windowStack.count(); i--)
					{
						const shared_ptr<Window>&window = windowStack[i];
						float rx,ry;
						Window::ClickResult::value_t rs = window->Resolve(m.x,m.y,rx,ry);
						if (rs != Window::ClickResult::Missed)
						{
							if (isModal)
								rs = Window::ClickResult::Ignored;
							else
							{
								#ifdef DEEP_GUI
									window->setShellDestination(1.00001);
								#else
									//windowStack.append(windowStack.drop(window));
								#endif
								cursor_mode = rs;
								if (rs == Window::ClickResult::Component)
								{
									bool IsEnabled = true;
									PComponent component = window->rootComponent->GetComponent(rx,ry, Component::HoverRequest, IsEnabled);
									if (IsEnabled && !MouseHover(window,component, rx,ry))
										cursor_mode = Window::ClickResult::DragWindow;
								}
								#ifdef DEEP_GUI
									for (unsigned j = i-1; j < windowStack.count(); j--)
										windowStack[j]->setShellDestination(radiusOf(j));
								#endif
							}
							break;
						}
						#ifdef DEEP_GUI
							else
								window->setShellDestination(radiusOf(i));
						#endif
						isModal |= window->isModal;
					}
				}
				switch (cursor_mode)
				{
					case Window::ClickResult::ResizeRight:
						setCursor(Mouse::CursorType::ResizeRight);
					break;
					case Window::ClickResult::ResizeLeft:
						setCursor(Mouse::CursorType::ResizeLeft);
					break;
					case Window::ClickResult::ResizeTop:
						setCursor(Mouse::CursorType::ResizeUp);
					break;
					case Window::ClickResult::ResizeBottom:
						setCursor(Mouse::CursorType::ResizeDown);
					break;
					case Window::ClickResult::ResizeTopLeft:
						setCursor(Mouse::CursorType::ResizeUpLeft);
					break;
					case Window::ClickResult::ResizeBottomRight:
						setCursor(Mouse::CursorType::ResizeDownRight);
					break;
					case Window::ClickResult::ResizeTopRight:
						setCursor(Mouse::CursorType::ResizeUpRight);
					break;
					case Window::ClickResult::ResizeBottomLeft:
						setCursor(Mouse::CursorType::ResizeDownLeft);
					break;
					case Window::ClickResult::Component:
					break;
					default:
						setCursor(Mouse::CursorType::Default);
					break;
				}
				if (cursor_mode == Window::ClickResult::Missed)
				{
					if (hovered)
						hovered->GetWindow()->Apply(hovered->OnMouseExit());
					hovered.reset();
				}
			}

			
			for (index_t i = 0; i < menu_stack.count(); i++)
			{
				shared_ptr<Window> window = menu_stack[i].lock();
				if (!window)
				{
					menu_stack.erase(i--);
					continue;
				}
				if (window->visualChanged)
				{
					if (window->operatorLink.expired())
						window->operatorLink = shared_from_this();	//I'm tired
					window->RenderBuffers(*display);
				}
			}
			for (index_t i = 0; i < windowStack.count(); i++)
			{
				const shared_ptr<Window>&window = windowStack[i];
				if (window->visualChanged)
					window->RenderBuffers(*display);
				#ifdef DEEP_GUI
					float delta = timing.delta;
					if (delta > 0.05)
						delta = 0.05;
					if (window->progress < 1)
					{
						window->progress += 3.0f*delta;
						if (window->progress > 1)
							window->progress = 1;
						_interpolate(window->origin.coord,window->destination.coord,window->progress,window->current_center.coord);
					}
				#endif
				
			}
			
			#ifdef DEEP_GUI
				Sort<DepthSort>::quickSortRAL(windowStack);
				windowStack.revert();
			#endif

			display->pick(projected_space);

			glDisable(GL_CULL_FACE);
			
			glBindTexture(GL_TEXTURE_2D,layer_texture);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			
			if (settingChanged)
			{
				windowShader.SetVariable(blurEffectSwitch,blurEffect);
				windowShader.SetVariable(refractEffectSwitch,refractEffect);
				windowShader.SetVariable(bumpEffectSwitch,bumpEffect);
				//logfile << windowShader.assembleSource()<<nl<<nl;
				currentWindowShader = windowShader.BuildShader();
				if (!currentWindowShader)
					ErrMessage(windowShader.Report());
				settingChanged = false;
			}

			
			ASSERT__(currentWindowShader->Install());
			float	w = display->clientWidth(),
					h = display->clientHeight();
			glWhite();
			for (index_t i = 0; i < windowStack.count(); i++)
			{
				const shared_ptr<Window>&window = windowStack[i];
				
				render(window,w,h,false);

			}
			
			for (index_t i = 0; i < menu_stack.count(); i++)
			{
				shared_ptr<Window> window = menu_stack[i].lock();
				if (!window)
				{
					menu_stack.erase(i--);
					continue;
				}
				render(window,w,h,true);
			}
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,0);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,0);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			/*if (!sky_texture.isEmpty())
			{
				glActiveTexture(GL_TEXTURE3);
				display->useTexture(NULL);
			}*/
			
			glActiveTexture(GL_TEXTURE0);
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
			glBindTexture(GL_TEXTURE_2D,0);
			currentWindowShader->Uninstall();
		
			/*

			for (index_t i = 0; i < windowStack.count(); i++)
			{
				const shared_ptr<Window>&window = windowStack[i];
				
				renderBox(window,w,h,false);

			}
			
			for (index_t i = 0; i < menu_stack.count(); i++)
			{
				shared_ptr<Window> window = menu_stack[i].lock();
				
				renderBox(window,w,h,true);
			}
			*/
			/*
			glWhite();
			glBegin(GL_LINE_LOOP);
				for (unsigned i = 0; i < 20; i++)
					project(-0.5+(float)i/19.0f,-0.5,0,0,1);
				for (unsigned i = 0; i < 20; i++)
					project(0.5-(float)i/19.0f,0.5,0,0,1);
			glEnd();
			
			
			float f[3];
			projected_space.screenToVector(mouse.location.fx*2-1,mouse.location.fy*2-1,Vector<float>::dummy,f);
			_normalize0(f);
			float p[2];
			unproject(f,p);

			float	px = (p[0])/display->clientWidth()*2,
					py = (p[1])/display->clientHeight()*2;
			
			glBegin(GL_LINE_LOOP);
				project(px-0.05,py,0,0,1);
				project(px,py-0.05,0,0,1);
				project(px+0.05,py,0,0,1);
				project(px,py+0.05,0,0,1);
			glEnd();
			
			display->pick(window_space);
			
			drawRect(rect);
			*/
			GlobalAspectConfiguration::loadAsProjection = aspect_was_in_projection;
			GlobalAspectConfiguration::worldZIsUp = world_z_was_up;
			display->restoreCamera();

		}
		
		/**
		@brief Queries whether the specified window is currently part of the local window stack
		@return true, if the specified window is contained in the local window stack, false otherwise.
		*/
		bool			Operator::windowIsVisible(const shared_ptr<Window>&window)	const
		{
			return windowStack.contains(window);
		}
		
		/**
		@brief Queries the top-most window (if any)
		@return Shared pointer to the top most window. May be empty, if the local window stack is empty.
		*/
		shared_ptr<Window>	Operator::getTopWindow() const
		{
			if (windowStack.isEmpty())
				return shared_ptr<Window>();
			return windowStack.last();
		}

		/**
		@brief Checks whether the operator shows at least one modal window (multiple modal windows maty be on top of each other)
		*/
		bool				Operator::showingModalWindows()	const
		{
			return windowStack.isNotEmpty() && windowStack.last()->isModal;
		}


		
		void			Operator::ShowWindow(const shared_ptr<Window>&window)
		{
			if (!window->operatorLink.expired())
			{
				shared_ptr<Operator>	op = window->operatorLink.lock();
				if (op && op.get() != this)
				{
					op->HideWindow(window);
					window->operatorLink = shared_from_this();
				}
			}
			else
				window->operatorLink = shared_from_this();
			if (windowStack.isNotEmpty() && windowStack.last() == window)
				return;
			windowStack.findAndErase(window);

			if (!window->isModal && windowStack.isNotEmpty() && windowStack.last()->isModal)
			{
				index_t at = windowStack.size()-2;
				while (at != InvalidIndex && windowStack[at]->isModal)
					at--;
				windowStack.insert(at+1,window);
			}
			else
			{
				if (windowStack.isNotEmpty())
					windowStack.last()->onFocusLost();
				windowStack << window;
				Component::ResetFocused();
				window->onFocusGained();
			}
			#ifdef DEEP_GUI
				window->current_center.shellRadius = window->destination.shellRadius = window->origin.shellRadius = radiusOf(windowStack-1);
			#endif

			#ifdef DEEP_GUI
				for (unsigned j = 0; j < windowStack.count(); j++)
					windowStack[j]->setShellDestination(radiusOf(j));
			#endif
			stack_changed=true;
		}
		
		bool			Operator::HideWindow(const shared_ptr<Window>&window)
		{
			bool was_top = windowStack.isNotEmpty() && windowStack.last() == window;
				
			if (windowStack.findAndErase(window))
			{
				if (was_top)
				{
					window->onFocusLost();
					Component::ResetFocused();
					if (windowStack.isNotEmpty())
						windowStack.last()->onFocusGained();
				}
				window->operatorLink.reset();
				#ifdef DEEP_GUI
					for (index_t j = 0; j < windowStack.count(); j++)
						windowStack[j]->setShellDestination(radiusOf(j));
				#endif
				window->onHide();
				stack_changed=true;
				return true;
			}
			return false;
		}
		
		
		static bool mouseDown(const shared_ptr<Window>&window, float x, float y)
		{
			handlingEvent = true;
			//ext.caught_by.reset();
			bool IsEnabled = true;
			PComponent component = window->rootComponent->GetComponent(x,y, Component::ClickRequest, IsEnabled);
			if (IsEnabled && component)
			{
				ext.customCursor = Mouse::CursorType::Default;
				Component::eEventResult rs = component->OnMouseDown(x,y,ext);
				clicked.reset();
				if (rs != Component::Unsupported)
				{
					window->Apply(rs);
					clicked = component;
					setCursor(ext.customCursor);
				}
				Component::SetFocused(clicked);
				handlingEvent = false;
				return rs != Component::Unsupported;
			}
			return false;
		}		
		
		bool			Operator::mouseDown()
		{
			TVec2<float> m;
			unprojectMouse(m);
			last = m;
			owns_mouse_down = false;
			for (index_t i = menu_stack.count()-1; i < menu_stack.count(); i--)
			{
				shared_ptr<Window> window = menu_stack[i].lock();
				if (!window)
				{
					menu_stack.erase(i);
					continue;
				}
			
				window->fixedPosition = true;

				float rx, ry;
				Window::ClickResult::value_t rs = window->Resolve(m.x,m.y,rx,ry);
				
				
				if (rs != Window::ClickResult::Missed)
				{
					owns_mouse_down = true;
					dragging.reset();
					if (rs == Window::ClickResult::Component)
						if (!GUI::mouseDown(window,rx,ry))
							rs = window->fixedPosition?Window::ClickResult::Ignored:Window::ClickResult::DragWindow;
					if (rs != Window::ClickResult::Component)
						Component::SetFocused(shared_ptr<Component>());
					return true;
				}
				else
				{
					window->hidden = timing.now64;
					window->onHide();
					menu_stack.erase(i);
				}
			}
			
			for (index_t i = windowStack.count()-1; i < windowStack.count(); i--)
			{
				shared_ptr<Window> window = windowStack[i];
			
				float rx, ry;
				Window::ClickResult::value_t rs = window->Resolve(m.x,m.y,rx,ry);
				
				if (rs != Window::ClickResult::Missed)
				{
					if (rs == Window::ClickResult::Component)
						if (!GUI::mouseDown(window,rx,ry))
							rs = window->fixedPosition?Window::ClickResult::Ignored:Window::ClickResult::DragWindow;
					if (rs != Window::ClickResult::Component)
						Component::ResetFocused();
					if (i+1 != windowStack.count())
					{
						windowStack.last()->onFocusLost();
						windowStack.erase(i);
						Component::ResetFocused();
						windowStack << window;
						window->onFocusGained();
						#ifdef DEEP_GUI
							for (unsigned j = 0; j < windowStack.count(); j++)
								windowStack[j]->setShellDestination(radiusOf(j));
						#endif
					}
					owns_mouse_down = true;
					dragging = (rs >= Window::ClickResult::DragWindow ? window : shared_ptr<Window>());
					dragType = rs;
					return rs != Window::ClickResult::Ignored;	//lets see if this is good
				}
				elif (window->isModal)
					return false;
			}
			Component::SetFocused(shared_ptr<Component>());
			return false;
		}
		
		bool			Operator::mouseWheel(short delta)
		{
			TVec2<float> m;
			unprojectMouse(m);
			for (index_t i = menu_stack.count()-1; i < menu_stack.count(); i--)
			{
				shared_ptr<Window> window = menu_stack[i].lock();
				if (!window)
				{
					menu_stack.erase(i);
					continue;
				}
				
				#ifdef DEEP_GUI
					float x = (m.x-window->current_center.x)*window->current_center.shellRadius+window->fsize.x/2;
					float y = (m.y-window->current_center.y)*window->current_center.shellRadius+window->fsize.y/2;
				#else
					float x = (m.x-window->x)+window->fsize.x/2;
					float y = (m.y-window->y)+window->fsize.y/2;
				#endif
				if (!window->GetCellLayout().border.contains(x,y))
				{
					window->hidden = timing.now64;
					menu_stack.erase(i);
					continue;
				}
				
				if (window->rootComponent && window->rootComponent->IsVisible() && window->rootComponent->IsEnabled())
				{
					bool IsEnabled = true;
					PComponent component = window->rootComponent->GetComponent(x,y,Component::MouseWheelRequest, IsEnabled);
					if (IsEnabled && component)
					{
						Component::eEventResult rs = component->OnMouseWheel(x,y,delta);
						window->Apply(rs);
						if (rs != Component::Unsupported)
							MouseHover(window,component,x,y);
					}
				}
				return true;
			}
			for (index_t i = windowStack.count()-1; i < windowStack.count(); i--)
			{
				shared_ptr<Window> window = windowStack[i];
				
				#ifdef DEEP_GUI
					float x = (m.x-window->current_center.x)*window->current_center.shellRadius+window->fsize.x/2;
					float y = (m.y-window->current_center.y)*window->current_center.shellRadius+window->fsize.y/2;
				#else
					float x = (m.x-window->x)+window->fsize.x/2;
					float y = (m.y-window->y)+window->fsize.y/2;
				#endif
				if (!window->GetCellLayout().border.contains(x,y))
					continue;
				
				if (window->rootComponent && window->rootComponent->IsVisible() && window->rootComponent->IsEnabled())
				{
					bool IsEnabled = true;
					PComponent component = window->rootComponent->GetComponent(x,y,Component::HoverRequest,IsEnabled);
					if (component && IsEnabled)
					{
						Component::eEventResult rs = component->OnMouseWheel(x,y,delta);
						window->Apply(rs);
						if (rs != Component::Unsupported)
							MouseHover(window,component,x,y);
					}
				}
				if (i+1 != windowStack.count())
				{
					windowStack.last()->onFocusLost();
					windowStack.erase(i);
					Component::ResetFocused();
					windowStack << window;
					window->onFocusGained();
				}
				#ifdef DEEP_GUI
					for (unsigned j = 0; j < windowStack.count(); j++)
						windowStack[j]->setShellDestination(radiusOf(j));
				#endif
				return true;
			}
			return false;
		}
		
		void			Operator::mouseUp()
		{
			if (!owns_mouse_down)
				return;
			handlingEvent = true;
			if (clicked)
			{
				TVec2<float> m;
				unprojectMouse(m);
				clicked->GetWindow()->SignalMouseUp(m.x,m.y);
			}
			clicked.reset();
			handlingEvent = false;
		}
		

		
		shared_ptr<Window>		Operator::ShowNewWindow(const NewWindowConfig&config, const shared_ptr<Component>&component /*=shared_ptr<Component>()*/)
		{
			return ShowNewWindow(config,&Window::commonStyle,component);
		}
		
		shared_ptr<Window>		Operator::ShowNewWindow(const NewWindowConfig&config,Layout*layout,const shared_ptr<Component>&component)
		{
			shared_ptr<Window> result(Window::CreateNew(config,layout,component));
			ShowWindow(result);
			return result;
		}
		
		void Operator::bind(Key::Name key)
		{
			input->bind(key,std::bind(Component::SignalKeyDown,key),std::bind(Component::SignalKeyUp,key));
			input->bindCtrl(key,std::bind(Component::SignalKeyDown,key));
		}

		
		/*static*/	shared_ptr<Operator>					Operator::create(Display<OpenGL>&display, const Mouse&mouse, InputMap&input, mode_t mode /*=Cylindrical*/)
		{
			shared_ptr<Operator> rs = shared_ptr<Operator>(new Operator(display,mouse,input,mode));
			input.pushProfile();
			input.bindProfile(my_profile);
				for (Key::Name k = (Key::Name)0; k <= Key::Max; k = (Key::Name)(k+1))
					rs->bind(k);
				//input.cascadeKeys();
				//for (Key::Name k = (Key::Name)0; k <= Key::Max; k = (Key::Name)(k+1))
				//{
				//	if ((k < Key::A || k > Key::Z) && (k < Key::N0 || k > Key::N9) && k != Key::SZ && k != Key::AE && k != Key::UE && k != Key::OE && k != Key::Space && k != Key::Period && k != Key::Comma && k != Key::Minus && k != Key::Mesh)
				//		input.cascade(k);
				//}
	
				//rs->bind(Key::Left);
				//rs->bind(Key::Right);
				//rs->bind(Key::Up);
				//rs->bind(Key::Down);
				//rs->bind(Key::Return);
				//rs->bind(Key::Enter);
				//rs->bind(Key::Backspace);
				//rs->bind(Key::Delete);
				//rs->bind(Key::Home);
				//rs->bind(Key::End);
				//rs->bind(Key::Tab);
				//rs->bind(Key::C);
				//rs->bind(Key::X);
				//rs->bind(Key::V);
				//rs->bind(Key::Tab);
			input.popProfile();
		

			bool aspect_was_in_projection = GlobalAspectConfiguration::loadAsProjection;
			GlobalAspectConfiguration::loadAsProjection = false;
			bool world_z_was_up = GlobalAspectConfiguration::worldZIsUp;
			GlobalAspectConfiguration::worldZIsUp = false;

			rs->updateDisplaySize();
			rs->projected_space.UpdateProjection(1.0f,0.1,100,92);
			//projected_space.retraction[2] = -1;
			rs->projected_space.UpdateView();
			rs->projected_space.depthTest = NoDepthTest;

			GlobalAspectConfiguration::loadAsProjection = aspect_was_in_projection;
			GlobalAspectConfiguration::worldZIsUp = world_z_was_up;
		
			GUI::Initialize();
			return rs;
		}

		void	Operator::updateDisplaySize()
		{
			float	w = display->clientWidth(),
					h = display->clientHeight();
			window_space.UpdateProjection(0,0,
								w,h,
								-1,
								1);
			if (!layer_texture)
				glGenTextures(1,&layer_texture);

			glBindTexture(GL_TEXTURE_2D,layer_texture);
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,display->clientWidth(),display->clientHeight(),0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
			glBindTexture(GL_TEXTURE_2D,0);

			float	min_x = -w/2,
					max_x = w/2,
					min_y = -h/2,
					max_y = h/2;
			for (index_t i = 0; i < windowStack.size(); i++)
			{
				windowStack[i]->x = clamped(windowStack[i]->x,min_x,max_x);
				windowStack[i]->y = clamped(windowStack[i]->y,min_y,max_y-windowStack[i]->fsize.y/2.f);
			}
		}
	
	
		void	setBlurEffect(bool setting)
		{
			settingChanged |= blurEffect != setting;
			blurEffect = setting;
		}
			
		void	setRefractEffect(bool setting)
		{
			settingChanged |= refractEffect != setting;
			refractEffect = setting;
		}
			
		void	setBumpEffect(bool setting)
		{
			settingChanged |= bumpEffect != setting;
			bumpEffect = setting;
		}
	
	
		
		
		
	}
}
