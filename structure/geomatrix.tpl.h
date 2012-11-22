#ifndef geomatrixTplH
#define geomatrixTplH


/******************************************************************

Depreciated.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

template <class FloatType> GeoData<FloatType>::GeoData():height(0),fertility(0),variance(0)
{}

template <class FloatType>
template <class Type> GeoData<FloatType>::GeoData(const Type&scalar):height(scalar),fertility(scalar),variance(0)
{}

template <class FloatType> GeoData<FloatType>::GeoData(const FloatType&height_, const FloatType&variance_, const FloatType&fertility_):height(height_),fertility(fertility_),variance(variance_)
{}


template <class FloatType>
template <class C> INLINE   GeoData<FloatType>     GeoData<FloatType>::operator+(const GeoData<C>&other)  const
{
    return GeoData<FloatType>(height+other.height,variance+other.variance,fertility+other.fertility);
}

template <class FloatType>
template <class C> INLINE   GeoData<FloatType>     GeoData<FloatType>::operator-(const GeoData<C>&other)  const
{
    return GeoData<FloatType>(height-other.height,variance-other.variance,fertility-other.fertility);
}

template <class FloatType>
template <class C> INLINE   GeoData<FloatType>     GeoData<FloatType>::operator*(const C&scalar)  const
{
    return GeoData<FloatType>(height*scalar,variance*scalar,fertility*scalar);
}

template <class FloatType>
template <class C> INLINE   GeoData<FloatType>     GeoData<FloatType>::operator/(const C&scalar)  const
{
    return GeoData<FloatType>(height/scalar,variance/scalar,fertility/scalar);
}

template <class FloatType>
template <class C> INLINE   GeoData<FloatType>&     GeoData<FloatType>::operator+=(const GeoData<C>&other)
{
    height += other.height;
    fertility += other.fertility;
    variance+=other.variance;
    return *this;
}

template <class FloatType>
template <class C> INLINE   GeoData<FloatType>&     GeoData<FloatType>::operator-=(const GeoData<C>&other)
{
    height -= other.height;
    fertility -= other.fertility;
    variance -= other.variance;
    return *this;
}

template <class FloatType>
template <class C> INLINE   GeoData<FloatType>&     GeoData<FloatType>::operator*=(const C&scalar)
{
    height *= scalar;
    fertility *= scalar;
    variance *= scalar;
    return *this;
}

template <class FloatType>
template <class C> INLINE   GeoData<FloatType>&     GeoData<FloatType>::operator/=(const C&scalar)
{
    height /= scalar;
    fertility /= scalar;
    variance /= scalar;
    return *this;
}



template <class FloatType> GeoVertex<FloatType>::GeoVertex()
{}

template <class FloatType>
template <class C0, class C1, class C2> GeoVertex<FloatType>::GeoVertex(const Vec3<C0>&vec, const Vec3<C1>&norm,const GeoData<C2>&dat):position(vec),normal(norm),data(dat)
{}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>   GeoVertex<FloatType>::operator+(const GeoVertex<C>&other)    const
{
    return GeoVertex<FloatType>(position+other.position,normal+other.normal,data+other.data);
}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>   GeoVertex<FloatType>::operator-(const GeoVertex<C>&other)    const
{
    return GeoVertex<FloatType>(position-other.position,normal-other.normal,data-other.data);
}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>   GeoVertex<FloatType>::operator*(const C&scalar)               const
{
    return GeoVertex<FloatType>(position*(FloatType)scalar,normal*(FloatType)scalar,data*scalar);
}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>   GeoVertex<FloatType>::operator/(const C&scalar)               const
{
    return GeoVertex<FloatType>(position/(FloatType)scalar,normal/(FloatType)scalar,data/scalar);
}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>&   GeoVertex<FloatType>::operator+=(const GeoVertex<C>&other)
{
    position += other.position;
    normal += other.normal;
    data += other.data;
}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>&   GeoVertex<FloatType>::operator-=(const GeoVertex<C>&other)
{
    position -= other.position;
    normal -= other.normal;
    data -= other.data;
}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>&   GeoVertex<FloatType>::operator*=(const C&scalar)
{
    position *= scalar;
    normal *= scalar;
    data *= scalar;
}

template <class FloatType>
template <class C> INLINE   GeoVertex<FloatType>&   GeoVertex<FloatType>::operator/=(const C&scalar)
{
    position /= scalar;
    normal /= scalar;
    data /= scalar;
}

template <class FloatType> INLINE FloatType GeoVertex<FloatType>::random()
{
    return (FloatType)rand()/RAND_MAX*2-1;
}

template <class FloatType> INLINE void GeoVertex<FloatType>::createFrom(const GeoVertex<FloatType>&v0, const GeoVertex<FloatType>&v1, const FloatType&hvariance, const FloatType&variance)
{
    data.height = (v0.data.height + v1.data.height)/2+random()*hvariance;
    data.variance = clamped((v0.data.variance + v1.data.variance)/2+random()*variance*2,0,1);
    data.fertility = (v0.data.fertility + v1.data.fertility)/2+random()*variance;
    _setLen(position.v,data.height);
}

template <class FloatType> INLINE void GeoVertex<FloatType>::setHeight(const FloatType&height)
{
    data.height = height;
    _setLen(position.v,height);
}

template <class FloatType> INLINE void GeoVertex<FloatType>::check(const String&context) const
{
    if (vabs(data.height - position.length()) > GLOBAL_ERROR*10)
        FATAL__("generator-exception ("+context+"): data.height should be "+FloatToStr(position.length())+" but is "+FloatToStr(data.height)+" (difference "+FloatToStr(vabs(data.height - position.length()))+" > "+FloatToStr(GLOBAL_ERROR*10)+")");
}


/*

template <class GeoModule> GeoNode<GeoModule>::GeoNode(unsigned x_, unsigned y_, GeoTriangle<GeoModule>*origin_):x(x_),y(y_),origin(origin_),target(&origin_->getVertex(x_,y_))
{
    if ((x && x != GeoTriangle<GeoModule>::range-1) || (y && y != GeoTriangle<GeoModule>::range-1))
    {
        max = 3+(bool)(x>0)+(bool)(y < GeoTriangle<GeoModule>::range-1)+(bool)(x!=y);
        corner = 0;
        return;
    }
    if (x == 0 && y == 0)
        corner = 1;
    else
        if (x == 0)
            corner = 2;
        else
            corner = 3;

    BYTE n0 = (corner+1)%3,
         n1 = corner-1;
    Link l0 = origin->link[origin->neighbour[n0].link,(origin->neighbour[n0].orientation+2)%3),
         l1 = origin->link[origin->neighbour[n1].link,(origin->neighbour[n1].orientation+1)%3);


    max = 1;
    while (l0.link && l0.link != origin)
    {
        max++;
        l0 = origin->link(l0.link->neighbour[l0.orientation].link, (l0.link->neighbour[l0.orientation].orientation+2)%3);
    }

    if (l0.link == this)
        return;


    while (l1.link)
    {
        max++;
        l1 = link(l1.link->neighbour[l1.orientation].link, (l1.link->neighbour[l1.orientation].orientation+1)%3);
    }
}

template <class GeoModule> bool GeoNode<GeoModule>::evolve(DynamicPQueue<GeoNode<GeoModule> >&queue, Float&step)
{
    unsigned distro=0;

    if (!corner)
    {
        if (x)
        {
            distro += distribute(x-1,y,queue,step);
            distro += distribute(x-1,y-1,queue,step);
        }
        else
            distro += xport(0,y,queue,step);


        if (y < GeoTriangle<GeoModule>::range-1)
        {
            distro += distribute(x,y+1,queue,step);
            distro += distribute(x+1,y+1,queue,step);
        }
        else
            distro += xport(1,x,queue,step);

        if (x < y)
        {
            distro += distribute(x+1,y,queue,step);
            distro += distribute(x+1,y+1,queue,step);
        }
        else
            distro += xport(2,GeoTriangle<GeoModule>::range-1-x,queue,step);
            
        return distro == max;
    }
    if (rand() > RAND_MAX/6)    



}*/


