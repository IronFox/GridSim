#ifndef lwoH
#define lwoH

/******************************************************************

LWO-access-unit.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

//---------------------------------------------------------------------------

#include <stdio.h>
#include "../math/vector.h"
#include "../container/lvector.h"
#include "../container/buffer.h"

#define LWO_ASSEMBLE(c0,c1,c2,c3)	(((unsigned)(c3)<<24)|((unsigned)(c2)<<16)|((unsigned)(c1)<<8)|(unsigned)(c0))


#define LWO_POLS    0x534C4F50
#define LWO_PNTS    0x53544E50
#define LWO_SRFS    0x53465253
#define LWO_SURF    0x46525553
#define LWO_LWOB    0x424F574C
#define LWO_LWO2    0x324F574C

#define LWO_COLR    0x524C4F43
#define LWO_FLAG    0x47414C46
#define LWO_SMAN    0x4E414D53
#define LWO_TRAN    0x4E415254
#define LWO_LUMI    0x494D554C
#define LWO_SPEC    0x43455053
#define LWO_DIFF    0x46464944
#define LWO_CTEX    0x58455443
#define LWO_Plan    0x6E616C50
#define LWO_Cyli    0x696C7943
#define LWO_Sphe    0x65687053
#define LWO_Cubi    0x69627543
#define LWO_TIMG    0x474D4954
#define LWO_TFP0    0x30504654
#define LWO_TFP1    0x31504654
#define LWO_TWRP    0x50525754
#define LWO_TCTR    0x52544354
#define LWO_TSIZ    0x5A495354
#define LWO_TFLG    0x474C4654
//#define LWO_VMAP    'PAMV'
#define LWO_VMAP	LWO_ASSEMBLE('P','A','M','V')
//#define LWO_PTAG    'GATP'
#define LWO_PTAG	LWO_ASSEMBLE('G','A','T','P')
//#define LWO_TXUV    'VUXT'
#define LWO_TXUV	LWO_ASSEMBLE('V','U','X','T')
//#define LWO_WGHT    'THGW'
#define LWO_WGHT	LWO_ASSEMBLE('T','H','G','W')


// Texture flags
#define TXF_AXIS_X          0x1
#define TXF_AXIS_Y          0x2
#define TXF_AXIS_Z          0x4
#define TXF_WORLDCOORD      0x8
#define TXF_NEGATIVE        0x10
#define TXF_PIXBLEND        0x20
#define TXF_ANTIALIAS       0x40
#define TXF_UDECAL          0x80
#define TXF_VDECAL          0x100


#define MAX_MATERIAL_CNT    0x400
#define MAX_SECTION_CNT     0x400
#define LW_COUNT_MASK       0x3FF

#define LWO_FACE            0x45434146
#define LWO_CURV            0x56525543
#define LWO_PTCH            0x48435450
#define LWO_MBAL            0x4C41424D
#define LWO_BONE            0x454E4F42


enum en_MaterialType{MT_PLANAR,MT_CYLINDRICAL,MT_SPHERICAL,MT_CUBIC};
enum en_GroupType{GT_FACE,GT_CURVE,GT_PATCH,GT_METABALL,GT_BONE};



struct SLWpoint;
struct SLWfragment;
struct SLWuvi;
struct SLWwi;
struct SLWplink;

class CLWFaceGroup;
class CLWPolygonSection;
class CLWPolygonMap;
class CLWVertexMap;
class CLWMaterial;
class CLWContainer;
class CLWInfo;
class CLWObject;
class CLWStructure;



struct SLWpoint
{
    union
    {
        struct
        {
            float x,y,z;
        };
        float   coord[3];
    };
};


struct SLWfragment
{
        fpos_t      addr,current;
        UINT32      size,progress;

                    SLWfragment();
                    SLWfragment(fpos_t,UINT32);
        void        reset();
};


struct SLWuvi
{
        float       u,v;
        unsigned    index;
};

struct SLWwi
{
        float       weight;
        unsigned    index;
};

struct SLWplink
{
        unsigned    index,
                    surface;
};

class CLWFaceGroup
{
public:
        UINT16          material,vertices;
        UINT32         *vertex;
        en_GroupType    type;


                        CLWFaceGroup();
                        CLWFaceGroup(const CLWFaceGroup&other);
virtual                ~CLWFaceGroup();
        void            operator=(const CLWFaceGroup&other);
        void            allocate();
        UINT32&         operator[](UINT16 index) const;
};

class CLWPolygonSection:public Buffer<SLWfragment>
{
public:
        SLWfragment    fragment;
        
                        CLWPolygonSection();
                        CLWPolygonSection(fpos_t,UINT32);
                        
        unsigned        countGroups(CLWInfo*info);
        unsigned        getGroups(CLWFaceGroup*targets, CLWInfo*info);
};


class CLWPolygonMap
{
public:
        SLWplink      *surf;
        unsigned        length;
        
                        CLWPolygonMap();
virtual                ~CLWPolygonMap();
};

class CLWVertexMap
{
public:
        SLWuvi        *uvi;
        SLWwi         *wi;
        unsigned        nverts;
        UINT32          type;
        char           *name;
        
                        CLWVertexMap();
virtual                ~CLWVertexMap();


};

class CLWMaterial
{
public:
        char           *name,*su_image;
        UINT32          size;
        BYTE           *data;
        TVec3<float> center,dimension;
        BYTE            axis;
        en_MaterialType type;
        int             flags;
        float           tiles_x, tiles_y;
        TVec2<float> offset,scale;
		float			angle,sm_angle;
        float           su_red,su_green,su_blue;
        USHORT          su_flags,su_lumi,su_diff,su_spec,su_refl,su_tran;


                        CLWMaterial();
virtual                ~CLWMaterial();
        void            rename(const char*Name);
        void            allocate();
        void            scan();
template <class C>
static  void            xyz2h(const C&x, const C&y, const C&z, C&out);
template <class C>
static  void            xyz2lp(const C&x, const C&y, const C&z, C&h, C&p);
template <class C>
static  C               fract(const C&x);
template <class C>
        void            texCoord(const C*position, const C*norm, C*texcoord);

};


class CLWContainer
{
public:
        unsigned							offset;
        SLWfragment							vtx;
        List::Vector<CLWPolygonSection>		pols;
        Buffer<SLWfragment>					vmaps;

                        CLWContainer();
                        CLWContainer(fpos_t,UINT32);
        bool            nextPoint(FILE*f, SLWpoint&target);
        unsigned        countFaceGroups(FILE*f, bool high_version);
};



class CLWInfo:public List::Vector<CLWContainer>
{
public:
        FILE           *f;
        SLWfragment    srf;

        Buffer<SLWfragment> materials;

        bool            high_version;

                        CLWInfo();
virtual                ~CLWInfo();
        void            close();
        void            zero();
        bool            open(const char*filename);
        
        void            resetMaterials();

        unsigned        countVertices();
        unsigned        countFaceGroups();
        unsigned        countMaterials();
        bool            nextMaterial(CLWMaterial&target);
        bool            retrieveMaterialContent(CLWMaterial&material);
        bool            extractPolyMap(SLWfragment fragment, CLWPolygonMap&map);
        bool            extractVertexMap(SLWfragment fragment, CLWVertexMap&map);
};

class CLWObject
{
private:
        bool            nextGroup(FILE*f, SLWfragment*frg, CLWFaceGroup&target, bool high_version, unsigned offset);

public:
        char           *name;
        UINT32          vertex_cnt;
        SLWpoint       *vertex;
        UINT32          group_cnt;
        CLWFaceGroup   *group;

                        CLWObject();
virtual                ~CLWObject();
        void            allocate();
        void            rename(const char*Name);
};

class CLWStructure
{
private:
        bool            nextGroup(FILE*f, SLWfragment*frg, CLWFaceGroup&target, bool high_version, unsigned offset);

public:
        unsigned        objects;
        CLWObject      *object;
        UINT32          material_cnt;
        CLWMaterial    *material;

                        CLWStructure();
virtual                ~CLWStructure();
        bool            open(const char*filename);
};

#include "lwo.tpl.h"
#endif
