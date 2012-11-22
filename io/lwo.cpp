#include "../global_root.h"
#include "lwo.h"

/******************************************************************

LWO-access-unit.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#define SWAP(a,b) { a^=b; b^=a; a^=b; }
#define  BSWAP_W(w)     SWAP( (((BYTE *)&w)[0]), (((BYTE *)&w)[1]) )
#define  BSWAP_L(l)     { BSWAP_W( (((WORD *)&l)[0])); BSWAP_W( (((WORD *)&l)[1])); SWAP( (((WORD *)&l)[0]), (((WORD *)&l)[1]) ) }


void SLWfragment::reset()
{
    current = addr;
    progress = 0;
}

SLWfragment::SLWfragment()
{}

SLWfragment::SLWfragment(fpos_t p, UINT32 s):addr(p),size(s)
{}


CLWFaceGroup::CLWFaceGroup():material(0),vertices(0),vertex(NULL),type(GT_FACE)
{}

CLWFaceGroup::CLWFaceGroup(const CLWFaceGroup&other):vertices(other.vertices),vertex(alloc<UINT32>(other.vertices)),type(other.type)
{
    memcpy(vertex,other.vertex,sizeof(UINT32)*vertices);
}

CLWFaceGroup::~CLWFaceGroup()
{
    dealloc(vertex);
}

void CLWFaceGroup::operator=(const CLWFaceGroup&other)
{
    dealloc(vertex);
    alloc(vertex,other.vertices);
    type = other.type;
    vertices = other.vertices;
    memcpy(vertex,other.vertex,sizeof(UINT32)*vertices);
}

UINT32&CLWFaceGroup::operator[](UINT16 index) const
{
    return vertex[index];
}

void CLWFaceGroup::allocate()
{
    dealloc(vertex);
    alloc(vertex,vertices);
}


CLWPolygonSection::CLWPolygonSection()
{}

CLWPolygonSection::CLWPolygonSection(fpos_t addr, UINT32 size):fragment(addr,size)
{}

unsigned CLWPolygonSection::countGroups(CLWInfo*info)
{
    unsigned result = 0;
    UINT32  progress(0);
    FILE*f = info->f;
    if (!info->high_version)
    {
        USHORT  junk, detail, cnt;
        SHORT   material;

        fsetpos(f,&fragment.addr);
        while (progress<fragment.size)
        {
            ASSERT__(fread(&cnt,2,1,f));
            BSWAP_W(cnt);
            progress+=2+2*cnt;
            fseek(f,2*cnt,SEEK_CUR);

            ASSERT__(fread(&material,2,1,f));
            BSWAP_W(material);
            progress+=2;

            if (material<0)
            {
                ASSERT__(fread(&detail,2,1,f));
                BSWAP_W(detail);
                progress+=2;

                for(USHORT i = 0;i < detail; i++)
                {
                    ASSERT__(fread(&junk,2,1,f));
                    BSWAP_W(junk);
                    fseek(f,(junk+1)*2,SEEK_CUR);
                    progress += ((junk+2)*2);
                }
            }
            result++;
        }
        return result;
    }

    if (fragment.size < 4)
        return 0;
//    UINT16  cnt;
    //UINT16  junk, detail;
    UINT32  type;
    //INT     material;

    fsetpos(f,&fragment.addr);
    ASSERT__(fread(&type,sizeof(type),1,f));
    if (type == LWO_FACE || type == LWO_PTCH)
    {
        UINT32 size(0);
        UINT16 lsize;
        while (size < fragment.size)
        {
            ASSERT__(fread(&lsize,2,1,f));
            BSWAP_W(lsize);
            lsize &= LW_COUNT_MASK;
            for (UINT16 k = 0; k < lsize; k++)
            {
                BYTE b;
                ASSERT__(fread(&b,1,1,f));
                if (b == 0xFF)
                {
                    fseek(f,3,SEEK_CUR);
                    size+=4;
                }
                else
                {
                    fseek(f,1,SEEK_CUR);
                    size+=2;
                }
            }
            result++;
        }
    }
    return result;
}


unsigned CLWPolygonSection::getGroups(CLWFaceGroup*targets, CLWInfo*info)
{
    SLWfragment frg = fragment;
    FILE*f = info->f;
    frg.reset();

    unsigned numof(0);
    if (!info->high_version)
    {
        USHORT junk, detail;

        fsetpos(f,&frg.addr);
        while (frg.progress < frg.size)
        {
            CLWFaceGroup&target = targets[numof++];

            ASSERT__(fread(&target.vertices,2,1,f));
            BSWAP_W(target.vertices);
            target.allocate();
            frg.progress+=2;


            for (USHORT i = 0; i < target.vertices && frg.progress < frg.size-2; i++)
            {
                UINT16 index;
               	ASSERT__(fread(&index,2,1,f));
                BSWAP_W(index);
                target[i] = index;
                frg.progress+=2;
            }

            short material;
            ASSERT__(fread(&material,2,1,f));
            BSWAP_W(material);
            frg.progress+=2;

            if (material<0)
            {
                ASSERT__(fread(&detail,2,1,f));
                BSWAP_W(detail);
                frg.progress+=2;

                for(USHORT i = 0;i < detail; i++)
                {
                   	ASSERT__(fread(&junk,2,1,f));
                    BSWAP_W(junk);
                    fseek(f,(junk+1)*2,SEEK_CUR);
                    frg.progress += ((junk+2)*2);
                }
                target.material = -material;
            }
            else
                target.material = material;
        }
        return numof;
    }

    fsetpos(f,&frg.addr);

    UINT32 ident;
   	ASSERT__(fread(&ident,sizeof(ident),1,f));
    if (ident != LWO_FACE && ident != LWO_PTCH)
        return 0;
    fgetpos(f,&frg.current);
    frg.progress+=sizeof(ident);

    while (frg.progress < frg.size)
    {
        CLWFaceGroup&target = targets[numof++];

        UINT16 lsize;
        ASSERT__(fread(&lsize,2,1,f));
        frg.progress+=2;
        BSWAP_W(lsize);
        lsize &= LW_COUNT_MASK;
        target.vertices = lsize;
        target.allocate();

        for (UINT16 k = 0; k < lsize; k++)
        {
            BYTE b;
            UINT32 index(0);
            BYTE    in[3];
           	ASSERT__(fread(&b,1,1,f));
            if (b == 0xFF)
            {
               	ASSERT__(fread(in,1,3,f));
                index = (in[0]<<16)+(in[1]<<8)+in[2];
                frg.progress+=4;
            }
            else
            {
               	ASSERT__(fread(in,1,1,f));
                index = (b<<8)+in[0];
                frg.progress+=2;
            }
            target[k] = index;
        }
    }
    unsigned offset(0);
	for (index_t j = 0; j < count(); j++)
    {
	    SLWfragment*tag = &at(j);
        CLWPolygonMap map;
        if (info->extractPolyMap(*tag,map)) //i think there should only be one valid poly-map.
            for (unsigned i = 0; i < map.length; i++)
            {
                unsigned index = map.surf[i].index+offset;
                if (index >= numof)
                    continue;
                targets[index].material = map.surf[i].surface;
            }
    }
    return numof;
}



CLWPolygonMap::CLWPolygonMap():surf(NULL),length(0)
{}

CLWPolygonMap::~CLWPolygonMap()
{
    dealloc(surf);
}









CLWMaterial::CLWMaterial():name(NULL),size(0),data(NULL)
{}

CLWMaterial::~CLWMaterial()
{
    dealloc(name);
    dealloc(data);
}

void CLWMaterial::rename(const char*Name)
{
    dealloc(name);
    name = SHIELDED_ARRAY(new char[strlen(Name)+1],strlen(Name)+1);
    strcpy(name,Name);
}

void CLWMaterial::allocate()
{
    dealloc(data);
    alloc(data,size);
}


void CLWMaterial::scan()
{
    BYTE *pntr = data;
    USHORT step(0);
/*    int len,max;
    char     *st;*/

    pntr = data;
	center = Vector<>::zero;
	dimension = Vector<>::one;
    axis = 2;
    type = MT_CUBIC;
    flags = 0;
    tiles_x = 1;
    tiles_y = 1;
    su_red = 0.8f;
    su_green = 0.8f;
    su_blue = 0.8f;
    su_lumi = 0;
    su_diff = 256;
    su_spec = 0;
    su_refl = 0;
    su_tran = 0;
    su_image = NULL;
    angle = 0;
    sm_angle = 0;



    for(unsigned len = 0; len < size; pntr += (6 + step), len+=(6 + step) )
    {
        step = *(USHORT*)&(pntr[4]);
        BSWAP_W(step);
        UINT32 id = *(UINT32*)pntr;
        pntr+=6;
        switch (id)
        {
            case LWO_COLR:
            {
                BYTE*p=pntr;
                su_red = *p++;
                su_red /= 255;
                su_green = *p++;
                su_green /= 255;
                su_blue = *p++;
                su_blue /= 255;
            }
            break;
            case LWO_FLAG:
                flags = *(short *)pntr;
                BSWAP_W(flags);
            break;
            case LWO_SMAN:
                sm_angle = *(float*)pntr;
                BSWAP_L(sm_angle);
            break;
            case LWO_TRAN:
                su_tran = *(USHORT*)pntr;
                BSWAP_W(su_tran);
            break;
            case LWO_LUMI:
                su_lumi = *(USHORT*)pntr;
                BSWAP_W(su_lumi);
            break;
            case LWO_SPEC:
                su_spec = *(USHORT*)pntr;
                BSWAP_W(su_spec);
            break;
            case LWO_DIFF:
                su_diff = *(USHORT*)pntr;
                BSWAP_W(su_diff);
            break;
            case LWO_CTEX:
            {
                UINT32  sub_id = *(UINT32*)pntr;
                type = MT_PLANAR;
                switch (sub_id)
                {
                    case LWO_Plan:
                        type = MT_PLANAR;
                    break;
                    case LWO_Cyli:
                        type = MT_CYLINDRICAL;
                    break;
                    case LWO_Sphe:
                        type = MT_SPHERICAL;
                    break;
                    case LWO_Cubi:
                        type = MT_CUBIC;
                    break;
                }
            }
            break;
            case LWO_TIMG:
                su_image = (char*)pntr;
            break;
            case LWO_TFP0:
                tiles_x = *(float*)pntr;
                BSWAP_L(tiles_x);
            break;
            case LWO_TFP1:
                tiles_y = *(float*)pntr;
                BSWAP_L(tiles_y);
            break;
            case LWO_TWRP:
                if (*(USHORT*)pntr)
                    flags |= TXF_UDECAL;
                if (*(USHORT*)&pntr[2])
                    flags |= TXF_VDECAL;
            break;
            case LWO_TCTR:
				Vec::def(center,((float*)pntr)[0],((float*)pntr)[1],((float*)pntr)[2]);
                BSWAP_L(center.x);
                BSWAP_L(center.y);
                BSWAP_L(center.z);
            break;
            case LWO_TSIZ:
				Vec::def(dimension,((float*)pntr)[0],((float*)pntr)[1],((float*)pntr)[2]);
                BSWAP_L(dimension.x);
                BSWAP_L(dimension.y);
                BSWAP_L(dimension.z);
            break;
            case LWO_TFLG:
            {
                short v = *(short*)pntr;
                BSWAP_W(v);
                flags |= v;
                if (v & 0x1)
                    axis = 0;
                else
                    if (v & 0x2)
                        axis = 1;
                    else
                        if (v & 0x4)
                            axis = 2;
            }
            break;
        }
    }
}


