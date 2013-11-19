#ifndef eveTplH
#define eveTplH


namespace Engine
{

	template <class GL>Display<GL>::Display():exec_target(NULL),context_error(false),region_locked(false),framebuffer_bound(false),resolution_overridden(false),config(default_buffer_config)
	{
		GL::global_instance = this;
	}


	template <class GL> inline void Display<GL>::renderToken()
	{
		renderSomething();
	}

	template <class GL> inline void Display<GL>::renderPivot()
	{
		renderSomething();
	}
	
	template <class GL> 
	void						Display<GL>::renderLights()
	{
		Array<Light*>	lights;
		GL::getSceneLights(lights, true);
		if (!lights_defined)
		{
			lights_defined = true;
			
		/*	spot.select(SimpleGeometry::Lines);
			spot.color(1,1,1);
			spot.vertex(0,0,0);
			spot.color(0,0,1);
			spot.vertex(0,1,1);
			spot.color(1,1,1);
			spot.vertex(0,0,0);
			spot.color(0,0,1);
			spot.vertex(0,-1,1);
			spot.color(1,1,1);
			spot.vertex(0,0,0);
			spot.color(0,0,1);
			spot.vertex(1,0,1);
			spot.color(1,1,1);
			spot.vertex(0,0,0);
			spot.color(0,0,1);
			spot.vertex(-1,0,1);
			for (unsigned i = 0; i < 40; i++)
			{
				float	f0 = (float)i/40.0f*2*M_PI,
						f1 = (float)(i+1)/40.0f*2*M_PI;
				spot.vertex(cos(f0),sin(f0),1);
				spot.vertex(cos(f1),sin(f1),1);
			}*/
			
			spot.select(SimpleGeometry::Triangles);
			for (unsigned i = 0; i < 40; i++)
			{
				float	f0 = (float)i/40.0f*2*M_PI,
						f1 = (float)(i+1)/40.0f*2*M_PI;
				spot.color(0,0,1,0);
				spot.vertex(cos(f0),sin(f0),1);
				spot.vertex(cos(f1),sin(f1),1);
				spot.color(1,1,1);
				spot.vertex(0,0,0);
			}
			
			spot.seal();
			
			
			omni.select(SimpleGeometry::Lines);
			omni.color(1,1,1);
			omni.vertex(2,0,0);
			omni.vertex(-2,0,0);
			omni.vertex(0,2,0);
			omni.vertex(0,-2,0);
			omni.vertex(0,0,2);
			omni.vertex(0,0,-2);
			omni.seal();
			
			

			direct.select(SimpleGeometry::Quads);
			for (unsigned i = 0; i < 40; i++)
			{
				float	f0 = (float)i/40.0f*2*M_PI,
						f1 = (float)(i+1)/40.0f*2*M_PI;
				direct.color(0,0,1,0);
				direct.vertex(cos(f1)*0.5,sin(f1)*0.5,0);
				direct.vertex(cos(f0)*0.5,sin(f0)*0.5,0);
				direct.color(1,1,1);
				direct.vertex(cos(f0)*0.5,sin(f0)*0.5,1);
				direct.vertex(cos(f1)*0.5,sin(f1)*0.5,1);
			}
			
			direct.select(SimpleGeometry::Triangles);
			direct.color(1,1,1);
			for (unsigned i = 0; i < 40; i++)
			{
				float	f0 = (float)i/40.0f*2*M_PI,
						f1 = (float)(i+1)/40.0f*2*M_PI;
				direct.vertex(cos(f1)*0.5,sin(f1)*0.5,1);
				direct.vertex(cos(f0)*0.5,sin(f0)*0.5,1);
				direct.vertex(0,0,1);
			}
			
			direct.seal();
			
			
		}
		for (index_t i = 0; i < lights.count(); i++)
		{
		
			switch (lights[i]->GetType())
			{
				case Light::Spot:
				{
					float scale = Vec::distance(camera_location,lights[i]->GetPosition())/5;
					TMatrix4<> system;
					float angle = pow(0.94,lights[i]->GetSpotExponent());
					/*if (angle*45 > lights[i]->getSpotCutoff())
						angle = lights[i]->getSpotCutoff()/45;*/
					float z_scale = 1.0/clamped(angle,0.1,1);
					Mat::makeAxisSystem(lights[i]->GetPosition(),lights[i]->GetSpotDirection(),2,system);
					Vec::mult(system.x.xyz,scale/z_scale);
					Vec::mult(system.y.xyz,scale/z_scale);
					Vec::mult(system.z.xyz,scale*z_scale);
					
					GL::enterSubSystem(system);
						GL::render(spot);
					
					GL::exitSubSystem();
				
				}
				break;
				case Light::Omni:
				{
					float scale = Vec::distance(camera_location,lights[i]->GetPosition())/10;
					TMatrix4<> system;
					Mat::eye(system);
					Vec::copy(lights[i]->GetPosition(),system.w.xyz);
					Vec::mult(system.x.xyz,scale);
					Vec::mult(system.y.xyz,scale);
					Vec::mult(system.z.xyz,scale);
					GL::enterSubSystem(system);
						GL::render(omni);
					GL::exitSubSystem();
				}
				break;
				case Light::Direct:
				{
					float scale = Vec::length(camera_location)/4;
					TMatrix4<> system;
					TVec3<>	offset;
					Vec::mult(lights[i]->GetPosition(),scale,offset);
					Mat::makeAxisSystem(offset,lights[i]->GetPosition(),2,system);
					Vec::mult(system.x.xyz,scale);
					Vec::mult(system.y.xyz,scale);
					Vec::mult(system.z.xyz,scale*2);
					GL::enterSubSystem(system);
						GL::render(direct);
					GL::exitSubSystem();
				}
				break;
			}
		
		
		}
		
		
		
	}