template <class GeoModule> GeoVertex<typename GeoModule::FloatType> GeoTriangle<GeoModule>::centerOf(const Vertex&v0, const Vertex&v1)
{
    Vertex  result;
    Float   delta[3],distance(_distance(v0.position.v,v1.position.v)),
            normal0[3],normal1[3],direction0[3],direction1[3],control0[3],control1[3];
    _sub(v1.position.v,v0.position.v,delta);
    _cross(v0.normal.v,delta,normal0);
    _cross(v1.normal.v,delta,normal1);
    _cross(v0.normal.v,normal0,direction0);
    _cross(v1.normal.v,normal1,direction1);
    Float balance = v0.data.height/v1.data.height;
    _setLen(direction0,-distance/3);
    _setLen(direction1,distance/3);
    _add(v0.position.v,direction0,control0);
    _add(v1.position.v,direction1,control1);
    _resolveNURBS(v0.position.v,control0,control1,v1.position.v,0.5,result.position.v);
//    _add(v0.normal.v,v1.normal.v,result.normal.v);
  //  result.data = (v0.data+v1.data)/2;
    result.data.height = result.position.length();


//    result.data.variance += vabs(v0.data.variance-v1.data.variance)*0.5*random();
    result.data.fertility = (v0.data.fertility+v1.data.fertility)/2+vabs(v0.data.fertility-v1.data.fertility)*0.1*random();
    result.data.variance = clamped((v0.data.variance+v1.data.variance)/2+vabs(v0.data.variance-v1.data.variance)*2*random(),0,1);

    



    Float d,var = clamped((result.data.height-super->bottom.height+super->height_range)/super->height_range,0,1);   //clamped((result.data.variance-0.5)*2*(result.data.height-super->bottom.height-super->height_range)/super->height_range,0,1);  //0.5-cos(result.data.variance*M_PI)/2;
    //sin(result.data.variance*4*M_PI)/2+0.5;    //
/*        if (result.data.variance < 0.75)
        else*/

    if (depth > 1)
        d = (vpow(var,1.5) + vpow(var,19))* distance/10*super->height_range*depth;
    else
        d = vabs(v0.data.height-v1.data.height)/6;

    result.data.height += d*random();
//    result.data.height = super->bottom.height + (super->top.height-super->bottom.height)*result.data.variance;
//    result.data.height = (result.data.height + super->bottom.height+pow(var,2)*(super->height_range))/2;
//    result.data.height = super->bottom.height + super->height_range*var;        
    _setLen(result.position.v,result.data.height);

//    vary(result);
    return result;
}

template <class GeoModule> INLINE typename GeoModule::FloatType  GeoTriangle<GeoModule>::random()
{
    return (Float)rand()/RAND_MAX*2-1;
}



template <class GeoModule> INLINE void GeoTriangle<GeoModule>::dropChildren()
{
    BEGIN
    if (c0)
        delete c0;
    if (c1)
        delete c1;
    if (c2)
        delete c2;
    if (c3)
        delete c3;
    c0 = NULL;
    c1 = NULL;
    c2 = NULL;
    c3 = NULL;
    updateBox();
    END
}


template <class GeoModule> SGeoTriangleLink<GeoModule>  GeoTriangle<GeoModule>::link()
{
    SGeoTriangleLink<GeoModule>    result;
    result.link = NULL;
    result.orientation = 0;
    return result;
}

template <class GeoModule> SGeoTriangleLink<GeoModule>  GeoTriangle<GeoModule>::link(GeoTriangle<GeoModule> *link_)
{
    SGeoTriangleLink<GeoModule>    result;
    result.link = link_;
    result.orientation = 0;
    return result;
}

template <class GeoModule> SGeoTriangleLink<GeoModule>  GeoTriangle<GeoModule>::link(GeoTriangle<GeoModule> *link_, BYTE orientation_)
{
    SGeoTriangleLink<GeoModule>    result;
    result.link = link_;
    result.orientation = orientation_;
    return result;
}


