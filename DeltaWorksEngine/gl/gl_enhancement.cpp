#include "../global_root.h"
#include "gl_enhancement.h"

/******************************************************************

Collection of useful gl-functions and definitions.

******************************************************************/

namespace Engine
{

	GLvoid		glCross(float x, float y, float z, float spacing, float size)
	{
		float p[3] = {x,y,z};
		glCross(p,spacing,size);
	}

	GLvoid		glCross(float x, float y, float z, float size)
	{
		float p[3] = {x,y,z};
		glCross(p,size);
	}


	const char*         glGetErrorName(GLenum id)
	{
		switch (id)
		{
			case GL_NO_ERROR:           return "No Error";
			case GL_INVALID_ENUM:       return "Invalid Enumeration";
			case GL_INVALID_VALUE:      return "Invalid Value";
			case GL_INVALID_OPERATION:  return "Invalid Operation";
			case GL_STACK_OVERFLOW:     return "Stack-Overflow";
			case GL_STACK_UNDERFLOW:    return "Stack-Underflow";
			case GL_OUT_OF_MEMORY:      return "Out Of Memory";
		}
		return "Unknown Error";
	}


	void                glPrintError()
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		#if SYSTEM==WINDOWS
			MessageBoxA(NULL,"GL_ERROR",glGetErrorName(error),MB_OK);
		#else
			cout << glGetErrorName(error)<<endl;
		#endif
	}


	const char*         getTypeName(GLenum type)
	{
		#define ecase(token)    case token: return #token;
		switch (type)
		{
			ecase(GL_BYTE)
			ecase(GL_UNSIGNED_BYTE)
			ecase(GL_SHORT)
			ecase(GL_UNSIGNED_SHORT)
			ecase(GL_INT)
			ecase(GL_UNSIGNED_INT)
			ecase(GL_FLOAT)
			ecase(GL_DOUBLE)
		}
		return "GL_UNKNOWN";
		#undef ecase
	}

	GLuint      glFormatSize(GLenum internal_format)
	{
		switch (internal_format)
		{
			case 1:
				return 8;
			case 2:
				return 16;
			case 3:
				return 24;
			case 4:
				return 32;
			case GL_ALPHA4:
			case GL_LUMINANCE4:
			case GL_INTENSITY4:
				return 4;
			case GL_ALPHA8:
			case GL_LUMINANCE8:
			case GL_LUMINANCE4_ALPHA4:
			case GL_LUMINANCE6_ALPHA2:
			case GL_INTENSITY:
			case GL_INTENSITY8:
			case GL_R3_G3_B2:
			case GL_RGBA2:
				return 8;
			case GL_ALPHA12:
			case GL_LUMINANCE12:
			case GL_RGB4:
				return 12;
			case GL_LUMINANCE16:
			case GL_LUMINANCE8_ALPHA8:
			case GL_LUMINANCE12_ALPHA4:
			case GL_INTENSITY16:
			case GL_RGBA4:
			case GL_RGB5_A1:
			case GL_ALPHA16:
				return 16;
			case GL_LUMINANCE12_ALPHA12:
			case GL_RGB8:
			case GL_RGB:
				return 24;
			case GL_LUMINANCE16_ALPHA16:
			case GL_RGBA8:
			case GL_RGBA:
			case GL_RGB10_A2:
				return 32;
			case GL_INTENSITY12:
			case GL_RGB5:
				return 15;
			case GL_RGB10:
				return 30;
			case GL_RGB12:
				return 36;
			case GL_RGB16:
			case GL_RGBA12:
				return 48;
			case GL_RGBA16:
				return 64;
		}
		return 0;
	}
}
