#define _CRT_SECURE_NO_WARNINGS
#include "AssetPackageFile.h"
#include "AssetNameList.h"
#include "AssetHeaderDataMap.h"
#include "AssetErrors.h"
#include "zlib.h"
#include <cassert>
#include <ctime>
#include <vector>
#include "sys/stat.h"

using namespace Enigma::AssetPackage;

constexpr unsigned int PACKAGE_FORMAT_TAG = 0x01;
constexpr char PACKAGE_HEADER_FILE_EXT[] = ".eph";
constexpr char PACKAGE_BUNDLE_FILE_EXT[] = ".epb";

using AssetHeaderData = AssetHeaderDataMap::AssetHeaderData;

unsigned int GetFileVersionWithModifyTime(const std::string& file_path)
{
    if (file_path.empty()) return 0;
    struct tm* clock;
    struct stat attrib;
    int err = stat(file_path.c_str(), &attrib);
    if (err != 0) return 0;
    time_t modify_time = (time_t)attrib.st_mtime;
    clock = localtime(&modify_time); // (attrib.st_mtime));
    unsigned int min_step = clock->tm_min / 4;
    // min_step 4 bits, hour 5 bits, mday 5 bits, month 4 bits, year since 2010
    unsigned int ver = (unsigned int)min_step + (((unsigned int)clock->tm_hour) << 4)
        + (((unsigned int)clock->tm_mday) << 9) + (((unsigned int)clock->tm_mon) << 14)
        + (((unsigned int)clock->tm_year - 110) << 18);
    return ver;
}

time_t GetTimeStampFromFileVersion(unsigned int ver)
{
    struct tm clock;
    memset(&clock, 0, sizeof(struct tm));
    clock.tm_min = (ver & 0xf) * 4;
    ver = ver >> 4;
    clock.tm_hour = ver & 0x1f;
    ver = ver >> 5;
    clock.tm_mday = ver & 0x1f;
    ver = ver >> 5;
    clock.tm_mon = ver & 0xf;
    ver = ver >> 4;
    clock.tm_year = ver + 110;
    return mktime(&clock);
}

AssetPackageFile::AssetPackageFile()
{
    m_formatTag = PACKAGE_FORMAT_TAG;
    m_fileVersion = 0;
    m_assetCount = 0;
    m_nameList = nullptr;
    m_headerDataMap = nullptr;
}

AssetPackageFile::~AssetPackageFile()
{
    if (m_nameList)
    {
        m_nameList = nullptr;
    }
    if (m_headerDataMap)
    {
        m_headerDataMap = nullptr;
    }
    if (m_headerFile.is_open())
    {
        m_headerFile.close();
    }
    if (m_bundleFile.is_open())
    {
        m_bundleFile.close();
    }
}

AssetPackageFile* AssetPackageFile::createNewPackage(const std::string& basefilename)
{
    AssetPackageFile* package = new AssetPackageFile();
    const error er = package->createNewPackageImp(basefilename);
    assert(!er);
    return package;
}

error AssetPackageFile::createNewPackageImp(const std::string& basefilename)
{
    if (basefilename.empty()) return ErrorCode::emptyFileName;

    resetPackage();

    m_nameList = std::make_unique<AssetNameList>();
    m_headerDataMap = std::make_unique<AssetHeaderDataMap>();
    m_baseFilename = basefilename;

    std::string header_filename = m_baseFilename + PACKAGE_HEADER_FILE_EXT;
    std::string bundle_filename = m_baseFilename + PACKAGE_BUNDLE_FILE_EXT;

    m_headerFile.open(header_filename.c_str(), std::fstream::in | std::fstream::out
        | std::fstream::binary | std::fstream::trunc);
    m_bundleFile.open(bundle_filename.c_str(), std::fstream::in | std::fstream::out
        | std::fstream::binary | std::fstream::trunc);
    if ((!m_headerFile) || (!m_bundleFile))
    {
        return ErrorCode::fileOpenFail;
    }

    saveHeaderFile();

    return ErrorCode::ok;
}

AssetPackageFile* AssetPackageFile::openPackage(const std::string& basefilename)
{
    AssetPackageFile* package = new AssetPackageFile();
    const error er = package->openPackageImp(basefilename);
    assert(!er);
    return package;
}

