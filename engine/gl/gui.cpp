#include "../../global_root.h"
#include "gui.h"

namespace Engine
{
	namespace GUI
	{

		#ifdef DEEP_GUI
			MAKE_SORT_CLASS(DepthSort,current_center.shell_radius);
		#endif
		
		static InputProfile	my_profile(true,false);
		

		Operator::Operator(Display<OpenGL>&display_, const Mouse&mouse_, InputMap&input_, mode_t mode_):owns_mouse_down(false),display(&display_),mouse(&mouse_),input(&input_),mode(mode_),created(false),stack_changed(false),layer_texture(0)
		{}

		namespace
		{

			bool 							setting_changed(true),
											blur_effect(true),
											refract_effect(true),
											bump_effect(true),
											handling_event(false);
			Component::TExtEventResult		ext;


			float							time_since_last_tick=0;

			shared_ptr<Component>			clicked,//component that caught the mouse down event
											focused,//last clicked component. unlike 'clicked' this variable will not be set to NULL if the mouse button is released
											hovered;//last hovered component
		
		
			TVec2<float>					last;
			shared_ptr<Window>				dragging;
			Window::ClickResult::value_t		drag_type = Window::ClickResult::Missed;
		

			GLShader::Template				window_shader;
			GLShader::Instance				normal_shader,
											*current_window_shader(NULL);
			index_t							blur_effect_switch,
											refract_effect_switch,
											bump_effect_switch;

		


			void							initialize(bool force = false)
			{
				static bool initialized = false;
				if (initialized || force)
					return;
				initialized = true;

				if (!gl_extensions.init(EXT_SHADER_OBJECTS_BIT))
					FATAL__("Failed to initialized shader objects");

				window_shader.clear();
				if (!window_shader.loadComposition(
					"[vertex]\n"
					"varying vec2 eye;\n"
					"varying mat3 tangent_space;\n"
					"void main()\n"
					"{\n"
						"gl_Position = ftransform();\n"
						"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
						"eye = gl_Position.xy/gl_Position.w/2.0+0.5;\n"
						"vec3	tangent = normalize(gl_NormalMatrix*gl_MultiTexCoord1.xyz),\n"
						"		normal = normalize(gl_NormalMatrix*gl_Normal);\n"
						"gl_FrontColor = gl_Color;\n"
						"#if bump_mapping||refract\n"
							"tangent_space[0] = -normalize(cross(normal,tangent));\n"
							"tangent_space[1] = cross(normal,tangent_space[0]);\n"
							"tangent_space[2] = normal;\n"
						"#endif\n"
					"}\n"
					"[fragment]\n"
					"uniform sampler2D background;\n"
					"uniform sampler2D normal_map;\n"
					"uniform sampler2D color_map;\n"
					//"uniform sampler2D sky_map;\n"
					//"const vec3 light = {0.57735026918962576450914878050196,0.57735026918962576450914878050196,0.57735026918962576450914878050196};\n"
					//"const vec3 light = {0,0.70710678118654752440084436210485,0.70710678118654752440084436210485};\n"
					"const vec3 light = vec3(0,0.70710678118654752440084436210485,0.70710678118654752440084436210485);\n"
					"varying vec2 eye;\n"
					"varying mat3 tangent_space;\n"
					"vec3 toneBackground(vec3 color)\n"
					"{\n"
						"float brightness = dot(color,vec3(0.35,0.5,0.15));\n"
						"return color / (1.0 + pow(brightness,4.0)*0.35);\n"
					"}\n"
					"void main()\n"
					"{\n"
						"vec3 color = vec3(0.0);\n"
						"#if bump_mapping||refract||blur\n"	//need normal alpha for blur
							"vec4 normal = texture2D(normal_map,gl_TexCoord[0].xy);\n"
							"#if bump_mapping||refract\n"
								"if (normal.a == 0.0)\n"
									"normal.xyz = tangent_space[2];\n"
								"else\n"
									"normal.xyz = tangent_space*(normal.xyz*2.0-1.0);\n"
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
						"#if blur\n"
							"for (int x = -2; x <= 2; x++)\n"
								"for (int y = -2; y <= 2; y++)\n"
								"{\n"
									"vec2 coord = (sample+vec2(float(x)*1.3/1280.0,float(y)*1.3/1024.0f));\n"
									"color += texture2D(background,coord).rgb;\n"
								"}\n"
								"color /= 25.0;\n"
								"color = toneBackground(color);\n"
							"#if bump_mapping\n"
								"float intensity = pow(max(dot(reflected,light),0.0),7.0)*0.5*gl_Color.a+0.05;\n"
								"gl_FragColor.rgb = mix(color+intensity,color_sample.rgb*(1.0+intensity),color_sample.a+(1.0-color_sample.a)*(1.0-normal.a));\n"
								//"gl_FragColor.rgb = mix(color.rgb/25.0+intensity,color_sample.rgb*(1.0+intensity),1.0);\n"
							"#else\n"
								"gl_FragColor.rgb = mix(color,color_sample.rgb,color_sample.a+(1.0-color_sample.a)*(1.0-normal.a));\n"
							"#endif\n"
							"gl_FragColor.a = color_sample.a+(1.0-color_sample.a)*normal.a;\n"
						"#elif refract\n"
							"color = texture2D(background,sample).rgb;\n"
							"color = toneBackground(color);\n"
							"#if bump_mapping\n"
								"float intensity = pow(max(dot(reflected,light),0.0),7.0)*0.5*gl_Color.a+0.05;\n"
								"gl_FragColor.xyz = mix(color+intensity,color_sample.rgb*(1.0+intensity),color_sample.a+(1.0-color_sample.a)*(1.0-normal.a));\n"
							"#else\n"
								"gl_FragColor.xyz = mix(color,color_sample.rgb,color_sample.a+(1.0-color_sample.a)*(1.0-normal.a));\n"
							"#endif\n"
							"gl_FragColor.a = color_sample.a+(1.0-color_sample.a)*normal.a;\n"
						"#else\n"
							"#if bump_mapping\n"
								"float intensity = pow(max(dot(reflected,light),0.0),7.0)*0.5*gl_Color.a+0.05;\n"
								"gl_FragColor.xyz = clamp(color_sample.rgb+intensity,0.0,1.0);\n"
								"gl_FragColor.a = color_sample.a+intensity*normal.a;\n"
							"#else\n"
								"gl_FragColor = color_sample;\n"
							"#endif\n"
						"#endif\n"
						"gl_FragColor.a *= 0.75+0.25*gl_Color.a;\n"
						//"gl_FragColor = vec4(0.0,1.0,0.0,1.0);\n"

						//"gl_FragColor.a *= 0.9+0.1*gl_Color.a;\n"
						/*"gl_FragColor.xy = sample;\n"
						"gl_FragColor.z = 0;\n"
						"gl_FragColor.a = 1;\n"*/
						//"gl_FragColor.xyz = normal.xyz*0.5+0.5;\n"
					"}\n"
				))
				{
					FATAL__("GUI Window shader compilation exception:\n"+window_shader.report());
				}
				else
				{
					blur_effect_switch = window_shader.map()->locate("blur");
					refract_effect_switch = window_shader.map()->locate("refract");
					bump_effect_switch = window_shader.map()->locate("bump_mapping");
					window_shader.predefineUniformi("normal_map",1);
					window_shader.predefineUniformi("color_map",2);
				}
			
				normal_shader.clear();
				if (!normal_shader.loadComposition(
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
					FATAL__("GUI Normal shader compilation exception:\n"+normal_shader.report());
				}
			
				/*Image	image;
				if (Magic::loadFromFile(image,"sky.png"))
					sky_texture = display->textureObject(image);*/
	 
				//window_shader.suppressWarnings();
				//window_shader.locate("sky_map").seti(3);
			
			
			
				glGetError();
			


			}
		}

