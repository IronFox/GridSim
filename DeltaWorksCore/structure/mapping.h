#ifndef mappingH
#define mappingH

template <typename T>
	class Mapping2D
	{
	protected:
			T*			data;
			unsigned	data_width,
						data_height;
	public:
						Mapping2D():data(NULL),data_width(0),data_height(0)
						{}
						
						Mapping2D(T*data_, unsigned width_,unsigned height_):data(data_),data_width(width_),data_height(height_)
						{}

						Mapping2D(Ctr::ArrayData<T>&data_, unsigned width_,unsigned height_):data(data_.pointer()),data_width(width_),data_height(height_)
						{}
	
			void		setInterpretation(unsigned width_, unsigned height_)
						{
							data_width = width_;
							data_height = height_;
						}
	inline	unsigned	width()	const
						{
							return data_width;
						}
	inline	unsigned	height() const
						{
							return data_height;
						}
	inline	unsigned	size()	const
						{
							return data_width*data_height;
						}
	inline	T&			operator[](unsigned element)
						{
							return data[element];
						}
	inline	const T&	operator[](unsigned element)	const
						{
							return data[element];
						}
						
	inline	T&			get(unsigned x, unsigned y)
						{
							return data[y*data_width+x];
						}
	
	inline	const T&	get(unsigned x, unsigned y)	const
						{
							return data[y*data_width+x];
						}
			T*			pointer()
						{
							return data;
						}
			const T*	pointer()	const
						{
							return data;
						}
	};

#endif
