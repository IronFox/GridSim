#ifndef geomatrixH
#define geomatrixH


#include "../math/vclasses.h"
#include "../math/object.h"
#include "../general/timer.h"
#include "../list/queue.h"
#include "../io/log.h"
#include "../math/object_def.h"

/******************************************************************

Depreciated.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

/* GeoModule-specifications:

    class GeoModule
    {
    public:
            struct              Sector {...};
            typedef ...         FloatType;
            const unsigned      Size;
            
    static  void                build(GeoTriangle*t);
    static  void                update(GeoTriangle*t);
    static  void                free(Sector*sector);
            bool                cast(GeoTriangle*t); //true if down_cast
    };
*/


//Log log_file(".\\geomatrix.log",true);

unsigned    recursion(0);

/*#ifdef __BORLANDC__
    #define __FUNC__    __func__
#endif*/

#define BEGIN //{log_file<<intend(recursion)<<"->"<<__FILE__<<"::"<<__FUNC__<<"(at line "<<IntToStr(__LINE__)<<")\n"; recursion++;}
#define LBEGIN  //{log_file<<intend(recursion)<<"->GeoTriangle(depth="<<IntToStr(depth)<<")::"<<__FUNC__<<"(at line "<<IntToStr(__LINE__)<<")\n"; recursion++;}
#define END //{recursion--; log_file<<intend(recursion)<<"<-"<<__FILE__<<"::"<<__FUNC__<<"(at line "<<IntToStr(__LINE__)<<")\n";}




template <class> class GeoTriangle;
template <class> class GeoSphere;
template <class> class GeoVertex;
template <class> class GeoData;
template <class> struct SGeoTriangleLink;

enum en_GeoRenderResult{RENDER_DISABLED, SECTOR_RENDERED, SECTOR_INVISIBLE, REQUEST_DETAIL};


class FloatModule
{
public:
        struct              Sector
        {

        };
        typedef float       FloatType;
static  const unsigned      Size = 5;


static  void                build(GeoTriangle<FloatModule>*)  {}
static  void                update(GeoTriangle<FloatModule>*) {}
static  void                free(Sector*)                       {}
        bool                cast(GeoTriangle<FloatModule>*)   {return false;}
};


/* triangle order-specification:

vertices:

    col->
row 1
 |  23
 V  456
    7890
    12345
    678901
    ...
    
chlidren:

    0
  . 3  .
1   .   2

.
|\
| \
|0 \
|---\
|\ 3|\
|1\ |2\
|__\|__\

(the third child's orientation is inverted)

neighbours:

    .
 0 / \ 2
  .___.
    1

orientation:

0:
      .
-> 0 / \ 2
    .___.
      1

1:
      .
-> 1 / \ 0
    .___.
      2

2:
      .
-> 2 / \ 1
    .___.
      0

max-vertices:

0



1      2


*/

/*
vertex-flags

#define VF_MOUNTAINOUS
*/

template <class FloatType>  struct GeoData
{

public:
        FloatType               height,     //distance from the center
                                fertility,  //growth-rate of the vertex [0,1]
                                variance;

                                GeoData();
template <class Type>           GeoData(const Type&scalar);
                                GeoData(const FloatType&height, const FloatType&variance, const FloatType&fertility);
                                
MFUNC   GeoData<FloatType>     operator+(const GeoData<C>&other)  const;
MFUNC   GeoData<FloatType>     operator-(const GeoData<C>&other)  const;
MFUNC   GeoData<FloatType>     operator*(const C&scalar)           const;
MFUNC   GeoData<FloatType>     operator/(const C&scalar)           const;
MFUNC   GeoData<FloatType>&    operator+=(const GeoData<C>&other);
MFUNC   GeoData<FloatType>&    operator-=(const GeoData<C>&other);
MFUNC   GeoData<FloatType>&    operator*=(const C&scalar);
MFUNC   GeoData<FloatType>&    operator/=(const C&scalar);
};


template <class FloatType>  class GeoVertex
{
private:
static INLINE  FloatType        random();
public:
        Vec3<FloatType>        position,normal;
        GeoData<FloatType>     data;

                                GeoVertex();
MFUNC3                          GeoVertex(const Vec3<C0>&vec, const Vec3<C1>&norm, const GeoData<C2>&dat);
MFUNC   GeoVertex<FloatType>   operator+(const GeoVertex<C>&other)    const;
MFUNC   GeoVertex<FloatType>   operator-(const GeoVertex<C>&other)    const;
MFUNC   GeoVertex<FloatType>   operator*(const C&scalar)               const;
MFUNC   GeoVertex<FloatType>   operator/(const C&scalar)               const;
MFUNC   GeoVertex<FloatType>&  operator+=(const GeoVertex<C>&other);
MFUNC   GeoVertex<FloatType>&  operator-=(const GeoVertex<C>&other);
MFUNC   GeoVertex<FloatType>&  operator*=(const C&scalar);
MFUNC   GeoVertex<FloatType>&  operator/=(const C&scalar);

        void                    vary(const FloatType&hvariance, const FloatType&variance);
INLINE  void                    setHeight(const FloatType&height);
INLINE  void                    createFrom(const GeoVertex<FloatType>&v0, const GeoVertex<FloatType>&v1, const FloatType&hvariance, const FloatType&variance);
INLINE  void                    check(const String&context) const;

};

/*

template <class GeoModule> class GeoNode
{
public:
typedef typename GeoModule::FloatType Float;
        GeoVertex<Float>*          target;
        GeoTriangle<GeoModule>*    origin;
        unsigned                    x,y,max;
        BYTE                        corner;

                                    GeoNode(unsigned x, unsigned y, GeoTriangle<GeoModule>*origin);
        bool                        evolve(DynamicPQueue<GeoNode<GeoModule> >&queue, Float&step);

};*/