			Textout<GLTextureFont2>			Component::textout;




		Layout								Window::common_style,
											Window::menu_style,
											Window::hint_style;

		//static Log							logfile("gui.log",true);

		
		static Keyboard::charReader		old_reader=NULL;
		static	bool						old_read = false;
		
		//Display<OpenGL>					*display(NULL);
		
		/*TCell::~TCell()
		{
			if (display)
			{
				display->discardObject(normal_texture);
				display->discardObject(color_texture);
			}
		}*/

		TCell&	TCell::operator=(const TCell&other)
		{
			return *this;
		}

		void						Operator::showMenu(const shared_ptr<Window>&menu_window)
		{
			/*			result->x = region.x.center()-display->clientWidth()/2;
				result->y = region.y.center()-display->clientHeight()/2;*/

			float	w = display->clientWidth()/2,
					h = display->clientHeight()/2;
			if (menu_window->x - menu_window->fwidth/2 < -w)
				menu_window->x = -w+menu_window->fwidth/2;
			if (menu_window->x + menu_window->fwidth/2 > w)
				menu_window->x = w-menu_window->fwidth/2;
			
			if (menu_window->y - menu_window->fheight/2 < -h)
				menu_window->y = -h+menu_window->fheight/2;
			if (menu_window->y + menu_window->fheight/2 > h)
				menu_window->y = h-menu_window->fheight/2;
			
			menu_stack << menu_window;
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
		
		void		TFreeCell::paintColor(Display<OpenGL>&display,const TFreeCell&cell)
		{
			if (cell.color->isEmpty())	//NULL-pointer sensitive
				return;
			display.useTexture(cell.color,true);
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
		}
		
		void		TFreeCell::paintNormal(Display<OpenGL>&display,const TFreeCell&cell, bool pressed)
		{
			if (cell.normal->isEmpty())	//NULL-pointer sensitive
				return;
			display.useTexture(cell.normal,true);
			glMatrixMode(GL_TEXTURE);
				glPushMatrix();
					if (pressed)
						glScalef(-1,-1,1);
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
					glRotatef(-90,0,0,1);
					glBegin(GL_QUADS);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
				case 2:
					glRotatef(-180,0,0,1);
					glBegin(GL_QUADS);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
				case 3:
					glRotatef(-270,0,0,1);
					glBegin(GL_QUADS);
						glTexCoord2f(0,1); glVertex2f(cell.region.x.min,cell.region.y.min);
						glTexCoord2f(0,0); glVertex2f(cell.region.x.max,cell.region.y.min);
						glTexCoord2f(1,0); glVertex2f(cell.region.x.max,cell.region.y.max);
						glTexCoord2f(1,1); glVertex2f(cell.region.x.min,cell.region.y.max);
					glEnd();
				break;
			}
				glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			
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
				Magic::loadFromFile(image,file.getLocation());
				out.color.load(image,global_anisotropy,true);
				out.width = image.width();
				out.height = image.height();
			}
			if (folder.findFile("bump/"+attrib,file))
			{
				loadBump(file.getLocation(),image);

				if (out.width != 0 && (out.width != image.width() || out.height != image.height()))
				{
					throw IO::StructureCompositionFault("Trying to match bump texture of different size (color image was "+String(out.width)+"*"+String(out.height)+", bump texture is "+image.toString()+")");
				}
				out.normal.load(image,global_anisotropy,true);
				out.width = image.width();
				out.height = image.height();
			}
			out.width *= scale;
			out.height *= scale;
		}
		
		void Component::charRead(char c)
		{
			eEventResult rs = Unsupported;
			if (focused)
			{
				rs = focused->onChar(c);
				focused->window()->apply(rs);
			}
			if (rs == Unsupported && old_read)
				old_reader(c);
		}

		void					Component::signalLayoutChange()	const
		{
			shared_ptr<Window>	wnd = window();
			if (wnd)
				wnd->layout_changed = wnd->visual_changed = true;
		}
		void					Component::signalVisualChange()	const
		{
			shared_ptr<Window>	wnd = window();
			if (wnd)
				wnd->visual_changed = true;
		}


		/*virtual*/	void					Component::setEnabled(bool enabled)
		{
			if (this->enabled != enabled)
			{
				this->enabled = enabled;
				signalVisualChange();
			}
		}
	
		/*virtual*/	void					Component::setVisible(bool visible)
		{
			if (this->visible != visible)
			{
				this->visible = visible;
				signalVisualChange();
			}
		}
	
		shared_ptr<Operator>	Component::getOperator() const
		{
			if (window_link.expired())
				return shared_ptr<Operator>();
			return window_link.lock()->operator_link.lock();
		}

		shared_ptr<Operator>	Component::requireOperator() const
		{
			if (window_link.expired())
			{
				FATAL__("Operator required! Window link not set to component of type "+type_name);
				return shared_ptr<Operator>();
			}
			shared_ptr<Operator> rs = window_link.lock()->operator_link.lock();
			if (!rs)
				FATAL__("Operator required! Window link set but operator link not set");
			return rs;
		}

		/*virtual*/ void			Component::setWindow(const weak_ptr<Window>&wnd)
		{
			window_link = wnd;

			for (index_t i = 0; i < countChildren(); i++)
				child(i)->setWindow(wnd);
		}
		
		void Component::resetFocused()
		{
			setFocused(shared_ptr<Component>());
		}

		void Component::setFocused(const shared_ptr<Component>&component)
		{
			if (focused == component)
				return;
			time_since_last_tick = 0;
			if (focused)
			{
				eEventResult result = focused->onFocusLost();
				shared_ptr<Window> wnd = focused->window();
				if (wnd)
					wnd->apply(result);
				if (!component)
				{
					input.popProfile();
					keyboard.reader = old_reader;
					keyboard.read = old_read;
				}
			}
			elif (component)
			{
				old_reader = keyboard.reader;
				old_read = keyboard.read;
				keyboard.reader = charRead;
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
				focused->window()->apply(focused->onFocusGained());
			}
		}


		void Operator::cylindricVertex(float px, float py, float tx, float ty, float r/* =1*/) const
		{
			float	radial = px/3,
					h = sqrt(r*r-sqr(radial));
				glNormal3f(-radial/r,0,h/r); 
				glTangent3f(0,1,0);
				glTexCoord2f(tx,ty);
			glVertex3f(px,py,-h);
		}
		
		void Operator::cylindricVertex(Window*window, float x, float y)	const
		{
			#ifdef DEEP_GUI
				float	px = (window->current_center.x*window->current_center.shell_radius+window->fwidth*x/2)/display->clientWidth()*2,
						py = (window->current_center.y*window->current_center.shell_radius+window->fheight*y/2)/display->clientHeight()*2;
				project(px,py,(x*0.5+0.5)*window->usage_x,(y*0.5+0.5)*window->usage_y,window->current_center.shell_radius);
			#else
				float	px = (window->x+window->fwidth*x/2)/display->clientWidth()*2,
						py = (window->y+window->fheight*y/2)/display->clientHeight()*2;
				cylindricVertex(px,py,(x*0.5+0.5)*window->usage_x,(y*0.5+0.5)*window->usage_y);
			#endif
		}


		void Operator::planarVertex(float px, float py, float tx, float ty) const
		{
				glTexCoord2f(tx,ty);
			glVertex3f(px,py,0);
		}
		
