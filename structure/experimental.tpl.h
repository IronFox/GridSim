#ifndef experimentalTplH
#define experimentalTplH



/******************************************************************

Experimental.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/

template <class C> bool  _oMakeStrip(C*field, unsigned len)
{
    if (!len || len%3)
        return false;
    C_Object<S_GraphDef<int> > object;

    _oMakeObject(object,field,len);
    _oMakeStrip(object,field);
    return true;
}


template <class C0, class C1>  bool _oRearange(const C0*v_field, unsigned band, C1*index_field, unsigned len, float tollerance)
{
    if (len%3 || !len)
        return false;
    typedef S_GraphDef<const C0*> Def;

    C_Object<Def> object;
    _oMakeObject(object,index_field,len);
    for (unsigned i = 0; i < object.vertex_cnt; i++)
        object.vertex[i].link = &v_field[object.vertex[i].index*band];
    for (unsigned i = 0; i < object.face_cnt; i++)
    {
        float longest = 0;
        for (BYTE k = 0; k < 3; k++)
        {
            float len = _distance(object.face[i].vertex[(k+1)%3]->link,object.face[i].vertex[k]->link);
            if (len > longest)
            {
                longest = len;
                object.face[i].attrib = k;
            }
        }
    }
    float ideal0[] = {1,1,1},
          ideal1[] = {1,1,-1},
          ideal2[] = {0,0,1};
    _normalize(ideal0);
    _normalize(ideal1);
    for (unsigned i = 0; i < object.edge_cnt; i++)
    {
        S_ObjEdge<Def>&e = object.edge[i];
        S_ObjFace<Def>*f0 = e.face[0],
                      *f1 = e.face[1];
        if (f1 && !f0->marked && !f1->marked)
        {
            unsigned e0 = _oIndexOf(f0,&e),
                     e1 = _oIndexOf(f1,&e),
                     a0  = (e0+2)%3,
                     a1  = (e1+2)%3;
/*            if (f1->attrib != e1 || f0->attrib != e0)
                continue;*/
            float n0[3],n1[3];
            _oTriangleNormal(f0->vertex[0]->link,f0->vertex[1]->link,f0->vertex[2]->link,n0);
            _oTriangleNormal(f1->vertex[0]->link,f1->vertex[1]->link,f1->vertex[2]->link,n1);
            if (_intensity(n0,n1)<1-tollerance)
                continue;

            float d0[3],d1[3],c0[3],c1[3],l;
            _subtract(e.vertex[0]->link, e.vertex[1]->link,d0);
            _subtract(f1->vertex[a1]->link,f0->vertex[a0]->link,d1);
            _center(e.vertex[0]->link, e.vertex[1]->link,c0);
            _center(f1->vertex[a1]->link,f0->vertex[a0]->link,c1);
            l = vmin(_length(d0),_length(d1))/10;
            if (_distance(c0,c1) > l)
                continue;
            f0->marked = true;
            f1->marked = true;



            float n[3],s[9];
            _normalize(d0);
            _normalize(d1);


            if (_dot(d0,ideal0)>_dot(d1,ideal0))
                continue;

            f0->vertex[(a0+2)%3] = f1->vertex[a1];
            f1->vertex[(a1+2)%3] = f0->vertex[a0];

        }
    }

    for (unsigned i = 0; i < object.face_cnt; i++)
        for (BYTE k = 0; k < 3; k++)
            index_field[i*3+k] = object.face[i].vertex[k]->index;
    return true;
}




    template <class C0, class C1, class C2, class C3, unsigned Dim0, unsigned Dim1> inline void        _oResolveNURBSfield(const C0***map, const C1&inner, const C2&outer, C3*out)
    {
        _clear(out);
        for (unsigned out = 0; out < Dim0; out++)
            for (unsigned in = 0; in < Dim1; in++)
                _lineSeek(out,map[out][in],resolveNURBSweight<C2,Dim0>(out,outer)*resolveNURBSweight<C2,Dim1>(in,inner),out);
    }

    template <class C0, class C1, class C2, class C3, class C4, class C5, class C6> inline void        _oResolveNURBSfield(const C0 string0[4][3], const C1 string1[4][3], const C2 string2[4][3], const C3 string3[4][3], const C4&x, const C5&y, C6*out)
    {
        _clear(out);
        for (BYTE k = 0; k < 4; k++)
        {
            C5 str = resolveNURBSweight<C5,4>(k,x);
            _lineSeek(out,string0[k],str*y*y*y,out);
            _lineSeek(out,string1[k],str*3*y*y*(1-y),out);
            _lineSeek(out,string2[k],str*3*y*(1-y)*(1-y),out);
            _lineSeek(out,string3[k],str*(1-y)*(1-y)*(1-y),out);
        }
    }





    template <class C> static void _oPlant(SObjEdge<SGraphDef<C> >*edge, SObjVertex<SGraphDef<C> >*next, unsigned&depth, unsigned recursion=0)
    {
    //    cout << "planting edge "<<edge->vertex[0]->ident<<"-"<<edge->vertex[1]->ident<<endl;
        if (recursion > MAX_RECURSION)
            return;
        if (edge->face[0]->marked)
            return;
        if (edge->face[1] && edge->face[1]->marked)
            return;
        edge->frame = true;
        edge->face[0]->marked = true;
        if (edge->face[1])
            edge->face[1]->marked = true;
        depth++;
        next->marked = true;
        SObjEdge<SGraphDef<C> >*e = next->first;
        while (e)
        {
            //cout << "looping "<<e->vertex[0]->ident<<"-"<<e->vertex[1]->ident<<endl;
    //            ShowMessage(" sub-planting "+IntToStr(e->vertex[0]->ident)+"-"+IntToStr(e->vertex[1]->ident));
            char index = _oIndexOf(e,next);
            if (index == -1)
                FATAL__("linkage broken");
            if (e != edge && !e->frame && !e->vertex[!index]->marked)
                _oPlant(e,e->vertex[!index],depth,recursion+1);
            e = e->next[index];
        }
    }


    template <class C> static void _oIdentifyRing(SObjFace<SGraphDef<C> >*fce, BYTE from, unsigned ring_id, unsigned&len)
    {
    //    cout << "now at "<<fce->vertex[0]->ident<<", "<<fce->vertex[1]->ident<<", "<<fce->vertex[2]->ident<<endl;
        fce->attrib = ring_id;
        len++;
        BYTE next = (from+1)%3;
        if (fce->edge[next]->frame)
            next = (next+1)%3;
        if (fce->n[next] && fce->n[next]->ring != ring_id)
            _oIdentifyRing(fce->n[next],fce->opposing[next],ring_id,len);
    }



    template <class C0, class C1> void  _oMakeStrip(Mesh<SGraphDef<C0> >&source, C1*target)
    {
        typedef SObjVertex<SGraphDef<C0> > Vtx;
        typedef SObjEdge<SGraphDef<C0> >   Edg;
        typedef SObjFace<SGraphDef<C0> >   Fce;

        if (!source.vertex_cnt || !source.edge_cnt || !source.face_cnt)
            return;

        unsigned root_id(0);
        bool found(true);
        while (found)
        {
            Vtx*root = &source.vertex[root_id++];
            root->frame_point = true;
            unsigned depth(0);
            Edg*edg = root->first;
            while (edg)
            {
                char index = _oIndexOf(edg,root);
                if (index == -1)
                    FATAL__("linkage broken");
    //            ShowMessage("planting "+IntToStr(edg->vertex[0]->ident)+"-"+IntToStr(edg->vertex[1]->ident));
                _oPlant(edg,edg->vertex[!index],depth);
    //            ShowMessage("returned");
                edg = edg->next[index];
            }
         //   ShowMessage("tree planted at "+IntToStr(root_id)+" with "+IntToStr(depth)+" leaves");

            while (root_id < source.vertex_cnt && !_oValidRoot(&source.vertex[root_id]))
                root_id++;
            found = root_id < source.vertex_cnt;
        }
    //    cout << "done planting"<<endl;

        unsigned face_id(0),ring_id(0);
        found = true;
        while (found)
        {
            ring_id++;
            Fce*root = &source.face[face_id++];
            root->ring = ring_id;
            unsigned len(1);
            for (BYTE k = 0; k < 3; k++)
                if (!root->edge[k]->frame && root->n[k])
                    _oIdentifyRing(root->n[k],root->opposing[k],ring_id,len);

    //        cout << "ring "<<ring_id<<" identified with "<<len<<"faces"<<endl;
            while (face_id < source.face_cnt && source.face[face_id].ring)
                face_id++;
            found = face_id < source.face_cnt;
        }
    //    cout << "done identifying"<<endl;
    }


