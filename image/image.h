#ifndef imageH
#define imageH


/******************************************************************

Image-class used to store and process textures
(or images in general).

******************************************************************/

#include "../global_root.h"
#include "../io/bit_stream.h"
#include "../math/vector.h"
#include <stdio.h>
#include <ppl.h>
#include <random>



#define		TEX_NORMAL_BIT			0x80
#define		TEX_OBJECT_SPACE_BIT	0x40
#define		TEX_CODEC_MASK			(0xFF^(TEX_NORMAL_BIT|TEX_OBJECT_SPACE_BIT))

#define		COS_RESIZE


CONSTRUCT_ENUMERATION6(PixelType, None, Color, StrictColor, Depth,ObjectSpaceNormal, TangentSpaceNormal);
//StrictColor enforces loading 1 and 2 channel images as R and RG texturesk, respectively. Effective only when loading images into VRAM, mostly identical to Color otherwise



class ImageRandomSource : std::mt19937
{
public:
	typedef std::mt19937	Super;
	ImageRandomSource(unsigned long seed) : Super(std::max<unsigned long>(1,seed))
	{}


	float	operator()()	{return float(Super::operator()()) / 4294967295.f;}


};


/**
	@brief Basic image implementation
*/
class BaseImage
{
public:
		typedef UINT32					dimension_t,dim_t;	
			
		struct THeader	//! File header used for certain texture io-operations
		{
					UINT32				crc;	//!< Image crc32 checksum (including settings)
			union
			{
				struct
				{
					BYTE				x_exp,	//!< Size X-exponent (final image width = 2^x_exp)
										y_exp,	//!< Size Y-exponent (final image height = 2^y_exp)
										channels,	//!< Image color channels
										type;		//!< Image content type
				};
				UINT32					settings;
			};
		};						

protected:
		dimension_t				image_width,		//!< Width of the image in pixels.
								image_height;		//!< Height of the image in pixels.
		BYTE					image_channels;	//!< Number of color channels
		PixelType				content_type;	//!< Contained image type. Certain methods will behave differently depending on this value.


public:
		String					origin;		//!< Custom string. Possibly a filename. This value is carried along but not used by any method of this file
		enum channel_append_t{	//! Operation to perfom if new channels are appended via the setChannel() method.
								CA_ZERO=0,	//!< Set the content of new channels to 0 (black)
								CA_REPEAT,	//!< Repeat the content of the last defined channel
								CA_UNDEF,	//!< Leave the content of new channels undefined
								CA_HALF,	//!< Set the content of new channels to half the maximum possible value (grey)
								CA_MAX		//!< Set the content of new channels to the maximum possible value (white)
								};
							
		enum combiner_t{	//! Operation to perform when drawing images into the local image.
								IC_COPY,			//!< Overwrites the local pixel data with the provided remote pixel data
								IC_ALPHA_BLEND,		//!< Blends the provided remote pixel data above the local pixel data (using the remote 4th channel as opacity value)
								IC_MULTIPLY,		//!< Each channel of the local pixel data will be multiplied with the respective channel of the remote pixel data
								IC_MULT_NEGATIVE,	//!< Each channel of the local pixel data will be negative multiplied with the respective channel of the remote pixel data
								IC_ALPHA_ADD,		//!< The pixel data will be multipled with any available alpha channel and then added. Behaves identical to IC_ADD if no alpha channel is available
								IC_ADD,				//!< The pixel data of the remote image will be added to the pixel data of the local image (and clamped to [0,255] if necessary).
							
							
								Copy=IC_COPY,
								Replace=IC_COPY,
								AlphaBlend=IC_ALPHA_BLEND,
								Multiply=IC_MULTIPLY,
								MultNegative=IC_MULT_NEGATIVE,
								AlphaAdd=IC_ALPHA_ADD,
								Blend=IC_ALPHA_ADD,
								Add=IC_ADD
								};