template <class GeoModule> SGeoTriangleLink<GeoModule>  GeoSphere<GeoModule>::link(GeoTriangle<GeoModule> *link_, BYTE orientation_)
{
    SGeoTriangleLink<GeoModule>    result;
    result.link = link_;
    result.orientation = orientation_;
    return result;
}

template <class GeoModule> void GeoTriangle<GeoModule>::clearLinkage()
{
    n0 = link();
    n1 = link();
    n2 = link();
}


template <class GeoModule> GeoTriangle<GeoModule>::GeoTriangle():c0(NULL),c1(NULL),c2(NULL),c3(NULL),super(NULL),depth(0),p0(true),p1(true),p2(true)
{
    clearLinkage();
}

template <class GeoModule> GeoTriangle<GeoModule>::GeoTriangle(GeoSphere<GeoModule>*super_,const Link&parent_, const Link&n0_, const Link&n1_, const Link&n2_, const __int64&time):c0(NULL),c1(NULL),c2(NULL),c3(NULL),parent(parent_),n0(n0_),n1(n1_),n2(n2_),depth(parent_.link->depth-1),super(super_),p0(true),p1(true),p2(true),referenced(time)
{
    LBEGIN
    super->sectors++;
    super->memory+=sizeof(*this);
    if (n0.link)
    {
        n0.link->neighbour[n0.orientation] = link(this,0);
        n0.link->primary[n0.orientation] = false;
    }
    if (n1.link)
    {
        n1.link->neighbour[n1.orientation] = link(this,1);
        n1.link->primary[n1.orientation] = false;
    }
    if (n2.link)
    {
        n2.link->neighbour[n2.orientation] = link(this,2);
        n2.link->primary[n2.orientation] = false;
    }
        
    if (parent.orientation < 3)
    {
        unsigned yoff = parent.orientation?range/2:0,
                 xoff = parent.orientation==2?range/2:0;

        for (unsigned y = 0; y <= range/2; y++)
            for (unsigned x = 0; x <= y; x++)
            {
                getVertex(x*2,y*2) = parent.link->getVertex(xoff+x, yoff+y);
            }
    }
    else
        for (unsigned y = 0; y <= range/2; y++)
            for (unsigned x = 0; x <= y; x++)
            {
                getVertex(x*2,y*2) = parent.link->getVertex(range/2-x, range-y-1);
            }

    for (BYTE k = 0; k < 3; k++)
    {
        Link&n = neighbour[k];
        unsigned*local = border[k],
                *remote = border[n.orientation];
        if (n.link)
            for (unsigned i = 1; i < range; i+=2)
                vertex[local[i]] = n.link->vertex[remote[range-1-i]];
        else
            for (unsigned i = 1; i < range; i+=2)
                vertex[local[i]] = centerOf(vertex[local[i-1]],vertex[local[i+1]]);
    }
    


    for (unsigned y = 1; y < range; y+=2)
    {
        for (unsigned x = 2; x <= y; x+=2)
            getVertex(x,y) = centerOf(getVertex(x,y-1),getVertex(x,y+1));
        for (unsigned x = 1; x < y; x+=2)
            getVertex(x,y) = centerOf(getVertex(x-1,y-1),getVertex(x+1,y+1));
    }
    for (unsigned y = 2; y < range-1; y+=2)
        for (unsigned x = 1; x <=y; x+=2)
            getVertex(x,y) = centerOf(getVertex(x-1,y),getVertex(x+1,y));

    fillAllNormals();

    if (parent.orientation < 3)
    {
        unsigned yoff = parent.orientation?range/2:0,
                 xoff = parent.orientation==2?range/2:0;

        for (unsigned y = 0; y <= range/2; y++)
            for (unsigned x = 0; x <= y; x++)
                getVertex(x*2,y*2).normal = parent.link->getVertex(xoff+x, yoff+y).normal;
    }
    else
        for (unsigned y = 0; y <= range/2; y++)
            for (unsigned x = 0; x <= y; x++)
                getVertex(x*2,y*2).normal = parent.link->getVertex(range/2-x, range-y-1).normal;


//    fillHalfNormals();

    mergeHalfNormals();


//    mergeCornerNormals();
//    ShowMessage(getCorner(0).position.toString()+" - "+getCorner(1).position.toString()+" - "+getCorner(2).position.toString());
    updateBox();
    GeoModule::build(this);
    END
}

template <class GeoModule> GeoTriangle<GeoModule>::~GeoTriangle()
{
    LBEGIN
    if (n0.link)
    {
        n0.link->neighbour[n0.orientation].link = NULL;
        n0.link->primary[n0.orientation] = true;
    }
    if (n1.link)
    {
        n1.link->neighbour[n1.orientation].link = NULL;
        n1.link->primary[n1.orientation] = true;
    }
    if (n2.link)
    {
        n2.link->neighbour[n2.orientation].link = NULL;
        n2.link->primary[n2.orientation] = true;
    }
    super->sectors--;
    super->memory-=sizeof(*this);
    if (c0)
        delete c0;
    if (c1)
        delete c1;
    if (c2)
        delete c2;
    if (c3)
        delete c3;
    GeoModule::free(this);
    END
}


template <class GeoModule>
template <class C0, class C1> inline bool GeoTriangle<GeoModule>::intersectsBox(const C0 p0[3], const C1 p1[3])
{
    return _oIntersectsBox(p0,p1,local_box);
}










template <class GeoModule> void GeoTriangle<GeoModule>::setup(const Float*p0, const Float*p1, const Float*p2, const __int64&time)
{
    LBEGIN
    referenced = time;
//    ShowMessage(FloatToStr(p0[3])+" - "+FloatToStr(p1[3])+" - "+FloatToStr(p2[3]));
    Float   dx[4],dy[4];
    _subtract4(p0,p1,dy);
    _subtract4(p2,p1,dx);
    for (unsigned y = 0; y < range; y++)
        for (unsigned x = 0; x <= y; x++)
        {
            Vertex&v = getVertex(x,y);
            Float fy = 1-(Float)y/(range-1),
                  fx = (Float)x/(range-1),
                  *out = v.position.v;
            _mad(p1,dy,fy,out);
            _mad(out,dx,fx);
            v.data.variance = (p1[3]+dy[3]*fy + dx[3]*fx);
//            ShowMessage(v.data.variance);
            _setLen(v.position.v,v.data.height);
            _divide(v.position.v,v.data.height,v.normal.v);  //normalized normal
        }
    END
}


