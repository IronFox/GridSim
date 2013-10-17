#ifndef imageTplH
#define imageTplH




template <class Nature>
	template <typename Float>
		void		GenericImage<Nature>::sampleNormal(Float x0, Float y0, Float x1, Float y1, TVec3<Float>&target)		const
		{
			T	sampled[0x100];
			ImageTemplate<T>::sample(x0,y0,x1,y1,sampled);

			target.x = Nature::toNormalComponent(sampled[0]);
			target.y = Nature::toNormalComponent(sampled[1]);
			target.z = Nature::toNormalComponent(sampled[2]);

			Vec::normalize0(target);
		}

template <typename T>
	template <typename Float>
		void		ImageTemplate<T>::sample(Float x0, Float y0, Float x1, Float y1, T*out)		const
		{
			float   pwidth = Float(1)/Float(image_width),
					pheight = Float(1)/Float(image_height);
		/*    x0-=pwidth/2;
			y0-=pheight/2;
			x1-=pwidth/2;
			y1-=pheight/2;*/
			#ifdef COS_RESIZE
				float   xp0 = cosFactor(1-fmod(x0,pwidth)/pwidth),
						yp0 = cosFactor(1-fmod(y0,pheight)/pheight),
						xp1 = cosFactor(fmod(x1,pwidth)/pwidth),
						yp1 = cosFactor(fmod(y1,pheight)/pheight);
			#else
				float   xp0 = 1-fmod(x0,pwidth)/pwidth,
						yp0 = 1-fmod(y0,pheight)/pheight,
						xp1 = fmod(x1,pwidth)/pwidth,
						yp1 = fmod(y1,pheight)/pheight;
			#endif
			int     px0 = (int)(x0*(image_width)),
					py0 = (int)(y0*(image_height)),
					px1 = (int)(x1*(image_width)),
					py1 = (int)(y1*(image_height));

			if (px0 == px1)
			{
				px1++;
				if (xp0<GLOBAL_ERROR)
					xp0 = 1-xp1;
			}
			else
				if (x1-x0<=pwidth)
					xp1 = 1-xp0;
			if (py0 == py1)
			{
				py1++;
				if (yp0<GLOBAL_ERROR)
					yp0 = 1-yp1;
			}
			else
				if (y1-y0<=pheight)
					yp1 = 1-yp0;


			Float   count(0),target[0x100];
			Vec::clearD(target,image_channels);

			for (int x = px0+1; x < px1; x++)
				for (int y = py0+1; y < py1; y++)
					if (const T*pixel = getVerified(x,y))
					{
						Vec::addD(target,pixel,image_channels);
						count++;
					}
			for (int x = px0+1; x < px1; x++)
			{
				const T	*p0 = getVerified(x,py0),
						*p1 = getVerified(x,py1);
				for (BYTE k = 0; k < image_channels; k++)
				{
					if (p0)
						target[k] += yp0*p0[k];
					if (p1)
						target[k] += yp1*p1[k];
				}
				if (p0)
					count += yp0;
				if (p1)
					count += yp1;
			}
			for (int y = py0+1; y < py1; y++)
			{
				const T	*p0 = getVerified(px0,y),
						*p1 = getVerified(px1,y);
				for (BYTE k = 0; k < image_channels; k++)
				{
					if (p0)
						target[k] += xp0*p0[k];
					if (p1)
						target[k] += xp1*p1[k];
				}
				if (p0)
					count += xp0;
				if (p1)
					count += xp1;
			}
			const T	*p0 = getVerified(px0,py0),
					*p1 = getVerified(px0,py1),
					*p2 = getVerified(px1,py1),
					*p3 = getVerified(px1,py0);

			for (BYTE k = 0; k < image_channels; k++)
			{
				if (p0)
					target[k] += xp0*yp0*p0[k];
				if (p1)
					target[k] += xp0*yp1*p1[k];
				if (p2)
					target[k] += xp1*yp1*p2[k];
				if (p3)
					target[k] += xp1*yp0*p3[k];
			}
			if (p0)
				count += xp0*yp0;
			if (p1)
				count += xp0*yp1;
			if (p2)
				count += xp1*yp1;
			if (p3)
				count += xp1*yp0;
			Vec::divideD(target,count,image_channels);
			Vec::copyD(target,out,image_channels);

		}


template <typename T>
	ImageTemplate<T>::ImageTemplate():image_data(NULL)
	{}

template <typename T>
	ImageTemplate<T>::ImageTemplate(const ImageTemplate<T>&other):image_data(NULL)
	{
		setDimensions(other.image_width,other.image_height,other.image_channels);
		memcpy(image_data,other.image_data,other.size());
	}

template <typename T>
	ImageTemplate<T>::ImageTemplate(dimension_t width, dimension_t height, BYTE channels,PixelType type):image_data(NULL)
	{
		setDimensions(width,height,channels);
		content_type = type;
	}

template <typename T>
	ImageTemplate<T>::ImageTemplate(const THeader&header):BaseImage(0x1<<header.x_exp,0x1<<header.y_exp,header.channels,getContentType(header)),image_data(NULL)
	{
		alloc(image_data,image_width*image_height*image_channels);
	}

template <typename T>
	ImageTemplate<T>::~ImageTemplate()
	{
		dealloc(image_data);
	}

template <typename T>
	ImageTemplate<T>&		ImageTemplate<T>::operator=(const ImageTemplate<T>&other)
	{
		content_type = other.content_type;
		origin = other.origin;
		setDimensions(other.image_width,other.image_height,other.image_channels);
		memcpy(image_data,other.image_data,other.size());
		return *this;
	}



template <typename T>
	void		ImageTemplate<T>::applyHeader(const THeader&header)
	{
		setDimensions(0x1<<header.x_exp,0x1<<header.y_exp,header.channels);
		setContentType(getContentType(header));
	}

template <typename T>
	inline	T*			ImageTemplate<T>::getData()
	{
		return image_data;
	}

template <typename T>
	inline	const T*	ImageTemplate<T>::getData() const
	{
		return image_data;
	}

template <typename T>
	inline	T*			ImageTemplate<T>::data()
	{
		return image_data;
	}

template <typename T>
	inline	const T*	ImageTemplate<T>::data() const
	{
		return image_data;
	}

template <typename T>
	void		ImageTemplate<T>::adoptData(ImageTemplate<T>&other)
	{
		dealloc(image_data);
	
		content_type = other.content_type;
	
		origin.adoptData(other.origin);
	
		image_width = other.image_width;
		image_height = other.image_height;
		image_channels = other.image_channels;
		image_data = other.image_data;
	
		other.image_width = 0;
		other.image_height = 0;
		other.image_data = NULL;
	}
template <typename T>
	void		ImageTemplate<T>::swap(ImageTemplate<T>&other)
	{
		origin.swap(other.origin);
	
		swp(content_type,other.content_type);
		swp(image_width,other.image_width);
		swp(image_height,other.image_height);
		swp(image_channels,other.image_channels);
		swp(image_data,other.image_data);
	}

template <typename T>
	void		ImageTemplate<T>::setDimensions(dimension_t width, dimension_t height, BYTE channels)
	{
		if (width*height*channels != image_width*image_height*image_channels)
			re_alloc(image_data,width*height*channels);
		image_width = width;
		image_height = height;
		image_channels = channels;
	}

template <typename T>
	void		ImageTemplate<T>::setDimension(dimension_t width, dimension_t height, BYTE channels)
	{
		setDimensions(width,height,channels);
	}

template <typename T>
	void		ImageTemplate<T>::setSize(dimension_t width, dimension_t height, BYTE channels)
	{
		setDimensions(width,height,channels);
	}

template <typename T>
	inline void		ImageTemplate<T>::setChannel(dimension_t X, dimension_t Y, BYTE channel, T newData)
	{
		image_data[Y*image_width*image_channels+X*image_channels+channel] = newData;
	}

template <typename T>
	void		ImageTemplate<T>::setChannel(BYTE channel, T new_value)
	{
		if (channel >= image_channels)
			return;
		Concurrency::parallel_for(dimension_t(0),image_width,[this,channel,new_value](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
				setChannel(x,y,channel,new_value);
		});
	}

template <typename T>
	void		ImageTemplate<T>::paintRect(dimension_t left, dimension_t bottom, dimension_t width, dimension_t height, T r)
	{
		if (image_channels != 1)
			return;
		for (dimension_t x = left; x < left+width; x++)
			for (dimension_t y = bottom; y < bottom + height; y++)
			{
				if (x < image_width && y < image_height)
				{
					T*pixel = get(x,y);
					pixel[0] = r;
				}
			}
	}

template <typename T>
	void		ImageTemplate<T>::paintRect(dimension_t left, dimension_t bottom, dimension_t width, dimension_t height, T r, T g)
	{
		switch (image_channels)
		{
			case 1:
				paintRect(left,bottom,width,height,r);
			break;
		}
		if (image_channels != 2)
			return;
		for (dimension_t x = left; x < left+width; x++)
			for (dimension_t y = bottom; y < bottom + height; y++)
			{
				if (x < image_width && y < image_height)
				{
					T*pixel = get(x,y);
					pixel[0] = r;
					pixel[1] = g;
				}
			}
	}


template <typename T>
	void		ImageTemplate<T>::paintRect(dimension_t left, dimension_t bottom, dimension_t width, dimension_t height, T r, T g, T b)
	{
		switch (image_channels)
		{
			case 1:
				paintRect(left,bottom,width,height,r);
			break;
			case 2:
				paintRect(left,bottom,width,height,r,g);
			break;
		}
		if (image_channels != 3)
			return;
		for (dimension_t x = left; x < left+width; x++)
			for (dimension_t y = bottom; y < bottom + height; y++)
			{
				if (x < image_width && y < image_height)
				{
					T*pixel = get(x,y);
					pixel[0] = r;
					pixel[1] = g;
					pixel[2] = b;
				}
			}

	}

template <typename T>
	void		ImageTemplate<T>::paintRect(dimension_t left, dimension_t bottom, dimension_t width, dimension_t height, T r, T g, T b, T a)
	{
		switch (image_channels)
		{
			case 1:
				paintRect(left,bottom,width,height,r);
			break;
			case 2:
				paintRect(left,bottom,width,height,r,g);
			break;
			case 3:
				paintRect(left,bottom,width,height,r,g,b);
			break;
		}
		if (image_channels != 4)
			return;
		for (dimension_t x = left; x < left+width; x++)
			for (dimension_t y = bottom; y < bottom + height; y++)
			{
				if (x < image_width && y < image_height)
				{
					T*pixel = get(x,y);
					pixel[0] = r;
					pixel[1] = g;
					pixel[2] = b;
					pixel[3] = a;
				}
			}

	}
