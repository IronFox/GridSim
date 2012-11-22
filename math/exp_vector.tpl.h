#ifndef exp_vectorTplH
#define exp_vectorTplH

/******************************************************************

Expression-template-vector.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/



namespace VectorMath
{

    template <class C> OpVec<C>::OpVec()
    {}

    template <class C>
    template <class C0, class C1, class C2> OpVec<C>::OpVec(const C0&x_, const C1&y_, const C2&z_):x(x_),y(y_),z(z_)
    {}

    template <class C>
    template <class C0, class C1> OpVec<C>::OpVec(const C0&x_, const C1&y_):x(x_),y(y_),z(0)
    {}

    template <class C> OpVec<C>::OpVec(const C&v_):x(v),y(v),z(v)
    {}

    template <class C> OpVec<C>::OpVec(const OpVec<C>&other)
    {
        _c3(other.v,v);
    }

    template <class C>
    template <class C0> OpVec<C>::OpVec(const OpVec<C0>&other)
    {
        _c3(other.v,v);
    }

    template <class C>
    template <class E, class C1> OpVec<C>::OpVec(const VecExp<E,C1>&exp)
    {
        v[0] = (C)exp[0];
        v[1] = (C)exp[1];
        v[2] = (C)exp[2];
    }

    template <class C>
    template <class C1> inline void OpVec<C>::operator+=(const OpVec<C1>&other)
    {
        _add(v,other.v);
    }

    template <class C> inline void OpVec<C>::operator+=(const C&value)
    {
        _add(v,value);
    }

    template <class C>
    template <class E, class C1> inline void OpVec<C>::operator+=(const VecExp<E,C1>&exp)
    {
        v[0] += exp[0];
        v[1] += exp[1];
        v[2] += exp[2];
    }

    template <class C>
    template <class E, class C1> inline void OpVec<C>::operator=(const VecExp<E,C1>&exp)
    {
        v[0] = (C)exp[0];
        v[1] = (C)exp[1];
        v[2] = (C)exp[2];
    }


    template <class C>
    template <class C1> inline void OpVec<C>::operator-=(const OpVec<C1>&other)
    {
        _subtract(v,other.v);
    }

    template <class C> inline void OpVec<C>::operator-=(const C&value)
    {
        _subtract(v,value);
    }

    template <class C>
    template <class E, class C1> inline void OpVec<C>::operator-=(const VecExp<E,C1>&exp)
    {
        v[0] -= exp[0];
        v[1] -= exp[1];
        v[2] -= exp[2];
    }

    template <class C>
    template <class C1> inline void OpVec<C>::operator*=(const OpVec<C1>&other)
    {
        _multiply(v,other.v);
    }

    template <class C> inline void OpVec<C>::operator*=(const C&value)
    {
        _multiply(v,value);
    }

    template <class C>
    template <class C1> inline void OpVec<C>::operator/=(const OpVec<C1>&other)
    {
        _divide(v,other.v);
    }

    template <class C> inline void OpVec<C>::operator/=(const C&value)
    {
        _divide(v,value);
    }

    template <class C>
    template <class C1> inline void OpVec<C>::operator=(const OpVec<C1>&other)
    {
        _c3(other.v,v);
    }

    template <class C> inline void OpVec<C>::operator=(const C&value)
    {
        _v3(v,value,value,value);
    }

    template <class C> inline  C& OpVec<C>::operator[](unsigned component)
    {
        return  v[component];
    }

    template <class C> inline  void OpVec<C>::normalize()
    {
        _normalize(v);
    }

    template <class C> inline  void OpVec<C>::normalize0()
    {
        _normalize0(v);
    }

    template <class C> inline  OpVec<C> OpVec<C>::normalized()                const
    {
        OpVec<C> result;
        C len = _length(v);
        _divide(v,len,result.v);
        return result;
    }

    template <class C> inline  OpVec<C> OpVec<C>::normalized0()               const
    {
        OpVec<C> result;
        C len = _length(v);
        if (len)
            _divide(v,len,result.v);
        else
            _v3(result.v,1,0,0);
        return result;
    }

    template <class C> inline  C OpVec<C>::length()                    const
    {
        return _length(v);
    }

    template <class C> inline  String OpVec<C>::toString()                  const
    {
        return _toString(v);
    }

}





#endif