template <class GeoModule> SGeoTriangleLink<GeoModule> GeoTriangle<GeoModule>::resolveChild(const Link&ref, BYTE index)
{
    if (!ref.link)
        return link();
    return link(ref.link->child[(index+ref.orientation)%3],ref.orientation);
}

template <class GeoModule> SGeoTriangleLink<GeoModule> GeoTriangle<GeoModule>::resolveChildRec(const Link&ref, BYTE index)
{
    if (!ref.link || !ref.link->child[(index+ref.orientation)%3] || !ref.link->recursive)
        return ref;
    return link(ref.link->child[(index+ref.orientation)%3],ref.orientation);
}


template <class GeoModule> void GeoTriangle<GeoModule>::checkTimeouts(const __int64&time, float ttl)
{
            if (timer.toSecondsf(time-referenced) > ttl)
            {
                dropChildren();
            }
            else
            {
                if (c0)
                    c0->checkTimeouts(time,ttl/2);
                if (c1)
                    c1->checkTimeouts(time,ttl/2);
                if (c2)
                    c2->checkTimeouts(time,ttl/2);
                if (c3)
                    c3->checkTimeouts(time,ttl/2);
            }
}


template <class GeoModule> void GeoTriangle<GeoModule>::cast(GeoModule&module, const __int64&time, float ttl)
{
    LBEGIN

    if (recursive = module.cast(this)&&depth)
    {

        if (!c0)
            c0 = new Triangle(super,link(this,0),resolveChild(n0,1),link(),resolveChild(n2,0),time);
        if (!c1)
            c1 = new Triangle(super,link(this,1),resolveChild(n0,0),resolveChild(n1,1),link(),time);
        if (!c2)
            c2 = new Triangle(super,link(this,2),link(),resolveChild(n1,0),resolveChild(n2,1),time);
        if (!c3)
            c3 = new Triangle(super,link(this,3),link(c2,0),link(c0,1),link(c1,2),time);

        c0->cast(module,time,ttl/2);
        c1->cast(module,time,ttl/2);
        c2->cast(module,time,ttl/2);
        c3->cast(module,time,ttl/2);

    }
    else
    {
        recursive = false;
        if (depth)
            checkTimeouts(time,ttl);
    }
    referenced = time;
    END
}

template <class GeoModule> void GeoTriangle<GeoModule>::resolve()
{
    resolve(n0,n1,n2);
}

template <class GeoModule> void GeoTriangle<GeoModule>::resolve(const Link&l0, const Link&l1, const Link&l2)
{
    LBEGIN
    vis0 = l0.link?l0.link->depth:depth;
    vis1 = l1.link?l1.link->depth:depth;
    vis2 = l2.link?l2.link->depth:depth;

    if (recursive)
    {
        c0->resolve(resolveChildRec(l0,1), link(c3,1),resolveChildRec(l2,0));
        c1->resolve(resolveChildRec(l0,0),resolveChildRec(l1,1),link(c3,2));
        c2->resolve(link(c3,0),resolveChildRec(l1,0),resolveChildRec(l2,1));
        c3->resolve();
    }
    END
}


template <class GeoModule> void GeoTriangle<GeoModule>::render(GeoModule&module)
{
    if (module.render(this) == REQUEST_DETAIL && depth)
    {
        if (!c0)
            c0 = new Triangle(super,link(this,0),resolveChild(n0,1),link(),resolveChild(n2,0));
        if (!c1)
            c1 = new Triangle(super,link(this,1),resolveChild(n0,0),resolveChild(n1,1),link());
        if (!c2)
            c2 = new Triangle(super,link(this,2),link(),resolveChild(n1,0),resolveChild(n2,1));
        if (!c3)
            c3 = new Triangle(super,link(this,3),link(c2,0),link(c0,1),link(c1,2));

        c0->render(module);
        c1->render(module);
        c2->render(module);
        c3->render(module);
    }
    else
        if (depth <= 2)
        {
            if (c0)
                delete c0;
            if (c1)
                delete c1;
            if (c2)
                delete c2;
            if (c3)
                delete c3;
            c0 = NULL;
            c1 = NULL;
            c2 = NULL;
            c3 = NULL;
        }
}

template <class GeoModule> void GeoTriangle<GeoModule>::updateBox()
{
BEGIN

    _c3(vertex[0].position.v,local_box);
    _c3(local_box,local_box+3);
    
    _oDetDimension(vertex[vertices-range].position.v,local_box);
    _oDetDimension(vertex[vertices-1].position.v, local_box);
    _oDetDimension(vertex[vertices/4-1].position.v, local_box);
    _oDetDimension(vertex[vertices/4-range/2].position.v,local_box);
    _oDetDimension(vertex[vertices-range/2].position.v,local_box);
    
/*    for (unsigned y = 0; y < range; y+=4)
        for (unsigned x = 0; x <= y; x+=4)
        _oDetDimension(getVertex(x,y).position.v,local_box);*/
    Float center[3];
    _center(local_box,local_box+3,center);
    _scale(center,1.5,local_box);
    _scale(center,1.5,local_box+3);
    
    _c6(local_box,global_box);
    
    if (c0)
    {
        _oDetDimension(c0->global_box,global_box);
        _oDetDimension(c0->global_box+3,global_box);
    }
    if (c1)
    {
        _oDetDimension(c1->global_box,global_box);
        _oDetDimension(c1->global_box+3,global_box);
    }
    if (c2)
    {
        _oDetDimension(c2->global_box,global_box);
        _oDetDimension(c2->global_box+3,global_box);
    }
    if (c3)
    {
        _oDetDimension(c3->global_box,global_box);
        _oDetDimension(c3->global_box+3,global_box);
    }

    if (parent.link)
        parent.link->modifyBox(global_box);
END
}