template <class Nature>
	void		GenericImage<Nature>::addRect(dimension_t left, dimension_t bottom, dimension_t width, dimension_t height, T r, T g, T b)
	{
		if (image_channels != 3)
			return;
		Concurrency::parallel_for(left,left+width,[this,bottom,height,r,g,b](dimension_t x)
		{
			for (dimension_t y = bottom; y < bottom + height; y++)
			{
				if (x < image_width && y < image_height)
				{
					T*pixel = this->get(x,y);
					
					pixel[0] = GenericImage<Nature>::ImageNature::clamp((F)pixel[0] + r);
					pixel[1] = GenericImage<Nature>::ImageNature::clamp((F)pixel[1] + g);
					pixel[2] = GenericImage<Nature>::ImageNature::clamp((F)pixel[2] + b);
				}
			}
		});
	}


template <typename T>
	void		ImageTemplate<T>::noiseFillChannel(BYTE channel, T min_value, T max_value)
	{
		noiseFillChannel(channel,min_value,max_value,rand());
	}
template <typename T>
	void		ImageTemplate<T>::noiseFillChannel(BYTE channel, T min_value, T max_value, unsigned seed)
	{
		if (channel >= image_channels)
			return;
		Concurrency::parallel_for(dimension_t(0),image_width,[this,channel,seed,min_value,max_value](dimension_t x)
		{
			ImageRandomSource rnd((unsigned long)(seed+x*29));
			for (dimension_t y = 0; y < image_height; y++)
			{
				T value = min_value+T(rnd()*(max_value-min_value));
				setChannel(x,y,channel,value);
			}
		});
	}



template <typename T>
	void		ImageTemplate<T>::blurChannel(BYTE channel, float radius, bool loop)
	{
		if (channel >= image_channels || radius <= 0)
			return;
		Array2D<T>	channels(image_width,image_height);
	
		int steps = (int)ceil(radius);
		Concurrency::parallel_for(INT32(0),INT32(image_width),[this,steps,loop,radius,channel,&channels](INT32 x)
		{
			for (INT32 y = 0; y < (INT32)image_height; y++)
			{
				float sum = 0;
				float counter = 0;
				for (INT32 ix = -steps; ix <= steps; ix++)
					for (INT32 iy = -steps; iy <= steps; iy++)
					{
						INT32 ax = x+ix;
						INT32 ay = y+iy;
						if (!loop)
						{
							if (ax < 0 || ay < 0 || ax >= (INT32)image_width || ay >= (INT32)image_height)
								continue;
						}
						else
						{
							while (ax < 0)
								ax += image_width;
							while (ay < 0)
								ay += image_height;
							while (ax >= (INT32)image_width)
								ax -= image_width;
							while (ay >= (INT32)image_height)
								ay -= image_height;
						}
					
				
				
						float intensity = 1.0f-(float)((float)ix*ix+(float)iy*iy)/(radius*radius);
					
						if (intensity <= 0)
							continue;
					
						counter += intensity;
						sum += intensity*(float)getPixel(ax,ay)[channel];
					}
				ASSERT__(counter > 0);
				channels.get(x,y) = T(sum/counter);
			}
		});
		Concurrency::parallel_for(dimension_t(0),image_width,[this,&channels,channel](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
				getPixel(x,y)[channel] = channels.get(x,y);
		});
	}


template <typename T>
	void		ImageTemplate<T>::linearBlurChannel(BYTE channel, float radius, bool loop)
	{
		if (channel >= image_channels || radius <= 0)
			return;
		Array2D<T>	channels(image_width,image_height);
	
		int steps = (int)ceil(radius);
		Concurrency::parallel_for(INT32(0),INT32(image_width),[this,steps,loop,radius,channel,&channels](INT32 x)
		{
			for (INT32 y = 0; y < (INT32)image_height; y++)
			{
				float sum = 0;
				float counter = 0;
				for (INT32 ix = -steps; ix <= steps; ix++)
					{
						INT32 ax = x+ix;
						if (!loop)
						{
							if (ax < 0 || ax >= (INT32)image_width)
								continue;
						}
						else
						{
							while (ax < 0)
								ax += image_width;
							ax %= (INT32)image_width;
						}
					
						float intensity = 1.0f-(float)((float)ix)/(radius);
					
						if (intensity <= 0)
							continue;
					
						counter += intensity;
						sum += intensity*(float)getPixel(ax,y)[channel];
					}
				ASSERT__(counter > 0);
				channels.get(x,y) = T(sum/counter);
			}
		});
		Concurrency::parallel_for(INT32(0),INT32(image_width),[this,steps,loop,radius,channel,&channels](INT32 x)
		{
			for (INT32 y = 0; y < (INT32)image_height; y++)
			{
				float sum = 0;
				float counter = 0;
				for (INT32 iy = -steps; iy <= steps; iy++)
					{
						INT32 ay = y+iy;
						if (!loop)
						{
							if (ay < 0 || ay >= (INT32)image_height)
								continue;
						}
						else
						{
							while (ay < 0)
								ay += image_height;
							ay %= (INT32)image_height;
						}
					
						float intensity = 1.0f-(float)((float)iy)/(radius);
					
						if (intensity <= 0)
							continue;
					
						counter += intensity;
						sum += intensity*(float)channels.get(x,ay);
					}
				ASSERT__(counter > 0);
				getPixel(x,y)[channel] = T(sum/counter);
			}
		});
	}


template <typename T>
	void		ImageTemplate<T>::fill(T red, T green, T blue, T alpha)
	{
		size_t pixels = size_t(image_width)*size_t(image_height);
		switch (image_channels)
		{
			case 0:
			break;
			case 1:
				Concurrency::parallel_for(index_t(0),pixels,[this,red](index_t i)
				{
					image_data[i] = red;
				});
			break;
			case 2:
				Concurrency::parallel_for(index_t(0),pixels,[this,red,green](index_t i)
				{
					image_data[i*2] = red;
					image_data[i*2+1] = green;
				});
			break;
			case 3:
				Concurrency::parallel_for(index_t(0),pixels,[this,red,green,blue](index_t i)
				{
					image_data[i*3] = red;
					image_data[i*3+1] = green;
					image_data[i*3+2] = blue;
				});
			break;
			case 4:
				Concurrency::parallel_for(index_t(0),pixels,[this,red,green,blue,alpha](index_t i)
				{
					image_data[i*4] = red;
					image_data[i*4+1] = green;
					image_data[i*4+2] = blue;
					image_data[i*4+3] = alpha;
				});
			break;
			default:
				Concurrency::parallel_for(index_t(0),pixels,[this,red,green,blue,alpha](index_t i)
				{
					image_data[i*image_channels] = red;
					image_data[i*image_channels+1] = green;
					image_data[i*image_channels+2] = blue;
					image_data[i*image_channels+3] = alpha;
				});
			break;
		}
	}

template <class Nature>
	void		GenericImage<Nature>::setChannels(BYTE new_channels,BaseImage::channel_append_t append)
	{
		if (!new_channels || new_channels==image_channels)
			return;
		if (!image_channels && append == BaseImage::CA_REPEAT)
			append = BaseImage::CA_UNDEF;
		T*buffer = alloc<T>(size_t(image_width)*size_t(image_height)*size_t(new_channels));
		Concurrency::parallel_for(dimension_t(0),image_width,[this,buffer,new_channels](dimension_t x)
		{
			typedef GenericImage<Nature>::Size Size;
			for (dimension_t y = 0; y < image_height; y++)
			{
			 //   BYTE*old = getPixel(x,y);
				T*target = &buffer[Size(y)*Size(image_width)*Size(new_channels)+Size(x)*Size(new_channels)];
				Vec::copyD(getPixel(x,y),target,vmin(image_channels,new_channels));
			};
		});
		T fixed_value = 0;
		switch (append)
		{
				case BaseImage::CA_ZERO:
					fixed_value = 0;
				break;
				case BaseImage::CA_MAX:
					fixed_value = max;
				break;
				case BaseImage::CA_HALF:
					fixed_value = max/2;
				break;
		}


		if (new_channels > image_channels)
			switch (append)
			{
				case BaseImage::CA_REPEAT:
				{
					Concurrency::parallel_for(dimension_t(0),image_width,[this,buffer,new_channels](dimension_t x)
					{
						typedef GenericImage<Nature>::Size Size;
						for (dimension_t y = 0; y < image_height; y++)
							Vec::setD(&buffer[(Size(y)*Size(image_width)+Size(x))*Size(new_channels)+Size(image_channels)],this->getPixel(x,y)[image_channels-1],new_channels-image_channels);
					});
				}
				break;
				case BaseImage::CA_ZERO:
				case BaseImage::CA_MAX:
				case BaseImage::CA_HALF:
					Concurrency::parallel_for(dimension_t(0),image_width,[this,buffer,new_channels,fixed_value](dimension_t x)
					{
						typedef GenericImage<Nature>::Size Size;
						for (dimension_t y = 0; y < image_height; y++)
							Vec::setD(&buffer[(Size(y)*Size(image_width)+Size(x))*Size(new_channels)+Size(image_channels)],fixed_value,new_channels-image_channels);
					});
				break;
				default:
				break;
			}
		dealloc(image_data);
		image_data = buffer;
		image_channels = new_channels;
	}

template <typename T>
	inline void		ImageTemplate<T>::set3(dimension_t X, dimension_t Y, const T*data)
	{
		T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
		pixel[0] = data[0];
		pixel[1] = data[1];
		pixel[2] = data[2];
	}

template <typename T>
	inline void		ImageTemplate<T>::set(dimension_t X, dimension_t Y, T red, T green, T blue)
	{
		T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
		switch (image_channels)
		{
			case 0:
			break;
			case 1:
				pixel[0] = red;
			break;
			case 2:
				pixel[0] = red;
				pixel[1] = green;
			break;
			default:
				pixel[0] = red;
				pixel[1] = green;
				pixel[2] = blue;
			break;
		}
	}

template <typename T>
	inline void		ImageTemplate<T>::set(dimension_t X, dimension_t Y, T red, T green, T blue, T alpha)
	{
		T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
		switch (image_channels)
		{
			case 0:
			break;
			case 1:
				pixel[0] = red;
			break;
			case 2:
				pixel[0] = red;
				pixel[1] = green;
			break;
			case 3:
				pixel[0] = red;
				pixel[1] = green;
				pixel[2] = blue;
			break;
			default:
				pixel[0] = red;
				pixel[1] = green;
				pixel[2] = blue;
				pixel[3] = alpha;
			break;

		}
	}




template <typename T>
	inline void		ImageTemplate<T>::set4(dimension_t X, dimension_t Y, const T*data)
	{
		T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
		pixel[0] = data[0];
		pixel[1] = data[1];
		pixel[2] = data[2];
		pixel[3] = data[3];
	}
	
template <typename T>
	inline void		ImageTemplate<T>::set(dimension_t X, dimension_t Y, const T*data)
	{
		if (!image_data)
			return;
		T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
		for (BYTE i = 0; i < image_channels; i++)
			pixel[i] = data[i];
	}


