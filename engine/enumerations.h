#ifndef engine_enumerationsH
#define engine_enumerationsH

/******************************************************************

E:\include\engine\enumerations.h

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

namespace Engine
{


	/** \brief Enum Space
		
		Specifies a number of used enumerations.
	*/
	namespace VisualEnum
	{

		/** \enum ePBufferType
			\brief Pixel buffer type
			
			Specifies a buffer type for a new pixel buffer object.
		*/
		enum	ePBufferType
		{
			PBT_RGB, /*!< 24 bpp RGB buffer */
			PBT_RGBA, /*!< 32 bpp RGBA buffer */
			PBT_DEPTH, /*!< 16 bpp depth buffer */
			PBT_RGB_CUBE, /*!< 24 bpp RGB cube map (6 faces) */
			PBT_RGBA_CUBE /*!< 32 bpp RGBA cube map (6 faces) */
		};

		/**
			\brief Texture mode
			
			Specifies which texture mode to use for new texture objects.
		*/
		CONSTRUCT_ENUMERATION4(TextureFilter,None,Linear,Bilinear,Trilinear);



		/**
			\brief Language object state
			
			Characterizes the current state of the specified objekt (texture, vertex-buffer, index-buffer, etc.)
		*/
		enum	eObjectState
		{
			InvalidObject,	/*!< The specified object is invalid */
			EmptyObject,	/*!< The specified object is currently empty */
			OccupiedObject	/*!< The specified object is valid and filled with some sort of data */
		};



		enum	eDepthTest	//! Depth test configuration
		{
			NoDepthTest,		//!< Depth test is disabled
			NormalDepthTest,	//!< Depth test is enabled (test: less or equal)
			InvertedDepthTest	//!< Depth test is enabled with inverted equation (test: greater)
		};


		CONSTRUCT_ENUMERATION5(TextureMapping, Default, None, Normal, Reflection, Refraction);
		CONSTRUCT_ENUMERATION4(SystemType,None,Identity,Environment,Custom);




	}

	using namespace VisualEnum;
	
}

#endif