template <class GeoModule> void GeoTriangle<GeoModule>::modifyBox(Float child_box[6])
{
    _oDetDimension(child_box,global_box);
    _oDetDimension(child_box+3,global_box);
    if (parent.link)
        parent.link->modifyBox(global_box);
}

template <class GeoModule> void GeoTriangle<GeoModule>::setNeighbours(const Link&link0, const Link&link1, const Link&link2)
{
    BEGIN
    n0 = link0;
    n1 = link1;
    n2 = link2;
    if (n0.link)
        n0.link->primary[n0.orientation] = !p0;
    if (n1.link)
        n1.link->primary[n1.orientation] = !p1;
    if (n1.link)
        n1.link->primary[n1.orientation] = !p2;
    END
}

template <class GeoModule> void GeoTriangle<GeoModule>::copyEdge(BYTE k)
{
    BEGIN
    Link&n = neighbour[k];
    if (!n.link)
        return;
    unsigned*border0 = border[k],
            *border1 = border[n.orientation];
    for (unsigned i = 0; i < range; i++)
        vertex[border0[i]] = n.link->vertex[border1[range-1-i]];
    END
}


template <class GeoModule> void GeoTriangle<GeoModule>::fillHalfNormals()
{
    BEGIN
    
    #define DERIVED(x, y)   (!(x%2) && !(y%2))
    
    for (unsigned y = 1; y < range-1; y++)
        for (unsigned x = 1; x < y; x++)
            _clear(getVertex(x,y).normal.v);
    for (BYTE k = 0; k < 3; k++)
        for (unsigned i = 1; i < range-1; i+=2)
            _clear(vertex[border[k][i]].normal.v);


    for (unsigned y = 1; y < range; y++)
    {
        for (unsigned x = 0; x < y; x++)
        {
            Vertex  &v0 = getVertex(x,y),
                    &v1 = getVertex(x+1,y),
                    &v2 = getVertex(x,y-1);

            Float   normal[3];
            _oTriangleNormal(v0.position.v,v1.position.v,v2.position.v,normal);
            if ((x && x < y) || (y%2))
                _add(v0.normal.v,normal);
            if ((x < y-1) || (y%2))
                _add(v1.normal.v,normal);
            if ((x && x < y) || !(y%2))
                _add(v2.normal.v,normal);
        }
        for (unsigned x = 0; x < y-1; x++)
        {
            Vertex  &v0 = getVertex(x+1,y),
                    &v1 = getVertex(x+1,y-1),
                    &v2 = getVertex(x,y-1);

            Float   normal[3];
            _oTriangleNormal(v0.position.v,v1.position.v,v2.position.v,normal);
            if ((x < y-1) || (y%2))
                _add(v0.normal.v,normal);
            if ((x < y-1) || !(y%2))
                _add(v1.normal.v,normal);
            if ((x && x < y) || !(y%2))
                _add(v2.normal.v,normal);
        }
    }
    for (unsigned y = 1; y < range-1; y++)
        for (unsigned x = 1; x < y; x++)
            _normalize(getVertex(x,y).normal.v);
    for (BYTE k = 0; k < 3; k++)
        for (unsigned i = 1; i < range-1; i+=2)
            _normalize(vertex[border[k][i]].normal.v);
    END

}

template <class GeoModule> void GeoTriangle<GeoModule>::fillAllNormals()
{
    BEGIN
    for (unsigned i = 0; i < vertices; i++)
        _clear(vertex[i].normal.v);

    for (unsigned y = 1; y < range; y++)
    {
        for (unsigned x = 0; x < y; x++)
        {
            Vertex  &v0 = getVertex(x,y),
                    &v1 = getVertex(x+1,y),
                    &v2 = getVertex(x,y-1);

            Float   normal[3];
            _oTriangleNormal(v0.position.v,v1.position.v,v2.position.v,normal);
            _add(v0.normal.v,normal);
            _add(v1.normal.v,normal);
            _add(v2.normal.v,normal);
        }
        for (unsigned x = 0; x < y-1; x++)
        {
            Vertex  &v0 = getVertex(x+1,y),
                    &v1 = getVertex(x+1,y-1),
                    &v2 = getVertex(x,y-1);

            Float   normal[3];
            _oTriangleNormal(v0.position.v,v1.position.v,v2.position.v,normal);
            _add(v0.normal.v,normal);
            _add(v1.normal.v,normal);
            _add(v2.normal.v,normal);
        }
    }
    for (unsigned i = 0; i < vertices; i++)
        _normalize(vertex[i].normal.v);
    END
/*    for (unsigned y = 1; y < range-1; y++)
        for (unsigned x = 1; x < y; x++)
            _normalize(getVertex(x,y).normal.v);*/
}


template <class GeoModule> void GeoTriangle<GeoModule>::mergeHalfNormals()
{
    BEGIN
    for (BYTE k = 0; k < 3; k++)
        if (neighbour[k].link)
        {
            for (unsigned i = 1; i < range-1; i+=2)
            {
                Vertex&local = vertex[border[k][i]],
                      &remote = neighbour[k].link->vertex[border[neighbour[k].orientation][range-i-1]];
                _add(local.normal.v,remote.normal.v);
                _normalize(local.normal.v);
                _c3(local.normal.v,remote.normal.v);
            }
            neighbour[k].link->updateBox();
            GeoModule::update(neighbour[k].link);
        }
    END
}

template <class GeoModule> void GeoTriangle<GeoModule>::mergeBorderNormals(bool update)
{
    BEGIN
    for (BYTE k = 0; k < 3; k++)
        if (neighbour[k].link)
        {
            for (unsigned i = 1; i < range-1; i++)
            {
                Vertex&local = vertex[border[k][i]],
                      &remote = neighbour[k].link->vertex[border[neighbour[k].orientation][range-i-1]];
                _add(local.normal.v,remote.normal.v);
                _normalize(local.normal.v);
                _c3(local.normal.v,remote.normal.v);
            }
            if (update)
            {
                neighbour[k].link->updateBox();
                GeoModule::update(neighbour[k].link);
            }
        }
    END
}

unsigned instance;