template <typename T>
	inline void		ImageTemplate<T>::set(dimension_t X, dimension_t Y, const T*data, BYTE channels)
	{
		if (!image_data || !channels)
			return;
		index_t offset = (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
		for (BYTE k = 0; k < channels; k++)
			image_data[offset+k] = data[k];
		for (BYTE k = channels; k < image_channels; k++)
			image_data[offset+k] = data[channels-1];
	}











template <class Nature>
	template <typename Float>
		inline void		GenericImage<Nature>::set3f(dimension_t X, dimension_t Y, const Float data[3])
		{
			T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
			pixel[0] = Nature::floatToChannel(data[0]);
			pixel[1] = Nature::floatToChannel(data[1]);
			pixel[2] = Nature::floatToChannel(data[2]);
		}

template <class Nature>
	template <typename Float>
		inline void		GenericImage<Nature>::setf(dimension_t X, dimension_t Y, Float red, Float green, Float blue)
		{
			T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
			switch (image_channels)
			{
				case 0:
				break;
				case 1:
					pixel[0] = Nature::floatToChannel(red);
				break;
				case 2:
					pixel[0] = Nature::floatToChannel(red);
					pixel[1] = Nature::floatToChannel(green);
				break;
				default:
					pixel[0] = Nature::floatToChannel(red);
					pixel[1] = Nature::floatToChannel(green);
					pixel[2] = Nature::floatToChannel(blue);
				break;
			}
		}

template <class Nature>
	template <typename Float>
		inline void		GenericImage<Nature>::setf(dimension_t X, dimension_t Y, Float red, Float green, Float blue, Float alpha)
		{
			T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
			switch (image_channels)
			{
				case 0:
				break;
				case 1:
					pixel[0] = Nature::floatToChannel(red);
				break;
				case 2:
					pixel[0] = Nature::floatToChannel(red);
					pixel[1] = Nature::floatToChannel(green);
				break;
				case 3:
					pixel[0] = Nature::floatToChannel(red);
					pixel[1] = Nature::floatToChannel(green);
					pixel[2] = Nature::floatToChannel(blue);
				break;
				default:
					pixel[0] = Nature::floatToChannel(red);
					pixel[1] = Nature::floatToChannel(green);
					pixel[2] = Nature::floatToChannel(blue);
					pixel[3] = Nature::floatToChannel(alpha);
				break;

			}
		}




template <class Nature>
	template <typename Float>
		inline void		GenericImage<Nature>::set4f(dimension_t X, dimension_t Y, const Float data[4])
		{
			T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
			pixel[0] = Nature::floatToChannel(data[0]);
			pixel[1] = Nature::floatToChannel(data[1]);
			pixel[2] = Nature::floatToChannel(data[2]);
			pixel[3] = Nature::floatToChannel(data[3]);
		}
	
template <class Nature>
	template <typename Float>
		inline void		GenericImage<Nature>::setf(dimension_t X, dimension_t Y, const Float*data)
		{
			if (!image_data)
				return;
			T*pixel = image_data + (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
			for (BYTE i = 0; i < image_channels; i++)
				pixel[i] = Nature::floatToChannel(data[i]);
		}


template <class Nature>
	template <typename Float>
		inline void		GenericImage<Nature>::setf(dimension_t X, dimension_t Y, const Float*data, BYTE channels)
		{
			if (!image_data || !channels)
				return;
			index_t offset = (size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
			for (BYTE k = 0; k < channels; k++)
				image_data[offset+k] = Nature::floatToChannel(data[k]);
			for (BYTE k = channels; k < image_channels; k++)
				image_data[offset+k] = Nature::floatToChannel(data[channels-1]);
		}

template <typename T>
	void			ImageTemplate<T>::copyChannel(const ImageTemplate<T>&source_image, BYTE source_channel, BYTE target_channel)
	{
		if (source_image.width() != width() || source_image.height() != height() || source_channel >= source_image.channels() || target_channel >= channels())
		{
			FATAL__("Copy operation cannot be performed due to image incompatibility or parameter invalidity");
			return;
		}
		Concurrency::parallel_for(dimension_t(0),image_width,[this,&source_image,source_channel,target_channel](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
			{
				this->get(x,y)[target_channel] = source_image.get(x,y)[source_channel];
			};
		});
	}

template <typename T>
	T				ImageTemplate<T>::sampleChannelAt(float x, float y, BYTE channel, bool loop)	const
	{
		if (!image_width || !image_height || channel >= image_channels)
			return 0;
		if (loop)
		{
			x = x - floor(x),
				//fmod(x,1.f);
			y = y - floor(y);
			//fmod(y,1.f);
			x*=(image_width);
			y*=(image_height);
		}
		else
		{
			x = clamped(x,0.f,1.f);
			y = clamped(y,0.f,1.f);
			x*=(image_width-1);
			y*=(image_height-1);
		}
		dimension_t x0 = (dimension_t)floor(x),
					y0 = (dimension_t)floor(y),
					x1,
					y1;

		if (loop)
		{
			x1 = (x0+1) % image_width;
			y1 = (y0+1) % image_height;
		}
		else
		{
			x1 = std::min(x0+1,image_width-1);
			y1 = std::min(y0+1,image_height-1);
		}
		float	xf = x - floor(x),
					//fmod(x,1.f),
				yf = y - floor(y);
				//fmod(y,1.f);
		float	s00 = (float)get(x0,y0)[channel],
				s10 = (float)get(x1,y0)[channel],
				s11 = (float)get(x1,y1)[channel],
				s01 = (float)get(x0,y1)[channel],
				i0 = s00 * (1.f - xf) + s10 * xf,
				i1 = s01 * (1.f - xf) + s11 * xf;
		//ShowMessage(String(s00)+"; "+String(s10)+"; "+String(s11)+"; "+String(s01));
		return static_cast<T>(i0 * (1.f - yf) + i1 * yf);
	}


template <typename T>
	T				ImageTemplate<T>::smoothSampleChannelAt(float x, float y, BYTE channel, bool loop)	const
	{
		if (!image_width || !image_height || channel >= image_channels)
			return 0;
		if (loop)
		{
			x = x - floor(x),
				//fmod(x,1.f);
			y = y - floor(y);
			//fmod(y,1.f);
			x*=(image_width);
			y*=(image_height);
		}
		else
		{
			x = clamped(x,0.f,1.f);
			y = clamped(y,0.f,1.f);
			x*=(image_width-1);
			y*=(image_height-1);
		}
		dimension_t x0 = (dimension_t)floor(x),
					y0 = (dimension_t)floor(y),
					x1,
					y1;

		if (loop)
		{
			x1 = (x0+1) % image_width;
			y1 = (y0+1) % image_height;
		}
		else
		{
			x1 = std::min(x0+1,image_width-1);
			y1 = std::min(y0+1,image_height-1);
		}
		float	xf = cubicFactor(x - floor(x)),
					//fmod(x,1.f),
				yf = cubicFactor(y - floor(y));
				//fmod(y,1.f);
		float	s00 = (float)get(x0,y0)[channel],
				s10 = (float)get(x1,y0)[channel],
				s11 = (float)get(x1,y1)[channel],
				s01 = (float)get(x0,y1)[channel],
				i0 = s00 * (1.f - xf) + s10 * xf,
				i1 = s01 * (1.f - xf) + s11 * xf;
		//ShowMessage(String(s00)+"; "+String(s10)+"; "+String(s11)+"; "+String(s01));
		return static_cast<T>(i0 * (1.f - yf) + i1 * yf);
	}



template <typename T>
	inline T*		ImageTemplate<T>::getVerified(dimension_t X, dimension_t Y)
	{
		if (X < image_width && Y < image_height && image_channels)
			return getPixel(X,Y);
		return NULL;
	}

template <typename T>
	inline const T*		ImageTemplate<T>::getVerified(dimension_t X, dimension_t Y) const
	{
		if (X < image_width && Y < image_height && image_channels)
			return getPixel(X,Y);
		return NULL;
	}


template <typename T>
	void		ImageTemplate<T>::swapChannels(BYTE c0, BYTE c1)
	{
		if (c0 == c1 || c0 >= image_channels || c1 >= image_channels)
			return;
		typedef size_t	size_type;
		size_t pixels = size_t(image_width)*size_t(image_height);
		Concurrency::parallel_for(index_t(0),pixels,[this,c0,c1](index_t i)
		{
			swp(
				image_data[i*size_type(image_channels)+size_type(c0)],
				image_data[i*size_type(image_channels)+size_type(c1)]
			);
		});
	}


template <typename T>
	void		ImageTemplate<T>::appendAlpha(const ImageTemplate<T>*other)
	{
		if (!other || !other->image_channels || other->image_width != image_width || other->image_height != image_height)
			return;
		typedef size_t	size_type;
		if (image_channels != 4)
		{
			T*buffer = alloc<T>(size_t(image_width)*size_t(image_height)*size_t(4));
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&buffer](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
				{
				 //   BYTE*old = getPixel(x,y);
					Vec::copyD(this->getPixel(x,y),&buffer[size_type(y)*size_type(image_width)*size_type(4)+size_type(x)*size_type(4)],vmin(image_channels,4));
				};
			});

			dealloc(image_data);
			image_data = buffer;
			image_channels = 4;
		}

		Concurrency::parallel_for(dimension_t(0),image_width,[this,&other](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
				setChannel(x,y,3,other->getPixel(x,y)[0]);
		});
	}

template <typename T>
	void		ImageTemplate<T>::appendAlphaAndDelete(ImageTemplate<T>*other)
	{
		appendAlpha(other);
		if (other)
			DISCARD(other);
	}

template <typename T>
	inline size_t ImageTemplate<T>::size()																const
	{
		return (size_t)image_width*(size_t)image_height*(size_t)image_channels*sizeof(T);
	}

template <class Nature>
	void	GenericImage<Nature>::changeSaturation(float scale_by)
	{
		if (!isColorMap() || image_channels < 3)
			return;
		Concurrency::parallel_for(dimension_t(0),image_height,[this,scale_by](dimension_t y)
		{
			for (dimension_t x = 0; x < image_width; x++)
			{
				T	*pixel = getPixel(x,y);
				F	avg = (static_cast<F>(pixel[0]) + static_cast<F>(pixel[1]) + static_cast<F>(pixel[2]))/static_cast<F>(3);
				pixel[0] = static_cast<T>(avg + (pixel[0] - avg)*scale_by);
				pixel[1] = static_cast<T>(avg + (pixel[1] - avg)*scale_by);
				pixel[2] = static_cast<T>(avg + (pixel[2] - avg)*scale_by);
			}
		});
	}

template <class Nature>
	void	GenericImage<Nature>::power(BYTE channel, float exponent)
	{
		if (!isColorMap())
			return;
		Concurrency::parallel_for(dimension_t(0),image_height,[this,exponent,channel](dimension_t y)
		{
			for (dimension_t x = 0; x < image_width; x++)
			{
				F	val = pow(static_cast<F>(getPixel(x,y)[channel]) / static_cast<F>(max),exponent);
				getPixel(x,y)[channel] = static_cast<T>(val * static_cast<F>(max));
			}
		});

	}



template <class Nature>
	void GenericImage<Nature>::flipHorizontal()
	{
		if (isNormalMap() && image_channels >= 3)
		{
			Concurrency::parallel_for(dimension_t(0),image_height,[this](dimension_t y)
			{
				TVec3<F> n0,n1;
				for (dimension_t x = 0; x < image_width/2; x++)
				{
					getNormal(x,y,n0);
					getNormal(image_width-x-1,y,n1);
					n0.x *= -1;
					n1.x *= -1;
					setNormal(x,y,n1);
					setNormal(image_width-x-1,y,n0);
				}
			});
		
			if (image_channels > 3)
			{
				BYTE range = image_channels-3;
				Concurrency::parallel_for(dimension_t(0),image_height,[this,range](dimension_t y)
				{
					T buffer[0x100];
					for (dimension_t x = 0; x < image_width/2; x++)
					{
						T	*lower = getPixel(x,y)+3,
							*upper = getPixel(image_width-x-1,y)+3;
						Vec::copyD(lower,buffer,range);
						Vec::copyD(upper,lower,range);
						Vec::copyD(buffer,upper,range);
					}
				});
			}			
		}
		else
		{
			Concurrency::parallel_for(dimension_t(0),image_height,[this](dimension_t y)
			{
				T buffer[0x100];
				for (dimension_t x = 0; x < image_width/2; x++)
				{
					Vec::copyD(getPixel(x,y),buffer,image_channels);
					set(x,y,getPixel(image_width-x-1,y));
					set(image_width-x-1,y,buffer);
				}
			});
		}
	}



template <class Nature>
	void GenericImage<Nature>::flipVertical()
	{
		if (isNormalMap() && image_channels >= 3)
		{
			Concurrency::parallel_for(dimension_t(0),image_width,[this](dimension_t x)
			{
				TVec3<F> n0,n1;
				for (dimension_t y = 0; y < image_height/2; y++)
				{
					getNormal(x,y,n0);
					getNormal(x,image_height-y-1,n1);
					n0.y *= -1;
					n1.y *= -1;
					setNormal(x,y,n1);
					setNormal(x,image_height-y-1,n0);
				}
			});
			if (image_channels > 3)
			{
				BYTE range = image_channels-3;
				Concurrency::parallel_for(dimension_t(0),image_width,[this,range](dimension_t x)
				{
					T buffer[0x100];
					for (dimension_t y = 0; y < image_height/2; y++)
					{
						T	*lower = getPixel(x,y)+3,
							*upper = getPixel(x,image_height-y-1)+3;
						Vec::copyD(lower,buffer,range);
						Vec::copyD(upper,lower,range);
						Vec::copyD(buffer,upper,range);
					}
				});
			}
		}
		else
		{
			Concurrency::parallel_for(dimension_t(0),image_width,[this](dimension_t x)
			{
				T buffer[0x100];
				for (dimension_t y = 0; y < image_height/2; y++)
				{
					Vec::copyD(getPixel(x,y),buffer,image_channels);
					set(x,y,getPixel(x,image_height-y-1));
					set(x,image_height-y-1,buffer);
				}
			});
		}
	}


template <class Nature>
	void GenericImage<Nature>::rotate90CW()
	{
		GenericImage<Nature>	temp(image_height,image_width,image_channels);
		temp.content_type = content_type;
		temp.origin.adoptData(origin);



		if (isNormalMap())
		{
			Concurrency::parallel_for(dimension_t(0),image_height,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_width; y++)
				{
					TVec3<F> n;
					getNormal(image_width-y-1,x,n);
					swp(n.x,n.y);
					n.y *= -1;
					temp.setNormal(x,y,n);
				}
			});
		}
		else
			Concurrency::parallel_for(dimension_t(0),image_height,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_width; y++)
					temp.set(x,y,getPixel(image_width-y-1,x));
			});

		adoptData(temp);
	}

