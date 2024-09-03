#include "FactoryDesc.h"

using namespace Enigma::Engine;

FactoryDesc& FactoryDesc::ClaimAsNative(const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::Native;
    m_prefab_deferredFilename = combineFilenameAtPath(filename, path_id);
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
    m_prefab_deferredFilename = combineFilenameAtPath(filename, path_id);
    return *this;
}

FactoryDesc& FactoryDesc::ClaimAsInstanced(const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::Instanced;
    m_prefab_deferredFilename = combineFilenameAtPath(filename, path_id);
    return *this;
}

FactoryDesc& FactoryDesc::ClaimAsResourceAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::ResourceAsset;
    if (!name.empty()) m_resourceName = name;
    m_resourceFilename = combineFilenameAtPath(filename, path_id);
    return *this;
}

FactoryDesc& FactoryDesc::ClaimFromResource(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_type = InstanceType::FromResource;
    if (!name.empty()) m_resourceName = name;
    m_resourceFilename = combineFilenameAtPath(filename, path_id);
    return *this;
}

FactoryDesc& FactoryDesc::PathId(const std::string& path_id)
{
    if (path_id.empty()) return *this;
    if (!m_resourceFilename.empty())
    {
        auto pos = m_resourceFilename.find_last_of('@');
        if (pos != std::string::npos)
        {
            m_resourceFilename = m_resourceFilename.substr(0, pos) + "@" + path_id;
        }
        else
        {
            m_resourceFilename = m_resourceFilename + "@" + path_id;
        }
    }
    if (!m_prefab_deferredFilename.empty())
    {
        auto pos = m_prefab_deferredFilename.find_last_of('@');
        if (pos != std::string::npos)
        {
            m_prefab_deferredFilename = m_prefab_deferredFilename.substr(0, pos) + "@" + path_id;
        }
        else
        {
            m_prefab_deferredFilename = m_prefab_deferredFilename + "@" + path_id;
        }
    }
    return *this;
}

std::string FactoryDesc::PathId() const
{
    if (!m_resourceFilename.empty())
    {
        auto pos = m_resourceFilename.find_last_of('@');
        if (pos != std::string::npos)
        {
            return m_resourceFilename.substr(pos + 1);
        }
        else
        {
            return "";
        }
    }
    if (!m_prefab_deferredFilename.empty())
    {
        auto pos = m_prefab_deferredFilename.find_last_of('@');
        if (pos != std::string::npos)
        {
            return m_prefab_deferredFilename.substr(pos + 1);
        }
        else
        {
            return "";
        }
    }
    return "";
}

std::string FactoryDesc::combineFilenameAtPath(const std::string& filename, const std::string& path_id) const
{
    if (filename.empty()) return filename;
    if (path_id.empty()) return filename;
    return filename + "@" + path_id;
}
