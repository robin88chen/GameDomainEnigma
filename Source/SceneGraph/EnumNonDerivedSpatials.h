/*********************************************************************
 * \file   EnumNonDerivedSpatials.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef ENUM_NON_DERIVED_SPATIALS_H
#define ENUM_NON_DERIVED_SPATIALS_H

#include <vector>
#include "SceneTraveler.h"
#include "Frameworks/Rtti.h"

namespace Enigma::SceneGraph
{
    class Spatial;

    class EnumNonDerivedSpatials : public SceneTraveler
    {
    public:
        EnumNonDerivedSpatials(const Frameworks::Rtti& baseRtti);
        EnumNonDerivedSpatials(const EnumNonDerivedSpatials&) = default;
        EnumNonDerivedSpatials(EnumNonDerivedSpatials&&) = default;
        ~EnumNonDerivedSpatials() override;
        EnumNonDerivedSpatials& operator=(const EnumNonDerivedSpatials&) = default;
        EnumNonDerivedSpatials& operator=(EnumNonDerivedSpatials&&) = default;

        virtual TravelResult travelTo(const SpatialPtr&) override;

        const std::vector<SpatialPtr>& GetSpatials() { return m_spatials; }

    protected:
        Frameworks::Rtti* m_baseRtti;
        std::vector<SpatialPtr> m_spatials;
    };
}


#endif // ENUM_NON_DERIVED_SPATIALS_H
