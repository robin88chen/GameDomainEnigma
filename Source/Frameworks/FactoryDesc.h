/*********************************************************************
 * \file   FactoryDesc.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef FACTORY_DESC_H
#define FACTORY_DESC_H

#include <string>

namespace Enigma::Frameworks
{
    class FactoryDesc
    {
    public:
        FactoryDesc(const std::string& rtti_name) : m_rttiName(rtti_name) {};
        FactoryDesc(const FactoryDesc& desc) = default;
        FactoryDesc(FactoryDesc&& desc) = default;
        ~FactoryDesc() = default;

        FactoryDesc& operator=(const FactoryDesc& desc) = default;
        FactoryDesc& operator=(FactoryDesc&& desc) = default;

        const std::string& GetRttiName() { return m_rttiName; };
        const std::string& GetRttiName() const { return m_rttiName; };

    protected:
        std::string m_rttiName;
    };
}

#endif // FACTORY_DESC_H
