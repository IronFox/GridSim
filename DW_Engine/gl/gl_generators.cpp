#include "../global_root.h"
#include "gl_generators.h"

/******************************************************************

Predefined OpenGL display-list generators.

******************************************************************/


GLuint Generator::glBox(float size)
{
	GLuint rs = glGenLists(2);
    glNewList(rs,GL_COMPILE);
		glBegin(GL_QUADS);
			glNormal3f(0,0,-1);
			glTexCoord2f(0,0);
				glVertex3f(-size,-size,-size);
			glTexCoord2f(1,0);
				glVertex3f(size,-size,-size);
			glTexCoord2f(1,1);
				glVertex3f(size,size,-size);
			glTexCoord2f(0,1);
				glVertex3f(-size,size,-size);

			glNormal3f(0,0,1);
			glTexCoord2f(0,0);
				glVertex3f(size,-size,size);
			glTexCoord2f(1,0);
				glVertex3f(-size,-size,size);
			glTexCoord2f(1,1);
				glVertex3f(-size,size,size);
			glTexCoord2f(0,1);
				glVertex3f(size,size,size);

			glNormal3f(-1,0,0);
			glTexCoord2f(0,0);
				glVertex3f(-size,-size,-size);
			glTexCoord2f(1,0);
				glVertex3f(-size,-size,size);
			glTexCoord2f(1,1);
				glVertex3f(-size,size,size);
			glTexCoord2f(0,1);
				glVertex3f(-size,size,-size);

			glNormal3f(1,0,0);
			glTexCoord2f(0,0);
				glVertex3f(size,-size,size);
			glTexCoord2f(1,0);
				glVertex3f(size,-size,-size);
			glTexCoord2f(1,1);
				glVertex3f(size,size,-size);
			glTexCoord2f(0,1);
				glVertex3f(size,size,size);

			glNormal3f(0,-1,0);
			glTexCoord2f(0,0);
				glVertex3f(-size,-size,size);
			glTexCoord2f(1,0);
				glVertex3f(size,-size,size);
			glTexCoord2f(1,1);
				glVertex3f(size,-size,-size);
			glTexCoord2f(0,1);
				glVertex3f(-size,-size,-size);

			glNormal3f(0,1,0);
			glTexCoord2f(0,0);
				glVertex3f(-size,size,-size);
			glTexCoord2f(1,0);
				glVertex3f(size,size,-size);
			glTexCoord2f(1,1);
				glVertex3f(size,size,size);
			glTexCoord2f(0,1);
				glVertex3f(-size,size,size);
		glEnd();
    glEndList();
    return rs;
}

GLuint Generator::glSkyBox()
{
    GLuint rs = glGenLists(2);
    glNewList(rs,GL_COMPILE);
        glBegin(GL_QUADS);
            glTexCoord2f(1,0);
            glVertex3f(-1,-1,1);
            glTexCoord2f(1,0.98);
            glVertex3f(-1,1,1);
            glTexCoord2f(0,0.98);
            glVertex3f(1,1,1);
            glTexCoord2f(0,0);
            glVertex3f(1,-1,1);
            glTexCoord2f(0,0.98);
            glVertex3f(-1,1,-1);
            glTexCoord2f(0,0);
            glVertex3f(-1,-1,-1);
            glTexCoord2f(1,0);
            glVertex3f(1,-1,-1);
            glTexCoord2f(1,0.98);
            glVertex3f(1,1,-1);
            glTexCoord2f(0,0.98);
            glVertex3f(1,1,-1);
            glTexCoord2f(0,0);
            glVertex3f(1,-1,-1);
            glTexCoord2f(1,0);
            glVertex3f(1,-1,1);
            glTexCoord2f(1,0.98);
            glVertex3f(1,1,1);
            glTexCoord2f(1,0);
            glVertex3f(-1,-1,-1);
            glTexCoord2f(1,0.98);
            glVertex3f(-1,1,-1);
            glTexCoord2f(0,0.98);
            glVertex3f(-1,1,1);
            glTexCoord2f(0,0);
            glVertex3f(-1,-1,1);
        glEnd();
    glEndList();

    glNewList(rs+1,GL_COMPILE);
        glBegin(GL_QUADS);
            glTexCoord2f(0.99,0.999);
            glVertex3f(1,1,1);
            glTexCoord2f(0.01,0.999);
            glVertex3f(-1,1,1);
            glTexCoord2f(0.01,0.01);
            glVertex3f(-1,1,-1);
            glTexCoord2f(0.99,0.01);
            glVertex3f(1,1,-1);
        glEnd();
    glEndList();
    return rs;
}

GLuint Generator::glTextureCube(float size)
{
    GLuint rs = glGenLists(1);
    glNewList(rs,GL_COMPILE);
        glBegin(GL_QUADS);
            glNormal3f(0,0,-1); //there is a reason why I put size to texcoord - can you guess?
            glTexCoord3f(size,-size,-size);   glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord3f(-size,-size,-size);  glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord3f(-size,size,-size);   glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord3f(size,size,-size);    glVertex3f(0.5*size,0.5*size,-0.5*size);
            glNormal3f(0,0,1);
            glTexCoord3f(-size,-size,size);   glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glTexCoord3f(size,-size,size);    glVertex3f(0.5*size,-0.5*size,0.5*size);
            glTexCoord3f(size,size,size);     glVertex3f(0.5*size,0.5*size,0.5*size);
            glTexCoord3f(-size,size,size);    glVertex3f(-0.5*size,0.5*size,0.5*size);
            glNormal3f(0,-1,0);
            glTexCoord3f(-size,-size,-size);  glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord3f(size,-size,-size);   glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord3f(size,-size,size);    glVertex3f(0.5*size,-0.5*size,0.5*size);
            glTexCoord3f(-size,-size,size);   glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glNormal3f(0,1,0);
            glTexCoord3f(size,size,-size);    glVertex3f(0.5*size,0.5*size,-0.5*size);
            glTexCoord3f(-size,size,-size);   glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord3f(-size,size,size);    glVertex3f(-0.5*size,0.5*size,0.5*size);
            glTexCoord3f(size,size,size);     glVertex3f(0.5*size,0.5*size,0.5*size);
            glNormal3f(-1,0,0);
            glTexCoord3f(-size,size,-size);   glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord3f(-size,-size,-size);  glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord3f(-size,-size,size);   glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glTexCoord3f(-size,size,size);    glVertex3f(-0.5*size,0.5*size,0.5*size);
            glNormal3f(1,0,0);
            glTexCoord3f(size,-size,-size);   glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord3f(size,size,-size);    glVertex3f(0.5*size,0.5*size,-0.5*size);
            glTexCoord3f(size,size,size);     glVertex3f(0.5*size,0.5*size,0.5*size);
            glTexCoord3f(size,-size,size);    glVertex3f(0.5*size,-0.5*size,0.5*size);
        glEnd();
    glEndList();
    return rs;
}

