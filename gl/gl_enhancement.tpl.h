#ifndef gl_enhancementTplH
#define gl_enhancementTplH

/******************************************************************

Collection of useful gl-functions and definitions.


******************************************************************/


template <typename T>
		GLvoid		gl2dBox(const T center[2], float width, float height)
		{
			glBegin(GL_LINE_LOOP);
				glVertex2f(center[0]-width/2,center[1]-height/2);
				glVertex2f(center[0]+width/2,center[1]-height/2);
				glVertex2f(center[0]+width/2,center[1]+height/2);
				glVertex2f(center[0]-width/2,center[1]+height/2);
			glEnd();
		}
		

template <typename T>
		GLvoid		gl2dBox(const T center[2], float size)
		{
			glBegin(GL_LINE_LOOP);
				glVertex2f(center[0]-size/2,center[1]-size/2);
				glVertex2f(center[0]+size/2,center[1]-size/2);
				glVertex2f(center[0]+size/2,center[1]+size/2);
				glVertex2f(center[0]-size/2,center[1]+size/2);
			glEnd();
		}

template <typename T0, typename T1, typename T2>
	GLvoid		glCardboardN(const T0 center[3], const T1 x[3], const T2 y[3], float width, float height)
	{
		float z[3];
		_cross(x,y,z);
		glCardboardN(center,x,y,z,width,height);
	}

template <typename T0, typename T1, typename T2, typename T3>
	GLvoid		glCardboardN(const T0 center[3], const T1 x[3], const T2 y[3], const T3 z[3], float width, float height)
	{
		glBegin(GL_QUADS);
			glNormal3fv(z);
			glTexCoord2f(0,0);
			glVertex3f(	center[0]-x[0]*width/2,
						center[1]-x[1]*width/2,
						center[2]-x[2]*width/2);
			
			glTexCoord2f(1,0);
			glVertex3f(	center[0]+x[0]*width/2,
						center[1]+x[1]*width/2,
						center[2]+x[2]*width/2);
		
			glTexCoord2f(1,1);
			glVertex3f(	center[0]+x[0]*width/2+y[0]*height,
						center[1]+x[1]*width/2+y[1]*height,
						center[2]+x[2]*width/2+y[2]*height);
		
			glTexCoord2f(0,1);
			glVertex3f(	center[0]-x[0]*width/2+y[0]*height,
						center[1]-x[1]*width/2+y[1]*height,
						center[2]-x[2]*width/2+y[2]*height);
			
			glNormal3fv(x);
			glTexCoord2f(0,0);
			glVertex3f(	center[0]-z[0]*width/2,
						center[1]-z[1]*width/2,
						center[2]-z[2]*width/2);
			
			glTexCoord2f(1,0);
			glVertex3f(	center[0]+z[0]*width/2,
						center[1]+z[1]*width/2,
						center[2]+z[2]*width/2);
		
			glTexCoord2f(1,1);
			glVertex3f(	center[0]+z[0]*width/2+y[0]*height,
						center[1]+z[1]*width/2+y[1]*height,
						center[2]+z[2]*width/2+y[2]*height);
		
			glTexCoord2f(0,1);
			glVertex3f(	center[0]-z[0]*width/2+y[0]*height,
						center[1]-z[1]*width/2+y[1]*height,
						center[2]-z[2]*width/2+y[2]*height);
		
		
		
		glEnd();
	}

template <typename T0, typename T1, typename T2>
	GLvoid		glCardboard(const T0 center[3], const T1 x[3], const T2 y[3], float width, float height)
	{
		float z[3];
		_cross(x,y,z);
		glCardboard(center,x,y,z,width,height);
	}

