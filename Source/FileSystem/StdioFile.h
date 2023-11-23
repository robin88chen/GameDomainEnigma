/*********************************************************************
 * \file   StdioFile.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _STDIO_FILE_H
#define _STDIO_FILE_H

#define _CRT_SECURE_NO_WARNINGS

#include "IFile.h"
#include "ReadWriteOption.h"
#include <fstream>

namespace Enigma::FileSystem
{
    class StdioFile : public IFile
    {
    public:
        StdioFile();
        StdioFile(const StdioFile&) = delete;
        StdioFile(StdioFile&&) = delete;
        StdioFile(const std::string& fullpath, const ReadWriteOption& rw_option);
        virtual ~StdioFile() override;
        StdioFile& operator=(const StdioFile&) = delete;
        StdioFile& operator=(StdioFile&&) = delete;

        virtual std::string GetFullPath() override { return m_fullPath; };

        virtual std::optional<std::vector<unsigned char>> Read(size_t offset, size_t size_request) override;
        virtual size_t  Write(size_t offset, const std::vector<unsigned char>& in_buff) override;

        virtual size_t Size() override;

        virtual time_t FileTime() override;

        virtual bool IsExisted() override;
        virtual bool IsWritable() override { return m_isWritable; };

        static bool IsFileExisted(const std::string& filepath);

    protected:
        virtual error open() override;
        virtual error close() override;

    private:
        std::fstream m_file;
        std::string m_fullPath;
        ReadWriteOption m_rwOption;
        time_t m_fileTime;
        bool m_isWritable;
    };
}
#undef _CRT_SECURE_NO_WARNINGS

#endif // !_STDIO_FILE_H