template <class C>  bool        _oStrip(const C*in_field, C in_len, C**out_field, C&out_len, C**out_segments, C&out_num_segments, unsigned index_threshold)
{
    out_len = 0;
    out_num_segments = 0;
    (*out_segments) = NULL;
    (*out_field) = NULL;
    if (in_len%3 || !in_len)
        return false;
    C flen = in_len/3;
    typedef C_oStripVertex  T_Vertex;
    typedef C_oStripFace    T_Face;
    unsigned max = 0;
    for (unsigned i = 0; i < in_len; i++)
        max = vmax(max,in_field[i]);
    T_Vertex*vmap = new T_Vertex[max+1];
    T_Face*fmap = new T_Face[flen];



    for (unsigned i = 0; i < in_len; i++)
        fmap[i/3].vertex[i%3] = &vmap[in_field[i]];

    for (unsigned i = 0; i < in_len; i++)
        vmap[in_field[i]].add(&fmap[i/3]);

    for (unsigned i = 0; i < flen; i++)
        for (BYTE k = 0; k < 3; k++)
            if (!fmap[i].neighbour[k])
                fmap[i].setNeighbour(k,vmap[in_field[i*3+k]].findOther(&vmap[in_field[i*3+(k+1)%3]],&fmap[i]));


    C_ArrayReference<const C,256> seg_buffer,out_buffer;
    unsigned current(0),free(0),remaining(flen),runs(0);
    while (remaining)
    {
        BYTE last_neighbours(4);
        char direction(1),last(0),loop(0);
//        ShowMessage("searching from "+IntToStr(free));
        while (fmap[free].done || (fmap[free].marked == remaining && loop++<flen))  //that should find us a triangle to start with
            if (++free >= flen)
            {
                free = 0;
                runs++;
            }
//        ShowMessage("found "+IntToStr(free));

        unsigned current = free;
        if (current >= flen)
            FATAL__("trying to access face "+IntToStr(current)+" of "+IntToStr(flen));
        fmap[current].marked = remaining;
        if (fmap[current].done)
            FATAL__("error rooted");
        bool searching(!fmap[current].openNeighbours()<=1);
        while (searching)
        {
            if (current >= flen)
                FATAL__("trying to access face "+IntToStr(current)+" of "+IntToStr(flen));
            if (fmap[current].canGo((last+direction)%3,remaining))
            {
                T_Face*rs = fmap[current].go((last+direction)%3,last);
                current = rs-fmap;
                if (rs < fmap || rs >= &fmap[flen])
                    FATAL__("invalid result");
                direction = direction%2+1;
                fmap[current].marked = remaining;
                continue;
            }
            searching = false; //gotcha
        }


        unsigned cnt = 3,larger,lesser;
        bool left(true);
        char dir = fmap[current].wayOut(larger,lesser);
        if ((larger<5) && runs<=2)
        {
            free = current+1;
            if (free >= flen)
            {
                free = 0;
                runs++;
            }
            continue;
        }
        for (BYTE k = 0; k < 3; k++)
            out_buffer.appendElement(&in_field[current*3+(k+dir+2)%3]);
        if (fmap[current].done)
            FATAL__("having a done-problem here");
        fmap[current].done = true;
            if (remaining)
                remaining--;
            else
                FATAL__("funny/strange error");
        char lonely = -1;
        while (!searching)
        {
            char not_dir;
            if (dir == -1)
            {
/*                if (not_dir != -1 && lonely != -1)
                {
                      out_buffer.appendElement(&in_field[current*3+lonely]);
                  //    if (!left)
                      {
//                        out_buffer.appendElement(&in_field[current*3+lonely]);
  //                      cnt++;
//                        left = !left;
                        }
                    out_buffer.appendElement(&in_field[current*3+(lonely+1+left)%3]);
    //              left = !left;
                    T_Face*other = fmap[current].neighbour[not_dir];
                    char back = fmap[current].opposing[not_dir];
                  left = !left;
                    lonely = (back+2)%3;
                    dir = (back+left+1)%3;
                    if (left)
                        not_dir = (dir+2)%3;
                    else
                        not_dir = (dir+1)%3;
                    if (!other->neighbour[not_dir] || other->neighbour[not_dir]->done)
                        not_dir = -1;
                    if (!other->neighbour[dir] || other->neighbour[dir]->done)
                        dir = -1;
                    current = other-fmap;
                    other->done = true;
                    out_buffer.appendElement(&in_field[current*3+lonely]);
                    cnt+=3;
//                    left = !left;
                    remaining--;
                }
                else*/
                    searching = true;         //end of line
                continue;
            }
            current = fmap[current].walk(left,dir,not_dir,lonely)-fmap;
            if (not_dir!=-1)
            {
                T_Face*f = fmap[(BYTE)current].neighbour[(BYTE)not_dir];
                BYTE n = f->openNeighbours();
                if (n <= last_neighbours)
                {
                    free = f-fmap;
                    last_neighbours = n;
                }
            }
            if (current >= flen)
                FATAL__("invalid current");
            out_buffer.appendElement(&in_field[current*3+lonely]);
            left = !left;
            if (fmap[current].done)
                FATAL__("invalid path");
            fmap[current].done = true;
            if (remaining)
                remaining--;
            else
                FATAL__("funny/strange error");
            cnt++;

        }
        seg_buffer.appendElement((C*)cnt);
    }

//    cout << "finished vertices: "<<out_buffer.count()<<" segments: "<<seg_buffer.count()<<endl;
    delete[] fmap;
    delete[] vmap;

    out_num_segments = 1;
    out_len = 0;
    C raw=0;
    seg_buffer.reset();
    while (const C*el = seg_buffer.each())
    {
        bool final_seg = ((C)el) >= index_threshold;
        out_num_segments+= final_seg;
        if (!final_seg)
            raw+= (((C)el)-2)*3;
        else
            out_len += (C)el;
    }
    out_len+=raw;
    (*out_segments) = new C[out_num_segments];
    (*out_segments)[0] = raw;
    unsigned at(1);

    seg_buffer.reset();
    while (const C*el = seg_buffer.each())
    {
        C val = (C)el;
        if (val >= index_threshold)
            (*out_segments)[at++] = val;
    }
    (*out_field) = new C[out_len];

    unsigned offset(0);
    at = 0;
    seg_buffer.reset();
    while (const C*el = seg_buffer.each())
    {
        C val = (C)el;
        if (val < index_threshold)
        {
            (*out_field)[offset++] = *out_buffer[at++];
            (*out_field)[offset++] = *out_buffer[at++];
            (*out_field)[offset++] = *out_buffer[at++];
            for (unsigned i = 3; i < val; i++)
            {
                if (!(i % 2))
                {
                    (*out_field)[offset++] = *out_buffer[at-2];
                    (*out_field)[offset++] = *out_buffer[at-1];
                }
                else
                {
                    (*out_field)[offset++] = *out_buffer[at-1];
                    (*out_field)[offset++] = *out_buffer[at-2];
                }
                (*out_field)[offset++] = *out_buffer[at++];
            }
        }
        else
            at+=val;
    }
    at = 0;
    seg_buffer.reset();
    while (const C*el = seg_buffer.each())
    {
        C val = (C)el;
        if (val >= index_threshold)
        {
            for (unsigned i = 0; i < val; i++)
                (*out_field)[offset++] = *out_buffer[at++];
        }
        else
            at+=val;
    }
    if (offset != out_len)
        FATAL__("bad num_out");
    return true;
}