template <typename T0, typename T1, typename T2, typename T3>
	GLvoid		glCardboard(const T0 center[3], const T1 x[3], const T2 y[3], const T3 z[3], float width, float height)
	{
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(	center[0]-x[0]*width/2,
						center[1]-x[1]*width/2,
						center[2]-x[2]*width/2);
			
			glTexCoord2f(1,0);
			glVertex3f(	center[0]+x[0]*width/2,
						center[1]+x[1]*width/2,
						center[2]+x[2]*width/2);
		
			glTexCoord2f(1,1);
			glVertex3f(	center[0]+x[0]*width/2+y[0]*height,
						center[1]+x[1]*width/2+y[1]*height,
						center[2]+x[2]*width/2+y[2]*height);
		
			glTexCoord2f(0,1);
			glVertex3f(	center[0]-x[0]*width/2+y[0]*height,
						center[1]-x[1]*width/2+y[1]*height,
						center[2]-x[2]*width/2+y[2]*height);
			
			glTexCoord2f(0,0);
			glVertex3f(	center[0]-z[0]*width/2,
						center[1]-z[1]*width/2,
						center[2]-z[2]*width/2);
			
			glTexCoord2f(1,0);
			glVertex3f(	center[0]+z[0]*width/2,
						center[1]+z[1]*width/2,
						center[2]+z[2]*width/2);
		
			glTexCoord2f(1,1);
			glVertex3f(	center[0]+z[0]*width/2+y[0]*height,
						center[1]+z[1]*width/2+y[1]*height,
						center[2]+z[2]*width/2+y[2]*height);
		
			glTexCoord2f(0,1);
			glVertex3f(	center[0]-z[0]*width/2+y[0]*height,
						center[1]-z[1]*width/2+y[1]*height,
						center[2]-z[2]*width/2+y[2]*height);
		
		
		
		glEnd();
	}


template <typename T>
	GLvoid		glSolidCube(const T p0[3], const T p1[3])
	{
		glBegin(GL_QUADS);
			glVertex3f(p0[0],p0[1],p0[2]);
			glVertex3f(p0[0],p1[1],p0[2]);
			glVertex3f(p1[0],p1[1],p0[2]);
			glVertex3f(p1[0],p0[1],p0[2]);
		
			glVertex3f(p0[0],p1[1],p1[2]);
			glVertex3f(p0[0],p0[1],p1[2]);
			glVertex3f(p1[0],p0[1],p1[2]);
			glVertex3f(p1[0],p1[1],p1[2]);
		
			glVertex3f(p0[0],p0[1],p0[2]);
			glVertex3f(p0[0],p0[1],p1[2]);
			glVertex3f(p0[0],p1[1],p1[2]);
			glVertex3f(p0[0],p1[1],p0[2]);
		
			glVertex3f(p1[0],p0[1],p1[2]);
			glVertex3f(p1[0],p0[1],p0[2]);
			glVertex3f(p1[0],p1[1],p0[2]);
			glVertex3f(p1[0],p1[1],p1[2]);
		
			glVertex3f(p0[0],p0[1],p0[2]);
			glVertex3f(p1[0],p0[1],p0[2]);
			glVertex3f(p1[0],p0[1],p1[2]);
			glVertex3f(p0[0],p0[1],p1[2]);
		
			glVertex3f(p1[0],p1[1],p0[2]);
			glVertex3f(p0[0],p1[1],p0[2]);
			glVertex3f(p0[0],p1[1],p1[2]);
			glVertex3f(p1[0],p1[1],p1[2]);
		
		glEnd();
	}
	
template <typename T>
	GLvoid		glCubeTexturedSolidCube(const T p0[3], const T p1[3])
	{
		glBegin(GL_QUADS);
			#undef _POINT_
			#define _POINT_(x,y,z)	glTexCoord3f(x,y,z); glVertex3f(x,y,z);
			glNormal3f(0,0,-1);
			_POINT_(p0[0],p0[1],p0[2]);
			_POINT_(p0[0],p1[1],p0[2]);
			_POINT_(p1[0],p1[1],p0[2]);
			_POINT_(p1[0],p0[1],p0[2]);
		
			glNormal3f(0,0,1);
			_POINT_(p0[0],p1[1],p1[2]);
			_POINT_(p0[0],p0[1],p1[2]);
			_POINT_(p1[0],p0[1],p1[2]);
			_POINT_(p1[0],p1[1],p1[2]);
		
			glNormal3f(-1,0,0);
			_POINT_(p0[0],p0[1],p0[2]);
			_POINT_(p0[0],p0[1],p1[2]);
			_POINT_(p0[0],p1[1],p1[2]);
			_POINT_(p0[0],p1[1],p0[2]);
		
			glNormal3f(1,0,0);
			_POINT_(p1[0],p0[1],p1[2]);
			_POINT_(p1[0],p0[1],p0[2]);
			_POINT_(p1[0],p1[1],p0[2]);
			_POINT_(p1[0],p1[1],p1[2]);
		
			glNormal3f(0,-1,0);
			_POINT_(p0[0],p0[1],p0[2]);
			_POINT_(p1[0],p0[1],p0[2]);
			_POINT_(p1[0],p0[1],p1[2]);
			_POINT_(p0[0],p0[1],p1[2]);
		
			glNormal3f(0,1,0);
			_POINT_(p1[0],p1[1],p0[2]);
			_POINT_(p0[0],p1[1],p0[2]);
			_POINT_(p0[0],p1[1],p1[2]);
			_POINT_(p1[0],p1[1],p1[2]);
		
		glEnd();
	}
	