template <class Nature>
	void GenericImage<Nature>::rotate90CCW()
	{
		GenericImage<Nature>	temp(image_height,image_width,image_channels);
		temp.content_type = content_type;
		temp.origin.adoptData(origin);

		if (isNormalMap())
		{
			Concurrency::parallel_for(dimension_t(0),image_height,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_width; y++)
				{
					TVec3<F> n;
					getNormal(image_width-y-1,x,n);
					swp(n.x,n.y);
					n.x *= -1;
					temp.setNormal(x,y,n);
				}
			});
		}
		else
			Concurrency::parallel_for(dimension_t(0),image_height,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_width; y++)
					temp.set(x,y,getPixel(y,image_height-x-1));
			});

		adoptData(temp);
	}








template <class Nature>
	bool GenericImage<Nature>::scaleHalf()
	{
		if (image_width<2 || image_height<2)
			return false;
		GenericImage<Nature>	temp(image_width/2,image_height/2,image_channels);
		temp.content_type = content_type;
		temp.origin.adoptData(origin);
		if (! temp.scaleHalf(*this))
			return false;
		adoptData(temp);
		return true;
	}

template <class Nature>
	bool GenericImage<Nature>::scaleHalf(const Self&source)
	{
		setDimension(source.getWidth()/2,source.getHeight()/2,source.getChannels());
		if (image_channels == 4)
		{
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&source](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
					if (isNormalMap())
					{
						TVec3<F> norm0,norm1,norm2,norm3;
						source.getNormal(x*2,y*2,norm0);
						source.getNormal(x*2+1,y*2,norm1);
						source.getNormal(x*2+1,y*2+1,norm2);
						source.getNormal(x*2,y*2+1,norm3);
						F a0 = toFloat(source.getPixel(x*2,y*2)[3]);
						F a1 = toFloat(source.getPixel(x*2+1,y*2)[3]);
						F a2 = toFloat(source.getPixel(x*2+1,y*2+1)[3]);
						F a3 = toFloat(source.getPixel(x*2,y*2+1)[3]);
						F aSum = a0 + a1 + a2 + a3;
						if (aSum > 0)
						{
							Vec::mult(norm0,a0);
							Vec::mad(norm0,norm1,a1);
							Vec::mad(norm0,norm2,a2);
							Vec::mad(norm0,norm3,a3);
							Vec::div(norm0,aSum);
							Vec::normalize0(norm0);
							this->setNormal(x,y,norm0);
							this->getPixel(x,y)[3] = floatToChannel(aSum/(F)4);
						}
						else
						{
							Vec::add(norm0,norm1);
							Vec::add(norm0,norm2);
							Vec::add(norm0,norm3);
							Vec::normalize0(norm0);
							this->setNormal(x,y,norm0);
							this->getPixel(x,y)[3] = 0;
						}
					}
					else
					{
						const T	*p0 = source.getPixel(x*2,y*2),
								*p1 = source.getPixel(x*2+1,y*2),
								*p2 = source.getPixel(x*2+1,y*2+1),
								*p3 = source.getPixel(x*2,y*2+1);
						F a0 = toFloat(p0[3]);
						F a1 = toFloat(p1[3]);
						F a2 = toFloat(p2[3]);
						F a3 = toFloat(p3[3]);
						F aSum = a0 + a1 + a2 + a3;
						T*outPixel = this->get(x,y);
						if (aSum > 0)
						{
							F avg0 = ((F)p0[0] * a0 + (F)p1[0] * a1 + (F)p2[0] * a2 + (F)p3[0] * a3) / aSum;
							F avg1 = ((F)p0[1] * a0 + (F)p1[1] * a1 + (F)p2[1] * a2 + (F)p3[1] * a3) / aSum;
							F avg2 = ((F)p0[2] * a0 + (F)p1[2] * a1 + (F)p2[2] * a2 + (F)p3[2] * a3) / aSum;
							outPixel[0] = (T)avg0;
							outPixel[1] = (T)avg1;
							outPixel[2] = (T)avg2;
							outPixel[3] = floatToChannel(aSum / (F)4);
						}
						else
						{
							F avg0 = ((F)p0[0] + (F)p1[0] + (F)p2[0] + (F)p3[0]) / (F)4;
							F avg1 = ((F)p0[1] + (F)p1[1] + (F)p2[1] + (F)p3[1]) / (F)4;
							F avg2 = ((F)p0[2] + (F)p1[2] + (F)p2[2] + (F)p3[2]) / (F)4;
							outPixel[0] = (T)avg0;
							outPixel[1] = (T)avg1;
							outPixel[2] = (T)avg2;
							outPixel[3] = 0;
						}
					}
			});
		}
		else
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&source](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
					if (isNormalMap() && image_channels >= 3)
					{
						TVec3<F> norm0,norm1,norm2,norm3;
						source.getNormal(x*2,y*2,norm0);
						source.getNormal(x*2+1,y*2,norm1);
						source.getNormal(x*2+1,y*2+1,norm2);
						source.getNormal(x*2,y*2+1,norm3);
						Vec::add(norm0,norm1);
						Vec::add(norm0,norm2);
						Vec::add(norm0,norm3);
						Vec::normalize0(norm0);
						this->setNormal(x,y,norm0);
						if (image_channels > 3)
						{
							for (BYTE c = 3; c < image_channels; c++)
							{
								F col = source.getPixel(x*2,y*2)[c];
								col += source.getPixel(x*2+1,y*2)[c];
								col += source.getPixel(x*2+1,y*2+1)[c];
								col += source.getPixel(x*2,y*2+1)[c];
								this->setChannel(x,y,c,(T)(col/4));
							}
						}
					}
					else
					{
						for (BYTE c = 0; c < image_channels; c++)
						{
							F col = source.getPixel(x*2,y*2)[c];
							col += source.getPixel(x*2+1,y*2)[c];
							col += source.getPixel(x*2+1,y*2+1)[c];
							col += source.getPixel(x*2,y*2+1)[c];
							this->setChannel(x,y,c,(T)(col/4));
						}
		/*                if (!x && !y)
							ShowMessage(_toString(temp.getPixel(x,y),image_channels));*/
					}
			});

		return true;
	}

template <class Nature>
	bool GenericImage<Nature>::scaleXHalf()
	{
		if (image_width<2)
			return false;
		GenericImage<Nature>	temp(image_width/2,image_height,image_channels);
		temp.content_type = content_type;
		temp.origin.adoptData(origin);

		Concurrency::parallel_for(dimension_t(0),image_height,[this,&temp](dimension_t y)
		{
			for (dimension_t x = 0; x < image_width/2; x++)
				if (isNormalMap() && image_channels == 3)
				{
					TVec3<F> norm0,norm1;
					getNormal(x*2,y,norm0);
					getNormal(x*2+1,y,norm1);
					Vec::add(norm0,norm1);
					Vec::normalize0(norm0);
					temp.setNormal(x,y,norm0);
				}
				else
					for (BYTE c = 0; c < image_channels; c++)
						temp.setChannel(x,y,c,(T)(((F)this->getPixel(x*2,y)[c]+this->getPixel(x*2+1,y)[c])/2));
		});
		adoptData(temp);
		return true;
	}


