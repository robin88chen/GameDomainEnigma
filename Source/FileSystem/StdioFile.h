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

        virtual std::string getFullPath() override { return m_fullPath; };

        virtual std::optional<std::vector<unsigned char>> read(size_t offset, size_t size_request) override;
        virtual size_t  write(size_t offset, const std::vector<unsigned char>& in_buff) override;

        virtual size_t size() override;

        virtual time_t filetime() override;

        virtual bool isExisted() override;
        virtual bool isWritable() override { return m_isWritable; };

        static bool isFileExisted(const std::string& filepath);

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
