/******************************************************************

Predefined image generators.


******************************************************************/




template <class Nature>
	void        igRadial(GenericImage<Nature>&target, const CGColor&inner, const CGColor&outer, bool use_cos)
	{
		typedef typename Nature::float_type_t	F;
		target.setChannels(4);
		UINT32 r = vmin(target.getWidth(),target.getHeight());
		F			a = sqr(target.getWidth()/2),
					b = sqr(target.getHeight()/2);
		for (UINT32 x = 0; x < target.getWidth(); x++)
			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				F   dist = clamped(sqrt(sqr(x-target.getWidth()/2)/a+sqr(y-target.getHeight()/2)/b),0,1);
				if (use_cos)
					dist = cosFactor(dist);
				F	idist(F(1)-dist),
					color[] = {(dist)*outer.red + (idist)*inner.red, (dist)*outer.green + (idist)*inner.green, (dist)*outer.blue + (idist)*inner.blue, (dist)*outer.alpha + (idist)*inner.alpha};
				target.set4f(x,y,color);
			}
	}

template <class Nature>
	void        igExtRadial(GenericImage<Nature>&target, const CGColor&inner, const CGColor&outer, bool smooth)
	{
		typedef typename Nature::float_type_t	F;
		TVec4<F> color;
		target.setChannels(4);
		F		a = sqr(target.getWidth()/2),
				b = sqr(target.getHeight()/2);
	
		for (UINT32 x = 0; x < target.getWidth(); x++)
			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				F   dist = sqrt(sqr(x-target.getWidth()/2)/a+sqr(y-target.getHeight()/2)/b);
				F	f = smooth?-cos(M_PI/(dist*5+1))/2.0+0.5:vmin(1/(dist+0.0001)/15,1.1);
				//;
				//f = f/15-0.1;
				f-=0.1;
				if (f < 0)
					f = 0;
				Vec::interpolate(outer,inner,f,color);
				Vec::clamp(color,0,1);
				target.set4f(x,y,color.v);
			}
	}


template <class Nature>
	static void generateCubeSide(GenericImage<Nature>&target, UINT32 resolution, char x0, char y0, char z0, char x1, char y1, char z1, BYTE x_axis, BYTE y_axis, BYTE ignored_axis)
	{
		typedef typename Nature::float_type_t	F;
		TVec3<F>	n0 = {x0,y0,z0},
					nd = {x1-x0, y1-y0, z1-z0};
		target.setDimension(resolution,resolution,3);
		target.setContentType(PixelType::ObjectSpaceNormal);
		for (UINT32 x = 0; x < resolution; x++)
			for (UINT32 y = 0; y < resolution; y++)
			{
				F		fx = (F)x/(resolution-1),
						fy = (F)y/(resolution-1);
				TVec3<F>	n;
				n.v[x_axis] = n0.v[x_axis]+fx*nd.v[x_axis];
				n.v[y_axis] = n0.v[y_axis]+fy*nd.v[y_axis];
				n.v[ignored_axis] = n0.v[ignored_axis];
				Vec::normalize0(n);
				target.setNormal(x,y,n);
			}
	}

template <class Nature>
	void		igNormalSphere(GenericImage<Nature> side[6], UINT32 resolution)
	{
		generateCubeSide(side[0],resolution,	1,1,1,		1,-1,-1,	2,1,0);	//positive x
		generateCubeSide(side[1],resolution,	-1,1,-1,	-1,-1,1,	2,1,0);	//negative x
	
		generateCubeSide(side[2],resolution,	-1,1,-1,	1,1,1,		0,2,1);	//positive y
		generateCubeSide(side[3],resolution,	-1,-1,1,	1,-1,-1,	0,2,1);	//negative y
	
		generateCubeSide(side[4],resolution,	-1,1,1,		1,-1,1,		0,1,2);	//positive z
		generateCubeSide(side[5],resolution,	1,1,-1,		-1,-1,-1,	0,1,2);	//negative z
	}