	template <class GL> void Display<GL>::renderSomething()
	{
	    if (!pivot_defined)
	    {
	        pivot_defined = true;
	        pivot.select(SimpleGeometry::Triangles);
	        pivot.resize(8);

	        pivot.color(1,0,0); pivot.vertex(0,0,1);
	        pivot.color(0,1,0); pivot.vertex(1,0,0);
	        pivot.color(0,0,1); pivot.vertex(0,1,0);

	        pivot.color(0,1,0); pivot.vertex(1,0,0);
	        pivot.color(1,1,0); pivot.vertex(0,0,-1);
	        pivot.color(0,0,1); pivot.vertex(0,1,0);

	        pivot.color(1,1,0); pivot.vertex(0,0,-1);
	        pivot.color(0,1,1); pivot.vertex(-1,0,0);
	        pivot.color(0,0,1); pivot.vertex(0,1,0);

	        pivot.color(0,1,1); pivot.vertex(-1,0,0);
	        pivot.color(1,0,0); pivot.vertex(0,0,1);
	        pivot.color(0,0,1); pivot.vertex(0,1,0);

	        pivot.color(0,1,0); pivot.vertex(1,0,0);
	        pivot.color(1,0,0); pivot.vertex(0,0,1);
	        pivot.color(0,0,1); pivot.vertex(0,-1,0);

	        pivot.color(1,1,0); pivot.vertex(0,0,-1);
	        pivot.color(0,1,0); pivot.vertex(1,0,0);
	        pivot.color(0,0,1); pivot.vertex(0,-1,0);

	        pivot.color(0,1,1); pivot.vertex(-1,0,0);
	        pivot.color(1,1,0); pivot.vertex(0,0,-1);
	        pivot.color(0,0,1); pivot.vertex(0,-1,0);

	        pivot.color(1,0,0); pivot.vertex(0,0,1);
	        pivot.color(0,1,1); pivot.vertex(-1,0,0);
	        pivot.color(0,0,1); pivot.vertex(0,-1,0);
	    }
	    GL::render(pivot);
	}



	
	#if SYSTEM==WINDOWS
	template <class GL>bool Display<GL>::create(const DisplayConfig&dconfig)
	{

	    framebuffer_bound = false;
		resolution_overridden = false;
		while (true)
	    {
			HWND hWnd = context.createWindow(dconfig.window_name,dconfig.border_style, dconfig.onResize,dconfig.icon_filename);
	        if (!hWnd)
	        {
	            context_error = true;
	            return false;
	        }
	        if (!GL::createContext(hWnd,config))
	        {
	            context.destroyWindow();
	            if (GL::getErrorCode() == ERR_RETRY)
	                continue;
	            context_error = false;
	            return false;
	        }
	        const RECT&window = context.windowLocation();
	        GL::setRegion(rect(0,0,window.right-window.left,window.bottom-window.top));
	        return true;
	    }

	}
	#elif SYSTEM==UNIX
	template <class GL>bool Display<GL>::create(const DisplayConfig&dconfig)
	{
	    framebuffer_bound = false;
	    Display*connection = context.connect();
	    if (!connection)
	    {
	        context_error = true;
	        return false;
	    }
	    while (true)
	    {
	        TWindowAttributes attributes;
	        if (!GL::createContext(connection,config,attributes))
	        {
	            if (GL::getErrorCode() == ERR_RETRY)
	                continue;
	            context.disconnect();
	            context_error = false;
	            return false;
	        }
	        Window wnd = context.createWindow(dconfig.window_name,attributes,dconfig.hide_border,dconfig.icon_filename);
	        if (!wnd)
	        {
	            GL::destroyContext();
	            context_error = true;
	            context.disconnect();
	            return false;
	        }
	        if (!GL::bindContext(wnd))
	        {
	            context_error = false;
	            context.destroyWindow();
	            GL::destroyContext();
	            context.disconnect();
	            context_error = false;
	            return false;
	        }
	        const RECT&window = context.windowLocation();
	        GL::setRegion(rect(0,0,window.right-window.left,window.bottom-window.top));

	        current_target_resolution = window_client_resolution = context.clientSize();
			pixel_aspect = current_target_resolution.aspect();

	        return true;
	    }
	}
	#endif


