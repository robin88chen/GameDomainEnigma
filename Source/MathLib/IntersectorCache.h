/********************************************************************
 * \file   IntersectorCache.h
 * \brief  
    記錄本次Intersect資料，做為下次計算的起始點，可以加快計算
    主要是為了 Model, Mesh 的三角形
    提出的假設是，大部分的計算都會跟前次關聯，例如，都在附近的三角形，
    都在同一個Mesh內
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTERSECTOR_CACHE_H
#define _MATH_INTERSECTOR_CACHE_H

namespace Enigma::MathLib
{
    /** Math Lib Intersector Cache \n
      Intersector Cache Base Class, abstract class
    */
    class IntersectorCache
    {
    public:
        virtual ~IntersectorCache() {};
    };

};

#endif // !_MATH_INTERSECTOR_CACHE_H