		void Operator::planarVertex(Window*window, float x, float y)	const
		{
			float	px = (window->x+window->fwidth*x/2)/display->clientWidth()*2,
					py = (window->y+window->fheight*y/2)/display->clientHeight()*2;
			planarVertex(px,py,(x*0.5+0.5)*window->usage_x,(y*0.5+0.5)*window->usage_y);
		}
		
		
		void Operator::project(Window*window, float x, float y, TVec2<float>&p)	const
		{
			/*cout << "x="<<x<<endl;
			cout << "y="<<y<<endl;
			cout << "shell.r="<<window->current_center.shell_radius<<endl;
			cout << "shell.x="<<window->current_center.x<<endl;
			cout << "shell.y="<<window->current_center.y<<endl;
			cout << "fwidth="<<window->fwidth<<endl;
			cout << "fheight="<<window->fheight<<endl;
			cout << "client width="<<display->clientWidth()<<endl;
			cout << "client height="<<display->clientHeight()<<endl;*/
			#ifdef DEEP_GUI
				float	r = window->current_center.shell_radius,
						px = (window->current_center.x*r+window->fwidth*x/2)/display->clientWidth()*2,
						py = (window->current_center.y*r+window->fheight*y/2)/display->clientHeight()*2,
						radial = px/3,
						h = sqrt(r*r-sqr(radial));
			#else
				float	px = (window->x+window->fwidth*x/2)/display->clientWidth()*2,
						py = (window->y+window->fheight*y/2)/display->clientHeight()*2,
						radial = px/3,
						h = sqrt(1.0f-sqr(radial));
			#endif
			
			/*cout << "radial="<<radial<<endl;
			cout << "equation=sqrt("<<r<<"*"<<r<<"-sqr("<<radial<<"))"<<endl;*/
			TVec3<float>	mp = {px,py,-h};
			//cout << "mp="<<_toString(mp)<<endl;
			projected_space.pointToScreen(mp,p);
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
			projected_space.screenToVector(mouse->location.fx*2-1,mouse->location.fy*2-1,Vector<>::dummy,f);
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

		
		Layout::Layout():min_width(1),min_height(1),variable_rows(1),cell_count(0),override(NULL)
		{}
		
	
		void	Layout::loadFromFile(const String&filename, float scale)
		{
			rows.free();
			
			
			FileSystem::Folder	folder(FileSystem::extractFilePath(filename));
			
			title_position.setAll(0);
			
			List::Vector<TIORow>	iorows;
			
			XML::Container	xml;
			xml.loadFromFile(filename);
				
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

				if (!convert(segments[0].c_str(),title_position.x.min)
					||
					!convert(segments[1].c_str(),title_position.y.min)
					||
					!convert(segments[2].c_str(),title_position.x.max)
					||
					!convert(segments[3].c_str(),title_position.y.max))
				{
					throw IO::DriveAccess::FileFormatFault(globalString("One or more segments of 'title_position' attribute of XML 'layout' node could not be converted to float"));
				}
				title_position *= scale;
			}
			{
				if (!xlayout->query("border_edge",attrib))
					throw IO::DriveAccess::FileFormatFault(globalString("XML node 'layout' lacks 'border_edge' attribute"));
					
				Array<String>	segments;
				explode(',',attrib,segments);
				if (segments.count() != 4)
					throw IO::DriveAccess::FileFormatFault(globalString("'border_edge' attribute of XML 'layout' node does not contain 4 comma-separated segments"));

				if (!convert(segments[0].c_str(),border_edge.x.min)
					||
					!convert(segments[1].c_str(),border_edge.y.min)
					||
					!convert(segments[2].c_str(),border_edge.x.max)
					||
					!convert(segments[3].c_str(),border_edge.y.max))
				{
					throw IO::DriveAccess::FileFormatFault(globalString("One or more segments of 'border_edge' attribute of XML 'layout' node could not be converted to float"));
				}
				border_edge *= scale;
			}
			{
				if (!xlayout->query("client_edge",attrib))
					throw IO::DriveAccess::FileFormatFault(globalString("XML node 'layout' lacks 'client_edge' attribute"));

				Array<String>	segments;
				explode(',',attrib,segments);
				if (segments.count() != 4)
					throw IO::DriveAccess::FileFormatFault(globalString("'client_edge' attribute of XML 'layout' node does not contain 4 comma-separated segments"));

				if (!convert(segments[0].c_str(),client_edge.x.min)
					||
					!convert(segments[1].c_str(),client_edge.y.min)
					||
					!convert(segments[2].c_str(),client_edge.x.max)
					||
					!convert(segments[3].c_str(),client_edge.y.max))
				{
					throw IO::DriveAccess::FileFormatFault(globalString("One or more segments of 'client_edge' attribute of XML 'layout' node could not be converted to float"));
				}
				client_edge *= scale;
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
							cell->variable_width = attrib == "stretch";
							
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
			
			min_width = 0;
			min_height = 0;
			variable_rows = 0;
			cell_count = 0;
			rows.setSize(iorows);
			for (unsigned i = 0; i < iorows; i++)
			{
				TRow&row = rows[i];
				row.cells.setSize(iorows[i]->cells);
				row.variable_height = iorows[i]->variable_height;
				row.height = 0;
				row.variable_cells = 0;
				row.fixed_width = 0;
				
				cell_count += row.cells.count();
				
				for (unsigned j = 0; j < row.cells.count(); j++)
				{
					TIOCell*icell = iorows[i]->cells[j];
					TCell&cell = row.cells[j];
					cell.width = icell->normal.width()*scale;
					cell.variable_width = icell->variable_width;
					
					cell.normal_texture.load(icell->normal,global_anisotropy,true,TextureFilter::Trilinear,false);
					cell.color_texture.load(icell->color,global_anisotropy,true,TextureFilter::Trilinear,false);
					if (icell->normal.height()*scale>rows[i].height)
						row.height = icell->normal.height()*scale;
					if (cell.variable_width)
						row.variable_cells++;
					else
						row.fixed_width += cell.width;
				}
				if (row.fixed_width > min_width)
					min_width = row.fixed_width;
				if (!row.variable_height)
					min_height += row.height;
				else
					variable_rows ++;
				if (!row.variable_cells)
					row.variable_cells++;
			}
			if (!variable_rows)
				variable_rows++;
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
		
		void	Layout::updateCells(const Rect<float>&window_location, TCellLayout&layout)	const
		{
			layout.cells.setSize(cell_count);
			
			//applyArea(layout.body,window_location,body_location);
			//applyArea(layout.title,window_location,title_location);
			float	width = window_location.width(),
				height = window_location.height();

			float variable_height = (height-min_height)/variable_rows;
			if (variable_height < 0)
				variable_height = 0;
			
			float y = window_location.y.max;
			
			unsigned cell_index = 0;
			
			layout.title.x.min = title_position.x.min>=0?window_location.x.min + title_position.x.min:window_location.x.max+title_position.x.min;
			layout.title.x.max = title_position.x.max>=0?window_location.x.min + title_position.x.max:window_location.x.max+title_position.x.max;
			
			layout.title.y.min = title_position.y.min>=0?window_location.y.min + title_position.y.min:window_location.y.max+title_position.y.min;
			layout.title.y.max = title_position.y.max>=0?window_location.y.min + title_position.y.max:window_location.y.max+title_position.y.max;
			
			
			
			for (unsigned i = 0; i < rows.count(); i++)
			{
				const TRow&row = rows[i];
				
				
				float row_height = row.variable_height?variable_height:row.height;
				
				float variable_width = (width-row.fixed_width)/row.variable_cells;
				if (variable_width < 0)
					variable_width = 0;
				float x = window_location.x.min;
				for (unsigned j = 0; j < row.cells.count(); j++)
				{

					const TCell&cell = row.cells[j];
					float cell_width = cell.variable_width?variable_width:cell.width;
					
					ASSERT2__(cell_index < cell_count,cell_index,cell_count);
					TCellInstance&instance = layout.cells[cell_index++];
					
					instance.region.x.min = x;
					instance.region.x.max = x+cell_width;
					instance.region.y.max = y;
					instance.region.y.min = y-row_height;
					//instance.width = cell.width;
					instance.color_texture = &cell.color_texture;
					instance.normal_texture = &cell.normal_texture;
					x+= cell_width;
					
					
				}
				y-=row_height;
			}
			ASSERT_EQUAL__(cell_index,layout.cells.count());
			layout.client.x.min = /* floor */round(window_location.x.min+client_edge.x.min);
			layout.client.x.max = /* ceil */round(window_location.x.max-client_edge.x.max);
			layout.client.y.min = /* floor */round(window_location.y.min+client_edge.y.min);
			layout.client.y.max = /* ceil */round(window_location.y.max-client_edge.y.max);
			layout.border.x.min = window_location.x.min+border_edge.x.min;
			layout.border.x.max = window_location.x.max-border_edge.x.max;
			layout.border.y.min = window_location.y.min+border_edge.y.min;
			layout.border.y.max = window_location.y.max-border_edge.y.max;
		}
		


		
		
		
		
		
		bool	Window::remove()
		{
			shared_ptr<Operator>	op = operator_link.lock();
			if (op)
			{
				return op->removeWindow(shared_from_this());
			}
			return false;
		}


		Window::Window(bool modal, Layout*style):is_modal(modal),exp_x(0),exp_y(0),iwidth(1),iheight(1),progress(0),fwidth(1),fheight(1),usage_x(0),usage_y(0),layout(style),title(""),size_changed(true),layout_changed(true),visual_changed(true),fixed_position(false),fixed_size(false),hidden(timer.now())
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
				ASSERT_IS_CONSTRAINED__(window->current_center.shell_radius,-100000,100000);
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
			
			if (window == window_stack.last())	//top most window also copies for menu windows, which override the parent window rather than blurring over it
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
			
			if ((blur_effect || refract_effect) && !is_menu)	//menu reuses the copy before the top most non-menu window
			{
				glGetError();
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, (int)rect.x.min, (int)rect.y.min, (int)rect.x.min, (int)rect.y.min,  (int)rect.width(), (int)rect.height());
				if (glGetError())
				{
					//FATAL__("Failed to copy "+rect.toString());
					return;
				}
			}
			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,window->normal_buffer.color_target[0].texture_handle);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,window->color_buffer.color_target[0].texture_handle);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			glActiveTexture(GL_TEXTURE0);
			
