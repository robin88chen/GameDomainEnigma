#include "PackageMountPath.h"
#include "AssetPackage/AssetPackageFile.h"
#include "PackageContent.h"
#include "Platforms/MemoryMacro.h"
#include <cassert>

#undef CreateFile

using namespace Enigma::FileSystem;

PackageMountPath::PackageMountPath(const std::shared_ptr<AssetPackage::AssetPackageFile>& package, const std::string& path_id)
    : IMountPath(path_id)
{
    assert(package);
    m_assetPackage = package;
    m_packageFilename = m_assetPackage->getBaseFilename();
}

PackageMountPath::~PackageMountPath()
{
    m_assetPackage = nullptr;
}

IFile* PackageMountPath::createFile(const std::string& filename, const ReadWriteOption& rw_option)
{
    if ((!m_assetPackage) || (filename.length() == 0)) return nullptr;
    bool readonly = true;
    if ((rw_option & ReadWriteOptionWrite).any()) readonly = false;
    if (!readonly) return nullptr;  // must readonly
    IFile* file = menew PackageContent(m_assetPackage, filename);
    return file;
}

bool PackageMountPath::equalMountPath(IMountPath* path)
{
    assert(path);
    if (!equalPathId(path->getPathId())) return false;
    PackageMountPath* pkg_path = dynamic_cast<PackageMountPath*>(path);
    if (!pkg_path) return false;
    if (pkg_path->m_packageFilename != m_packageFilename) return false;
    return true;
}

bool PackageMountPath::equalMountPath(const std::filesystem::path& path)
{
    return m_packageFilename == path.string();
}

bool PackageMountPath::equalMountPath(const std::string& path)
{
    return m_packageFilename == path;
}