								BaseImage(dimension_t width=0, dimension_t height=0, BYTE channels=3, PixelType type=PixelType::Color):image_width(width),image_height(height),image_channels(channels),content_type(type)
								{}
virtual							~BaseImage()	{};
		dimension_t				width()																const;	//!< Queries current image width \return Image width in pixels
		dimension_t				getWidth()															const;	//!< Queries current image width \return Image width in pixels
		dimension_t				height()															const;	//!< Queries current image height \return Image height in pixels
		dimension_t				getHeight()															const;	//!< Queries current image height \return Image height in pixels
		float					aspect()															const;	//!< Queries the pixel aspect (width/height) of the local image
		float					getAspect()															const;	//!< Queries the pixel aspect (width/height) of the local image
		dimension_t				dimension(BYTE index)												const; 	//!< Queries current image width/height \param index Coordinate index to retrieve (0=width, 1=height) \return Image width or height in pixels
		dimension_t				getDimension(BYTE index)											const; 	//!< Queries current image width/height \param index Coordinate index to retrieve (0=width, 1=height) \return Image width or height in pixels
		bool					isEmpty()															const;	//!< Checks if the local image contains at least one pixel and one channel, false otherwise
		bool					isNotEmpty()														const;	//!< identical to !isEmpty()		
		BYTE					channels()															const;	//!< Queries current image color channels \return Number of color channels
		BYTE					getChannels()														const;	//!< Queries current image color channels \return Number of color channels
		PixelType				contentType()														const;	//!< Queries content type \return content type
		PixelType				getContentType()													const;	//!< Queries content type \return content type
static	PixelType				contentType(const THeader&header);											//!< Extracts the content type from the provided image header
static	PixelType				getContentType(const THeader&header);										//!< Extracts the content type from the provided image header
		String					contentTypeString()													const;	//!< Generates a short string describing the current content type \return content type string
		String					getContentTypeString()												const;	//!< Generates a short string describing the current content type \return content type string
		void					setContentType(PixelType type);												//!< Sets content type. Does not change the actual data.
		bool					writeHeader(THeader&header)											const;	//!< Generates a header from the local image data \return true if the local image coordinates could be transformed into exponents, false otherwise. The resulting header will contain approximated exponents if false is returned
		THeader					header()															const;	//!< Generates a header from the local image data
		THeader					getHeader()															const;	//!< Generates a header from the local image data
		String					toString()															const;		//!< Generates a string representation of the local image (width, height, channels, type, etc)
		bool					isColorMap()														const;	//!< Checks if the local map contains color pixels
		bool					isNormalMap()														const;	//!< Checks if the local map contains normal pixels
};