template <typename T>
	GLvoid		glWireCube(const T p0[3], const T p1[3])
	{
		glBegin(GL_LINES);
				
			//connections from 0,0,0				
			glVertex3f(p0[0],p0[1],p0[2]);
			glVertex3f(p1[0],p0[1],p0[2]);
			
			glVertex3f(p0[0],p0[1],p0[2]);
			glVertex3f(p0[0],p1[1],p0[2]);
			
			glVertex3f(p0[0],p0[1],p0[2]);
			glVertex3f(p0[0],p0[1],p1[2]);
			
			
			glVertex3f(p0[0],p0[1],p1[2]);
			glVertex3f(p0[0],p1[1],p1[2]);
			
			glVertex3f(p0[0],p0[1],p1[2]);
			glVertex3f(p1[0],p0[1],p1[2]);
			
			
			glVertex3f(p0[0],p1[1],p0[2]);
			glVertex3f(p0[0],p1[1],p1[2]);
			
			glVertex3f(p0[0],p1[1],p0[2]);
			glVertex3f(p1[0],p1[1],p0[2]);
			
			glVertex3f(p1[0],p0[1],p0[2]);
			glVertex3f(p1[0],p1[1],p0[2]);
		
			glVertex3f(p1[0],p0[1],p0[2]);
			glVertex3f(p1[0],p0[1],p1[2]);
		
			glVertex3f(p1[0],p1[1],p0[2]);
			glVertex3f(p1[0],p1[1],p1[2]);
			
			glVertex3f(p1[0],p0[1],p1[2]);
			glVertex3f(p1[0],p1[1],p1[2]);
			
			glVertex3f(p0[0],p1[1],p1[2]);
			glVertex3f(p1[0],p1[1],p1[2]);
		
		glEnd();
	}
	
template <typename T>
	GLvoid		glWireCube(const T c[3], float size)
	{
		T	lower[3] = {c[0]-size,c[1]-size,c[2]-size},
			upper[3] = {c[0]+size,c[1]+size,c[2]+size};
		glWireCube(lower,upper);
	}
	
template <typename T>
	GLvoid		glSolidCube(const T c[3], float size)
	{
		T	lower[3] = {c[0]-size,c[1]-size,c[2]-size},
			upper[3] = {c[0]+size,c[1]+size,c[2]+size};
		glSolidCube(lower,upper);
	}
	
template <typename T>
	GLvoid		glCubeTexturedSolidCube(const T c[3], float size)
	{
		T	lower[3] = {c[0]-size,c[1]-size,c[2]-size},
			upper[3] = {c[0]+size,c[1]+size,c[2]+size};
		glCubeTexturedSolidCube(lower,upper);
	}
	
template <typename T>
	GLvoid		glCross(const T c[3], float spacing, float size)
	{
		glBegin(GL_LINES);
			glVertex3f(c[0]+spacing,c[1],c[2]);
			glVertex3f(c[0]+size,c[1],c[2]);
			glVertex3f(c[0]-spacing,c[1],c[2]);
			glVertex3f(c[0]-size,c[1],c[2]);
		
			glVertex3f(c[0],c[1]+spacing,c[2]);
			glVertex3f(c[0],c[1]+size,c[2]);
			glVertex3f(c[0],c[1]-spacing,c[2]);
			glVertex3f(c[0],c[1]-size,c[2]);
		
			glVertex3f(c[0],c[1],c[2]+spacing);
			glVertex3f(c[0],c[1],c[2]+size);
			glVertex3f(c[0],c[1],c[2]-spacing);
			glVertex3f(c[0],c[1],c[2]-size);
		
		glEnd();
	}
	