CLWInfo::CLWInfo():f(NULL)
{
    zero();
}

CLWInfo::~CLWInfo()
{
    if (f)
        fclose(f);
}


void CLWInfo::close()
{
    if (f)
        fclose(f);
    f = NULL;
}

void CLWInfo::zero()
{
    clear();
    materials.reset();
    #if SYSTEM==WINDOWS
        srf.addr = 0;
    #endif
    srf.size = 0;
}

bool CLWInfo::open(const char*filename)
{
    UINT32  readsize=0,size,objsize,
            id;
    fpos_t  addr;

    close();
    zero();
    f = fopen(filename,"rb");
    fgetpos(f,&srf.addr);
    if (!f)
        return false;
    fseek(f, 8, SEEK_SET);
   	ASSERT__(fread(&id,sizeof(id),1,f));
    if (id != LWO_LWOB && id != LWO_LWO2)
    {
        fclose(f);
        return false;
    }
    high_version = id == LWO_LWO2;
    fseek(f,4,SEEK_SET);
   	ASSERT__(fread(&objsize,4,1,f));
    BSWAP_L(objsize);
    fseek(f,4,SEEK_CUR); //Skip the LWOB deal
    readsize=4;
//    bool vtx_set(false);
    unsigned offset(0);
    CLWContainer*current(NULL);
    CLWPolygonSection*cpols(NULL);
    while(readsize<objsize)
    {
       	ASSERT__(fread(&id,sizeof(id),1,f));
       	ASSERT__(fread(&size,sizeof(size),1,f));
        BSWAP_L(size);
        fgetpos(f,&addr);
        fseek(f,size,SEEK_CUR);
        readsize+=size+8;

        switch (id)
        {
            case LWO_POLS:
                if (!current)
                    ErrMessage("LWO: pol-section misplaced");
                else
                    cpols = current->pols.append(SHIELDED(new CLWPolygonSection(addr,size)));
            break;
            case LWO_PNTS:
                current = append(SHIELDED(new CLWContainer(addr,size)));
                current->offset = offset;
                offset+=size/12;
                cpols = NULL;
            break;
            case LWO_SRFS:
                srf.addr = addr;
                srf.size = size;
            break;
            case LWO_SURF:
                materials << SLWfragment(addr,size);
            break;
            case LWO_VMAP:
                if (!current)
                    ErrMessage("LWO: vmap-section misplaced");
                else
                    current->vmaps << SLWfragment(addr,size);
            break;
            case LWO_PTAG:
                if (!cpols)
                    ErrMessage("LWO: ptag-section misplaced");
                else
                    cpols->operator<<(SLWfragment(addr,size));
            break;
        }
    }
    srf.reset();
    return true;
}

