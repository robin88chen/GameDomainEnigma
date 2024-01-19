/*********************************************************************
 * \file   ModelPrimitiveRay3IntersectionFinder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef MODEL_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
#define MODEL_PRIMITIVE_RAY3_INTERSECTION_FINDER_H

#include "Primitives/PrimitiveRay3IntersectionFinder.h"

namespace Enigma::Renderables
{
    class ModelPrimitive;

    class ModelPrimitiveRay3IntersectionFinder : public Primitives::PrimitiveRay3IntersectionFinder
    {
    public:
        ModelPrimitiveRay3IntersectionFinder();
        virtual ~ModelPrimitiveRay3IntersectionFinder();

        static PrimitiveRay3IntersectionFinder* create();

        static void registerFactory();

        virtual MathLib::Intersector::Result test(const std::shared_ptr<Primitives::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;
        virtual std::tuple<std::vector<Primitives::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            find(const std::shared_ptr<Primitives::Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const override;

    private:
        MathLib::Intersector::Result testModel(const std::shared_ptr<ModelPrimitive>& model, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
        std::tuple<std::vector<Primitives::IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            findModel(const std::shared_ptr<ModelPrimitive>& model, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const;
    };
}


#endif // MODEL_PRIMITIVE_RAY3_INTERSECTION_FINDER_H
