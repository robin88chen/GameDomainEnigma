/*********************************************************************
 * \file   IntrPrimitiveRay3.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef INTR_PRIMITIVE_RAY3_H
#define INTR_PRIMITIVE_RAY3_H

#include "MathLib/Intersector.h"
#include "MathLib/Vector3.h"
#include "MathLib/MathGlobal.h"
#include "MathLib/Ray3.h"
#include <memory>
#include <vector>

namespace Enigma::Engine
{
    class Primitive;
    class PrimitiveRay3IntersectionFinder;

    class IntrPrimitiveRay3 : public MathLib::Intersector
    {
    public:
        struct ResultRecord
        {
            float m_tParam;
            MathLib::Vector3 m_vecPoint;
            std::weak_ptr<Primitive> m_primitive;
            ResultRecord() : m_tParam(-MathLib::Math::MAX_FLOAT), m_vecPoint(MathLib::Vector3::ZERO) {};
            ResultRecord(float param, const MathLib::Vector3& vec, const std::shared_ptr<Primitive>& prim)
                : m_tParam(param), m_vecPoint(vec), m_primitive(prim) {};
            ~ResultRecord() {}
        };

    public:
        IntrPrimitiveRay3(const std::shared_ptr<Primitive>& primitive, const MathLib::Ray3& ray);
        virtual ~IntrPrimitiveRay3() override;

        // object access
        const MathLib::Ray3& GetRay() const;
        const std::shared_ptr<Primitive>& GetPrimitive();

        // static test-intersection query
        virtual Result test(std::unique_ptr<MathLib::IntersectorCache> last_result) override;
        virtual Result find(std::unique_ptr<MathLib::IntersectorCache> last_result) override;

        // the intersection set
        size_t GetQuantity() const;
        const MathLib::Vector3& GetPoint(unsigned int i) const;
        float GetRayT(unsigned int i) const;
        std::shared_ptr<Primitive> GetResultPrimitive(unsigned int i) const;

        void SetRequiredResultCount(unsigned count);

        const std::vector<ResultRecord>& GetResultRecord() const;

    private:
        // the objects to intersect
        const MathLib::Ray3& m_ray;
        std::shared_ptr<Primitive> m_primitive;

        PrimitiveRay3IntersectionFinder* m_intersectionFinder;

        // information about the intersection set
        std::vector<ResultRecord> m_resultRecords;
    };
}

#endif // INTR_PRIMITIVE_RAY3_H