unsigned CLWInfo::countVertices()
{
    unsigned cnt = 0;
    reset();
    while (CLWContainer*container = each())
        cnt += container->vtx.size/12;
    return cnt;
}


unsigned CLWInfo::countMaterials()
{
    if (!high_version)
    {
        char b = 1;
        unsigned    len(0),result(0);
        fsetpos(f,&srf.addr);
        while (len < srf.size && b != 0)
        {
            len++;
           	ASSERT__(fread(&b,1,1,f));
            if (!b)
                result++;
        }
        if (!b)
            result++;
        return result;
    }
    return unsigned(materials.count());
}

CLWContainer::CLWContainer()
{}

CLWContainer::CLWContainer(fpos_t p,UINT32 s):vtx(p,s)
{}


bool CLWContainer::nextPoint(FILE*f, SLWpoint&target)
{
    fsetpos(f,&vtx.current);
    if (vtx.progress+12 <= vtx.size)
    {
       	ASSERT__(fread(&target.x,4,1,f));
       	ASSERT__(fread(&target.y,4,1,f));
       	ASSERT__(fread(&target.z,4,1,f));

        BSWAP_L(target.x);
        BSWAP_L(target.y);
        BSWAP_L(target.z);

        fgetpos(f,&vtx.current);
        vtx.progress += 12;
        return true;
    }
    return false;
}


