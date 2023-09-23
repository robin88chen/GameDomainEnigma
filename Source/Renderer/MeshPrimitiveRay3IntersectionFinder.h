/*********************************************************************
 * \file   MeshPrimitiveRay3IntersectionFinder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef MESH_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
#define MESH_PRIMITIVE_RAY3_INTERSECTION_FINDER_H

#include "GameEngine/PrimitiveRay3IntersectionFinder.h"

namespace Enigma::Renderer
{
    class MeshPrimitive;

    class MeshPrimitiveRay3IntersectionFinder : public Engine::PrimitiveRay3IntersectionFinder
    {
    public:
        MeshPrimitiveRay3IntersectionFinder();
        virtual ~MeshPrimitiveRay3IntersectionFinder();

        static PrimitiveRay3IntersectionFinder* Create();

        static void RegisterFactory();

        virtual MathLib::Intersector::Result Test(const std::shared_ptr<Engine::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;
        virtual std::tuple<std::vector<Engine::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            Find(const std::shared_ptr<Engine::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;

    private:
        MathLib::Intersector::Result TestMesh(const std::shared_ptr<MeshPrimitive>& mesh, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
        std::tuple<std::vector<Engine::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            FindMesh(const std::shared_ptr<MeshPrimitive>& mesh, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
    };
}

#endif // MESH_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
