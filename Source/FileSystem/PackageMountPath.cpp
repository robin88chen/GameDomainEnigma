#include "PackageMountPath.h"
#include "AssetPackageFile.h"
#include "PackageContent.h"
#include "MemoryMacro.h"
#include <cassert>

#undef CreateFile

using namespace Enigma::FileSystem;

PackageMountPath::PackageMountPath(const std::shared_ptr<AssetPackage::AssetPackageFile>& package, const std::string& path_id)
    : IMountPath(path_id)
{
    assert(package);
    m_assetPackage = package;
    m_packageFilename = m_assetPackage->GetBaseFilename();
}

PackageMountPath::~PackageMountPath()
{
    m_assetPackage = nullptr;
}

IFile* PackageMountPath::CreateFile(const std::string& filename, const std::string& rw_option)
{
    if ((!m_assetPackage) || (filename.length() == 0)) return nullptr;
    bool readonly = true;
    if (rw_option.find("w") != std::string::npos) readonly = false;
    if (!readonly) return nullptr;  // must readonly
    IFile* file = menew PackageContent(m_assetPackage, filename);
    return file;
}

bool PackageMountPath::EqualMountPath(IMountPath* path)
{
    assert(path);
    if (!EqualPathID(path->GetPathID())) return false;
    PackageMountPath* pkg_path = dynamic_cast<PackageMountPath*>(path);
    if (!pkg_path) return false;
    if (pkg_path->m_packageFilename != m_packageFilename) return false;
    return true;
}

bool PackageMountPath::EqualMountPath(const std::filesystem::path& path)
{
    return EqualMountPath(path.string());
}

bool PackageMountPath::EqualMouthPath(const std::string& path)
{
    return m_packageFilename == path;
}