void CLWInfo::resetMaterials()
{
    srf.reset();
    materials.reset();
}

bool CLWInfo::nextMaterial(CLWMaterial&target)
{
#if 0
    if (!high_version)
    {
        char b = 1;
        unsigned    len(0);
        fsetpos(f,&srf.current);
        while (srf.progress+len < srf.size && b != 0)
        {
            len++;
           	ASSERT__(fread(&b,1,1,f));
        }
        if (!len)
            return false;
        srf.progress+=len;
        fsetpos(f,&srf.current);
        dealloc(target.name);
        target.name = SHIELDED_ARRAY(new char[len],len);
       	ASSERT__(fread(target.name,1,len,f));
        fgetpos(f,&srf.current);
        return true;
    }
    SLWfragment*frag = materials.each();
    if (!frag || !frag->size)
        return false;
    fsetpos(f,&frag->addr);
    char name_buffer[0x100];
    BYTE at(0);

    do
       	ASSERT__(fread(&name_buffer[at],1,1,f))
    while (name_buffer[at] && ++at < (BYTE)frag->size);
    if (name_buffer[at])
        return false;
    target.rename(name_buffer);
    target.size = frag->size-at;
    target.allocate();
   	ASSERT__(fread(target.data,1,target.size,f));
#endif /*0*/
    return true;
}

