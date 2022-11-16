#include "FactoryDesc.h"

using namespace Enigma::Engine;

FactoryDesc& FactoryDesc::ClaimAsNative(const std::string& name)
{
    m_type = InstanceType::Native;
    if (!name.empty()) m_prefab_deferredFilename = name;
    return *this;
}

FactoryDesc& FactoryDesc::ClaimByPrefab(const std::string& prefab_name)
{
    m_type = InstanceType::ByPrefab;
    if (!prefab_name.empty()) m_prefab_deferredFilename = prefab_name;
    return *this;
}

FactoryDesc& FactoryDesc::ClaimAsDeferred(const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::Deferred;
    if (!filename.empty())
    {
        if (path_id.empty())
        {
            m_prefab_deferredFilename = filename;
        }
        else
        {
            m_prefab_deferredFilename = filename + "@" + path_id;
        }
    }
    return *this;
}

FactoryDesc& FactoryDesc::ClaimAsInstanced(const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::Instanced;
    if (!filename.empty())
    {
        if (path_id.empty())
        {
            m_prefab_deferredFilename = filename;
        }
        else
        {
            m_prefab_deferredFilename = filename + "@" + path_id;
        }
    }
    return *this;
}

FactoryDesc& FactoryDesc::ClaimAsResourceAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::ResourceAsset;
    if (!name.empty()) m_resourceName = name;
    if (!filename.empty())
    {
        if (path_id.empty())
        {
            m_resourceFilename = filename;
        }
        else
        {
            m_resourceFilename = filename + "@" + path_id;
        }
    }
    return *this;
}

FactoryDesc& FactoryDesc::ClaimFromResource(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::FromResource;
    if (!name.empty()) m_resourceName = name;
    if (!filename.empty())
    {
        if (path_id.empty())
        {
            m_resourceFilename = filename;
        }
        else
        {
            m_resourceFilename = filename + "@" + path_id;
        }
    }
    return *this;
}