	template <class GL>String Display<GL>::errorStr()
	{
	    if (context_error)
	        return "Context returns ("+IntToStr(context.errorCode())+"): "+String(context.errorStr());
	    return "System "+String(GL::name())+" returns ("+IntToStr(GL::getErrorCode())+"): "+String(GL::GetErrorStr());
	}


	template <class GL> inline bool Display<GL>::hideCursor()
	{
	    return mouse.hideCursor();
	}

	template <class GL> inline void Display<GL>::showCursor()
	{
	    mouse.showCursor();
	}

	template <class GL> inline  void  Display<GL>::locateWindow(unsigned left, unsigned top, unsigned width, unsigned height)
	{
	    context.locateWindow(left,top,width,height);
		overrideSetClientResolution(context.clientSize());
	}

	template <class GL>	inline	void	Display<GL>::overrideSetClientResolution(const Resolution&region)
	{
		overridden_client_resolution = region;
		resolution_overridden = true;
	}

	template <class GL> inline  void Display<GL>::locateWindow(const RECT&rect)
	{
		context.locateWindow(rect);
		overrideSetClientResolution(context.clientSize());
	}

	template <class GL> inline  void Display<GL>::resizeWindow(unsigned width, unsigned height, DisplayConfig::border_style_t style)
	{
	    context.resizeWindow(width,height,style);
		overrideSetClientResolution(context.clientSize());
	}
	template <class GL>
		inline	void		Display<GL>::setDimension(unsigned width, unsigned height, DisplayConfig::border_style_t style)
		{
			resizeWindow(width,height,style);
		}
	template <class GL>
		inline	void		Display<GL>::setSize(unsigned width, unsigned height, DisplayConfig::border_style_t style)
		{
			resizeWindow(width,height,style);
		}
	template <class GL>
		inline	void		Display<GL>::setDimensions(unsigned width, unsigned height, DisplayConfig::border_style_t style)
		{
			resizeWindow(width,height,style);
		}


