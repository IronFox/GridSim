#ifndef gl_enhancementH
#define gl_enhancementH

/******************************************************************

Collection of useful gl-functions and definitions.

******************************************************************/


#include "../gl/gl.h"
#include "../gl/glext.h"
#include <iostream>

//#pragma comment(lib,"psapi.lib")


//using namespace std;

#define GL_FACE_CULL    GL_CULL_FACE

inline	GLvoid		glColor3fva(const GLfloat*c3, GLfloat a);
inline  GLvoid      glTranslate(const GLfloat*v);
inline  GLvoid      glTranslate(const GLdouble*v);
inline  GLvoid      glTranslateBack(const GLfloat*v);
inline  GLvoid      glTranslateBack(const GLdouble*v);
inline  GLvoid      glScale(GLfloat value);
inline  GLvoid      glScale(GLdouble value);
inline  GLvoid      glScale(const GLfloat*v);
inline  GLvoid      glScale(const GLdouble*v);
inline  GLvoid      glMaterial(GLenum face, GLenum pname, GLfloat param);
inline  GLvoid      glMaterial(GLenum face, GLenum pname, const GLfloat *params);
inline  GLvoid      glMaterial(GLenum face, GLenum pname, GLint param);
inline  GLvoid      glMaterial(GLenum face, GLenum pname, const GLint *params);
inline  GLvoid      glVertex2v(const GLint*);
inline  GLvoid      glVertex2v(const GLfloat*);
inline  GLvoid      glVertex2v(const GLdouble*);
inline  GLvoid      glVertex3v(const GLint*);
inline  GLvoid      glVertex3v(const GLfloat*);
inline  GLvoid      glVertex3v(const GLdouble*);
inline  GLvoid      glNormal3v(const GLfloat*);
inline  GLvoid      glNormal3v(const GLdouble*);
template <typename T0, typename T1, typename T2>
	inline	GLvoid		glVertex2fvs(const T0 b[2], const T1 d[2], const T2&factor);
template <typename T0, typename T1>
	inline	GLvoid		glVertex2fvs(const T0 b[2], const T1 d[2]);
template <typename T>
	inline	GLvoid		glVertex3fvs(const T v[3], const T&factor);
template <typename T0, typename T1, typename T2>
	inline	GLvoid		glVertex3fvs(const T0 b[3], const T1 d[3], const T2&factor);
template <typename T0, typename T1>
	inline	GLvoid		glVertex3fvs(const T0 b[3], const T1 d[3]);
template <typename T0, typename T1, typename T2, typename T3, typename T4>
	inline	GLvoid		glVertex2fvs2(const T0 b[2], const T1 d[2], const T2&factor, const T3 d2[2], const T4&factor2);
template <typename T0, typename T1, typename T2, typename T3>
	inline	GLvoid		glVertex2fvs2(const T0 b[2], const T1 d[2], const T2&factor, const T3 d2[2]);
template <typename T0, typename T1, typename T2, typename T3, typename T4>
	inline	GLvoid		glVertex3fvs2(const T0 b[3], const T1 d[3], const T2&factor, const T3 d2[3], const T4&factor2);
template <typename T0, typename T1, typename T2, typename T3>
	inline	GLvoid		glVertex3fvs2(const T0 b[3], const T1 d[3], const T2&factor, const T3 d2[3]);
inline  GLvoid      glWhite(GLfloat alpha=1);
inline  GLvoid      glBlack(GLfloat alpha=1);
inline  GLvoid      glRed(GLfloat alpha=1);
inline  GLvoid      glMaroon(GLfloat alpha=1);
inline  GLvoid      glGreen(GLfloat alpha=1);
inline  GLvoid      glBlue(GLfloat alpha=1);
inline  GLvoid      glYellow(GLfloat alpha=1);
inline  GLvoid      glOrange(GLfloat alpha=1);
inline  GLvoid      glMagenta(GLfloat alpha=1);
inline  GLvoid      glCyan(GLfloat alpha=1);
inline	GLvoid		glGrey(GLfloat intensity, GLfloat alpha=1);
inline  GLvoid      glLineMode();
inline  GLvoid      glFillMode();
        GLuint      glFormatSize(GLenum internal_format);       //size in bits that the respective type consumes per pixel
		
template <typename T>
		GLvoid		glWireCube(const T lower_corner[3], const T upper_corner[3]);	//!< Renders a wire cube	@param lower_corner 3d coordinates of the lower corner @param upper_corner 3d coordinates of the upper corner
template <typename T>
		GLvoid		glSolidCube(const T lower_corner[3], const T upper_corner[3]);
template <typename T>
		GLvoid		glCross(const T center[3], float spacing, float size);
template <typename T>
		GLvoid		glCross(const T center[3], float size);

		GLvoid		glCross(float x, float y, float z, float spacing, float size);
		GLvoid		glCross(float x, float y, float z, float size);
template <typename T>
		GLvoid		glWireCube(const T center[3], float size);
template <typename T>
		GLvoid		glSolidCube(const T center[3], float size);
template <typename T>
		GLvoid		glCubeTexturedSolidCube(const T center[3], float size);

template <typename T>
		GLvoid		gl2dBox(const T center[2], float width, float height);

template <typename T>
		GLvoid		gl2dBox(const T center[2], float size);
/**
	@brief Renders a crossed cardboard along the specified axes (for trees or planet patches)
	The required z axis is derived from the cross product of x and y.
	@param center Patch center
	@param x Normalized cardboard x axis
	@param y Normalized cardboard y axis
	@param width Quad width along the x and z axis
	@param height Quad height along the y axis
*/
template <typename T0, typename T1, typename T2>
		GLvoid		glCardboard(const T0 center[3], const T1 x[3], const T2 y[3], float width, float height);
/**
	@brief Renders a crossed cardboard along the specified axes (for trees or planet patches)
	Similar to the above but with an explicit z axis.
	@param center Patch center
	@param x Normalized cardboard x axis
	@param y Normalized cardboard y axis
	@param z Normalized cardboard z axis
	@param width Quad width along the x and z axis
	@param height Quad height along the y axis
*/
template <typename T0, typename T1, typename T2, typename T3>
		GLvoid		glCardboard(const T0 center[3], const T1 x[3], const T2 y[3], const T3 z[3], float width, float height);

template <typename T0, typename T1, typename T2>
		GLvoid		glCardboardN(const T0 center[3], const T1 x[3], const T2 y[3], float width, float height);	//!< Version of the above with defined normal vectors
template <typename T0, typename T1, typename T2, typename T3>
		GLvoid		glCardboardN(const T0 center[3], const T1 x[3], const T2 y[3], const T3 z[3], float width, float height);	//!< Version of the above with defined normal vectors

const char*         getTypeName(GLenum type);
const char*         glGetErrorName(GLenum id);
void                glPrintError();
#define             glThrowError()      {GLenum error = glGetError(); if (error) FATAL__(glGetErrorName(error));}


struct GLDepthComponent;
struct GLhalf;


/**
	@brief OpenGL type constant
	
	For any OpenGL-compatible type GLType<...>::constant will yield the respective OpenGL enumeration constant
 */
template <typename T>
	struct GLType
	{
		typedef T			Type;
		typedef TFalse		IsSupported;
		static const bool	is_supported = false;
	};




template <class C>  inline  void        glVertex(const C&, const C&, const C&);
template <class C>  inline  GLenum      glType();
template <class C>  inline  void        glLoadMatrix(const C*v);
template <class C>  inline  void        glMultMatrix(const C*v);



#include "gl_enhancement.tpl.h"
#endif
