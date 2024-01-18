/*********************************************************************
 * \file   MeshPrimitiveRay3IntersectionFinder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef MESH_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
#define MESH_PRIMITIVE_RAY3_INTERSECTION_FINDER_H

#include "MathLib/Intersector.h"
#include "MathLib/Ray3.h"
#include "Primitives/PrimitiveRay3IntersectionFinder.h"
#include "Primitives/Primitive.h"

namespace Enigma::Renderer
{
    class MeshPrimitive;

    class MeshPrimitiveRay3IntersectionFinder : public Primitives::PrimitiveRay3IntersectionFinder
    {
    public:
        MeshPrimitiveRay3IntersectionFinder();
        virtual ~MeshPrimitiveRay3IntersectionFinder();

        static Primitives::PrimitiveRay3IntersectionFinder* create();

        static void RegisterFactory();

        virtual MathLib::Intersector::Result test(const std::shared_ptr<Primitives::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;
        virtual std::tuple<std::vector<Primitives::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            find(const std::shared_ptr<Primitives::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;

    private:
        MathLib::Intersector::Result TestMesh(const std::shared_ptr<MeshPrimitive>& mesh, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
        std::tuple<std::vector<Primitives::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            FindMesh(const std::shared_ptr<MeshPrimitive>& mesh, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
    };
}

#endif // MESH_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
