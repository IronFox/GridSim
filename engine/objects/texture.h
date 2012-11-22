#ifndef objects_textureH
#define objects_textureH



namespace Engine
{
	CONSTRUCT_ENUMERATION5(TextureDimension, None, Linear, Planar, Volume, Cube);

	typedef TextureDimension	TextureDim;

	/**
		@brief General texture object
	*/
	class GenericTexture
	{
	protected:
			UINT32			texture_width,			//!< Texture width in pixels
							texture_height;			//!< Texture height in pixels
			BYTE			texture_channels;		//!< Number of texture channels (1 to 4)
			TextureDim		texture_dimension;	//!< Effective dimension of this texture
			PixelType		texture_type;			//!< Type of the contained pixels
	public:
							GenericTexture():texture_width(0),texture_height(0),texture_channels(3)
							{}
			
	virtual					~GenericTexture()
							{}
							
			void			adoptData(GenericTexture&other)
							{
								texture_width = other.texture_width;
								texture_height = other.texture_height;
								texture_channels = other.texture_channels;
								texture_dimension = other.texture_dimension;
								other.flush();	//not virtual. safe to call here
							}
			void			swap(GenericTexture&other)
							{
								swp(texture_width,other.texture_width);
								swp(texture_height,other.texture_height);
								swp(texture_channels,other.texture_channels);
								swp(texture_dimension,other.texture_dimension);
							}
			
			void			flush()
							{
								texture_width = 0;
								texture_height = 0;
								texture_channels = 3;
								texture_dimension = TextureDimension::None;
							}
							
	inline	unsigned		width()	const		//! Queries current texture width in pixels
							{
								return texture_width;
							}
							
	inline	unsigned		height()	const		//! Queries current texture height in pixels
							{
								return texture_height;
							}
							
	inline	BYTE			channels()	const		//! Queries current texture channels (1 for luminance, 2 for luminance&alpha, 3 for RGB, 4 for RGBA)
							{
								return texture_channels;
							}
	
	inline	bool			isCube()	const
							{
								return texture_dimension == TextureDimension::Cube;
							}
	inline	TextureDim		dimension()	const
							{
								return texture_dimension;
							}
	inline	bool			isLinear()	const		//! Checks whether or not the local texture is linear (1D)
							{
								return texture_width > 0 && texture_height == 1;
							}
	inline	bool			isTransparent()	const
							{
								return (texture_channels==4 || texture_channels==2);	//RGBA or INTENSITY+ALPHA
							}

	inline	bool			operator==(const GenericTexture&that)
							{
								return this->texture_width == that.texture_width && this->texture_height == that.texture_height && this->texture_channels == that.texture_channels && this->texture_dimension == that.texture_dimension;
							}
							
	inline	bool			operator!=(const GenericTexture&that)	{return !operator==(that);}

	virtual	void			overrideSetAttributes(UINT32 new_width, UINT32 new_height, BYTE new_channels, TextureDimension dim)
							{
								texture_width = new_width;
								texture_height = new_height;
								texture_channels = new_channels;
								texture_dimension = dim;
							}


	};
}



#endif