template <class Nature>
	bool GenericImage<Nature>::scaleYHalf()
	{
		if (image_height<2)
			return false;
		GenericImage<Nature>	temp(image_width,image_height/2,image_channels);
		temp.setContentType(content_type);
		temp.origin.adoptData(origin);

		Concurrency::parallel_for(dimension_t(0),image_width,[this,&temp](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height/2; y++)
				if (isNormalMap() && image_channels == 3)
				{
					TVec3<F> norm0,norm1;
					getNormal(x,y*2,norm0);
					getNormal(x,y*2+1,norm1);
					Vec::add(norm0,norm1);
					Vec::normalize0(norm0);
					temp.setNormal(x,y,norm0);
				}
				else
					for (BYTE c = 0; c < image_channels; c++)
						temp.setChannel(x,y,c,(T)(((F)this->getPixel(x,y*2)[c]+this->getPixel(x,y*2+1)[c])/2));
		});
		adoptData(temp);
		return true;
	}

template <class Nature>
	void GenericImage<Nature>::scaleDouble(bool loop)
	{
		if (image_width * 2 < image_width || image_height * 2 < image_height)
			return;
		if (!image_width || !image_height)
		{
			image_width *= 2;
			image_height *= 2;
			return;
		}

		ImageTemplate<T>	temp(image_width*2,image_height*2,image_channels);
		temp.setContentType(content_type);
		temp.origin.adoptData(origin);

		Concurrency::parallel_for(dimension_t(0),image_width,[this,&temp](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
				temp.set(x*2,y*2,getPixel(x,y));
		});
    
		if (!loop)
		{
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height-1; y++)
					for (BYTE k = 0; k < image_channels; k++)
						temp.setChannel(x*2,y*2+1,k,(T)(((F)getPixel(x,y)[k]+getPixel(x,y+1)[k])/2));
			});
			Concurrency::parallel_for(dimension_t(0),image_width-1,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
					for (BYTE k = 0; k < image_channels; k++)
						temp.setChannel(x*2+1,y*2,k,(T)(((F)getPixel(x,y)[k]+getPixel(x+1,y)[k])/2));
			});
			Concurrency::parallel_for(dimension_t(0),image_width-1,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height-1; y++)
					for (BYTE k = 0; k < image_channels; k++)
						temp.setChannel(x*2+1,y*2+1,k,(T)(((F)getPixel(x,y)[k]+getPixel(x,y+1)[k]+getPixel(x+1,y+1)[k]+getPixel(x+1,y)[k])/4));
			});
			Concurrency::parallel_for(dimension_t(0),image_height*2,[this,&temp](dimension_t y)
			{
				temp.set(image_width*2-1,y,temp.getPixel(image_width*2-2,y));
			});
			Concurrency::parallel_for(dimension_t(0),image_width*2,[this,&temp](dimension_t x)
			{
				temp.set(x,image_height*2-1,temp.getPixel(x,image_height*2-2));
			});
		}
		else
		{
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
					for (BYTE k = 0; k < image_channels; k++)
						temp.setChannel(x*2,y*2+1,k,(T)(((F)getPixel(x,y)[k]+getPixel(x,(y+1)%image_height)[k])/2));
			});
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
					for (BYTE k = 0; k < image_channels; k++)
						temp.setChannel(x*2+1,y*2,k,(T)(((F)getPixel(x,y)[k]+getPixel((x+1)%image_width,y)[k])/2));
			});
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&temp](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
					for (BYTE k = 0; k < image_channels; k++)
						temp.setChannel(x*2+1,y*2+1,k,(T)(((F)getPixel(x,y)[k]+getPixel(x,(y+1)%image_height)[k]+getPixel((x+1)%image_width,(y+1)%image_height)[k]+getPixel((x+1)%image_width,y)[k])/4));
			});
		}
    
       
		adoptData(temp);
	}

template <class Nature>
	void GenericImage<Nature>::scaleXDouble(bool loop)
	{
		if (!image_width || image_width*2 < image_width)
			return;
		GenericImage<Nature>	temp(image_width*2,image_height,image_channels);
		temp.origin.adoptData(origin);
		temp.setContentType(content_type);

		Concurrency::parallel_for(dimension_t(0),image_height,[this,&temp](dimension_t y)
		{
			for (dimension_t x = 0; x < image_width; x++)
			{
				temp.set(x*2,y,this->getPixel(x,y));
				if (x == image_width-1)
					continue;
				if (isNormalMap() && image_channels == 3)
				{
					TVec3<F> norm0,norm1;
					getNormal(x,y,norm0);
					getNormal(x+1,y,norm1);
					Vec::add(norm0,norm1);
					Vec::normalize0(norm0);
					temp.setNormal(x*2+1,y,norm0);
				}
				else
					for (BYTE c = 0; c < image_channels; c++)
						temp.setChannel(x*2+1,y,c,(T)(((F)this->getPixel(x,y)[c]+this->getPixel(x+1,y)[c])/2));
			}
			temp.set(image_width*2-1,y,this->getPixel(image_width-1,y));
		});
		adoptData(temp);

	}

template <class Nature>
	void GenericImage<Nature>::scaleYDouble(bool loop)
	{
		if (!image_height || image_height*2 < image_height)
			return;
		GenericImage<Nature>	temp(image_width,image_height*2,image_channels);
		temp.origin.adoptData(origin);
		temp.setContentType(content_type);

		Concurrency::parallel_for(dimension_t(0),image_width,[this,&temp](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
			{
				temp.set(x,y*2,this->getPixel(x,y));
				if (y == image_height-1)
					continue;
				if (isNormalMap() && image_channels == 3)
				{
					TVec3<F> norm0,norm1;
					getNormal(x,y,norm0);
					getNormal(x,y+1,norm1);
					Vec::add(norm0,norm1);
					Vec::normalize0(norm0);
					temp.setNormal(x,y*2+1,norm0);
				}
				else
					for (BYTE c = 0; c < image_channels; c++)
						temp.setChannel(x,y*2+1,c,(T)(((F)this->getPixel(x,y)[c]+this->getPixel(x,y+1)[c])/2));
			}
			temp.set(x,image_height*2-1,this->getPixel(x,image_height-1));
		});
		adoptData(temp);
	}


template <class Nature>
	bool GenericImage<Nature>::scaleTo(dimension_t width, dimension_t height)
	{
		if (!image_width || !image_height)
			return false;
		while (image_width <= width/2)
			scaleXDouble();
		while (image_height <= height/2)
			scaleYDouble();
		while (image_width >= width*2)
			scaleXHalf();
		while (image_height >= height*2)
			scaleYHalf();
		return image_width == width && image_height == height;
	}

template <class Nature>
	void GenericImage<Nature>::resample(dimension_t w, dimension_t h)
	{
		if (scaleTo(w,h))
			return;
		GenericImage<Nature>	temp(w,h,image_channels);
		temp.origin.adoptData(origin);
		temp.setContentType(content_type);
		if (isNormalMap())
		{
			Concurrency::parallel_for(dimension_t(0),w,[this,&temp,w,h](dimension_t x)
			{
				TVec3<F> normal;
				for (dimension_t y = 0; y < h; y++)
				{
					sampleNormal((F)x/w,(F)y/h,(F)(x+1)/w,(F)(y+1)/h,normal);
					temp.setNormal(x,y,normal);
					//_c3(pixel,temp.getPixel(x,y));
				}
			});
		}
		else
			Concurrency::parallel_for(dimension_t(0),w,[this,&temp,w,h](dimension_t x)
			{
				T pixel[256];
				for (dimension_t y = 0; y < h; y++)
				{
					sample((F)x/w,(F)y/h,(F)(x+1)/w,(F)(y+1)/h,pixel);
					temp.set(x,y,pixel);
				}
			});
		adoptData(temp);
	}

template <class Nature>
	void	GenericImage<Nature>::crop(dimension_t x, dimension_t y, dimension_t width, dimension_t height)
	{
		if (x >= image_width || y >= image_height)
		{
			free();
			return;
		}

		dimension_t	xext = vmin(width,image_width-x),
					yext = vmin(height,image_height-y);
             
		if (!xext || !yext)
		{
			free();
			return;
		}
		if (xext == image_width && yext == image_height)
			return;
		const BYTE c_num = image_channels;

		GenericImage<Nature>		temp(xext,yext,image_channels);
		temp.origin.adoptData(origin);
		temp.setContentType(content_type);
		
		Concurrency::parallel_for(dimension_t(0),yext,[this,&temp,x,y,xext](dimension_t Y)
		{
			memcpy(temp.getPixel(0,Y),getPixel(x,y+Y),sizeof(T)*xext*image_channels);
		});

		adoptData(temp);
	}

template <class Nature>
	void GenericImage<Nature>::truncate(dimension_t width, dimension_t height)
	{
		if (image_width <= width && image_height <= height)
			return;
	
		GenericImage<Nature>	new_image(vmin(width,image_width),vmin(height,image_height),image_channels);
		new_image.origin.adoptData(origin);
		new_image.setContentType(content_type);
		new_image.paint(this,0,0,IC_COPY);
	
		adoptData(new_image);
	}
	

template <typename T>
	void ImageTemplate<T>::readFrom(const ImageTemplate<T>*other)
	{
		setDimensions(other->image_width,other->image_height,other->image_channels);
		content_type = other->content_type;
		origin = other->origin;
		read(other->image_data);
	}

template <typename T>
	void	ImageTemplate<T>::read(const T* data)
	{
		memcpy(image_data,data,size());
	}

template <typename T>
	bool	ImageTemplate<T>::columnIsOpaque(dimension_t x)	const
	{
		for (dimension_t y = 0; y < image_height; y++)
			if (getPixel(x,y)[image_channels-1] != 0)
				return true;
		return false;
	}

template <typename T>
	bool	ImageTemplate<T>::rowIsOpaque(dimension_t y)	const
	{
		for (dimension_t x = 0; x < image_width; x++)
			if (getPixel(x,y)[image_channels-1] != 0)
				return true;
		return false;
	}