template <class Nature>
	void        igRing(GenericImage<Nature>&target, typename Nature::float_type_t width, const CGColor&outer, const CGColor&outer_ring, const CGColor&inner_ring, const CGColor&inner, bool use_cos)
	{
		typedef typename Nature::float_type_t	F;
		F r = vmin(target.getWidth(),target.getHeight());
		TVec4<F>	color;
		target.setChannels(4);
    
		for (UINT32 x = 0; x < target.getWidth(); x++)
			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				F dist = clamped(sqrt((F)sqr(x-target.getWidth()/2)+(F)sqr(y-target.getHeight()/2))/(r/2),0,1);
				if (dist >= 0.98)
					color = outer;
				else
					if (dist >= 0.95)
						Vec::interpolate(outer_ring,outer,(dist-0.95)/0.03,color);
					else
						if (dist >= 0.95-width)
							Vec::interpolate(inner_ring,outer_ring,(dist-0.95+width)/width,color);
						else
							if (dist >= 0.92-width)
								Vec::interpolate(inner,inner_ring,(dist-0.92+width)/0.03,color);
							else
								color = inner;
				target.set4f(x,y,color.v);
			}

	}

template <class Nature>
	void        igVertical(GenericImage<Nature>&target, const CGColor&top, const CGColor&center, const CGColor&bottom, bool use_cos)
	{
		typedef typename Nature::float_type_t	F;
		TVec4<F> c;
		target.setChannels(4);
		for (UINT32 x = 0; x < target.getWidth(); x++)
			for (UINT32 y = 0; y < target.getHeight()/2; y++)
			{
				F fc = (F)y/(F)target.getHeight()*2;
				if (use_cos)
					fc = cosFactor(fc);
				Vec::interpolate(bottom,center,fc,c);
				target.set4f(x,y,c.v);
			}
		for (UINT32 x = 0; x < target.getWidth(); x++)
			for (UINT32 y = target.getHeight()/2; y < target.getHeight(); y++)
			{
				F fc = (F)((F)y-target.getHeight()/2)/(F)target.getHeight()*2;
				if (use_cos)
					fc = cosFactor(fc);
				Vec::interpolate(center,top,fc,c);
				target.set4f(x,y,c.v);
			}
	}

template <class Nature>
	void        igHorizontal(GenericImage<Nature>&target, const CGColor&top, const CGColor&center, const CGColor&bottom, bool use_cos)
	{
		typedef typename Nature::float_type_t	F;
		TVec4<F> c;
		target.setChannels(4);
		for (UINT32 x = 0; x < target.getWidth()/2; x++)
			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				F fc = (F)x/(F)target.getWidth()*2;
				if (use_cos)
					fc = cosFactor(fc);
				Vec::interpolate(bottom,center,fc,c);
				target.set4f(x,y,c.v);
			}
        
		for (UINT32 x = target.getWidth()/2; x < target.getWidth(); x++)
			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				F fc = (F)((F)x-target.getWidth()/2)/(F)target.getWidth()*2;
				if (use_cos)
					fc = cosFactor(fc);
				Vec::interpolate(center,top,fc,c);
				target.set4f(x,y,c.v);
			}

	}

template <class Nature>
	void        igHorizontalCubic(GenericImage<Nature>&target, const CGColor&bottom, const CGColor&top)
	{
		igHorizontalCubic(target, bottom, top, 0, target.width());
	}
template <class Nature>
	void        igHorizontalCubic(GenericImage<Nature>&target, const CGColor&bottom, const CGColor&top, UINT begin, UINT width)
	{
		typedef typename Nature::float_type_t	F;
		TVec4<F> c;
		target.setChannels(4);
		for (UINT32 x = begin; x < begin + width; x++)
		{
			F fc = cubicStep<F, F, F>(x, begin, begin + width);
			//sin((F)(x-begin) / width * 2 * M_PI)*0.5 + 0.5;
			Vec::interpolate(bottom, top, fc, c);

			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				target.set4f(x, y, c.v);
			}
		}
	}

template <class Nature>
	void        igVerticalSinus(GenericImage<Nature>&target, const CGColor&bottom, const CGColor&top)
	{
		typedef typename Nature::float_type_t	F;
		TVec4<F> c;
		target.setChannels(4);
		for (UINT32 x = 0; x < target.getWidth(); x++)
			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				F fc = sin((F)y/target.getHeight()*2*M_PI)*0.5+0.5;
				Vec::interpolate(bottom,top,fc,c);
				target.set4f(x,y,c.v);
			}
	}