			if (is_menu)
				glWhite();
			else
				glBlack(window_stack.last()==window);


			//ASSERT__(current_window_shader->isInstalled());
			glThrowError();

			switch (mode)
			{
				case Cylindrical:
				{
					//float r = 1.0f+((float)window_stack.count()-window->has_depth)*0.1;
					#ifdef DEEP_GUI
						unsigned res = (unsigned)((float)window->iwidth/window->current_center.shell_radius/50);
					#else
						unsigned res = (unsigned)((float)window->iwidth/50);
					#endif
				
					//ShowMessage(String(fwidth)+" => "+String(iwidth)+" => "+String(res));
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
					//float r = 1.0f+((float)window_stack.count()-window->has_depth)*0.1;
					#ifdef DEEP_GUI
						unsigned res = (unsigned)((float)window->iwidth/window->current_center.shell_radius/50);
					#else
						unsigned res = (unsigned)((float)window->iwidth/50);
					#endif
				
					//ShowMessage(String(fwidth)+" => "+String(iwidth)+" => "+String(res));
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
		
		
		void	Window::setComponent(const shared_ptr<Component>&component)
		{
			component_link = component;
			if (component)
				component->setWindow(shared_from_this());
			layout_changed = visual_changed = true;
		}
		
		void	Window::drag(const TVec2<float>&d)
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
		
		void	Window::dragResize(TVec2<float>&d,ClickResult::value_t click_result)
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
			
			float	mw = minWidth(),
					mh = minHeight();
			if (fwidth + dx < mw && dx != 0)
			{
				d.x *= (mw-fwidth)/dx;
				dx = mw-fwidth;
			}
			if (fheight + dy < mh && dy != 0)
			{
				d.y *= (mh-fheight)/dy;
				dy = mh-fheight;
			}
			
			if (d.x)
				fwidth  += dx;
			if (d.y)
				fheight += dy;
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
			size_changed = layout_changed = visual_changed = true;
			iwidth = (unsigned)round(fwidth);
			iheight = (unsigned)round(fheight);
			//updateLayout();
		}
		
		
		void	Window::updateLayout()
		{
			Rect<float>	reg(0,0,/*ceil*/(iwidth),/*ceil*/(iheight));
			if (layout)
				layout->updateCells(reg,cell_layout);
			else
				cell_layout.client = cell_layout.border = reg;
			const Rect<float>&client = cell_layout.client;
			if (component_link)
				component_link->updateLayout(client);
			layout_changed = false;			
		}
		
		float	Window::minWidth()	const
		{
			float rs=0;
			if (component_link)
				rs = component_link->minWidth(true);
			if (layout)
			{
				rs += layout->client_edge.x.min+layout->client_edge.x.max;
				if (rs < layout->min_width)
					rs = layout->min_width;
			}
			return rs;
		}
		
		void	Window::setSize(float width, float height)
		{
			fwidth = width;
			iwidth = (unsigned)round(fwidth);
			fheight = height;
			iheight = (unsigned)round(fheight);
			size_changed = layout_changed = visual_changed = true;
		}
		
		void	Window::setHeight(float height)
		{
			fheight = height;
			iheight = (unsigned)round(fheight);
			size_changed = layout_changed = visual_changed = true;
		}
		
		void	Window::setWidth(float width)
		{
			fwidth = width;
			iwidth = (unsigned)round(fwidth);
			size_changed = layout_changed = visual_changed = true;
		}
		
		float	Window::minHeight()	const
		{
			float rs=0;
			if (component_link)
				rs = component_link->minHeight(true);
			if (layout)
			{
				rs += layout->client_edge.y.min+layout->client_edge.y.max;
				if (rs < layout->min_height)
					rs = layout->min_height;
			}
			return rs;
		}
		
		#ifdef DEEP_GUI
			void	Window::setShellDestination(float new_shell_radius)
			{
				if (destination.shell_radius == new_shell_radius)
					return;
				origin = current_center;
				destination.shell_radius = new_shell_radius;
				progress = 0;
			}
		#endif
		
		
		void			Window::mouseUp(float x, float y)
		{
			#ifdef DEEP_GUI
				x = (x-current_center.x)*current_center.shell_radius;
				y = (y-current_center.y)*current_center.shell_radius;
			#else
				x = (x-this->x);
				y = (y-this->y);
			#endif
			
			x += fwidth*0.5;
			y += fheight*0.5;
			apply(clicked->onMouseUp(x,y));
		}
		

		Window::ClickResult::value_t			Window::resolve(float x_, float y_, float&inner_x, float&inner_y)
		{
			#ifdef DEEP_GUI
				float x = (x_-current_center.x)*current_center.shell_radius+fwidth/2;
				float y = (y_-current_center.y)*current_center.shell_radius+fheight/2;
			#else
				float x = (x_-this->x)+fwidth/2;
				float y = (y_-this->y)+fheight/2;
			#endif
			inner_x = x;
			inner_y = y;
			
			if (!cell_layout.border.contains(x,y))
				return ClickResult::Missed;
			
			if (!fixed_size && !fixed_position)
			{
				if (x > cell_layout.border.x.max-BorderWidth)
				{
					if (y > cell_layout.border.y.max-BorderWidth)
						return ClickResult::ResizeTopRight;
					if (y < cell_layout.border.y.min+BorderWidth)
						return ClickResult::ResizeBottomRight;
					return ClickResult::ResizeRight;
				}
				if (x < cell_layout.border.x.min+BorderWidth)
				{
					if (y > cell_layout.border.y.max-BorderWidth)
						return ClickResult::ResizeTopLeft;
					if (y < cell_layout.border.y.min+BorderWidth)
						return ClickResult::ResizeBottomLeft;
					return ClickResult::ResizeLeft;
				}
				
				if (y > cell_layout.border.y.max-BorderWidth)
					return ClickResult::ResizeTop;
				if (y < cell_layout.border.y.min+BorderWidth)
					return ClickResult::ResizeBottom;
			}
				
			if (component_link && (!layout || cell_layout.client.contains(x,y))&&component_link->visible&&component_link->enabled&&component_link->cell_layout.border.contains(x,y))
				return ClickResult::Component;
			if (fixed_position)
				return ClickResult::Ignored;
			return ClickResult::DragWindow;
		}

		
		void	Window::apply(Component::eEventResult rs)
		{
			switch (rs)
			{
				case Component::RequestingReshape:
					layout_changed = true;
				case Component::RequestingRepaint:
					visual_changed = true;
				break;
			}
		}

		
		
		float Operator::radiusOf(index_t stack_layer)	const
		{
			return 0.9+0.1*(window_stack.count()-stack_layer);
		}
		
		
		void		Operator::apply(const Rect<int>&port)
		{
			glViewport(port.x.min,port.y.min,port.width(),port.height());
			texture_space.make(port.x.min,port.y.min,
								port.x.max,port.y.max,
								-1,1);
			display->pick(texture_space);
		}
		
		void		Operator::focus(const Rect<float>&region)	//!< Focuses on an area by applying the current viewport and translation to the specified region and further limiting the viewport. The existing translation will be modified by dx and dy
		{
			Rect<int>&next = focus_stack.append();
			//next = region;
			next.x.min = (int)floor(region.x.min);
			next.y.min = (int)floor(region.y.min);
			next.x.max = (int)ceil(region.x.max);
			next.y.max = (int)ceil(region.y.max);
			if (focus_stack.count() > 1)
			{
				const Rect<int>&prev = focus_stack[focus_stack.count()-2];
				next.constrainBy(prev);
			}
			apply(next);
		}
		
		void		Operator::unfocus()	//!< Reverts the focus process by jumping back to the next upper focus
		{
			ASSERT__(focus_stack.length()>0);
			focus_stack.pop();
			apply(focus_stack.last());
		}
		
		/*
		static inline Viewport	viewport(const Viewport&port)
		{
			static Viewport	current(Rect<float>(0,0,1,1));
			Viewport rs = current;
			current = port;
			int l = (int)floor(port.x.min),
				b = (int)floor(port.y.min),
				r = (int)ceil(port.x.max),
				t = (int)ceil(port.y.max);
			
			glViewport(l+(int)port.absolute_dx,b+(int)port.absolute_dy,r-l,t-b);
			texture_space.make(l,b,
								r,t,
								-1,1);
			display->pick(texture_space);
			glTranslatef(port.relative_dx,port.relative_dy,0);
			
			return rs;
		}
		*/
		

		Component::Component(const String&type_name_):current_region(0,0,1,1),offset(0,0,0,0),anchored(false,false,false,false),width(1),height(1),type_name(type_name_),layout(NULL),tick_interval(0.2),enabled(true),visible(true)
		{}
		
		void	Component::keyDown(int key)
		{
			//cout << "caught "<<key<<endl;
			eEventResult rs = Unsupported;
			if (focused)
			{
				rs = focused->onKeyDown((Key::Name)key);
				if (focused && !focused->window_link.expired())	//key down may do all sorts of things, unset focused or remove it from its window
					focused->window_link.lock()->apply(rs);
				if (focused && rs == Unsupported && key == Key::Tab)
				{
					shared_ptr<Component> next=focused;
					while ((next = focused->window_link.lock()->component_link->successorOf(next)) && !next->tabFocusable());
					if (next)
						setFocused(next);
					return;
				}
			}
			if (rs == Unsupported)
			{
				//cout << "forwarding down"<<endl;
				input.cascadeKeyDown((Key::Name)key);
			}
		}
		
		void	Component::keyUp(int key)
		{
			eEventResult rs = Unsupported;
			if (focused)
			{
				rs = focused->onKeyUp((Key::Name)key);
				focused->window_link.lock()->apply(rs);
			}
			if (rs == Unsupported)
				input.cascadeKeyUp((Key::Name)key);
		}
		
		
		
		
		Component::~Component()
		{
			/*auto shared_this = shared_from_this();
			if (handling_event && ext.caught_by == shared_this)
				ext.caught_by.reset();
			if (focused == shared_this)
				focused.reset();
			if (hovered == shared_this)
				hovered.reset();
			if (clicked == shared_this)
				clicked.reset();*/
			//stupid, because destructor will only be invoked once no shared reference exists
		}
		
		bool		Component::isFocused()	const
		{
			return focused.get() == this;
		}
		
		void		Component::locate(const Rect<float>&parent_region,Rect<float>&region)	const
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
		
		void		Component::updateLayout(const Rect<float>&parent_region)
		{
			locate(parent_region,current_region);
			
			if (layout)
				layout->updateCells(current_region,cell_layout);
			else
				cell_layout.border = cell_layout.client = current_region;
		
		}
		
		float		Component::minWidth(bool include_offsets)	const
		{
			float rs = clientMinWidth();
			if (anchored.right && include_offsets)
				rs -= offset.right;
			if (anchored.left && include_offsets)
				rs += offset.left;
			if (layout)
			{
				rs += layout->client_edge.x.min+layout->client_edge.x.max;
				if (rs < layout->min_width)
					rs = layout->min_width;
			}
			return rs;
		}
		
		float		Component::minHeight(bool include_offsets)	const
		{
			float rs = clientMinHeight();
			if (anchored.top && include_offsets)
				rs -= offset.top;
			if (anchored.bottom && include_offsets)
				rs += offset.bottom;
			if (layout)
			{
				rs += layout->client_edge.y.min+layout->client_edge.y.max;
				if (rs < layout->min_height)
					rs = layout->min_height;
			}
			return rs;
		}
		
		
		
		
		
		

		

		
		void			Window::renderBuffers(Display<OpenGL>&display)
		{
			//cout << "rendering window "<<x<<", "<<y<<" / "<<fwidth<<", "<<fheight<<endl;
			if (layout_changed)
				updateLayout();
			layout_changed = false;
			visual_changed = false;
			if (size_changed)
				onResize();
			size_changed = false;
			ASSERT_GREATER__(fwidth,0);
			ASSERT_GREATER__(fheight,0);
			unsigned	ex = (unsigned)ceil(log((float)iwidth)/M_LN2),
						ey = (unsigned)ceil(log((float)iheight)/M_LN2);
			if (!ex || !ey)
				return;
			bool do_resize = ex > exp_x || ey > exp_y || ex+1 < exp_x || ey+1 < exp_y;

			Resolution target_resolution (1<<ex,1<<ey);
			
			if (!color_buffer.color_target[0].texture_handle)
			{
				color_buffer = gl_extensions.createFrameBuffer(target_resolution,Engine::DepthStorage::SharedBuffer,1,&GLType<GLbyte>::rgba_type_constant);
				normal_buffer = gl_extensions.createFrameBuffer(target_resolution,Engine::DepthStorage::SharedBuffer,1,&GLType<GLbyte>::rgba_type_constant);
				exp_x = ex;
				exp_y = ey;
			}
			elif (do_resize)
			{
				ASSERT__(gl_extensions.resizeFrameBuffer(color_buffer,target_resolution));
				ASSERT__(gl_extensions.resizeFrameBuffer(normal_buffer,target_resolution));
				exp_x = ex;
				exp_y = ey;
			}
			
			unsigned	resx = (1<<exp_x),
						resy = (1<<exp_y);
			ASSERT__(color_buffer.color_target[0].texture_handle != 0);
			ASSERT__(normal_buffer.color_target[0].texture_handle != 0);
			ASSERT2__(iwidth <= resx,iwidth,resx);
			ASSERT2__(iheight <= resy,iheight,resy);
			
			//height = ceil(height);
			usage_x = /*ceil*/(float)(iwidth)/(float)resx;
			usage_y = /*ceil*/(float)(iheight)/(float)resy;
			
			
			glWhite();
		
			display.lockRegion();
			
			normal_shader.install();
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			
			Rect<float>	view(0,0,iwidth,iheight);
			
			
			
			ASSERT__(gl_extensions.bindFrameBuffer(normal_buffer));
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_BLEND);

			shared_ptr<Operator> op = operator_link.lock();

			if (op)
			{
				op->resetFocus();
				op->focus(view);
			}
			
			for (index_t j = 0; j < cell_layout.cells.count(); j++)
			{
				const TCellInstance&cell = cell_layout.cells[j];
				display.useTexture(cell.normal_texture,true);
				Rect<float>	rect(cell.region);
				//rect.translate(-region.x.min,-region.y.min);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.x.min,rect.y.min);
					glTexCoord2f(1,0); glVertex2f(rect.x.max,rect.y.min);
					glTexCoord2f(1,1); glVertex2f(rect.x.max,rect.y.max);
					glTexCoord2f(0,1); glVertex2f(rect.x.min,rect.y.max);
				glEnd();
			}
			glEnable(GL_BLEND);
			if (component_link && component_link->visible)
			{
				component_link->onNormalPaint();
			}
			normal_shader.uninstall();
			ASSERT__(gl_extensions.bindFrameBuffer(color_buffer));
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_BLEND);

