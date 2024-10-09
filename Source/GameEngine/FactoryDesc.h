/*********************************************************************
 * \file   FactoryDesc.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef FACTORY_DESC_H
#define FACTORY_DESC_H

#include "Frameworks/Rtti.h"
#include <string>

namespace Enigma::Engine
{
    class FactoryDesc
    {
    public:
        enum class InstanceType
        {
            Native,  ///< 原生物件
            ByPrefab, ///< 從 Prefab clone
            Deferred, ///< Deferred IO
            Instanced, ///< Deferred 物件進行內容實體化
            FromResource, ///< 從Resource讀Raw Data
            ResourceAsset, ///< Raw Data Resource
        };
    public:
        FactoryDesc(const std::string& rtti_name) : m_type(InstanceType::Native), m_rttiName(rtti_name) {}
        FactoryDesc(const Frameworks::Rtti& rtti) : m_type(InstanceType::Native), m_rttiName(rtti.getName()) {}
        FactoryDesc(const FactoryDesc& desc) = default;
        FactoryDesc(FactoryDesc&& desc) = default;
        ~FactoryDesc() = default;
        FactoryDesc& operator=(const FactoryDesc& desc) = default;
        FactoryDesc& operator=(FactoryDesc&& desc) = default;

        FactoryDesc& claimAsNative(const std::string& filename, const std::string& path_id = "");
        FactoryDesc& claimByPrefab(const std::string& prefab_name);
        FactoryDesc& claimAsDeferred(const std::string& filename, const std::string& path_id = "");
        FactoryDesc& claimAsInstanced(const std::string& filename, const std::string& path_id = "");
        FactoryDesc& claimAsResourceAsset(const std::string& name, const std::string& filename, const std::string& path_id = "");
        FactoryDesc& claimFromResource(const std::string& name, const std::string& filename, const std::string& path_id = "");
        FactoryDesc& pathId(const std::string& path_id);
        /** Claim as native without change file name */
        FactoryDesc& claimAsNative() { return claimAsNative(""); }
        /** Claim as instanced without change file name */
        FactoryDesc& claimByPrefab() { return claimByPrefab(""); }

        /** Claim as deferred without change file name */
        FactoryDesc& claimAsDeferred() { return claimAsDeferred("", ""); }
        /** Claim as instanced without change file name */
        FactoryDesc& claimAsInstanced() { return claimAsInstanced("", ""); }

        /** Claim as resource asset without change resource name and file name */
        FactoryDesc& claimAsResourceAsset() { return claimAsResourceAsset("", ""); }
        /** Claim from resource without change resource name and file name */
        FactoryDesc& claimFromResource() { return claimFromResource("", ""); }

        InstanceType instanceType() { return m_type; };
        InstanceType instanceType() const { return m_type; };

        const std::string& rttiName() { return m_rttiName; };
        const std::string& rttiName() const { return m_rttiName; };
        const std::string& prefabFilename() { return m_prefab_deferredFilename; };
        const std::string& prefabFilename() const { return m_prefab_deferredFilename; };
        const std::string& resourceName() { return m_resourceName; };
        const std::string& resourceName() const { return m_resourceName; };
        const std::string& resourceFilename() { return m_resourceFilename; };
        const std::string& resourceFilename() const { return m_resourceFilename; };
        const std::string& deferredFilename() { return m_prefab_deferredFilename; };
        const std::string& deferredFilename() const { return m_prefab_deferredFilename; };
        std::string pathId() const;

    protected:
        std::string combineFilenameAtPath(const std::string& filename, const std::string& path_id) const;

    protected:
        InstanceType m_type;
        std::string m_resourceName;
        std::string m_resourceFilename;
        std::string m_rttiName;
        std::string m_prefab_deferredFilename; ///< prefab or deferred filename
    };
}

#endif // FACTORY_DESC_H