template <typename T>
	bool	ImageTemplate<T>::truncateToOpaque()
	{
		if (image_channels != 4 && image_channels != 2)
			return false;
		if (!image_width || !image_height)
			return false;
		dimension_t	left = 0,right=image_width,
				bottom = 0,top=image_height;
		while (left < right && !columnIsOpaque(left))
			left++;
		while (right > left && !columnIsOpaque(right-1))
			right--;
		while (bottom < top && !rowIsOpaque(bottom))
			bottom++;
		while (bottom < top && !rowIsOpaque(top-1))
			top--;
		if (bottom == top || left == right)
		{
			setSize(0,0,image_channels);
			return true;
		}
		if (bottom == 0 && top == 0 && right == image_width && top == image_height)
			return false;
		ImageTemplate<T>	new_data(right-left,top-bottom,image_channels);
		new_data.origin.adoptData(origin);
		new_data.content_type = content_type;

		if (image_channels == 4)
			Concurrency::parallel_for(dimension_t(0),new_data.image_width,[this,&new_data,left,bottom](dimension_t x)
			{
				for (dimension_t y = 0; y < new_data.image_height; y++)
					_c4(getPixel(left+x,bottom+y),new_data.getPixel(x,y));
			});
		else
			Concurrency::parallel_for(dimension_t(0),new_data.image_width,[this,&new_data,left,bottom](dimension_t x)
			{
				for (dimension_t y = 0; y < new_data.image_height; y++)
					_c2(getPixel(left+x,bottom+y),new_data.getPixel(x,y));
			});
		adoptData(new_data);
		return true;
	}


template <typename T>
	bool	ImageTemplate<T>::exportRectangle(dimension_t xoffset, dimension_t yoffset, dimension_t w, dimension_t h, T*target)	const
	{
		if (yoffset+h > image_height || xoffset+w > image_width)
			return false;

		Concurrency::parallel_for(dimension_t(yoffset),yoffset+h,[this,=target](dimension_t y)
		{
			memcpy(target+size_t(y-yoffset)*size_t(w)*size_t(image_channels),
					image_data+size_t(y)*size_t(image_width)*size_t(image_channels)+size_t(xoffset)*size_t(image_channels),
					size_t(w)*size_t(image_channels)*sizeof(T));
		});
		return true;
	}

template <typename T>
	bool	ImageTemplate<T>::importRectangle(dimension_t xoffset, dimension_t yoffset, dimension_t w, dimension_t h, const T*target)
	{
		if (yoffset+h > image_height || xoffset+w > image_width)
			return false;
		Concurrency::parallel_for(dimension_t(yoffset),yoffset+h,[this,=target](dimension_t y)
		{
			memcpy(image_data+size_t(y)*size_t(image_width)*size_t(image_channels)+size_t(xoffset)*size_t(image_channels),
					target+size_t(y-yoffset)*size_t(w)*size_t(image_channels),
					size_t(w)*size_t(image_channels)*sizeof(T));
		});
		return true;
	}

template <class Nature>
	template <typename Float>
		bool	GenericImage<Nature>::importRectanglef(dimension_t xoffset, dimension_t yoffset, dimension_t w, dimension_t h, const Float*target)
		{
			if (yoffset+h > image_height || xoffset+w > image_width)
				return false;
			Concurrency::parallel_for(dimension_t(0),h,[this,=target](dimension_t y)
			{
				for (dimension_t x = 0; x < w; x++)
				{
					index_t toffset = (size_t(y+yoffset)*size_t(image_width)+size_t(x+xoffset))*size_t(image_channels),
							soffset = (size_t(y)*size_t(w)+size_t(x))*size_t(image_channels);
					for (BYTE c = 0; c < image_channels; c++)
						 image_data[toffset+c] = Nature::floatToChannel(target[soffset+c]);
				}
			});
			return true;
		}

template <class Nature>
	template <typename Float>
		bool	GenericImage<Nature>::exportRectanglef(dimension_t xoffset, dimension_t yoffset, dimension_t w, dimension_t h, Float*target) const
		{
			if (yoffset+h > image_height || xoffset+w > image_width)
				return false;
			Concurrency::parallel_for(dimension_t(0),h,[this,=target](dimension_t y)
			{
				for (dimension_t x = 0; x < w; x++)
				{
					index_t soffset = (size_t(y+yoffset)*size_t(image_width)+size_t(x+xoffset))*size_t(image_channels),
							toffset = (size_t(y)*size_t(w)+size_t(x))*size_t(image_channels);
					for (BYTE c = 0; c < image_channels; c++)
						target[toffset+c] = Nature::toFloat(image_data[soffset+c]);
				}
			});
			return true;
		}


template <typename T>
	void	ImageTemplate<T>::extractChannels(BYTE channel, BYTE c_num, ImageTemplate<T>&target)
	{
		if (channel+c_num > image_channels || !c_num)
		{
			target.free();
			return;
		}
		target.setSize(image_width,image_height,c_num);
		target.setContentType(content_type);
		target.origin = origin;

		Concurrency::parallel_for(dimension_t(0),image_width,[this,&target,channel](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
				target.set(x,y,getPixel(x,y)+channel);
		});
	}

template <class Nature>
	void	GenericImage<Nature>::fractalDouble(F noise_level, bool loop)
	{
		fractalDouble(noise_level,loop,rand());
	}

template <class Nature>
	void	GenericImage<Nature>::fractalDouble(F noise_level, bool loop, unsigned seed)
	{
		if (!image_width || !image_height || !image_channels || image_width*2<image_width ||image_height*2<image_height)
			return;
	
		if (!loop)
		{
			ImageTemplate<T> new_image(image_width*2-1,image_height*2-1,image_channels);

			new_image.origin.adoptData(origin);
			new_image.setContentType(content_type);

			#if 0
				X X X X X X X X
						   
				X X X X X X X X
						   
				X X X X X X X X
						   
				X X X X X X X X
						   
				X X X X X X X X
			#endif
		
		
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&new_image](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
				{
					const T	*from = getPixel(x,y);
					T		*to = new_image.getPixel(x*2,y*2);
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = from[k];
				}
			});
		
			#if 0
				* * * * * * * *
				 X X X X X X X 
				* * * * * * * *
				 X X X X X X X 
				* * * * * * * *
				 X X X X X X X 
				* * * * * * * *
				 X X X X X X X 
				* * * * * * * *
			#endif
			
			Concurrency::parallel_for(dimension_t(0),image_width-1,[this,&new_image,noise_level,seed](dimension_t x)
			{
				ImageRandomSource rnd((unsigned long)(seed+x*29));

				for (dimension_t y = 0; y+1 < image_height; y++)
				{
					const T		*from0 = getPixel(x,y),
								*from1 = getPixel(x+1,y),
								*from2 = getPixel(x+1,y+1),
								*from3 = getPixel(x,y+1);
					T			*to = new_image.getPixel(x*2+1,y*2+1);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k]+(F)from3[k])/F(4)+(F(rnd())*F(2)-F(1))*noise_level);
				}
			});
			
			#if 0
				* * * * * * * *
				 *X*X*X*X*X*X* 
				* * * * * * * *
				 *X*X*X*X*X*X* 
				* * * * * * * *
				 *X*X*X*X*X*X* 
				* * * * * * * *
				 *X*X*X*X*X*X* 
				* * * * * * * *
			
				 0
				3X1
				 2
			#endif
			
			Concurrency::parallel_for(dimension_t(1),image_width-1,[this,&new_image,noise_level,seed](dimension_t x)
			{
				ImageRandomSource rnd((unsigned long)(seed+x*29+1023));

				for (dimension_t y = 0; y+1 < image_height; y++)
				{
					const T		*from0 = getPixel(x,y),
								*from1 = new_image.getPixel(x*2+1,y*2+1),
								*from2 = getPixel(x,y+1),
								*from3 = new_image.getPixel(x*2-1,y*2+1);
					T			*to = new_image.getPixel(x*2,y*2+1);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k]+(F)from3[k])/F(4)+(F(rnd())*F(2)-F(1))*noise_level);
				}
			});
			
			#if 0
				* * * * * * * *
				X************* 
				* * * * * * * *
				X************* 
				* * * * * * * *
				X************* 
				* * * * * * * *
				X************* 
				* * * * * * * *
			
				 0
				 X1
				 2
			#endif
			
				Concurrency::parallel_for(dimension_t(0),image_height-1,[this,&new_image,noise_level,seed](dimension_t y)
				{
					ImageRandomSource rnd((unsigned long)(seed+y*29+10234));

					const T		*from0 = getPixel(0,y),
								*from1 = new_image.getPixel(1,y*2+1),
								*from2 = getPixel(0,y+1);
					T			*to = new_image.getPixel(0,y*2+1);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k])/F(3)+(F(rnd())*F(2)-F(1))*noise_level);
				});

			
			#if 0
				* * * * * * * *
				**************X
				* * * * * * * *
				**************X
				* * * * * * * *
				**************X
				* * * * * * * *
				**************X
				* * * * * * * *
			
				 0
				2X
				 1
			#endif
			
				Concurrency::parallel_for(dimension_t(0),image_height-1,[this,&new_image,noise_level,seed](dimension_t y)
				{
					ImageRandomSource rnd((unsigned long)(seed+y*29+102349));

					const T		*from0 = getPixel(image_width-1,y),
								*from1 = getPixel(image_width-1,y+1),
								*from2 = new_image.getPixel((image_width-1)*2-1,y*2+1);
					T			*to = new_image.getPixel((image_width-1)*2,y*2+1);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k])/F(3)+(F(rnd())*F(2)-F(1))*noise_level);
				});
		
			#if 0
				* * * * * * * *
				***************
				*X*X*X*X*X*X*X*
				***************
				*X*X*X*X*X*X*X*
				***************
				*X*X*X*X*X*X*X*
				***************
				* * * * * * * *
			
				 3
				0X2
				 1
			#endif
			
			Concurrency::parallel_for(dimension_t(0),image_width-1,[this,&new_image,noise_level,seed](dimension_t x)
			{
				ImageRandomSource rnd((unsigned long)(seed+x*29+182349));

				for (dimension_t y = 1; y+1 < image_height; y++)
				{
					const T		*from0 = getPixel(x,y),
								*from1 = new_image.getPixel(x*2+1,y*2-1),
								*from2 = getPixel(x+1,y),
								*from3 = new_image.getPixel(x*2+1,y*2+1);
					T			*to = new_image.getPixel(x*2+1,y*2);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k]+(F)from3[k])/F(4)+(F(rnd())*F(2)-F(1))*noise_level);
				}
			});
			
			#if 0
				* * * * * * * *
				***************
				***************
				***************
				***************
				***************
				***************
				***************
				*X*X*X*X*X*X*X*
			
				 2
				0X1

			#endif
			
				Concurrency::parallel_for(dimension_t(0),image_width-1,[this,&new_image,noise_level,seed](dimension_t x)
				{
					ImageRandomSource rnd((unsigned long)(seed+x*29+1827349));

					const T		*from0 = getPixel(x,0),
								*from1 = getPixel(x+1,0),
								*from2 = new_image.getPixel(x*2+1,1);
					T			*to = new_image.getPixel(x*2+1,0);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k])/F(3)+(F(rnd())*F(2)-F(1))*noise_level);
				});
			
			#if 0
				*X*X*X*X*X*X*X*
				***************
				***************
				***************
				***************
				***************
				***************
				***************
				***************
			
			 
				0X2
				 1
			#endif
			
				Concurrency::parallel_for(dimension_t(0),image_width-1,[this,&new_image,noise_level,seed](dimension_t x)
				{
					ImageRandomSource rnd((unsigned long)(seed+x*29+827349));
					const T		*from0 = getPixel(x,image_height-1),
								*from1 = new_image.getPixel(x*2+1,(image_height-1)*2-1),
								*from2 = getPixel(x+1,image_height-1);
					T			*to = new_image.getPixel(x*2+1,(image_height-1)*2);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k])/F(3)+((F)rnd()*F(2)-F(1))*noise_level);
				});

			adoptData(new_image);
		}
		else
		{
			ImageTemplate<T> new_image(image_width*2,image_height*2,image_channels);
			new_image.origin.adoptData(origin);
			new_image.setContentType(content_type);
		
			#if 0
			                
				X X X X X X X X 
						    
				X X X X X X X X 
						    
				X X X X X X X X 
						    
				X X X X X X X X 
						    
				X X X X X X X X 
			#endif
		
		
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&new_image](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
				{
					const T		*from = getPixel(x,y);
					T			*to = new_image.getPixel(x*2,y*2);
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = from[k];
				}
			});
		
			#if 0
				 X X X X X X X X
				* * * * * * * * 
				 X X X X X X X X
				* * * * * * * *
				 X X X X X X X X
				* * * * * * * *
				 X X X X X X X X
				* * * * * * * *
				 X X X X X X X X
				* * * * * * * * 
			#endif
			
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&new_image,noise_level,seed](dimension_t x)
			{
				ImageRandomSource rnd((unsigned long)(seed+x*29+27349));

				for (dimension_t y = 0; y < image_height; y++)
				{
					const T		*from0 = getPixel(x,y),
								*from1 = getPixel((x+1)%image_width,y),
								*from2 = getPixel((x+1)%image_width,(y+1)%image_height),
								*from3 = getPixel(x,(y+1)%image_height);
					T			*to = new_image.getPixel(x*2+1,y*2+1);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k]+(F)from3[k])/F(4)+(F(rnd())*F(2)-F(1))*noise_level);
				}
			});
			
			#if 0
				X*X*X*X*X*X*X*X*
				* * * * * * * * 
				X*X*X*X*X*X*X*X*
				* * * * * * * * 
				X*X*X*X*X*X*X*X*
				* * * * * * * * 
				X*X*X*X*X*X*X*X*
				* * * * * * * * 
				X*X*X*X*X*X*X*X*
				* * * * * * * * 
			
				 0
				3X1
				 2
			#endif
			
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&new_image,noise_level,seed](dimension_t x)
			{
				ImageRandomSource rnd((unsigned long)(seed+x*29+7349));

				for (dimension_t y = 0; y < image_height; y++)
				{
					const T		*from0 = getPixel(x,y),
								*from1 = new_image.getPixel(x*2+1,y*2+1),
								*from2 = getPixel(x,(y+1)%image_height),
								*from3 = new_image.getPixel(x>0?x*2-1:image_width*2-1,y*2+1);
					T			*to = new_image.getPixel(x*2,y*2+1);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k]+(F)from3[k])/F(4)+(F(rnd())*F(2)-F(1))*noise_level);
				}
			});
	
			#if 0
				****************
				*X*X*X*X*X*X*X*X
				****************
				*X*X*X*X*X*X*X*X
				****************
				*X*X*X*X*X*X*X*X
				****************
				*X*X*X*X*X*X*X*X
				****************
				*X*X*X*X*X*X*X*X
			
				 3
				0X2
				 1
			#endif
			
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&new_image,noise_level,seed](dimension_t x)
			{
				ImageRandomSource rnd((unsigned long)(seed+x*29+349));


				for (dimension_t y = 0; y < image_height; y++)
				{
					const T		*from0 = getPixel(x,y),
								*from1 = new_image.getPixel(x*2+1,y>0?y*2-1:image_height*2-1),
								*from2 = getPixel((x+1)%image_width,y),
								*from3 = new_image.getPixel(x*2+1,y*2+1);
					T			*to = new_image.getPixel(x*2+1,y*2);
					typedef GenericImage<Nature>::ImageNature::float_type_t	F;
					for (BYTE k = 0; k < image_channels; k++)
						to[k] = GenericImage<Nature>::ImageNature::clamp(((F)from0[k]+(F)from1[k]+(F)from2[k]+(F)from3[k])/F(4)+(F(rnd())*F(2)-F(1))*noise_level);
				}
			});
		
			adoptData(new_image);
		}
	}