template <typename T>
	GLvoid		glCross(const T c[3], float size)
	{
		glBegin(GL_LINES);
			glVertex3f(c[0]+size,c[1],c[2]);
			glVertex3f(c[0]-size,c[1],c[2]);
		
			glVertex3f(c[0],c[1]+size,c[2]);
			glVertex3f(c[0],c[1]-size,c[2]);
		
			glVertex3f(c[0],c[1],c[2]+size);
			glVertex3f(c[0],c[1],c[2]-size);
		glEnd();
	}

inline	GLvoid		glColor3fva(const GLfloat*c3, GLfloat a)
{
	glColor4f(c3[0],c3[1],c3[2],a);
}

inline  GLvoid      glTranslate(const GLfloat*v)
{
    glTranslatef(v[0],v[1],v[2]);
}

inline  GLvoid      glTranslate(const GLdouble*v)
{
    glTranslated(v[0],v[1],v[2]);
}


inline  GLvoid      glTranslateBack(const GLfloat*v)
{
    glTranslatef(-v[0],-v[1],-v[2]);
}

inline  GLvoid      glTranslateBack(const GLdouble*v)
{
    glTranslated(-v[0],-v[1],-v[2]);
}

inline  GLvoid      glScale(GLfloat value)
{
    glScalef(value,value,value);
}

inline  GLvoid      glScale(GLdouble value)
{
    glScaled(value,value,value);
}

inline  GLvoid      glScale(const GLfloat*v)
{
    glScalef(v[0],v[1],v[2]);
}

inline  GLvoid      glScale(const GLdouble*v)
{
    glScaled(v[0],v[1],v[2]);
}

inline  GLvoid      glVertex2v(const GLint*v)
{
    glVertex2iv(v);
}

inline  GLvoid      glVertex2v(const GLfloat*v)
{
    glVertex2fv(v);
}

inline  GLvoid      glVertex2v(const GLdouble*v)
{
    glVertex2dv(v);
}

inline  GLvoid      glVertex3v(const GLint*v)
{
    glVertex3iv(v);
}

inline  GLvoid      glVertex3v(const GLfloat*v)
{
    glVertex3fv(v);
}

inline  GLvoid      glVertex3v(const GLdouble*v)
{
    glVertex3dv(v);
}

inline  GLvoid      glNormal3v(const GLfloat*v)
{
    glNormal3fv(v);
}

inline  GLvoid      glNormal3v(const GLdouble*v)
{
    glNormal3dv(v);
}

template <typename T0, typename T1, typename T2>
	inline	GLvoid		glVertex2fvs(const T0 b[2], const T1 d[2], const T2&scalar)
	{
		glVertex2f(b[0]+d[0]*scalar,b[1]+d[1]*scalar);
	}

template <typename T0, typename T1>
	inline	GLvoid		glVertex2fvs(const T0 b[2], const T1 d[2])
	{
		glVertex2f(b[0]+d[0],b[1]+d[1]);
	}

template <typename T>
	inline	GLvoid		glVertex3fvs(const T v[3], const T&factor)
	{
		glVertex3f(v[0]*factor,v[1]*factor,v[2]*factor);
	}

template <typename T0, typename T1, typename T2>
	inline	GLvoid		glVertex3fvs(const T0 b[3], const T1 d[3], const T2&scalar)
	{
		glVertex3f(b[0]+d[0]*scalar,b[1]+d[1]*scalar,b[2]+d[2]*scalar);
	}

template <typename T0, typename T1>
	inline	GLvoid		glVertex3fvs(const T0 b[3], const T1 d[3])
	{
		glVertex3f(b[0]+d[0],b[1]+d[1],b[2]+d[2]);
	}

template <typename T0, typename T1, typename T2, typename T3, typename T4>
	inline	GLvoid		glVertex3fvs2(const T0 b[3], const T1 d[3], const T2&factor, const T3 d2[3], const T4&factor2)
	{
		glVertex3f
				(
					b[0]+d[0]*factor+d2[0]*factor2,
					b[1]+d[1]*factor+d2[1]*factor2,
					b[2]+d[2]*factor+d2[2]*factor2
				);
	}
	