/**
	@brief General image template

	This object may hold integer and floating point channel values and provides only methods
	that may seamlessly be applied to either.
*/
template <typename T>
	class ImageTemplate:public BaseImage
	{
	protected:
			T						*image_data;		//!< Pointer to the actual image data.

		template <typename Float>
			void					sample(Float x0, Float y0, Float x1, Float y1, T*target)		const;

			bool					columnIsOpaque(dimension_t x)	const;
			bool					rowIsOpaque(dimension_t y)		const;



	public:

									ImageTemplate();
									ImageTemplate(const ImageTemplate<T>&other);									//!< Copy constructor
									ImageTemplate(dimension_t width, dimension_t height, BYTE channels=3,PixelType type=PixelType::Color);		//!< Direct constructor. Directly allocates a field of the specified size. \param width Image width \param height Image height, \param channels Number of color channels. \param type Image content type
									ImageTemplate(const THeader&header);															//!< Header constructor. Configures the local image based on the information stored in the provided header
	virtual							~ImageTemplate();
			ImageTemplate<T>&		operator=(const ImageTemplate<T>&other);												//!< Copy assignment
			void					free()
									{
										dealloc(image_data);
										image_data = NULL;
										image_width = 0;
										image_height = 0;
										image_channels = 3;
										content_type = PixelType::Color;
									}
			void					applyHeader(const THeader&header);											//!< Changes the size, channels, and type of the local image based on the provided header. Any content in the local image will be lost if the size of the local color array changes
	inline	T*						getData();																	//!< Retrieves a pointer to the actual pixel data of the image \return Pointer to the image pixel data array. The returned array <b>must not be deleted</b>
	inline	const T*				getData()															const;	//!< Const version of getData().
	inline	T*						data();																	//!< Retrieves a pointer to the actual pixel data of the image \return Pointer to the image pixel data array. The returned array <b>must not be deleted</b>
	inline	const T*				data()																const;	//!< Const version of getData().
			void					adoptData(ImageTemplate<T>&other);													//!< Clears any local data and adopts all pointers of the other image. The respective other image will be empty when the operation is executed.
			void					swap(ImageTemplate<T>&other);													//!< Swaps all pointers and attributes with the other image.
			void					setDimensions(dimension_t width, dimension_t height, BYTE channels);				//!< Alters the dimensions of the local image. The content of the pixel data field is lost if it is resized during this operation. \param width New image width in pixels \param height New image height in pixels. \param channels New number of color channels.
			void					setDimension(dimension_t width, dimension_t height, BYTE channels);				//!< Identical to setDimensions()
			void					setSize(dimension_t width, dimension_t height, BYTE channels);							//!< Identical to setDimensions()
	inline	void					setChannel(dimension_t X, dimension_t Y, BYTE channel, T newData);	//!< Alters the content of one color channel of one specific pixel.  \param X X-coordinate of the pixel (0 = left most pixel, must be valid) \param Y Y-coordinate of the pixel (usually 0 = bottom most pixel, must be valid) \param channel Target channel (must be valid) \param newData New value for the specified channel of the specified pixel.
			void					setChannel(BYTE channel, T new_value);							//!< Alters the content of one color channel of all pixels  \param channel Target channel (must be valid) \param newData New value for the specified channel of all pixels.
			void					paintRect(dimension_t left, dimension_t bottom, dimension_t width, dimension_t height, T r, T g, T b);
			void					noiseFillChannel(BYTE channel, T min_value, T max_value);		//!< Alters the content of one color channel of all pixels to random values \param channel Target channel (must be valid) \param min_value Minimum channel value @param max_value maximum channel value
			void					noiseFillChannel(BYTE channel, T min_value, T max_value, unsigned seed);		//!< Alters the content of one color channel of all pixels to random values \param channel Target channel (must be valid) \param min_value Minimum channel value @param max_value maximum channel value
			void					blurChannel(BYTE channel, float radius, bool loop);					//!< Blurs the specified image channel (all other channels are left unchanged) @param channel Channel to blur @param radius Radius controling the blur intensity @param loop Image is seamless and blurring should loop around image borders
			void					linearBlurChannel(BYTE channel, float radius, bool loop);					//!< Blurs the specified image channel (all other channels are left unchanged). Separates horizontal from vertical blurring @param channel Channel to blur @param radius Radius controling the blur intensity @param loop Image is seamless and blurring should loop around image borders
			void					fill(T red, T green, T blue, T alpha=TypeInfo<T>::zero);			//!< Fills the lower 4 (or less if less) channels of the local image with the specified values.
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least 3 entries
									
										set3() overwrites the local pixel data with the specified color ( @b data). The method does not check for correctness and assumes
										the local image has at least 3 channels.
										Corrupted x/y coordinates or insufficient entries in the \b data field will lead to access violations and/or segmentation faults.
									*/
	inline	void					set3(dimension_t X, dimension_t Y, const T*data);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least 4 entries
									
										set3() overwrites the local pixel data with the specified color ( @b data). The method does not check for correctness and assumes
										the local image has at least 4 channels.
										Corrupted x/y coordinates or insufficient entries in the \b data field will lead to access violations and/or segmentation faults.
									*/
	inline	void					set4(dimension_t X, dimension_t Y, const T*data);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least one entry per local channel
									
										set() overwrites the local pixel data with the specified color (\b data). The method does not check for correctness.
										Corrupted x/y coordinates or insufficient entries in the \b data field will lead to access violations and/or segmentation faults.
									*/
	inline 	void					set(dimension_t X, dimension_t Y, const T*data);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least one entry per local channel
										\param channels Number of entries in the provided \b data array.
									
										set() overwrites the local pixel data with the specified color (\b data). If \b channels is less than the local
										number of channels then the last entry in the \b data field will be repeated for the remaining channels.
										The method does not check for correctness. Corrupted x/y coordinates or insufficient entries in the \b data
										field will lead to access violations and/or segmentation faults.
									*/
	inline 	void					set(dimension_t X, dimension_t Y, const T*data, BYTE channels);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param red Red color component
										\param green Green color component
										\param blue Blue color component
										\param alpha Opacity color component 
									
										set() overwrites the local pixel data with the specified color (\b red, \b green, \b blue, \b alpha). Color components that are not
										provided by the local image are ignored. Channels above 4 are left unchanged.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
	inline	void					set(dimension_t X, dimension_t Y, T red, T green, T blue, T alpha); 
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param red Red color component
										\param green Green color component
										\param blue Blue color component
									
										set() overwrites the local pixel data with the specified color (\b red, \b green, \b blue). Color components that are not
										provided by the local image are ignored. Channels above 3 are left unchanged.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
	inline	void					set(dimension_t X, dimension_t Y, T red, T green, T blue); 

	T								sampleChannelAt(float x, float y, BYTE channel,bool loop)	const;
	inline	T*						get(dimension_t X, dimension_t Y)														//! Retrieves a pointer to the color data of the specified pixel. Warning: The method behavior is undefined for invalid pixel coordinates. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \return Pointer to the color data of the specified pixel.
									{
										return image_data+(size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
									}
	inline	const T*				get(dimension_t X, dimension_t Y)											const		//! Retrieves a pointer to the color data of the specified pixel. Warning: The method behavior is undefined for invalid pixel coordinates. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \return Pointer to the color data of the specified pixel.
									{
										return image_data+(size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
									}
	inline	T*						getPixel(dimension_t X, dimension_t Y)														//! Retrieves a pointer to the color data of the specified pixel. Warning: The method behavior is undefined for invalid pixel coordinates. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \return Pointer to the color data of the specified pixel.
									{
										return image_data+(size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
									}
	inline	const T*				getPixel(dimension_t X, dimension_t Y)											const		//! Retrieves a pointer to the color data of the specified pixel. Warning: The method behavior is undefined for invalid pixel coordinates. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \return Pointer to the color data of the specified pixel.
									{
										return image_data+(size_t(Y)*size_t(image_width)+size_t(X))*size_t(image_channels);
									}
	inline 	T*						getVerified(dimension_t X, dimension_t Y);											//!< Retrieves a pointer to the color data of the specified pixel. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \return Pointer to the color data of the specified pixel or NULL if the specified pixel does not exist.
	inline 	const T*				getVerified(dimension_t X, dimension_t Y)									const;		//!< Retrieves a pointer to the color data of the specified pixel. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \return Pointer to the color data of the specified pixel or NULL if the specified pixel does not exist.

			void					swapChannels(BYTE c0, BYTE c1);													//!< Exchanges the content of two channels for all pixels. \param c0 First channel index (0 = first(red) channel) \param c1 Second channel index (0 = first(red) channel).
			void					appendAlpha(const ImageTemplate<T>*other);												//!< Creates/overwrites the local 4th channel with the first channel of the specified other image for all pixels. \param other Pointer to another Image object. \b other is required to be of the exact same dimensions as the local image.
			void					appendAlphaAndDelete(ImageTemplate<T>*other);												//!< Performs appendAlpha(), then deletes \b other. \param other Pointer to another Image object. \b other is required to be of the exact same dimensions as the local image but will be deleted even if that should not be the case.
	inline	size_t					size()																const;		//!< Retrieves the size of the local pixel data. \return Size of the local pixel map in bytes (identical to getWidth()*getHeight()*getChannels()).
			void					readFrom(const ImageTemplate<T>*other);																	//!< Adapts the local image data to the specified image's data. Deprecated. \param other Image to copy data from.
			void					read(const T* data);																			//!< Adopts the local image data to the specified array content. \param data Array to copy from. Must be the exact same size as what size() returns.
			bool					exportRectangle(dimension_t x, dimension_t y, dimension_t width, dimension_t height, T*target)	const;	//!< Exports a rectangular pixel area from the local pixel data. \param x Pixel offset (X) \param y Pixel offset (Y) \param width Pixels in x-direction to export \param height Pixels in y-direction to export. \param target Array to write to. Must be at least (\b width * \b height * getChannels()) elements long.
			bool					importRectangle(dimension_t x, dimension_t y, dimension_t width, dimension_t height, const T*target);	//!< Overwrites a section in the local pixel data. \param x Pixel offset (X) \param y Pixel offset (Y) \param width Pixels in x-direction to overwrite \param height Pixels in y-direction to overwrite. \param target Array to read from. Must be at least (\b width * \b height * getChannels()) elements long.
			void					extractChannels(BYTE channel, BYTE c_num, ImageTemplate<T>&target);								//!< Extracts the specified channel range into the specified target image

			bool					truncateToOpaque();																	//!< Reduces the image to the minimum necessary rectangle covering all opaque pixels. This method has no effect if the image does not have 2 (intensity+opacity) or 4 (rgb+opacity) channels @return True if the local image has been changed, false otherwise

	};



/*!
	\brief Generalized integer image nature
	
	An image using this nature stores 2d pixel maps with a variable number of unsigned integer color channels.
	Color channels are clamped to the range [0,max_channel_value] whenever required, where max_channel_value
	is the largest number the used type can store (e.g. 255/0xFF for unsigned char, 65535/0xFFFF for unsigned short).
	The parameter type <T> may be any integer data type but should be unsigned.
*/
template <typename T>
	struct TIntImageNature
	{
			typedef float			float_type_t;
			typedef	T				channel_value_t;
	static const T					max_channel_value = TypeInfo<T>::max,
									normal_offset = (max_channel_value>>1)+1,
									normal_scale = normal_offset-1;

	/**
		@brief Converts a channel value to float
	*/
	static inline	float			toFloat(T v)
									{
										return float(v)/float(max_channel_value);
									}
	/**
		@brief Converts a float to a channel value
	*/
	template <typename Float>
	static inline	T				floatToChannel(Float v)
									{
										return T(v*Float(max_channel_value));
									}
	/**
		@brief Reads a normal component from a channel value
	*/
	static inline	float			toNormalComponent(T v)
									{
										return (float(v)-float(normal_offset))/float(normal_scale);
									}
	/**
		@brief Converts a normal component to a channel value
	*/
	template <typename Float>
	static inline	T				normalComponentToChannel(Float v)
									{
										return T(normal_offset+v*normal_scale);
									}
	/**
		@brief Clamps a generic (typically float) value to the allowed value range and returns a valid channel value
	*/
	template <typename Generic>
	static inline	T				clamp(Generic v)
									{
										return T(Math::clamp(v,0,max_channel_value));
									}


	};


/*!
	\brief Generalized floating point image nature
	
	An image using this nature stores 2d pixel maps with a variable number of floating point color channels.
	Colors channels are clamped to the range [0,1] whenever required.
	The parameter type <T> may be any float data type (float, double, long double).
*/
template <typename T>
	struct TFloatImageNature
	{
	public:
			typedef T				float_type_t;
			typedef	T				channel_value_t;
	static const T					max_channel_value,
									normal_offset,
									normal_scale;

	static inline	T				toFloat(T v)
									{
										return v;
									}
	template <typename Float>
	static inline	T				floatToChannel(Float v)
									{
										return v;
									}
	static inline	T				toNormalComponent(T v)
									{
										return ((v)-(normal_offset))/(normal_scale);
									}
	template <typename Float>
	static inline	T				normalComponentToChannel(Float v)
									{
										return (normal_offset+v*normal_scale);
									}
	template <typename Generic>
	static inline	T				clamp(Generic v)
									{
										return T(Math::clamp(v,0,1));
									}

	};

/*!
	\brief Generalized unclamped floating point image nature
	
	An image using this nature stores 2d pixel maps with a variable number of floating point color channels.
	This nature does not clamp values to the range [0,1] during certain operations.
*/
template <typename T>
	struct TUnclampedFloatImageNature:public TFloatImageNature<T>
	{
	public:
	static inline	T				toNormalComponent(T v)
									{
										return v;
									}
	template <typename Float>
	static inline	T				normalComponentToChannel(Float v)
									{
										return v;
									}
	template <typename Generic>
	static inline	T				clamp(Generic v)
									{
										return v;
									}

	};	

template <typename T>
	const T	TFloatImageNature<T>::max_channel_value = T(1);
template <typename T>
	const T	TFloatImageNature<T>::normal_offset = T(0.5);
template <typename T>
	const T	TFloatImageNature<T>::normal_scale = T(0.5);

/**
	@brief Generic image class using a nature template class to specify how to handle
	contained pixel values.

	A nature must specify the following members to be usable by GenericImage:<br>
	<br>
			typedef ...					float_type_t;							//floating point variable type that can properly represent a channel value in floating point space. e.g. float<br>
			typedef	...					channel_value_t;						//data variable type. this type is used to store channel values. e.g. unsigned char<br>
	static const T						max_channel_value = ...;				//maximum channel value. e.g. 255<br>
	<br>
	<br>
	static inline	float_type_t		toFloat(channel_value_t v);	//converts a channel value to float<br>
	template &lt;typename Float&gt;<br>
	static inline	channel_value_t		floatToChannel(Float v);	//converts a float value into a channel value<br>
	static inline	float_type_t		toNormalComponent(channel_value_t v);		//converts a channel value into a normal component (x, y, or z)<br>
	template &lt;typename Float&gt;<br>
	static inline	channel_value_t		normalComponentToChannel(Float v);	//converts a normal component into a channel value<br>
	template &lt;typename Generic&gt;<br>
	static inline	channel_value_t		clamp(Generic v);		//clamps a generic value to the allowed range and returns it as a channel value. The nature may choose not to clamp<br>
	<br>
	<br>
	Predefined natures are TIntImageNature<...>, TFloatImageNature<...>, and TUnclampedFloatImageNature<...>

*/
template <class Nature>
	class GenericImage:public ImageTemplate<typename Nature::channel_value_t>
	{
	protected:
		typedef size_t	Size;
		typedef Nature	ImageNature;

		template <typename Float>
			void					sampleNormal(Float x0, Float y0, Float x1, Float y1, TVec3<Float>&target)		const;

	public:
		typedef ImageTemplate<typename Nature::channel_value_t>	Parent;
		#ifdef __GNUC__
			using Parent::image_channels;
			using Parent::image_width;
			using Parent::image_height;
			using Parent::image_data;
			using Parent::origin;
			using Parent::content_type;
			using Parent::get;

			using Parent::combiner_t;


			using Parent::IC_COPY;
			using Parent::IC_ALPHA_BLEND;
			using Parent::IC_ALPHA_ADD;
			using Parent::IC_ADD;
			using Parent::IC_MULTIPLY;
			using Parent::IC_MULT_NEGATIVE;
		#endif

			typedef typename Nature::float_type_t			F;
			typedef typename Nature::channel_value_t			T;
			static const T max;


									GenericImage()	{};
									GenericImage(dimension_t width, dimension_t height, BYTE channels=3,PixelType type=PixelType::Color):ImageTemplate<T>(width,height,channels,type)
									{}
									GenericImage(const BaseImage::THeader&header):ImageTemplate<T>(header)
									{}
			
			void					scaleChannel(BYTE channel, F factor);							//!< Magnifies or shrinks the specified channel value of all pixels
			void					scaleChannel(BYTE channel, F offset, F factor);							//!< Magnifies or shrinks the specified channel value of all pixels
			void					fractalDouble(F noise_level, bool loop);									//!< Resizes the image to double its size (minus 1 (width/height*2-1) pixels if @b loop is not set, plain double otherwise) and fills the pixels inbetween with fractal noise @param noise_level Degree of randomness to apply to intermediate pixels @param loop Set true to interpolate edge pixels using original pixel data from the opposite edge
			void					fractalDouble(F noise_level, bool loop, unsigned seed);									//!< Resizes the image to double its size (minus 1 (width/height*2-1) pixels if @b loop is not set, plain double otherwise) and fills the pixels inbetween with fractal noise @param noise_level Degree of randomness to apply to intermediate pixels @param loop Set true to interpolate edge pixels using original pixel data from the opposite edge
			void					flipHorizontal();																//!< Flips the local pixel data horizontally. Also flips the x-coordinate of normals if the local image is a normal map.
			void					flipVertical();																	//!< Flips the local pixel data vertically. Also flips the y-coordinate of normals if the local image is a normal map.
			void					rotate90CW();																	//!< Rotates the local image 90 degrees clock wise.
			void					rotate90CCW();																	//!< Rotates the local image 90 degrees counter clock wise.
			bool					scaleHalf();																	//!< Scales the image to half its width and height. Normals will be renormalized if necessary. \return true if the local image could be halved, false otherwise.
			bool					scaleXHalf();																	//!< Scales the image to half its width. Normals will be renormalized if necessary. \return true if the local image could be halved, false otherwise.
			bool					scaleYHalf();																	//!< Scales the image to half its height. Normals will be renormalized if necessary. \return true if the local image could be halved, false otherwise.
			void					scaleDouble(bool loop=false);																	//!< Scales the image to double its width and height. Normals will be renormalized if necessary.
			void					scaleXDouble(bool loop=false);																	//!< Scales the image to double its width. Normals will be renormalized if necessary.
			void					scaleYDouble(bool loop=false);																	//!< Scales the image to double its height. Normals will be renormalized if necessary.
			bool					scaleTo(dimension_t width, dimension_t height);										//!< Scales the image closest to the specified width and height via doubling and/or halving. Normals will be renormalized if necessary. \param width Image target width \param height Image target height \return true if the resulting image is exactly the specified size, false otherwise.
			void					resample(dimension_t width, dimension_t height);										//!< Resizes the image using linear interpolation. Normals will be renormalized if necessary. The method in general behaves differently when resizing normal maps. \param width Image target width \param height Image target height
			void					crop(dimension_t offset_x, dimension_t offset_y, dimension_t width, dimension_t height);	//!< Crops the local image to the specified region
			void					truncate(dimension_t width, dimension_t height);										//!< Truncates the image to not be larger than the specified dimensions. The method has no effect if the picture is already that small or smaller
			void					changeSaturation(float scale_by);														//!< Alters the saturation among the first three channels of the local image
			void					power(BYTE channel, float exponent);																	//!< Apply the specified exponent too all pixels (only the specified channel, however)
									/*!
										\brief Draws a remote image into the pixel data of the local image
										\param other Image to draw
										\param combiner Draw rule

										draw() draws the content of the specified other image into the local image content.
										\a other may be larger than the remaining pixels in width and height direction
										relative to \a x_offset and \a y_offset.
									*/
			void					draw(const ImageTemplate<T>*other, dimension_t x_offset, dimension_t y_offset, BaseImage::combiner_t combiner=BaseImage::IC_ALPHA_BLEND);
			void					paint(const ImageTemplate<T>*other, dimension_t x_offset, dimension_t y_offset, BaseImage::combiner_t combiner=BaseImage::IC_ALPHA_BLEND);	//!< Identical to draw()
		template <class Nature1>
			void					toNormalMap(GenericImage<Nature1>&target, F intensity, bool seamless, BYTE height_channel=0)		const;		//!< Generates a normal map from the local color map. Size and type of the specified target image will be adjusted. @param target Image container to write the resulting normal map to @param intensity Intensity of the resulting normal map @param seamless True if border pixels should loop around to determine a seamless normal map @param height_channel Color channel to use as height value to calculate the normals from
			/**
									@brief Generates a normal map from the local color map

									@param target Image to store the final normal map in
									@param x_texel_distance Geometrical distance between two neighboring texels on the x axis (that share the same y coordinate)
									@param y_texel_distance Geometrical distance between two neighboring texels on the y axis (that share the same x coordinate)
									@param height_scale Geometrical variance to be applied to channel values before calculating a texel's normal
									@param seamless True if the left image edge should connect to the right and the bottom the top one, false otherwise
									@param height_channel Channel that should be used to retrieve the 'height' of a texel
			*/
		template <class Nature1>
			void					toNormalMap(GenericImage<Nature1>&target, F x_texel_distance, F y_texel_distance, F height_scale, bool seamless, BYTE height_channel=0)		const;


									/*!
										\brief Overwrites the color of a specific pixel with the specified floating point color
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least 3 entries
									
										set3f() overwrites the local pixel data with the specified color ( @b data). The method does not check for correctness and assumes
										the local image has at least 3 channels. Color values are expected in the range [0,1] and stretched the range of the local channel data type.
										Corrupted x/y coordinates or insufficient entries in the \b data field will lead to access violations and/or segmentation faults.
									*/
	template <typename Float>
	inline	void					set3f(dimension_t X, dimension_t Y, const Float data[3]);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least 4 entries
									
										set3() overwrites the local pixel data with the specified color ( @b data). The method does not check for correctness and assumes
										the local image has at least 4 channels. Color values are expected in the range [0,1] and stretched the range of the local channel data type.
										Corrupted x/y coordinates or insufficient entries in the \b data field will lead to access violations and/or segmentation faults.
									*/
	template <typename Float>
	inline	void					set4f(dimension_t X, dimension_t Y, const Float data[4]);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least one entry per local channel
									
										set() overwrites the local pixel data with the specified color (\b data). The method does not check for correctness.
										Color values are expected in the range [0,1] and stretched the range of the local channel data type.
										Corrupted x/y coordinates or insufficient entries in the \b data field will lead to access violations and/or segmentation faults.
									*/
	template <typename Float>
	inline 	void					setf(dimension_t X, dimension_t Y, const Float*data);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param data Pointer to an array providing at least one entry per local channel
										\param channels Number of entries in the provided \b data array.
									
										set() overwrites the local pixel data with the specified color (\b data). If \b channels is less than the local
										number of channels then the last entry in the \b data field will be repeated for the remaining channels.
										Color values are expected in the range [0,1] and stretched the range of the local channel data type.
										The method does not check for correctness. Corrupted x/y coordinates or insufficient entries in the \b data
										field will lead to access violations and/or segmentation faults.
									*/
	template <typename Float>
	inline 	void					setf(dimension_t X, dimension_t Y, const Float*data, BYTE channels);
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param red Red color component
										\param green Green color component
										\param blue Blue color component
										\param alpha Opacity color component 
									
										set() overwrites the local pixel data with the specified color (\b red, \b green, \b blue, \b alpha). Color components that are not
										provided by the local image are ignored. Channels above 4 are left unchanged. Color values are expected in the range [0,1] and stretched the range of the local channel data type.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
	template <typename Float>
	inline	void					setf(dimension_t X, dimension_t Y, Float red, Float green, Float blue, Float alpha); 
									/*!
										\brief Overwrites the color of a specific pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel
										\param red Red color component
										\param green Green color component
										\param blue Blue color component
									
										set() overwrites the local pixel data with the specified color (\b red, \b green, \b blue). Color components that are not
										provided by the local image are ignored. Channels above 3 are left unchanged.
										Color values are expected in the range [0,1] and stretched the range of the local channel data type.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
	template <typename Float>
	inline	void					setf(dimension_t X, dimension_t Y, Float red, Float green, Float blue); 




									/*!
										\brief Overwrites the color of a specific pixel with a normal.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel 
										\param data Pointer to a 3 element float normal vector
									
										setNormal() transforms the specified normal to a rgb color vector (-1.0f .. 1.0f will be translated to normal_offset+p*normal_scale).
										The final color is undefined if one of the normal components exceeds the range [-1,+1]. The method does not check for parameter validity and may behave
										unexpectedly or even crash if the contained data is not a 3 channel normal map or if the specified coordinates are invalid.
									*/
		template <typename Float>
			void					setNormal(dimension_t X, dimension_t Y, const TVec3<Float>&data); //-1.0f .. 1.0f will be translated to normal_offset+p*normal_scale

									/*!
										\brief Modifies the color of the specified pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel 
										\param data Pointer to an array providing at least one entry per local channel 
										\param alpha Opacity of the specified color.
									
										blend() blends the specified color above the local pixel data.  \alpha determines
										the opacity of the specified color (0 = base pixel remains unchanged, up to 255 = base pixel will be replaced by the specified color).
										Any values between 0 and 255 will cause a crossfade between the base pixel color and the specified color.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
			void					blend(dimension_t X, dimension_t Y, const T*data,T alpha);
									/*!
										\brief Modifies the color of the specified pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel 
										\param data Pointer to an array providing at least \b channels entries.
										\param alpha Opacity of the specified color.
										\param channels Number of entries in the provided \b data array.
									
										blend() blends the specified color above the local pixel data.  The first \b channels are blended,
										the remaining ignored. \alpha determines the opacity of the specified color (0 = base pixel remains
										unchanged, up to 255 = base pixel will be replaced by the specified color).
										Any values between 0 and 255 will cause a crossfade between the base pixel color and the specified color.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
			void					blend(dimension_t X, dimension_t Y, const T*data,T alpha, BYTE channels);
									/*!
										\brief Modifies the color of the specified pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel 
										\param data Pointer to a float array providing at least one entry per local channel 
										\param alpha Opacity of the specified color.
									
										blend() blends the specified color above the local pixel data.  The float values in the specified float array will be
										translated from 0.0 = 0 to 1.0 = 255. \alpha determines the opacity of the specified color (0.0 = base pixel remains
										unchanged, up to 1.0 = base pixel will be replaced by the specified color).
										Any values between 0 and 1 will cause a crossfade between the base pixel color and the specified color.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
		template <typename Float>
			void					blendf(dimension_t X, dimension_t Y, const Float*data, Float alpha);
									/*!
										\brief Modifies the color of the specified pixel.
										\param X X-coordinate of the pixel
										\param Y Y-coordinate of the pixel 
										\param data Pointer to an array providing at least \b channels entries.
										\param alpha Opacity of the specified color.
										\param channels Number of entries in the provided \b data array.
									
										blend() blends the specified color above the local pixel data.  The float values in the specified float array will be
										translated from 0.0 = 0 to 1.0 = 255. The first \b channels are blended, the remaining ignored.
										\alpha determines the opacity of the specified color (0.0 = base pixel remains unchanged, up to 1.0 = base pixel
										will be replaced by the specified color).
										Any values between 0 and 1 will cause a crossfade between the base pixel color and the specified color.
										The method does not check for correctness. Corrupted x/y coordinates will lead to access violations and/or
										segmentation faults.
									*/
		template <typename Float>
			void					blendf(dimension_t X, dimension_t Y, const Float*data, Float alpha, BYTE channels);
		template <typename Float>
			void					getNormal(dimension_t X, dimension_t Y, TVec3<Float>&out)						const;		//!< Retrieves the normal content of a pixel. The method behavior is undefined for invalid pixel coordinates. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \param out Normal output field
		template <typename Float>
			bool					getNormalVerified(dimension_t X, dimension_t Y, TVec3<Float>&out)				const;		//!< Retrieves the normal content of a pixel. \param X X-coordinate of the pixel \param Y Y-coordinate of the pixel  \param out Normal output field \return true if the specified pixel exists and a normal could be extracted, false otherwise.
		template <typename Float>
			bool					importRectanglef(dimension_t x, dimension_t y, dimension_t w, dimension_t h, const Float*target);
		template <typename Float>
			bool					exportRectanglef(dimension_t x, dimension_t y, dimension_t w, dimension_t h, Float*target) const;
			void					setChannels(BYTE new_channels,BaseImage::channel_append_t append = BaseImage::CA_REPEAT);			//!< Alters the number of channels in the local image. \param new_channels New number of color channels. \param append Action to perform if the new number of channels is greater than the old one.



	};

template <class Nature>
	const typename GenericImage<Nature>::T GenericImage<Nature>::max = Nature::max_channel_value;

typedef GenericImage<TIntImageNature<BYTE> >				Image;					//!< Standard 8bpc integer image. Smallest, least precise image type supported
typedef GenericImage<TIntImageNature<UINT16> >				Image16;				//!< 16bpc image. Very precise integer image type
typedef GenericImage<TFloatImageNature<float> >				FloatImage;			//!< 32bpc floating point image.
typedef GenericImage<TUnclampedFloatImageNature<float> >	UnclampedFloatImage;	//!< 32bpc floating point image. This image does not clamp channel values or remap for normal extraction/storage.



template <class Nature>
	bool	alignableImages(const GenericImage<Nature>*field, unsigned cnt);	//!< checks whether or not aligning is necessary. Aligning is the process of making a number of images equal in channel count and dimensions @param field Pointer to the first image to check. @param cnt Number of images in the field @return true if aligning is required
template <class Nature>
	bool	alignableImages(const GenericImage<Nature>**field, unsigned cnt);	//!< @overload

template <class Nature>
	void	alignImages(GenericImage<Nature>*field, unsigned cnt);		//!< aligns images' sizes and channels to the detected field-maximum. @param field Pointer to the first image to align @param cnt Number of images in the field
template <class Nature>
	void	alignImages(GenericImage<Nature>**field, unsigned cnt);	//!< @overload



#include "image.tpl.h"

#endif