template <class Nature>
	void		igCubic(GenericImage<Nature>&target, typename Nature::float_type_t frame_width, const CGColor&outer, const CGColor&inner, bool use_cos)
	{
		typedef typename Nature::float_type_t	F;
		F		half_width = target.getWidth()/2,
				half_height = target.getHeight()/2;
		TVec4<F>	color;
	
		for (UINT32 x = 0; x < target.getWidth(); x++)
			for (UINT32 y = 0; y < target.getHeight(); y++)
			{
				F		fx = fabs(((F)x-half_width)/half_width),
						fy = fabs(((F)y-half_height)/half_height),
						fc = (std::max(fx,fy)+frame_width-1.0f)/frame_width;
				if (fc < 0)
					target.set4f(x,y,inner.v);
				else
				{
					if (use_cos)
						fc = cosFactor(fc);
					Vec::interpolate(inner,outer,fc,color);
					target.set4f(x,y,color.v);
				}
			}

	}

template <class Nature>
	void		igSolarStripe(GenericImage<Nature>&target, typename Nature::float_type_t center_radius, typename Nature::float_type_t axis_x, typename Nature::float_type_t axis_y, typename Nature::float_type_t angle_border, typename Nature::float_type_t intensity, const CGColor&inner_color, const CGColor&outer, typename Nature::float_type_t smoothing)
	{
		typedef typename Nature::float_type_t	F;
		target.setChannels(4);
		TVec2<F>axis = {axis_x,axis_y},
				normal = {axis_y,-axis_x};
		F		growth = sin(angle_border*M_PI/180);
		for (UINT32 x = 0; x < target.width(); x++)
			for (UINT32 y = 0; y < target.height(); y++)
			{
				TVec2<F> f = {	(F)x/(F)(target.width()-1)*2.0f-1.0f,
								(F)y/(F)(target.height()-1)*2.0f-1.0f};
				F alpha = 0;
				TVec4<F>	color;
				F len = Vec::dot(f);
				if (len < sqr(center_radius))
					alpha = 1;
				else
				{
					F distance = (fabs(Vec::dot(normal,f))-center_radius)-fabs(Vec::dot(axis,f))*growth;
					alpha = 1.0-cosStep(distance,0,smoothing);
				}
				alpha *= (1/(pow(len,1)*100+1));
				//alpha = clamped(alpha,0,1);
			
			
			
				alpha *= intensity;
			
				alpha = vmin(alpha,1);
			
				Vec::interpolate(outer,inner_color,alpha,color);
				target.set4f(x,y,color.v);
			
				/*
				BYTE*pixel = inner.get(x,y);
				_set(pixel,255);
				pixel[3] = (BYTE)(alpha*255.0f);*/
			}


	}

template <class Nature>
	void		igHexagon(GenericImage<Nature>&target, typename Nature::float_type_t smoothing, const CGColor&inner,const CGColor&border,const CGColor&outer)
	{
		typedef typename Nature::float_type_t	F;
		static const F	a0 = 0, a1 = M_PI/3.0, a2 = M_PI*2.0/3.0;
		static const TVec2<F>	axis0 = {sin(a0),cos(a0)},
								axis1 = {sin(a1),cos(a1)},
								axis2 = {sin(a2),cos(a2)};

		target.setChannels(4);
		for (UINT32 x = 0; x < target.width(); x++)
			for (UINT32 y = 0; y < target.height(); y++)
			{
				TVec2<F> f =	{(F)x/(F)(target.width()-1)*2.0f-1.0f,
								(F)y/(F)(target.height()-1)*2.0f-1.0f};
				F alpha = 0;
				TVec4<F>	color;
				alpha = vmax(vmax(fabs(Vec::dot(f,axis0)),fabs(Vec::dot(f,axis1))),fabs(Vec::dot(f,axis2)))*1.2*(1.0+smoothing*2);
				if (alpha <= 1)
					Vec::interpolate(inner,border,alpha,color);
				else
				{
					alpha = cosStep(alpha,1.0f,1.0f+smoothing);
					Vec::interpolate(border,outer,alpha,color);
				}
						//a1 = _dot2(f,axis1),
			
			
			
			
				Vec::clamp(color,0,1);
				target.set4f(x,y,color.v);
			}
	}