template <typename T0, typename T1, typename T2, typename T3>
	inline	GLvoid		glVertex3fvs2(const T0 b[3], const T1 d[3], const T2&factor, const T3 d2[3])
	{
		glVertex3f
				(
					b[0]+d[0]*factor+d2[0],
					b[1]+d[1]*factor+d2[1],
					b[2]+d[2]*factor+d2[2]
				);
	}

template <typename T0, typename T1, typename T2, typename T3, typename T4>
	inline	GLvoid		glVertex2fvs2(const T0 b[2], const T1 d[2], const T2&factor, const T3 d2[2], const T4&factor2)
	{
		glVertex2f
				(
					b[0]+d[0]*factor+d2[0]*factor2,
					b[1]+d[1]*factor+d2[1]*factor2
				);
	}
	
template <typename T0, typename T1, typename T2, typename T3>
	inline	GLvoid		glVertex2fvs2(const T0 b[2], const T1 d[2], const T2&factor, const T3 d2[2])
	{
		glVertex2f
				(
					b[0]+d[0]*factor+d2[0],
					b[1]+d[1]*factor+d2[1]
				);
	}

inline  GLvoid      glWhite(GLfloat alpha)
{
    glColor4f(1,1,1,alpha);
}

inline  GLvoid      glBlack(GLfloat alpha)
{
    glColor4f(0,0,0,alpha);
}


inline  GLvoid      glRed(GLfloat alpha)
{
	glColor4f(1,0,0,alpha);
}

inline  GLvoid      glMaroon(GLfloat alpha)
{
	glColor4f(0.5f,0,0,alpha);
}

inline  GLvoid      glGreen(GLfloat alpha)
{
	glColor4f(0,1,0,alpha);
}

inline  GLvoid      glBlue(GLfloat alpha)
{
	glColor4f(0,0,1,alpha);
}

inline  GLvoid      glYellow(GLfloat alpha)
{
	glColor4f(1,1,0,alpha);
}

inline  GLvoid      glOrange(GLfloat alpha)
{
	glColor4f(1,0.5,0,alpha);
}

inline  GLvoid      glMagenta(GLfloat alpha)
{
	glColor4f(1,0,1,alpha);
}

inline  GLvoid      glCyan(GLfloat alpha)
{
	glColor4f(0,1,1,alpha);
}


inline	GLvoid		glGrey(GLfloat intensity, GLfloat alpha)
{
	glColor4f(intensity,intensity,intensity,alpha);
}

template <class C>  inline  void        glLoadMatrix(const C*v)
{
    GLdouble  temp[16];
    _c16(v,temp);
    glLoadMatrixd(temp);
}

template <>         inline  void        glLoadMatrix<GLfloat>(const GLfloat*v)
{
    glLoadMatrixf(v);
}



template <>         inline  void        glLoadMatrix<GLdouble>(const GLdouble*v)
{
    glLoadMatrixd(v);
}



template <class C>  inline  void        glMultMatrix(const C*v)
{
    double  temp[16];
    _c16(v,temp);
    glMultMatrixd(temp);
}

template <>         inline  void        glMultMatrix<GLfloat>(const GLfloat*v)
{
    glMultMatrixf(v);
}

template <>         inline  void        glMultMatrix<GLdouble>(const GLdouble*v)
{
    glMultMatrixd(v);
}

inline  void        glMaterial(GLenum face, GLenum pname, GLfloat param)
{
    glMaterialf(face,pname,param);
}

inline  void        glMaterial(GLenum face, GLenum pname, const GLfloat *params)
{
    glMaterialfv(face,pname,params);
}

inline  void        glMaterial(GLenum face, GLenum pname, GLint param)
{
    glMateriali(face,pname,param);
}

inline  void        glMaterial(GLenum face, GLenum pname, const GLint *params)
{
    glMaterialiv(face,pname,params);
}


#undef DEFINE_OPENGL_TYPE
#define DEFINE_OPENGL_TYPE(TYPE,TYPE_NAME, A_TYPE, R_TYPE, LA_TYPE, RG_TYPE, RGB_TYPE, RGBA_TYPE)\
	template <>\
		struct GLType<TYPE>\
		{\
			typedef TYPE		Type;\
			static const GLenum alpha_type_constant = A_TYPE;\
			static const GLenum luminance_alpha_type_constant = LA_TYPE;\
			static const GLenum r_type_constant = R_TYPE;\
			static const GLenum rg_type_constant = RG_TYPE;\
			static const GLenum rgb_type_constant = RGB_TYPE;\
			static const GLenum rgba_type_constant = RGBA_TYPE;\
			typedef TTrue		IsSupported;\
			static const bool	is_supported = true;\
			\
			static const GLenum	constant = TYPE_NAME;\
		};