template <class GeoModule> void GeoTriangle<GeoModule>::cornerNormal(BYTE index, Float check[3], Float out[3])
{
//    Vertex&corner = getCorner(index);
/*    if (_zero(check))
        _c3(corner.position.v,check);*/
/*    if (!_similar(corner.position.v,check,GLOBAL_ERROR))
        FATAL__("vertex-coords difere ("+IntToStr(instance)+") by "+FloatToStr(_distance(corner.position.v,check)));*/

    _oTriangleNormal(vertex[border[index][0]].position.v,vertex[border[index][1]].position.v,vertex[border[(index+2)%3][range-2]].position.v,out);
}


template <class GeoModule> GeoVertex<typename GeoModule::FloatType>& GeoTriangle<GeoModule>::getCorner(BYTE index)
{
    return vertex[border[index][0]];
}

template <class GeoModule> void GeoTriangle<GeoModule>::setCornerNormal(BYTE index, const Float normal[3])
{
    _c3(normal, getCorner(index).normal.v);

}

template <class GeoModule> void GeoTriangle<GeoModule>::mergeCornerNormals(bool update)
{
    BEGIN
    for (BYTE k = 0; k < 3; k++)
    {
        Float   normal[3],pos[3]={0,0,0};
        instance = 0;
        _oTriangleNormal(vertex[border[k][0]].position.v,vertex[border[k][1]].position.v,vertex[border[(k+2)%3][range-2]].position.v,normal);
//        cornerNormal(k,pos,normal);

        Link t = link(neighbour[(k+2)%3].link,(neighbour[(k+2)%3].orientation+2)%3);
        while (t.link && t.link != this)
        {
            instance++;
            Float   n[3];
            BYTE j = (t.orientation+1)%3;
            _oTriangleNormal(t.link->vertex[border[j][0]].position.v,t.link->vertex[border[j][1]].position.v,t.link->vertex[border[(j+2)%3][range-2]].position.v,n);
//            t.link->cornerNormal((t.orientation+1)%3, pos, n);
            _add(normal,n);
            t = link(t.link->neighbour[t.orientation].link, (t.link->neighbour[t.orientation].orientation+2)%3);
        }
        instance = 0;
        if (t.link != this)
        {
            t = link(neighbour[k].link,(neighbour[k].orientation+1)%3);
            while (t.link)
            {
                instance--;
                Float   n[3];
                BYTE j = (t.orientation);
                _oTriangleNormal(t.link->vertex[border[j][0]].position.v,t.link->vertex[border[j][1]].position.v,t.link->vertex[border[(j+2)%3][range-2]].position.v,n);

//                t.link->cornerNormal((t.orientation)%3, pos, n);
                _add(normal,n);
                t = link(t.link->neighbour[t.orientation].link, (t.link->neighbour[t.orientation].orientation+1)%3);
            }
        }
        
        _normalize(normal);
        setCornerNormal(k,normal);
        t = link(neighbour[(k+2)%3].link,(neighbour[(k+2)%3].orientation+2)%3);
        while (t.link && t.link != this)
        {
            _c3(normal,t.link->vertex[border[(t.orientation+1)%3][0]].normal.v);
//            t.link->setCornerNormal((t.orientation+1)%3, normal);
            if (update)
            {
                t.link->updateBox();
                GeoModule::update(t.link);
            }
            t = link(t.link->neighbour[t.orientation].link, (t.link->neighbour[t.orientation].orientation+2)%3);
        }

        if (t.link != this)
        {
            t = link(neighbour[k].link,(neighbour[k].orientation+1)%3);
            while (t.link)
            {
                _c3(normal,t.link->vertex[border[t.orientation][0]].normal.v);
    //            t.link->setCornerNormal((t.orientation)%3, normal);
                if (update)
                {
                    t.link->updateBox();
                    GeoModule::update(t.link);
                }
                t = link(t.link->neighbour[t.orientation].link, (t.link->neighbour[t.orientation].orientation+1)%3);
            }
        }
    }
    END
}


template <class GeoModule> void GeoTriangle<GeoModule>::checkLinkage()
{
    BEGIN
    if (parent.link && parent.link->child[parent.orientation] != this)
        FATAL__("parent-orientation broken");
    for (BYTE k = 0; k < 3; k++)
        if (neighbour[k].link)
        {
            if (neighbour[k].link == this)
                FATAL__("self referential error");
            if (neighbour[k].link->neighbour[neighbour[k].orientation].link != this || neighbour[k].link->neighbour[neighbour[k].orientation].orientation != k)
                FATAL__("neighbour-link "+IntToStr(k)+" broken");
            if (!_similar(neighbour[k].link->getCorner(neighbour[k].orientation).position.v,getCorner((k+1)%3).position.v,GLOBAL_ERROR)
                ||
                !_similar(neighbour[k].link->getCorner((neighbour[k].orientation+1)%3).position.v,getCorner(k).position.v,GLOBAL_ERROR))
                FATAL__("corners differ");
        }
    for (BYTE k = 0; k < 4; k++)
        if (child[k])
            child[k]->checkLinkage();
            
/*    for (unsigned i = 0; i < vertices; i++)
        if (vertex[i].position.x < box[0] || vertex[i].position.x > box[3]
            ||
            vertex[i].position.y < box[1] || vertex[i].position.y > box[4]
            ||
            vertex[i].position.z < box[2] || vertex[i].position.z > box[5])
                FATAL__("box insufficient");*/
    END
}



template <class GeoModule> void GeoTriangle<GeoModule>::showMe()
{
    cout << "holding "<<vertices<<" vertices in an "<<range<<"*"<<range<<"/2 cell-field. depth is "<<depth<<endl;
    for (unsigned y = 0; y < range; y++)
    {
        for (unsigned x = 0; x <= y; x++)
            cout << " "<<getVertex(x,y).position.toString().c_str();
        cout << endl;
    }
}