	template <class GL>
		/*static*/ inline  const RECT& Display<GL>::windowLocation()
		{
			return context.windowLocation();
		}

	template <class GL> inline
		/*static*/ unsigned			Display<GL>::width()
		{
			const RECT&	dim = context.windowLocation();
			return dim.right-dim.left;
		}

	template <class GL> inline
		UINT			Display<GL>::clientWidth()	const
		{
			return resolution_overridden ? overridden_client_resolution.width : context.clientWidth();
		}
	
	template <class GL> inline
		UINT			Display<GL>::clientHeight()	const
		{
			return resolution_overridden ? overridden_client_resolution.height : context.clientHeight();
		}
	
		

	template <class GL> inline
		/*static*/ unsigned			Display<GL>::height()
		{
			const RECT& dim = context.windowLocation();
			return dim.bottom-dim.top;
		}

	template <class GL> inline
		Resolution				Display<GL>::clientSize()	const
		{
			return resolution_overridden ? overridden_client_resolution : context.clientSize();
		}
		
	template <class GL>
		inline Resolution					Display<GL>::currentRargetResolution()	const
		{
			if (framebuffer_bound)
				return target_buffer_resolution;
			if (resolution_overridden)
				return overridden_client_resolution;
			return context.clientSize();
		}
		
	template <class GL> inline
		/*static*/ Resolution				Display<GL>::size()
		{
			return context.windowSize();
		}

	template <class GL> inline
		float				Display<GL>::pixelAspect()	const
		{
			if (framebuffer_bound)
				return target_buffer_resolution.aspect();
			if (resolution_overridden)
				return overridden_client_resolution.aspect();
			return context.pixelAspectf();
		}


	template <class GL> inline
		/*static*/ Resolution				Display<GL>::dimension()
		{
			return context.windowSize();
		}

	template <class GL> inline
		/*static*/ Resolution				Display<GL>::dimensions()
		{
			return context.windowSize();
		}





	template <class GL> inline void Display<GL>::setScreen(const TDisplayMode&mode)
	{
	    context.setScreen(mode);
		overrideSetClientResolution(context.clientSize());
	}

	template <class GL> inline void Display<GL>::queryScreen(ResolutionList*r_list, FrequencyList*f_list, DWORD w_min, DWORD h_min, DWORD f_min)
	{
	    context.queryScreen(r_list,f_list,w_min,h_min,f_min);
	}

	template <class GL>
		/*static*/ inline Resolution		Display<GL>::getScreenSize()
		{
			return context.getScreenSize();
		}

	
	#if SYSTEM==WINDOWS

	template <class GL>
		/*static*/ inline bool Display<GL>::getScreen(DEVMODE&mode)
		{
			return context.getScreen(mode);
		}

	template <class GL>
		/*static*/ inline bool Display<GL>::getScreen(DEVMODE*mode)
		{
			return context.getScreen(mode);
		}

	template <class GL>
		/*static*/ inline bool Display<GL>::isCurrent(const DEVMODE&screen)
		{
			return context.isCurrent(screen);
		}

	template <class GL>
		/*static*/ inline void						Display<GL>::AcceptFileDrop()
		{
			context.AcceptFileDrop();
		}
	template <class GL>
		/*static*/ inline void						Display<GL>::AcceptFileDrop(const DragEventHandler&dragHandler, const DropEventHandler&dropHandler)
		{
			context.AcceptFileDrop(dragHandler, dropHandler);
		}
	template <class GL>
		/*static*/ inline void						Display<GL>::BlockFileDrop()
		{
			context.BlockFileDrop();
		}


	#elif SYSTEM==UNIX