bool CLWInfo::retrieveMaterialContent(CLWMaterial&material)
{
    if (high_version)
        return true;
    if (!material.name)
        return false;
    char        name_buffer[256];
    unsigned    len = (unsigned)strlen(material.name);
    if (!len || len >= 256)
        return false;
    for (unsigned i = 0; i < materials.count(); i++)
        if (materials[i].size > len+1)
        {
            fsetpos(f,&materials[i].addr);
           	ASSERT__(fread(name_buffer,1,len+1,f));
            if (!strncmp(material.name,name_buffer,len))
            {
                material.size = materials[i].size-len-1;
                material.allocate();
               	ASSERT__(fread(material.data,1,material.size,f));
                return true;
            }
        }
    return false;
}

bool CLWInfo::extractPolyMap(SLWfragment fragment, CLWPolygonMap&map)
{
    fragment.reset();
    fsetpos(f,&fragment.addr);

    UINT32 tag;
   	ASSERT__(fread(&tag,sizeof(tag),1,f));
    if (tag != LWO_SURF)
        return false;
    fragment.progress+=sizeof(tag);
    map.length = (fragment.size-fragment.progress)/4;
    alloc(map.surf,map.length);

    unsigned at(0);
    while (fragment.progress < fragment.size)
    {
        USHORT index;
       	ASSERT__(fread(&index,sizeof(index),1,f));
        BSWAP_W(index);
        map.surf[at].index = index;
       	ASSERT__(fread(&index,sizeof(index),1,f));
        BSWAP_W(index);
        map.surf[at].surface = index;
        at++;
        fragment.progress+=4;
    }
    return true;
}

bool CLWInfo::extractVertexMap(SLWfragment fragment, CLWVertexMap&map)
{
    fragment.reset();
    fsetpos(f,&fragment.addr);
   	ASSERT__(fread(&map.type,sizeof(map.type),1,f));
    if (map.type != LWO_TXUV && map.type != LWO_WGHT)
        return false;
    fragment.progress+=sizeof(map.type);
    USHORT entries;
   	ASSERT__(fread(&entries,sizeof(entries),1,f));
    BSWAP_W(entries);
    map.nverts = entries;
    fragment.progress+=sizeof(entries);
    char name_buffer[0x100];
    BYTE name_len(0);
    char b;
    do
    {
       	ASSERT__(fread(&b,1,1,f));
        name_buffer[name_len++] = b;
        fragment.progress++;
    }
    while (b);
    map.name = alloc<char>(name_len);
    memcpy(map.name,name_buffer,name_len);
    map.name[name_len-1] = 0;
    ShowMessage(map.name);
    if (name_len%2)
    {
       	ASSERT__(fread(&b,1,1,f));
        fragment.progress++;
    }
    unsigned total = (fragment.size-fragment.progress)/(map.nverts*4+2); // o_0
    ShowMessage(total);

    unsigned vnum(0);

    if (map.type==LWO_TXUV)
    {
        alloc(map.uvi,total);
        while (fragment.progress < fragment.size)
        {
//            float u=0,v=0;
            USHORT index;
           	ASSERT__(fread(&index,sizeof(index),1,f));
            BSWAP_W(index);
            map.uvi[vnum].index = index;
           	ASSERT__(fread(&map.uvi[vnum].u,sizeof(float),1,f));
            BSWAP_L(map.uvi[vnum].u);
           	ASSERT__(fread(&map.uvi[vnum].v,sizeof(float),1,f));
            BSWAP_L(map.uvi[vnum].v);
            fragment.progress+=10;
            vnum++;
        }
    }
    else
        if (map.type==LWO_WGHT)
        {
            alloc(map.wi,total);
            while (fragment.progress < fragment.size)
            {
//                float power=0;
                USHORT index;
               	ASSERT__(fread(&index,sizeof(index),1,f));
                BSWAP_W(index);
                map.wi[vnum].index = index;
               	ASSERT__(fread(&map.wi[vnum].weight,sizeof(float),1,f));
                BSWAP_L(map.wi[vnum].weight);
                fragment.progress+=6;
                vnum++;
            }
        }
    map.nverts = vnum;
    return true;
}