template <class GeoModule> void GeoTriangle<GeoModule>::generateLayer(unsigned step)
{
    BEGIN
    Float   variance = (Float)step/(1<<size),
            hvariance = ((Float)step/(1<<size)*super->height_range);


    for (unsigned y = step; y < range; y+=2*step)
    {
        for (unsigned x = 2*step; x <= y; x+=2*step)
            getVertex(x,y).createFrom(getVertex(x,y-step), getVertex(x,y+step),hvariance,variance);
        for (unsigned x = step; x < y; x+=2*step)
            getVertex(x,y).createFrom(getVertex(x-step,y-step),getVertex(x+step,y+step),hvariance,variance);
    }
    for (unsigned y = 2*step; y < range-1; y+=2*step)
        for (unsigned x = step; x <=y; x+=2*step)
            getVertex(x,y).createFrom(getVertex(x-step,y),getVertex(x+step,y),hvariance,variance);

    for (BYTE k = 0; k < 3; k++)
        if (primary[k])
            for (unsigned i = step; i < range-step; i+=step*2)
            {
                vertex[border[k][i]].createFrom(vertex[border[k][i-step]],vertex[border[k][i+step]],hvariance,variance);
                if (neighbour[k].link)
                    neighbour[k].link->vertex[border[neighbour[k].orientation][range-1-i]] = vertex[border[k][i]];
            }
    END
}

template <class GeoModule> void GeoTriangle<GeoModule>::randomSmooth()
{
    BEGIN
    for (unsigned y = 1; y < range-1; y++)
        for (unsigned x = 1; x < y; x++)
        {
            unsigned influenced(1);
            Vertex&v = getVertex(x,y);
            v.data.height*=influenced;
            if (rand() < RAND_MAX/4)
            {
                v.data.height+=getVertex(x-1,y).data.height;
                influenced++;
            }
            if (rand() < RAND_MAX/4)
            {
                v.data.height+=getVertex(x+1,y).data.height;
                influenced++;
            }
            if (rand() < RAND_MAX/4)
            {
                v.data.height+=getVertex(x,y+1).data.height;
                influenced++;
            }
            if (rand() < RAND_MAX/4)
            {
                v.data.height+=getVertex(x,y-1).data.height;
                influenced++;
            }
            if (rand() < RAND_MAX/4)
            {
                v.data.height+=getVertex(x+1,y-1).data.height;
                influenced++;
            }
            if (rand() < RAND_MAX/4)
            {
                v.data.height+=getVertex(x-1,y+1).data.height;
                influenced++;
            }
            v.data.height/=influenced;
            _setLen(v.position.v,v.data.height);
        }
    END
}

template <class GeoModule> void GeoTriangle<GeoModule>::defineBorders()
{
    BEGIN
    for (unsigned i = 0; i < range; i++)
    {
        border[0][i] = getIndex(0,i);
        border[1][i] = getIndex(i,range-1);
        border[2][range-i-1] = getIndex(i,i);
    }

    for (unsigned i = 1; i < range; i++)
    {
        inner[0][i-1] = getIndex(1,i);
        inner[1][i-1] = getIndex(i-1,range-2);
        inner[2][range-i-1] = getIndex(i-1,i);
    }
    END
}




















template <class GeoModule> GeoSphere<GeoModule>::GeoSphere()
{
    sectors = ARRAYSIZE(triangle);
    memory = sizeof(*this);
}

template <class GeoModule> GeoSphere<GeoModule>::GeoSphere(const Float&lower, const Float&upper, unsigned depth)
{
    sectors = ARRAYSIZE(triangle);
    memory = sizeof(*this);
    generate(lower,upper,depth);
}


template <class GeoModule> GeoSphere<GeoModule>::GeoSphere(const Data&lower, const Data&upper, unsigned depth)
{
    sectors = ARRAYSIZE(triangle);
    memory = sizeof(*this);
    generate(lower,upper,depth);
}

template <class GeoModule> GeoData<typename GeoModule::FloatType> GeoSphere<GeoModule>::generateData(const Data&lower, const Data&upper)
{
    Float random0 = (Float)rand()/RAND_MAX*2-1,
          random1 = (Float)rand()/RAND_MAX,
          step = (lower.height-upper.height)/2,
          mid = lower.height+step;
    return Data(mid, 0, lower.fertility+random1*(upper.fertility-lower.fertility));
//    return Data(mid,lower.balance+(sqr(random0)*0.5+0.5)*(upper.balance-lower.balance),lower.fertility+random1*(upper.fertility-lower.fertility));
}

template <class GeoModule> void GeoSphere<GeoModule>::generate(const Float&lower, const Float&upper, unsigned depth)
{
    Data    low(lower,0,0.5),
            up(upper,1,0.5);
    generate(low,up,depth);
}


template <class GeoModule> void GeoSphere<GeoModule>::render(GeoModule&module)
{
    for (unsigned i = 0; i < ARRAYSIZE(triangle); i++)
        triangle[i].render(module);
}