	template <class GL> inline  int Display<GL>::findScreen(DWORD width, DWORD height, DWORD&refresh_rate)
	{
	    return context.findScreen(width,height,refresh_rate);
	}

/*
	template <class GL> inline const XRRScreenSize&   Display<GL>::getScreen(unsigned index)
	{
	    return context.getScreen(index);
	}

	template <class GL> inline bool Display<GL>::getScreen(XRRScreenSize*size)
	{
	    return context.getScreen(size);
	}

	template <class GL> inline bool Display<GL>::getScreen(XRRScreenSize&size)
	{
	    return context.getScreen(size);
	}

	template <class GL> inline bool Display<GL>::isCurrent(const XRRScreenSize&size)
	{
	    return context.isCurrent(size);
	}
*/


	#endif

	template <class GL> inline short Display<GL>::getRefreshRate()
	{
	    return context.getRefreshRate();
	}



	template <class GL> inline bool Display<GL>::applyScreen()
	{
	    bool result = context.applyScreen();
		overrideSetClientResolution(context.clientSize());
	    return result;
	}


	template <class GL> inline  bool Display<GL>::applyWindowScreen(DWORD refresh_rate)
	{
	    bool result = context.applyWindowScreen(refresh_rate);
		overrideSetClientResolution(context.clientSize());
	    return result;
	}

	template <class GL> inline  void Display<GL>::resetScreen()
	{
	    context.resetScreen();
		overrideSetClientResolution(context.clientSize());
	}

	template <class GL> inline	void	Display<GL>::capture(Image&target)
	{
		if (!framebuffer_bound)
		{
			if (resolution_overridden)
				target.setDimension(overridden_client_resolution.width,overridden_client_resolution.height,config.alpha_buffer_bits?4:3);
			else
				target.setDimension(context.clientWidth(),context.clientHeight(),config.alpha_buffer_bits?4:3);
		}
		else
			target.setDimension(target_buffer_resolution.width,target_buffer_resolution.height,framebuffer_alpha?4:3);
		GL::capture(target);
	}
	template <class GL> inline	void	Display<GL>::capture(FloatImage&target)
	{
		if (!framebuffer_bound)
		{
			if (resolution_overridden)
				target.setDimension(overridden_client_resolution.width,overridden_client_resolution.height,config.alpha_buffer_bits?4:3);
			else
				target.setDimension(context.clientWidth(),context.clientHeight(),config.alpha_buffer_bits?4:3);
		}
		else
			target.setDimension(target_buffer_resolution.width,target_buffer_resolution.height,framebuffer_alpha?4:3);
		GL::capture(target);
	}

	template <class GL> inline	void	Display<GL>::capture(typename GL::Texture&target)
	{
		Resolution res = currentRargetResolution();
		GL::capture(target,res.width,res.height);
	}

	template <class GL> inline	void	Display<GL>::captureDepth(typename GL::Texture&target)
	{
		Resolution res = currentRargetResolution();
		GL::captureDepth(target,res.width,res.height);
	}


	template <class GL>void Display<GL>::assign(pEngineExec target)
	{
	    exec_target = target;
	//    context.assign(target);
	}

	template <class GL>void Display<GL>::interruptCheckEvents()
	{
        #if SYSTEM==WINDOWS
            MSG msg;
            while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
            {
                if (!GetMessage(&msg, NULL,0,0))
                    FATAL__("event-handling error");
                TranslateMessage(&msg);
                process(msg);
//                    DispatchMessage(&msg);
            }
        #elif SYSTEM==UNIX
            XEvent event;
            while (!context.shutting_down && XPending(connection))
            {
                if (XNextEvent(connection,&event))
                    FATAL__("event-handling error");
                process(event);
            }
        #endif

	}

	template <class GL>void Display<GL>::execute()
	{
	    if (!exec_target)
	        return;
	    bool exec_loop = true;
	    #if SYSTEM==UNIX
	        Display*connection = context.connection();
	        if (!connection)
	            return;
	    #endif
		BYTE focus_check = 1;
	    while (exec_loop)
	    {
			if (!--focus_check)
			{
				focus_check = 5;
				if (mouse.isLocked())
					context.checkFocus();
			}
	        timing.update();
			if (context.isMinimized())
				Sleep(50.f);
			else
		        exec_loop = !context.shutting_down && exec_target();

	        mouse.update();	//set here to reset motion
			interruptCheckEvents();
	        GL::nextFrame();
	    }
	}

