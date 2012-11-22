#ifndef gl_helperH
#define gl_helperH

#include "../../gl/gl_enhancement.h"
#include "../../math/vector.h"



namespace GL
{
	MF_DECLARE(void)		vertex(const TVec2<GLfloat>&v)	{glVertex2fv(v.v);}
	MF_DECLARE(void)		vertex(const TVec2<GLdouble>&v)	{glVertex2dv(v.v);}
	MF_DECLARE(void)		vertex(const TVec2<GLshort>&v)	{glVertex2sv(v.v);}
	MF_DECLARE(void)		vertex(const TVec2<GLint>&v)	{glVertex2iv(v.v);}

	MF_DECLARE(void)		vertex(const TVec3<GLfloat>&v)	{glVertex3fv(v.v);}
	MF_DECLARE(void)		vertex(const TVec3<GLdouble>&v)	{glVertex3dv(v.v);}
	MF_DECLARE(void)		vertex(const TVec3<GLshort>&v)	{glVertex3sv(v.v);}
	MF_DECLARE(void)		vertex(const TVec3<GLint>&v)	{glVertex3iv(v.v);}

	MF_DECLARE(void)		vertex(const TVec4<GLfloat>&v)	{glVertex4fv(v.v);}
	MF_DECLARE(void)		vertex(const TVec4<GLdouble>&v)	{glVertex4dv(v.v);}
	MF_DECLARE(void)		vertex(const TVec4<GLshort>&v)	{glVertex4sv(v.v);}
	MF_DECLARE(void)		vertex(const TVec4<GLint>&v)	{glVertex4iv(v.v);}


	MF_DECLARE(void)		texCoord(const TVec1<GLfloat>&v)	{glTexCoord1fv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec1<GLdouble>&v)	{glTexCoord1dv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec1<GLshort>&v)	{glTexCoord1sv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec1<GLint>&v)		{glTexCoord1iv(v.v);}

	MF_DECLARE(void)		texCoord(const TVec2<GLfloat>&v)	{glTexCoord2fv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec2<GLdouble>&v)	{glTexCoord2dv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec2<GLshort>&v)	{glTexCoord2sv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec2<GLint>&v)		{glTexCoord2iv(v.v);}

	MF_DECLARE(void)		texCoord(const TVec3<GLfloat>&v)	{glTexCoord3fv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec3<GLdouble>&v)	{glTexCoord3dv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec3<GLshort>&v)	{glTexCoord3sv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec3<GLint>&v)		{glTexCoord3iv(v.v);}

	MF_DECLARE(void)		texCoord(const TVec4<GLfloat>&v)	{glTexCoord4fv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec4<GLdouble>&v)	{glTexCoord4dv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec4<GLshort>&v)	{glTexCoord4sv(v.v);}
	MF_DECLARE(void)		texCoord(const TVec4<GLint>&v)		{glTexCoord4iv(v.v);}


	MF_DECLARE(void)		color(const TVec3<GLfloat>&v)		{glColor3fv(v.v);}
	MF_DECLARE(void)		color(const TVec3<GLdouble>&v)		{glColor3dv(v.v);}
	MF_DECLARE(void)		color(const TVec3<GLshort>&v)		{glColor3sv(v.v);}
	MF_DECLARE(void)		color(const TVec3<GLint>&v)			{glColor3iv(v.v);}
	MF_DECLARE(void)		color(const TVec3<GLbyte>&v)		{glColor3bv(v.v);}
	MF_DECLARE(void)		color(const TVec3<GLushort>&v)		{glColor3usv(v.v);}
	MF_DECLARE(void)		color(const TVec3<GLuint>&v)		{glColor3uiv(v.v);}
	MF_DECLARE(void)		color(const TVec3<GLubyte>&v)		{glColor3ubv(v.v);}

	MF_DECLARE(void)		color(const TVec4<GLfloat>&v)		{glColor4fv(v.v);}
	MF_DECLARE(void)		color(const TVec4<GLdouble>&v)		{glColor4dv(v.v);}
	MF_DECLARE(void)		color(const TVec4<GLshort>&v)		{glColor4sv(v.v);}
	MF_DECLARE(void)		color(const TVec4<GLint>&v)			{glColor4iv(v.v);}
	MF_DECLARE(void)		color(const TVec4<GLbyte>&v)		{glColor4bv(v.v);}
	MF_DECLARE(void)		color(const TVec4<GLushort>&v)		{glColor4usv(v.v);}
	MF_DECLARE(void)		color(const TVec4<GLuint>&v)		{glColor4uiv(v.v);}
	MF_DECLARE(void)		color(const TVec4<GLubyte>&v)		{glColor4ubv(v.v);}


	MF_DECLARE(void)		normal(const TVec3<GLfloat>&v)		{glNormal3fv(v.v);}
	MF_DECLARE(void)		normal(const TVec3<GLdouble>&v)		{glNormal3dv(v.v);}
	MF_DECLARE(void)		normal(const TVec3<GLshort>&v)		{glNormal3sv(v.v);}
	MF_DECLARE(void)		normal(const TVec3<GLint>&v)		{glNormal3iv(v.v);}
	MF_DECLARE(void)		normal(const TVec3<GLbyte>&v)		{glNormal3bv(v.v);}

	MF_DECLARE(void)		renderWired(const TBox<float>&box)	{glWireCube(box.min.v,box.max.v);}
	MF_DECLARE(void)		renderSolid(const TBox<float>&box)	{glSolidCube(box.min.v,box.max.v);}

};






#endif