/*

template <class C>  void            _oCopy(const C*o, unsigned o_band, C*destination, unsigned cnt, unsigned band)
{
    for (unsigned i = 0; i < cnt; i++)
        _copy(&o[i*o_band],&destination[i*band],band);
}

template <class C>  void            _oTriangleToMatrix(const C*p1, const C*p2, const C*p3)
{
    C*buf = (C*)GLOBAL_MATRIX_BUFFER;
    _subtract(p2,p1,buf,3);
    _subtract((p3),(p1),&buf[3],3);
}


template <class C>  void            _oInvert(C*o, unsigned cnt, unsigned band)
{
    _oMultiply(o,cnt,band,-1,0,3);
}

template <class C>  void            _oNormalizeTriangle(const C*triangle, unsigned band, C*out)
{
    //I have absolutly no idea what this method was supposed to do but it seems, it was never needed or ever defined.
    //looooooooooooooooooool
}


template <class C>  void            _oDefineDimension(const C* o, unsigned cnt, unsigned band)
{
    bool defined[6]={false,false,false,false,false,false};
    C*dimension = (C*)GLOBAL_DIMENSION;
    OBJECT_LOOP_EACH(cnt,band)
    {
        for (unsigned i = 0; i < 3; i++)
        {
            if (!defined[i] || o[el+i]<dimension[i])
            {
                defined[i] = true;
                dimension[i] = o[el+i];
            };
            if (!defined[i+3] || o[el+i]>dimension[i+3])
            {
                defined[i+3] = true;
                dimension[i+3] = o[el+i];
            };
        };
    };
    _subtract(&dimension[3],dimension);

    //this is for higher range
    _multiply(&dimension[3],(C)MAP_OVERSIZE,(C*)GLOBAL_BUFFER);
    _subtract(dimension,(C*)GLOBAL_BUFFER);
    _multiply((C*)GLOBAL_BUFFER,2.0);
    _add(&dimension[3],(C*)GLOBAL_BUFFER);
}

template <class C>  void            _oStoreDimension(T_IndexBuffer b)
{
    double*v = (double*)&b[1];
    _copy((C*)GLOBAL_D_DIMENSION,v,6);
}

template <class C>  void            _oRestoreDimension(T_IndexBuffer b)
{
    double*v = (double*)&b[1];
    _copy(v,(C*)GLOBAL_D_DIMENSION,6);
}

template <class C>  void            _oRestoreDimension(T_IndexBuffer b);



template <class C>  void            _oSectorsOfVertex  (const C*v, T_IndexBuffer out, unsigned&cnt)
{
    C* dimension = (C*)GLOBAL_DIMENSION;
    _subtract(v,dimension,(C*)SECTOR_SET);               //translate point into dimension field
    _divide((C*)SECTOR_SET,&dimension[3]);    //point to factor (0..1)
    _multiply((C*)SECTOR_SET,(C)MAP_DETAIL);
    VECTOR_LOOP(3)
         SECTOR_ID[el] = ((C*)SECTOR_SET)[el];
    _oCheckAppendSector(out,cnt);
}

template <class C>  void            _oSectorsOfEdge    (const C*v, const C*w, T_IndexBuffer out, unsigned&cnt)
{
    C* dimension = (C*)GLOBAL_DIMENSION;
    C* sector_set = (C*)SECTOR_SET;
    _subtract(v,dimension,sector_set);                     //translation
    _divide(sector_set,&dimension[3]);          //to factors
    _multiply(sector_set,(C)MAP_DETAIL);                  //to sector (sector of v now in SECTOR_SET 0 to 2)
    _subtract(w,dimension,&sector_set[3]);                 //tr
    _divide(&sector_set[3],&dimension[3]);  //scl
    _multiply(&sector_set[3],(C)MAP_DETAIL);          //to sector (sector of w not in SECTOR_D_SET 3 to 5)
    _vectorSort(sector_set,&sector_set[3]);                       //use sectors like dimensions
    _subtract(&sector_set[3],sector_set,&sector_set[6]);          //get line_vector
    for (unsigned char row = 0; row < 3; row++)
        if ((int)sector_set[row] != (int)sector_set[3+row])            //if step found
        {
            for (unsigned i = (unsigned)sector_set[row]+1; i <= (unsigned)sector_set[3+row]; i++)  //then draw a line
            {
                double f = ((double)i-sector_set[row])/sector_set[6+row];
                for (unsigned char r2 = 0; r2 < 3; r2++)
                    SECTOR_ID[r2]=sector_set[r2]+f*sector_set[6+r2];
                _oCheckAppendSector(out,cnt);
            };
        }
    for (unsigned char i = 0; i < 3; i++)
        SECTOR_ID[i] = sector_set[i];
    _oCheckAppendSector(out,cnt);
}

template <class C>  void            _oSectorsOfTriangle(const C*o, unsigned band, T_IndexBuffer out, unsigned&cnt)
{
    C* v = (C*)GLOBAL_BUFFER,
       w = &((C*)GLOBAL_BUFFER)[3],
       dimensiont = C*GLOBAL_DIMENSION;

    VECTOR_LOOP(3)
    {
        _oSectorsOfEdge(&o[el*band],&o[((el+1)%3)*band],out,cnt);
        _oSectorsOfVertex(&o[el*band],out,cnt);
    };

    _subtract(&o[2*band],o,v,3);
    _subtract(&o[band],o,w,3);
    for (unsigned char direction = 0; direction < 3; direction++)
    {
        for (unsigned char d1 =1; d1 < MAP_DETAIL; d1++)
            for (unsigned char d2 =1; d2 < MAP_DETAIL; d2++)
            {
                unsigned char   x = !direction,
                                y = (direction < 2)+1;
                if (x==y) ShowMessage("bad");
                if (x==direction || y==direction) ShowMessage("even worse");
                C   fx = dimension[x]+((C)d1)/(C)MAP_DETAIL*dimension[x+3],
                    fy = dimension[y]+((C)d2)/(C)MAP_DETAIL*dimension[y+3],
                    beta_u = (fy*v[x]-fx*v[y])-(o[y]*v[x]-o[x]*v[y]),
                    beta_l = (w[y]*v[x]-w[x]*v[y]),
                    beta = beta_l?beta_u/beta_l:10,
                    alpha_u = (fy*w[x]-fx*w[y])-(o[y]*w[x]-o[x]*w[y]),
                    alpha_l = (v[y]*w[x]-v[x]*w[y]),
                    alpha = alpha_l?alpha_u/alpha_l:10;
                if (alpha > -GLOBAL_ERROR && beta > -GLOBAL_ERROR && alpha+beta < 1+GLOBAL_ERROR)
                {
                    C h = ((o[direction]+v[direction]*alpha+w[direction]*beta)-dimension[direction])/dimension[direction+3];
                    if (h >= 0 && h <= 1)
                        for (unsigned char field_x = d1-1; field_x <= d1; field_x++)
                            for (unsigned char field_y = d2-1; field_y <=d2; field_y++)
                            {
                                SECTOR_ID[x] = field_x;
                                SECTOR_ID[y] = field_y;
                                SECTOR_ID[direction] = h*10;
                                _oCheckAppendSector(out,cnt);
                            };
                };
            };
    };
}

template <class C>  unsigned        _oCreateObjectMap  (const C*o, unsigned cnt, unsigned band)
{
    bool inverted = _oInverted(o,cnt,band);
    _oDefineDimension(o,cnt,band);
    _oStoreDimension(GLOBAL_INDEX_BUFFER);
    GLOBAL_INDEX_BUFFER[7] = inverted;
    unsigned addr = cnt/3+8,c=8;
    C p[3];
    OBJECT_LOOP(cnt,band)
    {
           GLOBAL_INDEX_BUFFER[c] = addr;
           unsigned ct = 0;
           _oSectorsOfTriangle(&o[el],band,&GLOBAL_INDEX_BUFFER[addr],ct);
           addr+=ct+1;
           GLOBAL_INDEX_BUFFER[addr-1] = 0;
           c++;
    };
    GLOBAL_INDEX_BUFFER[0] = addr;  //final size
    addr++;
    unsigned faddr = addr+MAP_DETAIL*MAP_DETAIL*MAP_DETAIL;
    C*dimension = (C*)GLOBAL_DIMENSION;
    for (unsigned id = 0; id < MAP_DETAIL*MAP_DETAIL*MAP_DETAIL; id++)
    {
        GLOBAL_INDEX_BUFFER[addr+id] = faddr;
        _v3(p,((C)(id/(MAP_DETAIL*MAP_DETAIL))/MAP_DETAIL+0.5/MAP_DETAIL)*GLOBAL_D_DIMENSION[3]+GLOBAL_D_DIMENSION[0],((C)(id%(MAP_DETAIL*MAP_DETAIL)/MAP_DETAIL)+0.5)/(C)MAP_DETAIL*dimension[4]+dimension[1],((C)(id%(MAP_DETAIL*MAP_DETAIL)%MAP_DETAIL)+0.5)/(C)MAP_DETAIL*dimension[5]+dimension[2]);
        GLOBAL_INDEX_BUFFER[faddr++] = _oInside(p,o,cnt,band,NULL,inverted);
        unsigned tri = 0,
                 pos = (cnt/3)+7;
        while (tri < cnt/3)
        {
              unsigned i = GLOBAL_INDEX_BUFFER[pos++];
              if (!i) tri++;
              else
                  if (i==id) GLOBAL_INDEX_BUFFER[faddr++] = tri+1;
        };
        GLOBAL_INDEX_BUFFER[faddr++] = 0;
    };
    GLOBAL_INDEX_BUFFER[addr-1] = faddr-addr+1;
    return faddr;
}



template <class C>  unsigned        _oAnd(const C*o, unsigned o_cnt, unsigned o_band, T_SectorMap o_map, const C*p, unsigned p_cnt, unsigned p_band, T_SectorMap p_map, unsigned band)
{
    GLOBAL_VERTEX_CNT = 0;
    unsigned v_1 = _oCut(o,o_cnt,o_band,p,p_cnt,p_band,band),
             v_2 = _oCut(p,p_cnt,p_band,o,o_cnt,o_band,band),
             v_3 = _oDecission(&((C*)GLOBAL_OBJECT_BUFFER)[v_1*band],v_2,band,o,o_cnt,o_band,band,KICK_OUTSIDE,o_map);
    _oDecission((C*)GLOBAL_OBJECT_BUFFER,v_1,band,p,p_cnt,p_band,band,KICK_OUTSIDE,p_map);
    _copy(&((C*)GLOBAL_OBJECT_BUFFER)[(v_1+v_2)*band],(C*)GLOBAL_OBJECT_BUFFER,(GLOBAL_VERTEX_CNT-v_2-v_1)*band);
    GLOBAL_VERTEX_SPLIT = v_3;
    GLOBAL_VERTEX_CNT = GLOBAL_VERTEX_CNT-v_2-v_1;
    return GLOBAL_VERTEX_CNT;
}

template <class C>  unsigned        _oOr(const C*o, unsigned o_cnt, unsigned o_band, T_SectorMap o_map, const C*p, unsigned p_cnt, unsigned p_band, T_SectorMap p_map, unsigned band)
{
    GLOBAL_VERTEX_CNT = 0;
    unsigned v_1 = _oCut(o,o_cnt,o_band,p,p_cnt,p_band,band),
             v_2 = _oCut(p,p_cnt,p_band,o,o_cnt,o_band,band),
             v_3 = _oDecission((C*)GLOBAL_OBJECT_BUFFER,v_1,band,p,p_cnt,p_band,band,KICK_INSIDE,p_map);
    _oDecission(&((C*)GLOBAL_OBJECT_BUFFER)[v_1*band],v_2,band,o,o_cnt,o_band,band,KICK_INSIDE,o_map);
    _copy(&((C*)GLOBAL_OBJECT_BUFFER)[(v_1+v_2)*band],(C*)GLOBAL_OBJECT_BUFFER,(GLOBAL_VERTEX_CNT-v_2-v_1)*band);
    GLOBAL_VERTEX_SPLIT = v_3;
    GLOBAL_VERTEX_CNT = GLOBAL_VERTEX_CNT-v_2-v_1;
    return GLOBAL_VERTEX_CNT;
}

template <class C>  bool            _oInside(const C*v, const C*o, unsigned cnt, unsigned band, T_SectorMap map, bool inverted)
{
    if (map)
    {
        _oRestoreDimension<C>(map);
        unsigned sec,cnt=0;
        _oSectorsOfVertex(v,&sec,cnt);
        if (!cnt) return false; //vertex is not inside the map dimensions.
        if (cnt!=1) ShowMessage("inside-sector definition returns invalid value"); //just for checking purpose
       //so we should be having the correct sector id in "sec" now...
       / ok, I have a new plan again:
         if we use the axis-method we could speed things up by using the following method:
         This process will be repeated until
              a) we get one or more hits with the axis and can calculate in-outside-state
              b) we reach an empty sector and return the precalculated information without doing cuts (luckiest and fastest case)
              c) we reach the end of the present sector-z-row without a) or b) to take place. This neccessarily means, that an object-hole was found and the result is false

         This method should be quite fast...
         *sick* I didn't even check the mapping process yet
       *
       unsigned steps = MAP_DETAIL-sec%(MAP_DETAIL*MAP_DETAIL)%MAP_DETAIL;
       for (unsigned i = 0; i < steps; i++)
       {
          T_IndexBuffer sector = _oGetSector(map,sec+i);
          unsigned nearest = -1,p=1;
          double dist = 0;
          if (!sector[1])
          {
            GLOBAL_DIST_CHECK = 10*i;
    //        ShowMessage("escaped in "+IntToStr(sec+i));
            return sector[0];  //lucky case
          };
          while (sector[p])
          {
               C*triangle = &o[(sector[p]-1)*band*3];
               C*a=&((C*)GLOBAL_BUFFER)[3],b=&((C*)GLOBAL_BUFFER)[6];
               _subtract(&triangle[2*band],triangle,a,3);
               _subtract(&triangle[band],triangle,b,3);

               C         beta_u = (v[1]*a[0]-v[0]*a[1])-(triangle[1]*a[0]-triangle[0]*a[1]),
                         beta_l = (b[1]*a[0]-b[0]*a[1]),
                         alpha_u =(v[1]*b[0]-v[0]*b[1])-(triangle[1]*b[0]-triangle[0]*b[1]),
                         alpha_l =(a[1]*b[0]-a[0]*b[1]);
               if (beta_l && alpha_l)
               {
                  C alpha = alpha_u/alpha_l,
                  beta  = beta_u/beta_l;
                  if (alpha > -GLOBAL_ERROR && beta > -GLOBAL_ERROR && alpha+beta < 1+GLOBAL_ERROR)
                  {
                     C z = triangle[2]+a[2]*alpha+b[2]*beta-v[2];
                     if (z > 0 && (z < dist || nearest == -1))
                     {
                        dist = z;
                        nearest = sector[p];
                     };
                  };
               };
               p++;
          };
          if (nearest == -1)
          {
      //      ShowMessage("failed... "+IntToStr(sec+i));
            continue;  //hopefully more luck with the next sector
          };
    //      ShowMessage("succeeded in "+IntToStr(sec+i));

          C* triangle = &o[nearest*3*band];
          _subtract(&triangle[band],triangle,(C*)GLOBAL_BUFFER);
          _subtract(&triangle[2*band],triangle,&(C*)GLOBAL_BUFFER)[3]);
          _cross((C*)GLOBAL_BUFFER,&((C*)GLOBAL_BUFFER)[3],&((C*)GLOBAL_BUFFER)[6]);
          _v3(&((C*)GLOBAL_MATRIX_BUFFER)[6],0,0,1);
          return (_dotProduct(&((C*)GLOBAL_BUFFER)[6],&((C*)GLOBAL_MATRIX_BUFFER[6])/(_length(&((C*)GLOBAL_BUFFER)[6])))>0;
       };
       return false; //after all the map is not broken for there is a possible case
    };

    //ok, theoretically this should work. Now for non-mapped objects:
    /I'll try something new:

    if instead of using a random defined vector I use one parallel to a system axis. I might be able to use the 2d-cut-method I used earlier.
    This should take away the invertation processing delay and speed up things dramatically.
    Ok, it works so far.
    Such a simple technique... and it took me this long to figure it out.
    *



    C dist = 0;
    unsigned nearest = -1;
    OBJECT_LOOP(cnt,band)
    {
       C*triangle = &o[el];
       C*a=&((C*)GLOBAL_BUFFER)[3],b=&((C*)GLOBAL_BUFFER)[6];
       _subtract(&triangle[2*band],triangle,a,3);
       _subtract(&triangle[band],triangle,b,3);

       C     beta_u = (v[1]*a[0]-v[0]*a[1])-(triangle[1]*a[0]-triangle[0]*a[1]),
             beta_l = (b[1]*a[0]-b[0]*a[1]),
             alpha_u =(v[1]*b[0]-v[0]*b[1])-(triangle[1]*b[0]-triangle[0]*b[1]),
             alpha_l =(a[1]*b[0]-a[0]*b[1]);
       if (!beta_l || !alpha_l) continue;
       C     alpha = alpha_u/alpha_l,
             beta  = beta_u/beta_l;
       if (alpha < -GLOBAL_ERROR || beta < -GLOBAL_ERROR || alpha+beta > 1+GLOBAL_ERROR) continue;
       C z = triangle[2]+a[2]*alpha+b[2]*beta-v[2];
       if (z < 0) continue;  //we don't need to check in both directions
       if (z < dist || nearest == -1)
       {
    //      ShowMessage("cutting with "+doubleStr(alpha,false)+", "+doubleStr(beta,false)+" at z="+doubleStr(z,true)+" triangle is "+__undefToString(triangle,3,band));
          dist = z;
          nearest = el;
       };
    };


    if (nearest == -1)
    {
      return inverted; //no cut
    };
    //   ShowMessage("found: nearest is "+IntToStr(nearest));



    C*triangle = &o[nearest];  //this nearest is different to the above used (no index but addr)
    _subtract(&triangle[band],triangle,(C*)GLOBAL_BUFFER);
    _subtract(&triangle[2*band],triangle,&((C*)GLOBAL_BUFFER)[3]);
    _cross((C*)GLOBAL_BUFFER,&((C*)GLOBAL_BUFFER)[3],&((C*)GLOBAL_BUFFER)[6]);
    _v3(&((C*)GLOBAL_MATRIX_BUFFER)[6],0,0,1);
    bool rs = ((_dotProduct(&((C*)GLOBAL_BUFFER)[6],&((C*)GLOBAL_MATRIX_BUFFER)[6])/(_length(&((C*)GLOBAL_BUFFER)[6])))>0);
    //if (inverted) rs = !rs;
    return rs;
}

template <class C>  unsigned        _oCut(const C*butter, unsigned butter_cnt,unsigned butter_band, const C*knife,unsigned knife_cnt,unsigned knife_band, unsigned band)
{
    unsigned lower_limit = GLOBAL_VERTEX_CNT;
    _oCopy(butter,butter_band,&((C*)GLOBAL_OBJECT_BUFFER)[GLOBAL_VERTEX_CNT*band],butter_cnt,band);
    GLOBAL_VERTEX_CNT+=butter_cnt;
    /
    OBJECT_LOOP(knife_cnt,knife_band)
            _oSlice(&knife[el],&knife[el+band],&knife[el+2*band],lower_limit,band);
    OBJECT_LOOP(knife_cnt,knife_band)
            _oSlice(&knife[el],&knife[el+band],&knife[el+2*band],lower_limit,band);
    */
    /*
    despite my serious effords in making the method "clean" it still produces the following:
    a) triangles remain uncut even though their edges clearly pass cutting triangles
    b) triangles are erased even though they are in the valid area (possibly because of a)
    c) vertex-data is slightly corrupted (seen once: resulting normal not calculated correctly while vertex-position was correct)

    possible solution 1:
    It may be that non-cut edges result out of noninvertable matrices that should be invertable.
    I will try to swap the triangle-sides to reduce this phenomena
    *
    OBJECT_LOOP(knife_cnt,knife_band)
    {
            _oStab(&knife[el],&knife[el+band],lower_limit,band);
            _oStab(&knife[el],&knife[el+2*band],lower_limit,band);
            _oStab(&knife[el+2*band],&knife[el+band],lower_limit,band);
    };
    OBJECT_LOOP(knife_cnt,knife_band)
            _oSlice(&knife[el],&knife[el+band],&knife[el+2*band],lower_limit,band);
//    OBJECT_LOOP(knife_cnt,knife_band)
//            _oSlice(&knife[el],&knife[el+band],&knife[el+2*band],lower_limit,band);
    return GLOBAL_VERTEX_CNT-lower_limit;
}

template <class C>  unsigned        _oDecission(const C*o,unsigned o_cnt,unsigned o_band, const C*decission_space,unsigned d_cnt,unsigned d_band, unsigned band, unsigned char rule,T_SectorMap map)
{
    unsigned start = GLOBAL_VERTEX_CNT;
    C p[3];
    bool inverted=map?map[7]:_oInverted(decission_space,d_cnt,d_band);
    OBJECT_LOOP(o_cnt,o_band)
    {
        _add(&o[el],&o[el+band],&o[el+2*band],(C*)GLOBAL_BUFFER);
        _multiply((C*)GLOBAL_BUFFER,1.0/3,p);
        bool inside = _oInside(p,decission_space,d_cnt,d_band,map,inverted);
        if (_oValidTriangle(&o[el],band) && (rule != KICK_ALLWAYS&&(rule == KICK_NEVER || (rule == KICK_INSIDE && !inside) || (rule == KICK_OUTSIDE && inside))))
        {
            _oCopy(&o[el],o_band,&((C*)GLOBAL_OBJECT_BUFFER)[GLOBAL_VERTEX_CNT*band],3,band);
            GLOBAL_VERTEX_CNT+=3;
        };
    };
    return GLOBAL_VERTEX_CNT-start;
}

template <class C>  C_Object<C>     _oCube(C width, C height, C depth)
{
    C_Object<C> result(36,6);
    C*data = result.getVertices();

    data[0] = width/2;
    data[1] = height/2;
    data[2] = -depth/2;
    data[3] = 1;
    data[4] = 0;
    data[5] = 0;
    data[6] = width/2;   //right_side
    data[7] = height/2;
    data[8] = depth/2;
    data[9] = 1;
    data[10] = 0;
    data[11] = 0;
    data[12] = width/2;
    data[13] = -height/2;
    data[14] = -depth/2;
    data[15] = 1;
    data[16] = 0;
    data[17] = 0;
    data[18] = width/2;
    data[19] = height/2;
    data[20] = depth/2;
    data[21] = 1;
    data[22] = 0;
    data[23] = 0;
    data[24] = width/2;
    data[25] = -height/2;
    data[26] = depth/2;
    data[27] = 1;
    data[28] = 0;
    data[29] = 0;
    data[30] = width/2;
    data[31] = -height/2;
    data[32] = -depth/2;
    data[33] = 1;
    data[34] = 0;
    data[35] = 0;

    data[36] = -width/2;
    data[37] = -height/2;
    data[38] = -depth/2;
    data[39] = -1;
    data[40] = 0;
    data[41] = 0;
    data[42] = -width/2;       //left_side
    data[43] = height/2;
    data[44] = depth/2;
    data[45] = -1;
    data[46] = 0;
    data[47] = 0;
    data[48] = -width/2;
    data[49] = height/2;
    data[50] = -depth/2;
    data[51] = -1;
    data[52] = 0;
    data[53] = 0;
    data[54] = -width/2;
    data[55] = height/2;
    data[56] = depth/2;
    data[57] = -1;
    data[58] = 0;
    data[59] = 0;
    data[60] = -width/2;
    data[61] = -height/2;
    data[62] = -depth/2;
    data[63] = -1;
    data[64] = 0;
    data[65] = 0;
    data[66] = -width/2;
    data[67] = -height/2;
    data[68] = depth/2;
    data[69] = -1;
    data[70] = 0;
    data[71] = 0;

    data[72] = -width/2;
    data[73] = -height/2;
    data[74] = depth/2;
    data[75] = 0;
    data[76] = 0;
    data[77] = 1;
    data[78] = width/2;   //front_side
    data[79] = height/2;
    data[80] = depth/2;
    data[81] = 0;
    data[82] = 0;
    data[83] = 1;
    data[84] = -width/2;
    data[85] = height/2;
    data[86] = depth/2;
    data[87] = 0;
    data[88] = 0;
    data[89] = 1;
    data[90] = width/2;
    data[91] = height/2;
    data[92] = depth/2;
    data[93] = 0;
    data[94] = 0;
    data[95] = 1;
    data[96] = -width/2;
    data[97] = -height/2;
    data[98] = depth/2;
    data[99] = 0;
    data[100] = 0;
    data[101] = 1;
    data[102] = width/2;
    data[103] = -height/2;
    data[104] = depth/2;
    data[105] = 0;
    data[106] = 0;
    data[107] = 1;

    data[108] = -width/2;       //back_side
    data[109] = height/2;
    data[110] = -depth/2;
    data[111] = 0;
    data[112] = 0;
    data[113] = -1;
    data[114] = width/2;
    data[115] = height/2;
    data[116] = -depth/2;
    data[117] = 0;
    data[118] = 0;
    data[119] = -1;
    data[120] = width/2;
    data[121] = -height/2;
    data[122] = -depth/2;
    data[123] = 0;
    data[124] = 0;
    data[125] = -1;
    data[126] = -width/2;
    data[127] = height/2;
    data[128] = -depth/2;
    data[129] = 0;
    data[130] = 0;
    data[131] = -1;
    data[132] = width/2;
    data[133] = -height/2;
    data[134] = -depth/2;
    data[135] = 0;
    data[136] = 0;
    data[137] = -1;
    data[138] = -width/2;
    data[139] = -height/2;
    data[140] = -depth/2;
    data[141] = 0;
    data[142] = 0;
    data[143] = -1;


    data[144] = -width/2;
    data[145] = height/2;
    data[146] = depth/2;
    data[147] = 0;
    data[148] = 1;
    data[149] = 0;
    data[150] = width/2;   //top_side
    data[151] = height/2;
    data[152] = depth/2;
    data[153] = 0;
    data[154] = 1;
    data[155] = 0;
    data[156] = -width/2;
    data[157] = height/2;
    data[158] = -depth/2;
    data[159] = 0;
    data[160] = 1;
    data[161] = 0;
    data[162] = width/2;
    data[163] = height/2;
    data[164] = depth/2;
    data[165] = 0;
    data[166] = 1;
    data[167] = 0;
    data[168] = width/2;
    data[169] = height/2;
    data[170] = -depth/2;
    data[171] = 0;
    data[172] = 1;
    data[173] = 0;
    data[174] = -width/2;
    data[175] = height/2;
    data[176] = -depth/2;
    data[177] = 0;
    data[178] = 1;
    data[179] = 0;

    data[180] = -width/2;
    data[181] = -height/2;
    data[182] = -depth/2;
    data[183] = 0;
    data[184] = -1;
    data[185] = 0;
    data[186] = width/2;   //bottom_side
    data[187] = -height/2;
    data[188] = depth/2;
    data[189] = 0;
    data[190] = -1;
    data[191] = 0;
    data[192] = -width/2;
    data[193] = -height/2;
    data[194] = depth/2;
    data[195] = 0;
    data[196] = -1;
    data[197] = 0;
    data[198] = width/2;
    data[199] = -height/2;
    data[200] = depth/2;
    data[201] = 0;
    data[202] = -1;
    data[203] = 0;
    data[204] = -width/2;
    data[205] = -height/2;
    data[206] = -depth/2;
    data[207] = 0;
    data[208] = -1;
    data[209] = 0;
    data[210] = width/2;
    data[211] = -height/2;
    data[212] = -depth/2;
    data[213] = 0;
    data[214] = -1;
    data[215] = 0;
    return result;
}*/