//					OGL typedef	| OGL type const	| 1 channel texel	| 1 ch. R	| 2 channels texel			| 2 channels RG | 3 channels	| 4 channels
DEFINE_OPENGL_TYPE(	GLbyte,		GL_BYTE,			GL_ALPHA8,			GL_R8,		GL_LUMINANCE8_ALPHA8,		GL_RG8,			GL_RGB8,		GL_RGBA8);
DEFINE_OPENGL_TYPE(	GLubyte,	GL_UNSIGNED_BYTE,	GL_ALPHA8,			GL_R8,		GL_LUMINANCE8_ALPHA8,		GL_RG8,			GL_RGB8,		GL_RGBA8);
DEFINE_OPENGL_TYPE(	GLshort,	GL_SHORT,			GL_ALPHA16,			GL_R16,		GL_LUMINANCE16_ALPHA16,		GL_RG16,		GL_RGB16,		GL_RGBA16);
DEFINE_OPENGL_TYPE(	GLushort,	GL_UNSIGNED_SHORT,	GL_ALPHA16,			GL_R16,		GL_LUMINANCE16_ALPHA16,		GL_RG16,		GL_RGB16,		GL_RGBA16);
DEFINE_OPENGL_TYPE(	GLint,		GL_INT,				GL_ALPHA32I_EXT,	GL_R32I,	GL_LUMINANCE16_ALPHA16,		GL_RG32I,		GL_RGB32I,		GL_RGBA32I);	
DEFINE_OPENGL_TYPE(	GLuint,		GL_UNSIGNED_INT,	GL_ALPHA32UI_EXT,	GL_R32UI,	GL_LUMINANCE16_ALPHA16,		GL_RG32UI,		GL_RGB32UI,		GL_RGBA32UI);
DEFINE_OPENGL_TYPE(	GLhalf,		GL_FLOAT,			GL_ALPHA16F_ARB,	GL_R16F,	GL_LUMINANCE_ALPHA16F_ARB,	GL_RG16F,		GL_RGB16F_ARB,	GL_RGBA16F_ARB);
DEFINE_OPENGL_TYPE(	GLfloat,	GL_FLOAT,			GL_ALPHA32F_ARB,	GL_R32F,	GL_LUMINANCE_ALPHA32F_ARB,	GL_RG32F,		GL_RGB32F_ARB,	GL_RGBA32F_ARB);
DEFINE_OPENGL_TYPE(	GLdouble,	GL_DOUBLE,			GL_ALPHA32F_ARB,	GL_R32F,	GL_LUMINANCE_ALPHA32F_ARB,	GL_RG32F,		GL_RGB32F_ARB,	GL_RGBA32F_ARB);

DEFINE_OPENGL_TYPE( GLDepthComponent, GL_FLOAT,		GL_DEPTH_COMPONENT24,	GL_DEPTH_COMPONENT24,	GL_DEPTH_COMPONENT24,	GL_DEPTH_COMPONENT24,	GL_DEPTH_COMPONENT24,	GL_DEPTH_COMPONENT24);


template<typename T>
	inline  GLenum      glType()
	{
		return GLType<T>::constant;
	}
	
template<>
inline  GLvoid      glVertex<GLint>(const GLint&i0, const GLint&i1, const GLint&i2)
{
    glVertex3i(i0,i1,i2);
}

template<>
inline  GLvoid      glVertex<GLfloat>(const GLfloat&f0, const GLfloat&f1, const GLfloat&f2)
{
    glVertex3f(f0,f1,f2);
}

template<>
inline  GLvoid      glVertex<GLdouble>(const GLdouble&d0, const GLdouble&d1, const GLdouble&d2)
{
    glVertex3d(d0,d1,d2);
}

inline  GLvoid      glLineMode()
{
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

inline  GLvoid      glFillMode()
{
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}




#endif