error AssetPackageFile::openPackageImp(const std::string& basefilename)
{
    if (basefilename.empty()) return ErrorCode::emptyFileName;

    resetPackage();

    m_baseFilename = basefilename;

    std::string header_filename = m_baseFilename + PACKAGE_HEADER_FILE_EXT;
    std::string bundle_filename = m_baseFilename + PACKAGE_BUNDLE_FILE_EXT;

    m_headerFile.open(header_filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
    m_bundleFile.open(bundle_filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
    if ((!m_headerFile) || (!m_bundleFile))
    {
        return ErrorCode::fileOpenFail;
    }

    readHeaderFile();

    return ErrorCode::ok;
}

error AssetPackageFile::addAssetFile(const std::string& file_path, const std::string& asset_key, unsigned int version)
{
    assert(m_headerFile);
    assert(m_bundleFile);
    if ((file_path.empty()) || (asset_key.empty()))
    {
        return ErrorCode::emptyFileName;
    }
    unsigned int asset_ver = version;
    if (version == VERSION_USE_FILE_TIME)
    {
        asset_ver = GetFileVersionWithModifyTime(file_path);
    }
    std::ifstream asset_file{ file_path, std::fstream::in | std::fstream::binary };
    if (asset_file.fail()) return ErrorCode::fileOpenFail;
    asset_file.seekg(0, std::fstream::end);
    unsigned int file_length = (unsigned int)asset_file.tellg();
    asset_file.seekg(0);
    std::vector<char> buff;
    buff.resize(file_length, 0);
    asset_file.read(&buff[0], file_length);
    if (!asset_file)
    {
        asset_file.close();
        return ErrorCode::fileReadFail;
    }
    error add_result = addAssetMemory(buff, asset_key, asset_ver);

    asset_file.close();
    return add_result;
}

error AssetPackageFile::addAssetMemory(const std::vector<char>& buff, const std::string& asset_key, unsigned int version)
{
    assert(m_headerFile);
    assert(m_bundleFile);
    if (buff.empty())
    {
        return ErrorCode::emptyBuffer;
    }
    if (asset_key.empty())
    {
        return ErrorCode::emptyKey;
    }
    unsigned long comp_length = compressBound((uLong)buff.size());
    std::vector<unsigned char> comp_buff;
    comp_buff.resize(comp_length, 0);
    int comp_result = compress((unsigned char*)&comp_buff[0], &comp_length, (const unsigned char*)&buff[0], (uLong)buff.size());
    if (comp_result != Z_OK)
    {
        return ErrorCode::compressFail;
    }

    std::lock_guard<std::mutex> locker{ m_bundleFileLocker };

    m_bundleFile.seekp(0, std::fstream::end);
    unsigned int bundle_offset = (unsigned int)m_bundleFile.tellp();
    AssetHeaderData header_data;
    header_data.m_name = asset_key;
    header_data.m_offset = bundle_offset;
    header_data.m_orgSize = (unsigned int)buff.size();
    header_data.m_size = comp_length;
    header_data.m_version = version;
    header_data.m_crc = 0;

    error er = m_nameList->appendAssetName(asset_key);
    if (er) return er;
    er = m_headerDataMap->insertHeaderData(header_data);
    if (er)
    {
        // header add 失敗, 要再把 name list 改回
        m_nameList->removeAssetName(asset_key);
        return er;
    }

    m_bundleFile.write((const char*)&comp_buff[0], comp_length);
    m_bundleFile.flush();

    m_assetCount++;

    saveHeaderFile();

    return ErrorCode::ok;
}

error AssetPackageFile::tryRetrieveAssetToFile(const std::string& file_path, const std::string& asset_key)
{
    if (file_path.empty())
    {
        return ErrorCode::emptyFileName;
    }
    if (asset_key.empty())
    {
        return ErrorCode::emptyKey;
    }
    unsigned int asset_orig_size = getAssetOriginalSize(asset_key);
    if (asset_orig_size == 0) return ErrorCode::zeroSizeAsset;
    auto buff = tryRetrieveAssetToMemory(asset_key);
    if (!buff)
    {
        return ErrorCode::emptyBuffer;
    }

    std::ofstream output_file{ file_path, std::fstream::out | std::fstream::binary | std::fstream::trunc };
    if (!output_file) return ErrorCode::fileOpenFail;

    output_file.write(&((*buff)[0]), asset_orig_size);
    if (!output_file) return ErrorCode::fileWriteFail;
    auto write_bytes = output_file.tellp();

    output_file.close();

    if (write_bytes != asset_orig_size) return ErrorCode::writeSizeCheck;

    return ErrorCode::ok;
}

std::optional<std::vector<char>> AssetPackageFile::tryRetrieveAssetToMemory(const std::string& asset_key)
{
    assert(m_bundleFile);

    if (asset_key.empty()) return std::nullopt;
    unsigned int asset_orig_size = getAssetOriginalSize(asset_key);
    if (asset_orig_size == 0) return std::nullopt;

    auto header_data = tryGetAssetHeaderData(asset_key);
    if (!header_data) return std::nullopt;

    auto [comp_buff, read_bytes] = readBundleContent(header_data->m_offset, header_data->m_size);

    if (read_bytes != header_data->m_size) return std::nullopt;

    unsigned long buff_out_length = (unsigned long)asset_orig_size;
    std::vector<char> buff;
    buff.resize(buff_out_length, 0);
    int z_result = uncompress((unsigned char*)&buff[0], &buff_out_length,
        (const unsigned char*)&comp_buff[0], header_data->m_size);

    if (z_result != Z_OK) return std::nullopt;

    return buff;
}

unsigned int AssetPackageFile::getAssetOriginalSize(const std::string& asset_key)
{
    assert(m_headerDataMap);

    if (asset_key.empty()) return 0;
    auto header_data = tryGetAssetHeaderData(asset_key);
    if (!header_data) return 0;
    return header_data->m_orgSize;
}

time_t AssetPackageFile::getAssetTimeStamp(const std::string& asset_key)
{
    assert(m_headerDataMap);

    if (asset_key.empty()) return 0;

    auto header_data = tryGetAssetHeaderData(asset_key);
    if (!header_data) return 0;

    unsigned int ver = header_data->m_version;
    return GetTimeStampFromFileVersion(ver);
}

error AssetPackageFile::removeAsset(const std::string& asset_key)
{
    if (asset_key.empty()) return ErrorCode::emptyKey;
    if (!m_headerDataMap) return ErrorCode::invalidHeaderData;
    if (!m_nameList) return ErrorCode::invalidNameList;
    auto header_data = tryGetAssetHeaderData(asset_key);
    if (!header_data) return ErrorCode::invalidHeaderData;
    unsigned int content_size = header_data->m_size;
    unsigned int content_offset = header_data->m_offset;

    error er = repackBundleContent(content_size, content_offset);
    if (er) return er;

    // 前面都檢查過可以移除，所以這後面的 error 都做 assert
    er = m_nameList->removeAssetName(asset_key);
    assert(!er);
    m_headerDataMap->repackContentOffsets(content_size, content_offset);
    er = m_headerDataMap->removeHeaderData(asset_key);
    assert(!er);
    saveHeaderFile();

    return ErrorCode::ok;
}

std::optional<AssetHeaderDataMap::AssetHeaderData> AssetPackageFile::tryGetAssetHeaderData(
    const std::string& asset_key) const
{
    assert(m_headerDataMap);
    return m_headerDataMap->tryGetHeaderData(asset_key);
}

void AssetPackageFile::resetPackage()
{
    if (m_headerFile)
    {
        m_headerFile.close();
    }
    if (m_bundleFile)
    {
        m_bundleFile.close();
    }
    m_formatTag = PACKAGE_FORMAT_TAG;
    m_fileVersion = 0;
    m_assetCount = 0;
    m_nameList = nullptr;
    m_headerDataMap = nullptr;
}

void AssetPackageFile::saveHeaderFile()
{
    assert(m_headerFile);
    std::lock_guard<std::mutex> locker{ m_headerFileLocker };
    m_headerFile.seekp(0);

    //m_headerFile << m_formatTag << m_fileVersion << m_assetCount;
    m_headerFile.write((const char*)&m_formatTag, sizeof(m_formatTag));
    m_headerFile.write((const char*)&m_fileVersion, sizeof(m_fileVersion));
    m_headerFile.write((const char*)&m_assetCount, sizeof(m_assetCount));

    assert(m_nameList);
    std::vector<char> name_buff = m_nameList->exportToByteBuffer();
    unsigned int name_list_byte_size = (unsigned int)name_buff.size();
    m_headerFile.write((const char*)&name_list_byte_size, sizeof(name_list_byte_size));
    if (name_list_byte_size > 0)
    {
        m_headerFile.write(&name_buff[0], name_list_byte_size);
    }

    assert(m_headerDataMap);
    std::vector<char> header_buff = m_headerDataMap->exportToByteBuffer();
    unsigned int header_byte_size = (unsigned int)header_buff.size();
    m_headerFile.write((const char*)&header_byte_size, sizeof(header_byte_size));
    if (header_byte_size > 0)
    {
        m_headerFile.write(&header_buff[0], header_byte_size);
    }

    m_headerFile.flush();
}

void AssetPackageFile::readHeaderFile()
{
    assert(m_headerFile);
    std::lock_guard<std::mutex> locker{ m_headerFileLocker };
    m_headerFile.seekg(0);

    //m_headerFile >> m_formatTag >> m_fileVersion >> m_assetCount;
    m_headerFile.read((char*)&m_formatTag, sizeof(m_formatTag));
    m_headerFile.read((char*)&m_fileVersion, sizeof(m_fileVersion));
    m_headerFile.read((char*)&m_assetCount, sizeof(m_assetCount));

    unsigned int name_list_byte_size;
    m_headerFile.read((char*)&name_list_byte_size, sizeof(name_list_byte_size));
    std::vector<char> name_buff;
    if (name_list_byte_size > 0)
    {
        name_buff.resize(name_list_byte_size, 0);
        m_headerFile.read(&name_buff[0], name_list_byte_size);
    }
    m_nameList = std::make_unique<AssetNameList>();
    m_nameList->importFromByteBuffer(name_buff);

    unsigned int header_byte_size;
    m_headerFile.read((char*)&header_byte_size, sizeof(header_byte_size));
    std::vector<char> header_buff;
    if (header_byte_size > 0)
    {
        header_buff.resize(header_byte_size, 0);
        m_headerFile.read(&header_buff[0], header_byte_size);
    }
    m_headerDataMap = std::make_unique<AssetHeaderDataMap>();
    m_headerDataMap->importFromByteBuffer(header_buff);
}

std::tuple<std::vector<char>, unsigned int> AssetPackageFile::readBundleContent(unsigned int offset,
    unsigned int content_size)
{
    std::lock_guard<std::mutex> locker{ m_bundleFileLocker };
    m_bundleFile.seekg(offset);
    std::vector<char> out_buff;
    out_buff.resize(content_size, 0);
    m_bundleFile.read(&out_buff[0], content_size);
    if (!m_bundleFile) return { out_buff, 0 };
    return { out_buff, (unsigned int)m_bundleFile.tellg() - offset };
}

error AssetPackageFile::repackBundleContent(const unsigned int content_size, const unsigned int base_offset)
{
    assert(m_bundleFile);

    std::lock_guard<std::mutex> locker{ m_bundleFileLocker };

    m_bundleFile.seekp(0, std::fstream::end);
    auto bundle_org_size = m_bundleFile.tellp();
    if (bundle_org_size == 0) return ErrorCode::fileSizeError;
    std::vector<char> file_buff;
    file_buff.resize(bundle_org_size, 0);
    m_bundleFile.seekg(0);
    m_bundleFile.read(&file_buff[0], bundle_org_size);
    auto read_bytes = m_bundleFile.tellg();
    if (read_bytes != bundle_org_size) return ErrorCode::readSizeCheck;

    file_buff.erase(file_buff.begin() + base_offset, file_buff.begin() + base_offset + content_size);
    m_bundleFile.seekp(0);
    m_bundleFile.write(&file_buff[0], file_buff.size());
    m_bundleFile.flush();
    return ErrorCode::ok;
}

#undef _CRT_SECURE_NO_WARNINGS
