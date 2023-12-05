/*********************************************************************
 * \file   ModelPrimitiveRay3IntersectionFinder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef MODEL_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
#define MODEL_PRIMITIVE_RAY3_INTERSECTION_FINDER_H

#include "GameEngine/PrimitiveRay3IntersectionFinder.h"

namespace Enigma::Renderer
{
    class ModelPrimitive;

    class ModelPrimitiveRay3IntersectionFinder : public Engine::PrimitiveRay3IntersectionFinder
    {
    public:
        ModelPrimitiveRay3IntersectionFinder();
        virtual ~ModelPrimitiveRay3IntersectionFinder();

        static PrimitiveRay3IntersectionFinder* create();

        static void RegisterFactory();

        virtual MathLib::Intersector::Result Test(const std::shared_ptr<Engine::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;
        virtual std::tuple<std::vector<Engine::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            Find(const std::shared_ptr<Engine::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;

    private:
        MathLib::Intersector::Result TestModel(const std::shared_ptr<ModelPrimitive>& model, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
        std::tuple<std::vector<Engine::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            FindModel(const std::shared_ptr<ModelPrimitive>& model, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
    };
}


#endif // MODEL_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