	template <class GL>void Display<GL>::executeNoClr()
	{
	    if (!exec_target)
	        return;
	    bool exec_loop = true;
	    #if SYSTEM==UNIX
	        Display*connection = context.connection();
	        if (!connection)
	            return;
	    #endif
		BYTE focus_check = 1;
	    while (exec_loop)
	    {
			if (!--focus_check)
			{
				focus_check = 5;
				if (mouse.isLocked())
					context.checkFocus();
			}
	        timing.update();
			if (context.isMinimized())
				Sleep(50.f);
			else
		        exec_loop = !context.shutting_down && exec_target();

	        mouse.update();

	        #if SYSTEM==WINDOWS
	            MSG msg;
	            while (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	            {
	                if (!GetMessage(&msg, NULL,0,0))
	                    FATAL__("event-handling error");
	                TranslateMessage(&msg);
	                process(msg);
	//                    DispatchMessage(&msg);
	            }
	        #elif SYSTEM==UNIX
	            XEvent event;
	            while (XPending(connection))
	            {
	                if (!XNextEvent(connection,&event))
	                    FATAL__("event-handling error");
	                process(event);
	            }
	        #endif

	        GL::nextFrameNoClr();
	    }
	}

	#if SYSTEM==WINDOWS
	    template <class GL>inline void Display<GL>::process(const MSG&msg)
	    {
	        if ((msg.message == WM_DESTROY || msg.message == WM_CLOSE || msg.message == WM_QUIT) && !context.shutting_down)
	            destroy();
	        else
	            DispatchMessage(&msg);
	    }

	#elif SYSTEM==UNIX

	    template <class GL>inline void Display<GL>::process(XEvent&event)
	    {
	        if (event.type == DestroyNotify && !context.shutting_down)
	            destroy();
	        else
	            Engine::ExecuteEvent(event);
	    }

	#endif

	template <class GL>void Display<GL>::destroy()
	{
	    unbindFrameBuffer();
	    GL::destroyContext();
	    context.close();
		mouse.release();
	}

	template <class GL>
		void Display<GL>::terminate()
		{
			destroy();
		}

	template <class GL> void Display<GL>::lockRegion()
	{
	    region_locked = true;
	}

	template <class GL> void Display<GL>::unlockRegion()
	{
	    region_locked = false;
	}

	/**
		@brief Multi-window specific method. Allows to transform a float viewport to the corresponding pixel-region.

		Uses the locally stored dimensions if a frame buffer object is currently bound, the specified client size otherwise

		@param rect Viewport to transform
		@param clientSize Size of the client viewport (in pixels) to use, if no frame buffer object is currently bound
	*/
	template <class GL> RECT Display<GL>::transformViewport(const TFloatRect&rect, const Resolution&client_size)
	{
		if (framebuffer_bound)
			return transform(rect);

	    RECT result;
	    result.left     = (LONG)(rect.x.min       *client_size.width);
	    result.right    = (LONG)(rect.x.max      *client_size.width);
	    result.top      = (LONG)((rect.y.max)      *client_size.height);
	    result.bottom   = (LONG)((rect.y.min)   *client_size.height);
	    return result;

	}

	template <class GL> RECT Display<GL>::transform(const TFloatRect&rect)
	{
		Resolution res = currentRargetResolution();

	    RECT result;
	    result.left     = (LONG)(rect.x.min		*res.width);
	    result.right    = (LONG)(rect.x.max		*res.width);
	    result.top      = (LONG)((rect.y.max)	*res.height);
	    result.bottom   = (LONG)((rect.y.min)	*res.height);
	    return result;
	}


	template <class GL> void Display<GL>::pickRegion(const TFloatRect&rect)
	{
	    GL::setRegion(transform(rect));
	}


	template <class GL>
	template <class C>void Display<GL>::pick(const Aspect<C>&aspect)
	{
	    if (!region_locked)
	        GL::setRegion(transform(aspect.region));
		if (GlobalAspectConfiguration::loadAsProjection)
		{
			TMatrix4<>	view_projection;
			Mat::mult(aspect.projection,aspect.view,view_projection);
			GL::loadModelview(Matrix<>::eye4);
			GL::loadProjection(view_projection);
		}
		else
		{
			GL::loadModelview(aspect.view);
			GL::loadProjection(aspect.projection);
		}
		GL::setDepthTest(aspect.depthTest);
		environment_matrix.x.xyz = aspect.viewInvert.x.xyz;
		environment_matrix.y.xyz = aspect.viewInvert.y.xyz;
		environment_matrix.z.xyz = aspect.viewInvert.z.xyz;
		camera_location = aspect.viewInvert.w.xyz;
	}

	template <class GL>
	template <class C>void Display<GL>::pickCentered(const Aspect<C>&aspect)
	{
	    if (!region_locked)
	        GL::setRegion(transform(aspect.region));
	    TMatrix4<C>   view;
		view.x = aspect.view.x;
		view.y = aspect.view.y;
		view.z = aspect.view.z;
	    Vec::def(view.w,0,0,0,1);

		if (GlobalAspectConfiguration::loadAsProjection)
		{
			TMatrix4<>	view_projection;
			Mat::mult(aspect.projection,view,view_projection);
			GL::loadModelview(Matrix<>::eye4);
			GL::loadProjection(view_projection);
		}
		else
		{
			GL::loadModelview(view);
			GL::loadProjection(aspect.projection);
		}
		GL::setDepthTest(aspect.depthTest);

		environment_matrix.x.xyz = aspect.viewInvert.x.xyz;
		environment_matrix.y.xyz = aspect.viewInvert.y.xyz;
		environment_matrix.z.xyz = aspect.viewInvert.z.xyz;
		camera_location = aspect.viewInvert.w.xyz;
	}

	template <class GL>
	bool Display<GL>::bindFrameBuffer(const typename GL::FBO&pobj)
	{
	    if (pobj.isEmpty())
		{
			unbindFrameBuffer();
	        return false;
		}
	    if (GL::bindFrameBufferObject(pobj))
	    {
			framebuffer_bound = true;
	        target_buffer_resolution = pobj.size();
			framebuffer_alpha = pobj.primaryHasAlpha();
	        //pixelAspect = current_target_resolution.aspect();
			//ShowMessage("pbuffer bound. region is "+String(region_size.x)+", "+String(region_size.y)+". aspect is "+String(pixelAspect));
	        return true;
	    }
	    unbindFrameBuffer();
	    return false;
	}


	template <class GL>
	void Display<GL>::unbindFrameBuffer()
	{
	    if (!framebuffer_bound)
			return;
		
	    framebuffer_bound = false;
	    GL::unbindFrameBufferObject(clientSize());
	    //current_target_resolution = window_client_resolution;
		//const RECT&window = context.windowLocation();
		/*GL::setRegion(rect(0,0,window.right-window.left,window.bottom-window.top));*/	//so if i set region here everything goes boom..... guess stranger things happen
		//ShowMessage("pbuffer unbound. region is "+String(region_size.x)+", "+String(region_size.y)+". aspect is "+String(pixelAspect));
	}


	template <class GL> SimpleGeometry					Display<GL>::pivot;
	template <class GL> SimpleGeometry					Display<GL>::omni;
	template <class GL> SimpleGeometry					Display<GL>::spot;
	template <class GL> SimpleGeometry					Display<GL>::direct;
	template <class GL> bool                        Display<GL>::pivot_defined=false;
	template <class GL> bool                        Display<GL>::lights_defined=false;

}

#endif