			if (op)
			{
				op->resetFocus();
				op->focus(view);
			}
			for (index_t j = 0; j < cell_layout.cells.count(); j++)
			{
				const TCellInstance&cell = cell_layout.cells[j];
				display.useTexture(cell.color_texture,true);
				const Rect<float>	&rect(cell.region);
				//rect.translate(-region.x.min,-region.y.min);

				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.x.min,rect.y.min);
					glTexCoord2f(1,0); glVertex2f(rect.x.max,rect.y.min);
					glTexCoord2f(1,1); glVertex2f(rect.x.max,rect.y.max);
					glTexCoord2f(0,1); glVertex2f(rect.x.min,rect.y.max);
				glEnd();
			}
			glEnable(GL_BLEND);
			if (cell_layout.title.width()>0)
			{
				Component::textout.locate(cell_layout.title.x.min,cell_layout.title.y.center()-Component::textout.getFont().getHeight()/2+font_offset);
				Component::textout.color(1,1,1);
				Component::textout.print(title);
			}			
			if (component_link && component_link->visible)
				component_link->onColorPaint();
			


			
			display.unlockRegion();
			gl_extensions.unbindFrameBuffer();
			
		}

		shared_ptr<Component>		Component::successorOf(const shared_ptr<Component>&child_link)
		{
			count_t num_children = countChildren();
			for (index_t i = 0; i < num_children; i++)
			{
				shared_ptr<Component> link = child(i);
				if (link == child_link)
				{
					if (i < num_children-1)
					{
						shared_ptr<Component> next = child(i+1);
						while (next->countChildren())
							next = next->child(0);
						return next;
					}
					return shared_from_this();
				}
				shared_ptr<Component> successor = link->successorOf(child_link);
				if (successor)
					return successor;
			}
			return shared_ptr<Component>();
		}
		
		index_t		Component::indexOfChild(const shared_ptr<Component>&child_) const
		{
			count_t num_children = countChildren();
			for (index_t i = 0; i < num_children; i++)
				if (child(i) == child_)
					return i;
			return TypeInfo<index_t>::max;
		}
		
		
		void			Component::onColorPaint()
		{
			if (!layout)
				return;
			glDisable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER,0.0f);

			shared_ptr<Operator> op = requireOperator();

			Display<OpenGL>&display = op->getDisplay();

			for (index_t j = 0; j < cell_layout.cells.count(); j++)
			{
				const TCellInstance&cell = cell_layout.cells[j];
				display.useTexture(cell.color_texture,true);
				const Rect<float>&rect = cell.region;

				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.x.min,rect.y.min);
					glTexCoord2f(1,0); glVertex2f(rect.x.max,rect.y.min);
					glTexCoord2f(1,1); glVertex2f(rect.x.max,rect.y.max);
					glTexCoord2f(0,1); glVertex2f(rect.x.min,rect.y.max);
				glEnd();
			}
			glDisable(GL_ALPHA_TEST);
			glEnable(GL_BLEND);
			//display->setDefaultBlendFunc();			
		}
		
		void			Component::onNormalPaint()
		{
			if (!layout)
				return;
			shared_ptr<Operator> op = requireOperator();

			Display<OpenGL>&display = op->getDisplay();
			for (index_t j = 0; j < cell_layout.cells.count(); j++)
			{
				const TCellInstance&cell = cell_layout.cells[j];
				display.useTexture(cell.normal_texture,true);
				const Rect<float>&rect = cell.region;

				glBegin(GL_QUADS);
					glTexCoord2f(0,0); glVertex2f(rect.x.min,rect.y.min);
					glTexCoord2f(1,0); glVertex2f(rect.x.max,rect.y.min);
					glTexCoord2f(1,1); glVertex2f(rect.x.max,rect.y.max);
					glTexCoord2f(0,1); glVertex2f(rect.x.min,rect.y.max);
				glEnd();
			}
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
		static bool mouseHover(const shared_ptr<Window>&window, float x, float y)
		{
			handling_event = true;
			ext.custom_cursor = Mouse::CursorType::Default;
			ext.caught_by.reset();
			Component::eEventResult rs = window->component_link->onMouseHover(x,y,ext);
			if (rs != Component::Unsupported)
			{
				window->apply(rs);
				setCursor(ext.custom_cursor);
			}
			else
				ext.caught_by.reset();
			
			if (hovered != ext.caught_by)
			{
				if (hovered)
					hovered->window()->apply(hovered->onMouseExit());
				hovered = ext.caught_by;
			}
			handling_event = false;
			return rs != Component::Unsupported;
		}
		
		
		void			Operator::render()
		{
			if (!display)
				FATAL__("GUI not properly initialized. Display is NULL");
			bool aspect_was_in_projection = GlobalAspectConfiguration::load_as_projection;
			GlobalAspectConfiguration::load_as_projection = false;
			bool world_z_was_up = GlobalAspectConfiguration::world_z_is_up;
			GlobalAspectConfiguration::world_z_is_up = false;
			if (focused)
			{
				time_since_last_tick += timing.delta;
				if (time_since_last_tick > focused->tick_interval)
				{
					time_since_last_tick = 0;
					focused->window()->apply(focused->onTick());
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
				if (mouse->buttons.down[0] && owns_mouse_down && (window_stack.isNotEmpty() || menu_stack.isNotEmpty()))
				{
					if (dragging)
					{
						cursor_mode = drag_type;
						TVec2<float>	d =	{m.x-last.x,
											m.y-last.y};
						if (d.x || d.y)
						{
							if (drag_type == Window::ClickResult::DragWindow)
								dragging->drag(d);
							else
								dragging->dragResize(d,drag_type);
							last.x += d.x;
							last.y += d.y;
						}
					}
					else
					{
					/*	if (last_x != mouse.location.fx || last_y != mouse.location.fy)
						{
							window_stack.last()->mouseMove((float)(mouse.location.fx*display->clientWidth()),(float)(mouse.location.fy*display->clientHeight()));
							last_x = mouse.location.fx;
							last_y = mouse.location.fy;
						}*/
					}
				}
				if (clicked)
				{
					shared_ptr<Window>	window = menu_stack.isNotEmpty()?menu_stack.last().lock():window_stack.last();

					if (!window && menu_stack.isNotEmpty())	//last window has been erased
					{
						hideMenus();
						window = window_stack.last();
					}

					#ifdef DEEP_GUI
						float	rx = (m.x-window->current_center.x)*window->current_center.shell_radius,
								ry = (m.y-window->current_center.y)*window->current_center.shell_radius;
					#else
						float	rx = (m.x-window->x),
								ry = (m.y-window->y);
					#endif
					rx += window->fwidth*0.5;
					ry += window->fheight*0.5;
					ASSERT_NOT_NULL1__(clicked->window(),clicked->type_name);
					clicked->window()->apply(clicked->onMouseDrag(rx,ry));
					cursor_mode = Window::ClickResult::Component;
				}
				elif (cursor_mode == Window::ClickResult::Missed)
				{
					bool is_modal = false;
					for (index_t i = menu_stack.count()-1; i < menu_stack.count(); i--)
					{
						shared_ptr<Window> window = menu_stack[i].lock();
						if (!window)
						{
							menu_stack.erase(i);
							continue;
						}
						window->fixed_position = true;
						float rx,ry;
						Window::ClickResult::value_t rs = window->resolve(m.x,m.y,rx,ry);
						if (rs != Window::ClickResult::Missed)
						{
							if (is_modal)
								rs = Window::ClickResult::Ignored;
						
							cursor_mode = rs;
							if (rs == Window::ClickResult::Component)
								if (!mouseHover(window,rx,ry))
									cursor_mode = Window::ClickResult::DragWindow;
							is_modal = true;
						break;
						}
						is_modal = true;
					}
					for (index_t i = window_stack.count()-1; i < window_stack.count(); i--)
					{
						const shared_ptr<Window>&window = window_stack[i];
						float rx,ry;
						Window::ClickResult::value_t rs = window->resolve(m.x,m.y,rx,ry);
						if (rs != Window::ClickResult::Missed)
						{
							if (is_modal)
								rs = Window::ClickResult::Ignored;
							else
							{
								#ifdef DEEP_GUI
									window->setShellDestination(1.00001);
								#else
									//window_stack.append(window_stack.drop(window));
								#endif
								cursor_mode = rs;
								if (rs == Window::ClickResult::Component)
									if (!mouseHover(window,rx,ry))
										cursor_mode = Window::ClickResult::DragWindow;
								#ifdef DEEP_GUI
									for (unsigned j = i-1; j < window_stack.count(); j--)
										window_stack[j]->setShellDestination(radiusOf(j));
								#endif
							}
							break;
						}
						#ifdef DEEP_GUI
							else
								window->setShellDestination(radiusOf(i));
						#endif
						is_modal |= window->is_modal;
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
						hovered->window()->apply(hovered->onMouseExit());
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
				if (window->visual_changed)
				{
					if (window->operator_link.expired())
						window->operator_link = shared_from_this();	//I'm tired
					window->renderBuffers(*display);
				}
			}
			for (index_t i = 0; i < window_stack.count(); i++)
			{
				const shared_ptr<Window>&window = window_stack[i];
				if (window->visual_changed)
					window->renderBuffers(*display);
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
				Sort<DepthSort>::quickSortRAL(window_stack);
				window_stack.revert();
			#endif

			display->pick(projected_space);

			glDisable(GL_CULL_FACE);
			
			glBindTexture(GL_TEXTURE_2D,layer_texture);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			
			if (setting_changed)
			{
				window_shader.userConfig()->set(blur_effect_switch,blur_effect);
				window_shader.userConfig()->set(refract_effect_switch,refract_effect);
				window_shader.userConfig()->set(bump_effect_switch,bump_effect);
				//logfile << window_shader.assembleSource()<<nl<<nl;
				current_window_shader = window_shader.buildShader();
				if (!current_window_shader)
					ErrMessage(window_shader.report());
				setting_changed = false;
			}

			
			ASSERT__(current_window_shader->install());
			float	w = display->clientWidth(),
					h = display->clientHeight();
			glWhite();
			for (index_t i = 0; i < window_stack.count(); i++)
			{
				const shared_ptr<Window>&window = window_stack[i];
				
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
			current_window_shader->uninstall();
		
			/*

			for (index_t i = 0; i < window_stack.count(); i++)
			{
				const shared_ptr<Window>&window = window_stack[i];
				
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
			GlobalAspectConfiguration::load_as_projection = aspect_was_in_projection;
			GlobalAspectConfiguration::world_z_is_up = world_z_was_up;
		}
		
		
		bool			Operator::windowIsVisible(const shared_ptr<Window>&window)	const
		{
			return window_stack.contains(window);
		}
		
		shared_ptr<Window>	Operator::getTopWindow() const
		{
			if (window_stack.isEmpty())
				return shared_ptr<Window>();
			return window_stack.last();
		}

		
		void			Operator::insertWindow(const shared_ptr<Window>&window)
		{
			if (!window->operator_link.expired())
			{
				shared_ptr<Operator>	op = window->operator_link.lock();
				if (op && op.get() != this)
				{
					op->removeWindow(window);
					window->operator_link = shared_from_this();
				}
			}
			else
				window->operator_link = shared_from_this();
			if (window_stack.isNotEmpty() && window_stack.last() == window)
				return;
			window_stack.findAndErase(window);

			if (!window->is_modal && window_stack.isNotEmpty() && window_stack.last()->is_modal)
			{
				index_t at = window_stack.size()-2;
				while (at != InvalidIndex && window_stack[at]->is_modal)
					at--;
				window_stack.insert(at+1,window);
			}
			else
			{
				if (window_stack.isNotEmpty())
					window_stack.last()->onFocusLost();
				window_stack << window;
				Component::resetFocused();
				window->onFocusGained();
			}
			#ifdef DEEP_GUI
				window->current_center.shell_radius = window->destination.shell_radius = window->origin.shell_radius = radiusOf(window_stack-1);
			#endif

			#ifdef DEEP_GUI
				for (unsigned j = 0; j < window_stack.count(); j++)
					window_stack[j]->setShellDestination(radiusOf(j));
			#endif
			stack_changed=true;
		}
		
		bool			Operator::removeWindow(const shared_ptr<Window>&window)
		{
			bool was_top = window_stack.isNotEmpty() && window_stack.last() == window;
				
			if (window_stack.findAndErase(window))
			{
				if (was_top)
				{
					window->onFocusLost();
					Component::resetFocused();
					if (window_stack.isNotEmpty())
						window_stack.last()->onFocusGained();
				}
				window->operator_link.reset();
				#ifdef DEEP_GUI
					for (index_t j = 0; j < window_stack.count(); j++)
						window_stack[j]->setShellDestination(radiusOf(j));
				#endif
				stack_changed=true;
				return true;
			}
			return false;
		}
		
		
		static bool mouseDown(const shared_ptr<Window>&window, float x, float y)
		{
			handling_event = true;
			ext.custom_cursor = Mouse::CursorType::Default;
			ext.caught_by.reset();
			Component::eEventResult rs = window->component_link->onMouseDown(x,y,ext);
			clicked.reset();
			if (rs != Component::Unsupported)
			{
				window->apply(rs);
				clicked = ext.caught_by;
				setCursor(ext.custom_cursor);
			}
			Component::setFocused(clicked?clicked->getFocused():shared_ptr<Component>());
			handling_event = false;
			return rs != Component::Unsupported;
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
			
				window->fixed_position = true;

				float rx, ry;
				Window::ClickResult::value_t rs = window->resolve(m.x,m.y,rx,ry);
				
				
				if (rs != Window::ClickResult::Missed)
				{
					owns_mouse_down = true;
					dragging.reset();
					if (rs == Window::ClickResult::Component)
						if (!GUI::mouseDown(window,rx,ry))
							rs = window->fixed_position?Window::ClickResult::Ignored:Window::ClickResult::DragWindow;
					if (rs != Window::ClickResult::Component)
						Component::setFocused(shared_ptr<Component>());
					return true;
				}
				else
				{
					window->hidden = timing.now64;
					menu_stack.erase(i);
				}
			}
			
			for (index_t i = window_stack.count()-1; i < window_stack.count(); i--)
			{
				shared_ptr<Window> window = window_stack[i];
			
				float rx, ry;
				Window::ClickResult::value_t rs = window->resolve(m.x,m.y,rx,ry);
				
				if (rs != Window::ClickResult::Missed)
				{
					if (rs == Window::ClickResult::Component)
						if (!GUI::mouseDown(window,rx,ry))
							rs = window->fixed_position?Window::ClickResult::Ignored:Window::ClickResult::DragWindow;
					if (rs != Window::ClickResult::Component)
						Component::resetFocused();
					if (i+1 != window_stack.count())
					{
						window_stack.last()->onFocusLost();
						window_stack.erase(i);
						Component::resetFocused();
						window_stack << window;
						window->onFocusGained();
						#ifdef DEEP_GUI
							for (unsigned j = 0; j < window_stack.count(); j++)
								window_stack[j]->setShellDestination(radiusOf(j));
						#endif
					}
					owns_mouse_down = true;
					dragging = (rs >= Window::ClickResult::DragWindow ? window : shared_ptr<Window>());
					drag_type = rs;
					return true;
				}
				elif (window->is_modal)
					return false;
			}
			Component::setFocused(shared_ptr<Component>());
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
					float x = (m.x-window->current_center.x)*window->current_center.shell_radius+window->fwidth/2;
					float y = (m.y-window->current_center.y)*window->current_center.shell_radius+window->fheight/2;
				#else
					float x = (m.x-window->x)+window->fwidth/2;
					float y = (m.y-window->y)+window->fheight/2;
				#endif
				if (!window->cell_layout.border.contains(x,y))
				{
					window->hidden = timing.now64;
					menu_stack.erase(i);
					continue;
				}
				
				if (window->component_link && window->component_link->isVisible() && window->component_link->isEnabled())
				{
					Component::eEventResult rs = window->component_link->onMouseWheel(x,y,delta);
					window->apply(rs);
					if (rs != Component::Unsupported)
						mouseHover(window,x,y);
				}
				return true;
			}
			for (index_t i = window_stack.count()-1; i < window_stack.count(); i--)
			{
				shared_ptr<Window> window = window_stack[i];
				
				#ifdef DEEP_GUI
					float x = (m.x-window->current_center.x)*window->current_center.shell_radius+window->fwidth/2;
					float y = (m.y-window->current_center.y)*window->current_center.shell_radius+window->fheight/2;
				#else
					float x = (m.x-window->x)+window->fwidth/2;
					float y = (m.y-window->y)+window->fheight/2;
				#endif
				if (!window->cell_layout.border.contains(x,y))
					continue;
				
				if (window->component_link && window->component_link->isVisible() && window->component_link->isEnabled())
				{
					Component::eEventResult rs = window->component_link->onMouseWheel(x,y,delta);
					window->apply(rs);
					if (rs != Component::Unsupported)
						mouseHover(window,x,y);
				}
				if (i+1 != window_stack.count())
				{
					window_stack.last()->onFocusLost();
					window_stack.erase(i);
					Component::resetFocused();
					window_stack << window;
					window->onFocusGained();
				}
				#ifdef DEEP_GUI
					for (unsigned j = 0; j < window_stack.count(); j++)
						window_stack[j]->setShellDestination(radiusOf(j));
				#endif
				return true;
			}
			return false;
		}
		
		void			Operator::mouseUp()
		{
			if (!owns_mouse_down)
				return;
			handling_event = true;
			if (clicked)
			{
				TVec2<float> m;
				unprojectMouse(m);
				clicked->window()->mouseUp(m.x,m.y);
			}
			clicked.reset();
			handling_event = false;
		}
		

		
		shared_ptr<Window>		Operator::createWindow(const Rect<float>&region, const String&name,modal_t modal, const shared_ptr<Component>&component /*=shared_ptr<Component>()*/)
		{
			return createWindow(region,name,modal,&Window::common_style,component);
		}
		
		shared_ptr<Window>		Operator::createWindow(const Rect<float>&region, const String&name, modal_t modal,Layout*layout,const shared_ptr<Component>&component)
		{
			shared_ptr<Window> result(new Window(modal == ModalWindow,layout));
			if (window_stack.isNotEmpty())
			{
				if (!window_stack.last()->is_modal || modal)
				{
					window_stack.last()->onFocusLost();
					window_stack.append(result);
					Component::resetFocused();
					result->onFocusGained();
				}
				else
				{
					index_t at = window_stack.size()-2;
					while (at != InvalidIndex && window_stack[at]->is_modal)
						at--;
					window_stack.insert(at+1,result);
				}
			}
			else
			{
				Component::resetFocused();
				window_stack.append(result);
				result->onFocusGained();
			}
			result->operator_link = shared_from_this();
			
			#ifdef DEEP_GUI
				result->destination.x = region.x.center()-display->clientWidth()/2;
				result->destination.y = region.y.center()-display->clientHeight()/2;
				
				for (index_t i = 0; i < window_stack.count(); i++)
					window_stack[i]->destination.shell_radius = radiusOf(i);
				//result->destination.shell_radius = radiusOf(window_stack.count()-1);
				//result->destination.x *= result->destination.shell_radius;
				//result->destination.y *= result->destination.shell_radius;
				result->origin.x = 0;
				result->origin.y = 0;
				result->origin.shell_radius = 1000;
				result->origin = result->destination;
				result->current_center = result->origin;
			#else
				result->x = region.x.center()-display->clientWidth()/2;
				result->y = region.y.center()-display->clientHeight()/2;
			#endif
			result->fwidth = region.width();
			result->fheight = region.height();
			result->iwidth = (unsigned)round(result->fwidth);
			result->iheight = (unsigned)round(result->fheight);
			result->setComponent(component);
			result->title = name;
			Rect<float> reg(0,0,result->iwidth,result->iheight);
			if (layout)
				layout->updateCells(reg,result->cell_layout);
			else
				result->cell_layout.border = result->cell_layout.client = reg;
			return result;
		}
		
		void Operator::bind(Key::Name key)
		{
			input->bind(key,std::bind(Component::keyDown,key),std::bind(Component::keyUp,key));
			input->bindCtrl(key,std::bind(Component::keyDown,key));
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
		

			bool aspect_was_in_projection = GlobalAspectConfiguration::load_as_projection;
			GlobalAspectConfiguration::load_as_projection = false;
			bool world_z_was_up = GlobalAspectConfiguration::world_z_is_up;
			GlobalAspectConfiguration::world_z_is_up = false;

			rs->updateDisplaySize();
			rs->projected_space.make(1.0f,0.1,100,92);
			//projected_space.retraction[2] = -1;
			rs->projected_space.build();
			rs->projected_space.depth_test = NoDepthTest;

			GlobalAspectConfiguration::load_as_projection = aspect_was_in_projection;
			GlobalAspectConfiguration::world_z_is_up = world_z_was_up;
		
			GUI::initialize();
			return rs;
		}

		void	Operator::updateDisplaySize()
		{
			float	w = display->clientWidth(),
					h = display->clientHeight();
			window_space.make(0,0,
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
			for (index_t i = 0; i < window_stack.size(); i++)
			{
				window_stack[i]->x = clamped(window_stack[i]->x,min_x,max_x);
				window_stack[i]->y = clamped(window_stack[i]->y,min_y,max_y-window_stack[i]->fheight/2.f);
			}
		}
	
	
		void	setBlurEffect(bool setting)
		{
			setting_changed |= blur_effect != setting;
			blur_effect = setting;
		}
			
		void	setRefractEffect(bool setting)
		{
			setting_changed |= refract_effect != setting;
			refract_effect = setting;
		}
			
		void	setBumpEffect(bool setting)
		{
			setting_changed |= bump_effect != setting;
			bump_effect = setting;
		}
	
	
		
		
		
	}
}