CLWObject::CLWObject():name(NULL),vertex_cnt(0),vertex(NULL),group_cnt(0),group(NULL)
{}

CLWObject::~CLWObject()
{
    dealloc(name);
    dealloc(vertex);
    dealloc(group);
}

void CLWObject::allocate()
{
    dealloc(vertex);
    dealloc(group);
    alloc(vertex,vertex_cnt);
    alloc(group,group_cnt);
}

/*
bool CLWObject::open(const char*filename)
{
    CLWInfo          info;
    if (!info.open(filename))
        return false;
    vertex_cnt = info.countVertices();
    group_cnt = info.countFaceGroups();
    material_cnt = info.countMaterials();
    allocate();
    ShowMessage(vertex_cnt);

    unsigned vat(0),gat(0);
    info.reset();
    while (CLWContainer*container = info.each())
    {
        container->vtx.reset();
        while (container->nextPoint(info.f,vertex[vat]))
            vat++;
        container->reset();
        while (SLWfragment*fragment = container->each())
        {
            fragment->reset();
            while (nextGroup(info.f,fragment,group[gat],info.high_version,container->offset))
                gat++;
        }
    }

    for (unsigned i = 0; i < material_cnt; i++)
    {
        info.nextMaterial(material[i]);
        if (info.retrieveMaterialContent(material[i]))
            material[i].scan();
    }
    return true;
}*/

CLWStructure::CLWStructure():objects(0),object(NULL),material_cnt(0),material(NULL)
{}

CLWStructure::~CLWStructure()
{
    dealloc(object);
    dealloc(material);
}

bool CLWStructure::open(const char*filename)
{
    CLWInfo          info;
    if (!info.open(filename))
        return false;
        
    objects = unsigned(info.count());
    material_cnt = info.countMaterials();
    dealloc(object);
    alloc(object,objects);
    dealloc(material);
    alloc(material,material_cnt);

    for (unsigned i = 0; i < info.count(); i++)
    {
        CLWContainer*container = info[i];
        
        object[i].group_cnt = 0;
        container->pols.reset();
        while (CLWPolygonSection*seg = container->pols.each())
            object[i].group_cnt+=seg->countGroups(&info);
        object[i].vertex_cnt = container->vtx.size/12;
        object[i].allocate();

        unsigned vat(0),gat(0);
        container->vtx.reset();
        while (vat < object[i].vertex_cnt && container->nextPoint(info.f,object[i].vertex[vat++]));
        container->pols.reset();
        while (CLWPolygonSection*seg = container->pols.each())
            gat += seg->getGroups(&object[i].group[gat],&info);
    }

    info.resetMaterials();
    for (unsigned i = 0; i < material_cnt; i++)
    {
        info.nextMaterial(material[i]);
        if (info.retrieveMaterialContent(material[i]))
            material[i].scan();
    }
    return true;
};


