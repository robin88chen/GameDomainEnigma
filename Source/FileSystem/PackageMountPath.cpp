#include "PackageMountPath.h"
#include "AssetPackageFile.h"
#include "PackageContent.h"
#include "MemoryMacro.h"
#include <cassert>

#undef CreateFile

Enigma::PackageMountPath::PackageMountPath(AssetPackageFile* package, const std::string& path_id)
    : IMountPath(path_id)
{
    assert(package);
    m_assetPackage = package;
    m_packageFilename = m_assetPackage->GetBaseFilename();
}

Enigma::PackageMountPath::~PackageMountPath()
{
    m_assetPackage = nullptr;
}

Enigma::IFile* Enigma::PackageMountPath::CreateFile(const std::string& filename, const std::string& rw_option)
{
    if ((!m_assetPackage) || (filename.length() == 0)) return nullptr;
    bool readonly = true;
    if (rw_option.find("w") != std::string::npos) readonly = false;
    if (!readonly) return nullptr;  // must readonly
    IFile* file = menew PackageContent(m_assetPackage, filename);
    return file;
}

bool Enigma::PackageMountPath::EqualMountPath(IMountPath* path)
{
    assert(path);
    if (!EqualPathID(path->GetPathID())) return false;
    PackageMountPath* pkg_path = dynamic_cast<PackageMountPath*>(path);
    if (!pkg_path) return false;
    if (pkg_path->m_packageFilename != m_packageFilename) return false;
    return true;
}

bool Enigma::PackageMountPath::EqualMountPath(const std::filesystem::path& path)
{
    return EqualMountPath(path.string());
}

bool Enigma::PackageMountPath::EqualMouthPath(const std::string& path)
{
    return m_packageFilename == path;
}
