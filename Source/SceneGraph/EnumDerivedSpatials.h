/*********************************************************************
 * \file   EnumDerivedSpatials.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef ENUM_DERIVED_SPATIALS_H
#define ENUM_DERIVED_SPATIALS_H

#include <vector>
#include "SceneTraveler.h"
#include "Frameworks/Rtti.h"

namespace Enigma::SceneGraph
{
    class Spatial;

    class EnumDerivedSpatials : public SceneTraveler
    {
    public:
        EnumDerivedSpatials(const Frameworks::Rtti& baseRtti);
        EnumDerivedSpatials(const EnumDerivedSpatials&) = default;
        EnumDerivedSpatials(EnumDerivedSpatials&&) = default;
        ~EnumDerivedSpatials() override;
        EnumDerivedSpatials& operator=(const EnumDerivedSpatials&) = default;
        EnumDerivedSpatials& operator=(EnumDerivedSpatials&&) = default;

        virtual TravelResult travelTo(const SpatialPtr&) override;

        const std::vector<SpatialPtr>& GetSpatials() { return m_spatials; }

    protected:
        Frameworks::Rtti* m_baseRtti;
        std::vector<SpatialPtr> m_spatials;
    };
}


#endif // ENUM_DERIVED_SPATIALS_H
