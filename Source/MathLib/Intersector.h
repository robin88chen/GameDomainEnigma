/********************************************************************
 * \file   Intersector.h
 * \brief  
    Intersector Base Class
    在這個架構中，我們將幾何圖形的資料與交點計算分開。這個基礎類別
    是所有交點計算的基底。
    將計算與資料分開，幾何圖形資料不會互相牽扯，例如，不會因為要計
    算平面與直線的交點，而讓平面與直線耦合度增加，可以維持獨立性
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTERSECTOR_H
#define _MATH_INTERSECTOR_H

namespace Enigma::MathLib
{
    class IntersectorCache;
    /** Math Lib Intersector \n
      Intersector Base Class, abstract class
    */
    class Intersector
    {
    public:
        struct Result
        {
            Result() : m_hasIntersect(false), m_cache(nullptr) {}
            Result(bool has_intr, IntersectorCache* cache) : m_hasIntersect(has_intr), m_cache(cache) {}

            bool m_hasIntersect;
            IntersectorCache* m_cache;
        };
    public:
        virtual ~Intersector();

        /** Static intersection queries.  The default implementations return 'false'. */
        virtual Result Test(IntersectorCache* last_result);

        /** Static intersection queries.  The default implementations call Test() (return 'false').
         Produces a set of intersection.  The derived class is responsible for providing access to that set, since the nature
         of the set is dependent on the object types. */
        virtual Result Find(IntersectorCache* last_result);

        /** Intersection Type -- information about the intersection set */
        enum class IntersectionType
        {
            EMPTY = 0,
            POINT,
            SEGMENT,
            RAY,
            LINE,
            POLYGON,
            PLANE,
            POLYHEDRON,
            OTHER
        };

        IntersectionType GetIntersectionType() const;

    protected:
        Intersector();
        IntersectionType m_intersectionType;
    };

};

#endif // !_MATH_INTERSECTOR_H