template <class GeoModule> void GeoSphere<GeoModule>::generate(const Data&lower, const Data&upper, unsigned depth)
{
    bottom = lower;
    top = upper;
    height_range = (upper.height-lower.height);
    range = 1LL<<depth;
    Float mid = lower.height+height_range/2;


    UINT32 index_map[3*20],
           *offset=index_map;







    #define t(a,b,c)    {_v3(offset,a,b,c); offset+=3;}
    t(0,1,2);
    t(0,2,3);
    t(0,3,4);
    t(0,4,5);
    t(0,5,1);

    t(2,1,6);
    t(3,2,7);
    t(4,3,8);
    t(5,4,9);
    t(1,5,10);
    
    t(2,6,7);
    t(3,7,8);
    t(4,8,9);
    t(5,9,10);
    t(1,10,6);

    t(7,6,11);
    t(8,7,11);
    t(9,8,11);
    t(10,9,11);
    t(6,10,11);
    #undef t
    
    Mesh<TGraphDef<SDef<Float> > >  object;
    _oMakeGraph(object,index_map,20*3);

    _sphereCoords(0,90,1,object.vertex[0].position);
    _sphereCoords(0,30,1,object.vertex[1].position);
    _sphereCoords(-72,30,1,object.vertex[2].position);
    _sphereCoords(-144,30,1,object.vertex[3].position);
    _sphereCoords(-216,30,1,object.vertex[4].position);
    _sphereCoords(-288,30,1,object.vertex[5].position);

    _sphereCoords(-36,-30,1,object.vertex[6].position);
    _sphereCoords(-108,-30,1,object.vertex[7].position);
    _sphereCoords(-180,-30,1,object.vertex[8].position);
    _sphereCoords(-252,-30,1,object.vertex[9].position);
    _sphereCoords(-324,-30,1,object.vertex[10].position);
    
    _sphereCoords(0,-90,1,object.vertex[11].position);

    for (unsigned i = 0; i < object.vertex_cnt; i++)
        object.vertex[i].position[3] = (Float)rand()/RAND_MAX;


/*    for (unsigned i = 0; i < 3; i++)
    {
        for (unsigned j = 0; j < object.edge_cnt; j++)
        {
            Float center[3];
            _center(object.edge[j].vertex[0]->position,object.edge[j].vertex[1]->position,center);
            _scaleAbsolute(center,object.edge[j].vertex[0]->position,distance/2);
            _scaleAbsolute(center,object.edge[j].vertex[1]->position,distance/2);
        }
    }*/


    #define NUM_P 100
    Float   point[NUM_P][3];
    for (unsigned i = 0; i < NUM_P; i++)
    {
        point[i][0] = (Float)rand()/RAND_MAX*2-1;
        point[i][1] = (Float)rand()/RAND_MAX*2-1;
        point[i][2] = (Float)rand()/RAND_MAX*2-1;
        _normalize(point[i]);
    }


    __int64 time = timer.now();

    for (unsigned i = 0; i < object.face_cnt; i++)
    {
        for (BYTE j = 0; j < 4; j++)
            if (triangle[i].child[j])
            {
                delete triangle[i].child[j];
                triangle[i].child[j] = NULL;
            }
        triangle[i].super = this;
        triangle[i].depth = depth;

        for (unsigned j = 0; j < GeoTriangle<GeoModule>::vertices; j++)
        {
            triangle[i].vertex[j].data = generateData(bottom,top);
        }

        triangle[i].setup(object.face[i].vertex[0]->position,object.face[i].vertex[1]->position,object.face[i].vertex[2]->position,time);

        triangle[i].setNeighbours(
            link(triangle+(object.face[i].n0-object.face),object.face[i].opposing[0]),
            link(triangle+(object.face[i].n1-object.face),object.face[i].opposing[1]),
            link(triangle+(object.face[i].n2-object.face),object.face[i].opposing[2]));
    }

    /*
    Vertex&first = triangle[rand()].vertex[rand()*GeoTriangle<GeoModule>::vertices/RAND_MAX];
    Node*primary = new Node(Vertex::range/3,Vertex::range*2/3,triangle[rand()*ARRAYSIZE(triangle)/RAND_MAX);
    primary->target->data.flags = 1;
    float step = height_range/10;
    DynamicPQueue<Node>  queue;
    if (primary->evolve(queue,step))
        delete primary;
    while (Node*n = queue.get())
        if (n->evolve(queue,step))
            delete n;
*/
    for (unsigned i = GeoModule::Size-1; i < GeoModule::Size; i--)
        for (unsigned j = 0; j < 20; j++)
            triangle[j].generateLayer(1<<i);




/*
    triangle[0].copyEdge(0);
    triangle[1].copyEdge(0);
    triangle[2].copyEdge(0);
    triangle[3].copyEdge(0);
    triangle[4].copyEdge(0);

    triangle[5].copyEdge(0);
    triangle[6].copyEdge(0);
    triangle[7].copyEdge(0);
    triangle[8].copyEdge(0);
    triangle[9].copyEdge(0);

    triangle[10].copyEdge(0);
    triangle[10].copyEdge(2);
    triangle[11].copyEdge(0);
    triangle[11].copyEdge(2);
    triangle[12].copyEdge(0);
    triangle[12].copyEdge(2);
    triangle[13].copyEdge(0);
    triangle[13].copyEdge(2);
    triangle[14].copyEdge(0);
    triangle[14].copyEdge(2);

    triangle[15].copyEdge(0);
    triangle[16].copyEdge(0);
    triangle[17].copyEdge(0);
    triangle[18].copyEdge(0);
    triangle[19].copyEdge(0);

    triangle[15].copyEdge(1);
    triangle[16].copyEdge(1);
    triangle[17].copyEdge(1);
    triangle[18].copyEdge(1);
    triangle[19].copyEdge(1);   */

/*    for (unsigned i = 0; i < 20; i++)
        triangle[i].randomSmooth();*/

    for (unsigned i = 0; i < 20; i++)
        triangle[i].fillAllNormals();

    for (unsigned i = 0; i < 20; i++)
        triangle[i].mergeBorderNormals(false);

    for (unsigned i = 0; i < 20; i++)
    {
        triangle[i].mergeCornerNormals(false);
        triangle[i].updateBox();
        GeoModule::build(triangle+i);
    }


}

template <class GeoModule> void GeoSphere<GeoModule>::showMe()
{
    for (BYTE k = 0; k < 8; k++)
    {
        cout << "("<<(int)k<<") ";
        triangle[k].showMe();
        cout << endl<<endl;
    }
}

template <class GeoModule> void GeoSphere<GeoModule>::checkLinkage()
{
    for (BYTE k = 0; k < 8; k++)
        triangle[k].checkLinkage();
}

template <class GeoModule> void GeoSphere<GeoModule>::trace(GeoModule&module, const __int64&time, float ttl)
{
    BEGIN
    for (unsigned i = 0; i < ARRAYSIZE(triangle); i++)
        triangle[i].cast(module,time,ttl);
    for (unsigned i = 0; i < ARRAYSIZE(triangle); i++)
        triangle[i].resolve();
    END
}

template <class GeoModule> void GeoSphere<GeoModule>::checkTimeouts(const __int64&time, float top_level_ttl)
{
    for (unsigned i = 0; i < ARRAYSIZE(triangle); i++)
        triangle[i].checkTimeouts(time,top_level_ttl);
}



/*
template <class GeoModule> unsigned  GeoTriangle<GeoModule>::border[3][range];
template <class GeoModule> unsigned  GeoTriangle<GeoModule>::inner[3][range-1];
*/



#endif