GLuint Generator::glTangentMappedCube(float size)
{
    GLuint rs = glGenLists(1);
    glNewList(rs,GL_COMPILE);
        glBegin(GL_QUADS);
            glNormal3f(0,0,-1);
			glColor3f(1,0,0);
            glTexCoord2f(1,0);	glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(0,0);	glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(0,1);	glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(1,1);	glVertex3f(0.5*size,0.5*size,-0.5*size);
            glNormal3f(0,0,1);
			glColor3f(1,0,0);
            glTexCoord2f(0,0);	glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(1,0);	glVertex3f(0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(1,1);	glVertex3f(0.5*size,0.5*size,0.5*size);
            glTexCoord2f(0,1);	glVertex3f(-0.5*size,0.5*size,0.5*size);
            glNormal3f(0,-1,0);
			glColor3f(1,0,0);
            glTexCoord2f(0,0);	glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(1,0);	glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(1,1);	glVertex3f(0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(0,1);	glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glNormal3f(0,1,0);
			glColor3f(1,0,0);
            glTexCoord2f(1,0);	glVertex3f(0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(0,0);	glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(0,1);	glVertex3f(-0.5*size,0.5*size,0.5*size);
            glTexCoord2f(1,1);	glVertex3f(0.5*size,0.5*size,0.5*size);
            glNormal3f(-1,0,0);
			glColor3f(0,1,0);
            glTexCoord2f(1,0);	glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(0,0);	glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(0,1);	glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(1,1);	glVertex3f(-0.5*size,0.5*size,0.5*size);
            glNormal3f(1,0,0);
			glColor3f(0,1,0);
            glTexCoord2f(0,0);	glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(1,0);	glVertex3f(0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(1,1);	glVertex3f(0.5*size,0.5*size,0.5*size);
            glTexCoord2f(0,1);	glVertex3f(0.5*size,-0.5*size,0.5*size);
        glEnd();
    glEndList();
    return rs;


}


GLuint Generator::glCube(float size)
{
    GLuint rs = glGenLists(1);
    glNewList(rs,GL_COMPILE);
        glBegin(GL_QUADS);
            glNormal3f(0,0,-1);
            glTexCoord2f(1,0); glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(0,0); glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(0,1); glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(1,1); glVertex3f(0.5*size,0.5*size,-0.5*size);
            glNormal3f(0,0,1);
            glTexCoord2f(0,0); glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(1,0); glVertex3f(0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(1,1); glVertex3f(0.5*size,0.5*size,0.5*size);
            glTexCoord2f(0,1); glVertex3f(-0.5*size,0.5*size,0.5*size);
            glNormal3f(0,-1,0);
            glTexCoord2f(0,0); glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(1,0); glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(1,1); glVertex3f(0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(0,1); glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glNormal3f(0,1,0);
            glTexCoord2f(1,0); glVertex3f(0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(0,0); glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(0,1); glVertex3f(-0.5*size,0.5*size,0.5*size);
            glTexCoord2f(1,1); glVertex3f(0.5*size,0.5*size,0.5*size);
            glNormal3f(-1,0,0);
            glTexCoord2f(1,0); glVertex3f(-0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(0,0); glVertex3f(-0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(0,1); glVertex3f(-0.5*size,-0.5*size,0.5*size);
            glTexCoord2f(1,1); glVertex3f(-0.5*size,0.5*size,0.5*size);
            glNormal3f(1,0,0);
            glTexCoord2f(0,0); glVertex3f(0.5*size,-0.5*size,-0.5*size);
            glTexCoord2f(1,0); glVertex3f(0.5*size,0.5*size,-0.5*size);
            glTexCoord2f(1,1); glVertex3f(0.5*size,0.5*size,0.5*size);
            glTexCoord2f(0,1); glVertex3f(0.5*size,-0.5*size,0.5*size);
        glEnd();
    glEndList();
    return rs;
}

GLuint Generator::glCross(float size)
{
    GLuint rs = glGenLists(1);
    glPushMatrix();
        glNewList(rs,GL_COMPILE);
            glBegin(GL_LINES);
                glVertex3f(0,0.5*size,0);
                glVertex3f(0,1*size,0);
                glVertex3f(0,-0.5*size,0);
                glVertex3f(0,-1*size,0);
                glVertex3f(0.5*size,0,0);
                glVertex3f(1*size,0,0);
                glVertex3f(-0.5*size,0,0);
                glVertex3f(-1*size,0,0);
                glVertex3f(0,0,0.5*size);
                glVertex3f(0,0,1*size);
                glVertex3f(0,0,-0.5*size);
                glVertex3f(0,0,-1*size);
                glVertex3f(0.5*size,0,0);
                glVertex3f(0,0,0.5*size);
                glVertex3f(0,0,0.5*size);
                glVertex3f(0.5*size,0,0.5*size);
                glVertex3f(0.5*size,0,0.5*size);
                glVertex3f(0.5*size,0,0);
                glVertex3f(-0.5*size,0,0);
                glVertex3f(0,0,-0.5*size);
                glVertex3f(0,0,-0.5*size);
                glVertex3f(-0.5*size,0,-0.5*size);
                glVertex3f(-0.5*size,0,-0.5*size);
                glVertex3f(-0.5*size,0,0);
                glVertex3f(0.25*size,0.5*size,-0.25*size);
                glVertex3f(0.25*size,-0.5*size,-0.25*size);
                glVertex3f(0.25*size,-0.5*size,-0.25*size);
                glVertex3f(0.5*size,0,-0.5*size);
                glVertex3f(0.5*size,0,-0.5*size);
                glVertex3f(0.25*size,0.5*size,-0.25*size);
                glVertex3f(-0.25*size,0.5*size,0.25*size);
                glVertex3f(-0.25*size,-0.5*size,0.25*size);
                glVertex3f(-0.25*size,-0.5*size,0.25*size);
                glVertex3f(-0.5*size,0,0.5*size);
                glVertex3f(-0.5*size,0,0.5*size);
                glVertex3f(-0.25*size,0.5*size,0.25*size);
            glEnd();
        glEndList();
    glPopMatrix();
    return rs;
}

GLuint	Generator::glBlendGrid(unsigned width, unsigned height, float red, float green, float blue)
{
    GLuint rs = glGenLists(1);
    glNewList(rs,GL_COMPILE);
        glBegin(GL_LINES);
			for (int i = -(int)width/2; i <= (int)width/2; i++)
			{
				if (!(i%10))
					glColor3f(red,green,blue);
				elif (!(i%5))
					glColor4f(red,green,blue,0.75);
				elif (!(i%2))
					glColor4f(red,green,blue,0.5);
				else
					glColor4f(red,green,blue,0.25);
				glVertex2i(i,-(int)height/2);
				glVertex2i(i,(int)height/2);
			}
			for (int i = -(int)height/2; i <= (int)height/2; i++)
			{
				if (!(i%10))
					glColor3f(red,green,blue);
				elif (!(i%5))
					glColor4f(red,green,blue,0.75);
				elif (!(i%2))
					glColor4f(red,green,blue,0.5);
				else
					glColor4f(red,green,blue,0.25);
				glVertex2i(-(int)width/2, i);
				glVertex2i((int)width/2, i);
			}
		
		
		glEnd();
	glEndList();
	return rs;
}

GLuint Generator::glGrid(unsigned width, unsigned height, unsigned depth)
{
    GLuint rs = glGenLists(1);
    glNewList(rs,GL_COMPILE);
        glBegin(GL_LINES);
            float h;
            for (unsigned i = 1; i < width; i++)
                if (i != width/2)
                {
                    int true_x = (int)i-(int)width/2;
                    if (!(i%10))
                        h = 0.12;
                    else
                        if (!(i%5))
                            h = 0.1;
                        else
                            if (!(i%2))
                                h = 0.05;
                            else
                                h = 0.03;
                    for (unsigned j = 0; j < depth*5; j++)
                    {
                        glVertex3f(true_x,0,-(float)depth/2+(float)j/5);
                        glVertex3f(true_x,0,-(float)depth/2+(float)j/5+h);
                    }
                }
            for (unsigned i = 1; i < depth; i++)
                if (i != depth/2)
                {
                    int true_z = (int)i-(int)depth/2;
                    if (!(i%10))
                        h = 0.12;
                    else
                        if (!(i%5))
                            h = 0.1;
                        else
                            if (!(i%2))
                                h = 0.05;
                            else
                                h = 0.03;
                    for (unsigned j = 0; j < width*5; j++)
                    {
                        glVertex3f(-(float)width/2+(float)j/5,0,true_z);
                        glVertex3f(-(float)width/2+(float)j/5+h,0,true_z);
                    }
                }
            glVertex3f(-(float)width/2,0,-(float)depth/2);
            glVertex3f((float)width/2,0,-(float)depth/2);
            glVertex3f((float)width/2,0,-(float)depth/2);
            glVertex3f((float)width/2,0,(float)depth/2);
            glVertex3f((float)width/2,0,(float)depth/2);
            glVertex3f(-(float)width/2,0,(float)depth/2);
            glVertex3f(-(float)width/2,0,(float)depth/2);
            glVertex3f(-(float)width/2,0,-(float)depth/2);
            glVertex3f(0,0,(float)depth/2+0.5);
            glVertex3f(0,0,-(float)depth/2);
            glVertex3f(0,0,(float)depth/2+0.5);
            glVertex3f(0.1,0,(float)depth/2+0.4);
            glVertex3f(0,0,(float)depth/2+0.5);
            glVertex3f(-0.1,0,(float)depth/2+0.4);
            glVertex3f((float)width/2+0.5,0,0);
            glVertex3f(-(float)width/2,0,0);
            glVertex3f((float)width/2+0.5,0,0);
            glVertex3f((float)width/2+0.4,0,0.1);
            glVertex3f((float)width/2+0.5,0,0);
            glVertex3f((float)width/2+0.4,0,-0.1);
            glVertex3f(0,-(float)height/2,0);
            glVertex3f(0,(float)height/2,0);
            for (unsigned i = 1; i < height; i++)
                if (i!=height/2)
                {
                    int true_y = (int)i-(int)height/2;
                    if (!(i%10))
                        h = 0.15;
                    else
                        if (!(i%5))
                            h = 0.1;
                        else
                            if (!(i%2))
                                h = 0.065;
                            else
                                h = 0.05;
                    glVertex3f(h,true_y,h);
                    glVertex3f(-h,true_y,h);
                    glVertex3f(-h,true_y,h);
                    glVertex3f(-h,true_y,-h);
                    glVertex3f(-h,true_y,-h);
                    glVertex3f(h,true_y,-h);
                    glVertex3f(h,true_y,-h);
                    glVertex3f(h,true_y,h);
                }
            glVertex3f(0,(float)height/2,0);
            glVertex3f(0.1,(float)height/2-0.1,0.1);
            glVertex3f(0,(float)height/2,0);
            glVertex3f(-0.1,(float)height/2-0.1,-0.1);
        glEnd();
    glEndList();
    return rs;
}

GLuint Generator::glGridSystem(unsigned width, unsigned height, unsigned depth)
{
    GLuint rs = glGenLists(1);
    glNewList(rs,GL_COMPILE);
        glBegin(GL_LINES);
            glVertex3f(0,0,(float)depth/2+0.5);
            glVertex3f(0,0,-(float)depth/2);
            glVertex3f(0,0,(float)depth/2+0.5);
            glVertex3f(0.1,0,(float)depth/2+0.4);
            glVertex3f(0,0,(float)depth/2+0.5);
            glVertex3f(-0.1,0,(float)depth/2+0.4);
            glVertex3f((float)width/2+0.5,0,0);
            glVertex3f(-(float)width/2,0,0);
            glVertex3f((float)width/2+0.5,0,0);
            glVertex3f((float)width/2+0.4,0,0.1);
            glVertex3f((float)width/2+0.5,0,0);
            glVertex3f((float)width/2+0.4,0,-0.1);
            
			glVertex3f(0,0,0);
            glVertex3f(0,(float)height/2+0.5,0);
			
			
            glVertex3f(0,(float)height/2+0.5,0);
            glVertex3f(0.1,(float)height/2+0.4,0.1);
            glVertex3f(0,(float)height/2+0.5,0);
            glVertex3f(-0.1,(float)height/2+0.4,-0.1);
        glEnd();
    glEndList();
    return rs;
}



GLuint Generator::glTextureSphere(float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    TVec3<GLfloat> p[4],c,n;
    glNewList(result,GL_COMPILE);
        glBegin(GL_QUADS);
        for (BYTE d = 0; d < 3; d++)
        {
            BYTE vx = (d+1)%3,
                 vy = (d+2)%3;
            for (unsigned x = 1; x < iterations; x++)
                for (unsigned y = 1; y < iterations; y++)
                {
                    p[0].v[vx] = 1-(GLfloat)(x-1)/(GLfloat)(iterations-1)*2;
                    p[0].v[vy] = 1-(GLfloat)(y-1)/(GLfloat)(iterations-1)*2;
                    p[0].v[d]  = 1;
                    p[1].v[vx] = 1-(GLfloat)(x)/(GLfloat)(iterations-1)*2;
                    p[1].v[vy] = 1-(GLfloat)(y-1)/(GLfloat)(iterations-1)*2;
                    p[1].v[d]  = 1;
                    p[2].v[vx] = 1-(GLfloat)(x)/(GLfloat)(iterations-1)*2;
                    p[2].v[vy] = 1-(GLfloat)(y)/(GLfloat)(iterations-1)*2;
                    p[2].v[d]  = 1;
                    p[3].v[vx] = 1-(GLfloat)(x-1)/(GLfloat)(iterations-1)*2;
                    p[3].v[vy] = 1-(GLfloat)(y)/(GLfloat)(iterations-1)*2;
                    p[3].v[d]  = 1;

                    float least(1),max(0);
                    for (BYTE k = 0; k < 4; k++)
                    {
                        float tx = Vec::angleOne(p[k].v[0],p[k].v[2]);
                        if (tx < least)
                            least = tx;
                        if (tx > max)
                            max = tx;
                    }
                    bool crossed = (max-least)>0.5;
                    for (BYTE k = 0; k < 4; k++)
                    {

                        Vec::divide(p[k],Vec::length(p[k]),n);
                        Vec::multiply(n,radius,c);
                        float tx = Vec::angleOne(c.x,c.z);
                        if (crossed && tx < 0.25)
                            tx++;
                        glTexCoord2f(tx, atan2(n.v[1],(GLfloat)vsqrt(sqr(n.x)+sqr(n.z))) /M_PI+0.5);
                        glNormal3fv(n.v);
                        glVertex3fv(c.v);
                    }
                    for (BYTE k = 3; k < 4; k--)
                    {
                        p[k].v[d] = -1;
                        Vec::divide(p[k],Vec::length(p[k]),n);
                        Vec::multiply(n,radius,c);
                        float tx = Vec::angleOne(c.v[0],c.v[2]);
                        if (crossed && tx < 0.25)
                            tx++;
//                        ShowMessage(_toString(n)+" -> "+FloatToStr(atan2(n[1],(GLfloat)vsqrt(sqr(n[0])+sqr(n[2]))) /M_PI+0.5));
                        glTexCoord2f(tx, atan2(n.v[1],(GLfloat)vsqrt(sqr(n.v[0])+sqr(n.v[2]))) /M_PI+0.5);
                        glNormal3fv(n.v);
                        glVertex3fv(c.v);
                    }
                }
        }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glCubeCoordSphere(float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    TVec3<GLfloat> p[4],c,n;
    glNewList(result,GL_COMPILE);
        glBegin(GL_QUADS);
        for (BYTE d = 0; d < 3; d++)
        {
            BYTE vx = (d+1)%3,
                 vy = (d+2)%3;
            for (unsigned x = 1; x < iterations; x++)
                for (unsigned y = 1; y < iterations; y++)
                {
                    p[0].v[vx] = 1-(GLfloat)(x-1)/(GLfloat)(iterations-1)*2;
                    p[0].v[vy] = 1-(GLfloat)(y-1)/(GLfloat)(iterations-1)*2;
                    p[0].v[d]  = 1;
                    p[1].v[vx] = 1-(GLfloat)(x)/(GLfloat)(iterations-1)*2;
                    p[1].v[vy] = 1-(GLfloat)(y-1)/(GLfloat)(iterations-1)*2;
                    p[1].v[d]  = 1;
                    p[2].v[vx] = 1-(GLfloat)(x)/(GLfloat)(iterations-1)*2;
                    p[2].v[vy] = 1-(GLfloat)(y)/(GLfloat)(iterations-1)*2;
                    p[2].v[d]  = 1;
                    p[3].v[vx] = 1-(GLfloat)(x-1)/(GLfloat)(iterations-1)*2;
                    p[3].v[vy] = 1-(GLfloat)(y)/(GLfloat)(iterations-1)*2;
                    p[3].v[d]  = 1;

                    for (BYTE k = 0; k < 4; k++)
                    {
                        Vec::divide(p[k],Vec::length(p[k]),n);
                        Vec::multiply(n,radius,c);
                        glNormal3fv(n.v);
                        glTexCoord3fv(n.v);
                        glVertex3fv(c.v);
                    }
                    for (BYTE k = 3; k < 4; k--)
                    {
                        p[k].v[d] = -1;
                        Vec::divide(p[k],Vec::length(p[k]),n);
                        Vec::multiply(n,radius,c);
                        glNormal3fv(n.v);
                        glTexCoord3fv(n.v);
                        glVertex3fv(c.v);
                    }
                }
        }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glSphere(float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    TVec3<GLfloat> p[4],c,n;
    glNewList(result,GL_COMPILE);
        glBegin(GL_QUADS);
        for (BYTE d = 0; d < 3; d++)
        {
            BYTE vx = (d+1)%3,
                 vy = (d+2)%3;
            for (unsigned x = 1; x < iterations; x++)
                for (unsigned y = 1; y < iterations; y++)
                {
                    p[0].v[vx] = 1-(GLfloat)(x-1)/(GLfloat)(iterations-1)*2;
                    p[0].v[vy] = 1-(GLfloat)(y-1)/(GLfloat)(iterations-1)*2;
                    p[0].v[d]  = 1;
                    p[1].v[vx] = 1-(GLfloat)(x)/(GLfloat)(iterations-1)*2;
                    p[1].v[vy] = 1-(GLfloat)(y-1)/(GLfloat)(iterations-1)*2;
                    p[1].v[d]  = 1;
                    p[2].v[vx] = 1-(GLfloat)(x)/(GLfloat)(iterations-1)*2;
                    p[2].v[vy] = 1-(GLfloat)(y)/(GLfloat)(iterations-1)*2;
                    p[2].v[d]  = 1;
                    p[3].v[vx] = 1-(GLfloat)(x-1)/(GLfloat)(iterations-1)*2;
                    p[3].v[vy] = 1-(GLfloat)(y)/(GLfloat)(iterations-1)*2;
                    p[3].v[d]  = 1;

                    for (BYTE k = 0; k < 4; k++)
                    {
                        Vec::divide(p[k],Vec::length(p[k]),n);
                        Vec::multiply(n,radius,c);
                        glNormal3fv(n.v);
                        glVertex3fv(c.v);
                    }
                    for (BYTE k = 3; k < 4; k--)
                    {
                        p[k].v[d] = -1;
                        Vec::divide(p[k],Vec::length(p[k]),n);
                        Vec::multiply(n,radius,c);
                        glNormal3fv(n.v);
                        glVertex3fv(c.v);
                    }
                }
        }
        glEnd();
    glEndList();
    return result;
}


GLuint Generator::glSphereSide(float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    TVec3<GLfloat> buffer[8];
    glNewList(result,GL_COMPILE);
        glBegin(GL_TRIANGLES);
            for (unsigned x = 1; x < iterations; x++)
                for (unsigned y = 1; y < iterations; y++)
                {
                    buffer[0].x =1-(float)(x-1)/(float)(iterations-1)*2;
                    buffer[0].z =1-(float)(y-1)/(float)(iterations-1)*2;
                    buffer[0].y =1;
                    buffer[1].x =1-(float)(x)/(float)(iterations-1)*2;
                    buffer[1].z =1-(float)(y-1)/(float)(iterations-1)*2;
                    buffer[1].y =1;
                    buffer[2].x =1-(float)(x)/(float)(iterations-1)*2;
                    buffer[2].z =1-(float)(y)/(float)(iterations-1)*2;
                    buffer[2].y =1;
                    buffer[3].x =1-(float)(x-1)/(float)(iterations-1)*2;
                    buffer[3].z=1-(float)(y)/(float)(iterations-1)*2;
                    buffer[3].y=1;
                    float l1 = Vec::length(buffer[0]),
                          l2 = Vec::length(buffer[1]),
                          l3 = Vec::length(buffer[2]),
                          l4 = Vec::length(buffer[3]);
                    Vec::multiply(buffer[0],radius/l1,buffer[4]);
                    Vec::multiply(buffer[1],radius/l2,buffer[5]);
                    Vec::multiply(buffer[2],radius/l3,buffer[6]);
                    Vec::multiply(buffer[3],radius/l4,buffer[7]);
                    glTexCoord2f((float)x/(float)iterations,(float)(y-1)/(float)iterations);
                    glNormal3f(buffer[5].x/radius,buffer[5].y/radius,buffer[5].z/radius);
                    glVertex3fv(buffer[5].v);
                    glTexCoord2f((float)(x-1)/(float)iterations,(float)(y-1)/(float)iterations);
                    glNormal3f(buffer[4].x/radius,buffer[4].y/radius,buffer[4].z/radius);
                    glVertex3fv(buffer[4].v);
                    glTexCoord2f((float)(x)/(float)iterations,(float)(y)/(float)iterations);
                    glNormal3f(buffer[6].x/radius,buffer[6].y/radius,buffer[6].z/radius);
                    glVertex3fv(buffer[6].v);
                    glTexCoord2f((float)(x-1)/(float)iterations,(float)(y-1)/(float)iterations);
                    glNormal3f(buffer[4].x/radius,buffer[4].y/radius,buffer[4].z/radius);
                    glVertex3fv(buffer[4].v);
                    glTexCoord2f((float)(x-1)/(float)iterations,(float)(y)/(float)iterations);
                    glNormal3f(buffer[7].x/radius,buffer[7].y/radius,buffer[7].z/radius);
                    glVertex3fv(buffer[7].v);
                    glTexCoord2f((float)(x)/(float)iterations,(float)y/(float)iterations);
                    glNormal3f(buffer[6].x/radius,buffer[6].y/radius,buffer[6].z/radius);
                    glVertex3fv(buffer[6].v);
                }
        glEnd();
    glEndList();
    return result;
}



GLuint Generator::glGround(double*map, unsigned map_width, unsigned map_height,double min_x, double min_z, double max_x, double max_z)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_QUADS);
            for (unsigned x = 0; x < map_width -1; x++)
                for (unsigned y = 0; y < map_height-1; y++)
                {
                    glTexCoord2d((double)x/(double)(map_width-1),(double)y/(double)(map_height-1));
                    glVertex3d(min_x+(max_x-min_x)*(double)x/(double)(map_width-1),map[x*map_height+y],min_z+(max_z-min_z)*(double)y/(double)(map_height-1));
                    glTexCoord2d((double)x/(double)(map_width-1),(double)(y+1)/(double)(map_height-1));
                    glVertex3d(min_x+(max_x-min_x)*(double)x/(double)(map_width-1),map[x*map_height+y+1],min_z+(max_z-min_z)*(double)(y+1)/(double)(map_height-1));
                    glTexCoord2d((double)(x+1)/(double)(map_width-1),(double)(y+1)/(double)(map_height-1));
                    glVertex3d(min_x+(max_x-min_x)*(double)(x+1)/(double)(map_width-1),map[(x+1)*map_height+y+1],min_z+(max_z-min_z)*(double)(y+1)/(double)(map_height-1));
                    glTexCoord2d((double)(x+1)/(double)(map_width-1),(double)y/(double)(map_height-1));
                    glVertex3d(min_x+(max_x-min_x)*(double)(x+1)/(double)(map_width-1),map[(x+1)*map_height+y],min_z+(max_z-min_z)*(double)y/(double)(map_height-1));
                }
        glEnd();
    glEndList();
    return result;
}


GLuint Generator::glSimpleCross(float size)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_LINES);
            glVertex3f(-size/2,0,0);
            glVertex3f(size/2,0,0);
            glVertex3f(0,-size/2,0);
            glVertex3f(0,size/2,0);
            glVertex3f(0,0,-size/2);
            glVertex3f(0,0,size/2);
        glEnd();
    glEndList();
    return result;
}



GLuint Generator::glCircle(float radius, unsigned iterations, BYTE zero_component)
{
    BYTE x = (zero_component+1)%3,
         y = (zero_component+2)%3;
    GLuint result = glGenLists(1);
    float step = M_PI*2/iterations,p[3];
    p[zero_component] = 0;
    glNewList(result,GL_COMPILE);
        glBegin(GL_LINE_LOOP);
            for (unsigned i = 0; i < iterations; i++)
            {
                p[x] = sin(step*i)*radius;
                p[y] = cos(step*i)*radius;
                glVertex3fv(p);
            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glFilledCircle(float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    float step = M_PI*2/iterations;
    glNewList(result,GL_COMPILE);
        glBegin(GL_POLYGON);
            for (unsigned i = 0; i < iterations; i++)
                glVertex2f(cos(step*i)*radius,sin(step*i)*radius);
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glCylinderX(float bottom, float top, float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    float step = M_PI*2/iterations;
    glNewList(result,GL_COMPILE);
        glBegin(GL_POLYGON);
            for (unsigned i = 0; i < iterations; i++)
                glVertex3f(bottom,cos(-step*i)*radius,sin(-step*i)*radius);
        glEnd();
        glBegin(GL_POLYGON);
            for (unsigned i = 0; i < iterations; i++)
                glVertex3f(top,cos(step*i)*radius,sin(step*i)*radius);
        glEnd();
        glBegin(GL_QUADS);
            for (unsigned i = 0; i < iterations; i++)
            {
                glVertex3f(top,cos(step*i)*radius,sin(step*i)*radius);
                glVertex3f(bottom,cos(step*i)*radius,sin(step*i)*radius);
                glVertex3f(bottom,cos(step*(i+1))*radius,sin(step*(i+1))*radius);
                glVertex3f(top,cos(step*(i+1))*radius,sin(step*(i+1))*radius);
            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glCylinderY(float bottom, float top, float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    float step = M_PI*2/iterations;
    glNewList(result,GL_COMPILE);
        glBegin(GL_POLYGON);
            for (unsigned i = 0; i < iterations; i++)
                glVertex3f(cos(-step*i)*radius,bottom,sin(-step*i)*radius);
        glEnd();
        glBegin(GL_POLYGON);
            for (unsigned i = 0; i < iterations; i++)
                glVertex3f(cos(step*i)*radius,top,sin(step*i)*radius);
        glEnd();
        glBegin(GL_QUADS);
            for (unsigned i = 0; i < iterations; i++)
            {
                glVertex3f(cos(step*i)*radius,top,sin(step*i)*radius);
                glVertex3f(cos(step*i)*radius,bottom,sin(step*i)*radius);
                glVertex3f(cos(step*(i+1))*radius,bottom,sin(step*(i+1))*radius);
                glVertex3f(cos(step*(i+1))*radius,top,sin(step*(i+1))*radius);
            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glTangentMappedCylinderY(float bottom, float top, float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    float step = 1.0/iterations;
    glNewList(result,GL_COMPILE);
        glBegin(GL_QUADS);
            for (unsigned i = 0; i < iterations; i++)
            {
                float	v0 = step*i,
						v1 = step*(i+1),
						x0 = cos(v0*M_PI*2),
						z0 = sin(v0*M_PI*2),
						x1 = cos(v1*M_PI*2),
						z1 = sin(v1*M_PI*2),
						xr0 = x0*radius,
						xr1 = x1*radius,
						zr0 = z0*radius,
						zr1 = z1*radius;

				glNormal3f(x1, 0, z1); 
				glColor3f(z1,0,-x1);
				
                glTexCoord2f(v1,1);
				glVertex3f(xr1,top,zr1);
                glTexCoord2f(v1,0);
				glVertex3f(xr1,bottom,zr1);
						
				glNormal3f(x0, 0, z0); 
				glColor3f(z0,0,-x0);

                glTexCoord2f(v0,0);	
				glVertex3f(xr0,bottom,zr0);
				glTexCoord2f(v0,1);	
				glVertex3f(xr0,top,zr0);

            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glCylinderZ(float bottom, float top, float radius, unsigned iterations)
{
    GLuint result = glGenLists(1);
    float step = M_PI*2/iterations;
    glNewList(result,GL_COMPILE);
        glBegin(GL_POLYGON);
            for (unsigned i = 0; i < iterations; i++)
                glVertex3f(cos(-step*i)*radius,sin(-step*i)*radius,bottom);
        glEnd();
        glBegin(GL_POLYGON);
            for (unsigned i = 0; i < iterations; i++)
                glVertex3f(cos(step*i)*radius,sin(step*i)*radius,top);
        glEnd();
        glBegin(GL_TRIANGLE_STRIP);
            for (unsigned i = 0; i <= iterations; i++)
            {
                glVertex3f(cos(step*i)*radius,sin(step*i)*radius,top);
                glVertex3f(cos(step*i)*radius,sin(step*i)*radius,bottom);
            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glArrow(float length, float radius, unsigned steps, BYTE up)
{
    BYTE x = (up+1)%3,
         y = (up+2)%3;
    float p0[3]={0,0,0},p1[3],p2[3];
    p0[up] = length;

    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_LINES);
            glVertex3f(0,0,0);
            glVertex3fv(p0);
        glEnd();
        glBegin(GL_TRIANGLES);
            p0[x] = 0;
            p0[y] = 0;
            p1[up] = length-radius*2;
            p2[up] = length-radius*2;
            for (unsigned i = 0; i < steps; i++)
            {
                float w = (float)i/(float)(steps)*2*M_PI,
                      wn = (float)(i+1)/(float)(steps)*2*M_PI;

                p0[up] = length;
                p1[x] = cos(wn)*radius;
                p1[y] = sin(wn)*radius;
                p2[x] = cos(w)*radius;
                p2[y] = sin(w)*radius;
                glVertex3fv(p0);
                glVertex3fv(p1);
                glVertex3fv(p2);

                p0[up] = length-radius*2;
                glVertex3fv(p0);
                glVertex3fv(p2);
                glVertex3fv(p1);
            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glWireEightSided(float size)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_LINE_STRIP);
            glVertex3f(0,size,0);
            glVertex3f(size,0,0);
            glVertex3f(0,0,size);
            glVertex3f(-size,0,0);
            glVertex3f(0,0,-size);
            glVertex3f(0,-size,0);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f(0,0,size);
            glVertex3f(0,size,0);
            glVertex3f(-size,0,0);
            glVertex3f(0,size,0);
            glVertex3f(0,0,-size);
            glVertex3f(0,size,0);
            glVertex3f(0,-size,0);
            glVertex3f(0,0,size);
            glVertex3f(0,-size,0);
            glVertex3f(size,0,0);
            glVertex3f(0,-size,0);
            glVertex3f(-size,0,0);
            glVertex3f(size,0,0);
            glVertex3f(0,0,-size);
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glCornerWireBox(float size)
{
	float	d = size/2,
			l = size/6;
	GLuint result = glGenLists(1);
	glNewList(result,GL_COMPILE);
		glBegin(GL_LINES);
			glVertex3f(d,d,d);
			glVertex3f(d-l,d,d);
			glVertex3f(d,d,d);
			glVertex3f(d,d-l,d);
			glVertex3f(d,d,d);
			glVertex3f(d,d,d-l);
			
			glVertex3f(-d,d,d);
			glVertex3f(-d+l,d,d);
			glVertex3f(-d,d,d);
			glVertex3f(-d,d-l,d);
			glVertex3f(-d,d,d);
			glVertex3f(-d,d,d-l);
		
			glVertex3f(-d,-d,d);
			glVertex3f(-d+l,-d,d);
			glVertex3f(-d,-d,d);
			glVertex3f(-d,-d+l,d);
			glVertex3f(-d,-d,d);
			glVertex3f(-d,-d,d-l);
		
			glVertex3f(-d,-d,-d);
			glVertex3f(-d+l,-d,-d);
			glVertex3f(-d,-d,-d);
			glVertex3f(-d,-d+l,-d);
			glVertex3f(-d,-d,-d);
			glVertex3f(-d,-d,-d+l);
		
			glVertex3f(d,-d,-d);
			glVertex3f(d-l,-d,-d);
			glVertex3f(d,-d,-d);
			glVertex3f(d,-d+l,-d);
			glVertex3f(d,-d,-d);
			glVertex3f(d,-d,-d+l);
		
			glVertex3f(d,d,-d);
			glVertex3f(d-l,d,-d);
			glVertex3f(d,d,-d);
			glVertex3f(d,d-l,-d);
			glVertex3f(d,d,-d);
			glVertex3f(d,d,-d+l);
		
			glVertex3f(-d,d,-d);
			glVertex3f(-d+l,d,-d);
			glVertex3f(-d,d,-d);
			glVertex3f(-d,d-l,-d);
			glVertex3f(-d,d,-d);
			glVertex3f(-d,d,-d+l);
		
			glVertex3f(d,-d,d);
			glVertex3f(d-l,-d,d);
			glVertex3f(d,-d,d);
			glVertex3f(d,-d+l,d);
			glVertex3f(d,-d,d);
			glVertex3f(d,-d,d-l);
		
		glEnd();
	glEndList();
	return result;
}

GLuint Generator::glWireBox(float size)
{
    float d = size/2;
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_LINE_LOOP);
            glVertex3f(-d,d,-d);
            glVertex3f(d,d,-d);
            glVertex3f(d,d,d);
            glVertex3f(-d,d,d);
            glVertex3f(-d,-d,d);
            glVertex3f(d,-d,d);
            glVertex3f(d,-d,-d);
            glVertex3f(-d,-d,-d);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f(-d,d,-d);
            glVertex3f(-d,d,d);
            glVertex3f(-d,-d,-d);
            glVertex3f(-d,-d,d);
            glVertex3f(d,d,-d);
            glVertex3f(d,-d,-d);
            glVertex3f(d,d,d);
            glVertex3f(d,-d,d);
        glEnd();
    glEndList();
    return result;
}


GLuint Generator::glMassiveDepthArrow(float width, float height)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_QUADS);
            glVertex3f(width,width,0);
            glVertex3f(-width,width,0);
            glVertex3f(-width,width,height);
            glVertex3f(width,width,height);

            glVertex3f(-width,-width,0);
            glVertex3f(width,-width,0);
            glVertex3f(width,-width,height);
            glVertex3f(-width,-width,height);

            glVertex3f(width,-width,height*0.75);
            glVertex3f(width,-width,0);
            glVertex3f(width,width,0);
            glVertex3f(width,width,height*0.75);

            glVertex3f(-width,width,height*0.75);
            glVertex3f(-width,width,0);
            glVertex3f(-width,-width,0);
            glVertex3f(-width,-width,height*0.75);

            glVertex3f(-width,-width,height);
            glVertex3f(width,-width,height);
            glVertex3f(width,width,height);
            glVertex3f(-width,width,height);


            glVertex3f(-width*3,-width,height*0.46);
            glVertex3f(-width,-width,height*0.75);
            glVertex3f(-width,-width,height);
            glVertex3f(-width*3,-width,height*0.71);

            glVertex3f(-width,width,height*0.75);
            glVertex3f(-width*3,width,height*0.46);
            glVertex3f(-width*3,width,height*0.71);
            glVertex3f(-width,width,height);

            glVertex3f(-width*3,width,height*0.71);
            glVertex3f(-width*3,width,height*0.46);
            glVertex3f(-width*3,-width,height*0.46);
            glVertex3f(-width*3,-width,height*0.71);

            glVertex3f(-width*3,width,height*0.71);
            glVertex3f(-width*3,-width,height*0.71);
            glVertex3f(-width,-width,height);
            glVertex3f(-width,width,height);

            glVertex3f(-width*3,-width,height*0.46);
            glVertex3f(-width*3,width,height*0.46);
            glVertex3f(-width,width,height*0.75);
            glVertex3f(-width,-width,height*0.75);


            glVertex3f(width,-width,height*0.75);
            glVertex3f(width*3,-width,height*0.46);
            glVertex3f(width*3,-width,height*0.71);
            glVertex3f(width,-width,height);

            glVertex3f(width*3,width,height*0.46);
            glVertex3f(width,width,height*0.75);
            glVertex3f(width,width,height);
            glVertex3f(width*3,width,height*0.71);

            glVertex3f(width*3,width,height*0.46);
            glVertex3f(width*3,width,height*0.71);
            glVertex3f(width*3,-width,height*0.71);
            glVertex3f(width*3,-width,height*0.46);

            glVertex3f(width*3,-width,height*0.71);
            glVertex3f(width*3,width,height*0.71);
            glVertex3f(width,width,height);
            glVertex3f(width,-width,height);

            glVertex3f(width*3,width,height*0.46);
            glVertex3f(width*3,-width,height*0.46);
            glVertex3f(width,-width,height*0.75);
            glVertex3f(width,width,height*0.75);
        glEnd();
    glEndList();
    return result;
}



GLuint Generator::glWiredDepthArrow(float width, float height)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_LINES);
            glVertex3f(width,width,0);
            glVertex3f(width,width,height*0.75);
            glVertex3f(-width,width,0);
            glVertex3f(-width,width,height*0.75);
            glVertex3f(-width,-width,0);
            glVertex3f(-width,-width,height*0.75);
            glVertex3f(width,-width,0);
            glVertex3f(width,-width,height*0.75);

            glVertex3f(width,width,0);
            glVertex3f(-width,width,0);
            glVertex3f(-width,width,0);
            glVertex3f(-width,-width,0);
            glVertex3f(-width,-width,0);
            glVertex3f(width,-width,0);
            glVertex3f(width,-width,0);
            glVertex3f(width,width,0);



            glVertex3f(width,width,height*0.75);
            glVertex3f(width,-width,height*0.75);
            glVertex3f(-width,width,height*0.75);
            glVertex3f(-width,-width,height*0.75);

            glVertex3f(width,width,height*0.75);
            glVertex3f(width*3,width,height*0.46);
            glVertex3f(width*3,width,height*0.46);
            glVertex3f(width*3,-width,height*0.46);
            glVertex3f(width,-width,height*0.75);
            glVertex3f(width*3,-width,height*0.46);

            glVertex3f(-width,width,height*0.75);
            glVertex3f(-width*3,width,height*0.46);
            glVertex3f(-width*3,width,height*0.46);
            glVertex3f(-width*3,-width,height*0.46);
            glVertex3f(-width,-width,height*0.75);
            glVertex3f(-width*3,-width,height*0.46);


            glVertex3f(width,width,height);
            glVertex3f(width*3,width,height*0.71);
            glVertex3f(width*3,width,height*0.71);
            glVertex3f(width*3,-width,height*0.71);
            glVertex3f(width,-width,height);
            glVertex3f(width*3,-width,height*0.71);

            glVertex3f(-width,width,height);
            glVertex3f(-width*3,width,height*0.71);
            glVertex3f(-width*3,width,height*0.71);
            glVertex3f(-width*3,-width,height*0.71);
            glVertex3f(-width,-width,height);
            glVertex3f(-width*3,-width,height*0.71);

            glVertex3f(width,width,height);
            glVertex3f(-width,width,height);
            glVertex3f(-width,width,height);
            glVertex3f(-width,-width,height);
            glVertex3f(-width,-width,height);
            glVertex3f(width,-width,height);
            glVertex3f(width,-width,height);
            glVertex3f(width,width,height);

            glVertex3f(width*3,width,height*0.71);
            glVertex3f(width*3,width,height*0.46);
            glVertex3f(-width*3,width,height*0.71);
            glVertex3f(-width*3,width,height*0.46);
            glVertex3f(-width*3,-width,height*0.71);
            glVertex3f(-width*3,-width,height*0.46);
            glVertex3f(width*3,-width,height*0.71);
            glVertex3f(width*3,-width,height*0.46);

        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glCappedCursor(float size, float cap_width)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_QUADS);
            glVertex3f(cap_width,0,size);
            glVertex3f(0,cap_width,size);
            glVertex3f(-cap_width,0,size);
            glVertex3f(0,-cap_width,size);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(cap_width,0,-size);
            glVertex3f(0,cap_width,-size);
            glVertex3f(-cap_width,0,-size);
            glVertex3f(0,-cap_width,-size);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(0,size,cap_width);
            glVertex3f(cap_width,size,0);
            glVertex3f(0,size,-cap_width);
            glVertex3f(-cap_width,size,0);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(0,-size,cap_width);
            glVertex3f(cap_width,-size,0);
            glVertex3f(0,-size,-cap_width);
            glVertex3f(-cap_width,-size,0);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(size,cap_width,0);
            glVertex3f(size,0,cap_width);
            glVertex3f(size,-cap_width,0);
            glVertex3f(size,0,-cap_width);
        glEnd();
        glBegin(GL_QUADS);
            glVertex3f(-size,cap_width,0);
            glVertex3f(-size,0,cap_width);
            glVertex3f(-size,-cap_width,0);
            glVertex3f(-size,0,-cap_width);
        glEnd();

    glEndList();
    return result;
}

GLuint Generator::glGeoSphere(float radius, unsigned segments)
{
    GLuint result=glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_LINE_LOOP);
            for (unsigned i = 0; i < segments; i++)
                glVertex2f(cos(M_PI*2*i/segments)*radius,sin(M_PI*2*i/segments)*radius);
        glEnd();
        glBegin(GL_LINE_LOOP);
            for (unsigned i = 0; i < segments; i++)
                glVertex3f(cos(M_PI*2*i/segments)*radius,0,sin(M_PI*2*i/segments)*radius);
        glEnd();
        glBegin(GL_LINE_LOOP);
            for (unsigned i = 0; i < segments; i++)
                glVertex3f(0,cos(M_PI*2*i/segments)*radius,sin(M_PI*2*i/segments)*radius);
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glGeoHull(float radius, unsigned segments)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        for (unsigned i = 0; i < segments; i++)
        {
            float x0 = (float)i/segments,
                  x1 = (float)(i+1)/segments,
                  r0 = 90.f+360.0f*x0,
                  r1 = 90.f+360.0f*x1;
            glBegin(GL_TRIANGLE_STRIP);
                for (unsigned j = 0; j < segments/2; j++)
                {
                    float r2 = 90.0f*((float)j-segments/4)/(float)(segments/4);
					TVec3<>	p;
                    Vec::sphereCoords(r0,r2,radius,p);
                    glTexCoord2f(1.0-x0,(float)j/(segments/2-1));
                    glNormal3f(p.x/radius,p.z/radius,p.y/radius);
                    glVertex3f(p.x,p.z,p.y);
                    Vec::sphereCoords(r1,r2,radius,p);
                    glTexCoord2f(1.0-x1,(float)j/(segments/2-1));
                    glNormal3f(p.x/radius,p.z/radius,p.y/radius);
                    glVertex3f(p.x,p.z,p.y);
                }
            glEnd();
        }
    glEndList();
    return result;
}

GLuint Generator::glCylinderSideY(float radius, float floor, float top, unsigned segments, bool texcoords)
{
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_TRIANGLE_STRIP);
            for (unsigned i = 0; i <= segments; i++)
            {
                float angle = -M_PI_2+2*M_PI*i/segments;
				if (texcoords)
					glTexCoord2f((float)i/segments,0);
                glNormal3f(cos(angle),0,sin(angle));
                glVertex3f(cos(angle)*radius,floor,sin(angle)*radius);
				if (texcoords)
					glTexCoord2f((float)i/segments,1);
				glVertex3f(cos(angle)*radius,top,sin(angle)*radius);
            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glConeSegment(float angle, float inner_radius, float outer_radius, float segment_begin, float segment_end, unsigned segments)
{

    angle *= M_PI/2;
    segment_begin *= 2*M_PI;
    segment_end *= 2*M_PI;
    float r0 = sin(angle)*inner_radius,
          d0 = cos(angle)*inner_radius,
          r1 = sin(angle)*outer_radius,
          d1 = cos(angle)*outer_radius;
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
        glBegin(GL_TRIANGLE_STRIP);
            for (unsigned i = 0; i < segments; i++)
            {
                float f = segment_begin + (segment_end-segment_begin)*i/(segments-1),
                      s = sin(f),
                      c = cos(f),
                      ax = c*r0,
                      ay = s*r0,
                      bx = c*r1,
                      by = s*r1;
                glVertex3f(d1,bx,by);
                glVertex3f(d0,ax,ay);
            }
        glEnd();
    glEndList();
    return result;
}

GLuint Generator::glFiveEdgedTwelveSided(float radius)
{

	static const float f = (1.0f+sqrt(5.0))/2.0f;
	static const float vertices[][3]=
	{
		{1,1,1},	//top support 1
		{-1,1,1},
		{1,1,-1},	//top support 2
		{-1,1,-1},
		{1,-1,1},
		{-1,-1,1},
		{1,-1,-1},
		{-1,-1,-1},
		{0,1.0f/f,f},//top seal 1
		{0,1.0f/f,-f},//top seal 2
		{0,-1.0f/f,f},
		{0,-1.0f/f,-f},
		{1.0f/f,f,0},	//top edge
		{-1.0f/f,f,0},
		{1.0f/f,-f,0},	//bottom edge
		{-1.0f/f,-f,0},
		{f,0,1.0f/f},
		{f,0,-1.0f/f},
		{-f,0,1.0f/f},
		{-f,0,-1.0f/f}
	};

	static const unsigned indices[][5]=
	{
		{1,8,0,12,13},
		{13,12,2,9,3},
		{12,0,16,17,2},
		{3,19,18,1,13},
		{15,14,4,10,5},
		{7,11,6,14,15},
		{6,17,16,4,14},
		{15,5,18,19,7},
		{9,2,17,6,11},
		{3,9,11,7,19},
		{1,18,5,10,8},
		{0,8,10,4,16}
	};
	return 0;
}


GLuint	Generator::glHudGrid(float min_x, float min_y, float max_x, float max_y, unsigned xcells, unsigned ycells)
{
	if (!xcells || !ycells)
		return 0;
	float	delta_x = max_x-min_x,
			delta_y = max_y-min_y,
			step_x = delta_x/xcells,
			step_y = delta_y/ycells;
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
	
		glBegin(GL_LINES);
			for (unsigned x = 0; x <= xcells; x++)
			{
				float	fx = (float)x/xcells,
						cx = min_x+fx*delta_x;
				glVertex2f(cx,min_y);
				glVertex2f(cx,min_y+step_y*0.1);
				
				glVertex2f(cx,max_y);
				glVertex2f(cx,max_y-step_y*0.1);
				
				for (unsigned y = 1; y < ycells; y++)
				{
					float	fy = (float)y/ycells,
							cy = min_y+fy*delta_y;
					glVertex2f(cx,cy-step_y*0.1);
					glVertex2f(cx,cy+step_y*0.1);
				}
			}
			for (unsigned y = 0; y <= ycells; y++)
			{
				float	fy = (float)y/ycells,
						cy = min_y+fy*delta_y;
				glVertex2f(min_x,cy);
				glVertex2f(min_x+step_x*0.1,cy);
				
				glVertex2f(max_x,cy);
				glVertex2f(max_x-step_x*0.1,cy);
				
				for (unsigned x = 1; x < xcells; x++)
				{
					float	fx = (float)x/xcells,
							cx = min_x+fx*delta_x;
					glVertex2f(cx-step_x*0.1,cy);
					glVertex2f(cx+step_x*0.1,cy);
				}
			}
		glEnd();
	glEndList();
	return result;
}


GLuint	Generator::glMultiFacedSphere(float radius)
{
	static const float 
						t = (1.0f+sqrt(5.0))/2.0f,
						t2 = t*t,
						t3 = t*t*t,
						s2t = 2.0f+t;




	static const float vertices[][3]=
	{
		{1, 1, t3},
		{1, 1, -t3},
		{1, -1, -t3},
		{1, -1, t3},
		{-1, 1, t3},
		{-1, 1, -t3},
		{-1, -1, -t3},
		{-1, -1, t3},
		
		{t3, 1, 1},
		{t3, -1, 1},
		{t3, -1, -1},
		{t3, 1, -1},
		{-t3, 1, 1},
		{-t3, -1, 1},
		{-t3, -1, -1},
		{-t3, 1, -1},
		
		{1, t3, 1},
		{-1, t3, 1},
		{-1, -t3, 1},
		{1, -t3, 1},
		{1, t3, -1},
		{-1, t3, -1},
		{-1, -t3, -1},
		{1, -t3, -1},
		
		{t2, t, 2*t},
		{-t2, t, 2*t},
		{-t2, -t, 2*t},
		{t2, -t, 2*t},
		{t2, t, -2*t},
		{-t2, t, -2*t},
		{-t2, -t, -2*t},
		{t2, -t, -2*t},
		
		{2*t, t2, t},
		{-2*t, t2, t},
		{-2*t, -t2, t},
		{2*t, -t2, t},
		{2*t, t2, -t},
		{-2*t, t2, -t},
		{-2*t, -t2, -t},
		{2*t, -t2, -t},
		
		{t, 2*t, t2},
		{-t, 2*t, t2},
		{-t, -2*t, t2},
		{t, -2*t, t2},
		{t, 2*t, -t2},
		{-t, 2*t, -t2},
		{-t, -2*t, -t2},
		{t, -2*t, -t2},
		
		{s2t, 0, t2},
		{-s2t, 0, t2},
		{-s2t, 0, -t2},
		{s2t, 0, -t2},
		
		{t2, s2t, 0},
		{-t2, s2t, 0},
		{-t2, -s2t, 0},
		{t2, -s2t, 0},
		
		{0, t2, s2t},
		{0, -t2, s2t},
		{0, -t2, -s2t},
		{0, t2, -s2t}


	};


	static const unsigned indices3[][3]=
	{
		{50,14,15},
		{12,13,49},
		{48,9,8},
		{11,10,51},
		{4,0,56},
		{1,5,59},
		{25,41,33},
		{7,57,3},
		{36,28,44},
		{24,32,40},
		{29,37,45},
		{2,58,6},
		{46,38,30},
		{43,35,27},
		{39,47,31},
		{34,42,26},
		{18,54,22},
		{20,16,52},
		{19,23,55},
		{53,17,21}
	};

	static const unsigned indices4[][4]=
	{
		{4,7,3,0},
		{1,2,6,5},
		{8,9,10,11},
		{12,49,25,33},
		{48,8,32,24},
		{30,38,14,50},
		{48,27,35,9},
		{0,24,40,56},
		{11,51,28,36},
		{50,15,37,29},
		{5,29,45,59},
		{33,41,17,53},
		{25,4,56,41},
		{28,1,59,44},
		{10,39,31,51},
		{58,46,30,6},
		{34,26,49,13},
		{36,44,20,52},
		{47,58,2,31},
		{42,57,7,26},
		{57,43,27,3},
		{40,32,52,16},
		{43,19,55,35},
		{55,23,47,39},
		{46,22,54,38},
		{45,37,53,21},
		{54,18,42,34},
		{23,19,18,22},
		{12,15,14,13},
		{20,21,17,16}
	};

	static const unsigned indices5[][5]=
	{
		{25,49,26,7,4},
		{0,3,27,48,24},
		{5,6,30,50,29},
		{51,31,2,1,28},
		{15,12,33,53,37},
		{8,11,36,52,32},
		{41,56,40,16,17},
		{44,59,45,21,20},
		{14,38,54,34,13},
		{35,55,39,10,9},
		{42,18,19,43,57},
		{47,23,22,46,58}
	};
	return 0;
}

GLuint Generator::glPlane(float min_x, float min_y, float max_x, float max_y, unsigned xcells, unsigned ycells)
{
	if (!xcells || !ycells)
		return 0;
	float	delta_x = max_x-min_x,
			delta_y = max_y-min_y;
    GLuint result = glGenLists(1);
    glNewList(result,GL_COMPILE);
	
		for (unsigned y = 0; y+1 < ycells; y++)
		{
			float	fy0 = float(y)/float(xcells-1),
					fy1 = float(y+1)/float(xcells-1);
			glBegin(GL_TRIANGLE_STRIP);
				for (unsigned x = 0; x < xcells; x++)
				{
					float fx = float(x)/float(xcells-1);
					glTexCoord2f(fx,fy1);
						glVertex2f(min_x+fx*delta_x,min_y+fy1*delta_y);
					glTexCoord2f(fx,fy0);
						glVertex2f(min_x+fx*delta_x,min_y+fy0*delta_y);
				}
			glEnd();
		}
	glEndList();
	return result;
}


