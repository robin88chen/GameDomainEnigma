#define _CRT_SECURE_NO_WARNINGS
#include "StdioFile.h"
#include "Platforms/PlatformLayerUtilities.h"
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

StdioFile::StdioFile(const std::string& fullpath, const ReadWriteOption& rw_option) : IFile()
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

std::optional<std::vector<unsigned char>> StdioFile::Read(size_t offset, size_t size_request)
{
    Debug::Printf("Read File in thread %d\n", std::this_thread::get_id());
    if ((!m_file) || (!m_file.is_open()))
    {
        MakeErrorCode(ErrorCode::fileStatusError);
        return std::nullopt;
    }

    m_file.seekg(0, std::fstream::end);
    size_t file_length = (size_t)m_file.tellg();
    if (offset > file_length)
    {
        MakeErrorCode(ErrorCode::readOffsetError);
        return std::nullopt;
    }
    if (offset + size_request > file_length) size_request = file_length - offset;

    m_file.seekg(offset);
    if (!m_file)
    {
        MakeErrorCode(ErrorCode::fileStatusError);
        return std::nullopt;
    }
    std::vector<unsigned char> out_buff;
    out_buff.resize(size_request, 0);
    m_file.read((char*)&out_buff[0], size_request);
    std::ios::iostate s = m_file.rdstate();
    if (!m_file)
    {
        MakeErrorCode(ErrorCode::readFail);
        return std::nullopt;
    }

    return out_buff;
}

size_t StdioFile::Write(size_t offset, const std::vector<unsigned char>& in_buff)
{
    Debug::Printf("Write File in thread %d\n", std::this_thread::get_id());
    if ((!m_file) || (!m_file.is_open()))
    {
        MakeErrorCode(ErrorCode::fileStatusError);
        return 0;
    }
    if (in_buff.empty())
    {
        MakeErrorCode(ErrorCode::emptyWriteBuffer);
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
        MakeErrorCode(ErrorCode::fileStatusError);
        return 0;
    }

    m_file.write((const char*)&in_buff[0], in_buff.size());
    if (!m_file)
    {
        MakeErrorCode(ErrorCode::writeFail);
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
        MakeErrorCode(ErrorCode::fileStatusError);
        return 0;
    }
    m_file.seekg(0, std::fstream::end);
    if (!m_file)
    {
        MakeErrorCode(ErrorCode::fileStatusError);
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
    return std::filesystem::is_regular_file(filepath);
}


error StdioFile::open()
{
    if (m_fullPath.length() == 0) return ErrorCode::emptyFilePath;
    if (!m_rwOption.any()) return ErrorCode::emptyRWOption;
    Debug::Printf("stdio file %s with option %s", m_fullPath.c_str(), m_rwOption.to_string().c_str());

    std::fstream::openmode mode{};
    if ((m_rwOption & ReadWriteOptionWrite).any())
    {
        mode |= std::fstream::out;
        m_isWritable = true;
    }
    if ((m_rwOption & ReadWriteOptionRead).any())
    {
        mode |= std::fstream::in;
    }
    if ((m_rwOption & ReadWriteOptionOpenAlways).any())
    {
        mode |= std::fstream::trunc;
    }
    if ((m_rwOption & ReadWriteOptionBinary).any())
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

error StdioFile::close()
{
    if (!m_file) return ErrorCode::fileStatusError;
    m_file.close();
    return ErrorCode::ok;
}

#undef _CRT_SECURE_NO_WARNINGS
