#ifndef exp_vectorH
#define exp_vectorH

/******************************************************************

Expression-template-vector.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


namespace VectorMath
{
    /** vector-class using expression-templates **
    
        this is an experimental vector-class using expression-templates to evaluate vector-expressions.
        the class is stable and operational but though intended to it is _not_ faster than ordinary vector-classes.
        
        since I'm using asm-style vector-functions most of the time, i don't care that much about this vector-class

        Author: Stefan Elsen @2005
    */

    template <class> class OpVec;
    template <class, class> class ScalarExp;
    template <class, class> class VecExp;
    template <class, class, class, class> class VecOp;
    template <class, class, class, class> class VecCross;

    class OpAdd;
    class OpSub;
    class OpMult;
    class OpDiv;



    #define Operation(name,operation) class name {public:template<class C0, class C1> static inline C0 eval(const C0&val0, const C1&val1){return val0 operation val1;}}
    Operation(OpAdd,+);
    Operation(OpSub,-);
    Operation(OpMult,*);
    Operation(OpDiv,/);
    #undef Operation


    template <class E, class C>  class   ScalarExp
    {
    public:
            E       expression_;
                    ScalarExp(const E&expression):expression_(expression)
                    {}

    inline  C       operator*() const
                    {
                        return *expression_;
                    }

    inline  C       operator[](unsigned index)    const
                    {
                        return *expression_;
                    }

    inline  void    operator++()
                    {}
    };

    template <class E, class C> class VecExp
    {
    public:
            E           expression_;

                        VecExp(const E&expression):expression_(expression)
                        {}

            C           operator*()                 const
                        {
                            return *expression_;
                        }

    inline  C           operator[](unsigned index)   const
                        {
                            return expression_[index];

                        }
            void        operator++()
                        {
                            expression_++;
                        }
    inline  C           length()
                        {
                            C   v[3] = {expression_[0],expression_[1],expression_[2]};
                            return _length(v);
                        }
            OpVec<C> normalized()
                        {
                            OpVec<C> result;
                            C   len = length();
                            result[0] = expression_[0]/len;
                            result[1] = expression_[1]/len;
                            result[2] = expression_[2]/len;
                            return result;
                        }
            OpVec<C> normalized0()
                        {
                            OpVec<C> result;
                            C   len = length();
                            if (len)
                            {
                                result[0] = expression_[0]/len;
                                result[1] = expression_[1]/len;
                                result[2] = expression_[2]/len;
                            }
                            else
                                _v3(result,1,0,0);
                            return result;
                        }
    };

    template <class E0, class E1, class C, class Operation> class VecOp
    {
    public:
            E0      expression0_;
            E1      expression1_;
                    VecOp(const E0&expression0, const E1&expression1):expression0_(expression0),expression1_(expression1)
                    {}

    inline  C       operator*()                     const
                    {
                        return Operation::eval(*expression0_,*expression1_);
                    }
    inline  C       operator[](unsigned index)      const
                    {
                        return Operation::eval(expression0_[index],expression1_[index]);
                    }

            void    operator++()
                    {
                        expression0_++;
                        expression1_++;
                    }
    };

    template <class E0, class E1, class C, class Dummy> class VecCross
    {
    public:
            E0      expression0_;
            E1      expression1_;
                    VecCross(const E0&expression0, const E1&expression1):expression0_(expression0),expression1_(expression1)
                    {}

    inline  C       operator[](unsigned index)      const
                    {
                        return expression0_[(index+1)%3]*expression1_[(index+2)%3] - expression0_[(index+2)%3]*expression1_[(index+1)%3];
                    }
    };



    template <class E0, class E1, class C0, class C1>
        C0
            operator*(const VecExp<E0,C0>&exp0, const VecExp<E1,C1>&exp1)
            {
                return exp0[0]*exp1[0] + exp0[1]*exp1[1] + exp0[2]*exp1[2];
            }


    template <class E0, class C0, class C1>
        C0
            operator*(const VecExp<E0,C0>&exp, const OpVec<C1>&vec)
            {
                return exp[0]*vec.x + exp[1]*vec.y + exp[2]*vec.z;
            }

    template <class E, class C0, class C1>
        C0
            operator*(const OpVec<C0>&vec, const VecExp<E,C1>&exp)
            {
                return exp[0]*vec.x + exp[1]*vec.y + exp[2]*vec.z;
            }

    template <class C0, class C1>
        C0
            operator*(const OpVec<C0>&vec0, const OpVec<C1>&vec1)
            {
                return _dot(vec0.v,vec1.v);
            }

    #define TplVec                          template <class C>
    #define TplStrict                       template <class E, class C>
    #define TplVecUnary                     template <class E, class C0, class C1>
    #define TplVecBinary                    template <class C0, class C1>

    #define LeftVec                         VecExp<const C0*,C0>
    #define RightVec                        VecExp<const C1*,C1>

    #define OpScalarLeft(op,sign)            TplStrict\
                                                VecExp<VecOp<ScalarExp<const C*,C>,VecExp<E,C>,C,op>,C>\
                                                    operator sign (const C&v, const VecExp<E,C>&exp)\
                                                    {\
                                                        typedef VecOp<ScalarExp<const C*,C>,VecExp<E,C>,C,op>   ResultType;\
                                                        return VecExp<ResultType,C>(ResultType(ScalarExp<const C*,C>(&v),exp));\
                                                    }

    #define OpScalarRight(op,sign)          TplStrict\
                                                VecExp<VecOp<VecExp<E,C>,ScalarExp<const C*,C>,C,op>,C>\
                                                    operator sign (const VecExp<E,C>&exp, const C&v)\
                                                    {\
                                                        typedef VecOp<VecExp<E,C>,ScalarExp<const C*,C>,C,op>   ResultType;\
                                                        return VecExp<ResultType,C>(ResultType(exp,ScalarExp<const C*,C>(&v)));\
                                                    }

    #define OpBinary(op,sign,volume)        template <class E0, class E1, class C0, class C1>\
                                                VecExp<volume<VecExp<E0,C0>,VecExp<E1,C1>,C0,op>,C0>\
                                                    operator sign (const VecExp<E0,C0>&exp0, const VecExp<E1,C1>&exp1)\
                                                    {\
                                                        typedef volume<VecExp<E0,C0>,VecExp<E1,C1>,C0,op>    ResultType;\
                                                        return VecExp<ResultType,C0>(ResultType(exp0,exp1));\
                                                    }

    #define OpVecLeft(op,sign,volume)       TplVecUnary\
                                                VecExp<volume<LeftVec,VecExp<E,C1>,C0,op>,C0>\
                                                    operator sign (const OpVec<C0>&vec, const VecExp<E,C1>&exp)\
                                                    {\
                                                        typedef volume<LeftVec,VecExp<E,C1>,C0,op> ResultType;\
                                                        return VecExp<ResultType,C0>(ResultType(LeftVec(vec.v),exp));\
                                                    }

    #define OpVecRight(op,sign,volume)      TplVecUnary\
                                                VecExp<volume<VecExp<E,C0>,RightVec,C0,op>,C0>\
                                                    operator sign (const VecExp<E,C0>&exp, const OpVec<C1>&vec)\
                                                    {\
                                                        typedef volume<VecExp<E,C1>,RightVec,C0,op> ResultType;\
                                                        return VecExp<ResultType,C0>(ResultType(exp,RightVec(vec.v)));\
                                                    }

    #define OpVecBinary(op,sign,volume)     TplVecBinary\
                                                VecExp<volume<VecExp<const C0*,C0>,VecExp<const C1*,C1>,C0,op>,C0>\
                                                    operator sign (const OpVec<C0>&vec0, const OpVec<C1>&vec1)\
                                                    {\
                                                        typedef volume<VecExp<const C0*,C0>,VecExp<const C1*,C1>,C0,op>   ResultType;\
                                                        return VecExp<ResultType,C0>(ResultType(VecExp<const C0*,C0>(vec0.v),VecExp<const C1*,C1>(vec1.v)));\
                                                    }

    #define OpScalarVec(op,sign)            TplVec\
                                                VecExp<VecOp<ScalarExp<const C*,C>,VecExp<const C*,C>,C,op>,C>\
                                                    operator sign (const C&v, const OpVec<C>&vec)\
                                                    {\
                                                        typedef VecOp<ScalarExp<const C*,C>,VecExp<const C*,C>,C,op>   ResultType;\
                                                        return VecExp<ResultType,C>(ResultType(ScalarExp<const C*,C>(&v),VecExp<const C*,C>(vec.v)));\
                                                    }

    #define OpVecScalar(op,sign)            TplVec\
                                                VecExp<VecOp<VecExp<const C*,C>,ScalarExp<const C*,C>,C,op>,C>\
                                                    operator sign (const OpVec<C>&vec, const C&v)\
                                                    {\
                                                        typedef VecOp<VecExp<const C*,C>,ScalarExp<const C*,C>,C,op>   ResultType;\
                                                        return VecExp<ResultType,C>(ResultType(VecExp<const C*,C>(vec.v),ScalarExp<const C*,C>(&v)));\
                                                    }

    //a,b e K^n  ; v e K ; E0,E1 e Expression^n
    OpBinary(OpAdd,+, VecOp);     //E0 + E1
    OpBinary(OpSub,-, VecOp);     //E0 - E1
    OpBinary(OpMult,&,VecOp);     //E0 & E1
    OpBinary(OpDiv,/,VecOp);      //E0 / E1
    OpBinary(OpAdd,|, VecCross);  //E0 x E1
    OpVecLeft(OpAdd,+, VecOp);    //a + E1
    OpVecRight(OpAdd,+, VecOp);   //E0 + b
    OpVecLeft(OpSub,-, VecOp);    //a - E1
    OpVecRight(OpSub,-, VecOp);   //E0 - b
    OpVecLeft(OpMult,&, VecOp);   //a & E1
    OpVecRight(OpMult,&, VecOp);  //E0 & b
    OpVecLeft(OpDiv,/, VecOp);    //a / E1
    OpVecRight(OpDiv,/, VecOp);   //E0 / b
    OpVecLeft(OpAdd,|,VecCross);  //a x E1
    OpVecRight(OpAdd,|,VecCross); //E0 x b
    OpScalarLeft(OpAdd,+);          //v + E1
    OpScalarRight(OpAdd,+);         //E0 + v
    OpScalarLeft(OpSub,-);          //v - E1
    OpScalarRight(OpSub,-);         //E0 - v
    OpScalarLeft(OpMult,*);         //v * E1
    OpScalarRight(OpMult,*);        //E0 * v
    OpScalarLeft(OpDiv,/);          //v / E1
    OpScalarRight(OpDiv,/);         //E0 / v
    OpVecBinary(OpAdd,+,VecOp);    //a + b
    OpVecBinary(OpSub,-,VecOp);    //a - b
    OpVecBinary(OpMult,&,VecOp);   //a & b //element-wise multiplication
    OpVecBinary(OpDiv,/,VecOp);    //a / b //element-wise division
    OpVecBinary(OpAdd,|,VecCross); //a x b
    OpVecScalar(OpAdd,+);           //a + v
    OpVecScalar(OpSub,-);           //a - v
    OpVecScalar(OpMult,*);          //a * v
    OpVecScalar(OpDiv,/);           //a / v
    OpScalarVec(OpAdd,+);           //v + a //v calculated like a vector with equal number of elements
    OpScalarVec(OpSub,-);           //v - a
    OpScalarVec(OpMult,*);          //v * a
    OpScalarVec(OpDiv,/);           //v / a

    //note: operations with scalars require usage of same types in both scalar and vector/expression.

    #undef TplVec
    #undef TplStrict
    #undef TplVecUnary
    #undef TplVecBinary
    #undef LeftVec
    #undef RightVec
    #undef OpScalarLeft
    #undef OpScalarRight
    #undef OpBinary
    #undef OpVecLeft
    #undef OpVecRight
    #undef OpVecBinary
    #undef OpVecScalar
    #undef OpScalarVec

    #define     MVECEXP template <class E, class C1> inline


    template <class C> class OpVec
    {
    public:
            union
            {
                C                   v[3];
                struct
                {
                    C               x,y,z;
                };
            };

                                    OpVec();
    MFUNC3                          OpVec(const C0&x_, const C1&y_, const C2&z_);
    MFUNC2                          OpVec(const C0&x_, const C1&y_);
                                    OpVec(const C&v_);
                                    OpVec(const OpVec<C>&other);
    MFUNC1                          OpVec(const OpVec<C0>&other);
    MVECEXP                         OpVec(const VecExp<E,C1>&);

    MFUNC1  void                    operator+=(const OpVec<C0>&);
            void                    operator+=(const C&);
    MVECEXP void                    operator+=(const VecExp<E,C1>&);
    MFUNC1  void                    operator-=(const OpVec<C0>&);
            void                    operator-=(const C&);
    MVECEXP void                    operator-=(const VecExp<E,C1>&);
    MFUNC1  void                    operator*=(const OpVec<C0>&);
            void                    operator*=(const C&);
    MFUNC1  void                    operator/=(const OpVec<C0>&);
            void                    operator/=(const C&);
    MFUNC1  void                    operator=(const OpVec<C0>&);
    MVECEXP void                    operator=(const VecExp<E,C1>&);
            void                    operator=(const C&);

    inline  C&                      operator[](unsigned component);

    inline  void                    normalize();
    inline  void                    normalize0();                       //normalize and double-check for zero-length
    inline  OpVec<C>               normalized()                const;  //returns normalized vector
    inline  OpVec<C>               normalized0()               const;
    inline  C                       length()                    const;
    inline  String                 toString()                  const;
    };




    #undef     MVEC
    #undef     MVECEXP

}

using namespace VectorMath;


#include "exp_vector.tpl.h"

#endif
