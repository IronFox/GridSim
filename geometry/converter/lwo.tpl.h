#ifndef accessTplH
#define accessTplH

/******************************************************************

LWO-access-unit.

******************************************************************/




template <class C>
void CLWMaterial::xyz2h(const C&x, const C&y, const C&z, C&out)
{
    if (!x && !z)
        out = 0;
    else
    {
        if (!z)
            out = x < 0?M_PI_2:-M_PI_2;
        else
            out = -vatan2(x,z);
        if (out < 0)
            out += M_2_PI;
    }
}

template <class C>
void CLWMaterial::xyz2lp(const C&x, const C&y, const C&z, C&h, C&p)
{
    if (!x && !z)
    {
        h = 0;
        if (!y)
            p = y < 0?-M_PI_2:M_PI_2;
        else
            p = 0;
    }
    else
    {
        h = -vatan2(x,z);
        if (h < 0)
            h += M_2_PI;
        C  l = vsqrt(x*x + z*z);
        if (!l)
            p = y < 0?-M_PI_2:M_PI_2;
        else
            p = vatan(y/l);
    }
}


template <class C>
C CLWMaterial::fract(const C&x)
{
    if(x>1 || x<0)
    {
        C t = x-floor(x);
        return (t>1?0:t);
    }
    return x;
}


template <class C>
void CLWMaterial::texCoord(const C*position, const C*norm, C*texcoord)
{
    C t,h,l,relative[3],anorm[3];

    _subtract(position,center,relative);
    switch (type)
    {
        case MT_CYLINDRICAL:
        case MT_SPHERICAL:
            switch (axis)
            {
                case 0:
                    xyz2lp(relative[2],relative[0],-relative[1],h,l);
                break;
                case 1:
                    xyz2lp(-relative[0],relative[1],relative[2],h,l);
                break;
                case 2:
                    xyz2lp(-relative[0],relative[2],-relative[1],h,l);
                break;
            }
            t = -relative[axis] / dimension.v[axis] + 0.5;
            while (t < 0)
                t++;

            h = fract((1-h)/M_2_PI * tiles_x);
            l = fract((1-l)/M_PI * tiles_y);

            texcoord[0] = h;
            if (type == MT_CYLINDRICAL)
                texcoord[1] = t;
            else
                texcoord[1] = l;
        break;
        case MT_CUBIC:
        case MT_PLANAR:
            _c3(norm,anorm);
            _positive(anorm);
            axis = _highest(anorm);
            texcoord[0] = (axis?relative[0]/dimension.v[0]:relative[2]/dimension.v[2])+0.5;
            texcoord[1] = -(axis==1?relative[2]/dimension.v[2]:relative[1]/dimension.v[1])+0.5;
        break;
    }
}



#endif