/*
template <class C>  void            _oAdd(C*o, unsigned o_cnt, unsigned o_band, C*vector, unsigned channel_start, unsigned el_count)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
       for (unsigned channel = channel_start; channel < channel_start+el_count && channel < o_band; channel++)
           o[el+channel] +=v[channel-channel_start];
}

template <class C>  void            _oAdd(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
           o[el+channel]+=value;
}

template <class C>  void            _oAdd(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel_start, unsigned band)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        for (unsigned channel = channel_start; channel < channel_start+band && channel < o_band; channel++)
            o[el+channel]+=value;
}


template <class C>  void            _oSubtract(C*o, unsigned o_cnt, unsigned o_band, C*vector, unsigned channel_start, unsigned el_count)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        for (unsigned channel = channel_start; channel < channel_start+el_count && channel < o_band; channel++)
            o[el+channel] -=v[channel-channel_start];
}

template <class C>  void            _oSubtract(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        o[el+channel]-=value;
}

template <class C>  void            _oSubtract(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel_start, unsigned band)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        for (unsigned channel = channel_start; channel < channel_start+band && channel < o_band; channel++)
            o[el+channel]-=value;
}


template <class C>  void            _oMultiply(C*o, unsigned o_cnt, unsigned o_band, C*vector, unsigned channel_start, unsigned el_count)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        for (unsigned channel = channel_start; channel < channel_start+el_count && channel < o_band; channel++)
            o[el+channel] *=v[channel-channel_start];
}

template <class C>  void            _oMultiply(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        o[el+channel]*=value;
}

template <class C>  void            _oMultiply(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel_start, unsigned band)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        for (unsigned channel = channel_start; channel < channel_start+band && channel < o_band; channel++)
            o[el+channel]*=value;
}

template <class C>  void            _oDivide(C*o, unsigned o_cnt, unsigned o_band, C*vector, unsigned channel_start, unsigned el_count)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        for (unsigned channel = channel_start; channel < channel_start+el_count && channel < o_band; channel++)
            o[el+channel] /=v[channel-channel_start];
}

template <class C>  void            _oDivide(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        o[el+channel]/=value;
}

template <class C>  void            _oDivide(C*o, unsigned o_cnt, unsigned o_band, C value, unsigned channel_start, unsigned band)
{
    OBJECT_LOOP_EACH(o_cnt,o_band)
        for (unsigned channel = channel_start; channel < channel_start+band && channel < o_band; channel++)
            o[el+channel]/=value;
}





template <class C>  bool            _oValidTriangle(C*o, unsigned band)
{
    for (unsigned char i = 0; i < 3; i++)
    {
        unsigned char j = (i+1)%3;
        if (sqr(o[j*band]-o[i*band])+sqr(o[j*band+1]-o[i*band+1])+sqr(o[j*band+2]-o[i*band+2]) < sqr((C)MIN_EDGE_SIZE)) return false;
    }
    _subtract(&o[band],o,(C*)SECTOR_SET); //using this to avoid crosspassing with the main-buffers
    _subtract(&o[2*band],o,&((C*)SECTOR_SET)[3]);
    return (_dotProduct((C*)SECTOR_SET,&((C*)SECTOR_SET)[3])/(_length((C*)SECTOR_SET)*_length(&((C*)SECTOR_SET)[3])))<1;
}

template <class C>  bool            _oValidTriangle(C*v1, C*v2, C*v3)
{
    if (sqr(v2[0]-v1[0])+sqr(v2[1]-v1[1])+sqr(v2[2]-v1[2]) < (C)MIN_EDGE_SIZE) return false;
    if (sqr(v3[0]-v1[0])+sqr(v3[1]-v1[1])+sqr(v3[2]-v1[2]) < (C)MIN_EDGE_SIZE) return false;
    if (sqr(v3[0]-v2[0])+sqr(v3[1]-v2[1])+sqr(v3[2]-v2[2]) < (C)MIN_EDGE_SIZE) return false;
    return true;
}


template <class C>  void            _oCutPoint(const C*n, const C*p, const C*d, const C*b, C*out)
{
    C buffer[3];
    _subtract(p,b,buffer); //dif
    C alpha = _dotProduct(n,buffer)/_dot(n,d);
    _multiply(d,alpha,out);
    _add(out,b);
}

template <class C>  void            _oCutPoint(const C*n, const C*p, const C*d, const C*b, C*out, unsigned el_count)
{
    C*buffer=(C*)GLOBAL_BUFFER;
    _subtract(p,b,buffer,el_count); //dif
    C alpha = _dotProduct(n,buffer,el_count)/_dot(n,d,el_count);
    _multiply(d,alpha,out,el_count);
    _add(out,b,el_count);
}


/
template <class C>  unsigned               _oIntersectTriangle(C*in_out,unsigned ext_band,unsigned level)
{
//    ShowMessage(__toString(in_out,3,12+ext_band));
    unsigned step = 1;
    unsigned base = 0,stride = 12+ext_band,tri_stride=stride*3;
    for (unsigned i = 0; i < level; i++)
    {
        for (unsigned j = 0; j < step; j++)
            _oIntersectTriangle(&in_out[base+j*tri_stride],&in_out[base+step*tri_stride+j*(4*tri_stride)],stride);
        base+=step*tri_stride;
        step*=4;
    };
    return base+step*tri_stride;
}*

template <class C>  unsigned        _oIntersectTriangle(C*in_out,unsigned band, unsigned level)
{
    unsigned step = 1;
    unsigned base = 0,stride = band>13?band:13,tri_stride=stride*3;
    for (unsigned i = 0; i < level; i++)
    {
        for (unsigned j = 0; j < step; j++)
            _oIntersectTriangle(&in_out[base+j*tri_stride],&in_out[base+step*tri_stride+j*(4*tri_stride)],stride,i);
        base+=step*tri_stride;
        step*=4;
    };
    return base+step*tri_stride;
}

template <class C>  void               _oIntersectTriangle(const C*triangle, C*out, unsigned stride, unsigned level)
{
    //again: format:
    //position[3] left_smoothing_normal[3] right_smoothing_normal[3] smoothing_limit normalized_normal[3] data[stride-10]
    //if smoothing-limit is set to -1 no smoothing_limit
//    ShowMessage(__toString(triangle,3,stride));
    C normal_out[9],position_out[9];
    for (BYTE i = 0; i < 3; i++)  //first let's get the intersected points and normals
    {
        BYTE n = (i+1)%3;
        if (triangle[i*stride+9]>level || triangle[i*stride+9]<0)
            _oIntersect(&triangle[i*stride],&triangle[i*stride+3],&triangle[n*stride],&triangle[i*stride+6],(C)0.5,&position_out[i*3],&normal_out[i*3]);
        else
        {
//            ShowMessage("got exception");
            _add(&triangle[i*stride],&triangle[n*stride],&position_out[3*i]);
            _divide(&position_out[i*3],(C)2);
            _add(&triangle[i*stride+3],&triangle[i*stride+6],&normal_out[3*i]);
            _normalize(&normal_out[i*3]);
        }
    }
    for (BYTE i = 0; i < 3; i++)  //calculate the outer triangles
    {
        /
        put i
        put cut i
        put cut i+2
        *
        BYTE j = (i+1)%3,
             k = (i+2)%3;
        _copy(&triangle[i*stride],&out[i*stride*3],stride);
        _c3(&normal_out[i*3],&out[i*stride*3+6]);
        out[i*stride*3+9] = triangle[i*stride+9];


        _c3(&position_out[i*3],&out[i*stride*3+stride]);
        if (triangle[i*stride+9]<0)
            _c3(&normal_out[i*3],&out[i*stride*3+stride+3]);
        else
        {
            _add(&triangle[i*stride+10],&triangle[j*stride+10],&out[i*stride*3+stride+3]);
            _normalize(&out[i*stride*3+stride+3]);
//            ShowMessage("defining middle to "+_toString(&out[i*stride*3+stride+3]));
        }
        if (triangle[k*stride+9]<0)
            _c3(&normal_out[k*3],&out[i*stride*3+stride+6]);
        else
        {
            _add(&triangle[k*stride+10],&triangle[i*stride+10],&out[i*stride*3+stride+6]);
            _normalize(&out[i*stride*3+stride+6]);
//            ShowMessage("defining second middle to "+_toString(&out[i*stride*3+stride+6]));
        }
        out[i*stride*3+stride+9] = -1;
        _add(&triangle[i*stride+10],&triangle[j*stride+10],&out[i*stride*3+stride+10],stride-10);
        _normalize(&out[i*stride*3+stride+10]);
        _divide(&out[i*stride*3+stride+13],(C)2,stride-13);


        _c3(&position_out[k*3],&out[i*stride*3+2*stride]);
        _c3(&normal_out[k*3],&out[i*stride*3+2*stride+3]);
        _c3(&triangle[k*stride+6],&out[i*stride*3+2*stride+6]);
        out[i*stride*3+2*stride+9] = triangle[k*stride+9];
        _add(&triangle[k*stride+10],&triangle[i*stride+10],&out[i*stride*3+stride*2+10],stride-10);
        _normalize(&out[i*stride*3+stride*2+10]);
        _divide(&out[i*stride*3+stride*2+13],(C)2,stride-13);
    }

    for (BYTE i = 0; i < 3; i++) //and the inner one
    {
        _c3(&position_out[i*3],&out[9*stride+i*stride]);
        BYTE prev = (i+2)%3,
             next = (i+1)%3;
        if (triangle[i*stride+9]<0)
        {
            _c3(&normal_out[i*3],&out[9*stride+i*stride+3]);
            _c3(&normal_out[i*3],&out[9*stride+prev*stride+6]);
        }
        else
        {
            _add(&triangle[i*stride+10],&triangle[next*stride+10],&out[9*stride+i*stride+3]);
            _normalize(&out[9*stride+i*stride+3]);
            _c3(&out[9*stride+i*stride+3],&out[9*stride+prev*stride+6]);
        }
        _add(&triangle[i*stride+10],&triangle[next*stride+10],&out[9*stride+i*stride+10],stride-10);
        _normalize(&out[9*stride+i*stride+10]);
        _divide(&out[9*stride+i*stride+13],(C)2,stride-13);
        out[9*stride+i*stride+9] = -1;
    }
//    ShowMessage("out:\n"+__toString(out,12,stride));
}

template <class C>  void            _oIntersectionCenter(const C*p, const C*d, const C*n, const C*dif, C*out)
{
    C vd[3],tmp[3];
    _cross(dif,d,tmp);
    if (_length(tmp)<GLOBAL_ERROR)
        _v3(tmp,positive(d[1])+1,d[2],d[0]);
    _cross(tmp,d,vd);
    C sub_alpha = _dotProduct(n,vd);
    C alpha = positive(sub_alpha)>GLOBAL_D_ERROR?-_dotProduct(n,p)/sub_alpha:-10000000;
    _multiply(vd,alpha,out);
    _add(out,p);
//    ShowMessage("in: "+_toString(p)+" and "+_toString(d)+" (dif "+_toString(dif)+") resulting in vd="+_toString(vd)+" out: "+_toString(out));
}

template <class C>  void            _oIntersect(const C*p1, const C*n1, const C*p2, const C*n2, C offset, C*position_out, C*normal_out)
{
    bool nurb_mode; //to determine wether use "normal" nurb-mode or sphere-mode
    C dif0[3],dif1[3];
    _subtract(p2,p1,dif0);
    _subtract(p1,p2,dif1);
//    ShowMessage("examining: "+_toString(p1)+"->"+_toString(n1)+" and "+_toString(p2)+"->"+_toString(n2));
    if (abs(_dot(n1,n2)) < GLOBAL_D_ERROR)
        nurb_mode = false;
    else
    {
        C a0 = _dot(n2,dif0)/_dot(n2,n1),
          a1 = _dot(n1,dif1)/_dot(n1,n2);
        //ShowMessage("factors: "+FloatToStr(a0)+", "+FloatToStr(a1));
        nurb_mode = a0 <= GLOBAL_D_ERROR || a1 <= GLOBAL_D_ERROR;
    }
    nurb_mode = true; //circle-mode has some problems so just leave it for the time being

    if (!nurb_mode)
    {
        //ShowMessage("in");
        C s0[3],s1[3],c[3],d0[3],d1[3],dist0,dist1,alpha,m[3];
        _cross(n1,n2,c);
        _cross(n1,c,d0);
        _cross(n2,c,d1);
        _oCutPoint(n1,p1,d1,p2,s1);
        _oCutPoint(n2,p2,d0,p1,s0);
        dist0 = _distance(s0,p1);
        dist1 = _distance(s1,p2);
        alpha = dist0/(dist0+dist1);
        _subtract(s1,s0,m);
        _multiply(m,alpha);
        _add(m,s0);
        //ShowMessage(_toString(m));
        _subtract(m,p1,d0);
        _subtract(p2,m,d1);
        dist1 = 1-cos(offset*M_PI/2);
        dist0 = sin(offset*M_PI/2);
        _multiply(d0,dist0);
        _multiply(d1,dist1);
        _add(d0,d1,position_out);
        _add(position_out,p1);
        dist1 = 1-cos(offset*M_PI/2)*2;
        dist0 = sin(offset*M_PI/2)*2;
        _multiply(d0,dist0);
        _multiply(d1,dist1);
        _add(d0,d1,normal_out);
        _add(normal_out,p1);
        _subtract(normal_out,position_out);
        _normalize(normal_out);
        //ShowMessage("bow out: p: "+_toString(position_out)+" n: "+_toString(normal_out)+" with normal_2d: "+FloatToStr(dist0)+", "+FloatToStr(dist1));
        return;
    }
    C dir[3],d0[3],d1[3],m[5][3];
    _subtract(p2,p1,dir);
    _cross(dir,n1,dif0);
    if (_zero(dif0))
        _cross(n2,n1,dif0);
    _cross(n1,dif0,d0);
    _cross(dir,n2,dif1);
    if (_zero(dif1))
        _cross(n2,n1,dif1);
    _cross(dif1,n2,d1);
    C len = _length(dir)/3;
    _setLen(d0,len);
    _setLen(d1,len);
    _add(d0,p1);
    _add(d1,p2); //nurb-points not in d0 and d1
    //ShowMessage("nurbs: "+_toString(d0)+" + "+_toString(d1));
    _rangeSeek(p1,d0,offset,m[0]);
    _rangeSeek(d0,d1,offset,m[1]);
    _rangeSeek(d1,p2,offset,m[2]);
    _rangeSeek(m[0],m[1],offset,m[3]);
    _rangeSeek(m[1],m[2],offset,m[4]);
    _rangeSeek(m[3],m[4],offset,position_out);//ok, so much for the position but how to get the normal???

    _add(p1,n1,dif0);
    _add(p2,n2,dif1);
    _add(d0,n1);
    _add(d1,n2);
    _rangeSeek(dif0,d0,offset,m[0]);
    _rangeSeek(d0,d1,offset,m[1]);
    _rangeSeek(d1,dif1,offset,m[2]);
    _rangeSeek(m[0],m[1],offset,m[3]);
    _rangeSeek(m[1],m[2],offset,m[4]);
    _rangeSeek(m[3],m[4],offset,normal_out);
    _subtract(normal_out,position_out);
    _normalize(normal_out); //that should do the trick!
}


template <class C>  C               _oIntersect(const C*p1, const C*d1, const C*p2, const C*d2, C*position_out, C*direction_out, unsigned band)
{
    C   m[16],x[16],fp1[16],fp2[16];
    _subtract(p1,p2,x,band);  //d2 is not negative:
    _add(d1,d2,m,band);
    C fc = (0.5*_dot(m,x,band)+_length(x,band))/(1-_dot(d1,d2,band));
    _multiply(d1,fc,fp1,band);
    _add(fp1,p1,band);
    _multiply(d2,-fc,fp2,band);
    _add(fp2,p2,band);
    _add(fp1,fp2,position_out,band);
    _multiply(position_out,(C)0.5,band);
    _subtract(fp2,fp1,direction_out,band);
    _multiply(direction_out,(C)1/_length(direction_out,band),band);
    return fc;
}


template <class C>  unsigned      _oCreateIntersectedPath(const C*p1, const C*d1, const C*p2, const C*d2, C*path_out, unsigned level, unsigned stride)
{
    if (stride < 6) stride = 6;
    unsigned step = 1<<(level+1),
             max = step+1;
    _copy(p1,path_out,3);
    _copy(d1,&path_out[3],3);
    _copy(p2,&path_out[step*stride],3);
    _copy(d2,&path_out[step*stride+3],3);
    for (unsigned i = 0; i < level+1; i++)
    {
        for (unsigned j = step/2; j < max; j+=step)
            _oIntersect(&path_out[(j-step/2)*stride],&path_out[(j-step/2)*stride+3],&path_out[(j+step/2)*stride],&path_out[(j+step/2)*stride+3],&path_out[j*stride],&path_out[j*stride+3],stride);
        step/=2;
    }
    return max;
}

template <class C>  unsigned      _oCreateIntersectedPath(const C*p1, const C*d1, const C*p2, const C*d2, C*path_out, unsigned level, unsigned stride, unsigned band)
{
    if (stride < 2*band) stride = 2*band;
    unsigned step = us_exp(2,level+1),
             max = step+1;
    _copy(p1,path_out,band);
    _copy(d1,&path_out[band],band);
    _copy(p2,&path_out[step*stride],band);
    _copy(d2,&path_out[step*stride+band],band);
    for (unsigned i = 0; i < level+1; i++)
    {
        for (unsigned j = step/2; j < max; j+=step)
            _oIntersect(&path_out[(j-step/2)*stride],&path_out[(j-step/2)*stride+band],&path_out[(j+step/2)*stride],&path_out[(j+step/2)*stride+band],&path_out[j*stride],&path_out[j*stride+band],band);
        step/=2;
    }
    return max;
}


/internal usage*

template <class C>  void            _oSliceTriangleEdge(C*p1, C*p2, C*p3,C*triangle, unsigned band, unsigned char edge)
{
    unsigned char next = (edge+1)%3,
                  final = (edge+2)%3;
    C*mbuffer=(C*)GLOBAL_MATRIX_BUFFER,
      buffer=(C*)GLOBAL_BUFFER,
      obuffer=(C*)GLOBAL_OBJECT_BUFFER;
    _subtract(&triangle[next*band],&triangle[edge*band],&mbuffer[6],3);  //z in global_matrix 6..8

    if (!__invertGaussSelect(mbuffer,&mbuffer[27],3))
    {
      //non invertable mut not necessarily mean non cutable
      _oTriangleToMatrix(p2,p3,p1);
      if (!__invertGaussSelect(mbuffer,&mbuffer[27],3))
      {
         _oTriangleToMatrix(p3,p1,p2);
         if (!__invertGaussSelect(mbuffer,&mbuffer[27],3,mbuffer,GLOBAL_D_ERROR))
         {
           _oTriangleToMatrix(p1,p2,p3);
           return; //can't do anything
         }
         else
         {
           _subtract(&triangle[edge*band],p3,&buffer[3]);   //b-a in global 0..2
         };
      }
      else
      {
        _subtract(&triangle[edge*band],p2,&buffer[3]);   //b-a in global 0..2
      };
      _oTriangleToMatrix(p1,p2,p3);
    }
    else
    {
      _subtract(&triangle[edge*band],p1,&buffer[3]);   //b-a in global 0..2
    };
    __multiply(&mbuffer[27],&buffer[3],buffer,1,3);         //final factors in global 0..2  (must remember to tread gamma inverted (global 2))

    if (buffer[0] >= -GLOBAL_ERROR && buffer[1] >= -GLOBAL_ERROR && buffer[2] <= GLOBAL_ERROR && buffer[0]+buffer[1] <= 1+GLOBAL_ERROR && buffer[2]>=-1-GLOBAL_ERROR)
    //if (GLOBAL_D_BUFFER[0] >= 0 && GLOBAL_D_BUFFER[1] >= 0 && GLOBAL_D_BUFFER[2] <= 0 && GLOBAL_D_BUFFER[0]+GLOBAL_D_BUFFER[1] <= 1 && GLOBAL_D_BUFFER[2]>=-1)
    //if (_validCut(p1,p2,p3,&triangle[edge*band],&triangle[next*band],GLOBAL_D_BUFFER,GLOBAL_MATRIX_D_BUFFER,&GLOBAL_MATRIX_D_BUFFER[3]))  //global-matrix-buffer should be defined
    {
           buffer[2]*=-1;  //invertation-problem solved
           _multiply(&triangle[edge*band],1-buffer[2],&buffer[3],band);
           _multiply(&triangle[next*band],buffer[2],&buffer[3+band],band);
           _add(&buffer[3],&buffer[3+band],&buffer[3+2*band],band);   //final new vertex in global 3+2*band...2+3*band
           _copy(&buffer[3+2*band],&GLOBAL_OBJECT_D_BUFFER[GLOBAL_VERTEX_CNT*band],band);
           _copy(&triangle[next*band],&obuffer[GLOBAL_VERTEX_CNT*band+band],band);   //creating new triangle
           _copy(&triangle[final*band],&obuffer[GLOBAL_VERTEX_CNT*band+2*band],band);
           if (!_oValidTriangle(&obuffer[GLOBAL_VERTEX_CNT*band],band)|| !_oValidTriangle(&triangle[edge*band],&triangle[final*band],&GLOBAL_D_BUFFER[3+2*band])) return;
           _copy(&buffer[3+2*band],&triangle[next*band],band);
           GLOBAL_VERTEX_CNT+=3;
    };
}

template <class C>  void            _oSlice(C*p1, C*p2, C*p3, unsigned lower_limit, unsigned band)
{
    C*mbuffer=(C*)GLOBAL_MATRIX_BUFFER,
      buffer=(C*)GLOBAL_BUFFER,
      obuffer=(C*)GLOBAL_OBJECT_BUFFER;
    unsigned upper_limit =GLOBAL_VERTEX_CNT;
    /
    _v3(GLOBAL_MATRIX_D_BUFFER,-GLOBAL_ERROR,-GLOBAL_ERROR,-GLOBAL_ERROR);
    _v2(&GLOBAL_MATRIX_D_BUFFER[3],1+GLOBAL_ERROR,1+GLOBAL_ERROR);
    *


    _oTriangleToMatrix(p1,p2,p3);

    for (unsigned el = lower_limit*band; el < upper_limit*band; el+=band*3)
    //for (unsigned el = lower_limit*band; el < GLOBAL_VERTEX_CNT*band && GLOBAL_VERTEX_CNT*band < CURRENT_BUFFER_SIZE; el+=band*3)
    {
       bool cut = false;
       for (unsigned char cell = 0; cell < 3 && !cut; cell++)
       {
         unsigned char c1 = cell,
                       c2 = (cell+1)%3,
                       c3 = (cell+2)%3;
         _subtract(&obuffer[el+c2*band],&obuffer[el+c1*band],&mbuffer[6],3);  //z in global_matrix 6..8

         if (!__invertGaussSelect(mbuffer,&mbuffer[27],3,GLOBAL_D_ERROR))
         {
            //non invertable mut not necessarily mean non cutable
            _oTriangleToMatrix(p2,p3,p1);
            if (!__invertGaussSelect(mbuffer,&mbuffer[27],3,GLOBAL_D_ERROR))
            {
               _oTriangleToMatrix(p3,p1,p2);
               if (!__invertGaussSelect(mbuffer,&mbuffer[27],3,GLOBAL_D_ERROR))
               {
                 _oTriangleToMatrix(p1,p2,p3);
                 continue; //can't do anything
               }
               else
               {
                 _subtract(&obuffer[el+c1*band],p3,&buffer[3]);   //b-a in global 0..2
               };
            }
            else
            {
              _subtract(&obuffer[el+c1*band],p2,&buffer[3],3);   //b-a in global 0..2
            };
            _oTriangleToMatrix(p1,p2,p3);
         }
         else
         {
             _subtract(&obuffer[el+c1*band],p1,&buffer[3],3);   //b-a in global 0..2
         };

         __multiply(&mbuffer[27],&buffer[3],buffer,1,3);         //final factors in global 0..2  (must remember to tread gamma inverted (global 2))

         if (buffer[0] >= -GLOBAL_ERROR && buffer[1] >= -GLOBAL_ERROR && buffer[2] <= GLOBAL_ERROR && buffer[0]+buffer[1] <= 1+GLOBAL_ERROR && buffer[2]>=-1-GLOBAL_ERROR)
    //     if (GLOBAL_D_BUFFER[0] >= 0 && GLOBAL_D_BUFFER[1] >= 0 && GLOBAL_D_BUFFER[2] <= 0 && GLOBAL_D_BUFFER[0]+GLOBAL_D_BUFFER[1] <= 1 && GLOBAL_D_BUFFER[2]>=-1)
    //     if (_validCut(p1,p2,p3,&GLOBAL_OBJECT_D_BUFFER[el+c1*band],&GLOBAL_OBJECT_D_BUFFER[el+c2*band],GLOBAL_D_BUFFER,GLOBAL_MATRIX_D_BUFFER,&GLOBAL_MATRIX_D_BUFFER[3]))
         {
    //       ShowMessage(__undefToString(GLOBAL_MATRIX_D_BUFFER,3,3));
           buffer[2]*=-1;  //invertation-problem solved
           _multiply(&obuffer[el+c1*band],1-buffer[2],&buffer[3],band);
           _multiply(&obuffer[el+c2*band],buffer[2],&buffer[3+band],band);
           _add(&buffer[3],&buffer[3+band],&buffer[3+2*band],band);   //final new vertex in global 3+2*band...2+3*band
           _copy(&buffer[3+2*band],&obuffer[GLOBAL_VERTEX_CNT*band],band);
           _copy(&obuffer[el+c2*band],&obuffer[GLOBAL_VERTEX_CNT*band+band],band);   //creating new triangle
           _copy(&obuffer[el+c3*band],&obuffer[GLOBAL_VERTEX_CNT*band+2*band],band);
           if (!_oValidTriangle(&obuffer[GLOBAL_VERTEX_CNT*band],band)|| !_oValidTriangle(&obuffer[el+c1*band],&obuffer[el+c3*band],&buffer[3+2*band])) continue;
           _copy(&buffer[3+2*band],&GLOBAL_OBJECT_D_BUFFER[el+c2*band],band);
           GLOBAL_VERTEX_CNT+=3;
           _oSliceTriangleEdge(p1,p2,p3,&obuffer[(GLOBAL_VERTEX_CNT-3)*band],band,1);
           _oSliceTriangleEdge(p1,p2,p3,&obuffer[el],band,c3);
           cut = true;
         }
       };
    };  //hmm.... might this work?
}

template <class C>  void            _oStab(C*start, C*end, unsigned lower_limit, unsigned band)
{
    //form: Alpha*x+Beta*y+a=Gamma*z+b

    C*mbuffer=(C*)GLOBAL_MATRIX_BUFFER,
      buffer=(C*)GLOBAL_BUFFER,
      obuffer=(C*)GLOBAL_OBJECT_BUFFER;
    _subtract(end,start,&mbuffer[6],3);   //z in global_matrix 6..8
    unsigned upper_limit = GLOBAL_VERTEX_CNT;     //preventing endlessloop
    for (unsigned el = lower_limit*band; el < upper_limit*band; el+=band*3)
    {
      _subtract(start,&obuffer[el],&buffer[3],3);  //b-a in global 3..5
      _subtract(&obuffer[el+band],&obuffer[el],mbuffer,3); //x in global_matrix 0..2
      _subtract(&obuffer[el+2*band],&obuffer[el],&mbuffer[3],3); //y in global_matrix 3..5
      if (!__invertGaussSelect(mbuffer,&mbuffer[27],3,INVERT_D_BUFFER,GLOBAL_D_ERROR)) continue;                //inverted matrix in global_matrix 27..35
      __multiply(&mbuffer[27],3,&buffer[3],3,buffer,1,3);         //final factors in global 0..2  (must remember to tread gamma inverted (global 2))
      if (buffer[0] > GLOBAL_ERROR && buffer[1] > GLOBAL_ERROR && buffer[2] < GLOBAL_ERROR && buffer[0]+buffer[1] < 1-GLOBAL_ERROR && buffer[2]>-1)
      {
        _multiply(&obuffer[el],(1-(buffer[0]+buffer[1])),&buffer[3],band);
        _multiply(&obuffer[el+band],buffer[0],&buffer[3+band],band);
        _multiply(&obuffer[el+2*band],buffer[1],&buffer[3+2*band],band);
        _add(&buffer[3],&buffer[3+band],&buffer[3+2*band],&buffer[3+3*band],band);   //final new vertex in global 3+3*band...2+4*band
        if (!_oValidTriangle(&obuffer[el],&obuffer[el+band],&buffer[3+3*band])
            ||
            !_oValidTriangle(&obuffer[el+band],&obuffer[el+band+2],&buffer[3+3*band])
            ||
            !_oValidTriangle(&obuffer[el],&obuffer[el+band+2],&buffer[3+3*band])
           ) continue;
        _copy(&obuffer[el],&obuffer[GLOBAL_VERTEX_CNT*band],band);   //creating new triangle a
        _copy(&obuffer[el+band],&obuffer[GLOBAL_VERTEX_CNT*band+band],band);
        _copy(&buffer[3+3*band],&obuffer[GLOBAL_VERTEX_CNT*band+2*band],band);
        GLOBAL_VERTEX_CNT+=3;
        _copy(&obuffer[el+band],&obuffer[GLOBAL_VERTEX_CNT*band],band);  //creating new triangle b
        _copy(&obuffer[el+2*band],&obuffer[GLOBAL_VERTEX_CNT*band+band],band);
        _copy(&buffer[3+3*band],&obuffer[GLOBAL_VERTEX_CNT*band+2*band],band);
        GLOBAL_VERTEX_CNT+=3;
        _copy(&buffer[3+3*band],&obuffer[el+band],band);           //updating original triangle - three new triangles where two were actually added plus one updated
      };
      //should work...
    };
}

*/





#endif
