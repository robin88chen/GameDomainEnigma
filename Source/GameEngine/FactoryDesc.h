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
        FactoryDesc(const std::string& rtti_name) : m_type(InstanceType::Native), m_rttiName(rtti_name) {};
        FactoryDesc(const FactoryDesc& desc) = default;
        FactoryDesc(FactoryDesc&& desc) = default;
        ~FactoryDesc() = default;
        FactoryDesc& operator=(const FactoryDesc& desc) = default;
        FactoryDesc& operator=(FactoryDesc&& desc) = default;

        FactoryDesc& ClaimAsNative(const std::string& name);
        FactoryDesc& ClaimByPrefab(const std::string& prefab_name);
        FactoryDesc& ClaimAsDeferred(const std::string& filename, const std::string& path_id = "");
        FactoryDesc& ClaimAsInstanced(const std::string& filename, const std::string& path_id = "");
        FactoryDesc& ClaimAsResourceAsset(const std::string& name, const std::string& filename, const std::string& path_id = "");
        FactoryDesc& ClaimFromResource(const std::string& name, const std::string& filename, const std::string& path_id = "");
        FactoryDesc& PathId(const std::string& path_id);
        /** Claim as native without change file name */
        FactoryDesc& ClaimAsNative() { return ClaimAsNative(""); }
        /** Claim as instanced without change file name */
        FactoryDesc& ClaimByPrefab() { return ClaimByPrefab(""); }

        /** Claim as deferred without change file name */
        FactoryDesc& ClaimAsDeferred() { return ClaimAsDeferred("", ""); }
        /** Claim as instanced without change file name */
        FactoryDesc& ClaimAsInstanced() { return ClaimAsInstanced("", ""); }

        /** Claim as resource asset without change resource name and file name */
        FactoryDesc& ClaimAsResourceAsset() { return ClaimAsResourceAsset("", ""); }
        /** Claim from resource without change resource name and file name */
        FactoryDesc& ClaimFromResource() { return ClaimFromResource("", ""); }

        InstanceType GetInstanceType() { return m_type; };
        InstanceType GetInstanceType() const { return m_type; };

        const std::string& GetRttiName() { return m_rttiName; };
        const std::string& GetRttiName() const { return m_rttiName; };
        const std::string& GetPrefab() { return m_prefab_deferredFilename; };
        const std::string& GetPrefab() const { return m_prefab_deferredFilename; };
        const std::string& GetResourceName() { return m_resourceName; };
        const std::string& GetResourceName() const { return m_resourceName; };
        const std::string& GetResourceFilename() { return m_resourceFilename; };
        const std::string& GetResourceFilename() const { return m_resourceFilename; };
        const std::string& GetDeferredFilename() { return m_prefab_deferredFilename; };
        const std::string& GetDeferredFilename() const { return m_prefab_deferredFilename; };

    protected:
        InstanceType m_type;
        std::string m_resourceName;
        std::string m_resourceFilename;
        std::string m_rttiName;
        std::string m_prefab_deferredFilename; ///< prefab or deferred filename
    };
}

#endif // FACTORY_DESC_H
