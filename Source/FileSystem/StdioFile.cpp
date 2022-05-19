#define _CRT_SECURE_NO_WARNINGS
#include "StdioFile.h"
#include "PlatformLayerUtilities.h"
#include "FileSystemErrors.h"
#include <cassert>
#include <iostream>
#include "sys/stat.h"
#include <filesystem>

using namespace Enigma::FileSystem;
using namespace Enigma::Platforms;

StdioFile::StdioFile() : IFile()
{
    m_fileTime = 0;
    m_isWritable = false;
}

StdioFile::StdioFile(const std::string& fullpath, const std::string& rw_option)
{
    m_fullPath = fullpath;
    m_rwOption = rw_option;
    m_fileTime = 0;
    m_isWritable = false;
}

StdioFile::~StdioFile()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

size_t StdioFile::Read(size_t offset, void* out_buff, size_t size)
{
    Debug::Printf("Read File in thread %d\n", std::this_thread::get_id());
    assert(out_buff);
    if ((!m_file) || (!m_file.is_open()))
    {
        make_error_code(ErrorCode::fileStatusError);
        return 0;
    }

    m_file.seekg(0, std::fstream::end);
    size_t file_length = (size_t)m_file.tellg();
    if (offset > file_length)
    {
        make_error_code(ErrorCode::readOffsetError);
        return 0;
    }
    size_t size_request = size;
    if (offset + size_request > file_length) size_request = file_length - offset;

    m_file.seekg(offset);
    if (!m_file)
    {
        make_error_code(ErrorCode::fileStatusError);
        return 0;
    }
    m_file.read((char*)out_buff, size_request);
    std::ios::iostate s = m_file.rdstate();
    if (!m_file)
    {
        make_error_code(ErrorCode::readFail);
        return 0;
    }
    size_t read_bytes = (size_t)m_file.tellg() - offset;

    return read_bytes;
}

size_t StdioFile::Write(size_t offset, void const* in_buff, size_t size)
{
    Debug::Printf("Write File in thread %d\n", std::this_thread::get_id());
    assert(in_buff);
    if ((!m_file) || (!m_file.is_open()))
    {
        make_error_code(ErrorCode::fileStatusError);
        return 0;
    }

    m_file.seekp(0, std::fstream::end);
    size_t file_length = (size_t)m_file.tellp();
    if (offset <= file_length)
    {
        m_file.seekp(offset);
    }
    if (!m_file)
    {
        make_error_code(ErrorCode::fileStatusError);
        return 0;
    }

    m_file.write((const char*)in_buff, size);
    if (!m_file)
    {
        make_error_code(ErrorCode::writeFail);
        return 0;
    }
    m_file.flush();
    size_t write_bytes = (size_t)m_file.tellp() - offset;

    return write_bytes;
}

size_t StdioFile::Size()
{
    if ((!m_file) || (!m_file.is_open()))
    {
        make_error_code(ErrorCode::fileStatusError);
        return 0;
    }
    m_file.seekg(0, std::fstream::end);
    if (!m_file)
    {
        make_error_code(ErrorCode::fileStatusError);
        return 0;
    }
    return (size_t)m_file.tellg();
}

time_t StdioFile::FileTime()
{
    if (m_fileTime != 0) return m_fileTime;

    struct tm* local_tm;
    struct stat attrib;
    int err = stat(m_fullPath.c_str(), &attrib);
    if (err != 0) return time_t{};
    time_t modify_time = (time_t)attrib.st_mtime;
    local_tm = localtime(&modify_time); // (attrib.st_mtime));
    m_fileTime = mktime(local_tm);
    return m_fileTime;
}

bool StdioFile::IsExisted()
{
    if (m_fullPath.length() <= 0) return false;
    if (m_isWritable) return true;
    struct stat attrib;
    int err = stat(m_fullPath.c_str(), &attrib);
    if (err != 0) return false;
    return true;
}

bool StdioFile::IsFileExisted(const std::string& filepath)
{
    if (filepath.length() <= 0) return false;
#if TARGET_PLATFORM == PLATFORM_WIN32
    return std::filesystem::is_regular_file(filepath);
#else
    struct stat attrib;
    int err = stat(filepath.c_str(), &attrib);
    if (err != 0) return false;
    return true;
#endif
}


error StdioFile::Open()
{
    if (m_fullPath.length() == 0) return ErrorCode::emptyFilePath;
    if (m_rwOption.length() == 0) return ErrorCode::emptyRWOption;
    Debug::Printf("stdio file %s with option %s", m_fullPath.c_str(), m_rwOption.c_str());

    std::fstream::openmode mode{};
    if (m_rwOption.find('w') != std::string::npos)
    {
        mode |= std::fstream::out;
        m_isWritable = true;
    }
    if (m_rwOption.find('r') != std::string::npos)
    {
        mode |= std::fstream::in;
    }
    if (m_rwOption.find('+') != std::string::npos)
    {
        mode |= std::fstream::trunc;
    }
    if (m_rwOption.find('b') != std::string::npos)
    {
        mode |= std::fstream::binary;
    }
    m_file.open(m_fullPath, mode);
    if (!m_file)
    {
        Debug::Printf("stdio file %s open error %s\n", m_fullPath.c_str(), strerror(errno));
        return ErrorCode::fileOpenError;
    }

    return ErrorCode::ok;
}

error StdioFile::Close()
{
    if (!m_file) return ErrorCode::fileStatusError;
    m_file.close();
    return ErrorCode::ok;
}

#undef _CRT_SECURE_NO_WARNINGS