template <class Nature>
	void	GenericImage<Nature>::scaleChannel(BYTE channel, F factor)
	{
		if (channel >= image_channels)
			return;
		Concurrency::parallel_for(dimension_t(0),image_width,[this,channel,factor](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
			{
        		T&val = getPixel(x,y)[channel];
				val = GenericImage<Nature>::ImageNature::clamp((F)val*factor);
			}
		});
	}

template <class Nature>
	void	GenericImage<Nature>::scaleChannel(BYTE channel, F offset, F factor)
	{
		if (channel >= image_channels)
			return;
		Concurrency::parallel_for(dimension_t(0),image_width,[this,channel,offset,factor](dimension_t x)
		{
			for (dimension_t y = 0; y < image_height; y++)
			{
        		T&val = getPixel(x,y)[channel];
				val = GenericImage<Nature>::ImageNature::clamp(offset + (F)val*factor);
			}
		});
	}


template <class Nature>
	void	GenericImage<Nature>::paint(const ImageTemplate<T>*other, dimension_t x, dimension_t y, BaseImage::combiner_t combiner)
	{
		draw(other,x,y,combiner);
	}

template <class Nature>
	void	GenericImage<Nature>::draw(const ImageTemplate<T>*other, dimension_t x, dimension_t y, BaseImage::combiner_t combiner)
	{
		if (x >= image_width || y >= image_height)
			return;


		dimension_t	xext = vmin(other->width(),image_width-x),
					yext = vmin(other->height(),image_height-y);
             
		if (!xext || !yext)
			return;
		BYTE c_num = vmin(image_channels,other->channels());
		if (!c_num)
			return;

		if (other->channels() != 4)
		{
			if (combiner == IC_ALPHA_BLEND)
				combiner = IC_COPY;
			elif (combiner == IC_ALPHA_ADD)
				combiner = IC_ADD;
		}

		switch (combiner)
		{
			case IC_ALPHA_BLEND:
				switch(c_num)
				{
					case 3:
						Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext](dimension_t X)
						{
							for (dimension_t Y = 0; Y < yext; Y++)
							{
								T*p = getPixel(x+X,y+Y);
								const T*o = other->getPixel(X,Y);
								p[0] = (T)(((F)p[0]*(max-o[3]) + (F)o[0]*o[3])/max);
								p[1] = (T)(((F)p[1]*(max-o[3]) + (F)o[1]*o[3])/max);
								p[2] = (T)(((F)p[2]*(max-o[3]) + (F)o[2]*o[3])/max);
							}
						});
					break;
					case 4:
						Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext](dimension_t X)
						{
							for (dimension_t Y = 0; Y < yext; Y++)
							{
								T*p = getPixel(x+X,y+Y);
								const T*o = other->getPixel(X,Y);
								typedef GenericImage<Nature>::ImageNature::float_type_t	F;
								F a = p[3]>0?vmin((F)o[3]/p[3],1):F(1);
								p[0] = (T)(((F)p[0]*(1.0-a) + (F)o[0]*a));
								p[1] = (T)(((F)p[1]*(1.0-a) + (F)o[1]*a));
								p[2] = (T)(((F)p[2]*(1.0-a) + (F)o[2]*a));
								p[3] = (T)(p[3]+o[3]*(255-p[3])/255);
							}
						});
					break;
					default:
						Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext,c_num](dimension_t X)
						{
							for (dimension_t Y = 0; Y < yext; Y++)
								blend(x+X,y+Y,other->getPixel(X,Y),other->getPixel(X,Y)[3],c_num);
						});
					break;
				}
			break;
			case IC_COPY:
				{
					auto p = this;
					Concurrency::parallel_for(dimension_t(0),xext,[this,p,&other,x,y,yext,c_num](dimension_t X)
					{
						for (dimension_t Y = 0; Y < yext; Y++)
							p->set(x+X,y+Y,other->getPixel(X,Y),c_num);
					});
				}
			break;
			case IC_MULTIPLY:
				Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext,c_num](dimension_t X)
				{
					for (dimension_t Y = 0; Y < yext; Y++)
					{
						T		*local = getPixel(x+X,y+Y);
						const T	*remote = other->getPixel(X,Y);
						typedef GenericImage<Nature>::ImageNature	Nature;
						for (BYTE c = 0; c < c_num; c++)
							local[c] = Nature::floatToChannel(Nature::toFloat(local[c]) * Nature::toFloat(remote[c]));
					}
				});
			break;
			case IC_MULT_NEGATIVE:
				Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext,c_num](dimension_t X)
				{
					for (dimension_t Y = 0; Y < yext; Y++)
					{
						T		*local = getPixel(x+X,y+Y);
						const T	*remote = other->getPixel(X,Y);
						typedef GenericImage<Nature>::ImageNature	Nature;
						for (BYTE c = 0; c < c_num; c++)
							local[c] = max-Nature::floatToChannel((Nature::toFloat(max-local[c])) * (Nature::toFloat(max-remote[c])));
					}
				});
			break;

			case IC_ALPHA_ADD:
		
				switch(c_num)
				{
					case 3:
						Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext](dimension_t X)
						{
							for (dimension_t Y = 0; Y < yext; Y++)
							{
								T		*p = getPixel(x+X,y+Y);
								const T	*o = other->getPixel(X,Y);
								typedef GenericImage<Nature>::ImageNature::float_type_t	F;
								typedef GenericImage<Nature>::ImageNature	Nature;
								F weight = Nature::toFloat(o[3]);
								p[0] = Nature::clamp((F)p[0] + (F)o[0]*weight);
								p[1] = Nature::clamp((F)p[1] + (F)o[1]*weight);
								p[2] = Nature::clamp((F)p[2] + (F)o[2]*weight);
							}
						});
					break;
					case 4:
						Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext](dimension_t X)
						{
							for (dimension_t Y = 0; Y < yext; Y++)
							{
								T		*p = getPixel(x+X,y+Y);
								const T	*o = other->getPixel(X,Y);
								typedef GenericImage<Nature>::ImageNature::float_type_t	F;
								typedef GenericImage<Nature>::ImageNature	Nature;
								F weight = Nature::toFloat(o[3]);
								p[0] = Nature::clamp((F)p[0] + (F)o[0]*weight);
								p[1] = Nature::clamp((F)p[1] + (F)o[1]*weight);
								p[2] = Nature::clamp((F)p[2] + (F)o[2]*weight);
								p[3] = Nature::clamp((F)p[3] + (F)o[3]*weight);
							}
						});
					break;
					default:
						Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext,c_num](dimension_t X)
						{
							for (dimension_t Y = 0; Y < yext; Y++)
							{
								T		*local = getPixel(x+X,y+Y);
								const T	*remote = other->getPixel(X,Y);
								typedef GenericImage<Nature>::ImageNature::float_type_t	F;
								typedef GenericImage<Nature>::ImageNature	Nature;
								F weight = Nature::toFloat(remote[3]);
								for (BYTE c = 0; c < c_num; c++)
									local[c] = Nature::clamp((F)local[c]+(F)((F)remote[c]*weight));
							}
						});
					break;
				}		

			break;
			case IC_ADD:
				Concurrency::parallel_for(dimension_t(0),xext,[this,&other,x,y,yext,c_num](dimension_t X)
				{
					for (dimension_t Y = 0; Y < yext; Y++)
					{
						T		*local = getPixel(x+X,y+Y);
						const T	*remote = other->getPixel(X,Y);
						typedef GenericImage<Nature>::ImageNature::float_type_t	F;
						typedef GenericImage<Nature>::ImageNature	Nature;
						for (BYTE c = 0; c < c_num; c++)
							local[c] = Nature::clamp((F)local[c]+(F)remote[c]);
					}
				});
			break;
		}

	}