template <class GeoModule> struct SGeoTriangleLink
{
        GeoTriangle<GeoModule> *link;
        BYTE                    orientation;
        
};

template <class GeoModule> class GeoTriangle:public GeoModule::Sector
{
private:
        typedef typename GeoModule::FloatType               Float;
        typedef GeoVertex<typename GeoModule::FloatType>   Vertex;
        typedef SGeoTriangleLink<GeoModule>                 Link;
        typedef GeoTriangle<GeoModule>                     Triangle;
        typedef GeoData<typename GeoModule::FloatType>     Data;
        
        __int64                     referenced;

        Link                        resolveChild(const Link&link, BYTE index);
        Link                        resolveChildRec(const Link&link, BYTE index);
        Vertex                      centerOf(const Vertex&v0, const Vertex&v1);
INLINE  void                        vary(Vertex&vertex);
static  INLINE Float                random();
static  Link                        link();
static  Link                        link(GeoTriangle<GeoModule> *link_);
static  Link                        link(GeoTriangle<GeoModule> *link_, BYTE orientation_);
        void                        clearLinkage();
        void                        modifyBox(Float child_box[6]);


public:

static  const unsigned  size        = GeoModule::Size;
static  const unsigned  range       = (1<<size)+1;
static  const unsigned  vertices    = ((1LL<<(2*size)) + 3LL*(1LL<<(size)) +2LL)/2LL;

        Float                   local_box[6],global_box[6];
        
        GeoSphere<GeoModule>  *super;
        
        Link                    parent;
        union
        {
            Link                neighbour[3];
            struct
            {
                Link            n0,n1,n2;
            };
        };
        union
        {
            Triangle            *child[4];
            struct
            {
                Triangle        *c0,*c1,*c2,*c3;
            };
        };
        Vertex                  vertex[vertices];
static  unsigned                border[3][range];
static  unsigned                inner[3][range-1];
        union
        {
            bool                primary[3];
            struct
            {
                bool            p0,p1,p2;
            };
        };
        unsigned                depth;
        bool                    recursive;
        union
        {
            unsigned            nvis[3];
            struct
            {
                unsigned        vis0,vis1,vis2;
            };
        };

                                GeoTriangle();
                                GeoTriangle(GeoSphere<GeoModule>*super_,const Link&parent, const Link&n0, const Link&n1, const Link&n2, const __int64&time);
virtual                        ~GeoTriangle();
static  void                    defineBorders();
        void                    setup(const Float*p0, const Float*p1, const Float*p2, const __int64&time);
        void                    render(GeoModule&module);
        void                    setNeighbours(const Link&link0, const Link&link1, const Link&link2);
        void                    copyEdge(BYTE k);
        void                    fillAllNormals();
        void                    fillHalfNormals();
        void                    mergeHalfNormals();
        void                    mergeBorderNormals(bool update=true);
        void                    mergeCornerNormals(bool update=true);
        void                    cornerNormal(BYTE index, Float check[3], Float out[3]);
        void                    setCornerNormal(BYTE index, const Float normal[3]);
        Vertex&                 getCorner(BYTE corner);
        void                    randomSmooth();
        void                    generateLayer(unsigned step);
        void                    showMe();
        void                        updateBox();

        void                    cast(GeoModule&module, const __int64&time, float ttl);
        void                    resolve(const Link&l0, const Link&l1, const Link&l2);
        void                    resolve();

        void                    dropChildren();        
        
MFUNC2  bool                    intersectsBox(const C0 p0[3], const C1 p1[3]);

static INLINE  unsigned                getIndex(unsigned col, unsigned row)
        {
            if (row >= range || col > row)
                FATAL__("index-get-error");
            return row*(row+1)/2+col;
        }

INLINE  Vertex&                 getVertex(unsigned col, unsigned row)
        {
            return vertex[getIndex(col,row)];
        }

        void                    checkLinkage();
        void                    checkTimeouts(const __int64&time, float ttl);
};



template <class GeoModule> class GeoSphere
{
private:
        typedef typename GeoModule::FloatType   Float;
        typedef GeoData<Float>                 Data;
        typedef GeoVertex<Float>               Vertex;

        struct SConnection
        {
            unsigned                            from,   to;
            Float                               normal[3];
        };

        
static  Data                            generateData(const Data&lower, const Data&upper);
static  SGeoTriangleLink<GeoModule>     link(GeoTriangle<GeoModule> *link_, BYTE orientation_);
static  void                            spherize(SObjFace<TGraphDef<Float> >&face, const Float&radius, const Float&step, Float*open=NULL);
public:
        unsigned                        sectors,memory;
        Data                            top,bottom;


        GeoTriangle<GeoModule>         triangle[20];
        UINT64                          range;
        typename GeoModule::FloatType   height_range;

                                        GeoSphere();
                                        GeoSphere(const Float&lower, const Float&upper, unsigned depth);
                                        GeoSphere(const Data&lower, const Data&upper, unsigned depth);
        void                            render(GeoModule&module);
        void                            trace(GeoModule&module, const __int64&time, float top_level_ttl);
        void                            generate(const Float&lower, const Float&upper, unsigned depth);
        void                            generate(const Data&lower, const Data&upper, unsigned depth);
        void                            showMe();
        void                            checkLinkage();
        void                            checkTimeouts(const __int64&time, float top_level_ttl);
};


#include "geomatrix.tpl.h"

#endif