template <class Nature>
	template <class Nature1>
		void	GenericImage<Nature>::toNormalMap(GenericImage<Nature1>&target, F x_texel_distance, F y_texel_distance, F height_scale, bool seamless, BYTE height_channel)		const
		{
			if (!image_channels)
				return;
			if (height_channel >= image_channels)
				height_channel = image_channels-1;
			target.setSize(image_width,image_height,3);
			target.setContentType(PixelType::TangentSpaceNormal);
	
	
			F		x_bump_scale = height_scale/x_texel_distance,
					y_bump_scale = height_scale/y_texel_distance;
				//Image normal(bump.width(),bump.height(),3);
		
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&target,seamless,x_bump_scale,y_bump_scale,height_channel](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
				{
					dimension_t	prevx = x?x-1:(seamless?image_width-1:0),
							prevy = y?y-1:(seamless?image_height-1:0),
							nextx = x+1<image_width?x+1:(seamless?0:image_width-1),
							nexty = y+1<image_height?y+1:(seamless?0:image_height-1);
		
					const T		*px0 = getPixel(prevx,y),
								*px1 = getPixel(nextx,y),
								*py0 = getPixel(x,prevy),
								*py1 = getPixel(x,nexty);
					F		dx = ((F)px1[height_channel]-(F)px0[height_channel])*x_bump_scale,
							dy = ((F)py1[height_channel]-(F)py0[height_channel])*y_bump_scale;
					TVec3<F>	n = {-dx,-dy,1};
					Vec::normalize(n);
					//cout << _toString(n);
					target.setNormal(x,y,n);
					//cout << " => "<<_toString(target.getPixel(x,y))<<" @"<<x_bump_scale << "x"<<y_bump_scale<< endl;
				}
			});
			//cout << x_bump_scale << "x"<<y_bump_scale<<endl;

		}


template <class Nature>
	template <class Nature1>
		void	GenericImage<Nature>::toNormalMap(GenericImage<Nature1>&target, F intensity, bool seamless, BYTE height_channel)		const
		{
			if (!image_channels)
				return;
			if (height_channel >= image_channels)
				height_channel = image_channels-1;
			target.setSize(image_width,image_height,3);
			target.setContentType(PixelType::TangentSpaceNormal);
	
	
			F		scale = (F(512)*F(max)),
					x_bump_scale = F(image_width)/scale*intensity,
					y_bump_scale = F(image_height)/scale*intensity;
				//Image normal(bump.width(),bump.height(),3);
		
			Concurrency::parallel_for(dimension_t(0),image_width,[this,&target,seamless,x_bump_scale,y_bump_scale,height_channel](dimension_t x)
			{
				for (dimension_t y = 0; y < image_height; y++)
				{
					dimension_t	prevx = x?x-1:(seamless?image_width-1:0),
							prevy = y?y-1:(seamless?image_height-1:0),
							nextx = x+1<image_width?x+1:(seamless?0:image_width-1),
							nexty = y+1<image_height?y+1:(seamless?0:image_height-1);
		
					const T		*px0 = getPixel(prevx,y),
								*px1 = getPixel(nextx,y),
								*py0 = getPixel(x,prevy),
								*py1 = getPixel(x,nexty);
					F		dx = ((F)px1[height_channel]-(F)px0[height_channel])*x_bump_scale,
							dy = ((F)py1[height_channel]-(F)py0[height_channel])*y_bump_scale;
					TVec3<F>	n = {-dx,-dy,1};
					Vec::normalize(n);
					//cout << _toString(n);
					target.setNormal(x,y,n);
					//cout << " => "<<_toString(target.getPixel(x,y))<<" @"<<x_bump_scale << "x"<<y_bump_scale<< endl;
				}
			});
			//cout << x_bump_scale << "x"<<y_bump_scale<<endl;
		}

template <class Nature>
	template <typename Float>
		void	GenericImage<Nature>::getNormal(dimension_t X, dimension_t Y, TVec3<Float>&out) const
		{
			const T*pixel = getPixel(X,Y);
			out.x = Nature::toNormalComponent(pixel[0]);
			out.y = Nature::toNormalComponent(pixel[1]);
			out.z = Nature::toNormalComponent(pixel[2]);
		}

template <class Nature>
	template <typename Float>
		bool GenericImage<Nature>::getNormalVerified(dimension_t X, dimension_t Y, TVec3<Float>&out)             const
		{
			if (X >= image_width || Y >= image_height || image_channels<3)
				return false;

			const T*pixel = getPixel(X,Y);
			out.x = Nature::toNormalComponent(pixel[0]);
			out.y = Nature::toNormalComponent(pixel[1]);
			out.z = Nature::toNormalComponent(pixel[2]);
			return true;
		}


template <class Nature>
	template <typename Float>
		void	GenericImage<Nature>::setNormal(dimension_t X, dimension_t Y, const TVec3<Float>&data)
		{
			T*p = getPixel(X,Y);
			//for (BYTE i = 0; i < 3; i++)
			p[0] = Nature::normalComponentToChannel(data.x);
			p[1] = Nature::normalComponentToChannel(data.y);
			p[2] = Nature::normalComponentToChannel(data.z);
		}

template <class Nature>
	void GenericImage<Nature>::blend(dimension_t X, dimension_t Y, const T*data,T alpha)
	{
		typedef typename TypeInfo<T>::GreaterType::Type	GT;	//next larger type (floats map to themselves)
		T*pixel = getPixel(X,Y);
		for (BYTE k = 0; k < image_channels; k++)
			pixel[k] = (GT(pixel[k])*(max-alpha) + GT(data[k])*alpha)/max;
	}

template <class Nature>
	template <typename Float>
		void GenericImage<Nature>::blendf(dimension_t X, dimension_t Y, const Float*data, Float alpha)
		{
			T*pixel = getPixel(X,Y);
			for (BYTE k = 0; k < image_channels; k++)
				pixel[k] = (T)((F)pixel[k]*(1-alpha) + (F)(data[k]*alpha*F(max)));
		}

template <class Nature>
	void GenericImage<Nature>::blend(dimension_t X, dimension_t Y, const T*data,T alpha, BYTE channels)
	{
		typedef typename TypeInfo<T>::GreaterType::Type	GT;	//next larger type
		if (channels > image_channels)
			channels = image_channels;
		T*pixel = getPixel(X,Y);
		for (BYTE k = 0; k < channels; k++)
			pixel[k] = (GT(pixel[k])*(max-alpha) + GT(data[k])*alpha)/max;
	}

template <class Nature>
	template <typename Float>
		void GenericImage<Nature>::blendf(dimension_t X, dimension_t Y, const Float*data, Float alpha, BYTE channels)
		{
			if (channels > image_channels)
				channels = image_channels;
			T*pixel = getPixel(X,Y);
			for (BYTE k = 0; k < channels; k++)
				pixel[k] = (T)((F)pixel[k]*(1-alpha) + (F)(data[k]*alpha*F(max)));
		}



template <class Nature>
	bool    alignableImages(const GenericImage<Nature>*field, unsigned cnt)
	{
		if (!cnt || !field)
			return false;
		unsigned w(field[0].getWidth()),h(field[0].getHeight());
		BYTE channels_(field[0].getChannels());
		for (unsigned i = 1; i < cnt; i++)
			if (field[i].getWidth() != w || field[i].getHeight() != h || field[i].getChannels() != channels_)
				return true;

		return false;
	}

template <class Nature>
	bool    alignableImages(const GenericImage<Nature>**field, unsigned cnt)
	{
		if (!cnt || !field)
			return false;
		unsigned w(field[0]->getWidth()),h(field[0]->getHeight());
		BYTE channels_(field[0]->getChannels());
		for (unsigned i = 1; i < cnt; i++)
			if (field[i]->getWidth() != w || field[i]->getHeight() != h || field[i]->getChannels() != channels_)
				return true;
		return false;
	}




template <class Nature>
	void    alignImages(GenericImage<Nature>*field, unsigned cnt)
	{
		if (!cnt || !field)
			return;
		unsigned w(field->getWidth()),h(field->getHeight());
		BYTE channels_(field->getChannels());
		for (unsigned i = 1; i < cnt; i++)
		{
			if (field[i].getWidth() > w)
				w = field[i].getWidth();
			if (field[i].getHeight() > h)
				h = field[i].getHeight();
			if (field[i].getChannels() > channels_)
				channels_ = field[i].getChannels();
		}
		for (unsigned i = 0; i < cnt; i++)
		{
			if (!field[i].scaleTo(w,h))
				field[i].resample(w,h);
			BYTE difference = channels_-field[i].getChannels();
			field[i].setChannels(channels_);
			for (BYTE k = channels_-difference; k < channels_; k++)
				field[i].setChannel(k,(BYTE)0x100);
		}
	}

template <class Nature>
	void    alignImages(GenericImage<Nature>**field, unsigned cnt)
	{
		if (!cnt || !field)
			return;
		unsigned w(0),h(0);
		BYTE channels_(0);
		for (unsigned i = 0; i < cnt; i++)
			if (field[i])
			{
				if (field[i]->getWidth() > w)
					w = field[i]->getWidth();
				if (field[i]->getHeight() > h)
					h = field[i]->getHeight();
				if (field[i]->getChannels() > channels_)
					channels_ = field[i]->getChannels();
			}
		for (unsigned i = 0; i < cnt; i++)
			if (field[i])
			{
				if (!field[i]->scaleTo(w,h))
					field[i]->resample(w,h);
				BYTE difference = channels_-field[i]->getChannels();
				field[i]->setChannels(channels_);
				for (BYTE k = channels_-difference; k < channels_; k++)
					field[i]->setChannel(k,(BYTE)0x100);
			}
	}



#endif
